#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <TimeLib.h>
#include "Watchdog_t4.h"

#include "nerduino.h"
#include "rtc.h"
#include "message.h"
#include "xbee_at.h"
#include "logger.h"
#include "debug.h"


#define ENABLE_TEST_LOGGING    0 // set to 1 to log the test messages in the main loop()
#define LOG_ALL_MESSAGES       0 // set to 1 to log all CAN messages, 0 to filter
#define SEND_XBEE_ALL_MESSAGES 0 // set to 1 to send all CAN messages, 0 to filter

#define BAUD_RATE 1000000U // 250 kbps 
#define MAX_MB_NUM 16 // maximum number of CAN mailboxes to use 

#define MIN_LOG_FREQUENCY 1000 // the max time length between logs (in ms)

#define SENSOR_LOG_FREQUENCY 100 // time between recording Nerduino sensor data
#define ACCELEROMETER_ID 0x300
#define TEMP_HUMID_ID 0x301

#define ANALOG1_PIN A0 // Pin 14 on teensy
#define ANALOG2_PIN A6       
#define ANALOG3_PIN A7 

#define ANALOG1_LOG_ID      0x302  
#define STRAIN_GAUGE_LOG_ID 0x303

#define LED_BLINK_DELAY_MS 500


FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; // main CAN object
WDT_T4<WDT1> wdt;

int blinkLedState = LOW;

// CAN Ids of the messages to log to SD card (only considered if LOG_ALL is 0)
const uint32_t LOG_IDS[] = {
  0x001,
  0x002, 
  0x003, 
  0x004, 
  0x0A0, 
  0x0A1, 
  0x0A2, 
  0x0A5, 
  0x0A6, 
  0x0A7,
  0x0A8,
  0x0A9,
  0x0AA,
  0x0AB, 
  0x0AC, 
  0x202, 
  0xC0,
  0x36};
const int NUM_LOG_IDS = sizeof(LOG_IDS) / sizeof(uint32_t);

// CAN Ids of the messages to send via XBee (only considered if SEND_XBEE_ALL_MESSAGES is 0)
const uint32_t SEND_XBEE_IDS[] = {
  0xA5, 
  0x202
};
const int NUM_SEND_XBEE_IDS = sizeof(SEND_XBEE_IDS) / sizeof(uint32_t);


// function declarations
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf); 
void incomingCANCallback(const CAN_message_t &msg);
void logAccelerometerData();
void logTempSensorData();
void logAnalogs();


/**
 * @brief Blinks the LED when logging, does nothing when not logging
 * 
 */
static void blinkLED() {
  static uint32_t lastLedBlinkTime = 0;
  if (millis() - lastLedBlinkTime > LED_BLINK_DELAY_MS) {
    if (LoggerActive() && blinkLedState == LOW) {
      blinkLedState = HIGH;
    } else {
      blinkLedState = LOW;
    }
    digitalWrite(LED_BUILTIN, blinkLedState);
    lastLedBlinkTime = millis();
  }
}


/**
 * @brief Init the many peripherals and communication lines.
 * 
 */
