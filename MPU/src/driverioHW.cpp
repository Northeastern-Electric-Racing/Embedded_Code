#include <nerduino.h>
#include "driverioHW.h"

/**************************************************************************************/
/**
 * @brief Button Class Implementation
 */
/**************************************************************************************/

Button::Button(uint8_t pinNumber)
{
    pin = pinNumber;
    pinMode(pin, INPUT_PULLUP);
    debounce.cancelTimer();
}

Button::~Button(){}

void Button::checkButtonPin()
{
    if(digitalRead(pin) == HIGH && state == NOT_PRESSED)
    {
        state = DEBOUNCING;
        debounce.startTimer(BUTTON_DEBOUNCE_TIME);
    }
    else if(digitalRead(pin) == LOW)
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
    if(digitalRead(pin) && debounce.isTimerExpired() && !debounce.isTimerReset())
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
    if(digitalRead(pin) && debounce.isTimerExpired() && !debounce.isTimerReset())
    {
        state = PRESSED;
        return true;
    }

    return false;
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
    if(!isBlinkEnabled) digitalWrite(pin,state);
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

Switch::Switch(uint8_t pinNumber)
{
    pin = pinNumber;
    pinMode(pin,INPUT_PULLUP);
    previousReading = getSwitchState();
}

Switch::~Switch(){}

bool Switch::getSwitchState()
{
    bool switchReading = digitalRead(pin);
    return switchReading;
}

bool Switch::hasSwitchToggled()
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

StartButton::StartButton(uint8_t ledPinNumber, uint8_t buttonPinNumber)
    : Button(buttonPinNumber), LED(ledPinNumber) {}

StartButton::~StartButton(){}

/**************************************************************************************/
/**
 * @brief Dashboard Class Implementation
 */
/**************************************************************************************/

Dashboard::Dashboard()
{}

Dashboard::~Dashboard(){}