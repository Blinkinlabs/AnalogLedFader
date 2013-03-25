//#include <avr/interrupt.h>
//#include "SoftPWM.h"
#include "HardPWM.h"
#include "Protocol.h"

// Address that this device responds to
#define DEVICE_ADDRESS   8



#define PIN_STATUS_LED   3

HardPWM hardPWM;

Protocol usbReceiver;
Protocol rs485Receiver;

void setup() {
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, HIGH);

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
      uint8_t dataSize = usbReceiver.getPacketSize();
      uint16_t* data = usbReceiver.getPacket16();
      
      if(dataSize >= (DEVICE_ADDRESS+1)*(8*2)) {
        for(uint8_t i = 0; i < pwmCount; i++) {
          hardPWM.write(i, data[i]);
        }
      }
    }
  }

  // Handle incoming data from RS485  
  if(Serial1.available()) {
    if(rs485Receiver.parseByte(Serial1.read())) {
      uint16_t* data = rs485Receiver.getPacket16();
      
      if(rs485Receiver.getPacketSize() >= (DEVICE_ADDRESS+8)*2) {
        for(uint8_t i = 0; i < pwmCount; i++) {
          hardPWM.write(i, data[i]);
        }
      }
    }
  }
}




