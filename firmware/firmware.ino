//#include <avr/interrupt.h>
//#include "SoftPWM.h"
#include "HardPWM.h"
#include "Protocol.h"

#define PIN_STATUS_LED   3

HardPWM hardPWM;

Protocol usbReceiver;
Protocol rs485Receiver;

void setup() {
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, HIGH);

  //  pinMode(PIN_DE, OUTPUT);  // enable RS485 input
  //  digitalWrite(PIN_DE, 0);

  // USB input
  Serial.begin(57600);
  usbReceiver.reset();

  // RS485 input
  Serial1.begin(38400);
  rs485Receiver.reset();

  hardPWM.begin();
}

void loop() {
  // Handle incoming data from USB
  if(Serial.available()) {
    if(usbReceiver.parseByte(Serial.read())) {
      uint16_t* data = usbReceiver.getPacket16();

      for(uint8_t i = 0; i < pwmCount; i++) {
        hardPWM.write(i, data[i]);
      }
    }
  }

  // Handle incoming data from RS485  
  if(Serial1.available()) {
    digitalWrite(PIN_STATUS_LED, HIGH);

    if(rs485Receiver.parseByte(Serial1.read())) {
      uint16_t* data = rs485Receiver.getPacket16();

      for(uint8_t i = 0; i < pwmCount; i++) {
        hardPWM.write(i, data[i]);
      }
    }
  }

  else {
    digitalWrite(PIN_STATUS_LED, LOW);
  }
}




