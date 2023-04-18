/**
 * @file mpuCanHandlers.cpp
 * @author Nick DePatie
 * @date 2022-04-05
 */
#include "mpu.h"

#define CANMSG_WHEELIO      0x400
#define CANMSG_BMSSTATUS    0x002

/***************************************************************************/
/**
 * CAN Handlers specific to MPU
 * Functions are called in /lib/NERduino/canMsgHandler.cpp
 * @note Motor Controller is little Endian
 */
/***************************************************************************/


/**
 * @brief Handler for MC faults
 * 
 * @param msg
 */
void mcErr_cb(const CAN_message_t &msg)
{
    //Serial.println("!!!!!!!!!!!!!!!!!!!!!!! MC ERROR !!!!!!!!!!!!!!!!!!!!");
}

void bmsAccStatus_cb(const CAN_message_t &msg)
{
    bms.setSoC(msg.buf[6]);

    int16_t liveVoltage = (msg.buf[0] << 8) | msg.buf[1];
    liveVoltage = liveVoltage;

    bms.setLiveVoltage(liveVoltage);
}

void bmsStatus_cb(const CAN_message_t &msg)
{
    bms.setBMSStatus(msg.buf[0]);
    bms.setFaultStatus((msg.buf[1]) << 24 | (msg.buf[2]) << 16 | (msg.buf[3]) << 8 | (msg.buf[4]));
    bms.setAvgTemp(msg.buf[5]);
}


void bmsCurrentLimits_cb(const CAN_message_t &msg)
{
    setCANLineOK();
    uint16_t dischargeCurrentLimit = (msg.buf[1] << 8) | msg.buf[0];
    bms.setCurrentLimit(dischargeCurrentLimit);
    uint16_t chargeCurrentLimit = (msg.buf[3] << 8) | msg.buf[2];
    bms.setChargeCurrentLimit(chargeCurrentLimit);
}


void motorMotion_cb(const CAN_message_t &msg)
{
    //angular motor speed is found at bytes 2 and 3
    int16_t motorSpeed = (int16_t)(msg.buf[3] << 8) | msg.buf[2];

    motorController.setMotorSpeed(-motorSpeed);
}

void motorTemp3_cb(const CAN_message_t &msg)
{
    int16_t motorTemp = (msg.buf[5] << 8) | msg.buf[4];
    motorController.setRadiatorTemp(motorTemp);
}

void bmsCurrents_cb(const CAN_message_t &msg)
{
    setCANLineOK();

    uint16_t dischargeCurrentLimit = (msg.buf[1] << 8) | msg.buf[0];
    bms.setCurrentLimit(dischargeCurrentLimit);

    uint16_t chargeCurrentLimit = (msg.buf[3] << 8) | msg.buf[2];
    bms.setChargeCurrentLimit(chargeCurrentLimit);

    int16_t currentDraw = (msg.buf[4] << 8) | msg.buf[5];
    bms.setCurrentDraw(currentDraw);

    //Serial.print("CurrentDraw: ");
    //Serial.println(currentDraw);
    //Serial.println(dischargeCurrentLimit);
    //6 and 7 are rolling avg current
}

void mpuCanCallback(const CAN_message_t &msg)
{
    switch(msg.id)
    {    //using a switch statement for set CAN IDs
        case CANMSG_MOTORETEMP3:
            motorTemp3_cb(msg);
            break;
        case CANMSG_MOTORMOTION:
            motorMotion_cb(msg);
            break;
        case CANMSG_ERR_MCFAULT:
            mcErr_cb(msg);
            break;
        case CANMSG_BMSSTATUS:
            bmsStatus_cb(msg);
            break;
        case CANMSG_BMSACCSTATUS:
            bmsAccStatus_cb(msg);
            break;
        case CANMSG_BMSCURRENTLIMITS:
            bmsCurrentLimits_cb(msg);
            break;
        case CANMSG_BMSCURRENTS:
            bmsCurrents_cb(msg);
            break;
        case CANMSG_WHEELIO:
            driverio.wheelIO_cb(msg);
        default:
            break;
    }
}