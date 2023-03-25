#include "gpioHW.h"

RadiatorFan::RadiatorFan(){}

RadiatorFan::RadiatorFan(uint8_t pinNum)
{
    pin = pinNum;
    pinMode(pin, OUTPUT);
    enableFan(false);
}

RadiatorFan::~RadiatorFan(){}

void RadiatorFan::enableFan(bool status)
{
    isEnabled = status;
    digitalWrite(pin, isEnabled);
}


CoolingPump::CoolingPump(){}

CoolingPump::CoolingPump(uint8_t pinNum)
{
    pin = pinNum;
    pinMode(pin, OUTPUT);
    enablePump(false);
}

CoolingPump::~CoolingPump(){}

void CoolingPump::enablePump(bool status)
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

bool TSMS::isReady()
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

