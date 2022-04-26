/**
 * @file orionbms.h
 * @author Nick DePatie
 * @date 2022-04-04
 */

#ifndef ORIONBMS_H
#define ORIONBMS_H

#include <nerduino.h>

#define CRITICAL_CELLTEMP           45      //degrees celcius
#define SHUTDOWN_CELLTEMP           55      //degrees celcius

#define CRITICAL_SOC                10      //percentage

#define BOOSTING_TIME_LIMIT         1000    //milliseconds
#define BOOSTING_EXIT_TIME          100     //milliseconds (Time for system to adjust to leaving boost state)

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
        int16_t currentDraw;

        uint8_t failsafeCode;
        Timer boosting_time;
        Timer boosting_warningTime;

        Timer isInChargeMode;

        bool airOpen = 0;
        uint8_t CLOSE_AIR_MSG[8] = {0x00,0x01,0x01,0x00,0x55,0x02,0x00,0x00};
        uint8_t OPEN_AIR_MSG[8] = {0x00,0x01,0x01,0x00,0xAA,0x00,0x00,0x00};

    public:
        ORIONBMS();

        ~ORIONBMS();

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
         * @brief Set whether the BMS is charging
         * 
         * @param p_isCharging
         */
        void setIsCharging(bool p_isCharging);

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


        /**
         * @brief Indicates the BMS is going into the "Boost" state
         *      where the BMS sends out more current than it's set current limit
         */
        void setBoosting();

        /**
         * @brief Gets whether or not the BMS is boosting
         * 
         */
        bool isBoosting();

        /**
         * @brief Gets if the BMS is about to leave the boosting state
         * @note this is important, as it gives the MC enough time to realize that we are limiting the current again
         * 
         */
        bool isLeavingBoosting();

        /**
         * @brief sets the current limit of the BMS
         * 
         */
        void setCurrentLimit(uint16_t p_currentLimit);

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