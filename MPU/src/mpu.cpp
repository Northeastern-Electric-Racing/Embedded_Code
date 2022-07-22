#include "mpu.h"

DRIVERIO driverio;
GPIO gpio;
PEDALS pedals;
CASCADIAMC motorController;
ORIONBMS bms;
WDT_T4<WDT1> wdt;
bool isShutdown = false;
bool ssReady = false;
Timer canTest_wait;
Timer boosting_debounce;
Timer spinningCheck_wait;


void driverioProcess()
{
    // Serial.println("DriverIO process...");
    driverio.handleSSButton();
    driverio.handleSSLED();
    driverio.handleReverseSwitch();
    driverio.handleErrorLights();
}


void pedalsProcess()
{
    //isShutdown = isShutdown ? true : !verifyMotorSpinning();
    // TEMP TODO: reimplement
    // if(!verifyMotorSpinning())
    // {Serial.println("MOTOR NOT SPINNING");}
    // Serial.println("Pedals process...");
    pedals.readBrake();
    bool accelFault = pedals.readAccel();
    isShutdown = isShutdown ? true : accelFault;
    if(accelFault){Serial.println("ACCEL FAULT");}
}


void gpioProcess()
{
    bool faultReset = gpio.handleMCHVFault();
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
    canTest_wait.startTimer(1000);
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