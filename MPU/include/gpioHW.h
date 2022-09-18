/**
 * @file gpioHW.h
 * @author Nick DePatie
 * @date 2022-08-07
 */
#ifndef GPIOHW_H
#define GPIOHW_H

#include <nerduino.h>

class RadiatorFan
{
    private:
        bool isEnabled = true;
        uint8_t pin;

    public:
        RadiatorFan();
        RadiatorFan(uint8_t pinNum);
        ~RadiatorFan();

        /**
         * @brief Enables/disables the fan entirely
         * 
         * @param status 
         */
        void enableFan(bool status);
};


class CoolingPump
{
    private:
        bool isEnabled = true;
        uint8_t pin;

    public:
        CoolingPump();
        CoolingPump(uint8_t pinNum);

        ~CoolingPump();

        /**
         * @brief Enables/disables the pump
         * 
         * @param status 
         */
        void enablePump(bool status);
};


class TSMS  //TSMS stands for "Tractive System Main Switch"
{
    private:
        bool isTSCycled = false;
        bool prevReading = false;
        uint8_t pin;

    public:
        TSMS();
        TSMS(uint8_t pinNum);
        ~TSMS();

        /**
         * @brief Returns if TS power has been cycled (1 -> 0)
         * @note Resets the power cycle flag upon reading
         * 
         * @return true
         * @return false 
         */
        bool isPowerCycled();

        /**
         * @brief Returns the current reading of the line (If TSMS is switched on)
         * 
         * @return true 
         * @return false 
         */
        bool isReady();
};

#endif