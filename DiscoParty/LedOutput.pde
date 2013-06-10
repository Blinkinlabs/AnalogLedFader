class LedOutput
{
  private String m_portName;
  private Serial m_outPort;

  private int m_numberOfChannels;
  private byte m_data[];

  LedOutput(PApplet parent, String portName, int numberOfChannels) {
    m_portName = portName;
    m_numberOfChannels = numberOfChannels;

    m_data = new byte[2+numberOfChannels*2+1];

    println("Connecting to LED Fader on: " + portName);
    m_outPort = new Serial(parent, portName, 115200);
  }


  void sendUpdate(int[] values) {
    byte data[] = new byte[3+m_numberOfChannels*2+1];

    data[0] = (byte)0xde;
    data[1] = (byte)0xad;
    data[2] = (byte)(m_numberOfChannels*2);
    for (int i = 0; i < m_numberOfChannels; i++) {
      data[3+i*2] =   (byte)((values[i])      & 0xFF); // low byte
      data[3+i*2+1] = (byte)((values[i] >> 8) & 0xFF); // high byte
    }

    IButtonCrc crc = new IButtonCrc();
    for (int i = 0; i < 2+m_numberOfChannels*2; i++) {
      crc.update(data[i]);
    }
    
    data[2+m_numberOfChannels*2] = crc.getCrc();  // TODO: Implement CRC

    // TODO: chunk here?
    m_outPort.write(data);
  }
}

/**
 * This is a Java implementation of the IButton/Maxim 8-bit CRC. Code ported
 * from the AVR-libc implementation, which is used on the RR3G end.
 * Taken from the ReplicatorG sources:
 * https://github.com/cibomahto/ReplicatorG/blob/master/src/replicatorg/app/tools/IButtonCrc.java
 */
public class IButtonCrc {

  private int crc = 0;

  /**
   	 * Construct a new, initialized object for keeping track of a CRC.
   	 */
  public IButtonCrc() {
    crc = 0;
  }

  /**
   	 * Update the CRC with a new byte of sequential data. See
   	 * include/util/crc16.h in the avr-libc project for a full explanation of
   	 * the algorithm.
   	 * 
   	 * @param data
   	 *            a byte of new data to be added to the crc.
   	 */
  public void update(byte data) {
    crc = (crc ^ data) & 0xff; // i loathe java's promotion rules
    for (int i = 0; i < 8; i++) {
      if ((crc & 0x01) != 0) {
        crc = ((crc >>> 1) ^ 0x8c) & 0xff;
      } 
      else {
        crc = (crc >>> 1) & 0xff;
      }
    }
  }

  /**
   	 * Get the 8-bit crc value.
   	 */
  public byte getCrc() {
    return (byte) crc;
  }

  /**
   	 * Reset the crc.
   	 */
  public void reset() {
    crc = 0;
  }
}

