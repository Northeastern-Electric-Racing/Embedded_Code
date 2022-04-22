#include "mpu.h"

MPU mpu;

MPU::MPU()
{
    pedals = PEDALS(&motorController, &bms);
    driverio = DRIVERIO(&motorController, &bms);
    gpio = GPIO(&motorController, &bms);

    ioRead_wait.cancelTimer();
    pinMode(RELAY_PIN, OUTPUT);
}


MPU::~MPU(){}


void MPU::driverioProcess()
{
    if(!ioRead_wait.isTimerExpired()){return;}
    
    Serial.println("DriverIO process...");
    driverio.handleSSButton();
    driverio.handleSSLED();
    driverio.handleReverseSwitch();
    driverio.handleErrorLights();
    ioRead_wait.startTimer(100);
}


void MPU::pedalsProcess()
{
    Serial.println("Pedals process...");
    pedals.readBrake();
    isShutdown = pedals.readAccel();
}


void MPU::gpioProcess()
{
    gpio.handleMCHVFault();
    gpio.handlePump();
    gpio.handleRadiatorFan();
    isShutdown = isCANLineOK();
}


void MPU::sendMCMsg()
{
    motorController.writeMCState();
}


void MPU::setBMSAvgTemp(uint8_t p_avgTemp)
{
    bms.setAvgTemp(p_avgTemp);
}


void MPU::setBMSSoC(uint8_t p_soc)
{
    bms.setSoC(p_soc);
}


bool MPU::isCANLineOK()
{
    return !canTest_wait.isTimerExpired();
}


void MPU::CANLineVerified()
{
    canTest_wait.startTimer(500);
}

void MPU::checkShutdownStatus()
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

void MPU::shutOffCar()
{
    writeFaultLatch(TRIGGER_FAULT);
    motorController.emergencyShutdown();
    while(1){};
}

void MPU::writeFaultLatch(bool status)
{
    digitalWrite(RELAY_PIN, status);
}