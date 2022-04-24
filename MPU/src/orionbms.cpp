#include "orionbms.h"

ORIONBMS::ORIONBMS(){}


ORIONBMS::~ORIONBMS(){}


void ORIONBMS::setSoC(uint8_t p_SoC)
{
    SoC = p_SoC;
}


void ORIONBMS::setAvgTemp(uint8_t p_avgTemp)
{
    avgTemp = p_avgTemp;
}


void ORIONBMS::setFailsafeCode(uint8_t p_failsafeCode)
{
    failsafeCode = p_failsafeCode;
}


uint8_t ORIONBMS::getSoC()
{
    return SoC;
}


bool ORIONBMS::isSoCCritical()
{
    return (SoC / 200) < CRITICAL_SOC;
}


uint8_t ORIONBMS::getAvgTemp()
{
    return avgTemp;
}


bool ORIONBMS::isAvgTempCritical()
{
    return avgTemp > CRITICAL_CELLTEMP;
}


bool ORIONBMS::isAvgTempShutdown()
{
    return avgTemp > SHUTDOWN_CELLTEMP;
}


bool ORIONBMS::getIsCharging()
{
    return isCharging;
}


void ORIONBMS::setBoosting()
{
    boosting_time.startTimer(BOOSTING_TIME_LIMIT);
    boosting_warningTime.startTimer(BOOSTING_TIME_LIMIT - BOOSTING_EXIT_TIME);
}


bool ORIONBMS::isBoosting()
{
    return !boosting_time.isTimerExpired();
}


bool ORIONBMS::isLeavingBoosting()
{
    return boosting_warningTime.isTimerExpired() && !boosting_time.isTimerExpired();
}


void ORIONBMS::setCurrentLimit(int16_t p_currentLimit)
{
    currentLimit = p_currentLimit;
}


void ORIONBMS::setCurrentDraw(int16_t p_currentDraw)
{
    currentDraw = p_currentDraw;
}


bool ORIONBMS::isCurrentPastLimit()
{
    return currentDraw > currentLimit;
}


bool ORIONBMS::isCharging()
{
    return currentDraw < 0;
}