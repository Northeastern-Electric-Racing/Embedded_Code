#include <Arduino.h>
#include <FlexCAN_T4.h>

#define BAUD_RATE 250000U        // 250 kbps
#define MAX_MB_NUM 16            // maximum number of CAN mailboxes to use
#define BRAKE_LIGHT_PIN 8        // Does not have to be PWM pin.
#define BRAKE_LIGHT_CAN_ID 0x103 // id for the brake light CAN message

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; // main CAN object

// prototype for the CAN receive callback
void changeBrakeLight(const CAN_message_t &msg);

/**
 * @brief Initializes the serial console, brake light LED pin, and CAN bus
 *
 */
void setup() {
  Serial.begin(115200);
  delay(400);

  pinMode(BRAKE_LIGHT_PIN, OUTPUT);

  myCan.begin();
  myCan.setBaudRate(BAUD_RATE);
  myCan.setMaxMB(MAX_MB_NUM);
  myCan.enableFIFO();
  myCan.enableFIFOInterrupt();
  myCan.onReceive(changeBrakeLight);
}

/**
 * @brief Starts the connection to the CAN bus
 *
 */
void loop() { myCan.events(); }

/**
 * @brief Changes the state of the brake light (on/off) according to
 *        the given CAN message
 *
 * @param msg received CAN message
 */
void changeBrakeLight(const CAN_message_t &msg) {
  if (msg.id == BRAKE_LIGHT_CAN_ID) {
    digitalWrite(BRAKE_LIGHT_PIN, msg.buf[0]);
  }
}
