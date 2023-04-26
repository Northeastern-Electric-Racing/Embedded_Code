#ifndef MPU_CONFIG_H
#define MPU_CONFIG_H

// BMS Parameters
#define CRITICAL_CELLTEMP 40 // degrees celcius
#define SHUTDOWN_CELLTEMP 55 // degrees celcius
#define CRITICAL_SOC 20      // percentage

// DriverIO Parameters
#define SPEAKER_DURATION 1500   // in milliseconds
#define LED_BLINK_TIME 1000     // in milliseconds
#define BUTTON_DEBOUNCE_TIME 50 // in milliseconds
#define CHANGE_STATE_TIME 400   // ms
#define POWER_TOGGLE_WAIT 1500  // ms

// GPIO Parameters
#define MAX_FANSPEED_TEMP 70 // degrees celcius

// Motor Controller Parameters
#define NOT_SPINNING_TORQUE_LIMIT 1500
#define CAN_CMD_DELAY 5 // ms

// motor torque constants
#define MAXIMUM_TORQUE 2220           // in Nm x 10 (ex: 123 = 12.3Nm)
#define CONT_TORQUE 1020              // ' '
#define POT_LOWER_BOUND 35            // a pot value from 0 to 1023
#define POT_UPPER_BOUND MAX_ADC_VALUE // a pot value from 0 to 1023

// regen braking constants
#define START_TIME 0 // delay from when brake is pressed to when regen starts
#define MAX_REGEN_TORQUE -250 // maximum regen torque value
#define RAMP_TIME                                                              \
  10 // time until the maximum regen torque is reached (in seconds)
#define MAX_BRAKE_ERRORS 5
#define MAX_ACCEL_ERRORS 5
#define ACCEL_OFFSET 250
#define ACCELERATOR_ERROR_PERCENT 25 // percent
#define BRAKES_ERROR_PERCENT 10      // percent
#define LEAVING_BOOST_TORQUE_SCALE 0.9
#define CL_TO_TOQRUE_CONST                                                     \
  16.51 // constant for calculating the current limited torque
#define ANALOG_BRAKE_THRESH 185
#define MAXIMUM_BRAKE 255

#define MPU_STATUS_ID 0xA

enum nero_states {
  OFF_MODE,
  PT_LANE_MODE,
  EFFICIENCY_MODE,
  SPEED_MODE,
  REVERSE_MODE,
  CHARGING_MODE,
  MAX_NERO_STATES
};

enum { OFF, PIT, EFFICIENCY, SPEED, REVERSE, MAX_DRIVE_STATES };
extern uint8_t drive_state;

enum mpu_states { BOOT, DRIVE, CHARGING, FAULT, MAX_MPU_STATES };

extern enum mpu_states mpu_state;

#endif // MPU_CONFIG_H