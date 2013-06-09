#ifndef _HARD_PWM_h
#define _HARD_PWM_h

#include <Arduino.h>

// Number of PWM outputs enabled
#define pwmCount  6 

// This is an alternate PWM interface for Arduino/Atmega32u4, that provides 488Hz,
// 14 or 10-bit PWM on the following outputs:
//
// PB7 (PWM0) (14 bit)
// PD7 (PWM1) (10 bit)
// PB5 (PWM2) (14 bit)
// PB6 (PWM3) (14 bit)
// PC6 (PWM4) (14 bit)
// PC7 (PWM5) (10 bit)
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
