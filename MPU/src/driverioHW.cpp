#include <nerduino.h>
#include "driverioHW.h"

/**************************************************************************************/
/**
 * @brief Button Class Implementation
 */
/**************************************************************************************/

BUTTON::BUTTON(uint8_t pinNumber)
{
    pin = pinNumber;
    pinMode(pin, INPUT_PULLUP);
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

/**************************************************************************************/
/**
 * @brief Speaker Class Implementation
 */
/**************************************************************************************/

SPEAKER::SPEAKER(uint8_t pinNumber)
{
    pin = pinNumber;
    pinMode(pin, OUTPUT);
    writeSpeaker(LOW);
    waitTime.cancelTimer();
}

SPEAKER::~SPEAKER(){}

void SPEAKER::writeSpeaker(bool state)
{
    //We need to invert the logic of the speaker due to the hardware
    digitalWrite(pin,!state);
}

void SPEAKER::playSpeaker()
{
    writeSpeaker(HIGH);
    waitTime.startTimer(SPEAKER_DURATION);
}

void SPEAKER::attemptToStopSpeaker()
{
    if(waitTime.isTimerExpired())
    {
        writeSpeaker(LOW);
    }
}

/**************************************************************************************/
/**
 * @brief LED Class Implementation
 */
/**************************************************************************************/

LED::LED(uint8_t pinNumber)
{
    pin = pinNumber;
    pinMode(pin, OUTPUT);
    writeLED(LOW);
}

LED::~LED(){}

void LED::writeLED(bool state)
{
    digitalWrite(pin,state);
}

void LED::blinkEnable(bool state)
{
    isBlinkEnabled = state;
    if(isBlinkEnabled) blinkTimer.startTimer(LED_BLINK_TIME);
}

void LED::updateBlink()
{
    if(!isBlinkEnabled) return;

    if(blinkTimer.isTimerExpired())
    {
        blinkState = !blinkState;
        writeLED(blinkState);
    }
}

/**************************************************************************************/
/**
 * @brief Switch Class Implementation
 */
/**************************************************************************************/

SWITCH::SWITCH(uint8_t pinNumber)
{
    pin = pinNumber;
    pinMode(pin,INPUT_PULLUP);
    previousReading = getSwitchState();
}

SWITCH::~SWITCH(){}

bool SWITCH::getSwitchState()
{
    bool switchReading = digitalRead(pin);
    previousReading = switchReading;
    return switchReading;
}

bool SWITCH::hasSwitchToggled()
{
    if(getSwitchState() == previousReading) return false;

    previousReading = getSwitchState();
    return true;
}

/**************************************************************************************/
/**
 * @brief Start Button Class Implementation
 */
/**************************************************************************************/

STARTBUTTON::STARTBUTTON(uint8_t ledPinNumber, uint8_t buttonPinNumber)
    : BUTTON(buttonPinNumber), LED(ledPinNumber) {}

STARTBUTTON::~STARTBUTTON(){}

/**************************************************************************************/
/**
 * @brief Dashboard Class Implementation
 */
/**************************************************************************************/

DASHBOARD::DASHBOARD()
{}

DASHBOARD::~DASHBOARD(){}