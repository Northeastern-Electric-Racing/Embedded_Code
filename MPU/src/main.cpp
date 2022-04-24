/**
 * @file main.cpp
 * @author Nick DePatie
 * @brief Entry Point for Program
 */
#include <nerduino.h>
#include "mpu.h"

void setup()
{
    NERduino.begin();
    initializeCAN(1);           //The "1" parameter is useless for now, in the future the parameter is which CAN line to initialize
}

void loop()
{
    myCan.events();
    mpu.gpioProcess();
    mpu.driverioProcess();
    mpu.pedalsProcess();
    mpu.sendMCMsg();
    // Serial.println("cycle");
    // mpu.checkShutdownStatus();
}
