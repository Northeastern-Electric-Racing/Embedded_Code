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

//Pins
#define ACCEL1_PIN A0
#define ACCEL2_PIN A1
#define BRAKE_PIN 10

// motor torque constants
#define MAXIMUM_TORQUE 2300 // in Nm x 10 (ex: 123 = 12.3Nm)
#define POT_LOWER_BOUND 35 // a pot value from 0 to 1023
#define POT_UPPER_BOUND 1023 // a pot value from 0 to 1023

// regen braking constants
#define START_TIME 0   // delay from when brake is pressed to when regen starts
#define MAX_REGEN_TORQUE -250 // maximum regen torque value
#define RAMP_TIME  10  // time until the maximum regen torque is reached (in seconds)

class PEDALS
{
    private:
        // message to turn motor off
        bool brakePressed = false;
        uint32_t timeBrake = 0;   // the time at which the brake was last pressed

        uint8_t accelTorqueLow = 0;
        uint8_t accelTorqueHigh = 0;

        Timer brakeReading_wait;
        Timer pedalReading_wait;

    public:
        PEDALS();

        PEDALS(CASCADIAMC *motorController);

        ~PEDALS();

        void readBrake();

        void readAccel();
};

#endif