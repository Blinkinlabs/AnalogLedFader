//#include <avr/interrupt.h>

#include "SoftPWM.h"

#define PIN_DE           3    // Set this high to enable transmission on the RS485 line
#define PIN_LED_DEBUG    11

SoftPWM softPWM;

void setup() {
//  pinMode(PIN_DE, OUTPUT);  // enable RS485 input
//  digitalWrite(PIN_DE, 0);

//  pinMode(PIN_LED_DEBUG, OUTPUT);
//  digitalWrite(PIN_LED_DEBUG, LOW);

  Serial.begin(57600);
  
  softPWM.begin();
}

void loop() {
  if(Serial.available()) {
    char c = Serial.read(); 
//    pwmValues[0] = c;
//    pwmValues[1] = c;
//    pwmValues[2] = c;
  }

  softPWM.setPWM(0, softPWM.readPWM(0) + 1);
  softPWM.setPWM(2, softPWM.readPWM(2) + 1);
}


