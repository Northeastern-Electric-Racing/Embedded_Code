#include "mpu.h"


MPU::MPU()
{
    pedals = PEDALS(&motorController);
    driverio = DRIVERIO(&motorController);

    ioRead_wait.cancelTimer();
}


MPU::~MPU(){}


void MPU::driverioProcess()
{
    if(ioRead_wait.isTimerExpired())
    {
        driverio.handleSSButton();
        driverio.handleReverseSwitch();
        ioRead_wait.startTimer(100);
    }
}


void MPU::pedalsProcess()
{
    pedals.readBrake();
    pedals.readAccel();
}