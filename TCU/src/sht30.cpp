#include "sht30.h"
#include <Arduino.h>

SHT30::SHT30()                                              //Constructor for the SHT30 class
{
    reset();
    Serial.println("SHT30 is reset.");
    Serial.println("Heating element is enabled: ");
    isHeaterEnabled();
}


SHT30::~SHT30(){}                                           //Destructor 


void SHT30::SHT30write(cmd_t *cmd, uint8_t numBytes)        //Allows for private I2C writing with SHT30
{
    Wire.beginTransmission(SHT30_I2C_ADDR);
    for(uint8_t i=0; i< numBytes; i++)
    {
        Wire.write(cmd->cmd2Bytes[i]);
    }
    Wire.endTransmission(false);
}


bool SHT30::SHT30read(uint8_t *msg, uint8_t numBytes)       //Allows for private I2C reading with SHT30
{
    Wire.requestFrom(SHT30_I2C_ADDR, (int)numBytes);
    if (Wire.available())
    {
        uint8_t i2cByte=0;
        while(Wire.available())
        {
            msg[i2cByte] = Wire.read();
            i2cByte++;
        }
        return true;
    }

    return false;
}


uint16_t SHT30::readStatusReg()                             //Reads the 16-bit status register
{
    cmd_t cmd = {.cmdVal = SWITCHBYTES(SHT30_READSTATUS)};

    SHT30write(&cmd, 2);
    uint8_t data[3];
    SHT30read(data, 3);
    uint16_t status = data[0];
    status <<= 8;
    status |= data[1];
    return status;
}


void SHT30::reset(void)                                     //Allows for a soft reset upon start up
{
    cmd_t cmd = {.cmdVal = SWITCHBYTES(SHT30_SOFTRESET)};
    SHT30write(&cmd, 2);
}


bool SHT30::isHeaterEnabled()                               //Check if the heating element is turned on
{
    uint16_t regVal = readStatusReg();
    return (bool) bitRead(regVal, SHT30_REG_HEATER_BIT);
}


void SHT30::enableHeater(bool h)                            //Allows for heating element control, bool = true->enables, false->disable. Clears built up condensation
{
    cmd_t cmd = {.cmdVal = h ? SWITCHBYTES(SHT30_HEATEREN) : SWITCHBYTES(SHT30_HEATERDIS)};
    SHT30write(&cmd, 2);
}


void SHT30::getTempHumid(uint8_t *msg)                              //Public function to start a measurement and print values
{
    cmd_t cmd = {.cmdVal = SWITCHBYTES(SHT30_START_CMD_WCS)};
    SHT30write(&cmd, 2);
    SHT30read(msg, 6);
}
