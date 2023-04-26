/**
 * @file message.h
 * @brief Defines standard message formats.
 *
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <rtc.h>
#include <stdint.h>

/* Message structure used throughout the system. */
typedef struct {
  rtc_time_t timestamp;
  uint16_t id;
  uint8_t length;
  uint8_t dataBuf[8];
} message_t;

#endif