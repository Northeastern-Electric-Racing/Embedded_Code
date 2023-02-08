#include "driverio.h"
#include <nerduino.h>

DriverIO::DriverIO(){}


DriverIO::DriverIO(CascadiaMC *p_motorController, OrionBMS *p_bms)
{
    powerToggle_wait.cancelTimer();

    motorController = p_motorController;
    bms = p_bms;

    motorController->setDirection(false);
}


DriverIO::~DriverIO(){}


void DriverIO::handleSSButtonPress()
{
    //Poll Button
    ssButton.checkButtonPin();

    //Poll Speaker if it is written high
    speaker.attemptToStopSpeaker();
    //speaker.playSpeaker();

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


void DriverIO::handleReverseButton()
{
    //Poll Button
    revButton.checkButtonPin();

    //If the button isn't pressed, do nothing
    if(!revButton.isButtonPressed_Pulse()) return;

    motorController->setDirection(!motorController->getDirection());
}

void DriverIO::wheelIO_cb(const CAN_message_t &msg)
{
    static union
    {
        uint8_t msg[8];

        struct
        {
            uint16_t pot1;
            uint16_t pot2;
            bool button1 : 1;
            bool button2 : 1;
            bool button3 : 1;
            bool button4 : 1;
            bool button5 : 1;
            bool button6 : 1;
            bool button7 : 1;
            bool button8 : 1;
            uint8_t dontcare1;
            uint8_t dontcare2;
            uint8_t dontcare3;
        }io;
    } wheelio;

    for(uint8_t byte = 0; byte < 8; byte++)
    {
        wheelio.msg[byte] = msg.buf[byte];
    }

    ssButton.setButtonState(wheelio.io.button8);
    revButton.setButtonState(wheelio.io.button7);
}