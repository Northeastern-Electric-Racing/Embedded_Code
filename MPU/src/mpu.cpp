#include "mpu.h"


MPU::MPU()
{
    pedals = PEDALS(&motorController);
    driverio = DRIVERIO(&motorController);
}


MPU::~MPU(){}


void MPU::driverioProcess()
{
    driverio.handleSSButton();
    driverio.handleReverseSwitch();
}


void MPU::pedalsProcess()
{
    pedals.readAccel();
    pedals.readBrake();
}