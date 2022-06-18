#include "mpu.h"

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


void CANLineVerified()
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
        writeFaultLatch(FAULT_OK);
    }
}


void shutOffCar()
{
    writeFaultLatch(TRIGGER_FAULT);
    motorController.emergencyShutdown();
    Serial.println("Shutting off Car!!!!!");
    while(1){}
    writeFaultLatch(FAULT_OK);
}


void writeFaultLatch(bool status)
{
    digitalWrite(RELAY_PIN, status);
}



void bmsCurrentProcess(int16_t currentDraw)
{
    bms.setCurrentDraw(currentDraw);

    if(!boosting_debounce.isTimerExpired())
    {
        if(!bms.isCurrentPastLimit())
        {
            boosting_debounce.cancelTimer();
        }
    }

    if(bms.isCurrentPastLimit() && boosting_debounce.isTimerExpired() && !boosting_debounce.isTimerReset())
    {
        bms.setBoosting();
    }

    if(bms.isCurrentPastLimit() && boosting_debounce.isTimerExpired())
    {
        boosting_debounce.startTimer(100);
    }

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