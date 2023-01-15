/**
 * @file driverioHW.h
 * @author Nick DePatie
 * @date 2022-07-20
 */

#ifndef DRIVERIOHW_H
#define DRIVERIOHW_H

#include <nerduino.h>

#define SPEAKER_DURATION        1500    //in milliseconds
#define LED_BLINK_TIME          1000    //in milliseconds
#define BUTTON_DEBOUNCE_TIME    50      //in milliseconds

typedef enum ButtonState_t
{
    NOT_PRESSED,
    DEBOUNCING,
    PRESSED
} ButtonState_t;


class Dashboard
{
    public:
        Dashboard();
        ~Dashboard();
};


class Button
{
    private:
        ButtonState_t state = NOT_PRESSED;
        Timer debounce;
        bool isPressed = false;

    public:
        Button();
        virtual ~Button();

        /**
         * @brief Checks the state of the passed button pin and starts debounce if going from 0->1
         * @note Does nothing if the button is already attempting a debounce
         */
        void checkButtonPin();

        /**
         * @brief Returns if the button has successfully passed the debounce
         * 
         * @return true
         * @return false
         */
        bool isButtonPressed();

        /**
         * @brief Returns if the button has successfully passed the debounce AND has not been read yet
         * 
         * @return true
         * @return false
         */
        bool isButtonPressed_Pulse();

        /**
         * @brief Set the current state of the button
         * @note Used for monitoring buttons over CAN
         * 
         * @param buttonState 
         */
        void setButtonState(bool buttonState);
};

class Speaker
{
    private:
        Timer waitTime;
        uint8_t pin;

        void writeSpeaker(bool state);

    public:
        Speaker();
        Speaker(uint8_t pinNumber);
        ~Speaker();

        /**
         * @brief start the speaker sound and speaker time
         */
        void playSpeaker();

        /**
         * @brief Attempts to stop speaker based on the internal timer
         */
        void attemptToStopSpeaker();
};

#endif