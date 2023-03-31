/**
 * @file gpioHW.h
 * @author Nick DePatie
 * @date 2022-08-07
 */
#ifndef GPIOHW_H
#define GPIOHW_H

#include <nerduino.h>

#define CANMSG_PDU_ID 0x666

class PDU
{
    private:
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

        void sendPDUMsg();

    public:
        PDU();

        ~PDU();

        void enableRadiatorFan(bool status);

        void enableAccFans(bool r_status, bool l_status);

        void enableCoolingPump(bool status);

        void enableBrakeLight(bool status);
};

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