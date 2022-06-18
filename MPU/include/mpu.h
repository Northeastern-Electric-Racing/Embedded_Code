/**
 * @file mpu.h
 * @author Nick DePatie
 * @date 2022-03-30
 */
#ifndef MPU_H
#define MPU_H

#include <nerduino.h>
#include "driverio.h"
#include "gpio.h"
#include "pedals.h"
#include "orionbms.h"
#include "Watchdog_t4.h"

enum
{
    TRIGGER_FAULT,
    FAULT_OK
};

DRIVERIO driverio;
GPIO gpio;
PEDALS pedals;
CASCADIAMC motorController;
ORIONBMS bms;

WDT_T4<WDT1> wdt;

bool isShutdown = false;
bool ssReady = false;

Timer canTest_wait;
Timer boosting_debounce;
Timer spinningCheck_wait;

/**
 * @brief Writes the fault latch whichever condition you pass (see above enumerator)
 * 
 * @param status 
 */
void writeFaultLatch(bool status);

/**
* @brief Shuts off the car in the event of an error
* @note ****Code stays here until power cycle for safety****
* 
*/
void shutOffCar();
        

void driverioProcess();

/**
 * @brief Handles all Pedals Processes
 * 
 */
void pedalsProcess();
        
/**
 * @brief Handles all GPIO Processes
 * 
 */
void gpioProcess();

/**
 * @brief Checks if the CAN line is intact
 * 
 */
bool isCANLineOK();

/**
 * @brief Sets the canLineOK to true
 * 
 */
void CANLineVerified();

/**
 * @brief Shuts down the car if the isShutdown boolean is false
 * 
 */
void checkShutdownStatus();

/**
 * @brief Sets the current draw and also puts the BMS into boost mode if the current exceeds the current limit
 * 
 */
void bmsCurrentProcess(int16_t currentDraw);

/**
 * @brief Make sure the motor is spinning if we send a motor torque command
 * 
 * @return true 
 * @return false 
 */
bool verifyMotorSpinning();


#endif