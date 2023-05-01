#include "cascadiamc.h"


CascadiaMC::CascadiaMC()
{
    motorCommand_wait.cancelTimer();
}


CascadiaMC::~CascadiaMC(){}


void CascadiaMC::disableMCLockout()
{
    while(!motorCommand_wait.isTimerExpired()){}

    sendMessageCAN1(CANMSG_ACCELERATIONCTRLINFO, 8, mcOff); // release lockout / OFF
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

    sendMessageCAN1(CANMSG_ACCELERATIONCTRLINFO, 8, mcMsg.canMsg);

    motorCommand_wait.startTimer(CAN_CMD_DELAY);
}


void CascadiaMC::toggleDirection()
{
    mcMsg.config.isForward = !mcMsg.config.isForward;
}

void CascadiaMC::setDirection(bool p_direction)
{
    mcMsg.config.isForward = p_direction;
}

void CascadiaMC::togglePower()
{
    mcMsg.config.isOn = !mcMsg.config.isOn;
    isMCLocked = true;
}

void CascadiaMC::setPower(bool state)
{
    if(mcMsg.config.isOn == state) return;

    isMCLocked = true;

    mcMsg.config.isOn = state;
}

bool CascadiaMC::getIsOn()
{
    return mcMsg.config.isOn;
}

bool CascadiaMC::getDirection()
{
    return mcMsg.config.isForward;
}

void CascadiaMC::changeTorque(int16_t p_accelTorque)
{
    mcMsg.config.accelTorque = p_accelTorque;
}


uint16_t CascadiaMC::getTorque()
{
    return mcMsg.config.accelTorque;
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

void CascadiaMC::setFeedbackTorque(int16_t fb_torque)
{
    feedback_torque = fb_torque;
}

int16_t CascadiaMC::getFeedbackTorque()
{
    return feedback_torque;
}