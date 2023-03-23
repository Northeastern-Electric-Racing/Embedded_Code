/**
 * @file rtc.h
 * @brief Defines timing utility functions for the Nerduino real time clock (RTC)
 * 
 */

#ifndef RTC_H
#define RTC_H

#include <stdint.h>

/* Return status codes for RTC functions. */
typedef enum {
  RTC_SUCCESS = 0,
  RTC_NOT_STARTED = 1
} RTC_STATUS;


/* Struct to wrap together timing data */
typedef struct {
  uint32_t seconds;
  uint32_t millis;
} rtc_time_t;

/**
 * @brief Initializes the RTC functionality.
 * 
 * @return RTC_STATUS
 */
RTC_STATUS RtcInit();

/**
 * @brief Gets the current RTC time in millisseconds since Jan 1, 1970
 * 
 * @param currentTime Return variable for current time
 * @return RTC_STATUS 
 */
RTC_STATUS RtcGetTime(rtc_time_t *currentTime);

/**
 * @brief Gets the current RTC time as a string in the format YYYY-MM-DDT00:00:00.000Z
 * 
 * @param currentTime Return variable for current time
 *   @note Param must have space for 25 characters
 * @return RTC_STATUS 
 */
RTC_STATUS RtcGetFormatedTime(char *currentTime);

#endif