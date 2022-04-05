#include "gpio.h"

GPIO::GPIO(){}

GPIO::GPIO(CASCADIAMC *p_motorController)
{
    pinMode(SS_READY_SEN, INPUT);

    motorController = p_motorController;
}

GPIO::~GPIO(){}

void GPIO::handleMCHVFault()
{
    if(!digitalRead(SS_READY_SEN))
    {
        isSSPowerCycle = true;
        return;
    }
    if (isSSPowerCycle && digitalRead(SS_READY_SEN) == HIGH)
    {
        motorController->clearFault();
        isSSPowerCycle = false;
    }
}