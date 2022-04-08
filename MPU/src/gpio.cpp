#include "gpio.h"

GPIO::GPIO(){}

GPIO::GPIO(CASCADIAMC *p_motorController, ORIONBMS *p_bms)
{
    pinMode(SS_READY_SEN, INPUT);

    motorController = p_motorController;
    bms = p_bms;
}

GPIO::~GPIO(){}

void GPIO::handleMCHVFault()
{
    if(!digitalRead(SS_READY_SEN))
    {
        Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
        isSSPowerCycle = true;
        return;
    }
    if (isSSPowerCycle && digitalRead(SS_READY_SEN) == HIGH)
    {
        Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        motorController->clearFault();
        isSSPowerCycle = false;
    }
}