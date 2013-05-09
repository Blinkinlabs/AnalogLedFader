#ifndef _PROTOCOL_h
#define _PROTOCOL_h

#include <Arduino.h>

#define MAX_DATA_SIZE       250  // Maximum length of the data packet we can receive (up to 254)

#define MODE_HEADER0        0x0
#define MODE_HEADER1        0x1
#define MODE_LENGTH         0x2
#define MODE_PAYLOAD        0x3
#define MODE_CRC            0x4
#define MODE_PAYLOAD_READY  0x5

#define HEADER0             0xde
#define HEADER1             0xad

// AnalogLED protocol decoder. Expects frames of data that look like this:
// [header (2 bytes)][length (1 byte)][payload (length bytes)][ibutton crc (1 byte)]
class Protocol {
private:
  uint8_t m_expectedLength;                ///< Number of bytes expected in the current packet  
  uint8_t m_mode;                          ///< Current operational mode
  uint8_t m_packetData[MAX_DATA_SIZE];     ///< Data received in this packet
  uint8_t m_packetLength;                  ///< Number of bytes received for this packet
  uint8_t m_crc;                           ///< CRC calculated for this packet

  // add a new byte to the CRC
  void updateCRC(uint8_t data);

  // store a new byte in the packet
  void addByte(uint8_t data);
public:
  // Initialize the packet processor
  void init();
  
  // Reset the packet processor state machine
  void reset();
  
  // Read a byte into the packet processor, and handle it as possible.
  // @param data Data byte to read in
  // @return true if a payload is ready to read
  boolean parseByte(uint8_t data);
  
  // Get the packet size (in bytes)
  uint8_t getPacketSize();
  
  // If a full packet was received, returns the address to the data and resets the
  // packet state machine. Otherwise, returns null
  uint8_t* getPacket();

  // If a full packet was received, returns the address to the data as a 16-bit array,
  // and resets the packet state machine. Otherwise, returns null
  uint16_t* getPacket16();
};

#endif

