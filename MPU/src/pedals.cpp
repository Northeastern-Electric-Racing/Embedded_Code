#include "pedals.h"


PEDALS::PEDALS(){}


PEDALS::PEDALS(CASCADIAMC *p_motorController, ORIONBMS *p_bms)
{
	pinMode(BRAKELIGHT_PIN, OUTPUT);
	digitalWrite(BRAKELIGHT_PIN, HIGH);

	brakeReading_wait.cancelTimer();
	pedalReading_wait.cancelTimer();
	pedalReading_debounce.cancelTimer();
	brakeLight_wait.cancelTimer();

	motorController = p_motorController;
	bms = p_bms;
}


PEDALS::~PEDALS(){}


bool PEDALS::readAccel()
{
	if(!pedalReading_wait.isTimerExpired()){return accelFault;}
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
		uint16_t pedalDiff = MAXIMUM_TORQUE;
		int16_t avgVal;
		pedalReading_debounce.startTimer(50);
		while(!pedalReading_debounce.isTimerExpired())
		{
			int accelPin1Val = analogRead(ACCEL1_PIN);
			int accelPin2Val = analogRead(ACCEL2_PIN);
			
			if(pedalDiff > abs(accelPin1Val-accelPin2Val))
			{
				pedalDiff = abs(accelPin1Val-accelPin2Val);
				avgVal = (accelPin1Val + accelPin2Val) / 2;
			}

			if (accelPin1Val == 0 ||
				accelPin2Val == 0 ||
				accelPin1Val == 1023 ||
				accelPin2Val == 1023 ||
				(abs(accelPin1Val - accelPin2Val) > (1023 * ACCELERATOR_ERROR_PER)))
			{
				accelErrors++;
				if(accelErrors >= MAX_ACCEL_ERRORS)
				{
					accelFault = true;
				}
			}
		}

		int16_t flippedVal = (avgVal * -1) + 1023; // reverse it so 0 is when pedal not pressed, and 1023 is at full press
		if (flippedVal < POT_LOWER_BOUND)
		{ // Set low point to prevent a positive torque in the resting pedal position
			flippedVal = 0;
		}

		double multiplier = (double)flippedVal / 950; // torque multiplier from 0 to 1;
		Serial.println(multiplier);
		appliedTorque = calcTorque(multiplier);
		Serial.println(appliedTorque);
	}

	motorController->changeTorque(appliedTorque);
	Serial.print("Acceleration:\t");
	Serial.println(appliedTorque / 10); // prints out applied torque

	pedalReading_wait.startTimer(50);

	if(accelFault)
	{
		Serial.println("ACCELFAULT");
	}

	return accelFault;
}


int16_t PEDALS::calcTorque(double torqueScale)
{
	int16_t pedalTorque = torqueScale * MAXIMUM_TORQUE;
	int16_t torqueLim = 10 * calcCLTorqueLimit();

	//scale torque if the BMS is leaving the boosting state
	if(bms->isLeavingBoosting() || !bms->isBoostReady())
	{
		if (pedalTorque > torqueLim) {
			pedalTorque = torqueLim;
		}
	}

	//Cleansing Value
	if(pedalTorque >= MAXIMUM_TORQUE)
	{
		pedalTorque = MAXIMUM_TORQUE;
		return pedalTorque;
	}
	if(pedalTorque < 0)
	{
		pedalTorque = 0;
		return pedalTorque;
	}

	return pedalTorque;
}


int16_t PEDALS::calcCLTorqueLimit()
{
	int16_t dcVoltage = abs(bms->getLiveVoltage());
	int16_t dcCurrent = bms->getCurrentLimit();
	int16_t motorSpeed = abs(motorController->getMotorSpeed());
	Serial.print("Vdc: ");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	Serial.print(dcVoltage);
	Serial.print(", Idc: ");
	Serial.print(dcCurrent);
	Serial.print(", wm: ");
	Serial.println(motorSpeed);

	Serial.print("CL Limit");
	Serial.println((CL_TO_TOQRUE_CONST * dcVoltage * dcCurrent) / (motorSpeed + 1));

	return (CL_TO_TOQRUE_CONST * dcVoltage * dcCurrent) / (motorSpeed + 1);
}


void PEDALS::readBrake()
{
	if(!brakeReading_wait.isTimerExpired()){return;}

	int brake1Val = analogRead(BRAKE1_PIN);
	int brake2Val = analogRead(BRAKE2_PIN);

	// if the brake is being pressed
	if (brake1Val > ANALOG_BRAKE_THRESH || brake2Val > ANALOG_BRAKE_THRESH)
	{
		brakeReading_debounce.startTimer(50);
		while (!brakeReading_debounce.isTimerExpired())
		{
			if((analogRead(BRAKE1_PIN) < ANALOG_BRAKE_THRESH) || (analogRead(BRAKE2_PIN) < ANALOG_BRAKE_THRESH))
			{
				brakeReading_debounce.cancelTimer();
				break;
			}
		}
		if(brake1Val || brake2Val)
		{
			if (!brakePressed)
			{ // if brake was not already being pressed, set new press time
				timeBrake = millis();
			}
			brakePressed = true;
		}
	}
	// if brake is not being pressed, toggle brakePressed variable
	else
	{
		brakePressed = false;
	}
	brakeReading_wait.startTimer(50);

	if(brakePressed)
	{
		digitalWrite(BRAKELIGHT_PIN, HIGH);
		brakeLight_wait.startTimer(200);
	}
	if(brakeLight_wait.isTimerExpired())
	{
		digitalWrite(BRAKELIGHT_PIN,LOW);
	}
	// Serial.print("Brake:\t\t");
	// Serial.println(brakePressed);
}