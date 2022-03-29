#include <nerduino.h>
#include "mpu.h"

void setup() {

  NERduino.begin();
  initializeCAN(1);   //The "1" parameter is useless for now, in the future the parameter is which CAN line to initialize
  
}

MPU mpu;

void loop() {
  // put your main code here, to run repeatedly:
  mpu.driverioProcess();
  mpu.pedalsProcess();
}