#include "gpio.h"

PDU pdu;

GPIO::GPIO(){}


GPIO::GPIO(CascadiaMC *p_motorController, OrionBMS *p_bms, PDU *p_pdu)
{
    tsms = TSMS(SS_READY_SEN);

    motorController = p_motorController;
    bms = p_bms;
    pdu = p_pdu;

    pdu->enableAccFans(true, true);
}


GPIO::~GPIO(){}


bool GPIO::getTSMS()
{
    // Serial.println(tsms.isReady());
    return tsms.isReady();
}

void GPIO::handlePump()
{
    pdu->enableCoolingPump(motorController->getIsOn());
}

void GPIO::handleRadiatorFan()
{
    int16_t temp = motorController->getRadiatorTemp() / 10;
    // int16_t temp = 70; // For Testing
    if (temp > MAX_FANSPEED_TEMP) {
        pdu->enableRadiatorFan(temp);
        return;
    }

    if (drive_state == EFFICIENCY) {
        temp = temp * radiatorFanSpeedPercentage; 
    }
    pdu->enableRadiatorFan(temp);
}

void GPIO::setRadiatorFanPercentage(float speed)
{
    radiatorFanSpeedPercentage = speed;
}

uint8_t GPIO::getMotorFanDialPercentage()
{
    return radiatorFanSpeedPercentage * 100;
}