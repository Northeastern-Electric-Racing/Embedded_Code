#include "driverio.h"
#include <nerduino.h>
// howdy feller

DRIVERIO::DRIVERIO(){}


DRIVERIO::DRIVERIO(CASCADIAMC *p_motorController, ORIONBMS *p_bms)
{
    pinMode(REVERSE_SW_PIN, INPUT_PULLUP);

    pinMode(SS_BUTT_PIN, INPUT_PULLUP);
    pinMode(SS_LED_PIN, OUTPUT);
    digitalWrite(SS_LED_PIN, LOW);

    pinMode(LED4_PIN, OUTPUT);
    digitalWrite(LED4_PIN, LOW);

    pinMode(LED5_PIN, OUTPUT);
    digitalWrite(LED5_PIN, LOW);

    pinMode(SPEAKER_PIN, OUTPUT);
    digitalWrite(SPEAKER_PIN, HIGH);
    

    powerToggle_wait.cancelTimer();
    ssButton_debounce.cancelTimer();
    speaker_wait.cancelTimer();

    motorController = p_motorController;
    bms = p_bms;
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
            if(isOn)
            {
                digitalWrite(SPEAKER_PIN, LOW);
                speaker_wait.startTimer(1500);
            }
            powerToggle_wait.startTimer(1000);
#ifdef DEBUG
            Serial.println("***********************Toggling Power**********************");
#endif
        }
    }
    if(speaker_wait.isTimerExpired())
    {
        digitalWrite(SPEAKER_PIN, HIGH);
    }
}


void DRIVERIO::handleReverseSwitch()
{
    Serial.print("Switch State:\t");
    Serial.println(isForward);
    if(digitalRead(REVERSE_SW_PIN) != isForward)
    {
        isForward = (bool)digitalRead(REVERSE_SW_PIN);

        motorController->toggleDirection(isForward);    //writes the direction of the motor to the MC message to be sent
#ifdef DEBUG
        Serial.println("~~~~~~~~~~~~~~~~~~~~Switching Direction~~~~~~~~~~~~~~~~~~~~~~~~");
#endif
    }
#ifdef DEBUG
    Serial.println(isForward ? "Forward" : "Reverse");
#endif
}

void DRIVERIO::handleErrorLights()
{
    writeLED4(bms->isSoCCritical());
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