/**
 * @file cascadiamc.h
 * @author Nick DePatie
 * @date 2022-03-30
 */
#ifndef CASCADIAMC_H
#define CASCADIAMC_H

#include <nerduino.h>


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

        Timer motorCommand_wait;

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
        void toggleDirection(bool p_isForward);

        /**
         * @brief Loads the desired power state into the MC message
         * 
         * @param p_isOn 
         */
        void toggleOn(bool p_isOn);

        /**
         * @brief Get if the MC is on
         * 
         */
        bool getIsOn();

        /**
         * @brief Loads the desired acceleration torque into the MC message
         * 
         * @param p_accelTorque 
         */
        void changeTorque(uint16_t p_accelTorque);

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
};

#endif