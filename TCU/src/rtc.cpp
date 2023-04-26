#include <Arduino.h>
#include <TimeLib.h>

#include "debug.h"
#include "rtc.h"

/* Keeps track of startup time */
static rtc_time_t startUpTime;

/**
 * @brief Wrapper function to pass to time sync function
 *
 * @return time_t
 */
static time_t getTeensy3Time() { return Teensy3Clock.get(); }

RTC_STATUS RtcInit() {
  setSyncProvider(getTeensy3Time);
  if (timeStatus() == timeSet) {
    DPRINTLN(F("RTC has set the system time"));
    startUpTime.seconds = now();
  } else {
    DPRINTLN(F("Unable to sync with the RTC"));
    startUpTime.seconds = 0;
  }
  startUpTime.millis = millis();
  return RTC_STATUS::RTC_SUCCESS;
}

RTC_STATUS RtcGetTime(rtc_time_t *currentTime) {
  uint32_t millisSinceStart = millis() - startUpTime.millis;
  currentTime->seconds = startUpTime.seconds + (millisSinceStart / 1000);
  currentTime->millis = millisSinceStart % 1000;

  if (startUpTime.seconds == 0) {
    return RTC_STATUS::RTC_NOT_STARTED;
  }
  return RTC_STATUS::RTC_SUCCESS;
}

RTC_STATUS RtcGetFormatedTime(char *currentTime) {
  uint32_t millisSinceStart = millis() - startUpTime.millis;
  uint32_t currentTimeSec = startUpTime.seconds + (millisSinceStart / 1000);
  uint32_t extraMillis = millisSinceStart % 1000;

  snprintf(currentTime, 25, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.3luZ\0",
           year(currentTimeSec), month(currentTimeSec), day(currentTimeSec),
           hour(currentTimeSec), minute(currentTimeSec), second(currentTimeSec),
           extraMillis);

  if (startUpTime.seconds == 0) {
    return RTC_STATUS::RTC_NOT_STARTED;
  }
  return RTC_STATUS::RTC_SUCCESS;
}
