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
    WDT_timings_t config;
    config.trigger = 5;         /* in seconds, 0->128 */
    config.timeout = 15;        /* in seconds, 0->128 */
    wdt.begin(config);
    Serial.begin(57600);

    pedals = Pedals(&motorController, &bms);
    driverio = DriverIO(&motorController, &bms);
    gpio = GPIO(&motorController, &bms);
    pinMode(RELAY_PIN, OUTPUT);
    writeFaultLatch(NOT_FAULTED);

    canTest_wait.cancelTimer();
    spinningCheck_wait.cancelTimer();

    delay(2000);
    Serial.println("Cycle");
}

void loop()
{
    //Serial.println(".");
    myCan.events();
    gpioProcess();
    driverioProcess();
    pedalsProcess();
    motorController.writeMCState();
    checkShutdownStatus();
    wdt.feed();
}
