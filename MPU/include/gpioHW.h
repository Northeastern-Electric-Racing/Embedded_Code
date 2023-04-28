/**
 * @file gpioHW.h
 * @author Nick DePatie
 * @date 2022-08-07
 */
#ifndef GPIOHW_H
#define GPIOHW_H

#include <nerduino.h>

#define CANMSG_PDU_ID 0x666
#define BRAKELIGHT_WAIT_MS  250

class PDU
{
    private:

        uint8_t motor_temp_to_rad_duty[20] = {0, 0, 0, 0, 0, 0, 25, 50, 75, 100, 128, 150, 200, 225, 255, 255, 255, 255, 255, 255};

        bool prev_brakelight_status = false;
        Timer brakelight_timer;

        union
        {
            uint8_t msg[5] = {0, 0, 0, 0, 0};

            struct
            {
                bool brake_light;
                bool cooling_pump;
                bool left_acc_fan;
                bool right_acc_fan;
                uint8_t radiator_fan_dty;
            } fields;
        } pdu;

    public:
        PDU();

        ~PDU();

        void enableRadiatorFan(uint8_t status);

        void enableAccFans(bool r_status, bool l_status);

        void enableCoolingPump(bool status);

        void enableBrakeLight(bool status);

        void sendPDUMsg();
};

extern PDU pdu;

class TSMS  //TSMS stands for "Tractive System Main Switch"
{
    private:
        bool isTSCycled = false;
        bool prevReading = false;
        uint8_t pin;

    public:
        TSMS();
        TSMS(uint8_t pinNum);
        ~TSMS();

        /**
         * @brief Returns if TS power has been cycled (1 -> 0)
         * @note Resets the power cycle flag upon reading
         * 
         * @return true
         * @return false 
         */
        bool isPowerCycled();

        /**
         * @brief Returns the current reading of the line (If TSMS is switched on)
         * 
         * @return true 
         * @return false 
         */
        bool isReady();
};

#endif