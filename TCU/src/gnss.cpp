#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <Wire.h>

#include "debug.h"
#include "gnss.h"

static SFE_UBLOX_GNSS gnss;
static HardwareSerial *port;

GNSS_STATUS GnssInit(HardwareSerial &serialPort, uint32_t baudRate) {
  port = &serialPort;
  (*port).begin(baudRate);
  delay(100);
  do {
    if (gnss.begin(serialPort)) {
      gnss.setSerialRate(baudRate);
      gnss.setUART1Output(COM_TYPE_UBX);
      gnss.setI2COutput(COM_TYPE_UBX);
      gnss.saveConfiguration();
      DPRINTLN(F("GNSS module successfully initialized"));
      return GNSS_STATUS::GNSS_SUCCESS;
    }
    DPRINTLN(F("Failed to initialize GNSS module"));
    delay(1000);
  } while (1);

  return GNSS_STATUS::GNSS_ERROR_INIT;
}

int32_t GnssGetLatitude() { return gnss.getLatitude(); }

int32_t GnssGetLongitude() { return gnss.getLongitude(); }

int32_t GnssGetAltitude() { return gnss.getAltitude(); }

int32_t GnssGetUnixSeconds() { return gnss.getUnixEpoch(); }

int32_t GnssGetGroundSpeed() { return gnss.getGroundSpeed(); }

int32_t GnssGetFixOk() { return gnss.getGnssFixOk(); }

int32_t GnssGetHeading() { return gnss.getHeading(); }