#include <nerduino.h>
#include "mpu.h"

Timer mainTimer;

void setup()
{
    NERduino.begin();
    initializeCAN(1);   //The "1" parameter is useless for now, in the future the parameter is which CAN line to initialize
}

MPU mpu;

void loop()
{
    myCan.events();
    mpu.driverioProcess();
    mpu.pedalsProcess();
    mpu.sendMCMsg();
    Serial.println("cycle");
}