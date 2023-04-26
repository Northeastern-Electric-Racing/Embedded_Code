#include "Watchdog_t4.h"
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <TimeLib.h>

#include "debug.h"
#include "gnss.h"
#include "logger.h"
#include "message.h"
#include "nerduino.h"
#include "rtc.h"
#include "xbee_at.h"

#define ENABLE_TEST_LOGGING                                                    \
  0 // set to 1 to log the test messages in the main loop()
#define LOG_ALL_MESSAGES 0 // set to 1 to log all CAN messages, 0 to filter
#define SEND_XBEE_ALL_MESSAGES                                                 \
  0 // set to 1 to send all CAN messages, 0 to filter

#define XBEE_SERIAL Serial2
#define XBEE_BAUD_RATE 115200
#define GNSS_SERIAL Serial1
#define GNSS_BAUD_RATE 38400

#define CAN_BAUD_RATE 1000000U // 1 Mbps
#define MAX_MB_NUM 16          // maximum number of CAN mailboxes to use

#define MIN_LOG_FREQUENCY 1000   // the max time length between logs (in ms)
#define SENSOR_LOG_FREQUENCY 100 // time between recording Nerduino sensor data
#define GNSS_LOG_FREQUENCY 500   // time between GNSS logs

#define ANALOG1_PIN A0 // Pin 14 on teensy
#define ANALOG2_PIN A6
#define ANALOG3_PIN A7

#define ACCELEROMETER_ID 0x300
#define TEMP_HUMID_ID 0x301
#define ANALOG1_LOG_ID 0x302
#define STRAIN_GAUGE_LOG_ID 0x303
#define GNSS_1_ID 0x304
#define GNSS_2_ID 0x305
#define GNSS_3_ID 0x306
#define LOGGING_STATUS_ID 0X307

#define LED_BLINK_DELAY_MS 500

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; // main CAN object
WDT_T4<WDT1> wdt;

int blinkLedState = LOW;

// CAN Ids of the messages to log to SD card (only considered if LOG_ALL is 0)
const uint32_t LOG_IDS[] = {
    0x001, 0x002, 0x003, 0x004, 0x0A0, 0x0A1, 0x0A2, 0x0A5, 0x0A6, 0x0A7, 0x0A8,
    0x0A9, 0x0AA, 0x0AB, 0x0AC, 0x202, 0xC0,  0x36,  0xB1,  0x300, 0x301, 0x302,
    0x303, 0x304, 0x305, 0x306, 0x307, 0x7,   0x8,   0x9,   0xA};
const int NUM_LOG_IDS = sizeof(LOG_IDS) / sizeof(uint32_t);

// CAN Ids of the messages to send via XBee (only considered if
// SEND_XBEE_ALL_MESSAGES is 0)
const uint32_t SEND_XBEE_IDS[] = {0xA5,  0x202, 0x300, 0x301, 0x304, 0x305,
                                  0x306, 0x307, 0xA,   0x1,   0x2};
const int NUM_SEND_XBEE_IDS = sizeof(SEND_XBEE_IDS) / sizeof(uint32_t);

// function declarations
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf);
void incomingCANCallback(const CAN_message_t &msg);
void logAccelerometerData();
void logTempSensorData();
void logAnalogs();
void logGnssData();
void checkLoggingStatus();
void tryLog(message_t *message);
void tryXbee(message_t *message);

/**
 * @brief Init the many peripherals and communication lines.
 *
 */
void setup() {
  Serial.begin(9600); // must be done first
  delay(100);

  RtcInit();
  LoggerInit(MIN_LOG_FREQUENCY);

  NERduino.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(ANALOG1_PIN, INPUT);
  // pinMode(ANALOG2_PIN, INPUT);
  // pinMode(ANALOG3_PIN, INPUT);

  GnssInit(GNSS_SERIAL, GNSS_BAUD_RATE);
  XBeeInit(&XBEE_SERIAL, XBEE_BAUD_RATE);

  myCan.begin();
  myCan.setBaudRate(CAN_BAUD_RATE);
  myCan.setMaxMB(MAX_MB_NUM);
  myCan.enableFIFO();
  myCan.enableFIFOInterrupt();
  myCan.onReceive(incomingCANCallback);

  WDT_timings_t config;
  config.trigger = 5;  /* in seconds, 0->128 */
  config.timeout = 15; /* in seconds, 0->128 */
  wdt.begin(config);
}

/**
 * @brief Continuously read incoming CAN messages and log messages.
 *
 */
