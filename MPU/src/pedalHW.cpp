#include "pedalHW.h"

PEDAL_HW::PEDAL_HW(){}

PEDAL_HW::PEDAL_HW(uint8_t p_errorPercent, uint8_t p_maxErrors, uint8_t pinNumbers[2])
{
    errorPercent = p_errorPercent;
    maxErrors = p_maxErrors;

    pin1 = pinNumbers[0];
    pin2 = pinNumbers[1];

	readingDebounce.cancelTimer();
}

uint8_t PEDAL_HW::readPressedPercentage()
{
    if(!readingDebounce.isTimerExpired())
    {
        minimizingDiffDebounce();
    }
    
    uint16_t finalReading = avgReading;
    uint8_t readingPercentage = finalReading/MAX_ADC_VALUE;

    avgReading = MAX_ADC_VALUE;
    readingDebounce.startTimer(PEDAL_DEBOUNCE_TIME);

    return readingPercentage;
}

void PEDAL_HW::minimizingDiffDebounce()
{
    uint16_t pin1Val = analogRead(pin1);
    uint16_t pin2Val = analogRead(pin2);

    // Iteratively narrow towards value
    if(readingDiff > abs(pin1Val-pin2Val))
    {
        readingDiff = abs(pin1Val-pin2Val);
        avgReading = (pin1Val + pin2Val) / 2;
    }

    checkForPedalError(pin1Val, pin2Val);
}

void PEDAL_HW::checkForPedalError(uint16_t val1, uint16_t val2)
{
    // Check for out of range values and errors (bottoming out of sensors/one faulty sensor)
    if (val1 == 0 ||
        val2 == 0 ||
        val1 == MAX_ADC_VALUE ||
        val2 == MAX_ADC_VALUE ||
        (abs(val1 - val2) > (MAX_ADC_VALUE * errorPercent)))
    {
        readingErrors++;
        if(readingErrors >= maxErrors)
        {
            readingFault = true;
        }
    }
}


BRAKELIGHT_HW::BRAKELIGHT_HW(){}

BRAKELIGHT_HW::BRAKELIGHT_HW(uint8_t pinNumber)
{
    brakeLightPin = pinNumber;
    pinMode(pinNumber, OUTPUT);
	digitalWrite(pinNumber, HIGH);
}

void BRAKELIGHT_HW::writeBrakeLight(bool state)
{
    if(brakeLightWait.isTimerExpired())
    {
        digitalWrite(brakeLightPin, state);
        if(state == HIGH)
        {
            brakeLightWait.startTimer(BRAKELIGHT_MIN_TIME_ON);
        }
    }
}