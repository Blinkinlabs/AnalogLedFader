//#include <avr/interrupt.h>

#include "SoftPWM.h"
#include "Protocol.h"

#define PIN_DE           3    // Set this high to enable transmission on the RS485 line
#define PIN_LED_DEBUG    11

SoftPWM softPWM;
Protocol receiver;

void setup() {
  //  pinMode(PIN_DE, OUTPUT);  // enable RS485 input
  //  digitalWrite(PIN_DE, 0);

  //  pinMode(PIN_LED_DEBUG, OUTPUT);
  //  digitalWrite(PIN_LED_DEBUG, LOW);

  Serial.begin(57600);
  receiver.reset();

  softPWM.begin();
  softPWM.setPWM(0, 100);
}

void loop() {
  if(Serial.available()) {
    char c = Serial.read(); 
    if(receiver.parseByte(c)) {
      uint8_t* data = receiver.getPacket();
      softPWM.setPWM(0, data[0]);
      softPWM.setPWM(1, data[1]);
    }
  }
}



