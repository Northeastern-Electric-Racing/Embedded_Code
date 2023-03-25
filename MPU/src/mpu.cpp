#include "mpu.h"

DriverIO driverio;
GPIO gpio;
Pedals pedals;
CascadiaMC motorController;
OrionBMS bms;
WDT_T4<WDT1> wdt;
bool isShutdown = false;
bool ssReady = false;
Timer canTest_wait;
Timer spinningCheck_wait;

void driverioProcess()
{
    // Serial.println("DriverIO process...");
    driverio.handleButtonState();
    driverio.handleReverse();
    driverio.handleSpeaker();
}


void pedalsProcess()
{
    //isShutdown = isShutdown ? true : !verifyMotorSpinning();
    // TEMP TODO: reimplement
    // if(!verifyMotorSpinning())
    // {Serial.println("MOTOR NOT SPINNING");}
    // Serial.println("Pedals process...");
    FaultStatus_t pedalFault = NOT_FAULTED;
    pedalFault = pedals.readBrake();
    pedalFault = pedals.readAccel();
    /*
    if(pedalFault == FAULTED)
    {
        Serial.println("ACCEL FAULT");
        isShutdown = true;
    }
    */
}


void gpioProcess()
{
    bool faultReset = gpio.handleTSMS();
    gpio.handlePump();
    gpio.handleRadiatorFan();

    isShutdown = isShutdown ? true : !isCANLineOK();
    if(!isCANLineOK()){Serial.println("CAN FAULT");}
}

bool isCANLineOK()
{
    if(canTest_wait.isTimerExpired() && (digitalRead(SS_READY_SEN) == HIGH))
    {
        Serial.println("CAN FUCKED@$#^!$^@$#&");
        return false;
    }
    return true;
}


void setCANLineOK()
{
    canTest_wait.startTimer(2000);
}


void checkShutdownStatus()
{
    if(isShutdown)
    {
        shutOffCar();
    }
    else
    {
        writeFaultLatch(NOT_FAULTED);
    }
}


void shutOffCar()
{
    writeFaultLatch(FAULTED);
    motorController.emergencyShutdown();
    Serial.println("Shutting off Car!!!!!");
    while(1){}
    writeFaultLatch(NOT_FAULTED);
}


void writeFaultLatch(FaultStatus_t status)
{
    //The logic is flipped because of how the enum is set up to allow for exceptions if fault (FAULT = 1 = true, NOT FAULTED = 0 = False)
    digitalWrite(RELAY_PIN, !status);
}


bool verifyMotorSpinning()
{
    if(motorController.shouldMotorBeSpinning() && motorController.getIsOn())
    {
        if(motorController.isMotorMoving())
        {
            spinningCheck_wait.startTimer(5000);
        }
        if(spinningCheck_wait.isTimerExpired()) {
            return false;
        }
    }
    return true;
}

void sendMPUStatus()
{
    union
    {
        uint8_t msg[1] = {0};

        struct
        {
            uint8_t driveState;
        } info;
    } mpu_msg;

    //TODO: Fix this bandaid fix to address the mismatch between MPU states and
    // desired NERO states
    switch (mpu_state)
    {
        case BOOT:
            mpu_msg.info.driveState = OFF_MODE;
            break;

        case DRIVE:
            switch (drive_state)
            {
                case OFF:
                case PIT:
                case EFFICIENCY:
                case SPEED:
                    mpu_msg.info.driveState = drive_state;
                    break;
                
                case REVERSE:
                    mpu_msg.info.driveState = REVERSE_MODE;
            };
            break;

        case CHARGING:
            mpu_msg.info.driveState = CHARGING_MODE;
            break;

        case FAULT:
            mpu_msg.info.driveState = OFF_MODE;
            break;

        default:
            break;
    };

    sendMessageCAN1(MPU_STATUS_ID, 1, mpu_msg.msg);
}