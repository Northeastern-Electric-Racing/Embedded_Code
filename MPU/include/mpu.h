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
        
        Timer ioRead_wait;

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
};

extern MPU mpu;

#endif