#include "cascadiamc.h"


CASCADIAMC::CASCADIAMC()
{

}


CASCADIAMC::~CASCADIAMC(){}


void CASCADIAMC::disableMCLockout()
{

}


void CASCADIAMC::writeMCState()
{
    disableMCLockout();
    while(!motorCommand_wait.isTimerExpired()){}

    //sendMessage(CANMSG_ACCELERATIONCTRLINFO, 8, mcMsg.canMsg);

    motorCommand_wait.startTimer(50);
}