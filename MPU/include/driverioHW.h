/**
 * @file driverioHW.h
 * @author Nick DePatie
 * @date 2022-07-20
 */

#ifndef DRIVERIOHW_H
#define DRIVERIOHW_H

#include <nerduino.h>

typedef enum ButtonState_t
{
    NOT_PRESSED,
    DEBOUNCING,
    PRESSED
} ButtonState_t;


class DASHBOARD
{

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
        void checkButtonPin(uint8_t debounceTime);

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

};


class LED
{

};


class SPEAKER
{
    
};


class STARTBUTTON: public BUTTON
{
    private:
        LED led;

    public:


};
#endif