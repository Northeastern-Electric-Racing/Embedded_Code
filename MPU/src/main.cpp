#include <nerduino.h>
#include "mpu.h"
#include <SD.h>
#include <TimeLib.h>

#define LOG_ALL 1 // set to 1 to log all CAN messages, 0 to filter

#define BAUD_RATE 250000U // 250 kbps 
#define MAX_MB_NUM 16 // maximum number of CAN mailboxes to use 

#define MAX_BUFFERED_MESSAGES 10 // max number of buffered CAN messages before logging to SD card
#define MIN_LOG_FREQUENCY 1000 // the max time length between logs (in ms)

typedef struct {
  char timestamp[25]; // timestamp in YYYY-MM-DDT00:00:00.000Z format
  uint32_t id;
  uint8_t length;
  uint8_t dataBuf[8]; // max number of bytes is 8
} message_format_t;

File logFile; // file logging object

// CAN Ids of the messages to log to SD card (only considered if LOG_ALL is 0)
const uint32_t LOG_IDS[] = {0x01, 0x02, 0x03, 0x04};
const int NUM_IDS = 4;

// Logging information
int bufLength = 0;
message_format_t messageBuf[MAX_BUFFERED_MESSAGES]; 
uint32_t lastLogTime = 0;
int fileNum = 0; // current file number
char fileName[16]; // format is log-0.txt (can support files up to number 99999999 as there can be 16 chars)

uint32_t startUpTimeMillis;
uint32_t startUpTimeRTC;

Timer mainTimer;

/**
 * @brief Returns a string with the current time in the format YYYY-MM-DDT00:00:00.000Z
 * 
 * @return int - 0 on success, 1 on error
 */
int getTimestamp(char *timestamp) {
  //time_t currentTime = now();
  
  // calculate millisecond precisions
  uint32_t millisSinceStart = millis() - startUpTimeMillis;
  //uint32_t millisSinceStartRTC = (currentTime - startUpTimeRTC) * 1000;
  
  // REMOVED THE BELOW FOR TESTING BEFORE RTC, REPLACE FOR ACTUAL CAR
  //uint32_t millisDifference = 0;
  //if (millisSinceStart - millisSinceStartRTC > 0) {
  //  currentTime += (millisSinceStart - millisSinceStartRTC) / 1000; // update currentTime if the millis go over a second
  //  millisDifference = (millisSinceStart - millisSinceStartRTC) % 1000; // set to be in range of 0-999 
  //}

  //sprintf(timestamp, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.3luZ", year(currentTime), month(currentTime), 
  //        day(currentTime), hour(currentTime), minute(currentTime), second(currentTime), millisDifference);

  int minutes = millisSinceStart / 60000;
  int seconds = (millisSinceStart - minutes*60000) / 1000;
  unsigned long milliseconds = millisSinceStart - minutes*60000 - seconds*1000;
  sprintf(timestamp, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.3luZ", 2021, 1, 1, 0, minutes, seconds, milliseconds);
  
  timestamp[24] = '\0'; // terminate string with NULL character

  return 0; // generic success code
}


/**
 * @brief Writes the messages currently buffered in messageBuf to the SD card
 * 
 * @return true on a successful write 
 * @return false when the write fails
 */
bool SDWrite() {
  logFile = SD.open(fileName, FILE_WRITE);
  Serial.print(" -- ");
  Serial.print(bufLength);
  Serial.print(" -- \n");

  if (logFile) {
    Serial.println("Writing to SD card...");

    // write all buffered messages to the SD card
    for (int i = 0; i < bufLength; i++) {
      logFile.print(messageBuf[i].timestamp);
      logFile.print(" ");
      logFile.print(messageBuf[i].id);
      logFile.print(" ");
      logFile.print(messageBuf[i].length);
      logFile.print(" [");
      for (int j = 0; j < messageBuf[i].length; j++) {
        logFile.print(messageBuf[i].dataBuf[j]);
        if ( j != messageBuf[i].length - 1) {
          logFile.print(",");
        }
      }
      logFile.print("]\n");
    }

    lastLogTime = millis();
    bufLength = 0;
    logFile.close();
    return true;
  } 
  else {
    Serial.println("Could not open file on SD card");
    return false;
  }
}

void setup()
{
    NERduino.begin();

    // init startup times
    startUpTimeMillis = millis();
    //startUpTimeRTC = now();

    initializeCAN(1);   //The "1" parameter is useless for now, in the future the parameter is which CAN line to initialize

    // init SD card
    while (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println(F("SD Init Failed!"));
        delay(250);
    }

    sprintf(fileName, "log-%d.txt", fileNum); // create the next file name in the fileName variable

    while (SD.exists(fileName)) { // find correct file number to log data to on this session
        Serial.print(F("log-"));
        Serial.print(fileNum);
        Serial.println(F(".txt exists..."));
        fileNum++;
        sprintf(fileName, "log-%d.txt", fileNum); // create the next file name in the fileName variable
    }

    Serial.print(F("setup complete. fileNum is "));
    Serial.println(String(fileNum));
}

MPU mpu;

void loop()
{
    myCan.events();
    mpu.driverioProcess();
    mpu.pedalsProcess();
    mpu.sendMCMsg();
    Serial.println("cycle");
}