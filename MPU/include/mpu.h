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

class MPU
{
    private:
        DRIVERIO driverio;
        GPIO gpio;
        PEDALS pedals;
        CASCADIAMC motorController;
        ORIONBMS bms;
        
        bool isShutdown = false;

        Timer ioRead_wait;
        Timer canTest_wait;

        enum
        {
            TRIGGER_FAULT,
            FAULT_OK
        };
        
        void writeFaultLatch(bool status);

    public:
        MPU();

        ~MPU();

        /**
         * @brief Handles all DriverIO Processes
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
         * @brief Sends the message that was loaded into the MC
         * 
         */
        void sendMCMsg();

        /**
         * @brief Passes loaded temp into private BMS object
         * 
         */
        void setBMSAvgTemp(uint8_t p_avgTemp);

        /**
         * @brief Passes Loaded SoC into private BMS object
         * 
         */
        void setBMSSoC(uint8_t p_soc);

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
        * @brief Shuts off the car in the event of an error
        * @note ****Code stays here until power cycle for safety****
        * 
        */
        void shutOffCar();
};

extern MPU mpu;

#endif