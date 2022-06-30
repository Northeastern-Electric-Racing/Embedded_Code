/**
 * @file pedalsHW.h
 * @author Nick DePatie
 * @brief Pedal Hardware Abstraction
 * @date 2022-06-30
 */

#ifndef PEDALHW_H
#define PEDALHW_H

#include <nerduino.h>

//Maximum ADC value based on resolution (10 bit resolution for Teensy 4.1)
#define MAX_ADC_VALUE               1023 //1111111111 in binary

//Timer macros
#define PEDAL_DEBOUNCE_TIME         40 //ms
#define BRAKELIGHT_MIN_TIME_ON      250 //ms

class PEDAL_HW
{
    private:
        //Pedal reading timers
        Timer readingDebounce;
        //Pedal reading
        uint16_t readingDiff;
        int16_t avgReading;
        //Pedal error counting
        uint8_t readingErrors = 0;
        bool readingFault = false;

        //Constants specific to which pedal
        uint8_t errorPercent;
        uint8_t maxErrors;
        uint8_t pin1;
        uint8_t pin2;

        /**
         * @brief Basic Debounce Algorithm
         * @note Saves the average of the closest readings of both pedals over a debounce period
         */
        void minimizingDiffDebounce();

        /**
         * @brief Checks if the two passed values would raise an error, and checks if the error limit has been hit
         * 
         * @param val1 
         * @param val2 
         */
        void checkForPedalError(uint16_t val1, uint16_t val2);

    public:
        PEDAL_HW();

        /**
         * @brief Construct a new pedal hw object (requires pedal specific parameters)
         * @param errorPercent
         * @param maxErrors
         */
        PEDAL_HW(uint8_t p_errorPercent, uint8_t p_maxErrors, uint8_t pinNumbers[2]);

        ~PEDAL_HW();

        /**
         * @brief Reads the current brake value with error-checking and accuracy debounce
         * 
         * @return Percentage (0-100) of how much brake is pressed
         */
        uint8_t readPressedPercentage();
};

class BRAKELIGHT_HW
{
    private:
        //Brake light writing timer
        Timer brakeLightWait;
        uint8_t brakeLightPin;

    public:
        BRAKELIGHT_HW();

        BRAKELIGHT_HW(uint8_t pinNumber);

        ~BRAKELIGHT_HW();

        /**
         * @brief Writes the brakelight to a specific state
         * @note This function is intended to be activated constantly in the main loop, as the timing logic depends on retrying
         * @param state 
         */
        void writeBrakeLight(bool state);
};

#endif