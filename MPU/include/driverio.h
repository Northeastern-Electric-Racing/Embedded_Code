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
#include "orionbms.h"

//Pins
#define LED4_PIN        3
#define LED5_PIN        5
#define SS_BUTT_PIN     29
#define SS_LED_PIN      33
#define SPEAKER_PIN     7
#define REVERSE_SW_PIN  9

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
        ORIONBMS *bms;

        Timer ssButton_debounce;
        Timer powerToggle_wait;
        Timer speaker_wait;

    public:
        DRIVERIO();

        DRIVERIO(CASCADIAMC *motorController, ORIONBMS *p_bms);

        ~DRIVERIO();

        void syncMC_IO();

        /**
         * @brief Debounces the Start/Stop button and toggles the power depending on if the button was pushed
         * 
         */
        void handleSSButton();

        /**
         * @brief Handles the state of the Start/Stop LED
         * 
         */
        void handleSSLED();

        /**
         * @brief Detects a change in the direction of the forward/reverse switch
         * 
         */
        void handleReverseSwitch();

        /**
         * @brief Uses the current BMS state and lights the LEDs up accordingly
         * 
         */
        void handleErrorLights();

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