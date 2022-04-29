#include "driverio.h"
#include <nerduino.h>

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

    pinMode(YLED_PIN, OUTPUT);
    digitalWrite(YLED_PIN, LOW);

    pinMode(SPEAKER_PIN, OUTPUT);
    digitalWrite(SPEAKER_PIN, HIGH);

    powerToggle_wait.cancelTimer();
    ssButton_debounce.cancelTimer();
    speaker_wait.cancelTimer();
    tempWarningBlink_wait.cancelTimer();

    motorController = p_motorController;
    bms = p_bms;
}


DRIVERIO::~DRIVERIO(){}


void DRIVERIO::handleSSButton()
{
    //if the button is still being held during and after the timer runs out, then toggle power
    if(!ssButton_debounce.isTimerExpired())
    {
        if(!digitalRead(SS_BUTT_PIN)) // If released
        {
            ssButton_debounce.cancelTimer();
        }
        return;
    }
    if(ssButtonDebounced() && bms->getChargeMode())
    {
        bms->toggleAIR();
        powerToggle_wait.startTimer(1500);
        ssButton_debounce.cancelTimer();
        return;
    }
    if(ssButtonDebounced() && (motorController->getIsOn() || (!motorController->getIsOn() && !motorController->checkFault())))
    {
        motorController->togglePower();    //Writes the power state of the motor to the MC message to be sent
        if(motorController->getIsOn())
        {
            writeSpeaker(HIGH);
            speaker_wait.startTimer(1500);
        }
        Serial.println("");
        powerToggle_wait.startTimer(1500);
        ssButton_debounce.cancelTimer();
        return;
    }
    if(speaker_wait.isTimerExpired())
    {
        writeSpeaker(LOW);
    }
    if(digitalRead(SS_BUTT_PIN) && powerToggle_wait.isTimerExpired()) // If pressed and no timer
    {
        ssButton_debounce.startTimer(25);
    }
}


bool DRIVERIO::ssButtonDebounced()
{
    return (digitalRead(SS_BUTT_PIN) && ssButton_debounce.isTimerExpired() && !ssButton_debounce.isTimerReset());
}


void DRIVERIO::handleSSLED()
{
    if(bms->getChargeMode())
    {
        digitalWrite(SS_LED_PIN,!(bms->isAIROpen()));
    }
    else
    {
        digitalWrite(SS_LED_PIN,motorController->getIsOn());
    }
}


void DRIVERIO::handleReverseSwitch()
{
    //Serial.print("Switch State:\t");
    //Serial.println(motorController->getDirection());
    if(digitalRead(REVERSE_SW_PIN) != motorController->getDirection())
    {
        motorController->toggleDirection();    //writes the direction of the motor to the MC message to be sent
    }
}


void DRIVERIO::handleErrorLights()
{
    writeLED4(bms->isSoCCritical());

    writeYLED(bms->isCharging());

    if(tempWarningBlink_wait.isTimerExpired())
    {
        writeLED5(LOW);
    }

    if(bms->isAvgTempCritical())
    {
        if(bms->isAvgTempShutdown())
        {
            writeLED5(HIGH);
            return;
        }
        writeLED5(HIGH);
        tempWarningBlink_wait.startTimer(500);
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
    digitalWrite(SPEAKER_PIN, !state);
}


void DRIVERIO::writeYLED(bool state)
{
    digitalWrite(YLED_PIN, state);
}