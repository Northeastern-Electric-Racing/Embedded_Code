/**
 * @file gnss.h
 * @brief Utilities to receive GNSS data
 */

#ifndef GNSS_H
#define GNSS_H

#include <Arduino.h>

/* Return status codes for GNSS functions. */
typedef enum { GNSS_SUCCESS = 0, GNSS_ERROR_INIT = 1 } GNSS_STATUS;

/**
 * @brief Initializes the GNSS Module
 *
 * @param serialPort Serial port
 * @param baudRate Baud rate
 * @return GNSS_STATUS
 */
GNSS_STATUS GnssInit(HardwareSerial &serialPort, uint32_t baudRate);

int32_t GnssGetUnixSeconds();
int32_t GnssGetLatitude();
int32_t GnssGetLongitude();
int32_t GnssGetAltitude();
int32_t GnssGetGroundSpeed();
int32_t GnssGetFixOk();
int32_t GnssGetHeading();

#endif