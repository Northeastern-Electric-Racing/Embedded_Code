#include "gpio.h"

GPIO::GPIO(CASCADIAMC *p_motorController)
{
    pinMode(SS_READY_SEN, INPUT);

    motorController = p_motorController;
}

GPIO::~GPIO(){}

void GPIO::handleMCHVFault()
{
    if (isMCFault && digitalRead(SS_READY_SEN) == HIGH) {
        motorController->clearFault();
        isMCFault = false;
  }
}