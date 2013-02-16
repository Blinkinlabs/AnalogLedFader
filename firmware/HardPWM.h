#ifndef _HARD_PWM_h
#define _HARD_PWM_h

#include <Arduino.h>

#define pwmCount  6

// This is an alternate PWM interface for Arduino/Atmega32u4, that provides 16 MHz,
// 16- and 10-bit PWM outputs on the following channels:
//
// PB5 (PWM0) (16 bit)
// PB6 (PWM1) (16 bit)
// PB7 (PWM2) (16 bit)
// PC6 (PWM3) (16 bit)
// PC7 (PWM4) (10 bit)
// PD7 (PWM5) (10 bit)
  
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
