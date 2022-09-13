#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "nerduino.h"
#include "Watchdog_t4.h"
#include "xbee_at.h"
#include "logger.h"
#include "debug.h"

#define TEST_LOG 0 // set to 1 to log the test messages in the main loop()
#define LOG_ALL 0 // set to 1 to log all CAN messages, 0 to filter

#define BAUD_RATE 1000000U // 250 kbps 
#define MAX_MB_NUM 16 // maximum number of CAN mailboxes to use 

#define MIN_LOG_FREQUENCY 1000 // the max time length between logs (in ms)

#define ACCEL_HUMID_LOG_FREQUENCY 100 // time between logging accel/humid data
#define ACCEL_LOG_ID 0x300
#define HUMID_LOG_ID 0x301


FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; // main CAN object
WDT_T4<WDT1> wdt;

bool loggable = false;


// CAN Ids of the messages to log to SD card (only considered if LOG_ALL is 0)
const uint32_t LOG_IDS[] = {0x001, 0x002, 0x003, 0x004, 0x0A0, 0x0A1, 0x0A2, 0x0A5, 0x0A6, 0x0A7, 0x0AA, 0x0AB, 0x0AC, 0x202, 0xC0};
const int NUM_IDS = sizeof(LOG_IDS) / sizeof(uint32_t);


// function declarations
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf); 
void incomingCANCallback(const CAN_message_t &msg);
void logSensorData();


/**
 * @brief Init the many peripherals and communication lines.
 * 
 */
void setup() {
  Serial.begin(9600); 
  delay(100);

  LoggerInit(MIN_LOG_FREQUENCY);

  NERduino.begin();
  delay(3000); // TODO: Do we need this delay

  myCan.begin();
  myCan.setBaudRate(BAUD_RATE);
  myCan.setMaxMB(MAX_MB_NUM);
  myCan.enableFIFO(); 
  myCan.enableFIFOInterrupt(); 
  myCan.onReceive(incomingCANCallback);
  
  // XbeeInit(&Serial1, 115200);

  WDT_timings_t config;
  config.trigger = 5; /* in seconds, 0->128 */
  config.timeout = 15; /* in seconds, 0->128 */
  wdt.begin(config);
}


/**
 * @brief Continuously read incoming CAN messages and the values of the 
 *        accelerator potentiometers and brake switches
 * 
 */
void loop() {
  wdt.feed();
  myCan.events();

  if (LoggerWrite() == LOGGER_ERROR_SD_CARD) {
    LoggerInit(MIN_LOG_FREQUENCY);
    DPRINTLN(F("Reinitializing logger due to internal error")); 
  }

  // logging the extra sensor data from the accelerometer and temp/humid sensor
  static uint32_t dataLastRecorded = 0;
  if (millis() - dataLastRecorded > ACCEL_HUMID_LOG_FREQUENCY) {
    logSensorData();
    dataLastRecorded = millis();
  }

  // USED FOR TESTING WHEN NOT CONNECTED TO CAN
#if TEST_LOG == 1 
  static unsigned long writeTime = millis();
  static uint8_t writeData = 0;
  if (millis() - writeTime > 10) {
    const uint8_t buf[] = {writeData, writeData, writeData, writeData};
    LoggerBufferMessage(0x01, 4, buf);

    writeData++;
    writeData %= 20;
    writeTime = millis();
  }
#endif
}


/**
 * @brief Log the data from the attached sensors (accelerometer and humidity/temp sensor)
 * 
 */
void logSensorData() {
  XYZData_t xyzData[1];
  HumidData_t humidData[1];

  NERduino.getADXLdata(xyzData, 1);
  NERduino.getSHTdata(humidData, 1);

  uint8_t accelBuf[6] = {
    xyzData[0].XData.rawdata[0], xyzData[0].XData.rawdata[1],
    xyzData[0].YData.rawdata[0], xyzData[0].YData.rawdata[1],
    xyzData[0].ZData.rawdata[0], xyzData[0].ZData.rawdata[1]
  };

  uint8_t humidBuf[4] = {
    humidData[0].TempData.rawdata[0], humidData[0].TempData.rawdata[1],
    humidData[0].HumidData.rawdata[0], humidData[0].HumidData.rawdata[1]
  };

  LoggerBufferMessage(ACCEL_LOG_ID, 6, accelBuf);
  LoggerBufferMessage(HUMID_LOG_ID, 4, humidBuf);
}

/**
 * @brief Handles incoming CAN messages, adding certain ones to the logging message buffer.
 * 
 * @param msg received CAN message
 */
void incomingCANCallback(const CAN_message_t &msg)
{
  // only log if the message id is in the LOG_IDS list or LOG_ALL is enabled
  int foundId = 0;
  if (LOG_ALL) {
    foundId = 1;
  } else {
    for (int i = 0; i < NUM_IDS; i++) {
      if (LOG_IDS[i] == msg.id) {
        foundId = 1;
        break;
      }
    }
  }

  // exit the function if we shouldn't log message
  if (!foundId) {
    return;
  }

  // add message to log buffer
  LoggerBufferMessage(msg.id, msg.len, msg.buf);
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