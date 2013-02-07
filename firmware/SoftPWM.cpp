#include "SoftPWM.h"

const uint8_t pwmCount = 4;

const uint8_t pwmPorts[pwmCount] = {1, 1, 1, 3};  // Port that the PWM output is connected to (1=B, 2=C, etc)
const uint8_t pwmBits[pwmCount]  = {0, 1, 2, 6};  // Bit location of the PWM output in the port

uint8_t pwmValues[pwmCount];  // currrent logarithmic PWM setting for each PWM output


const uint8_t pwmSteps = 255;  // Number of PWM steps available
volatile uint8_t pwmStep;  // current step in the PWM counter


// Logarithmic counts for a 16-bit timer
// Max value determines the refresh frequency as well as the step accuracy
// Generated using python:
/*
steps = 255
maxValue = 255
for i in range(0,steps-1):
  print int((i/(steps - 1.0))*(i/(steps - 1.0))*maxValue)+1, ',',
  if(i%10 == 9):
    print ''
*/
static uint8_t timerCounts[pwmSteps - 1] = {
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 
1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 
2 , 2 , 2 , 3 , 3 , 3 , 3 , 3 , 4 , 4 , 
4 , 4 , 5 , 5 , 5 , 5 , 6 , 6 , 6 , 7 , 
7 , 7 , 7 , 8 , 8 , 9 , 9 , 9 , 10 , 10 , 
10 , 11 , 11 , 12 , 12 , 12 , 13 , 13 , 14 , 14 , 
15 , 15 , 16 , 16 , 17 , 17 , 18 , 18 , 19 , 19 , 
20 , 20 , 21 , 22 , 22 , 23 , 23 , 24 , 25 , 25 , 
26 , 26 , 27 , 28 , 28 , 29 , 30 , 30 , 31 , 32 , 
33 , 33 , 34 , 35 , 35 , 36 , 37 , 38 , 38 , 39 , 
40 , 41 , 42 , 42 , 43 , 44 , 45 , 46 , 47 , 47 , 
48 , 49 , 50 , 51 , 52 , 53 , 54 , 55 , 56 , 56 , 
57 , 58 , 59 , 60 , 61 , 62 , 63 , 64 , 65 , 66 , 
67 , 68 , 69 , 70 , 71 , 73 , 74 , 75 , 76 , 77 , 
78 , 79 , 80 , 81 , 82 , 84 , 85 , 86 , 87 , 88 , 
89 , 91 , 92 , 93 , 94 , 95 , 97 , 98 , 99 , 100 , 
102 , 103 , 104 , 106 , 107 , 108 , 109 , 111 , 112 , 113 , 
115 , 116 , 117 , 119 , 120 , 122 , 123 , 124 , 126 , 127 , 
129 , 130 , 131 , 133 , 134 , 136 , 137 , 139 , 140 , 142 , 
143 , 145 , 146 , 148 , 149 , 151 , 152 , 154 , 155 , 157 , 
159 , 160 , 162 , 163 , 165 , 167 , 168 , 170 , 172 , 173 , 
175 , 176 , 178 , 180 , 182 , 183 , 185 , 187 , 188 , 190 , 
192 , 194 , 195 , 197 , 199 , 201 , 202 , 204 , 206 , 208 , 
210 , 211 , 213 , 215 , 217 , 219 , 221 , 223 , 224 , 226 , 
228 , 230 , 232 , 234 , 236 , 238 , 240 , 242 , 244 , 246 , 
248 , 250 , 252 , 253 ,
};


volatile uint8_t portBStates[pwmSteps];  // The bit state of each port output, per PWM step.
volatile uint8_t portDStates[pwmSteps];

void SoftPWM::setupOutputPins() {
  // Set all pins to output
  for(uint8_t pwm = 0; pwm < pwmCount; pwm++) {
    switch(pwmPorts[pwm]) {
      case 1: // port B
        bitSet(DDRB, pwmBits[pwm]);
        break;
      case 3: // port D
        bitSet(DDRD, pwmBits[pwm]);
        break;
    }
  }
}

void SoftPWM::setupTimer() {
  // Configure for fast PWM, reset at OCR3A, clock /1
  TCCR3A = (1 << WGM31) | (1 << WGM30);
  TCCR3B = (1 << WGM33) | (1 << WGM32) | (0 << CS32) | (0 << CS31) | (1 << CS30);
  OCR3A = 0x0010;  // Set the compare register so that the interrupt will trigger soon

  // turn on timer3 interrupt
  // TODO: Why doo,esn't OCF3A work?
  bitSet(TIMSK3, TOIE3);
}

