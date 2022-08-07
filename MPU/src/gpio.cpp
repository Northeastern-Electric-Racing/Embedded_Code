#include "gpio.h"

GPIO::GPIO(){}


GPIO::GPIO(CASCADIAMC *p_motorController, ORIONBMS *p_bms)
{
    radiatorFan = RADIATORFAN(RADIATORFAN_PIN);
    coolingPump = COOLINGPUMP(PUMP_PIN);
    tsms = TSMS(SS_READY_SEN);

    motorController = p_motorController;
    bms = p_bms;
}


GPIO::~GPIO(){}


bool GPIO::handleMCHVFault()
{
    if(!tsms.isReady())
    {
        motorController->raiseFault();
        return false;
    }
    if(tsms.isPowerCycled() && tsms.isReady())
    {
        motorController->clearFault();
        return true;
    }
    return false;
}

void GPIO::handlePump()
{
    coolingPump.enablePump(motorController->getIsOn());
}

void GPIO::handleRadiatorFan()
{
    int16_t temp = motorController->getRadiatorTemp() / 10;

    radiatorFan.enableFan(temp > MAX_FANSPEED_TEMP);
}