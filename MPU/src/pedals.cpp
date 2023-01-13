#include "pedals.h"


Pedals::Pedals(){}


Pedals::Pedals(CascadiaMC *p_motorController, OrionBMS *p_bms)
{
	pinMode(BRAKELIGHT_PIN, OUTPUT);
	digitalWrite(BRAKELIGHT_PIN, HIGH);

	motorController = p_motorController;
	bms = p_bms;

	accelerator = PedalHW(ACCELERATOR_ERROR_PERCENT, MAX_ACCEL_ERRORS, accelPins);
	brakes = PedalHW(BRAKES_ERROR_PERCENT, MAX_BRAKE_ERRORS, brakePins);
	brakeLight = BRAKELIGHT_HW(BRAKELIGHT_PIN);
}


Pedals::~Pedals(){}


FaultStatus_t Pedals::readAccel()
{
	//Begin or continue the pedal reading process
	uint16_t pedalVal = accelerator.readValue();

	//If the pedal reading process is NOT finished, return NOT_FAULTED because it hasn't finished collecting data
	if(pedalVal == NOT_DONE_READING) return NOT_FAULTED;

	//If the pedal reading process has detected a FAULT, return FAULT
	if(accelerator.isFaulted() == FAULTED) return FAULTED;

	//Calculate percentage value for how far down the accelerator is pressed
	int16_t flippedVal = (pedalVal * -1) + 1023; // reverse it so 0 is when pedal not pressed, and 1023 is at full press

	//Cleansing Value
	//Note: POT_LOWER_BOUND is nonzero to allow for some mechanical give while not exerting torque
	if(flippedVal < POT_LOWER_BOUND) flippedVal = 0;
	if(flippedVal > POT_UPPER_BOUND) flippedVal = POT_UPPER_BOUND;

	double multiplier = ((double)flippedVal - ACCEL_OFFSET)  / 650; // torque multiplier from 0 to 1;

	//Calculate the correct amount of torque to command using precentage pressed value
	appliedTorque = calcTorque(multiplier);

	//Load the commanded torque to be sent to the motor controller
	motorController->changeTorque(appliedTorque);

#ifdef DEBUG_PEDALS
	Serial.print("Acceleration:\t");
	Serial.println(appliedTorque / 10); // prints out applied torque
#endif
	//Return the current fault status of the accelerator, send fault if it wasn't caught earlier
	return accelerator.isFaulted();
}


FaultStatus_t Pedals::readBrake()
{
	//Begin or continue the pedal reading process
	uint16_t pedalVal = brakes.readValue();

	uint8_t canBuf[2] = {pedalVal >> 8, pedalVal & 0xFF};

	sendMessage(0xB1, 2, canBuf);

	//If the pedal reading process is NOT finished, return NOT_FAULTED because it hasn't finished collecting data
	if(pedalVal == NOT_DONE_READING) return NOT_FAULTED;

	//If the pedal reading process has detected a FAULT, return FAULT
	if(brakes.isFaulted() == FAULTED) return FAULTED;

	//Set brakePressed to whether or not the brake value is greater than the braking threshold
	brakePressed = pedalVal > ANALOG_BRAKE_THRESH ? HIGH : LOW;

	brakeLight.writeBrakeLight(brakePressed);

	return brakes.isFaulted();
}


int16_t Pedals::calcTorque(double torqueScale)
{
	int16_t pedalTorque = 0;

	pedalTorque = torqueScale * MAXIMUM_TORQUE;

	int16_t torqueLim = calcCLTorqueLimit();

	// Scale torque to match BMS current limit
	if (pedalTorque > torqueLim) {
		pedalTorque = torqueLim;
	}

	//Cleansing Value just in case (Somewhat redundant)
	if(pedalTorque >= MAXIMUM_TORQUE)
	{
		pedalTorque = MAXIMUM_TORQUE;
	}
	else if(pedalTorque < 0)
	{
		pedalTorque = 0;
	}

	//Serial.print("Pedal: ");
	//Serial.println(pedalTorque);
	//Serial.print("C Limit:");
	//Serial.println(torqueLim);

	return pedalTorque;
}


int16_t Pedals::calcCLTorqueLimit()
{
	int16_t dcVoltage = abs(bms->getLiveVoltage());
	int16_t dcCurrent = bms->getCurrentLimit();
	int16_t motorSpeed = abs(motorController->getMotorSpeed());

	int16_t calculated = 102;

	//calculated = (0.9 * (CL_TO_TOQRUE_CONST * (dcVoltage / 10) * dcCurrent)) / (motorSpeed + 1);
	calculated = (0.9 * (CL_TO_TOQRUE_CONST * (dcVoltage / 10) * dcCurrent)) / (500 + 1);

	if ((calculated < 0) | (calculated > (MAXIMUM_TORQUE / 10))) {
		calculated = MAXIMUM_TORQUE / 10;
	}
	
	Serial.print("Vdc: ");
	Serial.print(dcVoltage);
	Serial.print(", Idc: ");
	Serial.print(dcCurrent);
	Serial.print(", wm: ");
	Serial.println(motorSpeed);
	
	Serial.print("CL Limit: ");
	Serial.print(calculated);

	Serial.print(", ");
	Serial.println(10 * calculated);

	return calculated * 10;
}


int16_t Pedals::calcCLRegenLimit()
{
	int16_t dcVoltage = abs(bms->getLiveVoltage());
	int16_t dcCurrent = bms->getChargeCurrentLimit();
	int16_t motorSpeed = abs(motorController->getMotorSpeed());

	return (CL_TO_TOQRUE_CONST * dcVoltage * dcCurrent) / (motorSpeed + 1);
}