#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "nerduino.h"

#define BAUD_RATE 250000U // 250 kbps 
#define MAX_MB_NUM 16 // maximum number of CAN mailboxes to use 

// CAN IDs
#define MC_STATES_ID       0xAA
#define CAN_START_MOTOR_ID 0x101
#define CAN_DIRECTION_ID   0x102

// pin values
#define CAN_FAULT_LED      0
#define GENERAL_LED_1      1 // custom behavior 
#define GENERAL_LED_2      2 // custom behavior
#define ON_OFF_LED         3 // intergrated LED on the ON_OFF button
#define ON_OFF_BUTTON      4
#define SHUTDOWN_DETECTION 5 // shutdown circuit enable for if the car can run
#define DIRECTION_SWITCH   8
#define SPEAKER            9


typedef struct {
  bool shutdownDetect;    // true when enabled and car can run
  bool power;             // true when car is on
  bool direction;         // true for forward
  bool prechargeComplete; // true when precharge has been completed
} States;

// CAN message buffers
const uint8_t ON[1] = {1};
const uint8_t OFF[1] = {0};
const uint8_t FORWARD[1] = {1}; 
const uint8_t REVERSE[1] = {0}; 

uint32_t speakerCooldown = 0; 

States states;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; // main CAN object


// function declarations
void onOffISR();
void shutdownISR();
void directionISR();
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf); 
void incomingCANCallback(const CAN_message_t &msg);


/**
 * @brief Initializes the serial console, CAN bus, dashboard pins, and state variables. The three input 
 *        pins are set trigger on interrupts to simplify the loop() code. The on/off button ISR is 
 *        triggered when the button is pressed, and both the direction switch and shutdown detection 
 *        ISRs are triggered on state change.
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

  pinMode(CAN_FAULT_LED, OUTPUT); 
  pinMode(GENERAL_LED_2, OUTPUT); 
  pinMode(GENERAL_LED_1, OUTPUT); 
  pinMode(ON_OFF_LED, OUTPUT); 
  pinMode(SPEAKER, OUTPUT);

  pinMode(ON_OFF_BUTTON, INPUT); 
  attachInterrupt(digitalPinToInterrupt(ON_OFF_BUTTON), onOffISR, FALLING);

  pinMode(SHUTDOWN_DETECTION, INPUT);
  attachInterrupt(digitalPinToInterrupt(SHUTDOWN_DETECTION), shutdownISR, CHANGE);

  pinMode(DIRECTION_SWITCH, INPUT); 
  attachInterrupt(digitalPinToInterrupt(DIRECTION_SWITCH), directionISR, CHANGE);

  states.direction = digitalRead(DIRECTION_SWITCH);
  states.power = false;
  states.prechargeComplete = false;
  states.shutdownDetect = digitalRead(SHUTDOWN_DETECTION);

  if (states.shutdownDetect) {
    digitalWrite(ON_OFF_LED, HIGH);
  }
}


/**
 * @brief Continuoulsy reads incoming CAN messages and turns the speaker off after its cooldown.
 */
void loop() {
  myCan.events();

  if (speakerCooldown < millis()) {
    analogWrite(SPEAKER, 0);
  }
}


/**
 * @brief ISR which triggers when the on/off button is pressed. If precharge isn't complete or the 
 *        shutdown circuit isn't enabled, nothing happens. Otherwise, the speaker is controlled and 
 *        an on/off CAN message is sent.
 */
void onOffISR() {
  if (!states.prechargeComplete || !states.shutdownDetect) {
    return;
  }

  states.power = !states.power;

  if (states.power) { // power on, start speaker
    analogWrite(SPEAKER, 128);
    speakerCooldown = millis() + 3000;
    
    sendMessage(CAN_START_MOTOR_ID, 1, ON); // Tell pedal box to turn inverter on
    
    if (states.direction) {
      sendMessage(CAN_DIRECTION_ID, 1, FORWARD); // tell pedal box to set inverter to forward
    } 
    else {
      sendMessage(CAN_DIRECTION_ID, 1, REVERSE);// tell pedal box to set inverter to reverse
    }

  } 
  else { // power off, turn speaker off
    analogWrite(SPEAKER, 0);
    sendMessage(CAN_START_MOTOR_ID, 1, OFF); // tell pedal box to turn inverter off
  }
}


/**
 * @brief ISR which triggers on changing state of the shutdown detect line.
 */
void shutdownISR() {
  states.shutdownDetect = digitalRead(SHUTDOWN_DETECTION);

  if (states.shutdownDetect) {
    digitalWrite(ON_OFF_LED, HIGH);
  }
  else {
    digitalWrite(ON_OFF_LED, LOW);
    states.power = false;
    analogWrite(SPEAKER, 0);
  }
}


/**
 * @brief ISR which triggers when the direction switch is toggled. If precharge isn't complete
 *        or the shutdown circuit isn't enabled, nothing happens. Otherwise, send a CAN message 
 *        to set the direction.
 */
void directionISR() {
  if (!states.prechargeComplete || !states.shutdownDetect) {
    return;
  }

  states.direction = digitalRead(DIRECTION_SWITCH);

  if (states.direction) {
    sendMessage(CAN_DIRECTION_ID, 1, FORWARD); // tell pedal box to set inverter to forward
  } 
  else {
    sendMessage(CAN_DIRECTION_ID, 1, REVERSE);// tell pedal box to set inverter to reverse
  }
}


/**
 * @brief Handles incoming CAN messages.
 * 
 * @param msg received CAN message
 */
void incomingCANCallback(const CAN_message_t &msg)
{
  if (msg.id == MC_STATES_ID) {
    states.prechargeComplete = (msg.buf[0] >> 4) & 1; // true if MC is in wait state 
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