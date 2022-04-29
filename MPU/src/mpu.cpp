#include "mpu.h"

MPU mpu;

MPU::MPU()
{
    pedals = PEDALS(&motorController, &bms);
    driverio = DRIVERIO(&motorController, &bms);
    gpio = GPIO(&motorController, &bms);

    ioRead_wait.cancelTimer();
    canTest_wait.cancelTimer();
    pinMode(RELAY_PIN, OUTPUT);
    writeFaultLatch(FAULT_OK);
}


MPU::~MPU(){}


void MPU::driverioProcess()
{
    if(!ioRead_wait.isTimerExpired()){return;}
    
    // Serial.println("DriverIO process...");
    driverio.handleSSButton();
    driverio.handleSSLED();
    driverio.handleReverseSwitch();
    driverio.handleErrorLights();
    ioRead_wait.startTimer(100);
}


void MPU::pedalsProcess()
{
    isShutdown = verifyMotorSpinning();
    // Serial.println("Pedals process...");
    pedals.readBrake();
    isShutdown = pedals.readAccel();
}


void MPU::gpioProcess()
{
    gpio.handleMCHVFault();
    gpio.handlePump();
    gpio.handleRadiatorFan();
    isShutdown = !isCANLineOK();
    Serial.println(!isCANLineOK());
}


void MPU::sendMCMsg()
{
    motorController.writeMCState();
}


void MPU::setBMSAvgTemp(uint8_t p_avgTemp)
{
    bms.setAvgTemp(p_avgTemp);
}


void MPU::setBMSSoC(uint8_t p_soc)
{
    bms.setSoC(p_soc);
}


bool MPU::isCANLineOK()
{
    if(canTest_wait.isTimerExpired())
    {
        Serial.println("CAN FUCKED@$#^!$^@$#&");
        return false;
    }
    return true;
}


void MPU::CANLineVerified()
{
    canTest_wait.startTimer(500);
}


void MPU::checkShutdownStatus()
{
    Serial.println(isShutdown);
    if(isShutdown)
    {
        shutOffCar();
    }
    else
    {
        writeFaultLatch(FAULT_OK);
    }
}


void MPU::shutOffCar()
{
    writeFaultLatch(TRIGGER_FAULT);
    motorController.emergencyShutdown();
    Serial.println("Shutting off Car!!!!!");
    while(1){}
    writeFaultLatch(FAULT_OK);
}


void MPU::writeFaultLatch(bool status)
{
    digitalWrite(RELAY_PIN, status);
}


void MPU::setCurrentLimit(uint16_t currentLimit)
{
    bms.setCurrentLimit(currentLimit);
}


void MPU::setChargeCurrentLimit(uint16_t currentLimit)
{
    bms.setChargeCurrentLimit(currentLimit);
}


void MPU::bmsCurrentProcess(int16_t currentDraw)
{
    bms.setCurrentDraw(currentDraw);

    if(bms.isCurrentPastLimit())
    {
        bms.setBoosting();
    }
}


void MPU::setMotorSpeed(int16_t motorSpeed)
{
    motorController.setMotorSpeed(motorSpeed);
}


void MPU::enableBMSChargingMode()
{
    bms.enableChargingMode();
}


void MPU::setBMSVoltage(int16_t voltage)
{
    bms.setLiveVoltage(voltage);
}


void MPU::setMotorTemp(int16_t temp)
{
    motorController.setRadiatorTemp(temp);
}

bool MPU::verifyMotorSpinning()
{
    if(motorController.shouldMotorBeSpinning())
    {
        return !motorController.isMotorMoving();
    }
    return false;
}
