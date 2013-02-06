//#include <avr/interrupt.h>

#define PIN_DE           3    // Set this high to enable transmission on the RS485 line

#define PIN_LED_WHITE    9
#define PIN_LED_BLUE     10
#define PIN_LED_DEBUG    11

const uint8_t pwmCount = 4;

const uint8_t pwmPorts[pwmCount] = {1, 1, 1, 3};  // Port that the PWM output is connected to (1=B, 2=C, etc)
const uint8_t pwmBits[pwmCount]  = {0, 1, 2, 6};  // Bit location of the PWM output in the port

uint8_t pwmValues[pwmCount];  // currrent logarithmic PWM setting for each PWM output

const uint16_t pwmSteps = 255;  // Number of PWM steps available. Must be 256.

volatile uint8_t pwmStep;  // current step in the PWM counter

// Logarithmic counts for an 8-bit timer
// Generated using python:
// for i in range(0,256):
//   print int((i/255.0)*(i/255.0)*254)+1, ',',
//   if(i%10 == 9):
//     print ''
static uint8_t timerCounts[pwmSteps] = {
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 
1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 
2 , 2 , 2 , 3 , 3 , 3 , 3 , 3 , 4 , 4 , 
4 , 4 , 4 , 5 , 5 , 5 , 6 , 6 , 6 , 6 , 
7 , 7 , 7 , 8 , 8 , 8 , 9 , 9 , 9 , 10 , 
10 , 11 , 11 , 11 , 12 , 12 , 13 , 13 , 14 , 14 , 
15 , 15 , 16 , 16 , 16 , 17 , 18 , 18 , 19 , 19 , 
20 , 20 , 21 , 21 , 22 , 22 , 23 , 24 , 24 , 25 , 
25 , 26 , 27 , 27 , 28 , 29 , 29 , 30 , 31 , 31 , 
32 , 33 , 34 , 34 , 35 , 36 , 36 , 37 , 38 , 39 , 
40 , 40 , 41 , 42 , 43 , 44 , 44 , 45 , 46 , 47 , 
48 , 49 , 49 , 50 , 51 , 52 , 53 , 54 , 55 , 56 , 
57 , 58 , 59 , 60 , 61 , 62 , 63 , 64 , 64 , 66 , 
67 , 68 , 69 , 70 , 71 , 72 , 73 , 74 , 75 , 76 , 
77 , 78 , 79 , 80 , 81 , 83 , 84 , 85 , 86 , 87 , 
88 , 90 , 91 , 92 , 93 , 94 , 96 , 97 , 98 , 99 , 
100 , 102 , 103 , 104 , 106 , 107 , 108 , 109 , 111 , 112 , 
113 , 115 , 116 , 117 , 119 , 120 , 121 , 123 , 124 , 126 , 
127 , 128 , 130 , 131 , 133 , 134 , 136 , 137 , 139 , 140 , 
142 , 143 , 144 , 146 , 148 , 149 , 151 , 152 , 154 , 155 , 
157 , 158 , 160 , 161 , 163 , 165 , 166 , 168 , 169 , 171 , 
173 , 174 , 176 , 178 , 179 , 181 , 183 , 184 , 186 , 188 , 
190 , 191 , 193 , 195 , 196 , 198 , 200 , 202 , 204 , 205 , 
207 , 209 , 211 , 213 , 214 , 216 , 218 , 220 , 222 , 224 , 
225 , 227 , 229 , 231 , 233 , 235 , 237 , 239 , 241 , 243 , 
245 , 247 , 249 , 251 , 253
};


volatile uint8_t portBStates[pwmSteps];  // The bit state of each port output, per PWM step.
volatile uint8_t portDStates[pwmSteps];

void startPWM() {
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
  
  // Configure for fast PWM, reset at OCR3A, clock /1
  TCCR3A = (1 << WGM31) | (1 << WGM30);
  TCCR3B = (1 << WGM33) | (1 << WGM32) | (0 << CS32) | (0 << CS31) | (1 << CS30);
  OCR3AH = 0x00;  // Set the high byte to 0, we want to work in 8-bit mode
  OCR3AL = 0x01;  // And trigger when the counter reaches this value.

  // turn on timer3 interrupt
  // TODO: Why doo,esn't OCF3A work?
  bitSet(TIMSK3, TOIE3);
}

// Re-populate the portnStates tables
void recalculatePwms() {
    for(uint8_t pwm = 0; pwm < pwmCount; pwm++) {
        for(uint16_t step = 0; step < pwmSteps; step++) {  // TODO: Use 8-bit maths
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
}

// Reset the PWM state machine so that all PWMs are turned off
void resetPWM() {
    for(uint8_t pwm = 0; pwm < pwmCount; pwm++) {
        pwmValues[pwm] = 0;
    }
    
    recalculatePwms();
}

void setup() {
//  pinMode(PIN_DE, OUTPUT);  // enable RS485 input
//  digitalWrite(PIN_DE, 0);

//  pinMode(PIN_LED_DEBUG, OUTPUT);
//  digitalWrite(PIN_LED_DEBUG, LOW);

  Serial.begin(57600);

  resetPWM();  // Clear the PWM state machine

  startPWM();  // Initialize pins and start the PWM machine

  // Set some default values to the PWM machine
  pwmValues[0] = 127;
  pwmValues[2] = 0;
  recalculatePwms();
}

void loop() {
  if(Serial.available()) {
    char c = Serial.read(); 
//    pwmValues[0] = c;
//    pwmValues[1] = c;
//    pwmValues[2] = c;
  }

//  pwmValues[0]=2;
  pwmValues[0]++;
  pwmValues[2]++;
  recalculatePwms();
//  pwmValues[4]++;
//  pwmValues[5]++;
//  delay(10);
}

ISR(TIMER3_OVF_vect) {
  PORTB = portBStates[pwmStep];  // Write out the pre-computed port states
  PORTD = portDStates[pwmStep];

  OCR3AL = timerCounts[pwmStep]; // Set timer to next increment
  
  pwmStep += 1;  // Then increment the counter (it will auto-wrap)
  if(pwmStep >= pwmSteps) {
    pwmStep = 0;
  }
}
