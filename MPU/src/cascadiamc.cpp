#include "cascadiamc.h"


CASCADIAMC::CASCADIAMC()
{
    motorCommand_wait.cancelTimer();
}


CASCADIAMC::~CASCADIAMC(){}


void CASCADIAMC::disableMCLockout()
{
    while(!motorCommand_wait.isTimerExpired()){}

    sendMessage(CANMSG_ACCELERATIONCTRLINFO, 8, mcOff); // release lockout / OFF

    motorCommand_wait.startTimer(50);
}


void CASCADIAMC::writeMCState()
{
    disableMCLockout();
    while(!motorCommand_wait.isTimerExpired()){}

    sendMessage(CANMSG_ACCELERATIONCTRLINFO, 8, mcMsg.canMsg);

    motorCommand_wait.startTimer(50);
}


void CASCADIAMC::toggleDirection(bool p_isForward)
{
    mcMsg.config.isForward = p_isForward;
}


void CASCADIAMC::toggleOn(bool p_isOn)
{
    mcMsg.config.isOn = p_isOn;
}


void CASCADIAMC::changeTorque(uint16_t p_accelTorque)
{
    mcMsg.config.accelTorque = p_accelTorque;
}