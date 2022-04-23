#include "nerduino.h"
#include "canMsgHandler.h"
#include <Arduino.h>

void canHandler_CANMSG_BMSSHUTDOWN(const CAN_message_t &msg)
{
  Serial.println("Message Received!");
}

void setup() {
  delay(3000);
  Serial.print("Uh");
  NERduino.begin();
}

void loop()
{
  XYZData_t xyzbuf[NUM_ADXL312_SAMPLES];
  HumidData_t humidbuf[3];

  NERduino.getADXLdata(xyzbuf, NUM_ADXL312_SAMPLES);
  Serial.println("Accelerometer Data:");
  for(uint8_t i=0; i<NUM_ADXL312_SAMPLES; i++)
  {
    Serial.print(xyzbuf[i].XData.data);
    Serial.print("\t");
    Serial.print(xyzbuf[i].YData.data);
    Serial.print("\t");
    Serial.print(xyzbuf[i].ZData.data);
    Serial.print("\t");
    Serial.println();
  }
 
  
  NERduino.getSHTdata(humidbuf,3);
  Serial.println("Humidity Data:");
  for(uint8_t i=0; i<3; i++)
  {
    Serial.print("Temperature C: ");
    Serial.print(humidbuf[i].tempC);
    Serial.println(" C");
    Serial.print("Temperature F: ");
    Serial.print(humidbuf[i].tempF);
    Serial.println(" F");
    Serial.print("Relative Humidity: ");
    Serial.print(humidbuf[i].relHumid);
    Serial.println(" %RH");
  }

  Serial.println("cycle...");

  delay(500);
}