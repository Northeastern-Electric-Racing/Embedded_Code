#include "gpioHW.h"

RADIATORFAN::RADIATORFAN(){}

RADIATORFAN::RADIATORFAN(uint8_t pinNum)
{
    pin = pinNum;
    pinMode(pin, OUTPUT);
    enableFan(false);
}

RADIATORFAN::~RADIATORFAN(){}

void RADIATORFAN::enableFan(bool status)
{
    isEnabled = status;
    digitalWrite(pin, isEnabled);
}


COOLINGPUMP::COOLINGPUMP(){}

COOLINGPUMP::COOLINGPUMP(uint8_t pinNum)
{
    pin = pinNum;
    pinMode(pin, OUTPUT);
    enablePump(false);
}

COOLINGPUMP::~COOLINGPUMP(){}

void COOLINGPUMP::enablePump(bool status)
{
    isEnabled = status;
    digitalWrite(pin, isEnabled);
}


TSMS::TSMS(){}

TSMS::TSMS(uint8_t pinNum)
{
    pin = pinNum;
    pinMode(pin, INPUT);
}

TSMS::~TSMS(){}

bool TSMS::getStatus()
{
    bool reading = digitalRead(pin);

    //If the previous reading was HIGH and the current reading is low, we cycled power
    if(!reading && prevReading)
    {
        isTSCycled = true;
    }

    prevReading = reading;
    return reading;
}

bool TSMS::isPowerCycled()
{
    bool returnStatus = isTSCycled;
    if(isTSCycled) isTSCycled = false;
    return returnStatus;
}

