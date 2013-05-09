// TODO: Make this threaded! We can't hit high framerates otherwise :-(

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
    // TODO: Sending serial over this setup is too slow. Probably need to switch to a bb8-style USB driver.
    
    //    byte data[] = new byte[2+numberOfChannels*2+1];
//
//    data[0] = (byte)0xde;
//    data[1] = (byte)0xad;
//    data[2] = (byte)(numberOfChannels*2);
//    for (int i = 0; i < numberOfChannels; i++) {
//      data[2+i*2] =   (byte)((values[i])      & 0xFF); // low byte
//      data[2+i*2+1] = (byte)((values[i] >> 8) & 0xFF); // high byte
//    }
//    data[2+numberOfChannels] = (byte)0xff;  // TODO: Implement CRC
//    this.outPort.write(data);

    m_outPort.write(0xde);
    m_outPort.write(0xad);
    m_outPort.write(m_numberOfChannels*2);
    for(int i = 0; i < m_numberOfChannels; i++) {
      m_outPort.write(values[i] & 0xFF);        // low byte
      m_outPort.write((values[i] >> 8) & 0xFF); // high byte
    }
    m_outPort.write(0xff);  // TODO: Implement CRC
  }
}

