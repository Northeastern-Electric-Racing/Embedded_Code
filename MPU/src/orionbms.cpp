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

bool ORIONBMS::isSoCCritical()
{
    return (SoC / 255) < CRITICAL_SOC;
}

bool ORIONBMS::isAvgTempCritical()
{
    return avgTemp > CRITICAL_CELLTEMP;
}