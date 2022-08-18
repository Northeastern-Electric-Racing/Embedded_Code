#include "timer.h"

Timer::Timer(){}

Timer::~Timer(){}

void Timer::startTimer(uint32_t timeout)
{
    running = true;
    expireTime = millis() + timeout;
}

void Timer::cancelTimer()
{
    running = false;
    expireTime = millis();
}

bool Timer::isTimerExpired()
{
    if(!running)
    {
        return true;
    }

    return expireTime > millis() ? false : true;
}