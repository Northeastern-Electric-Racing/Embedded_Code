#include <Arduino.h>
#include <FlexCAN_T4.h>

#define ACCEL_PIN1 A0
#define ACCEL_PIN2 A1
#define BRAKE_PIN1 2
#define BRAKE_PIN2 3

// delays between sending CAN messages (in milliseconds)
#define COMMAND_MESSAGE_DELAY 50
#define BRAKE_MESSAGE_DELAY   50
#define BMS_MESSAGE_DELAY     50

// the maximum consecutive invalid brake switch readings before throwing an error
#define MAX_BRAKE_ERRORS 5

// motor torque constants
#define MAXIMUM_TORQUE 255 // in Nm 
#define POT_LOWER_BOUND 35 // a pot value from 0 to 1023
#define POT_UPPER_BOUND 1000 // a pot value from 0 to 1023

// regen braking constants
#define START_TIME 0   // delay from when brake is pressed to when regen starts
#define MAX_REGEN_TORQUE -255 // maximum regen torque value
#define RAMP_TIME  10  // time until the maximum regen torque is reached (in seconds)
 
// CAN IDs  
#define CAN_START_MOTOR  0x101
#define CAN_DIRECTION    0x102
#define CAN_BRAKE        0x103
#define CAN_BRAKE_ERROR  0x104
#define CAN_ACCEL_ERROR  0x105
#define CAN_BMS_SHUTDOWN 0x003
#define CAN_MC_FAULTS    0xAB
#define CAN_BMS_STATES   0x6B0
#define CAN_BMS_FAULTS   0x06
#define CAN_MOTOR        0xC0

#define BAUD_RATE 250000U // 250 kbps 
#define MAX_MB_NUM 16 // maximum number of CAN mailboxes to use 

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; // main CAN object

// brake switch and potentiometer values
int accelPin1Val; 
int accelPin2Val;
int brakePin1Val;
int brakePin2Val;

// state variables
bool invertorOn = false;
bool isForward = true;
bool brakePressed = false;
bool dischargeEnabled = false; // true when torque can be given to motor
bool chargeEnabled = false;    // true when regen torque is allowed
uint32_t timeBrake = 0;   // the time at which the brake was last pressed

uint32_t lastCommandMessage = 0; // the time the last command message was sent
uint32_t lastBrakeMessage = 0; // the time the last brake message was sent
uint32_t lastBMSMessage = 0; // the time the last BMS message was sent

int currentBrakeErrors = 0; // current number of consectutive brake errors


// CAN info
const uint8_t MOTOR_OFF[8] = {0,0,0,0,0,0,0,0}; // message to turn motor off
const uint8_t ACCEL_ERROR[8] = {'P', 0, 5, 00, 0, 0, 0, 0}; // error message of "Vehicle Speed Sensor Malfunction" 
const uint8_t BRAKE_ERROR[8] = {'P', 0, 5, 04, 0, 0, 0, 0}; // error message of "Brake Switch A / B Correlation"
const uint8_t BMS_ERROR[8] = {1,1,1,1,1,1,1,1}; // error message for BMS to activate shutdown circuit


// function declarations
void readPotentiometers(); 
void readSwitches();
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf); 
void incomingCANCallback(const CAN_message_t &msg);



/**
 * @brief Init serial console, CAN bus, and brake switch digital pins
 * 
 */
void setup() {
  Serial.begin(115200); 
  delay(400);

  myCan.begin();
  myCan.setBaudRate(BAUD_RATE);
  myCan.setMaxMB(MAX_MB_NUM);
  myCan.enableFIFO(); 
  myCan.enableFIFOInterrupt(); 
  myCan.onReceive(incomingCANCallback);

  pinMode(BRAKE_PIN1, INPUT_PULLUP);
  pinMode(BRAKE_PIN2, INPUT_PULLUP);
}

