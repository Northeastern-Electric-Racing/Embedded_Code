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


bool GPIO::handleTSMS()
{
    // Serial.println(tsms.isReady());
    if(!tsms.isReady())
    {
        motorController->raiseFault();
        return false;
    }
    if(tsms.isPowerCycled() && tsms.isReady())
    {
        motorController->clearFault();
        Serial.println("Clearing!");
        return true;
    }
    return false;
}

void GPIO::handlePump()
{
    pdu->enableCoolingPump(motorController->getIsOn());
}

void GPIO::handleRadiatorFan()
{
    int16_t temp = motorController->getRadiatorTemp() / 10;

    pdu->enableRadiatorFan(temp > MAX_FANSPEED_TEMP);
}