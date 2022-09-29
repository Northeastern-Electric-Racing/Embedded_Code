#include <Arduino.h>
#include <TimeLib.h>

#include "rtc.h"
#include "debug.h"


/* Variables for keeping track of startup time */
static uint32_t startUpTimeMillis;
static uint32_t startUpTimeRTC;


/**
 * @brief Wrapper function to pass to time sync function
 * 
 * @return time_t 
 */
static time_t getTeensy3Time()
{
    return Teensy3Clock.get();
}


RTC_STATUS RtcInit() {
    setSyncProvider(getTeensy3Time);
    if (timeStatus() == timeSet) {
        DPRINTLN(F("RTC has set the system time")); 
        startUpTimeRTC = now();
    } else {
        DPRINTLN(F("Unable to sync with the RTC"));
        startUpTimeRTC = 0;
    }
    startUpTimeMillis = millis();
    return RTC_STATUS::RTC_SUCCESS;
}


RTC_STATUS RtcGetTime(uint64_t *currentTime) {
    uint32_t millisSinceStart = millis() - startUpTimeMillis;
    *currentTime = (startUpTimeRTC * 1000) + millisSinceStart;

    if (startUpTimeRTC == 0) {
        return RTC_STATUS::RTC_NOT_STARTED;
    }
    return RTC_STATUS::RTC_SUCCESS;
}


RTC_STATUS RtcGetFormatedTime(char *currentTime) {
    uint32_t millisSinceStart = millis() - startUpTimeMillis;
    uint32_t currentTimeSec = startUpTimeRTC + (millisSinceStart / 1000);
    uint32_t extraMillis = millisSinceStart % 1000;
    
    snprintf(currentTime, 25, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.3luZ\0", year(currentTimeSec), month(currentTimeSec), 
        day(currentTimeSec), hour(currentTimeSec), minute(currentTimeSec), second(currentTimeSec), extraMillis);
        
    if (startUpTimeRTC == 0) {
        return RTC_STATUS::RTC_NOT_STARTED;
    }
    return RTC_STATUS::RTC_SUCCESS;
}



