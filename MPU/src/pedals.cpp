#include "pedals.h"


Pedals::Pedals(){}


Pedals::Pedals(CascadiaMC *p_motorController, OrionBMS *p_bms, PDU *p_pdu)
{
	motorController = p_motorController;
	bms = p_bms;
	pdu = p_pdu;

	pdu->enableBrakeLight(brakePressed);

	accelerator = PedalHW(ACCELERATOR_ERROR_PERCENT, MAX_ACCEL_ERRORS, accelPins);
	brakes = PedalHW(BRAKES_ERROR_PERCENT, MAX_BRAKE_ERRORS, brakePins);
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
	// Serial.print("Motor Speed:\t");
	// Serial.print(mph);
	// Serial.print("\t");
	// Serial.print("Acceleration:\t");
	// Serial.print(appliedTorque); // prints out applied torque
	// Serial.print("Accumulator Values:\t");
	// for (int i = 0; i < ACCUMULATOR_SIZE; i++) {
	// 	Serial.print(torqueAccumulator[i]);
	// 	Serial.print("\t");
	// }

#endif
	//Return the current fault status of the accelerator, send fault if it wasn't caught earlier
	return accelerator.isFaulted();
}


FaultStatus_t Pedals::readBrake()
{
	//Begin or continue the pedal reading process
	uint16_t pedalVal = brakes.readValue();

	uint8_t canBuf[2] = {pedalVal >> 8, pedalVal & 0xFF};

	sendMessageCAN1(0xB1, 2, canBuf);

	//If the pedal reading process is NOT finished, return NOT_FAULTED because it hasn't finished collecting data
	if(pedalVal == NOT_DONE_READING) return NOT_FAULTED;

	//If the pedal reading process has detected a FAULT, return FAULT
	if(brakes.isFaulted() == FAULTED) return FAULTED;

	//Set brakePressed to whether or not the brake value is greater than the braking threshold
	brakePressed = pedalVal > ANALOG_BRAKE_THRESH ? HIGH : LOW;

	pdu->enableBrakeLight(brakePressed);

	return brakes.isFaulted();
}


int16_t Pedals::calcTorque(double torqueScale)
{
	int16_t pedalTorque = 0;

	pedalTorque = torqueScale * MAXIMUM_TORQUE;

	int16_t torqueLim = calcCLTorqueLimit(); //2200

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

	float mph = fabs((motorController->getMotorSpeed() * MOTOR_RPM_TO_MPH_CONST));
	int16_t regenTorqueLim = calcCLRegenLimit();
	if (drive_state == PIT || drive_state == REVERSE) {
		uint16_t newVal;
		//Results in a value from 0.5 to 0 (at least halving the max torque at all times in pit or reverse)
		if (mph > PIT_MAX_SPEED) {
			newVal = 0;
		}
		else {
			float torque_derating_factor = fabs(0.5 + ((-0.5/PIT_MAX_SPEED) * mph));
			newVal = pedalTorque * torque_derating_factor;
		}
		uint16_t ave = 0;
		uint16_t temp[ACCUMULATOR_SIZE];
		std::copy_n(torqueAccumulator, ACCUMULATOR_SIZE, temp);
		for (int i = 0; i < ACCUMULATOR_SIZE - 1; i++) {
			temp[i + 1] = torqueAccumulator[i];
			ave += torqueAccumulator[i+1];
		}
		ave += newVal;
		ave /= ACCUMULATOR_SIZE;
		temp[0] = newVal;
		if(pedalTorque > ave) {
			pedalTorque = ave;
		}
		std::copy_n(temp, ACCUMULATOR_SIZE, torqueAccumulator);
	}
   else if (drive_state == EFFICIENCY) {
		pedalTorque = pedalTorque * torqueLimitPercentage;
		if (pedalTorque == 0 && mph > 5) {
			uint32_t currTime = millis();
			if (!regenActive) {
				regenStartTime = currTime;
				regenActive = true;
			}
			if (currTime - regenStartTime < REGEN_RAMP_TIME) {
				pedalTorque = (((float)currTime - (float)regenStartTime) / REGEN_RAMP_TIME) * -1 * (min(REGEN_STRENGTHS[regenLevel], regenTorqueLim));
			} else {
				pedalTorque = -1 * min(REGEN_STRENGTHS[regenLevel], regenTorqueLim);
			}
		} else {
			regenStartTime = 0;
			regenActive = false;
		}
	}

	Serial.print("Pedal: ");
	Serial.println(pedalTorque);
	Serial.print("C Limit:");
	Serial.println(torqueLim);
	Serial.print("Torque Limit Percentage");
	Serial.println(torqueLimitPercentage);
	Serial.print("Regen Torque Limit:");
	Serial.println(regenTorqueLim);
	Serial.print("Regen Level:");
	Serial.println(REGEN_STRENGTHS[regenLevel]);
	Serial.print("MPH: ");
	Serial.println(mph);


	return pedalTorque;
}


int16_t Pedals::calcCLTorqueLimit()
{
	int16_t dcVoltage = abs(bms->getLiveVoltage());
	int16_t dcCurrent = bms->getCurrentLimit();
	int16_t motorSpeed = abs(motorController->getMotorSpeed());

	int16_t calculated = 102;

	calculated = (0.9 * (7.84 * (dcVoltage / 10) * dcCurrent)) / (500 + 1);
	// calculated = (dcCurrent * dcVoltage * sqrt(3)) / (motorSpeed * CL_TO_TOQRUE_CONST);

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
	Serial.println(10 * calculated);
	*/
	return calculated * 10;
}


int16_t Pedals::calcCLRegenLimit()
{
	int16_t dcVoltage = abs(bms->getLiveVoltage());
	int16_t dcChargeCurrent = abs(bms->getChargeCurrentLimit());
	int16_t motorSpeed = abs(motorController->getMotorSpeed());

	Serial.print("Vdc: ");
	Serial.println(dcVoltage);
	Serial.print("dcChargeCurrent: ");
	Serial.println(dcChargeCurrent);

	return (7.84 * dcVoltage * dcChargeCurrent) / (500 + 1);
}

uint8_t Pedals::getTorqueLimitPercentage()
{
	return torqueLimitPercentage * 100;
}

void Pedals::setTorqueLimitPercentage(float percentage)
{
	torqueLimitPercentage = percentage;
}

void Pedals::incrRegenLevel()
{
	switch (regenLevel)
	{
	case ZILCH:
		regenLevel = LIGHT;
		break;
	case LIGHT:
		regenLevel = MEDIUM;
		break;
	case MEDIUM:
		regenLevel = STRONG;
		break;
	case STRONG:
		regenLevel = ZILCH;
		break;
	default:
		regenLevel = ZILCH;
		break;
	}
}

uint8_t Pedals::getRegenLevel()
{
	return regenLevel;
}