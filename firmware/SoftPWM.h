#ifndef _SOFT_PWM_h
#define _SOFT_PWM_h

#include <Arduino.h>

class SoftPWM {
 private:
  void setupTimer();
  void setupOutputPins();
  
 public:
  void begin();
  void setPWM(uint8_t pwm, uint8_t value);
  uint8_t readPWM(uint8_t pwm);
};

#endif
