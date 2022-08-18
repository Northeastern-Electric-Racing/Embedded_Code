/**
 * @file xbee_at.h
 * @author Peter Rydzynski
 * @brief Utilities to send and receive data with the Xbee wireless modules
 * @date 2022-05-16
 * 
 * @details Xbees have two device modes, API and AT. This file defines AT mode, which
 * acts as a raw serial bridge between the two communicating modules. API mode, on
 * the other hand, defines packet structures for more functionality (and more complexity).
 * See the teensy 4.1 docs for an impelementation of API mode.
 *
 */

#include <Arduino.h>

/* Error codes for the function return values */
#define XBEE_SUCCESS              0
#define XBEE_ERROR_RX_EMPTY       1
#define XBEE_ERROR_TX_FULL        2
#define XBEE_ERROR_MAX_CALLBACKS  3
#define XBEE_ERROR_RECEIVE_MODE   4
#define XBEE_ERROR_RECEIVE_FORMAT 5
#define XBEE_ERROR_CONNECTION     6

/* Maximum length of a received message over the Xbee connection */
#define MAX_READ_LENGTH 100

/* Format for callback functions on data reception */
typedef void (*XbeeCallback) (uint8_t *buf, int length);

/**
 * @brief Initializes the Xbee connection
 * 
 * @param serialPort Serial port
 * @param baudRate Baud rate
 */
void XbeeInit(HardwareSerial *serialPort, uint32_t baudRate);

/**
 * @brief Registers a callback to be notified when data is received.
 * 
 * @param callback Function to register
 * @return int status code (0 on success, other on failure)
 */
int XbeeRegisterCallback(XbeeCallback callback);

/**
 * @brief Sends data over the Xbee connection
 * 
 * @param buf Data buffer to send
 * @param len Length of the data to send
 * @return Actual amount of data sent
 */
int XbeeSendData(uint8_t *buf, uint32_t len);

/**
 * @brief Receives data from the xbee connection. 
 *     NOTE: Only usable if we have not registered a callback.
 * 
 * @param buf Data buffer to store recevied data
 * @param maxLen Max length of data to store in the buffer
 * @return Actual amount of data received
 */
int XbeeReceiveData(uint8_t *buf, uint32_t maxLen);