void setup() {
  Serial.begin(9600); 
  delay(100);

  RtcInit();
  LoggerInit(MIN_LOG_FREQUENCY);

  NERduino.begin();
  delay(3000); // TODO: Do we need this delay

  myCan.begin();
  myCan.setBaudRate(BAUD_RATE);
  myCan.setMaxMB(MAX_MB_NUM);
  myCan.enableFIFO(); 
  myCan.enableFIFOInterrupt(); 
  myCan.onReceive(incomingCANCallback);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ANALOG1_PIN, INPUT);
  pinMode(ANALOG2_PIN, INPUT);
  pinMode(ANALOG3_PIN, INPUT);
  
  XBeeInit(&Serial1, 115200);

  WDT_timings_t config;
  config.trigger = 5; /* in seconds, 0->128 */
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
  blinkLED();

  if (LoggerWrite() == LOGGER_STATUS::LGR_ERROR_SD_CARD) {
    LoggerInit(MIN_LOG_FREQUENCY);
    DPRINTLN(F("Reinitializing logger due to internal error")); 
  }

  // logging the extra sensor data from the accelerometer and temp/humid sensor
  static uint32_t dataLastRecorded = 0;
  if (millis() - dataLastRecorded > SENSOR_LOG_FREQUENCY) {
    logAccelerometerData();
    logTempSensorData();
    logAnalogs();
    dataLastRecorded = millis();
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
 * @brief Log the data from the Nerduino accelerometer
 */
void logAccelerometerData() {
  XYZData_t xyzData[1];
  NERduino.getADXLdata(xyzData, 1);
  uint8_t accelBuf[6] = {
    xyzData[0].XData.rawdata[0], xyzData[0].XData.rawdata[1],
    xyzData[0].YData.rawdata[0], xyzData[0].YData.rawdata[1],
    xyzData[0].ZData.rawdata[0], xyzData[0].ZData.rawdata[1]
  };

  message_t message;
  message.id = ACCELEROMETER_ID;
  message.length = 6;
  memcpy(message.dataBuf, accelBuf, 6);
  RtcGetTime(&message.timestamp);

  LoggerBufferMessage(&message);
}


/**
 * @brief Log the data from the Nerduino humidity/temp sensor
 */
void logTempSensorData() {
  HumidData_t humidData[1];
  NERduino.getSHTdata(humidData, 1);
  uint8_t humidBuf[4] = {
    humidData[0].TempData.rawdata[0], humidData[0].TempData.rawdata[1],
    humidData[0].HumidData.rawdata[0], humidData[0].HumidData.rawdata[1]
  };

  message_t message;
  message.id = TEMP_HUMID_ID;
  message.length = 4;
  memcpy(message.dataBuf, humidBuf, 4);
  RtcGetTime(&message.timestamp);

  LoggerBufferMessage(&message);
}


/**
 * @brief Log data from connected analog sensors
 * 
 */
void logAnalogs() {
  int analog1Value = (analogRead(ANALOG1_PIN) - 509) * 74054.326212; // 0.00488 / 0.066 * 10^6 scale factor
  int analog2Value = analogRead(ANALOG2_PIN) * 3225.80645; // convert to 3.3V with 10^6 scale factor
  int analog3Value = analogRead(ANALOG3_PIN) * 3225.80645;

  uint8_t analog1Buf[4] = {
    analog1Value & 255, (analog1Value >> 8) & 255, (analog1Value >> 16) & 255, (analog1Value >> 24) & 255
  };
  uint8_t strainGaugeBuf[8] = {
    analog2Value & 255, (analog2Value >> 8) & 255, (analog2Value >> 16) & 255, (analog2Value >> 24) & 255,
    analog3Value & 255, (analog3Value >> 8) & 255, (analog3Value >> 16) & 255, (analog3Value >> 24) & 255
  };

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

  LoggerBufferMessage(&message1);
  LoggerBufferMessage(&message2);
}


/**
 * @brief Handles incoming CAN messages, adding certain ones to the logging message buffer.
 * 
 * @param msg received CAN message
 */
void incomingCANCallback(const CAN_message_t &msg) {
  message_t message;
  message.id = msg.id;
  message.length = msg.len;
  memcpy(message.dataBuf, msg.buf, msg.len);
  RtcGetTime(&message.timestamp);

  // Log if global logging is enabled or search through loggable message IDs
  if (LOG_ALL_MESSAGES) {
    LoggerBufferMessage(&message);
  } else {
    for (int i = 0; i < NUM_LOG_IDS; i++) {
      if (LOG_IDS[i] == msg.id) {
        LoggerBufferMessage(&message);
        break;
      }
    }
  }

  // Send if global XBee sending is enabled or search through sendable message IDs
  if (SEND_XBEE_ALL_MESSAGES) {
    XBeeSendMessage(&message);
  } else {
    for (int i = 0; i < NUM_SEND_XBEE_IDS; i++) {
      if (SEND_XBEE_IDS[i] == msg.id) {
        XBeeSendMessage(&message);
        break;
      }
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
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf) {
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