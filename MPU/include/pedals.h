/**
 * @file pedals.h
 * @author Nick DePatie, Peter Rydzynski, and Matt McCauley
 * @brief
 * @date 2022-03-25
 */
#ifndef PEDALS_H
#define PEDALS_H

#include <nerduino.h>
#include "mpuConfig.h"
#include "cascadiamc.h"
#include "orionbms.h"
#include "gpioHW.h"
#include "pedalHW.h"

//Pins
#define ACCEL1_PIN                  A0
#define ACCEL2_PIN                  A1
#define BRAKE1_PIN                  20
#define BRAKE2_PIN                  21
#define BRAKELIGHT_PIN              4

class Pedals
{
    private:
        bool brakePressed = false;
        uint32_t timeBrake = 0;     // the time at which the brake was last pressed

        CascadiaMC *motorController;
        OrionBMS *bms;
        PDU *pdu;

        PedalHW brakes;
        PedalHW accelerator;

        uint8_t brakePins[2] = {BRAKE1_PIN, BRAKE2_PIN};
        uint8_t accelPins[2] = {ACCEL1_PIN, ACCEL2_PIN};

        int16_t clTorque; //Torque limit determined by current limit

        int16_t appliedTorque = 0; // applied motor torque

        float torqueLimitPercentage = 1.0; // percentage of torque limiting

        /**
         * @brief Calculates what torque to send to the motor controller
         * 
         */
        int16_t calcTorque(double torqueScale);

        /**
         * @brief Calculates the torque limit based on the BMS's calculated discharge current limit
         * 
         * @return int16_t
         */
        int16_t calcCLTorqueLimit();

        /**
         * @brief calculates the regen charge limit based on the BMS's calculated charge current limit
         * 
         * @return int16_t 
         */
        int16_t calcCLRegenLimit();

    public:
        Pedals();

        Pedals(CascadiaMC *p_motorController, OrionBMS *p_bms, PDU *p_pdu);

        ~Pedals();

        /**
         * @brief Reads the status of the brake, and sets brakePressed to True if it is pressed
         * 
         */
        FaultStatus_t readBrake();

        /**
         * @brief Handles the sending of acceleration commands
         * @note Handles Regen Braking and Acceleration by using negative and positive torque commands, respectively
         * 
         * @return If the accelerators are not faulted
         */
        FaultStatus_t readAccel();

        /**
         * @brief retrieves the value of the torque percentage
         * 
         * @return int representing the percentage of torque limiting
        */
        uint8_t getTorqueLimitPercentage();

        /**
         * @brief Sets the percentage of torque limiting
         * 
         * @param percentage 
        */
        void setTorqueLimitPercentage(float percentage);
};

#endif