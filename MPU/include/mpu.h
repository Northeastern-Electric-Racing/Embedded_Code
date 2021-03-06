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
        bool ssReady = false;

        Timer ioRead_wait;
        Timer canTest_wait;
        Timer spinningCheck_wait;
        Timer boosting_debounce;

        enum
        {
            TRIGGER_FAULT,
            FAULT_OK
        };
        
        /**
         * @brief Writes the fault latch whichever condition you pass (see above enumerator)
         * 
         * @param status 
         */
        void writeFaultLatch(bool status);

        /**
        * @brief Shuts off the car in the event of an error
        * @note ****Code stays here until power cycle for safety****
        * 
        */
        void shutOffCar();
        

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
         * @brief sets the current draw limit for the BMS
         * 
         */
        void setCurrentLimit(uint16_t currentLimit);

        /**
         * @brief Set the Charge Current Limit for the BMS
         * 
         * @param currentLimit 
         */
        void setChargeCurrentLimit(uint16_t currentLimit);

        /**
         * @brief Sets the current draw and also puts the BMS into boost mode if the current exceeds the current limit
         * 
         */
        void bmsCurrentProcess(int16_t currentDraw);
        
        /**
         * @brief Set the motor speed of the MC
         * 
         * @param motorSpeed 
         */
        void setMotorSpeed(int16_t motorSpeed);

        /**
         * @brief Enables the BMS to stay in Charging Mode
         * 
         */
        void enableBMSChargingMode();

        /**
         * @brief Sets the Live BMS Voltage
         * 
         */
        void setBMSVoltage(int16_t voltage);

        /**
         * @brief Set the Motor Temp
         * 
         * @param temp 
         */
        void setMotorTemp(int16_t temp);

        /**
         * @brief Make sure the motor is spinning if we send a motor torque command
         * 
         * @return true 
         * @return false 
         */
        bool verifyMotorSpinning();
};

extern MPU mpu;

#endif