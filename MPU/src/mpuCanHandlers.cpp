/**
 * @file mpuCanHandlers.cpp
 * @author Nick DePatie
 * @date 2022-04-05
 */
#include "mpu.h"

/***************************************************************************/
/**
 * CAN Handlers specific to MPU
 * @note Motor Controller is little Endian
 */
/***************************************************************************/

/**
 * @brief Handler for MC faults
 * 
 * @param msg
 */
void canHandler_CANMSG_ERR_MCFAULT(const CAN_message_t &msg)
{
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!! MC ERROR !!!!!!!!!!!!!!!!!!!!");
}


void canHandler_CANMSG_BMSACCSTATUS(const CAN_message_t &msg)
{
    mpu.setBMSSoC(msg.buf[6]);

    int16_t liveVoltage = (msg.buf[0] << 8) | msg.buf[1];
    liveVoltage = liveVoltage;

    mpu.setBMSVoltage(liveVoltage);
}

void canHandler_CANMSG_BMSDTCSTATUS(const CAN_message_t &msg)
{
    mpu.setBMSAvgTemp(msg.buf[6]);
}


void canHandler_CANMSG_BMSCURRENTLIMITS(const CAN_message_t &msg)
{
    mpu.CANLineVerified();
    /*
    uint16_t dischargeCurrentLimit = (msg.buf[1] << 8) | msg.buf[0];
    mpu.setCurrentLimit(dischargeCurrentLimit);
    uint16_t chargeCurrentLimit = (msg.buf[3] << 8) | msg.buf[2];
    mpu.setChargeCurrentLimit(chargeCurrentLimit);
    */
}


void canHandler_CANMSG_MOTORMOTION(const CAN_message_t &msg)
{
    //angular motor speed is found at bytes 2 and 3
    int16_t motorSpeed = ((msg.buf[3] << 8) | msg.buf[2]) / 10;
    Serial.print("Motorspeed: ");
    Serial.println(motorSpeed);
    mpu.setMotorSpeed(motorSpeed);
}

void canHandler_CANMSG_BMSCHARGINGSTATE(const CAN_message_t &msg)
{
    mpu.enableBMSChargingMode();
}

void canHandler_CANMSG_MOTORETEMP3(const CAN_message_t &msg)
{
    int16_t motorTemp = (msg.buf[5] << 8) | msg.buf[4];
    Serial.print("MotorTemp: ");
    Serial.println(motorTemp / 10);
    mpu.setMotorTemp(motorTemp);
}

void canHandler_CANMSG_BMSCURRENTS(const CAN_message_t &msg)
{
    mpu.CANLineVerified();
    uint16_t dischargeCurrentLimit = (msg.buf[0] << 8) | msg.buf[1];
    mpu.setCurrentLimit(dischargeCurrentLimit);
    uint16_t chargeCurrentLimit = (msg.buf[2] << 8) | msg.buf[3];
    mpu.setChargeCurrentLimit(chargeCurrentLimit);
    int16_t currentDraw = (msg.buf[4] << 8) | msg.buf[5];
    mpu.bmsCurrentProcess(currentDraw);
    Serial.print("CurrentDraw: ");
    Serial.println(currentDraw);
    //Serial.println(dischargeCurrentLimit);
    //6 and 7 are rolling avg current
}