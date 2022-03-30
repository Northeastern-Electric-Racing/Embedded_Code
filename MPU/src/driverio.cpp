#include "driverio.h"
#include <nerduino.h>

DRIVERIO::DRIVERIO(){}


DRIVERIO::DRIVERIO(CASCADIAMC *p_motorController)
{
    pinMode(REVERSE_SW_PIN, INPUT_PULLUP);
    
    pinMode(SS_BUTT_PIN, INPUT_PULLUP);
    pinMode(SS_LED_PIN, OUTPUT);
    digitalWrite(SS_LED_PIN, HIGH);

    pinMode(LED4_PIN, OUTPUT);
    digitalWrite(LED4_PIN, LOW);

    pinMode(LED5_PIN, OUTPUT);
    digitalWrite(LED5_PIN, LOW);

    pinMode(SPEAKER_PIN, OUTPUT);
    digitalWrite(SPEAKER_PIN, LOW);

    powerToggle_wait.cancelTimer();
    ssButton_debounce.cancelTimer();

    motorController = p_motorController;
}


DRIVERIO::~DRIVERIO(){}


void DRIVERIO::handleSSButton()
{
    if(digitalRead(SS_BUTT_PIN) && powerToggle_wait.isTimerExpired())
    {
        ssButton_debounce.startTimer(50);
        
        //if the button is still being held during and after the timer runs out, then toggle power
        while(!ssButton_debounce.isTimerExpired())
        {
            if(!digitalRead(SS_BUTT_PIN))
            {
                break;
            }
        }
        if(digitalRead(SS_BUTT_PIN))
        {
            isOn = !isOn;
            digitalWrite(SS_LED_PIN, isOn);     //Writes SS LED to the power state of the motor

            motorController->toggleOn(isOn);    //Writes the power state of the motor to the MC message to be sent

            powerToggle_wait.startTimer(1000);
            Serial.println("Start/Stop Button Pressed!");
        }
    }
}


void DRIVERIO::handleReverseSwitch()
{
    if((bool)digitalRead(REVERSE_SW_PIN) != isForward)
    {
        isForward = (bool)digitalRead(REVERSE_SW_PIN);

        motorController->toggleDirection(isForward);    //writes the direction of the motor to the MC message to be sent

        Serial.println(isForward ? "Forward" : "Reverse");
    }
}


void DRIVERIO::writeLED4(bool state)
{
    digitalWrite(LED4_PIN, state);
}


void DRIVERIO::writeLED5(bool state)
{
    digitalWrite(LED5_PIN, state);
}


void DRIVERIO::writeSpeaker(bool state)
{
    digitalWrite(SPEAKER_PIN, state);
}