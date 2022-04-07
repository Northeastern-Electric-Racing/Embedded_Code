#include "pedals.h"


PEDALS::PEDALS(){}


PEDALS::PEDALS(CASCADIAMC *p_motorController, ORIONBMS *p_bms)
{
    pinMode(BRAKE1_PIN, INPUT_PULLDOWN);
	pinMode(BRAKE2_PIN, INPUT_PULLDOWN);
	pinMode(BRAKELIGHT_PIN, OUTPUT);
	digitalWrite(BRAKELIGHT_PIN, HIGH);

	brakeReading_wait.cancelTimer();
	pedalReading_wait.cancelTimer();

	motorController = p_motorController;
	bms = p_bms;
}


PEDALS::~PEDALS(){}


void PEDALS::readAccel()
{
	if(pedalReading_wait.isTimerExpired())
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
			int accelPin1Val = analogRead(ACCEL1_PIN); 
  			int accelPin2Val = analogRead(ACCEL2_PIN);

			//if brake "bottoms out" or if there is a large discrepancy between readings, flag an accelerator fault
			if (accelPin1Val == 0 ||
				accelPin2Val == 0 ||
				accelPin1Val == 1023 ||
				accelPin2Val == 1023 ||
				(abs(accelPin1Val - accelPin2Val) > (1023 * 0.05))) 
			{
				accelFault = true;
  			}

			int16_t avgVal = (accelPin1Val + accelPin2Val) / 2;
			int16_t flippedVal = (avgVal * -1) + 1023; // reverse it so 0 is when pedal not pressed, and 1023 is at full press
			if (flippedVal < POT_LOWER_BOUND)
			{ // Set low point to prevent a positive torque in the resting pedal position
			flippedVal = 0;
			}

			double multiplier = (double)flippedVal / 950; // torque multiplier from 0 to 1;

			appliedTorque = (multiplier * MAXIMUM_TORQUE);

			//scale torque based on factor between 1 and 0 based on the temperature of the cells
			//appliedTorque = (int16_t)(-1* appliedTorque * fastSigmoid(bms->getAvgTemp() - CRITICAL_CELLTEMP));
		}

		motorController->changeTorque(appliedTorque);
		Serial.print("Acceleration:\t");
		Serial.println(appliedTorque / 10); // prints out applied torque

		pedalReading_wait.startTimer(50);
	}
}


void PEDALS::readBrake()
{
	if(brakeReading_wait.isTimerExpired())
	{
		int brake1Val = digitalRead(BRAKE1_PIN);
		int brake2Val = digitalRead(BRAKE2_PIN);
		if(brake1Val != brake2Val)
		{
			brakeErrors++;
			if(brakeErrors >= MAX_BRAKE_ERRORS)
			{
				brakeFault = true;
			}
		}

		// if the brake is being pressed
		if (brake1Val == HIGH) {
			if (!brakePressed)
			{ // if brake was not already being pressed, set new press time
				timeBrake = millis();
			}
			brakePressed = true;
		}
		// if brake is not being pressed, toggle brakePressed variable
		else {
		brakePressed = false;
		}
		brakeReading_wait.startTimer(50);
	}
	digitalWrite(BRAKELIGHT_PIN, brakePressed);
	Serial.print("Brake:\t\t");
	Serial.println(brakePressed);
}