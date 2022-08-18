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
    sht30 = SHT30();
    return true;
}


void NERDUINO::getADXLdata(XYZData_t *xyzbuf, uint8_t numReadings)
{
    uint8_t msg[6];
    adxl312.getXYZ(msg);

    xyzbuf[0].XData.rawdata[0] = msg[0];
    xyzbuf[0].XData.rawdata[1] = msg[1];
    xyzbuf[0].YData.rawdata[0] = msg[2];
    xyzbuf[0].YData.rawdata[1] = msg[3];
    xyzbuf[0].ZData.rawdata[0] = msg[4];
    xyzbuf[0].ZData.rawdata[1] = msg[5];
}


void NERDUINO::getSHTdata(HumidData_t *humidbuf, uint8_t numReadings)
{
    uint8_t msg[6];
    sht30.getTempHumid(msg);

    humidbuf[0].TempData.rawdata[0]  = msg[1];
    humidbuf[0].TempData.rawdata[1]  = msg[0];
    humidbuf[0].HumidData.rawdata[0] = msg[4];
    humidbuf[0].HumidData.rawdata[1] = msg[3];
}