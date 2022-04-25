#include "canMsgHandler.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan;

void initializeCAN(uint8_t canLine)
{
    myCan.begin(); // needed to initialize the CAN object (must be first method called)
    myCan.setBaudRate(BAUD_RATE); // sets baud rate

    myCan.setMaxMB(MAX_MB_NUM);
    myCan.enableFIFO(); // enables the FIFO operation mode, where all received messages are received and accessed via a queue
    myCan.enableFIFOInterrupt(); // enables interrupts to be used with FIFO
    myCan.onReceive(incomingCANCallback); // sets the callback for received messages
    myCan.mailboxStatus(); // prints out mailbox config information
}


int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf)
{
CAN_message_t msg;
msg.id = id;
msg.len = len;
uint8_t *buf1;

    for (int i = 0; i < 8; i++) {
        if (i < len)
        {
            buf1 = const_cast<uint8_t*>(buf + i);
            msg.buf[i] = *buf1;
        }
        else
        {
            msg.buf[i] = 0; // copies buf to message, padding with 0s if length isn't 8
        }
    }

    return myCan.write(msg);
}

void incomingCANCallback(const CAN_message_t &msg)
{
    if(!SDWrite())
    {
        Serial.println("Error logging to SD Card!");
    }

    /**
     * Using a switch statement to rule out set CAN IDs first, and then using if
     * elses within the default case because the switch statement is more efficient
     */
    switch(msg.id)
    {                                                   //using a switch statement for set CAN IDs
        case CANMSG_ACCELERATIONCTRLINFO    :
            canHandler_CANMSG_ACCELERATIONCTRLINFO(msg);
            break;
        case CANMSG_MOTORTEMP1              :
            canHandler_CANMSG_MOTORTEMP1(msg);
            break;
        case CANMSG_MOTORTEMP2              :
            canHandler_CANMSG_MOTORTEMP2(msg);
            break;
        case CANMSG_MOTORETEMP3             :
            canHandler_CANMSG_MOTORETEMP3(msg);
            break;
        case CANMSG_MOTORMOTION             :
            canHandler_CANMSG_MOTORMOTION(msg);
            break;
        case CANMSG_MOTORCURRENT            :
            canHandler_CANMSG_MOTORCURRENT(msg);
            break;
        case CANMSG_MOTORVOLTAGE            :
            canHandler_CANMSG_MOTORVOLTAGE(msg);
            break;
        case CANMSG_MCVEHICLESTATE          :
            canHandler_CANMSG_MCVEHICLESTATE(msg);
            break;
        case CANMSG_ERR_MCFAULT             :
            canHandler_CANMSG_ERR_MCFAULT(msg);
            break;
        case CANMSG_MOTORTORQUETIMER        :
            canHandler_CANMSG_MOTORTORQUETIMER(msg);
            break;
        case CANMSG_BMSSTATUS2              :
            canHandler_CANMSG_BMSSTATUS2(msg);
            break;
        case CANMSG_BMSSHUTDOWN:
            canHandler_CANMSG_BMSSHUTDOWN(msg);
            break;
        case CANMSG_BMSDTCSTATUS:
            canHandler_CANMSG_BMSDTCSTATUS(msg);
            break;
        case CANMSG_BMSACCSTATUS:
            canHandler_CANMSG_BMSACCSTATUS(msg);
            break;
        case CANMSG_BMSCELLDATA:
            canHandler_CANMSG_BMSCELLDATA(msg);
            break;
        case CANMSG_BMSCURRENTLIMITS:
            canHandler_CANMSG_BMSCURRENTLIMITS(msg);
            break;
        case CANMSG_MC_SETPARAMETER:
            canHandler_CANMSG_MC_SETPARAMETER(msg);
            break;
        default:
            Serial.print("! CAN ID Invalid:\t");
            Serial.println(msg.id, HEX);
            break;
    }
}

/**************************************************************/
/**
 * @brief CAN Message Handle Commands
 * __attribute__((weak)) indicates that if the compiler doesn't find any other function with this same name, then it will default to these which just do nothing
 * This means we can use a master CAN processing command amongst all devices
 */

__attribute__((weak)) void canHandler_CANMSG_BMSACCSTATUS         (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_BMSCELLDATA          (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_BMSCURRENTLIMITS     (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_BMSSHUTDOWN          (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_BMSDTCSTATUS         (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_ACCELERATIONCTRLINFO (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MOTORTEMP1           (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MOTORTEMP2           (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MOTORETEMP3          (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MOTORMOTION          (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MOTORCURRENT         (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MOTORVOLTAGE         (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MCVEHICLESTATE       (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_ERR_MCFAULT          (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MOTORTORQUETIMER     (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_BMSSTATUS2           (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_BMSCHARGEDISCHARGE   (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MC_BMS_INTEGRATION   (const CAN_message_t &msg){return;}
__attribute__((weak)) void canHandler_CANMSG_MC_SETPARAMETER      (const CAN_message_t &msg){return;}

//For SD logging in the TCU, isn't used anywhere else
__attribute__((weak)) bool SDWrite(){return true;}

