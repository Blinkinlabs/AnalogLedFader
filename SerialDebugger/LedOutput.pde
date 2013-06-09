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
      data[3+i*2] =   (byte)(( int(values[i]*globalBrightness))       & 0xFF); // low byte
      data[3+i*2+1] = (byte)(((int(values[i]*globalBrightness)) >> 8) & 0xFF); // high byte
    }
    data[3+m_numberOfChannels*2] = (byte)0xff;  // TODO: Implement CRC
    m_outPort.write(data);
  }
}

