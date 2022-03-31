/**
 * @file driverio.h
 * @author Nick DePatie, Peter Rydzynski, and Matt McCauley
 *
 * @date 2022-03-30
 * 
 */
#ifndef DRIVERIO_H
#define DRIVERIO_H

#include <nerduino.h>
#include "cascadiamc.h"

//Pins
#define LED4_PIN 3
#define LED5_PIN 5
#define SS_BUTT_PIN 29
#define SS_LED_PIN 32
#define SPEAKER_PIN 7
#define REVERSE_SW_PIN 9

/**
 * @brief Class for handling all Driver IO functionality
 * 
 */
class DRIVERIO
{
    private:
        bool isForward = false;
        bool isOn = false;
        uint8_t accelTorqueLow = 0;
        uint8_t accelTorqueHigh = 0;

        CASCADIAMC *motorController;

        Timer ssButton_debounce;
        Timer powerToggle_wait;

    public:
        DRIVERIO();

        DRIVERIO(CASCADIAMC *motorController);

        ~DRIVERIO();

        /**
         * @brief Debounces the Start/Stop button and toggles the power depending on if the button was pushed
         * 
         */
        void handleSSButton();

        /**
         * @brief Detects a change in the direction of the forward/reverse switch
         * 
         */
        void handleReverseSwitch();

        /**
         * @brief Writes LED4 to a specific state
         * 
         * @param state 
         */
        void writeLED4(bool state);

        /**
         * @brief Writes LED5 to a specific state
         * 
         * @param state 
         */
        void writeLED5(bool state);

        /**
         * @brief Writes the speaker to a specific state
         * 
         * @param state 
         */
        void writeSpeaker(bool state);
};

#endif