#include "HardPWM.h"

void HardPWM::write(uint8_t channel, uint16_t value) {
  if(channel > pwmCount) {
    return;
  }
  
  m_pwmValues[channel] = value;
  
  switch(channel) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      TC4H = (value >> 8) & 0x03;
      OCR4A = (value & 0xFF);  // Set some default value
      break;
    case 5:
      TC4H = (value >> 8) & 0x03;
      OCR4D = (value & 0xFF);  // Set some default value
      break;
  }
}

void HardPWM::begin() {
  // Configure timer1 for 16-bit PWM @
  
  
  // configure timer4 for 10-bit PWM @15.625KHz
  // Only enable output for OC4A
  bitSet(DDRC, 7);  // OCR4A, PC7 (PWM4)
  bitSet(DDRD, 7);  // OCR4D, PD7 (PWM5)

  TCCR4A = (1 << COM4A1) | (0 << COM4A0) | (1 << PWM4A) | (0 << PWM4B);
  TCCR4B = (0 << CS43) | (0 << CS42) | (0 << CS41) | (1 << CS40);
 // TCCR4C = (1 << COM4D1) | (0 << COM4D0) | (1 << PWM4D);
  TCCR4D = (0 << WGM41) | (0 << WGM40);
  
  TC4H = 0x03;
  OCR4C = 0xFF;  // OCR4C contains the TOP value
  
//  // configure timer3 for 12-bit PWM @4KHz (16MHz / 4096)
//  // PWM, Phase and Frequency correct, reset on ICR
//  TCCR3A = (0 << WGM31) | (0 << WGM30);
//  TCCR3B = (1 << WGM33) | (0 << WGM32) | (0 << CS32) | (0 << CS31) | (1 << CS30);
//  ICR3 = 4095; // 2**12 - 1, for 12-bit mode
//  OCR3A = 0x0001;  // Set some default values
//  OCR3B = 0x0002;
//  OCR3C = 0x0004;
}