void loop() {
  wdt.feed();
  myCan.events();
  checkLoggingStatus();

  if (LoggerWrite() == LOGGER_STATUS::LGR_ERROR_SD_CARD) {
    LoggerInit(MIN_LOG_FREQUENCY);
    DPRINTLN(F("Reinitializing logger due to internal error"));
  }

  // logging extra sensor data
  static uint32_t dataLastRecorded = 0;
  if (millis() - dataLastRecorded > SENSOR_LOG_FREQUENCY) {
    logAccelerometerData();
    logTempSensorData();
    // logAnalogs();
    dataLastRecorded = millis();
  }

  // logging GNSS data
  static uint32_t gnssLastRecorded = 0;
  if (millis() - gnssLastRecorded > GNSS_LOG_FREQUENCY) {
    logGnssData();
    gnssLastRecorded = millis();
  }

  // USED FOR TESTING WHEN NOT CONNECTED TO CAN
#if ENABLE_TEST_LOGGING == 1
  static unsigned long writeTime = millis();
  static uint8_t writeData = 0;
  if (millis() - writeTime > 1000) {
    const uint8_t buf[] = {writeData, writeData, writeData, writeData};
    message_t message;
    message.id = 0x01;
    message.length = 4;
    memcpy(message.dataBuf, buf, 4);
    RtcGetTime(&message.timestamp);
    LoggerBufferMessage(&message);
    XBeeSendMessage(&message);

    writeData++;
    writeData %= 20;
    writeTime = millis();
  }
#endif
}

/**
 * @brief Handles incoming CAN messages, adding certain ones to the logging
 * message buffer.
 *
 * @param msg received CAN message
 */
void incomingCANCallback(const CAN_message_t &msg) {
  message_t message;
  message.id = msg.id;
  message.length = msg.len;
  memcpy(message.dataBuf, msg.buf, msg.len);
  RtcGetTime(&message.timestamp);
  tryLog(&message);
  tryXbee(&message);
}

/**
 * @brief Sends a CAN message using the given parameters
 *
 * @param id CAN id (assumed to be 11 bit)
 * @param len Length of message
 * @param buf Data buffer to send
 * @return int - Error code (Negative on failure, other on success)
 */
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf) {
  CAN_message_t msg;
  msg.id = id;
  msg.len = len;
  uint8_t *buf1;

  for (int i = 0; i < 8; i++) {
    if (i < len) {
      buf1 = const_cast<uint8_t *>(buf + i);
      msg.buf[i] = *buf1;
    } else {
      msg.buf[i] =
          0; // copies buf to message, padding with 0s if length isn't 8
    }
  }

  return myCan.write(msg);
}

/**
 * @brief Log the data from the Nerduino accelerometer
 */
void logAccelerometerData() {
  XYZData_t xyzData[1];
  NERduino.getADXLdata(xyzData, 1);
  uint8_t accelBuf[6] = {
      xyzData[0].XData.rawdata[0], xyzData[0].XData.rawdata[1],
      xyzData[0].YData.rawdata[0], xyzData[0].YData.rawdata[1],
      xyzData[0].ZData.rawdata[0], xyzData[0].ZData.rawdata[1]};

  message_t message;
  message.id = ACCELEROMETER_ID;
  message.length = 6;
  memcpy(message.dataBuf, accelBuf, 6);
  RtcGetTime(&message.timestamp);

  tryLog(&message);
  tryXbee(&message);
  sendMessage(message.id, message.length, message.dataBuf);
}

/**
 * @brief Log the data from the Nerduino humidity/temp sensor
 */
void logTempSensorData() {
  HumidData_t humidData[1];
  NERduino.getSHTdata(humidData, 1);
  uint8_t humidBuf[4] = {
      humidData[0].TempData.rawdata[0], humidData[0].TempData.rawdata[1],
      humidData[0].HumidData.rawdata[0], humidData[0].HumidData.rawdata[1]};

  message_t message;
  message.id = TEMP_HUMID_ID;
  message.length = 4;
  memcpy(message.dataBuf, humidBuf, 4);
  RtcGetTime(&message.timestamp);

  tryLog(&message);
  tryXbee(&message);
  sendMessage(message.id, message.length, message.dataBuf);
}

/**
 * @brief Log data from connected analog sensors
 *
 */
void logAnalogs() {
  int analog1Value = (analogRead(ANALOG1_PIN) - 509) *
                     74054.326212; // 0.00488 / 0.066 * 10^6 scale factor
  int analog2Value = analogRead(ANALOG2_PIN) *
                     3225.80645; // convert to 3.3V with 10^6 scale factor
  int analog3Value = analogRead(ANALOG3_PIN) * 3225.80645;

  uint8_t analog1Buf[4] = {analog1Value & 255, (analog1Value >> 8) & 255,
                           (analog1Value >> 16) & 255,
                           (analog1Value >> 24) & 255};
  uint8_t strainGaugeBuf[8] = {
      analog2Value & 255,         (analog2Value >> 8) & 255,
      (analog2Value >> 16) & 255, (analog2Value >> 24) & 255,
      analog3Value & 255,         (analog3Value >> 8) & 255,
      (analog3Value >> 16) & 255, (analog3Value >> 24) & 255};

  message_t message1;
  message1.id = ANALOG1_LOG_ID;
  message1.length = 4;
  memcpy(message1.dataBuf, analog1Buf, 4);
  RtcGetTime(&message1.timestamp);

  message_t message2;
  message2.id = STRAIN_GAUGE_LOG_ID;
  message2.length = 8;
  memcpy(message2.dataBuf, strainGaugeBuf, 8);
  RtcGetTime(&message2.timestamp);

  tryLog(&message1);
  tryXbee(&message1);
  tryLog(&message2);
  tryXbee(&message2);
}

