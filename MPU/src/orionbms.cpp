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

void ORIONBMS::setIsCharging(bool p_isCharging)
{
    isCharging = p_isCharging;
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

bool ORIONBMS::getIsCharging()
{
    return isCharging;
}