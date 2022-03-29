#ifndef CASCADIAMC_H
#define CASCADIAMC_H

#include <nerduino.h>


class CASCADIAMC
{
    private:

        const unsigned char mcOff[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        /*
        union
        {
            uint8_t canMsg[8] = {0,0,0,0,0,0,0,0};

            struct
            {
                uint16_t accelTorque    :16;
                uint16_t reserved       :16;
                bool isForward          :8;
                bool isOn               :8;
                uint16_t reserved       :16;
            } bitfieldMsg;
        }mcMsg;
        */
        Timer motorCommand_wait;

    public:
        CASCADIAMC();

        ~CASCADIAMC();

        void disableMCLockout();

        void writeMCState();
};

#endif