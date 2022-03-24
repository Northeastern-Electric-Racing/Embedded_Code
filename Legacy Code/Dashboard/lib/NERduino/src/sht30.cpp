#include "sht30.h"

SHT30::SHT30()                                            //Constructor for the SHT30 class
    {
        //reset();
        Serial.println("SHT30 is reset.");
        Serial.println("Heating element is enabled: ");
        //isHeaterEnabled();
    }


SHT30::~SHT30(){}                                          //Destructor 


void SHT30::SHT30write(cmd_t *cmd, uint8_t numBytes)    //Allows for private I2C writing with SHT30
{
    Wire.beginTransmission(SHT30_I2C_ADDR);
    for(uint8_t i=0; i< numBytes; i++)
    {
        Wire.write(cmd->cmd2Bytes[i]);
    }
    Wire.endTransmission(false);
}


bool SHT30::SHT30read(uint8_t *msg, uint8_t numBytes)     //Allows for private I2C reading with SHT30
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


uint16_t SHT30::readStatusReg()                       //Reads the 16-bit status register
{
    cmd_t cmd = {.cmdVal = SHT30_READSTATUS};

    SHT30write(&cmd, 2);
    uint8_t data[3];
    SHT30read(data, 3);
    uint16_t status = data[0];
    status <<= 8;
    status |= data[1];
    return status;
}


void SHT30::reset(void)                                    //Allows for a soft reset upon start up
{
    cmd_t cmd = {.cmdVal = SHT30_SOFTRESET};
    SHT30write(&cmd, 2);
}


bool SHT30::isHeaterEnabled()                             //Check if the heating element is turned on
{
    uint16_t regVal = readStatusReg();
    return (bool) bitRead(regVal, SHT30_REG_HEATER_BIT);
}


void SHT30::enableHeater(bool h)                                 //Allows for heating element control, bool = true->enables, false->disable. Clears built up condensation
{
    cmd_t cmd = {.cmdVal = h ? SHT30_HEATEREN : SHT30_HEATERDIS};
    SHT30write(&cmd, 2);
}

   
bool SHT30::getTempHumid(void)                             //Public function to start a measurement and print values
{
    cmd_t cmd = {.cmdVal = SHT30_START_CMD_WCS};
    uint8_t msg[6];
    SHT30write(&cmd, 2);
    SHT30read(msg, 6);

    float cTemp = ((((msg[0] * 256.0) + msg[1]) * 175) / 65535.0) - 45;
    float fTemp = (cTemp * 1.8) + 32;
    float humidity = ((((msg[3] * 256.0) + msg[4]) * 100) / 65535.0);
    Serial.print("Temperature C: ");
    Serial.print(cTemp);
    Serial.println(" C");
    Serial.print("Temperature F: ");
    Serial.print(fTemp);
    Serial.println(" F");
    Serial.print("Relative Humidity: ");
    Serial.print(humidity);
    Serial.println(" %RH");
    return true;
}
