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
#include "driverioHW.h"

//Pins
#define LED4_PIN        3
#define LED5_PIN        5
#define YLED_PIN        2
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
        CASCADIAMC *motorController;
        ORIONBMS *bms;

        SPEAKER speaker;
        LED socLED;
        LED tempLED;
        LED yLED;
        STARTBUTTON ssButton;
        SWITCH reverseSwitch;
        
        //For future development and integration with Raspberry Pi
        //DASHBOARD dashboard;

        Timer ssButton_debounce;
        Timer powerToggle_wait;
        Timer speaker_wait;
        Timer tempWarningBlink_wait;

        bool LED5_status = false;

    public:
        DRIVERIO();

        DRIVERIO(CASCADIAMC *motorController, ORIONBMS *p_bms);

        ~DRIVERIO();

        /**
         * @brief Debounces the Start/Stop button and toggles the power depending on if the button was pushed
         * 
         */
        void handleSSButtonPress();

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
};

#endif