#include "mpu.h"

MPU mpu;

MPU::MPU()
{
    pedals = PEDALS(&motorController, &bms);
    driverio = DRIVERIO(&motorController, &bms);
    gpio = GPIO(&motorController, &bms);

    ioRead_wait.cancelTimer();
}


MPU::~MPU(){}


void MPU::driverioProcess()
{
    if(ioRead_wait.isTimerExpired())
    {
        Serial.println("DriverIO process...");
        driverio.syncMC_IO();
        driverio.handleSSButton();
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
    gpio.handlePump();
    Serial.print("BMS SOC:\t");
    Serial.println(bms.getSoC());
    Serial.print("BMS Temp:\t");
    Serial.println(bms.getAvgTemp());
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