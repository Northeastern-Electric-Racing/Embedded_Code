#include <Arduino.h>
#include "nerduino.h"

NERDUINO NERduino;


NERDUINO::NERDUINO()
{
    Wire.begin();
    Serial.begin(9600);
}


NERDUINO::~NERDUINO(){}


bool NERDUINO::begin()
{
    adxl312 = ADXL312();
    //amc6821 = AMC6821();
    sht30 = SHT30();
}


void NERDUINO::getADXLdata(XYZData_t *xyzbuf, uint8_t numReadings)
{
    uint8_t *msg = new uint8_t[6];
    for(uint8_t i=0;i<numReadings;i++)
    {
        adxl312.getXYZ(msg);

        xyzbuf[i].XData.rawdata[0] = msg[0];
        xyzbuf[i].XData.rawdata[1] = msg[1];
        xyzbuf[i].YData.rawdata[0] = msg[2];
        xyzbuf[i].YData.rawdata[1] = msg[3];
        xyzbuf[i].ZData.rawdata[0] = msg[4];
        xyzbuf[i].ZData.rawdata[1] = msg[5];
        delay(5);
    }
    delete[] msg;
}


void NERDUINO::getSHTdata(HumidData_t *humidbuf, uint8_t numReadings)
{
    uint8_t *msg = new uint8_t[6];
    for(uint8_t i=0;i<numReadings;i++)
    {
        sht30.getTempHumid(msg);

        humidbuf[i].TempData.rawdata[0]  = msg[1];
        humidbuf[i].TempData.rawdata[1]  = msg[0];
        humidbuf[i].HumidData.rawdata[0] = msg[4];
        humidbuf[i].HumidData.rawdata[1] = msg[3];
    }

    delete[] msg;
}