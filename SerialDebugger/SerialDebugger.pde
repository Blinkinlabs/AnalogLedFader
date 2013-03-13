import controlP5.*;

import processing.serial.*;

ControlP5 cp5;
Serial outPort;

String VERSION_STRING = "0.1";

int NUMBER_OF_CHANNELS = 6;
int MAX_VALUE = 65535;

int[] values; // Light values to send

void setup() {
  size(600,300);
  frameRate(30);
  cp5 = new ControlP5(this);
  
  values = new int[NUMBER_OF_CHANNELS];
  
  String portName = "/dev/cu.usbmodemfd121";  // TODO: How to request cu.* devices?
  
  println(portName);
  outPort = new Serial(this, portName, 38400);
  
  // Position controls
  cp5.addToggle("rangeTest")
   .setPosition(10,10)
   ;
  
  cp5.addToggle("randomMotion")
   .setPosition(10,50)
   ;
   
  
  for(int i = 0; i < NUMBER_OF_CHANNELS; i++) {
    int speakersPerCol = 17;
    
    values[i] = 0;
    
    Slider s = cp5.addSlider("value[" + i + "]")
     .setPosition(150 + (i/speakersPerCol)*250,10+(i%speakersPerCol)*35)
     .setSize(300,30)
     .setRange(0,MAX_VALUE)
     .setSliderMode(Slider.FLEXIBLE)
     .setDecimalPrecision(0)
     .setValue(0)
     ;

    s.setValue(values[i]);
    s.setId(i);
  }
  
  // Debug info
  cp5.addTextlabel("label1")
    .setText("Debugger version " + VERSION_STRING)
    .setPosition(10,265)
    ;

  if(portName != "") {
    cp5.addTextlabel("label2")
     .setText("Transmitting on " + portName)
     .setPosition(10,280)
     ;
  } else {
    cp5.addTextlabel("label2")
     .setText("Could not find a port to transmit on!")
     .setPosition(10,280)
     ;
  }   
}


void sendUpdate() {
  outPort.write(0xde);
  outPort.write(0xad);
  outPort.write(NUMBER_OF_CHANNELS*2);
  for(int i = 0; i < NUMBER_OF_CHANNELS; i++) {
    outPort.write(values[i] & 0xFF);        // low byte
    outPort.write((values[i] >> 8) & 0xFF); // high byte
  }
  outPort.write(0xff);  // TODO: Implement CRC
}

boolean rangeTest = false;
boolean randomMotion = false;

float   rangeTestToggleTime = 0;
boolean rangeTestDirection  = false;

void draw() {
  background(0);
  stroke(255);
  
  if(rangeTest) {
    if(rangeTestToggleTime == 0) {
      rangeTestToggleTime = millis();
    }
    
    if(millis() > rangeTestToggleTime) {
      int newPosition;
      if(rangeTestDirection) {
        newPosition = 0;
      }
      else {
        newPosition = MAX_VALUE;
      }
      
      for(int i = 0; i < NUMBER_OF_CHANNELS; i++) {
        values[i] = newPosition;
      }

      rangeTestToggleTime = millis() + 100;
      rangeTestDirection = !rangeTestDirection;
    }
  }
  
  if(randomMotion) {
    values[(int)random(0,NUMBER_OF_CHANNELS)] = (int)random(0,MAX_VALUE);
  }
  
  sendUpdate();
}

void controlEvent(ControlEvent theEvent) {
  if (theEvent.isController()) {    
    // check if theEvent is coming from a position controller
    if (theEvent.controller().name().startsWith("value")) {
      // get the id of the controller and map the value
      // to an element inside the boxsize array.
      int id = theEvent.controller().id();
      if (id>=0 && id<values.length) {
        values[id] = (int)theEvent.value();
      }
    }
  }
}

