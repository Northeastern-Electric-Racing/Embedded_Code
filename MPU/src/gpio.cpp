#include "gpio.h"

GPIO::GPIO(){}


GPIO::GPIO(CASCADIAMC *p_motorController, ORIONBMS *p_bms)
{
    pinMode(SS_READY_SEN, INPUT);
    pinMode(PUMP_PIN, OUTPUT);

    motorController = p_motorController;
    bms = p_bms;
}


GPIO::~GPIO(){}


void GPIO::handleMCHVFault()
{
    if(!digitalRead(SS_READY_SEN))
    {
        //Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
        isSSPowerCycle = true;
        motorController->raiseFault();
        return;
    }
    if (isSSPowerCycle && digitalRead(SS_READY_SEN) == HIGH)
    {
        //Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        motorController->clearFault();
        isSSPowerCycle = false;
    }
}

void GPIO::handlePump()
{
    digitalWrite(PUMP_PIN, motorController->getIsOn());
}

void GPIO::handleRadiatorFan()
{
    uint8_t fanSpeed = 0;
    uint16_t temp = motorController->getRadiatorTemp();

    //Convert temp to pwm speed

    digitalWrite(RADIATORFAN_PIN, fanSpeed);
}