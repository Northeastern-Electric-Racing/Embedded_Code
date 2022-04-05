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

        bool isFault = false;
        uint8_t failsafeCode;

    public:
        ORIONBMS();

        ~ORIONBMS();

        void setSoC(uint8_t p_soc);

        void setAvgTemp(uint8_t p_avtTemp);

        void setFailsafeCode(uint8_t p_failsafeCode);
};

#endif