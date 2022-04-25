/**
 * @file canMsgHandler.h
 * @author Nick DePatie
 * @brief 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   CAN Node must be initialized externally by setting the desired CAN ID's into the EEPROM before use
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @date 2022-03-07
 */
#ifndef CANMSGHANDLER_H
#define CANMSGHANDLER_H

#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <EEPROM.h>


/*************************************************/
/**
 * CAN Settings
 */

#define BAUD_RATE 1000000U
#define MAX_MB_NUM 16 // maximum number of CAN mailboxes to use
extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; // main CAN object

/*************************************************/
/**
 * CAN Addresses
 */
#define CANMSG_BMSACCSTATUS         0x01
#define CANMSG_BMSSHUTDOWN          0x03
#define CANMSG_BMSCELLDATA          0x04
#define CANMSG_BMSDTCSTATUS         0x002
#define CANMSG_BMSCURRENTLIMITS     0x202
#define CANMSG_ACCELERATIONCTRLINFO 0xC0
#define CANMSG_MC_SETPARAMETER      0x0C1
#define CANMSG_MOTORTEMP1           0xA0
#define CANMSG_MOTORTEMP2           0xA1
#define CANMSG_MOTORETEMP3          0xA2
#define CANMSG_MOTORMOTION          0xA5
#define CANMSG_MOTORCURRENT         0xA6
#define CANMSG_MOTORVOLTAGE         0xA7
#define CANMSG_MCVEHICLESTATE       0xAA
#define CANMSG_ERR_MCFAULT          0xAB
#define CANMSG_MOTORTORQUETIMER     0xAC
#define CANMSG_BMSSTATUS2           0x6B1
#define CANMSG_BMSCHARGEDISCHARGE   0x6B0
#define CANMSG_MC_BMS_INTEGRATION   0x202

/*******************************************************************/
/**
 * Prototype Function
 */

/**
 * @brief Sends CAN message
 * 
 * @param id 
 * @param len 
 * @param buf 
 * @return int 
 */
int sendMessage(uint32_t id, uint8_t len, const uint8_t *buf);

/**
 * @brief Processes all CAN messages
 * @note  ID filtering should happen beforehand, maybe add relevent ID's to each .cpp file?
 * 
 * @param msg 
 */
void incomingCANCallback(const CAN_message_t &msg);

/**
 * @brief Initializes a CAN object for whichever line we are choosing
 * 
 * @param canLine   which CAN transceiver to use we want to use = NOT currently being used, we will probably need this eventually if we need to broadcast at different rates
 */
void initializeCAN(uint8_t canLine);

/**
 * @brief Reads in all EEPROM values and initializes the canAddr's struct
 * 
 */
void readEEPROMAddrs();

/**
 * @brief Configures the EEPROM address of the CAN message specified by the first index of the message to the address value specified by the second index of the message
 * 
 * @param msg
 */
void canHandler_CANMSG_CONFIGUREADDR(const CAN_message_t &msg);

/***********************************************************************************/
/**
 * @brief CAN Message Handle Commands
 * __attribute__((weak)) indicates that if the compiler doesn't find any other function with this same name, then it will default to these which just do nothing
 * This means we can use a master CAN processing command amongst all devices
 * 
 */

void canHandler_CANMSG_BMSACCSTATUS         (const CAN_message_t &msg);
void canHandler_CANMSG_BMSCELLDATA          (const CAN_message_t &msg);
void canHandler_CANMSG_BMSCURRENTLIMITS     (const CAN_message_t &msg);
void canHandler_CANMSG_BMSSHUTDOWN          (const CAN_message_t &msg);
void canHandler_CANMSG_BMSDTCSTATUS         (const CAN_message_t &msg);
void canHandler_CANMSG_ACCELERATIONCTRLINFO (const CAN_message_t &msg);
void canHandler_CANMSG_MOTORTEMP1           (const CAN_message_t &msg);
void canHandler_CANMSG_MOTORTEMP2           (const CAN_message_t &msg);
void canHandler_CANMSG_MOTORETEMP3          (const CAN_message_t &msg);
void canHandler_CANMSG_MOTORMOTION          (const CAN_message_t &msg);
void canHandler_CANMSG_MOTORCURRENT         (const CAN_message_t &msg);
void canHandler_CANMSG_MOTORVOLTAGE         (const CAN_message_t &msg);
void canHandler_CANMSG_MCVEHICLESTATE       (const CAN_message_t &msg);
void canHandler_CANMSG_ERR_MCFAULT          (const CAN_message_t &msg);
void canHandler_CANMSG_MOTORTORQUETIMER     (const CAN_message_t &msg);
void canHandler_CANMSG_BMSSTATUS2           (const CAN_message_t &msg);
void canHandler_CANMSG_BMSCHARGEDISCHARGE   (const CAN_message_t &msg);
void canHandler_CANMSG_MC_BMS_INTEGRATION   (const CAN_message_t &msg);
void canHandler_CANMSG_MC_SETPARAMETER      (const CAN_message_t &msg);

//For SD logging in the TCU, isn't used anywhere else
bool SDWrite();

/**************************************************/

#endif