/**
 * @file mpu.h
 * @author Nick DePatie
 * @date 2022-03-30
 */
#ifndef MPU_H
#define MPU_H

#include <nerduino.h>
#include "gpio.h"
#include "driverio.h"
#include "pedals.h"
#include "orionbms.h"
#include "Watchdog_t4.h"
#include "canMsgHandler.h"

extern DriverIO driverio;
extern GPIO gpio;
extern Pedals pedals;
extern CascadiaMC motorController;
extern OrionBMS bms;

extern WDT_T4<WDT1> wdt;

extern bool isShutdown;
extern bool ssReady;
extern bool tsms_status;

extern Timer canTest_wait;
extern Timer spinningCheck_wait;

/**
 * @brief Writes the fault latch whichever condition you pass (see above enumerator)
 * 
 * @param status 
 */
void writeFaultLatch(FaultStatus_t status);

/**
* @brief Shuts off the car in the event of an error
* @note ****Code stays here until power cycle for safety****
* 
*/
void shutOffCar();
        
/**
 * @brief Handles all driverio processes
 * 
 */
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
void setCANLineOK();

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

/**
 * @brief Callback for processing CAN messages based on ID
 * 
 */
void mpuCanCallback(const CAN_message_t &msg);

/**
 * @brief Broadcasts a message about the internal states of the MPU
 * 
 */
void sendMPUStatus();

#endif