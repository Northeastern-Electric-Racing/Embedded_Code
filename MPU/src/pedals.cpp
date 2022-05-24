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
	torqueBoost_time.cancelTimer();
    torqueBoost_cooldown.cancelTimer();

	motorController = p_motorController;
	bms = p_bms;
}


PEDALS::~PEDALS(){}


bool PEDALS::readAccel()
{
	if(!pedalReading_debounce.isTimerExpired())
	{
		int accelPin1Val = analogRead(ACCEL1_PIN);
		int accelPin2Val = analogRead(ACCEL2_PIN);

		// Iteratively narrow towards value
		if(pedalDiff > abs(accelPin1Val-accelPin2Val))
		{
			pedalDiff = abs(accelPin1Val-accelPin2Val);
			avgPedalVal = (accelPin1Val + accelPin2Val) / 2;
		}

		// Check for out of range values and errors
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
	else
	{
		int16_t flippedVal = (avgPedalVal * -1) + 1023; // reverse it so 0 is when pedal not pressed, and 1023 is at full press
		if (flippedVal < POT_LOWER_BOUND)
		{ // Set low point to prevent a positive torque in the resting pedal position
			flippedVal = 0;
		}
		double multiplier = ((double)flippedVal - 250)  / 650; // torque multiplier from 0 to 1;
		appliedTorque = calcTorque(multiplier);
		
		// Reset debounce and error checking params
		pedalReading_debounce.startTimer(40);
		accelErrors = 0;
		pedalDiff = MAXIMUM_TORQUE;
	}
	motorController->changeTorque(appliedTorque);
	//Serial.print("Acceleration:\t");
	//Serial.println(appliedTorque / 10); // prints out applied torque

	pedalReading_wait.startTimer(15);

	return accelFault;
}


int16_t PEDALS::calcTorque(double torqueScale)
{
	int16_t pedalTorque = 0;

	if (torqueBoostReady | torqueBoosting) {
		pedalTorque = torqueScale * MAXIMUM_TORQUE;
	} else {
		pedalTorque = torqueScale * CONT_TORQUE;
		if (pedalTorque > CONT_TORQUE) {
			pedalTorque = CONT_TORQUE;
		}
	}
	
	int16_t torqueLim = calcCLTorqueLimit();

	/*
	Serial.print("Pedal: ");
	Serial.println(pedalTorque);
	Serial.print("C Limit:");
	Serial.println(torqueLim);*/

	// Scale torque to match BMS current limit
	if (pedalTorque > torqueLim) {
		pedalTorque = torqueLim;
	}

	//Cleansing Value
	if(pedalTorque >= MAXIMUM_TORQUE)
	{
		pedalTorque = MAXIMUM_TORQUE;
	}
	else if(pedalTorque < 0)
	{
		pedalTorque = 0;
	}

	/*
	Serial.print("Boosting: ");
	Serial.println(torqueBoosting);
	Serial.print("Boost Ready: ");
	Serial.println(torqueBoostReady);*/

	if ((pedalTorque > CONT_TORQUE) & torqueBoostReady) {
		torqueBoost_time.startTimer(3000);
		torqueBoosting = true;
		torqueBoostReady = false;
	} else if (torqueBoost_time.isTimerExpired() & torqueBoosting) {
		torqueBoost_cooldown.startTimer(20000);
		torqueBoosting = false;
	} else if (torqueBoost_cooldown.isTimerExpired() & !torqueBoosting) {
		torqueBoostReady = true;
	}

	return pedalTorque;
}


int16_t PEDALS::calcCLTorqueLimit()
{
	int16_t dcVoltage = abs(bms->getLiveVoltage());
	int16_t dcCurrent = bms->getCurrentLimit();
	int16_t motorSpeed = abs(motorController->getMotorSpeed());

	int16_t calculated = 102;

	calculated = (0.9 * (CL_TO_TOQRUE_CONST * (dcVoltage / 10) * dcCurrent)) / (motorSpeed + 1);

	if ((calculated < 0) | (calculated > (MAXIMUM_TORQUE / 10))) {
		calculated = MAXIMUM_TORQUE / 10;
	}
	
	/*
	Serial.print("Vdc: ");                                               
	Serial.print(dcVoltage);
	Serial.print(", Idc: ");
	Serial.print(dcCurrent);
	Serial.print(", wm: ");
	Serial.println(motorSpeed);
	
	Serial.print("CL Limit: ");
	Serial.print(calculated);

	Serial.print(", ");
	Serial.println(calculated);*/

	return calculated * 10;
}


int16_t PEDALS::calcCLRegenLimit()
{
	int16_t dcVoltage = abs(bms->getLiveVoltage());
	int16_t dcCurrent = bms->getChargeCurrentLimit();
	int16_t motorSpeed = abs(motorController->getMotorSpeed());

	return (CL_TO_TOQRUE_CONST * dcVoltage * dcCurrent) / (motorSpeed + 1);
}


void PEDALS::readBrake()
{
	if(brakeReading_debounce.isTimerExpired())
	{
		int brake1Val = analogRead(BRAKE1_PIN);	//rear
		int brake2Val = analogRead(BRAKE2_PIN); //front
		
		int brakeAvg = (brake1Val + brake2Val) / 2;

		if (brakeAvg > ANALOG_BRAKE_THRESH) {
			digitalWrite(BRAKELIGHT_PIN, HIGH);
		} else {
			digitalWrite(BRAKELIGHT_PIN, LOW);
		}

		brakeReading_debounce.startTimer(250);
	}
}