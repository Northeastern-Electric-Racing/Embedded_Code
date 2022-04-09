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
    Serial.print("BMSACCUMULATORSTATUS:\t");
    for(uint8_t i=0; i<8; i++)
    {
        Serial.print(msg.buf[i]);
        Serial.print(",");
    }
    Serial.println("");

    mpu.setBMSSoC(msg.buf[6]);
}

void canHandler_CANMSG_BMSDTCSTATUS(const CAN_message_t &msg)
{
    Serial.print("BMSDTCSTATUS:\t");
    for(uint8_t i=0; i<8; i++)
    {
        Serial.print(msg.buf[i]);
        Serial.print(",");
    }
    Serial.println("");
    mpu.setBMSAvgTemp(msg.buf[6]);
}

void canHandler_CANMSG_MC_SETPARAMETER(const CAN_message_t &msg)
{
    Serial.print("MC SET PARAMETERS:\t");
    for(uint8_t i=0; i<8; i++)
    {
        Serial.print(msg.buf[i]);
        Serial.print(",");
    }
    Serial.println("");
}