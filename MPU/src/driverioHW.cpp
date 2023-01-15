#include <nerduino.h>
#include "driverioHW.h"

/**************************************************************************************/
/**
 * @brief Button Class Implementation
 */
/**************************************************************************************/

Button::Button()
{
    debounce.cancelTimer();
}

Button::~Button(){}

void Button::checkButtonPin()
{
    if(isPressed == true && state == NOT_PRESSED)
    {
        state = DEBOUNCING;
        debounce.startTimer(BUTTON_DEBOUNCE_TIME);
    }
    else if(isPressed == false)
    {
        debounce.cancelTimer();
        state = NOT_PRESSED;
    }
}

bool Button::isButtonPressed()
{
    if(state == PRESSED) return true;

    /**
     * Returns true if
     * 1. The button has successfully passed the debounce period
     * 2. The debounce timer has not been canceled/reset
     * 3. The button is still being held
     */
    if(isPressed && debounce.isTimerExpired() && !debounce.isTimerReset())
    {
        state = PRESSED;
        return true;
    }
    return false;
}

bool Button::isButtonPressed_Pulse()
{
    if(state == PRESSED) return false;

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
    if(isPressed && debounce.isTimerExpired() && !debounce.isTimerReset())
    {
        state = PRESSED;
        return true;
    }

    return false;
}

void Button::setButtonState(bool buttonState)
{
    isPressed = buttonState;
}

/**************************************************************************************/
/**
 * @brief Speaker Class Implementation
 */
/**************************************************************************************/

Speaker::Speaker(uint8_t pinNumber)
{
    pin = pinNumber;
    pinMode(pin, OUTPUT);
    writeSpeaker(LOW);
    waitTime.cancelTimer();
}

Speaker::~Speaker(){}

void Speaker::writeSpeaker(bool state)
{
    //We need to invert the logic of the speaker due to the hardware
    digitalWrite(pin,!state);
}

void Speaker::playSpeaker()
{
    writeSpeaker(HIGH);
    waitTime.startTimer(SPEAKER_DURATION);
}

void Speaker::attemptToStopSpeaker()
{
    if(waitTime.isTimerExpired())
    {
        writeSpeaker(LOW);
    }
}

/**************************************************************************************/
/**
 * @brief Dashboard Class Implementation
 */
/**************************************************************************************/

Dashboard::Dashboard()
{}

Dashboard::~Dashboard(){}