/**
 * The standard block size for SD card transfers is 512 bytes. Each write operation to the SD card should
 * be as close to this size as possible, as the remaining space will be padded and used anyway.
 *  - each message is a max of 49 bytes, so a buffer size on multiples of 10 is ideal
 * 
 * The Teensy Loader automatically syncs the RTC to the PCs system time on upload, so the time library
 * can be used for real time access if a coin cell is connected to VBAT on the teensy.
 * 
 * All RTC times are in units of seconds since Jan 1st 1970, which is an unsigned long.
 *   - We also keep track of the millis() of the car for extra precision on time stamps
 * 
 * The format for each CAN message logged to the SD card will be:
 *   - time canId length [dataBytes]
 *   - time is in RFC339 format: YYYY-MM-DDT00:00:00.000Z 
 *   - Example Format: 2022-01-12T14:32:21.657Z 123 6 [123,9,12,0,3,15] 
 * 
 */

#include <Arduino.h>
#include <SD.h>

#include "logger.h"
#include "rtc.h"
#include "debug.h"

#define MAX_BUFFERED_MESSAGES 50 // max number of buffered CAN messages before logging to SD card
#define MAX_LOG_FILE_SIZE 50000000U // 50 Megabytes

#define FILE_NAME_SIZE 19 // 10 for timestamp, 8 for text, 1 for termination


File logFile; // file logging object
char fileName[FILE_NAME_SIZE]; // format is log-1652888997.txt

// Buffer information (use 2 buffers to prevent overwrites during logging delays)
int buf1Length = 0;
int buf2Length = 0;
message_t messageBuf1[MAX_BUFFERED_MESSAGES]; 
message_t messageBuf2[MAX_BUFFERED_MESSAGES]; 
bool usingBuf1 = true;

bool initialized = false; // flag to verify logging has been initialized

// Timing information
uint32_t lastLogTime;
uint32_t minLogFrequency;


/**
 * @brief Sets the logging file name using the format log-NUM.txt, where NUM is either the current 
 *        unix time if using the RTC or a valid numeric index starting from 0.
 * 
 */
static void setFileName() {
  uint32_t fileNum = 0;
  rtc_time_t currentTime;
  RTC_STATUS timeStatus = RtcGetTime(&currentTime);
  if (timeStatus == RTC_STATUS::RTC_SUCCESS) {
    fileNum = currentTime.seconds;
  }

  snprintf(fileName, FILE_NAME_SIZE, "log-%lu.txt", fileNum);
  while (SD.exists(fileName)) {
    fileNum++;
    snprintf(fileName, FILE_NAME_SIZE, "log-%lu.txt", fileNum); 
  }
}


/**
 * @brief Resets the buffers, SD card, and clock
 * 
 */
static void reset() {
  noInterrupts()
  initialized = false;
  buf1Length = 0;
  buf2Length = 0;
  memset((void *)messageBuf1, 0, MAX_BUFFERED_MESSAGES * sizeof(message_t));
  memset((void *)messageBuf2, 0, MAX_BUFFERED_MESSAGES * sizeof(message_t));
  usingBuf1 = true;
  interrupts()
}


LOGGER_STATUS LoggerInit(uint32_t logFrequency) {
  if (!SD.begin(BUILTIN_SDCARD)) {
    DPRINTLN(F("Error initializing SD card logging"));
    return LOGGER_STATUS::LGR_ERROR_SD_CARD;
  }
  
  setFileName();
  DPRINT(F("Setup complete. File name is "));
  DPRINTLN(fileName);

  minLogFrequency = logFrequency;
  lastLogTime = millis();
  logFile = SD.open(fileName, FILE_WRITE);
  
  initialized = true;
  return LOGGER_STATUS::LGR_SUCCESS;
}


bool LoggerActive() {
  return initialized;
}


LOGGER_STATUS LoggerBufferMessage(message_t *message) {
  if (!initialized) {
    return LOGGER_STATUS::LGR_ERROR_SD_CARD;
  }

  noInterrupts();
  // find appropriate buffer to use
  message_t *messageBuf = usingBuf1 ? messageBuf1 : messageBuf2; 
  int bufLength = usingBuf1 ? buf1Length : buf2Length;

  if (bufLength >= MAX_BUFFERED_MESSAGES) {
    DPRINTLN(F("Tried to write to already full buffer"));
    return LOGGER_STATUS::LGR_ERROR_BUFFER_FULL;
  }

  messageBuf[bufLength] = *message;

  if (usingBuf1) {
    buf1Length++;
  } else {
    buf2Length++;
  }
  interrupts();

  return LOGGER_STATUS::LGR_SUCCESS;
}


LOGGER_STATUS LoggerWrite() {
  if (!initialized) {
    return LOGGER_STATUS::LGR_ERROR_SD_CARD;
  }

  // find appropriate buffer to use
  message_t *messageBuf = usingBuf1 ? messageBuf1 : messageBuf2; 
  int bufLength = usingBuf1 ? buf1Length : buf2Length;

  // only write on certain conditions
  if ((millis() - lastLogTime) <= minLogFrequency && bufLength < MAX_BUFFERED_MESSAGES) {
    return LOGGER_STATUS::LGR_ERROR_NO_WRITE;
  }

  usingBuf1 = !usingBuf1; // Switch main log buffer during write process

  // create a new file if current one is too large
  if (logFile.size() > MAX_LOG_FILE_SIZE) {
    logFile.close();
    setFileName();
    logFile = SD.open(fileName, FILE_WRITE);
  }

  DPRINT(F(" -- "));
  DPRINT(F("Added from buf "));
  if (usingBuf1) {
    DPRINT(F("2: "));
  } else {
    DPRINT(F("1: "));
  }
  DPRINT(bufLength);
  DPRINT(F(" -- \n"));

  if (logFile) {
    DPRINTLN(F("Writing to SD card..."));

    // write all buffered messages to the SD card
    for (int i = 0; i < bufLength; i++) {
      int writeLength = logFile.print(messageBuf[i].timestamp.seconds);
      // checks for error on write (assumes rest are fine if this passes)
      if (writeLength == 0) {
        DPRINTLN(F("Could not write to SD ... Reseting"));
        reset();
        return LOGGER_STATUS::LGR_ERROR_SD_CARD;
      }
      logFile.printf("%.3lu", messageBuf[i].timestamp.millis);
      logFile.print(F(" "));
      logFile.print(messageBuf[i].id);
      logFile.print(F(" "));
      logFile.print(messageBuf[i].length);
      logFile.print(F(" ["));
      for (int j = 0; j < messageBuf[i].length; j++) {
        logFile.print(messageBuf[i].dataBuf[j]);
        if (j != messageBuf[i].length - 1) {
          logFile.print(F(","));
        }
      }
      logFile.print(F("]\n"));
    }

    lastLogTime = millis();
    
    // clear bufLength variables (must clear the opposite of whatever one is in use)
    if (usingBuf1) {
      DPRINTLN(F("Reset buffer 2"));
      buf2Length = 0;
    } else {
      DPRINTLN(F("Reset buffer 1"));
      buf1Length = 0;
    }
    logFile.flush();
    return LOGGER_STATUS::LGR_SUCCESS;
  } 
  else {
    DPRINTLN(F("Could not open file on SD card"));
    reset();
    return LOGGER_STATUS::LGR_ERROR_SD_CARD;
  }
}
