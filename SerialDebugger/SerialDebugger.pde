import processing.serial.*;

Serial outPort;

void setup() {
  size(1024,300);
  frameRate(30);
  
  String portName = "/dev/cu.usbmodem12341";
  
  println(portName);
  outPort = new Serial(this, portName, 115200);
}

void send10Bit(int a) {
  outPort.write(a & 0xFF);  // Low byte
  outPort.write((a >> 8) & 0x03);  // High byte
}

void sendUpdate(int a, int b) {
  outPort.write(0xde);
  outPort.write(0xad);
  outPort.write(2*2);
  send10Bit(a);
  send10Bit(b);
  outPort.write(0xff);
}


int vala = 0;
int valb = 0;

void draw() {
  sendUpdate(int(map(mouseX,0,width,0,1023)), int(map(mouseY,0,width,0,1023)));
  println(int(map(mouseX,0,width,0,1023)));

  if(vala == 1) {
    vala = 0;
    valb = 0;
  } else {
    vala = 0;
    valb = 150;
  }
}
