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
	if(!pedalReading_debounce.isTimerExpired())
	{
		int accelPin1Val = analogRead(ACCEL1_PIN);
		int accelPin2Val = analogRead(ACCEL2_PIN);
		
		if(pedalDiff > abs(accelPin1Val-accelPin2Val))
		{
			pedalDiff = abs(accelPin1Val-accelPin2Val);
			avgPedalVal = (accelPin1Val + accelPin2Val) / 2;
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
	else
	{
		int16_t flippedVal = (avgPedalVal * -1) + 1023; // reverse it so 0 is when pedal not pressed, and 1023 is at full press
		if (flippedVal < POT_LOWER_BOUND)
		{ // Set low point to prevent a positive torque in the resting pedal position
			flippedVal = 0;
		}
		double multiplier = ((double)flippedVal - 250)  / 650; // torque multiplier from 0 to 1;
		appliedTorque = calcTorque(multiplier);
		//Serial.println(appliedTorque);
		pedalReading_debounce.startTimer(40);
		pedalDiff = MAXIMUM_TORQUE;
	}
	motorController->changeTorque(appliedTorque);
	//Serial.print("Acceleration:\t");
	//Serial.println(appliedTorque / 10); // prints out applied torque

	pedalReading_wait.startTimer(25);

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
	/*
	Serial.print("Vdc: ");                                               
	Serial.print(dcVoltage);
	Serial.print(", Idc: ");
	Serial.print(dcCurrent);
	Serial.print(", wm: ");
	Serial.println(motorSpeed);
	
	Serial.print("CL Limit");
	Serial.println((CL_TO_TOQRUE_CONST * dcVoltage * dcCurrent) / (motorSpeed + 1));
	*/

	return (CL_TO_TOQRUE_CONST * dcVoltage * dcCurrent) / (motorSpeed + 1);
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
	if(!brakeReading_debounce.isTimerExpired())
	{
		int brake1Val = analogRead(BRAKE1_PIN);	//rear
		int brake2Val = analogRead(BRAKE2_PIN); //front
		/*
		Serial.print("brake1:\t");
		Serial.println(brake1Val);
		Serial.print("brake2:\t");
		Serial.println(brake2Val);
		Serial.println();
		*/
		int avgBrakeVal_tmp = (brake1Val > brake2Val) ? brake1Val : brake2Val;

		if(avgBrakeVal_tmp > avgBrakeVal)
		{
			avgBrakeVal = avgBrakeVal_tmp;
		}
	}
	else
	{
		//Serial.println(avgBrakeVal);
		if(avgBrakeVal > ANALOG_BRAKE_THRESH)
		{
			brakePressed = true;
		}
		// if brake is not being pressed, toggle brakePressed variable
		else
		{
			brakePressed = false;
		}
		brakeReading_debounce.startTimer(300);
		avgBrakeVal = 0;
	}
	if(brakePressed)
	{
		digitalWrite(BRAKELIGHT_PIN, HIGH);
		brakeLight_wait.startTimer(200);
	}
	if(brakeLight_wait.isTimerExpired())
	{
		digitalWrite(BRAKELIGHT_PIN,LOW);
	}
}