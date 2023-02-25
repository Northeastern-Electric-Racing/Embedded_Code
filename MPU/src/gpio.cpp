#include "gpio.h"

GPIO::GPIO(){}


GPIO::GPIO(CascadiaMC *p_motorController, OrionBMS *p_bms)
{
    radiatorFan = RadiatorFan(RADIATORFAN_PIN);
    coolingPump = CoolingPump(PUMP_PIN);
    tsms = TSMS(SS_READY_SEN);

    motorController = p_motorController;
    bms = p_bms;
}


GPIO::~GPIO(){}


bool GPIO::handleTSMS()
{
    // Serial.println(tsms.isReady());
    if(!tsms.isReady())
    {
        motorController->raiseFault();
        return false;
    }
    if(tsms.isPowerCycled() && tsms.isReady())
    {
        motorController->clearFault();
        Serial.println("Clearing!");
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