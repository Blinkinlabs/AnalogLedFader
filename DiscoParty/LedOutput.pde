// TODO: Make this threaded! We can't hit high framerates otherwise :-(

class LedOutput
{
  private String portName;  // TODO: How to request cu.* devices?
  private Serial outPort;
  
  private int numberOfChannels;
  
  LedOutput(PApplet parent, String portName, int numberOfChannels) {
    this.portName = portName;
    this.numberOfChannels = numberOfChannels;
    
    println("Connecting to LED Fader on: " + portName);
    this.outPort = new Serial(parent, portName, 115200);
  }

  void sendUpdate(int[] values) {
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
