/**
 * @file gpio.h
 * @author Nick DePatie
 * @date 2022-03-30
 */
#ifndef GPIO_H
#define GPIO_H

#include <nerduino.h>
#include "mpuConfig.h"
#include "gpioHW.h"
#include "cascadiamc.h"
#include "orionbms.h"
#include "driverio.h"

#define SS_READY_SEN        39
#define PUMP_PIN            8
#define RADIATORFAN_PIN     28

class GPIO
{
    private:
        TSMS tsms;
        CascadiaMC *motorController;
        OrionBMS *bms;
        PDU *pdu;
        float radiatorFanSpeedPercentage = 1.0;

    public:
        GPIO();

        GPIO(CascadiaMC *p_motorController, OrionBMS *p_bms, PDU *p_pdu);

        ~GPIO();

        /**
         * @brief Handles the case for the TSMS switching states
         * 
         */
        bool getTSMS();

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

        /**
         * @brief Sets the percentage of the radiator fan
        */
        void setRadiatorFanPercentage(float speed);

        /**
         * @brief retrieves the value of the motor fan dial
         * 
         * @return int representing the percentage of the motor fan dial
        */
        uint8_t getMotorFanDialPercentage();
};

#endif