/**
 * @file cascadiamc.h
 * @author Nick DePatie
 * @date 2022-03-30
 */
#ifndef CASCADIAMC_H
#define CASCADIAMC_H

#define CAN_CMD_DELAY 5 // ms

#define FAULT_CLEAR_ATTEMPTS 5

#include <nerduino.h>

#define NOT_SPINNING_TORQUE_LIMIT   1500

class CASCADIAMC
{
    private:

        const unsigned char mcOff[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        const unsigned char FAULT_CLEAR[8] = {20, 0, 1, 0, 0, 0, 0, 0};

        union
        {
            uint8_t canMsg[8] = {0,0,0,0,0,0,0,0};

            struct
            {
                int16_t accelTorque     :16;    //bitfield might not be necessary, but I wanted to be explicit
                uint16_t reserved1      :16;
                bool isForward          :8;
                bool isOn               :8;
                uint16_t reserved2      :16;
            } config;
        }mcMsg;

        int16_t motorSpeed = 0;
        uint16_t radiatorTemp = 0;

        bool isMCLocked = false;
        bool isChangingDirection = false;
        bool isFaulted = false;

        /**
         * @brief disables Motor Controller Lockout by sending motor controller off message
         * 
         */
        void disableMCLockout();

    public:
        CASCADIAMC();

        ~CASCADIAMC();

        /**
         * @brief Writes the desired state of the motor controller using the saved config
         * @note Config should be fully loaded to minimize message clutter (i.e. try not to send the message too often)
         */
        void writeMCState();

        /**
         * @brief Loads the desired direction into the MC message
         * 
         * @param p_isForward 
         */
        void toggleDirection();

        /**
         * @brief Loads the desired power state into the MC message
         * 
         * @param p_isOn 
         */
        void togglePower();

        /**
         * @brief Get if the MC is on
         * 
         */
        bool getIsOn();

        /**
         * @brief Get if the MC is forward
         * 
         */
        bool getDirection();

        /**
         * @brief Loads the desired acceleration torque into the MC message
         * 
         * @param p_accelTorque 
         */
        void changeTorque(uint16_t p_accelTorque);

        /**
         * @brief Returns the acceleration torque from the MC message
         *  
         */
        uint16_t getTorque();

        /**
         * @brief Clears the Fault for Failing to Generate High Voltage
         * 
         */
        void clearFault();

        /**
         * @brief Raises the Fault for Failing to Generate High Voltage
         * 
         */
        void raiseFault();

        /**
         * @brief Checks for the Fault for Failing to Generate High Voltage
         * 
         */
        bool checkFault();

        /**
         * @brief Sets MC to off and Sends 0 torque command
         * 
         */
        void emergencyShutdown();

        /**
         * @brief Sets the angular motor speed
         * 
         */
        void setMotorSpeed(int16_t p_motorSpeed);

        /**
         * @brief Checks if the motor is moving
         * 
         * @return true 
         * @return false 
         */
        bool isMotorMoving();

        /**
         * @brief Sees if the commanded motor torque is 0
         * 
         * @return true 
         * @return false 
         */
        bool shouldMotorBeSpinning();

        /**
         * @brief Get the Motor Speed
         * 
         * @return int16_t 
         */
        int16_t getMotorSpeed();

        /**
         * @brief Set the Radiator Temperature
         * 
         * @param temp 
         */
        void setRadiatorTemp(int16_t temp);

        /**
         * @brief Get the Radiator Temperature
         * 
         * @return uint16_t 
         */
        int16_t getRadiatorTemp();
        
};

#endif