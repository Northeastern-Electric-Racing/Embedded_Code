/**
 * @file logger.h
 * @brief Defines logging utility functions for the Nerduino SD card
 * 
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include "message.h"

/* Return status codes for Logger functions. */
typedef enum {
  LGR_SUCCESS           = 0,
  LGR_ERROR_BUFFER_FULL = 1,
  LGR_ERROR_SD_CARD     = 2,
  LGR_ERROR_NO_WRITE    = 3
} LOGGER_STATUS;


/**
 * @brief Initializes the SD logging functionality.
 * 
 * @param logFrequency Minimum rate at which to log the buffered messages
 * @return LOGGER_STATUS
 */
LOGGER_STATUS LoggerInit(uint32_t logFrequency);


/**
 * @brief Adds the given message to the log buffer.
 * 
 * @param message pointer to the message
 * @return LOGGER_STATUS
 */
LOGGER_STATUS LoggerBufferMessage(message_t *message);


/**
 * @brief Writes the messages currently buffered to the SD card. 
 * 
 * @return LOGGER_STATUS
 */
LOGGER_STATUS LoggerWrite();


/**
 * @brief Finds whether the logger is currently logging messages
 * 
 * @return true when active, false otherwise
 */
bool LoggerActive();

#endif