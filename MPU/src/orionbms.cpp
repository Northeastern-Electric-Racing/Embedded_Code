#include "orionbms.h"

OrionBMS::OrionBMS()
{
    boostRecharge_wait.cancelTimer();
    isInChargeMode.cancelTimer();
}


OrionBMS::~OrionBMS(){}


void OrionBMS::setSoC(uint8_t p_SoC)
{
    SoC = p_SoC;
}


void OrionBMS::setAvgTemp(uint8_t p_avgTemp)
{
    avgTemp = p_avgTemp;
}


void OrionBMS::setFailsafeCode(uint8_t p_failsafeCode)
{
    failsafeCode = p_failsafeCode;
}


uint8_t OrionBMS::getSoC()
{
    return SoC;
}


bool OrionBMS::isSoCCritical()
{
    return SoC < CRITICAL_SOC;
}


uint8_t OrionBMS::getAvgTemp()
{
    return avgTemp;
}


bool OrionBMS::isAvgTempCritical()
{
    return avgTemp > CRITICAL_CELLTEMP;
}


bool OrionBMS::isAvgTempShutdown()
{
    return avgTemp > SHUTDOWN_CELLTEMP;
}


void OrionBMS::setCurrentLimit(uint16_t p_currentLimit)
{
    currentLimit = p_currentLimit;
}

void OrionBMS::setChargeCurrentLimit(uint16_t p_chargeCurrentLimit)
{
    chargeCurrentLimit = p_chargeCurrentLimit;
}


uint16_t OrionBMS::getCurrentLimit()
{
    return currentLimit;
}


uint16_t OrionBMS::getChargeCurrentLimit()
{
    return chargeCurrentLimit;
}


void OrionBMS::setCurrentDraw(int16_t p_currentDraw)
{
    currentDraw = p_currentDraw;
}


bool OrionBMS::isCurrentPastLimit()
{
    return currentDraw > currentLimit;
}


bool OrionBMS::isCharging()
{
    return currentDraw < 0;
}


void OrionBMS::setLiveVoltage(int16_t p_voltage)
{
    liveVoltage = p_voltage;
}


int16_t OrionBMS::getLiveVoltage()
{
    return liveVoltage;
}


void OrionBMS::enableChargingMode()
{
    isInChargeMode.startTimer(2000);
}


bool OrionBMS::getChargeMode()
{
    if(isInChargeMode.isTimerExpired())
    {
        airOpen = 1;
        sendMessageCAN1(CANMSG_MC_SETPARAMETER, 8, OPEN_AIR_MSG);
    }
    return !isInChargeMode.isTimerExpired();
}


void OrionBMS::toggleAIR()
{
    airOpen = !airOpen;
    sendMessageCAN1(CANMSG_MC_SETPARAMETER, 8, airOpen ? OPEN_AIR_MSG : CLOSE_AIR_MSG);
}


bool OrionBMS::isAIROpen()
{
    return airOpen;
}