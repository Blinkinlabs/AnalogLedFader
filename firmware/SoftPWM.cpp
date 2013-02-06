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
maxValue = 300
for i in range(0,divs):
  print int((i/(steps - 1.0))*(i/(steps - 1.0))*maxValue)+1, ',',
  if(i%10 == 9):
    print ''
*/
static uint16_t timerCounts[pwmSteps - 1] = {
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 
1 , 1 , 2 , 2 , 2 , 2 , 3 , 3 , 3 , 3 , 
4 , 4 , 4 , 5 , 5 , 5 , 6 , 6 , 7 , 7 , 
8 , 8 , 9 , 9 , 10 , 10 , 11 , 11 , 12 , 13 , 
13 , 14 , 14 , 15 , 16 , 17 , 17 , 18 , 19 , 20 , 
20 , 21 , 22 , 23 , 24 , 25 , 25 , 26 , 27 , 28 , 
29 , 30 , 31 , 32 , 33 , 34 , 35 , 36 , 37 , 38 , 
39 , 41 , 42 , 43 , 44 , 45 , 46 , 48 , 49 , 50 , 
51 , 53 , 54 , 55 , 56 , 58 , 59 , 61 , 62 , 63 , 
65 , 66 , 68 , 69 , 71 , 72 , 74 , 75 , 77 , 78 , 
80 , 81 , 83 , 85 , 86 , 88 , 90 , 91 , 93 , 95 , 
97 , 98 , 100 , 102 , 104 , 105 , 107 , 109 , 111 , 113 , 
115 , 117 , 119 , 121 , 123 , 125 , 126 , 129 , 131 , 133 , 
135 , 137 , 139 , 141 , 143 , 145 , 147 , 149 , 152 , 154 , 
156 , 158 , 161 , 163 , 165 , 167 , 170 , 172 , 174 , 177 , 
179 , 181 , 184 , 186 , 189 , 191 , 194 , 196 , 199 , 201 , 
204 , 206 , 209 , 211 , 214 , 217 , 219 , 222 , 224 , 227 , 
230 , 233 , 235 , 238 , 241 , 244 , 246 , 249 , 252 , 255 , 
258 , 260 , 263 , 266 , 269 , 272 , 275 , 278 , 281 , 284 , 
287 , 290 , 293 , 296 , 299 , 302 , 305 , 308 , 312 , 315 , 
318 , 321 , 324 , 328 , 331 , 334 , 337 , 341 , 344 , 347 , 
350 , 354 , 357 , 361 , 364 , 367 , 371 , 374 , 378 , 381 , 
385 , 388 , 392 , 395 , 399 , 402 , 406 , 409 , 413 , 417 , 
420 , 424 , 428 , 431 , 435 , 439 , 443 , 446 , 450 , 454 , 
458 , 461 , 465 , 469 , 473 , 477 , 481 , 485 , 489 , 493 , 
497 , 500 , 504 , 508 ,
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
    setPWM(pwm, 0);
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


void SoftPWM::setPWM(uint8_t pwm, uint8_t value) {
  pwmValues[pwm] = value;
  
  recalculatePWM(pwm);
}

uint8_t SoftPWM::readPWM(uint8_t pwm) {
  return pwmValues[pwm];
}


ISR(TIMER3_OVF_vect) {
  PORTB = portBStates[pwmStep];  // Write out the pre-computed port states
  PORTD = portDStates[pwmStep];

  OCR3A = timerCounts[pwmStep];  // Set timer to next increment
  
  pwmStep += 1;  // Then increment the counter (it will auto-wrap)
  if(pwmStep >= pwmSteps) {
    pwmStep = 0;
  }
}
