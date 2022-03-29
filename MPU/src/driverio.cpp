#include "driverio.h"
#include <nerduino.h>

DRIVERIO::DRIVERIO(){}


DRIVERIO::DRIVERIO(CASCADIAMC *motorController)
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

    motorCommand_wait.startTimer(0);
}


DRIVERIO::~DRIVERIO(){}


void DRIVERIO::disableMCLockout()
{
    while(!motorCommand_wait.isTimerExpired()){}

    //sendMessage(CANMSG_ACCELERATIONCTRLINFO, 8, MOTOR_OFF); // release lockout / OFF

    motorCommand_wait.startTimer(50);
}


void DRIVERIO::writeMCState()
{
    disableMCLockout();
    while(!motorCommand_wait.isTimerExpired()){}

    unsigned char message[8] = {accelTorqueLow,accelTorqueHigh,0,0,isForward,isOn,0,0};
    sendMessage(CANMSG_ACCELERATIONCTRLINFO, 8, message);

    motorCommand_wait.startTimer(50);
}


void DRIVERIO::handleSSButton()
{
    if(digitalRead(SS_BUTT_PIN) && powerToggle_wait.isTimerExpired())
    {
        ssButton_debounce.startTimer(50);
        
        //if the button is still being held during and after the timer runs, then toggle power
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
            disableMCLockout();
            digitalWrite(SS_LED_PIN, isOn);     //Writes SS LED to the power state of the motor
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