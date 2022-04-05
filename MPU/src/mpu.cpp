#include "mpu.h"

MPU mpu;

MPU::MPU()
{
    pedals = PEDALS(&motorController, &bms);
    driverio = DRIVERIO(&motorController, &bms);
    gpio = GPIO(&motorController);

    ioRead_wait.cancelTimer();
}


MPU::~MPU(){}


void MPU::driverioProcess()
{
    if(ioRead_wait.isTimerExpired())
    {
        Serial.println("DriverIO process...");
        driverio.handleSSButton();
        driverio.handleReverseSwitch();
        ioRead_wait.startTimer(100);
    }
}


void MPU::pedalsProcess()
{
    Serial.println("Pedals process...");
    pedals.readBrake();
    pedals.readAccel();
}

void MPU::gpioProcess()
{
    gpio.handleMCHVFault();
}

void MPU::sendMCMsg()
{
    motorController.writeMCState();
}