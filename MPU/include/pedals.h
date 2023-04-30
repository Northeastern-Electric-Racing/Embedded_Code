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

#define MOTOR_RPM_TO_MPH_CONST      0.013048225
#define PIT_MAX_SPEED               5
#define ACCUMULATOR_SIZE            5

class Pedals
{
    private:
        int REGEN_STRENGTHS[4] = {0, 300, 600, 1000}; // N-m to be applied, in N-m * 10

        uint32_t regenStartTime = 0;
        bool regenActive = false;

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
        uint16_t torqueAccumulator[ACCUMULATOR_SIZE] = {0}; // accumulator for torque averaging when in pit lane or reverse

        float torqueLimitPercentage = 1.0; // percentage of torque limiting

        Regen_Level_t regenLevel = ZILCH;

        bool enableCL = false;

        /**
         * @brief Calculates what torque to send to the motor controller
         * 
         */
        int16_t calcTorque(double torqueScale);


        /**
         * @brief Calculates a new, limited torque based off of motor RPM to limit speed
         * 
         * @param torque 
         * @param mph 
         */
        void limitTorque(int16_t *torque, const float mph);

        /**
         * @brief Implements a traction control algorithm to decrease slippage of the car
         * 
         * @param torque 
         * @param mph 
         */
        void controlLaunch(int16_t *torque, const float mph);

        /**
         * @brief Retrieves the current acceleration of the car in g's from onboard accelerometer
         * 
         * @param gforce_buf 
         */
        void getGForce(double gforce_buf[3][1]);

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

        /**
         * @brief Increments the level of regen torque
         * 
         * @param level 
        */
        void incrRegenLevel();

        /**
         * @brief Gets the level of regen torque
         * 
         * @param level
        */
        uint8_t getRegenLevel();
        
        /**
         * @brief Toggles the traction control
        */
        void toggleControlLaunch();

        /**
         * @brief Returns if launch control is enabled
        */
        bool getControlLaunch();
};

#endif