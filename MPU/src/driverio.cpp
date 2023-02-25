#include "driverio.h"
#include <nerduino.h>

DriverIO::DriverIO(){}

DriverIO::DriverIO(CascadiaMC *p_motorController, OrionBMS *p_bms)
{
    powerToggle_wait.cancelTimer();

    motorController = p_motorController;
    bms = p_bms;

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

void DriverIO::handleButtonState()
{
    bool state_changed = false;

    //Poll Button
    incrButton.checkButtonPin();
    decrButton.checkButtonPin();

    if (motorController->checkFault())
    {
        Serial.println("Motor controller not ready!!!");
        motorController->setPower(false);
        mpu_state = FAULT;
        return;
    }

    //If the BMS is charging
    if((incrButton.isButtonPressed_Pulse() ||
        decrButton.isButtonPressed_Pulse()) && 
        bms->getChargeMode())
    {
        bms->toggleAIR();
        powerToggle_wait.startTimer(1500);
        mpu_state = CHARGING;
        return;
    }
    else
    {
        mpu_state = DRIVE;
    }

    //If the BMS is not charging
    if(incrButton.isButtonPressed() && changeStateTimer.isTimerExpired())
    {
        drive_state = (drive_state + 1) % MAX_DRIVE_STATES;
        Serial.println("Increment!");
        changeStateTimer.startTimer(CHANGE_STATE_TIME);
        state_changed = true;
    }
    if(decrButton.isButtonPressed() && changeStateTimer.isTimerExpired())
    {
        drive_state = ((drive_state - 1) % MAX_DRIVE_STATES);
        if (drive_state == 255) drive_state = REVERSE;

        Serial.println("Decrement!");
        changeStateTimer.startTimer(CHANGE_STATE_TIME);
        state_changed = true;
    }

    bool motor_power = !(drive_state == OFF || mpu_state != DRIVE);

    motorController->setPower(motor_power);

    if(state_changed && ((drive_state == PIT) || (drive_state == REVERSE)))
    {
        speaker.playSpeaker();
    }
    powerToggle_wait.startTimer(POWER_TOGGLE_WAIT);
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

    incrButton.setButtonState(wheelio.io.button2);
    decrButton.setButtonState(wheelio.io.button4);
    Serial.println("WHEEL MSG RECEIVED!!");
}