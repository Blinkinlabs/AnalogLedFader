// TODO: Make this threaded! We can't hit high framerates otherwise :-(

class LedOutput
{
  private String portName;  // TODO: How to request cu.* devices?
  private Serial outPort;

  private int numberOfChannels;
  private byte data[];

  LedOutput(PApplet parent, String portName, int numberOfChannels) {
    this.portName = portName;
    this.numberOfChannels = numberOfChannels;
    
    this.data = new byte[2+numberOfChannels*2+1];

    println("Connecting to LED Fader on: " + portName);
    this.outPort = new Serial(parent, portName, 115200);
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

    this.outPort.write(0xde);
    this.outPort.write(0xad);
    this.outPort.write(numberOfChannels*2);
    for(int i = 0; i < numberOfChannels; i++) {
      this.outPort.write(values[i] & 0xFF);        // low byte
      this.outPort.write((values[i] >> 8) & 0xFF); // high byte
    }
    this.outPort.write(0xff);  // TODO: Implement CRC
  }
}

