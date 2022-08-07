/**
 * @file gpioHW.h
 * @author Nick DePatie
 * @date 2022-08-07
 */
#ifndef GPIOHW_H
#define GPIOHW_H

#include <nerduino.h>

class RADIATORFAN
{
    private:
        bool isEnabled = true;
        uint8_t pin;

    public:
        RADIATORFAN();
        RADIATORFAN(uint8_t pinNum);
        ~RADIATORFAN();

        /**
         * @brief Enables/disables the fan entirely
         * 
         * @param status 
         */
        void enableFan(bool status);
};


class COOLINGPUMP
{
    private:
        bool isEnabled = true;
        uint8_t pin;

    public:
        COOLINGPUMP();
        COOLINGPUMP(uint8_t pinNum);

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
         * @brief Returns the current reading of the line
         * 
         * @return true 
         * @return false 
         */
        bool getStatus();
};

#endif