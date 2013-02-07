#ifndef _PROTOCOL_h
#define _PROTOCOL_h

#include <Arduino.h>

#define MAX_DATA_SIZE       50       // Arbitrary
#define WRAPPER_SIZE        2+1+2    // 2 header, 1 length, 2 crc
#define MAX_PACKETLENGTH    MAX_DATA_SIZE + WRAPPER_SIZE // arbitrary?

#define MODE_HEADER0        0x0
#define MODE_HEADER1        0x1
#define MODE_LENGTH         0x2
#define MODE_PAYLOAD        0x3
#define MODE_CRC            0x4
#define MODE_PAYLOAD_READY  0x5

#define HEADER0             0xde
#define HEADER1             0xad

class Protocol {
private:
  uint8_t m_address;                       ///< Device address

  uint8_t m_expectedLength;                ///< Number of bytes expected in the current packet  
  uint8_t m_mode;                          ///< Current operational mode
  uint8_t m_packetData[MAX_PACKETLENGTH];  ///< Data received in this packet
  uint8_t m_packetLength;                  ///< Number of bytes received for this packet
  uint8_t m_crc;                           ///< CRC calculated for this packet

  // add a new byte to the CRC
  void updateCRC(uint8_t data);

  // store a new byte in the packet
  void addByte(uint8_t data);
public:
  // Initialize the packet processor
  // @param address Device address (must be less than MAX_ADDRESS)
  void init(uint8_t address);
  
  // Reset the packet processor state machine
  void reset();
  
  // Read a byte into the packet processor, and handle it as possible.
  // @param data Data byte to read in
  // @return true if a payload is ready to read
  boolean parseByte(uint8_t data);
  
  // Get the 
  uint8_t* getPacket();
};

#endif
