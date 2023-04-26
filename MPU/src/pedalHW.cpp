#include "pedalHW.h"

PedalHW::PedalHW() {}

PedalHW::~PedalHW() {}

PedalHW::PedalHW(float p_errorPercent, uint8_t p_maxErrors,
                 uint8_t *pinNumbers) {
  errorPercent = p_errorPercent;
  maxErrors = p_maxErrors;

  pin1 = pinNumbers[0];
  pin2 = pinNumbers[1];

  readingDebounce.cancelTimer();
}

uint16_t PedalHW::readValue() {
  // Acounting for first loop
  if (readingDebounce.isTimerReset())
    readingDebounce.startTimer(PEDAL_DEBOUNCE_TIME);

  if (!readingDebounce.isTimerExpired()) {
    minimizingDiffDebounce();
    return NOT_DONE_READING;
  }
  uint16_t finalReading = avgReading;
  readingDiff = MAX_ADC_VALUE;
  readingDebounce.startTimer(PEDAL_DEBOUNCE_TIME);
  return finalReading;
}

void PedalHW::minimizingDiffDebounce() {
  uint16_t pin1Val = analogRead(pin1);
  uint16_t pin2Val = analogRead(pin2);

  // Iteratively narrow towards value
  if (readingDiff > abs(pin1Val - pin2Val)) {
    readingDiff = abs(pin1Val - pin2Val);
    avgReading = (pin1Val + pin2Val) / 2;
  }
  checkForPedalError(pin1Val, pin2Val);
}

void PedalHW::checkForPedalError(uint16_t val1, uint16_t val2) {
  // Check for out of range values and errors (bottoming out of sensors/one
  // faulty sensor)
  if (val1 == 0 || val2 == 0 || val1 == MAX_ADC_VALUE ||
      val2 == MAX_ADC_VALUE ||
      (abs(val1 - val2) > (MAX_ADC_VALUE * errorPercent / 100))) {
    readingErrors++;
    if (readingErrors >= maxErrors) {
      readingFault = FAULTED;
    }
  }
}

FaultStatus_t PedalHW::isFaulted() { return readingFault; }
