/**
 * @file mpuCanHandlers.cpp
 * @author Nick DePatie
 * @date 2022-04-05
 */
#include "mpu.h"

/***************************************************************************/
/**
 * CAN Handlers specific to MPU
 */
/***************************************************************************/

/**
 * @brief CAN Message Handler for MC Message 1
 * 
 * @param msg
 */
void canHandler_CANMSG_MOTORTEMP1(const CAN_message_t &msg)
{
    //return;
    if (msg.id != 0xA0)
    {
    return;
    }

    Serial.print("MB "); Serial.print(msg.mb);
    Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
    Serial.print("  LEN: "); Serial.print(msg.len);
    Serial.print(" EXT: "); Serial.print(msg.flags.extended);
    Serial.print(" TS: "); Serial.print(msg.timestamp);
    Serial.print(" ID: "); Serial.print(msg.id, HEX);
    Serial.print(" Buffer: ");

    for ( uint8_t i = 0; i < msg.len; i++ ) 
    {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
    } Serial.println();
}

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

    int16_t currentDraw = (msg.buf[2] << 8) | msg.buf[3];

    mpu.bmsCurrentProcess(currentDraw);
}

void canHandler_CANMSG_BMSDTCSTATUS(const CAN_message_t &msg)
{
    mpu.setBMSAvgTemp(msg.buf[6]);
}

void canHandler_CANMSG_MC_SETPARAMETER(const CAN_message_t &msg)
{
}

void canHandler_CANMSG_MC_BMS_INTEGRATION(const CAN_message_t &msg)
{
    mpu.CANLineVerified();
    uint16_t dischargeCurrentLimit = (msg.buf[0] << 8) | msg.buf[1];
    mpu.setCurrentLimit(dischargeCurrentLimit);
}

void canHandler_CANMSG_MOTORMOTION(const CAN_message_t &msg)
{
    //angular motor speed is found at bytes 2 and 3
    int16_t motorSpeed = (msg.buf[2] << 8) | msg.buf[3];
    mpu.setMotorSpeed(motorSpeed);
}

void canHandler_CANMSG_BMSCHARGINGSTATE(const CAN_message_t &msg)
{
    mpu.enableBMSChargingMode();
}