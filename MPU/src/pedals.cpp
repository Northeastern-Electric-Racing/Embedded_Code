#include "pedals.h"

PEDALS::PEDALS(){}


PEDALS::PEDALS(CASCADIAMC *motorController)
{
    pinMode(BRAKE_PIN, INPUT_PULLUP);
}


PEDALS::~PEDALS(){}


void PEDALS::readAccel()
{
    int16_t appliedTorque = 0; // applied motor torque

    // send a regen torque if the brake button is pressed, otherwise send a normal torque
    if (brakePressed) 
    {
        int totalBrakeTime = (millis() - timeBrake) / 1000; // length of time in seconds that the brake has been held down
        
        if (totalBrakeTime - START_TIME > RAMP_TIME) 
        { // max regen torque is allowed
        appliedTorque = MAX_REGEN_TORQUE;
        } 
        else if (totalBrakeTime - START_TIME > 0) 
        {
        appliedTorque = totalBrakeTime * (MAX_REGEN_TORQUE / RAMP_TIME); // regen torque depends on how long the brake has been held
        }
        else 
        {
        // do nothing
        }

    }
    else 
    {
        int reading = analogRead(ACCEL1_PIN); // value from 0 to 1023 (need to reverse)
        int16_t flippedVal = (reading * -1) + 1023; // reverse it so 0 is when pedal not pressed, and 1023 is at full press

        if (flippedVal < POT_LOWER_BOUND) 
        { // Set low point to prevent a positive torque in the resting pedal position
        flippedVal = 0;
        }

        double multiplier = (double)flippedVal / 1023; // torque multiplier from 0 to 1;

        appliedTorque = multiplier * MAXIMUM_TORQUE;
    }

    accelTorqueHigh = appliedTorque >> 8;
    accelTorqueLow = appliedTorque & 0x00FF;

    Serial.println(appliedTorque / 10); // prints out applied torque
}


void PEDALS::readBrake()
{
    int brakeVal = digitalRead(BRAKE_PIN);

    // if the brake is being pressed
    if (brakeVal == LOW) {
      if (!brakePressed) { // if brake was not already being pressed, set new press time
        timeBrake = millis();
      }
      brakePressed = true;
    }
    // if brake is not being pressed, toggle brakePressed variable
    else {
      brakePressed = false;
    }
}