/**
 * @brief Continuoulsy read incoming CAN messages and the values of the 
 *        accelerator potentiometers and brake switches
 * 
 */
void loop() {
  myCan.events();
  
  if ((millis() - lastCommandMessage) > COMMAND_MESSAGE_DELAY) { // Adds delay between pedal potentiometer readings
    if (invertorOn) { // only read potentiometers when the inverter is on
      readPotentiometers();
      lastCommandMessage = millis(); // reset command message time
    }
  }

  if ((millis() - lastBrakeMessage) > BRAKE_MESSAGE_DELAY) { // Adds delay between brake switch readings
    readSwitches();
    lastBrakeMessage = millis(); // reset brake message time
  }

  if((millis() - lastBMSMessage) > BMS_MESSAGE_DELAY) {  // Checks if BMS message is being received regularly 
    sendMessage(CAN_BMS_SHUTDOWN, 8, BMS_ERROR); // triggers shutdown circuit through BMS
  }
}


/**
 * @brief Read the values of the two accelerator pedal potentiometers. Compare them to 
 *        make sure the values agree and send value to CAN command message.
 * 
 * @details The acceleration torque is taken directly from the two potentiometers. The regen torque
 *          is calculated as a function of the amount of time the brake has been pressed, along with
 *          the car speed (TBD on car speed). If there is any acceleration torque, regen is not factored
 *          in. Only when acceleration torque is 0 will regen torque be sent to the MC.
 * 
 */
void readPotentiometers() {
  accelPin1Val = analogRead(ACCEL_PIN1); 
  accelPin2Val = analogRead(ACCEL_PIN2);

  int16_t appliedTorque = 0;

  // Reports errors in potentiometer values if they "bottom" or "top" out or if they are not in sync
  if (accelPin1Val == 0 || accelPin2Val == 0 || 
      accelPin1Val == 1023 || accelPin2Val == 1023 ||
      (abs(accelPin1Val - accelPin2Val) > (1023 * 0.05))) {
    sendMessage(CAN_BMS_SHUTDOWN, 8, BMS_ERROR);
    sendMessage(CAN_MOTOR, 8, MOTOR_OFF);
    sendMessage(CAN_ACCEL_ERROR, 4, ACCEL_ERROR);
  }

  // find accel torque when BMS says discharge is enabled
  if (dischargeEnabled) {
    int16_t avgVal = (accelPin1Val + accelPin2Val) / 2;
    int16_t flippedVal = (avgVal * -1) + 1023; // reverse it so 0 is when pedal not pressed, and 1023 is at full press

    if (flippedVal < POT_LOWER_BOUND) { // Set 0 point to prevent a positive torque in the resting pedal position
      flippedVal = 0;
    }

    double multiplier = (double)flippedVal / 1023; // multiplier from 0 to 1;

    appliedTorque = multiplier * MAXIMUM_TORQUE;
  } 

  // calculate regen torque if no accel torque, the BMS says charging is enabled, and the brake is pressed
  if ((appliedTorque == 0) && chargeEnabled && brakePressed) {
    int totalBrakeTime = (millis() - timeBrake) / 1000; // length of time in seconds that the brake has been held down
   
    if (totalBrakeTime - START_TIME > RAMP_TIME) { // max regen torque is allowed
      appliedTorque = -MAX_REGEN_TORQUE;
    } 
    else if (totalBrakeTime - START_TIME > 0) {
      appliedTorque = totalBrakeTime * (MAX_REGEN_TORQUE / RAMP_TIME); // regen torque depends on how long the brake has been held
    }
    else {
      // do nothing
    }
  }

  uint8_t accelTorqueHighByte = appliedTorque >> 8;
  uint8_t accelTorqueLowByte = appliedTorque & 0x00FF;

  uint8_t message[8] = {accelTorqueLowByte,accelTorqueHighByte,0,0,isForward,1,0,0};
  sendMessage(CAN_MOTOR, 8, message); // send message to motor controller with torques and direction
}


