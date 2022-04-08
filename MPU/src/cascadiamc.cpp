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
    isMCLocked = false;
}


void CASCADIAMC::writeMCState()
{
    if(isMCLocked)
    {
        disableMCLockout();
        while(!motorCommand_wait.isTimerExpired()){}
    }
    while(!motorCommand_wait.isTimerExpired()){}

#ifdef DEBUG
    Serial.println("CAN MC Message:");
    for(uint8_t i=0; i<8; i++)
    {
        Serial.println(mcMsg.canMsg[i]);
    }
    Serial.println("");
#endif

    sendMessage(CANMSG_ACCELERATIONCTRLINFO, 8, mcMsg.canMsg);

    motorCommand_wait.startTimer(50);
}


void CASCADIAMC::toggleDirection(bool p_isForward)
{
    mcMsg.config.isForward = p_isForward;
    isMCLocked = true;
    isChangingDirection = true;
}


void CASCADIAMC::toggleOn(bool p_isOn)
{
    mcMsg.config.isOn = p_isOn;
    isMCLocked = true;
}

bool CASCADIAMC::getIsOn()
{
    return mcMsg.config.isOn;
}


void CASCADIAMC::changeTorque(uint16_t p_accelTorque)
{
    mcMsg.config.accelTorque = p_accelTorque;
}

void CASCADIAMC::clearFault()
{
    sendMessage(CANMSG_MC_SETPARAMETER, 8, FAULT_CLEAR);
    isFaulted = false;
}

void CASCADIAMC::raiseFault()
{
    isFaulted = true;
}

bool CASCADIAMC::checkFault()
{
    return isFaulted;
}