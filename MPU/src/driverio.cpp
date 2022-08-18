#include "driverio.h"
#include <nerduino.h>

DRIVERIO::DRIVERIO(){}


DRIVERIO::DRIVERIO(CASCADIAMC *p_motorController, ORIONBMS *p_bms)
{
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
    motorController->setDirection(reverseSwitch.getSwitchState());
}


void DRIVERIO::handleErrorLights()
{
    tempLED.updateBlink();
    if(bms->isAvgTempCritical())
    {
        if(bms->isAvgTempShutdown())
        {
            tempLED.blinkEnable(false);
            tempLED.writeLED(HIGH);
        }
        else tempLED.blinkEnable(true);
    }
    else
    {
        tempLED.blinkEnable(false);
        tempLED.writeLED(LOW);
    }

    //No updating of blinking for these LED's
    socLED.writeLED(bms->isSoCCritical());
    yLED.writeLED(((motorController->getTorque() > 1020) && motorController->getIsOn()) || bms->isCharging());
}