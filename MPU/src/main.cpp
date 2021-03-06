/**
 * @file main.cpp
 * @author Nick DePatie
 * @brief Entry Point for Program
 */
#include <nerduino.h>
#include "mpu.h"
#include "Watchdog_t4.h"

WDT_T4<WDT1> wdt;

void setup()
{
    NERduino.begin();
    initializeCAN(1);           //The "1" parameter is useless for now, in the future the parameter is which CAN line to initialize
    WDT_timings_t config;
    config.trigger = 5;         /* in seconds, 0->128 */
    config.timeout = 15;        /* in seconds, 0->128 */
    wdt.begin(config);
    delay(2000);
}

void loop()
{
    myCan.events();
    mpu.gpioProcess();
    mpu.driverioProcess();
    mpu.pedalsProcess();
    mpu.sendMCMsg();
    mpu.checkShutdownStatus();
    wdt.feed();
}
