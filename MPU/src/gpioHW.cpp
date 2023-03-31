#include "gpioHW.h"

PDU::PDU(){}

PDU::~PDU(){}

void PDU::sendPDUMsg()
{
    sendMessageCAN1(CANMSG_PDU_ID, 4, pdu.msg);
}

void PDU::enableRadiatorFan(bool status)
{
    pdu.fields.radiator_fan_dty = status;
    sendPDUMsg();
}

void PDU::enableAccFans(bool r_status, bool l_status)
{
    pdu.fields.right_acc_fan = r_status;
    pdu.fields.left_acc_fan = l_status;
    sendPDUMsg();
}

void PDU::enableCoolingPump(bool status)
{
    pdu.fields.cooling_pump = status;
    sendPDUMsg();
}

void PDU::enableBrakeLight(bool status)
{
    pdu.fields.brake_light = status;
    sendPDUMsg();
}

TSMS::TSMS(){}

TSMS::TSMS(uint8_t pinNum)
{
    pin = pinNum;
    pinMode(pin, INPUT);
}

TSMS::~TSMS(){}

bool TSMS::isReady()
{
    bool reading = digitalRead(pin);

    //If the previous reading was HIGH and the current reading is low, we cycled power
    if(!reading && prevReading)
    {
        isTSCycled = true;
    }

    prevReading = reading;
    return reading;
}

bool TSMS::isPowerCycled()
{
    bool returnStatus = isTSCycled;
    if(isTSCycled) isTSCycled = false;
    return returnStatus;
}

