import controlP5.*;
import processing.serial.*;

ControlP5 cp5;

String VERSION_STRING = "0.1";

int NUMBER_OF_CHANNELS = 13*6;
int MAX_VALUE = 65535;

float globalBrightness = .1;

LedOutput led;
String portName;
int[] values; // Light values to send

void setup() {
  size(1600,650);
  frameRate(60);
  cp5 = new ControlP5(this);
  
  values = new int[NUMBER_OF_CHANNELS];
  
  // auto connect to the first arduino-like thing we find
  for(String p : Serial.list()) {
    if(p.startsWith("/dev/cu.usbmodem")) {
      portName = p;
      led = new LedOutput(this, p, NUMBER_OF_CHANNELS);
    }
  }
  
  // Position controls
  cp5.addToggle("rangeTest")
   .setPosition(10,10)
   ;
  
  cp5.addToggle("randomMotion")
   .setPosition(10,50)
   ;
   
  
  for(int i = 0; i < NUMBER_OF_CHANNELS; i++) {
    int speakersPerCol = 18;
    
    values[i] = 0;
    
    Slider s = cp5.addSlider("value[" + i + "]")
     .setPosition(150 + (i/speakersPerCol)*250,10+(i%speakersPerCol)*35)
     .setSize(150,30)
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
  
  led.sendUpdate(values);
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

