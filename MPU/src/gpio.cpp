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


bool GPIO::handleMCHVFault()
{
    if(!digitalRead(SS_READY_SEN))
    {
        //Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
        isSSPowerCycle = true;
        motorController->raiseFault();
        return false;
    }
    if (isSSPowerCycle && digitalRead(SS_READY_SEN) == HIGH)
    {
        //Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        motorController->clearFault();
        isSSPowerCycle = false;
        return true;
    }
    return false;
}

void GPIO::handlePump()
{
    digitalWrite(PUMP_PIN, motorController->getIsOn());
}

void GPIO::handleRadiatorFan()
{
    int16_t temp = motorController->getRadiatorTemp() / 10;

    Serial.print("MC TEMP: ");
    Serial.println(temp);

    if (temp > MAX_FANSPEED_TEMP) {
        digitalWrite(RADIATORFAN_PIN, LOW);
    } else {
        digitalWrite(RADIATORFAN_PIN, HIGH);
    }
}