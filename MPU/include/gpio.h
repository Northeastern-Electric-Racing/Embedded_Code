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

#define SS_READY_SEN 6

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
        void handleMCHVFault();
};

#endif