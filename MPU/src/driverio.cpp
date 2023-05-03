#include "driverio.h"
#include <nerduino.h>

DriverIO::DriverIO(){}

DriverIO::DriverIO(CascadiaMC *p_motorController, OrionBMS *p_bms, GPIO *p_gpio, Pedals *p_pedals)
{
    powerToggle_wait.cancelTimer();

    motorController = p_motorController;
    bms = p_bms;
    gpio = p_gpio;
    pedals = p_pedals;

    motorController->setDirection(false);
}

DriverIO::~DriverIO(){}

void DriverIO::handleSpeaker()
{
    speaker.attemptToStopSpeaker();
}

void DriverIO::handleReverse()
{
    motorController->setDirection(drive_state != REVERSE);
}

void DriverIO::handleButtonState(bool tsms_status)
{
    bool state_changed = false;

    //Poll Button
    incrButton.checkButtonPin();
    decrButton.checkButtonPin();
    torqueDecreasePaddle.checkButtonPin();
    torqueIncreasePaddle.checkButtonPin();
    regenButton.checkButtonPin();

    if (tsms_status == false)
    {
        motorController->setPower(false);
        mpu_state = FAULT;
        drive_state = OFF;
        return;
    }

    if (isCharging)
    {
        Serial.println("CHARGING!!");
    }

    //If the BMS is charging
    if((incrButton.isButtonPressed() ||
        decrButton.isButtonPressed()) && 
        bms->getChargeMode() &&
        changeStateTimer.isTimerExpired())
    {
        bms->toggleAIR();
        changeStateTimer.startTimer(CHANGE_STATE_TIME);
        isCharging = !isCharging;
        mpu_state = isCharging ? CHARGING : OFF;
        drive_state = OFF;
        Serial.println("TOGGLE CHARGING!!!");
        return;
    }
    else if (isCharging)
    {
        return;
    }
    else
    {
        mpu_state = DRIVE;
    }

    //If the BMS is not charging
    // State buttons
    if(incrButton.isButtonPressed() && changeStateTimer.isTimerExpired())
    {
        prev_state = drive_state;
        drive_state = (drive_state + 1) % MAX_DRIVE_STATES;
        Serial.println("Increment!");
        changeStateTimer.startTimer(CHANGE_STATE_TIME);
        state_changed = true;
    }
    if(decrButton.isButtonPressed() && changeStateTimer.isTimerExpired())
    {
        prev_state = drive_state;
        drive_state = ((drive_state - 1) % MAX_DRIVE_STATES);
        if (drive_state == 255) drive_state = REVERSE;

        Serial.println("Decrement!");
        changeStateTimer.startTimer(CHANGE_STATE_TIME);
        state_changed = true;
    }

    // Efficiency mode buttons
    if (drive_state == EFFICIENCY) {
        if(torqueIncreasePaddle.isButtonPressed() && changeTorqueLimitTimer.isTimerExpired())
        {
            changeTorqueLimitTimer.startTimer(CHANGE_TORQUE_LIMIT_TIME);
            float curr_torque_limit = (float)pedals->getTorqueLimitPercentage() / 100.0;
            if (curr_torque_limit < 1.0)
            {
                curr_torque_limit += 0.1;
                pedals->setTorqueLimitPercentage(curr_torque_limit);
            }
        }

        if(torqueDecreasePaddle.isButtonPressed() && changeTorqueLimitTimer.isTimerExpired())
        {
            changeTorqueLimitTimer.startTimer(CHANGE_TORQUE_LIMIT_TIME);
            float curr_torque_limit = (float)pedals->getTorqueLimitPercentage() / 100.0;
            Serial.print("Current Torque Limit: ");
            Serial.println(curr_torque_limit);
            if (curr_torque_limit > 0.0)
            {
                curr_torque_limit -= 0.1;
                pedals->setTorqueLimitPercentage(curr_torque_limit);
            }
        }

        if(regenButton.isButtonPressed() && changeRegenTimer.isTimerExpired()) {
            changeRegenTimer.startTimer(CHANGE_REGEN_TIME);
            pedals->incrRegenLevel();
        }

        if(bmsFaultButton.isButtonPressed())
        {
            gpio->toggleBMSPreFault();
        }
    }

    if(drive_state == SPEED) {
        if(regenButton.isButtonPressed() && changeRegenTimer.isTimerExpired()) {
            changeRegenTimer.startTimer(CHANGE_REGEN_TIME);
            pedals->toggleControlLaunch();
        }
    }

    bool motor_power = !(drive_state == OFF || mpu_state != DRIVE);

    motorController->setPower(motor_power);

    if(state_changed && 
        prev_state == OFF && 
        ((drive_state == PIT) || (drive_state == REVERSE)))
    {
        speaker.playSpeaker();
    }
    powerToggle_wait.startTimer(POWER_TOGGLE_WAIT);

    if(state_changed)
    {
        motorController->setPower(false);
    }
}

void DriverIO::wheelIO_cb(const CAN_message_t &msg)
{
    union
    {
        uint8_t msg[8];

        struct
        {
            uint16_t pot_l;
            uint16_t pot_r;
            bool button5 : 1;
            bool button6 : 1;
            bool button3 : 1;
            uint8_t x1   : 1;
            bool paddle_r : 1;
            bool paddle_l : 1;
            bool button4 : 1;
            bool button2 : 1;
            uint8_t x2;
            uint8_t x3;
            uint8_t x4;
        } io;
    } wheelio;

    for(uint8_t byte = 0; byte < 8; byte++)
    {
        wheelio.msg[byte] = msg.buf[byte];
    }

    wheelio.io.pot_l = SWITCHBYTES(wheelio.io.pot_l);
    wheelio.io.pot_r = SWITCHBYTES(wheelio.io.pot_r);

    decrButton.setButtonState(wheelio.io.button2);
    incrButton.setButtonState(wheelio.io.button4);
    regenButton.setButtonState(wheelio.io.button5);
    bmsFaultButton.setButtonState(wheelio.io.button6);
    torqueIncreasePaddle.setButtonState(wheelio.io.paddle_r);
    torqueDecreasePaddle.setButtonState(wheelio.io.paddle_l);
    accumulatorFanDial.setDialValue(wheelio.io.pot_l);
    motorFanDial.setDialValue(wheelio.io.pot_r);
}

void DriverIO::handleDialState()
{
    float motorFanPercentage = ((float)motorFanDial.getDialValue() / 4096.0);
    if (motorFanPercentage < 0.1) motorFanPercentage = 0.0;
    else if (motorFanPercentage > 0.9) motorFanPercentage = 1.0;
    gpio->setRadiatorFanPercentage(motorFanPercentage);
}

uint8_t DriverIO::getAccumulatorFanDialPercentage() 
{
    float accumulatorFanPercentage = ((float)accumulatorFanDial.getDialValue() / 4096.0);
    if (accumulatorFanPercentage < 0.1) accumulatorFanPercentage = 0.0;
    else if (accumulatorFanPercentage > 0.9) accumulatorFanPercentage = 1.0;
    return accumulatorFanPercentage * 100;
}
