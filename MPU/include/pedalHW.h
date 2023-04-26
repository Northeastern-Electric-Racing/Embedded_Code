/**
 * @file pedalsHW.h
 * @author Nick DePatie
 * @brief Pedal Hardware Abstraction
 * @date 2022-06-30
 */

#ifndef PEDALHW_H
#define PEDALHW_H

#include <nerduino.h>

// Maximum ADC value based on resolution (10 bit resolution for Teensy 4.1)
#define MAX_ADC_VALUE 1023 // 1111111111 in binary

// Timer macros
#define PEDAL_DEBOUNCE_TIME 40     // ms
#define BRAKELIGHT_MIN_TIME_ON 250 // ms
#define NOT_DONE_READING 0xFFFF

class PedalHW {
private:
  // Pedal reading timers
  Timer readingDebounce;
  // Pedal reading
  uint16_t readingDiff;
  uint16_t avgReading;
  // Pedal error counting
  uint8_t readingErrors = 0;
  FaultStatus_t readingFault = NOT_FAULTED;

  // Constants specific to which pedal
  int8_t errorPercent;
  uint8_t maxErrors;
  uint8_t pin1;
  uint8_t pin2;

  /**
   * @brief Basic Debounce Algorithm
   * @note Saves the average of the closest readings of both pedals over a
   * debounce period
   */
  void minimizingDiffDebounce();

  /**
   * @brief Checks if the two passed values would raise an error, and checks if
   * the error limit has been hit
   *
   * @param val1
   * @param val2
   */
  void checkForPedalError(uint16_t val1, uint16_t val2);

public:
  PedalHW();

  /**
   * @brief Construct a new pedal hw object (requires pedal specific parameters)
   * @param errorPercent
   * @param maxErrors
   */
  PedalHW(float p_errorPercent, uint8_t p_maxErrors, uint8_t *pinNumbers);

  ~PedalHW();

  /**
   * @brief Reads the current pedal value with error-checking and accuracy
   * debounce
   *
   * @return The value being sensed (0 to MAX_ADC_VALUE)
   */
  uint16_t readValue();

  /**
   * @brief Returns if the maximum number of reading errors allowed has been
   * reached
   *
   * @return true
   * @return false
   */
  FaultStatus_t isFaulted();
};

#endif