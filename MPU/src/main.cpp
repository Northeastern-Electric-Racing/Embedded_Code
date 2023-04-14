/**
 * @file main.cpp
 * @author Nick DePatie
 * @brief Entry Point for Program
 */

#include <nerduino.h>
#include "mpu.h"
#include "mpuConfig.h"

enum mpu_states mpu_state;
uint8_t drive_state = OFF;

void setup()
{
    NERduino.begin();
    mpu_state = BOOT;
    initializeCAN(CANLINE_1, BAUD_RATE, &(*mpuCanCallback));
    WDT_timings_t config;
    config.trigger = 5;         /* in seconds, 0->128 */
    config.timeout = 15;        /* in seconds, 0->128 */
    wdt.begin(config);
    Serial.begin(57600);

    pdu = PDU();
    pedals = Pedals(&motorController, &bms, &pdu);
    driverio = DriverIO(&motorController, &bms, &gpio);
    gpio = GPIO(&motorController, &bms, &pdu);
    pinMode(RELAY_PIN, OUTPUT);
    writeFaultLatch(NOT_FAULTED);

    canTest_wait.cancelTimer();
    spinningCheck_wait.cancelTimer();

    delay(2000);
    Serial.println("Cycle");
}

void loop()
{
    gpioProcess();
    driverioProcess();
    pedalsProcess();
    motorController.writeMCState();
    pdu.sendPDUMsg();
    checkShutdownStatus();
    sendMPUStatus();
    wdt.feed();
}
