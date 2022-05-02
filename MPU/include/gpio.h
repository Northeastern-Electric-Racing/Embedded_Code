/**
 * @file gpio.h
 * @author Nick DePatie
 * @date 2022-03-30
 */
#ifndef GPIO_H
#define GPIO_H

#include <nerduino.h>
#include "cascadiamc.h"
#include "orionbms.h"

#define SS_READY_SEN        6
#define PUMP_PIN            8
#define RADIATORFAN_PIN     28

#define MAX_FANSPEED_TEMP   10   //degrees celcius

class GPIO
{
    private:
        bool isSSPowerCycle = true;

        CASCADIAMC *motorController;
        ORIONBMS *bms;

    public:
        GPIO();

        GPIO(CASCADIAMC *p_motorController, ORIONBMS *p_bms);

        ~GPIO();

        /**
         * @brief Handles the high voltage motor controller fault from when it initially boots
         * 
         */
        bool handleMCHVFault();

        /**
         * @brief Handles the logic behind starting/stopping the pump
         * 
         */
        void handlePump();

        /**
         * @brief Handles the logic behind starting/stopping the radiator fan
         * 
         */
        void handleRadiatorFan();
};

#endif