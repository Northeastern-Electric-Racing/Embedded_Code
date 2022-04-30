#include "timer.h"

Timer::Timer(){}

Timer::~Timer(){}

void Timer::startTimer(uint32_t timeout)
{
    running = true;
    expireTime = millis() + timeout;
    isReset = 0;
}

void Timer::cancelTimer()
{
    running = false;
    expireTime = millis();
    isReset = 1;
}

bool Timer::isTimerExpired()
{
    if(!running)
    {
        return true;
    }

    return expireTime > millis() ? false : true;
}

bool Timer::isTimerReset()
{
    return isReset;
}