#include "cascadiamc.h"


CascadiaMC::CascadiaMC()
{
    motorCommand_wait.cancelTimer();
}


CascadiaMC::~CascadiaMC(){}


void CascadiaMC::disableMCLockout()
{
    while(!motorCommand_wait.isTimerExpired()){}

    sendMessage(CANMSG_ACCELERATIONCTRLINFO, 8, mcOff); // release lockout / OFF
    Serial.println("UNLOCKED");
    motorCommand_wait.startTimer(CAN_CMD_DELAY);
    isMCLocked = false;
}


void CascadiaMC::writeMCState()
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

    motorCommand_wait.startTimer(CAN_CMD_DELAY);
}


void CascadiaMC::toggleDirection()
{
    if (mcMsg.config.isOn) {
        togglePower();
    }
    mcMsg.config.isForward = !mcMsg.config.isForward;
}

void CascadiaMC::setDirection(bool p_direction)
{
    if (mcMsg.config.isOn) {
        togglePower();
    }
    mcMsg.config.isForward = p_direction;
}


void CascadiaMC::togglePower()
{
    mcMsg.config.isOn = !mcMsg.config.isOn;
    isMCLocked = true;
}


bool CascadiaMC::getIsOn()
{
    return mcMsg.config.isOn;
}


bool CascadiaMC::getDirection()
{
    return mcMsg.config.isForward;
}


void CascadiaMC::changeTorque(uint16_t p_accelTorque)
{
    mcMsg.config.accelTorque = p_accelTorque;
}


uint16_t CascadiaMC::getTorque()
{
    return mcMsg.config.accelTorque;
}


void CascadiaMC::clearFault()
{
    int time = millis() + 250;
    while(millis() < time) {
        sendMessage(CANMSG_MC_SETPARAMETER, 8, FAULT_CLEAR);
        delay(5);
    }
    isFaulted = false;
}


void CascadiaMC::raiseFault()
{
    if (isFaulted == false) {
        Serial.println("CUCK");
        delay(1000);
    }
    isFaulted = true;
}


bool CascadiaMC::checkFault()
{
    return isFaulted;
}


void CascadiaMC::emergencyShutdown()
{
    mcMsg.config.isOn = false;
    mcMsg.config.accelTorque = 0;
    writeMCState();
}


void CascadiaMC::setMotorSpeed(int16_t p_motorSpeed)
{
    motorSpeed = p_motorSpeed;
}

bool CascadiaMC::isMotorMoving()
{
    return abs(motorSpeed) > 0;
}


bool CascadiaMC::shouldMotorBeSpinning()
{
    return mcMsg.config.accelTorque > NOT_SPINNING_TORQUE_LIMIT;
}


int16_t CascadiaMC::getMotorSpeed()
{
    return motorSpeed;
}

void CascadiaMC::setRadiatorTemp(int16_t temp)
{
    radiatorTemp = temp;
}

int16_t CascadiaMC::getRadiatorTemp()
{
    return radiatorTemp;
}