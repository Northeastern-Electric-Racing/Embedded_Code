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


class DASHBOARD
{
    public:
        DASHBOARD();
        ~DASHBOARD();
};


class BUTTON
{
    private:
        ButtonState_t state = NOT_PRESSED;
        Timer debounce;
        uint8_t pin;

        /**
         * @brief Flag that indicates that the button has passed debounce and hasn't been read yet
         */
        bool readyToReadPulse = false;

    public:
        BUTTON(uint8_t pinNumber);
        virtual ~BUTTON();

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
};


class SWITCH
{
    private:
        uint8_t pin;
        bool previousReading;

    public:
        SWITCH(uint8_t pinNumber);
        
        ~SWITCH();

        /**
         * @brief Get the current reading of the switch pin
         * 
         * @return true 
         * @return false 
         */
        bool getSwitchState();

        /**
         * @brief Return whether the switch state has been flipped since the last reading
         * 
         * @return true 
         * @return false 
         */
        bool hasSwitchToggled();
};


class LED
{
    private:
        Timer blinkTimer;
        uint8_t pin;
        bool isBlinkEnabled = false;

        /**
         * @brief Internal state of LED while blinking for toggling state
         */
        bool blinkState = LOW;

    public:
        LED(uint8_t pinNumber);

        virtual ~LED();

        /**
         * @note if the LED's blinking is enabled, this function does nothing and is overridden by blinking
         * 
         * @param state 
         */
        void writeLED(bool state);

        void blinkEnable(bool state);

        void updateBlink();
};


class SPEAKER
{
    private:
        Timer waitTime;
        uint8_t pin;

        void writeSpeaker(bool state);

    public:
        SPEAKER();
        SPEAKER(uint8_t pinNumber);
        ~SPEAKER();

        /**
         * @brief start the speaker sound and speaker time
         */
        void playSpeaker();

        /**
         * @brief Attempts to stop speaker based on the internal timer
         */
        void attemptToStopSpeaker();
};


class STARTBUTTON: public BUTTON, public LED
{
    public:
        STARTBUTTON(uint8_t ledPinNumber, uint8_t buttonPinNumber);
        ~STARTBUTTON();
};

#endif