/**
 * @file pedals.h
 * @author Nick DePatie, Peter Rydzynski, and Matt McCauley
 * @brief
 * @date 2022-03-25
 */
#ifndef PEDALS_H
#define PEDALS_H

#include <nerduino.h>
#include "cascadiamc.h"
#include "orionbms.h"

//Pins
#define ACCEL1_PIN              A0
#define ACCEL2_PIN              A1
#define BRAKE1_PIN              34
#define BRAKE2_PIN              35
#define BRAKELIGHT_PIN          4

// motor torque constants
#define MAXIMUM_TORQUE          150    // in Nm x 10 (ex: 123 = 12.3Nm)
#define POT_LOWER_BOUND         35      // a pot value from 0 to 1023
#define POT_UPPER_BOUND         1023    // a pot value from 0 to 1023

// regen braking constants
#define START_TIME              0       // delay from when brake is pressed to when regen starts
#define MAX_REGEN_TORQUE        -250    // maximum regen torque value
#define RAMP_TIME               10      // time until the maximum regen torque is reached (in seconds)

#define MAX_BRAKE_ERRORS        5
#define MAX_ACCEL_ERRORS        5

#define ACCELERATOR_ERROR_PER   0.05

class PEDALS
{
    private:
        // message to turn motor off
        bool brakePressed = false;
        uint32_t timeBrake = 0;     // the time at which the brake was last pressed

        CASCADIAMC *motorController;
        ORIONBMS *bms;

        Timer brakeReading_wait;
        Timer brakeReading_debounce;
        Timer pedalReading_wait;
        Timer pedalReading_debounce;

        uint8_t accelErrors = 0;
        bool accelFault = false;

    public:
        PEDALS();

        PEDALS(CASCADIAMC *p_motorController, ORIONBMS *p_bms);

        ~PEDALS();

        /**
         * @brief Reads the status of the brake, and sets brakePressed to True if it is pressed
         * 
         */
        void readBrake();

        /**
         * @brief Handles the sending of acceleration commands
         * @note Handles Regen Braking and Acceleration by using negative and positive torque commands, respectively
         * 
         * @return If the accelerators are not faulted
         */
        bool readAccel();
};

#endif