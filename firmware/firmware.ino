//#include <avr/interrupt.h>
//#include "SoftPWM.h"
#include "HardPWM.h"
#include "Protocol.h"

#define PIN_DE           3    // Set this high to enable transmission on the RS485 line

//SoftPWM softPWM;
HardPWM hardPWM;
Protocol receiver;

void setup() {
  //  pinMode(PIN_DE, OUTPUT);  // enable RS485 input
  //  digitalWrite(PIN_DE, 0);

  Serial.begin(57600);
  receiver.reset();
  
  hardPWM.begin();
  hardPWM.write(4, 1);
  hardPWM.write(5, 2);
//  softPWM.begin();
//  softPWM.setPWM(0, 1);
//  softPWM.setPWM(1, 2);
}

void loop() {
  if(Serial.available()) {
    char c = Serial.read(); 
    if(receiver.parseByte(c)) {
      uint16_t* data = receiver.getPacket16();
      
      hardPWM.write(4, data[0]);
      hardPWM.write(5, data[1]);
      
//      softPWM.setPWM(0, data[0]);
//      softPWM.setPWM(1, data[1]);
    }
  }
}



