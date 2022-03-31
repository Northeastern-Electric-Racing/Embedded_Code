#include "mpu.h"


MPU::MPU()
{
    pedals = PEDALS(&motorController);
    driverio = DRIVERIO(&motorController);

    ioRead_wait.cancelTimer();
}


MPU::~MPU(){}


void MPU::driverioProcess()
{
    if(ioRead_wait.isTimerExpired())
    {
        Serial.println("DriverIO process...");
        driverio.handleSSButton();
        driverio.handleReverseSwitch();
        ioRead_wait.startTimer(100);
    }
}


void MPU::pedalsProcess()
{
    Serial.println("Pedals process...");
    pedals.readBrake();
    pedals.readAccel();
}

void MPU::sendMCMsg()
{
    motorController.writeMCState();
}

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