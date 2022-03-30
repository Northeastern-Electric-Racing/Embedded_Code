#ifndef MPU_H
#define MPU_H

#include <nerduino.h>
#include "battbox.h"
#include "driverio.h"
#include "gpio.h"
#include "pedals.h"

class MPU
{
    private:
        BATTBOX battbox;
        DRIVERIO driverio;
        GPIO gpio;
        PEDALS pedals;
        CASCADIAMC motorController;

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
         * @brief Sends the message that was loaded into the MC
         * 
         */
        void sendMCMsg();
};

#endif