void SoftPWM::begin() {
  setupOutputPins();

  // TODO: memset the portnStates tables?
  for(uint8_t pwm = 0; pwm < pwmCount; pwm++) {
    write(pwm, 0);
  }
  
  setupTimer();
}

void recalculatePWM(uint8_t pwm) {
  for(uint8_t step = 0; step < pwmSteps; step++) {  // TODO: Use 8-bit maths
      switch(pwmPorts[pwm]) {
          case 1: // port B
              if(pwmValues[pwm] > step) {
                  bitSet(portBStates[step], pwmBits[pwm]);
              }
              else {
                  bitClear(portBStates[step], pwmBits[pwm]);
              }
              break;
          case 3: // port F
              if(pwmValues[pwm] > step) {
                  bitSet(portDStates[step], pwmBits[pwm]);
              }
              else {
                  bitClear(portDStates[step], pwmBits[pwm]);
              }
              break;
      }
  }
}


void SoftPWM::write(uint8_t pwm, uint8_t value) {
  pwmValues[pwm] = value;
  
  recalculatePWM(pwm);
}

uint8_t SoftPWM::read(uint8_t pwm) {
  return pwmValues[pwm];
}


ISR(TIMER3_OVF_vect) {
  if(pwmStep == 0) {
    PORTB = portBStates[0];  // Write out the pre-computed port states
//    PORTD = portDStates[0];  

    PORTB = portBStates[1];  // Write out the pre-computed port states
//    PORTD = portDStates[1];
    
    PORTB = portBStates[2];  // Write out the pre-computed port states
//    PORTD = portDStates[2];
    
    PORTB = portBStates[3];  // Write out the pre-computed port states
//    PORTD = portDStates[3];
    
    PORTB = portBStates[4];  // Write out the pre-computed port states
//    PORTD = portDStates[4];
    
    PORTB = portBStates[5];  // Write out the pre-computed port states
//    PORTD = portDStates[5];  

    PORTB = portBStates[6];  // Write out the pre-computed port states
//    PORTD = portDStates[6];
    
    PORTB = portBStates[7];  // Write out the pre-computed port states
//    PORTD = portDStates[7];
    
    PORTB = portBStates[8];  // Write out the pre-computed port states
//    PORTD = portDStates[8];
    
    PORTB = portBStates[9];  // Write out the pre-computed port states
//    PORTD = portDStates[9];
    
    PORTB = portBStates[10];  // Write out the pre-computed port states
//    PORTD = portDStates[10];  

    PORTB = portBStates[11];  // Write out the pre-computed port states
//    PORTD = portDStates[11];
    
    PORTB = portBStates[12];  // Write out the pre-computed port states
//    PORTD = portDStates[12];
    
    PORTB = portBStates[13];  // Write out the pre-computed port states
//    PORTD = portDStates[13];
    
    PORTB = portBStates[14];  // Write out the pre-computed port states
//    PORTD = portDStates[14];
    
    PORTB = portBStates[15];  // Write out the pre-computed port states
//    PORTD = portDStates[15];  

    PORTB = portBStates[16];  // Write out the pre-computed port states
//    PORTD = portDStates[16];
    
    PORTB = portBStates[17];  // Write out the pre-computed port states
//    PORTD = portDStates[17];
    
    PORTB = portBStates[18];  // Write out the pre-computed port states
//    PORTD = portDStates[18];
    
    PORTB = portBStates[19];  // Write out the pre-computed port states
//    PORTD = portDStates[19];
    
    PORTB = portBStates[20];  // Write out the pre-computed port states
//    PORTD = portDStates[20];  

    PORTB = portBStates[21];  // Write out the pre-computed port states
//    PORTD = portDStates[21];
    
    PORTB = portBStates[22];  // Write out the pre-computed port states
//    PORTD = portDStates[22];
    
    pwmStep = 22;
  }
  else {
    PORTB = portBStates[pwmStep];  // Write out the pre-computed port states
//    PORTD = portDStates[pwmStep];
    pwmStep += 1;
  }
  
  OCR3AL = timerCounts[pwmStep-1];  // Set timer to next increment
  
  if(pwmStep >= pwmSteps) {
    pwmStep = 0;
  }
}