/**
 * @brief Read the values of the two brake switches. Compare them to make sure 
 *        the values agree and send value to CAN
 * 
 */
void readSwitches() {
  brakePin1Val = digitalRead(BRAKE_PIN1);
  brakePin2Val = digitalRead(BRAKE_PIN2);

  if ((brakePin1Val == LOW) && (brakePin2Val == LOW)) {  // both switches are on
    if (!brakePressed) {  // set the brake time if the brake has just been pressed
      timeBrake = millis();
    } 
    brakePressed = true;
    currentBrakeErrors = 0;
    const uint8_t buf[1] = {1};
    sendMessage(CAN_BRAKE, 1, buf);
  } 
  else if ((brakePin1Val == HIGH) && (brakePin2Val == HIGH)) { // both switches are off
    brakePressed = false;
    currentBrakeErrors = 0;
    const uint8_t buf[1] = {0};
    sendMessage(CAN_BRAKE, 1, buf);
  } 
  else {  // error due to switches having different values
    brakePressed = false;
    currentBrakeErrors++;
    if (currentBrakeErrors >= MAX_BRAKE_ERRORS) { // send shutdown message, turn off motor, and send error message
      sendMessage(CAN_BMS_SHUTDOWN, 8, BMS_ERROR);
      sendMessage(CAN_MOTOR, 8, MOTOR_OFF);
      sendMessage(CAN_BRAKE_ERROR, 4, BRAKE_ERROR);
    }
    
  }
}


/**
 * @brief Handles incoming CAN messages. This CAN node acts as the ECU, so incoming messages are
 *        use to set the values of the global state variables
 * 
 * @param msg received CAN message
 */
void incomingCANCallback(const CAN_message_t &msg)
{
  if (msg.id == CAN_START_MOTOR) {
    sendMessage(CAN_MOTOR, 8, MOTOR_OFF); // turns motor off to release lockout or because switch is off
    invertorOn = msg.buf[0]; // set inverter state
  }
  else if (msg.id == CAN_DIRECTION) {
    isForward = msg.buf[0]; // set direction state
  } 
  else if (msg.id == CAN_BMS_STATES) {
    dischargeEnabled = msg.buf[5] & 1;     // get first bit of Relay State byte
    chargeEnabled = (msg.buf[5] >> 1) & 1; // get second bit of Relay State byte 
    lastBMSMessage = millis();
  } 
  else if (msg.id == CAN_MC_FAULTS) {
    if ((msg.buf[4] != 0) || ((msg.buf[5] >> 3) & 1) || ((msg.buf[5] >> 4) & 1)) { // only send message for fault bits 32-39, 43, 44
      sendMessage(CAN_BMS_SHUTDOWN, 8, BMS_ERROR); // triggers shutdown circuit through BMS
    }
  } 
  else if (msg.id == CAN_BMS_FAULTS) {
    if (((msg.buf[0] >> 5) & 1) || ((msg.buf[0] >> 6) & 1)) { // only send message for cell voltage too high/low faults
      sendMessage(CAN_BMS_SHUTDOWN, 8, BMS_ERROR); // triggers shutdown circuit through BMS
    }
  }
}

/**
 * @brief Sends a CAN message using the given parameters
 * 
 * @param id CAN id (assumed to be 11 bit)
 * @param len Length of message
 * @param buf Data buffer to send
 * @return int - Error code (Negative on failure, other on success)
 */
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf)
{
  CAN_message_t msg;
  msg.id = id;
  msg.len = len;
  uint8_t *buf1;

  for (int i = 0; i < 8; i++) {
    if (i < len) {
      buf1 = const_cast<uint8_t*>(buf + i);
      msg.buf[i] = *buf1;
    }
    else {
      msg.buf[i] = 0; // copies buf to message, padding with 0s if length isn't 8
    }
  }

  return myCan.write(msg);
}