#include <nerduino.h>
#include "driverioHW.h"


BUTTON::BUTTON(uint8_t pinNumber)
{
    pin = pinNumber;
    debounce.cancelTimer();
}


BUTTON::~BUTTON(){}


void BUTTON::checkButtonPin(uint8_t debounceTime)
{
    if(digitalRead(pin) == HIGH && state == NOT_PRESSED)
    {
        state = DEBOUNCING;
        debounce.startTimer(debounceTime);
    }
    else if(digitalRead(pin) == LOW)
    {
        debounce.cancelTimer();
        state = NOT_PRESSED;
    }
}


bool BUTTON::isButtonPressed()
{
    if(state == PRESSED) return true;

    /**
     * Returns true if
     * 1. The button has successfully passed the debounce period
     * 2. The debounce timer has not been canceled/reset
     * 3. The button is still being held
     */
    if(digitalRead(pin) && debounce.isTimerExpired() && !debounce.isTimerReset())
    {
        state = PRESSED;
        return true;
    }
    return false;
}


bool BUTTON::isButtonPressed_Pulse()
{
    if(!readyToReadPulse) return false;

    /**
     * Returns true if
     * 1. The button has successfully passed the debounce period
     * 2. The debounce timer has not been canceled/reset
     * 3. The button is still being held
     * 
     * AND
     * 
     * 4. The state of the button has not been read since the button has passed the debounce
     */
    if(digitalRead(pin) && debounce.isTimerExpired() && !debounce.isTimerReset())
    {
        state = PRESSED;
        readyToReadPulse = false;
        return true; 
    }

    return false;
}

