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
#include "mpuConfig.h"
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
class DriverIO
{
    private:
        CascadiaMC *motorController;
        OrionBMS *bms;

        Speaker speaker {SPEAKER_PIN};
        Button incrButton;
        Button decrButton;

        uint8_t prev_state = OFF;

        bool isCharging = false;

        //For future development and integration with Raspberry Pi
        //DASHBOARD dashboard;

        Timer powerToggle_wait;
        Timer changeStateTimer;

        bool canTransitionToOff();

        bool canTransitionToOn();

    public:
        DriverIO();

        DriverIO(CascadiaMC *motorController, OrionBMS *p_bms);

        ~DriverIO();

        /**
         * @brief Debounces the increment and decrement buttons
         * 
         */
        void handleButtonState(bool tsms_status);

        /**
         * @brief Handles whether the speaker is playing or not
         * 
         */
        void handleSpeaker();

        /**
         * @brief Handles if  we want to go in reverse 
         * 
         */
        void handleReverse();

        /**
         * @brief 
         * 
         * @param msg 
         */
        void wheelIO_cb(const CAN_message_t &msg);
};

#endif