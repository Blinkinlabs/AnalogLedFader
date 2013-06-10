#include "HardPWM.h"
#include "Protocol.h"

// Address that this device responds to. Change this for each board.
#define DEVICE_ADDRESS   12

// System baud rate. Leave at 250000 (13 boards * 6 channels * 16 bits/channel * 60fps = 74880baud minimum)
#define BAUD_RATE 250000

#define PIN_STATUS_LED   3

HardPWM hardPWM;

Protocol usbReceiver;
Protocol rs485Receiver;

void setup() {
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, HIGH);

  // USB input (The baud rate specified here doesn't matter)
  Serial.begin(115200);
  usbReceiver.reset();

  // RS485 input
  Serial1.begin(BAUD_RATE);
  rs485Receiver.reset();

  // Start the PWM engine
  hardPWM.begin();
}

void handleData(uint8_t dataSize, uint16_t* data) {
  // The data size is nonzero only if we already received a full packet.
  if(dataSize >= (DEVICE_ADDRESS + 1)*pwmCount*2) {
    for(uint8_t i = 0; i < pwmCount; i++) {
      hardPWM.write(i, data[i + DEVICE_ADDRESS*pwmCount]);
    }
  }
}


void loop() {

  // Handle incoming data from USB
  if(Serial.available()) {
    digitalWrite(PIN_STATUS_LED, LOW);
    
    if(usbReceiver.parseByte(Serial.read())) {
      uint8_t dataSize = usbReceiver.getPacketSize();
      uint16_t* data = usbReceiver.getPacket16();
      handleData(dataSize, data);
    }
  }

  // Handle incoming data from RS485  
  if(Serial1.available()) {
    digitalWrite(PIN_STATUS_LED, LOW);
    
    if(rs485Receiver.parseByte(Serial1.read())) {
      uint8_t dataSize = rs485Receiver.getPacketSize();
      uint16_t* data = rs485Receiver.getPacket16();
      handleData(dataSize, data);
    }
    digitalWrite(PIN_STATUS_LED, HIGH);
  }
}




