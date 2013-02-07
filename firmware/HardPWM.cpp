#include "HardPWM.h"

void HardPWM::write(uint8_t channel, uint16_t value) {
  if(channel >= pwmCount) {
    return;
  }
  
  m_pwmValues[channel] = value;
  
  switch(channel) {
    case 0:
      OCR1A = value;
      break;
    case 1:
      OCR1B = value;
      break;
    case 2:
      OCR1C = value;
      break;
    case 3:
      OCR3A = value;
      break;
    case 4:  //Note: in 10-bit mode for now.
      TC4H = (value >> 14) & 0x03;
      OCR4A = (value >> 6) & 0xFF;
      break;
    case 5:  //Note: in 10-bit mode for now.
      TC4H = (value >> 14) & 0x03;
      OCR4D = (value >> 6) & 0xFF;
      break;
  }
}

uint16_t HardPWM::read(uint8_t channel) {
  if(channel >= pwmCount) {
    return 0;
  }
  
  return m_pwmValues[channel];
}

void HardPWM::begin() {
  // Configure timer1 for 16-bit, PWM, Phase and Frequency Correct operation at @15.625KHz
  bitSet(DDRB, 5);  // OCR1A, PB5 (PWM0)
  bitSet(DDRB, 6);  // OCR1B, PB6 (PWM1)
  bitSet(DDRB, 7);  // OCR1C, PB7 (PWM2)
  
  TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (0 << COM1B0) | (1 << COM1C1) | (0 << COM1C0) | (0 << WGM11) | (0 << WGM10);
  TCCR1B = (1 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
  ICR1 = 0xFFFF;
  
    // Configure timer1 for 16-bit, PWM, Phase and Frequency Correct operation at @15.625KHz
  bitSet(DDRC, 6);  // OCR1A, PC6 (PWM3)
  
  TCCR3A = (1 << COM3A1) | (0 << COM3A0) | (0 << WGM31) | (0 << WGM30);
  TCCR3B = (1 << WGM33) | (0 << WGM32) | (0 << CS32) | (0 << CS31) | (1 << CS30);
  ICR3 = 0xFFFF;
  
  // configure timer4 for 10-bit PWM @15.625KHz
  // Only enable output for OC4A
  bitSet(DDRC, 7);  // OCR4A, PC7 (PWM4)
  bitSet(DDRD, 7);  // OCR4D, PD7 (PWM5)

  TCCR4C = (1 << COM4D1) | (0 << COM4D0) | (1 << PWM4D); // Note: set TCCR4C before TCCR4A because of shadow bits.
  TCCR4A = (1 << COM4A1) | (0 << COM4A0) | (1 << PWM4A) | (0 << PWM4B);
  TCCR4B = (0 << CS43) | (0 << CS42) | (0 << CS41) | (1 << CS40);
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