/**
 * @brief Log data from the GNSS module
 *
 */
void logGnssData() {
  int32_t latitude = GnssGetLatitude();
  int32_t longitude = GnssGetLongitude();
  int32_t fixStatus = GnssGetFixOk();
  int32_t altitude = GnssGetAltitude();
  int32_t groundSpeed = GnssGetGroundSpeed();
  int32_t heading = GnssGetHeading();

  uint8_t data1[8] = {latitude & 255,          (latitude >> 8) & 255,
                      (latitude >> 16) & 255,  (latitude >> 24) & 255,
                      longitude & 255,         (longitude >> 8) & 255,
                      (longitude >> 16) & 255, (longitude >> 24) & 255};
  uint8_t data2[8] = {fixStatus & 255,         (fixStatus >> 8) & 255,
                      (fixStatus >> 16) & 255, (fixStatus >> 24) & 255,
                      altitude & 255,          (altitude >> 8) & 255,
                      (altitude >> 16) & 255,  (altitude >> 24) & 255};
  uint8_t data3[8] = {groundSpeed & 255,
                      (groundSpeed >> 8) & 255,
                      (groundSpeed >> 16) & 255,
                      (groundSpeed >> 24) & 255,
                      heading & 255,
                      (heading >> 8) & 255,
                      (heading >> 16) & 255,
                      (heading >> 24) & 255};

  message_t message1;
  message1.id = GNSS_1_ID;
  message1.length = 8;
  memcpy(message1.dataBuf, data1, 8);
  RtcGetTime(&message1.timestamp);

  message_t message2;
  message2.id = GNSS_2_ID;
  message2.length = 8;
  memcpy(message2.dataBuf, data2, 8);
  RtcGetTime(&message2.timestamp);

  message_t message3;
  message3.id = GNSS_3_ID;
  message3.length = 8;
  memcpy(message3.dataBuf, data3, 8);
  RtcGetTime(&message3.timestamp);

  tryLog(&message1);
  tryXbee(&message1);
  sendMessage(message1.id, message1.length, message1.dataBuf);
  tryLog(&message2);
  tryXbee(&message2);
  sendMessage(message2.id, message2.length, message2.dataBuf);
  tryLog(&message3);
  tryXbee(&message3);
  sendMessage(message3.id, message3.length, message3.dataBuf);
}

/**
 * @brief Blinks the LED when logging, does nothing when not logging.
 * Also sends a log status CAN message.
 *
 */
void checkLoggingStatus() {
  static uint32_t lastLedBlinkTime = 0;
  if (millis() - lastLedBlinkTime > LED_BLINK_DELAY_MS) {
    if (LoggerActive()) {
      if (blinkLedState == LOW) {
        blinkLedState = HIGH;
      } else {
        blinkLedState = LOW;
      }
    } else {
      blinkLedState = LOW;
    }

    // send logging status message
    message_t message;
    message.id = LOGGING_STATUS_ID;
    message.length = 1;
    uint8_t data[1] = {0};
    if (LoggerActive()) {
      data[0] = 1;
    }
    memcpy(message.dataBuf, data, 1);
    RtcGetTime(&message.timestamp);
    tryLog(&message);
    tryXbee(&message);
    sendMessage(message.id, message.length, message.dataBuf);

    digitalWrite(LED_BUILTIN, blinkLedState);

    lastLedBlinkTime = millis();
  }
}

/**
 * @brief Log if global logging is enabled or ID is specified in LOG_IDS.
 *
 * @param message Message to log
 */
void tryLog(message_t *message) {
  if (LOG_ALL_MESSAGES) {
    LoggerBufferMessage(message);
  } else {
    for (int i = 0; i < NUM_LOG_IDS; i++) {
      if (LOG_IDS[i] == message->id) {
        LoggerBufferMessage(message);
        break;
      }
    }
  }
}

/**
 * @brief Send over XBee if global sending is enabled or ID is specified in
 * SEND_XBEE_IDS.
 *
 * @param message Message to send
 */
void tryXbee(message_t *message) {
  if (SEND_XBEE_ALL_MESSAGES) {
    XBeeSendMessage(message);
  } else {
    for (int i = 0; i < NUM_SEND_XBEE_IDS; i++) {
      if (SEND_XBEE_IDS[i] == message->id) {
        XBeeSendMessage(message);
        break;
      }
    }
  }
}