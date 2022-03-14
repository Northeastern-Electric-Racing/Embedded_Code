/**
 * File names are logged in the format "log-0.txt", where the 0 is an incrememnted number for
 * each operation session.
 * 
 * The standard block size for SD card transfers is 512 bytes. Each write operation to the SD card should
 * be as close to this size as possible, as the remaining space will be padded and used anyway.
 *  - buffer data up to 512 bytes before writing
 * 
 */

#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SD.h>

#define BAUD_RATE 250000U // 250 kbps 
#define MAX_MB_NUM 16 // maximum number of CAN mailboxes to use 

#define MAX_BUFFERED_MESSAGES 10 // max number of buffered CAN messages before logging to SD card

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; // main CAN object

File myFile;

// CAN Ids of the messages to log to SD card
const uint32_t LOG_IDS[] = {0x01, 0x02, 0x03, 0x04};
const int NUM_IDS = 4;

// Logging information
int bufLength = 0;
char* messageBuf[MAX_BUFFERED_MESSAGES]; 
uint32_t lastLogTime = 0;
int nextFileNum = 0; // log file names in the format log-0.txt


// function declarations
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf); 
void incomingCANCallback(const CAN_message_t &msg);
bool SDWrite(String messages);
void logMessage(const CAN_message_t &msg);



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

  while (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println(F("SD Init Failed!"));
    delay(250);
  }

  while (SD.exists("log-" + String(nextFileNum) + ".txt")) {
    Serial.print(F("log-"));
    Serial.print(String(nextFileNum));
    Serial.println(F(".txt exists..."));
    nextFileNum += 1;
  }

  Serial.print(F("setup complete. nextFileNum is "));
  Serial.println(String(nextFileNum));

  myFile = SD.open("log-" + String(nextFileNum) + ".txt", FILE_WRITE);

  if (myFile) {
    Serial.println("File could not be opened!");
    while(1){};
  }
}

/**
 * @brief Continuoulsy read incoming CAN messages and the values of the 
 *        accelerator potentiometers and brake switches
 * 
 */
void loop() {
  myCan.events();
}


/**
   Logs errors to SD card.
   @returns true if logging was successful, false if it failed
*/
bool SDWrite() {
  if (myFile) {
    for (int i = 0; i < bufLength; i++) {
      myFile.print(messageBuf[i]);
    }
    
    lastLogTime = millis();
    return true;
  } 
  
  return false;
}



/**
 * @brief Handles incoming CAN messages. This CAN node acts as the ECU, so incoming messages are
 *        use to set the values of the global state variables
 * 
 * @param msg received CAN message
 */
void incomingCANCallback(const CAN_message_t &msg)
{
  for (int i = 0; i < NUM_IDS; i++) {
    if (LOG_IDS[i] == msg.id) {
      String message;
      message += String(millis());
      message += ",";
      message += String(msg.id);
      message += ",";
      message += String(msg.len);
      message += ",";

      for (int i = 0; i < msg.len; i++) {
        message += String(msg.buf[i]);
        message += ",";
      }
      message += "\n";

      messageBuf[bufLength] = message;
      bufLength += 1;

      if (bufLength == MAX_BUFFERED_MESSAGES) {
        if (SDWrite()) { // if successfully
          Serial.println(F("Logging successful"));
        } else { 
          Serial.println(F("Error with logging"));
        }
        bufLength = 0;
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