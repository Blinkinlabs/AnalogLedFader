#ifndef _HARD_PWM_h
#define _HARD_PWM_h

#include <Arduino.h>

#define pwmCount  6

class HardPWM {
 private:
  uint16_t m_pwmValues[pwmCount];  // currrent logarithmic PWM setting for each PWM output

 public:
  // Start the software PWM driver. Initializes all outputs,
  // and starts the timer.
  void begin();
  
  // Set the value of an output pin
  // @param channel PWM output channel to update
  // @param value New output value (0-1024)
  void write(uint8_t channel, uint16_t value);
  
  // Read the value of an output pin
  // @param channel PWM output channel to read
  // @return value Channel value (0-1024)
  uint16_t read(uint8_t channel);
};

#endif
