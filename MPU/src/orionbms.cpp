#include "orionbms.h"

ORIONBMS::ORIONBMS()
{
    boosting_time.cancelTimer();
    boosting_warningTime.cancelTimer();
    boostRecharge_wait.cancelTimer();
    isInChargeMode.cancelTimer();
}


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
    return SoC < CRITICAL_SOC;
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

void ORIONBMS::setBoosting()
{
    if(!isBoostReady()){return;}
    boosting_time.startTimer(BOOSTING_TIME_LIMIT);
    boosting_warningTime.startTimer(BOOSTING_TIME_LIMIT - BOOSTING_EXIT_TIME);
    boostRecharge_wait.startTimer(BOOSTING_TIME_LIMIT + BOOSTING_RECHARGE_TIME);
}


bool ORIONBMS::isBoosting()
{
    return !boosting_time.isTimerExpired();
}


bool ORIONBMS::isLeavingBoosting()
{
    return boosting_warningTime.isTimerExpired() && !boosting_time.isTimerExpired();
}


bool ORIONBMS::isBoostReady()
{
    return boostRecharge_wait.isTimerExpired();
}


void ORIONBMS::setCurrentLimit(uint16_t p_currentLimit)
{
    currentLimit = p_currentLimit;
}

void ORIONBMS::setChargeCurrentLimit(uint16_t p_chargeCurrentLimit)
{
    chargeCurrentLimit = p_chargeCurrentLimit;
}


uint16_t ORIONBMS::getCurrentLimit()
{
    return currentLimit;
}


uint16_t ORIONBMS::getChargeCurrentLimit()
{
    return chargeCurrentLimit;
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


void ORIONBMS::setLiveVoltage(int16_t p_voltage)
{
    liveVoltage = p_voltage;
}


int16_t ORIONBMS::getLiveVoltage()
{
    return liveVoltage;
}


void ORIONBMS::enableChargingMode()
{
    isInChargeMode.startTimer(2000);
}


bool ORIONBMS::getChargeMode()
{
    if(isInChargeMode.isTimerExpired())
    {
        airOpen = 1;
        sendMessage(CANMSG_MC_SETPARAMETER, 8, OPEN_AIR_MSG);
    }
    return !isInChargeMode.isTimerExpired();
}


void ORIONBMS::toggleAIR()
{
    airOpen = !airOpen;
    sendMessage(CANMSG_MC_SETPARAMETER, 8, airOpen ? OPEN_AIR_MSG : CLOSE_AIR_MSG);
}


bool ORIONBMS::isAIROpen()
{
    return airOpen;
}