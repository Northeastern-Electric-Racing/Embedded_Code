#include "driverio.h"
#include <nerduino.h>

DRIVERIO::DRIVERIO(){}


DRIVERIO::DRIVERIO(CASCADIAMC *p_motorController, ORIONBMS *p_bms)
{
    reverseSwitch = SWITCH(REVERSE_SW_PIN);
    ssButton = STARTBUTTON(SS_LED_PIN, SS_BUTT_PIN);
    socLED = LED(LED4_PIN);
    tempLED = LED(LED5_PIN);
    yLED = LED(YLED_PIN);
    speaker = SPEAKER(SPEAKER_PIN);

    powerToggle_wait.cancelTimer();

    motorController = p_motorController;
    bms = p_bms;
}


DRIVERIO::~DRIVERIO(){}


void DRIVERIO::handleSSButtonPress()
{
    //Poll Button
    ssButton.checkButtonPin();

    //Poll Speaker if it is written high
    speaker.attemptToStopSpeaker();

    //If the button isn't pressed, do nothing
    if(!ssButton.isButtonPressed_Pulse()) return;

    //If the BMS is charging
    if(bms->getChargeMode())
    {
        bms->toggleAIR();
        powerToggle_wait.startTimer(1500);
        ssButton_debounce.cancelTimer();
        return;
    }

    //If the motor controller is on OR the motor is off and it is ready to go
    if(motorController->getIsOn() || 
        (!motorController->getIsOn() && !motorController->checkFault()))
    {
        motorController->togglePower();    //Writes the power state of the motor to the MC message to be sent
        if(motorController->getIsOn())
        {
            speaker.playSpeaker();
        }
        powerToggle_wait.startTimer(1500);
        return;
    }
}


void DRIVERIO::handleSSLED()
{
    if(bms->getChargeMode())
    {
        ssButton.writeLED(!bms->isAIROpen());
    }
    else
    {
        ssButton.writeLED(motorController->getIsOn());
    }
}


void DRIVERIO::handleReverseSwitch()
{
    if(reverseSwitch.hasSwitchToggled())
    {
        motorController->toggleDirection();    //writes the direction of the motor to the MC message to be sent
    }
}


void DRIVERIO::handleErrorLights()
{
    writeLED4(bms->isSoCCritical());

    //writeYLED(bms->isCharging() || bms->isBoosting());
    writeYLED(((motorController->getTorque() > 1020) && motorController->getIsOn()) || bms->isCharging());

    if(bms->isAvgTempCritical())
    {
        if(bms->isAvgTempShutdown())
        {
            Serial.println("shutdown");
            writeLED5(HIGH);
            return;
        }

        if(tempWarningBlink_wait.isTimerExpired())
        {
            Serial.println("check");
            LED5_status = !LED5_status;
            writeLED5(LED5_status);
            tempWarningBlink_wait.startTimer(1000);
        } 
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