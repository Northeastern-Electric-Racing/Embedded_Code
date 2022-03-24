/**
 * @file sht30.h
 * @author Nick DePatie
 * @brief 
 */
#ifndef SHT30_H
#define SHT30_H

#include <Arduino.h>
#include <Wire.h>

//SHT30-DIS Humidity Sensor
/**
 * https://www.mouser.com/datasheet/2/682/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital-971521.pdf  --Datasheet
 * 
 */

#define SHT30_I2C_ADDR                 0x44     //If ADDR (pin2) is connected to VDD, 0x45

#define SHT30_START_CMD_WCS            0x2C06   //Start measurement command with clock streching enabled and high repeatability
#define SHT30_START_CMD_NCS            0x2400   //Start measurement command with clock streching disabled and high repeatability 
#define SHT30_READSTATUS               0xF32D   //Read Out of Status Register
#define SHT30_CLEARSTATUS              0x3041   //Clear Status
#define SHT30_SOFTRESET                0x30A2   //Soft Reset
#define SHT30_HEATEREN                 0x306D   //Heater Enable
#define SHT30_HEATERDIS                0x3066   //Heater Disable

#define SHT30_REG_HEATER_BIT           0x0d     //Status Register Heater Bit


class SHT30
{
    private:
        /**
         * @brief Allows for 16 bit commands to be easily formatted
         */
        typedef union
        {
            uint16_t cmdVal;
            uint8_t cmd2Bytes[2];
        }cmd_t;
    
        /**
         * @brief Writes the the SHT30 via I2C
         * 
         * @param msg 
         * @param numBytes 
         */
        void SHT30write(cmd_t *cmd, uint8_t numBytes);

        /**
         * @brief Requests data from the SHT30 via I2C
         * 
         * @param msg 
         * @param numBytes 
         * @return true 
         * @return false 
         */
        bool SHT30read(uint8_t *msg, uint8_t numBytes);


    public:
        /**
         * @brief Construct a new SHT30 object
         * 
         */
        SHT30();                                   

        /**
         * @brief Destroy the SHT30 object
         * 
         */
        ~SHT30();

        /**
         * @brief Reads the 16 bit status register
         * 
         * @return uint16_t 
         */
        uint16_t readStatusReg();

        /**
         * @brief Allows for a soft reset upon start up
         * 
         */
        void reset(void);

        /**
         * @brief Check if the heating element is turned on
         * 
         * @return true 
         * @return false 
         */
        bool isHeaterEnabled();

        /**
         * @brief Allows for heating element control, bool = true->enables, false->disable. Clears built up condensation
         * 
         * @param h 
         */
        void enableHeater(bool h);
 
        /**
         * @brief Retrieve humidity reading
         * 
         * @return true 
         * @return false 
         */
        bool getTempHumid(void);

};

#endif