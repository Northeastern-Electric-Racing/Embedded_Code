/**
 * @file orionbms.h
 * @author Nick DePatie
 * @date 2022-04-04
 */

#ifndef ORIONBMS_H
#define ORIONBMS_H

#include <nerduino.h>

#define CRITICAL_CELLTEMP           40      //degrees celcius
#define SHUTDOWN_CELLTEMP           55      //degrees celcius

#define CRITICAL_SOC                20      //percentage

enum
{
    AIR_CLOSED,
    AIR_OPEN
};

class ORIONBMS
{
    private:
        uint8_t SoC;
        uint8_t avgTemp;
        uint16_t currentLimit;
        uint16_t chargeCurrentLimit;
        int16_t currentDraw;
        int16_t liveVoltage;

        uint8_t failsafeCode;
        Timer boostRecharge_wait;

        Timer isInChargeMode;

        bool airOpen = 1;
        uint8_t CLOSE_AIR_MSG[8] = {0x01,0x00,0x01,0x00,0x02,0x55,0x00,0x00};
        uint8_t OPEN_AIR_MSG[8] = {0x01,0x00,0x01,0x00,0x00,0xAA,0x00,0x00};

    public:
        ORIONBMS();

        ~ORIONBMS();

/***************************************************************
 * Functions for Handling Errors and Warnings
****************************************************************/

        /**
         * @brief Sets the current state of charge
         * 
         * @param p_SoC 
         */
        void setSoC(uint8_t p_SoC);

        /**
         * @brief Set the current average temperature
         * 
         * @param p_avgTemp 
         */
        void setAvgTemp(uint8_t p_avgTemp);

        /**
         * @brief Set the current failsafe code (Code not processed, only saved)
         * 
         * @param p_failsafeCode 
         */
        void setFailsafeCode(uint8_t p_failsafeCode);

        /**
         * @brief Retrieves the current SoC of the batteries
         * 
         */
        uint8_t getSoC();

        /**
         * @brief Returns whether the SoC is critically low
         * 
         * @return true 
         * @return false 
         */
        bool isSoCCritical();

        /**
         * @brief Gets the current average temperature
         * 
         * @return uint8_t 
         */
        uint8_t getAvgTemp();

        /**
         * @brief Returns whether the average temperature is critically high
         * 
         * @return true 
         * @return false 
         */
        bool isAvgTempCritical();

        /**
         * @brief Returns if the average temperature has reached the set shutdown temp
         * 
         * @return true 
         * @return false 
         */
        bool isAvgTempShutdown();

/***************************************************************
 * Functions for Current and Voltage Handling
****************************************************************/

        /**
         * @brief sets the current limit of the BMS
         * 
         */
        void setCurrentLimit(uint16_t p_currentLimit);

        /**
         * @brief Set the Charge Current Limit 
         * 
         * @param p_chargeCurrentLimit 
         */
        void setChargeCurrentLimit(uint16_t p_chargeCurrentLimit);

        /**
         * @brief Get the Current Limit of the BMS
         * 
         * @return uint16_t 
         */
        uint16_t getCurrentLimit();

        /**
         * @brief Get the Charge Current Limit
         * 
         * @return uint16_t 
         */
        uint16_t getChargeCurrentLimit();

        /**
         * @brief Set the current draw
         * 
         * @param p_currentDraw 
         */
        void setCurrentDraw(int16_t p_currentDraw);

        /**
         * @brief Checks if the passed current draw is greater than the set current limit
         * 
         * @param currentDraw 
         * @return true 
         * @return false 
         */
        bool isCurrentPastLimit();

        /**
         * @brief Checks if the car is charging/regen'ing by seeing if current draw is negative
         * 
         * @return true 
         * @return false 
         */
        bool isCharging();

        /**
         * @brief Set the Live Voltage object
         * 
         */
        void setLiveVoltage(int16_t p_voltage);

        /**
         * @brief Get the Live Voltage
         * 
         * @return int16_t 
         */
        int16_t getLiveVoltage();

/***************************************************************
 * Functions for Charging Mode/AIR control
****************************************************************/

        /**
         * @brief Set the Charge Mode object
         * 
         */
        void enableChargingMode();

        /**
         * @brief Get the Charge Mode state
         * 
         * @return true 
         * @return false 
         */
        bool getChargeMode();

        /**
         * @brief Toggles AIR status
         * 
         */
        void toggleAIR();

        /**
         * @brief Get AIR status
         * 
         */
        bool isAIROpen();
};

#endif