/**
 * @file xbee_at.h
 * @brief Utilities to send and receive data with the XBee wireless modules
 * 
 * @details XBees have two device modes, API and AT. This file defines AT mode, which
 * acts as a raw serial bridge between the two communicating modules. API mode, on
 * the other hand, defines packet structures for more functionality (and more complexity).
 * See the teensy 4.1 docs for an impelementation of API mode.
 *
 */

#ifndef XBEE_AT_H
#define XBEE_AT_H

#include <Arduino.h>
#include "message.h"


/* Maximum length of a received message over the XBee connection */
#define MAX_READ_LENGTH 100

/* Return status codes for XBee functions. */
typedef enum {
  XB_SUCCESS              = 0,
  XB_ERROR_RX_EMPTY       = 1,
  XB_ERROR_TX_FULL        = 2,
  XB_ERROR_MAX_CALLBACKS  = 3,
  XB_ERROR_RECEIVE_MODE   = 4,
  XB_ERROR_RECEIVE_FORMAT = 5,
  XB_ERROR_CONNECTION     = 6
} XBEE_STATUS;


/* Format for callback functions on data reception. */
typedef void (*XBeeCallback) (uint8_t *buf, int length);

/**
 * @brief Initializes the XBee connection
 * 
 * @param serialPort Serial port
 * @param baudRate Baud rate
 * @return XBEE_STATUS
 */
XBEE_STATUS XBeeInit(HardwareSerial *serialPort, uint32_t baudRate);

/**
 * @brief Registers a callback to be notified when data is received.
 * 
 * @param callback Function to register
 * @return XBEE_STATUS
 */
XBEE_STATUS XBeeRegisterCallback(XBeeCallback callback);

/**
 * @brief Sends data over the XBee connection
 * 
 * @param message pointer to the message
 * @return XBEE_STATUS
 */
XBEE_STATUS XBeeSendMessage(message_t *message);

/**
 * @brief Receives data from the XBee connection. 
 *     NOTE: Only usable if we have not registered a callback.
 * 
 * @param buf Data buffer to store recevied data
 * @param maxLen Max length of data to store in the buffer
 * @param receivedLength return variable for amount of data received
 * @return XBEE_STATUS
 */
XBEE_STATUS XBeeReceiveMessage(uint8_t *buf, uint32_t maxLen, uint32_t *receivedLength);

#endif