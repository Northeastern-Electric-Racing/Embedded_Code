/**
 * @file orionbms.h
 * @author Nick DePatie
 * @date 2022-04-04
 */

#ifndef ORIONBMS_H
#define ORIONBMS_H

#include <nerduino.h>

#define CRITICAL_CELLTEMP   45  //degrees celcius
#define CRITICAL_SOC        10  //percentage




class ORIONBMS
{
    private:
        uint8_t SoC;
        uint8_t avgTemp;

        uint8_t failsafeCode;

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
};

#endif