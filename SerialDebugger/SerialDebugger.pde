import processing.serial.*;

Serial outPort;

void setup() {
  size(300,300);
  
  String portName = "/dev/cu.usbmodem12341";
  
  println(portName);
  outPort = new Serial(this, portName, 115200);
}

void draw() {
  outPort.write(0xde);
  outPort.write(0xad);
  outPort.write(2);
  
  outPort.write((int)(mouseX/300.0*255));
  outPort.write((int)(mouseY/300.0*255));
  
  outPort.write(0xff);
}
