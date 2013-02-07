#ifndef _SOFT_PWM_h
#define _SOFT_PWM_h

#include <Arduino.h>

class SoftPWM {
 private:
  void setupTimer();
  void setupOutputPins();
  
 public:
  // Start the software PWM driver. Initializes all outputs,
  // and starts the timer.
  void begin();
  
  // Set the value of an output pin
  // @param channel PWM output channel to update
  // @param value New output value (0-255)
  void write(uint8_t channel, uint8_t value);
  
  // Read the value of an output pin
  // @param channel PWM output channel to read
  // @return value Channel value (0-255)
  uint8_t read(uint8_t channel);
};

#endif
