/***********************************************************
* DMX-512 Reception                                        *
* Developed by Max Pierson                                 *
* Version Rev15 9 Oct 2010                                 *
* Released under the WTFPL license, although I would       *
* appreciate Attribution and Share-Alike                   *
* See blog.wingedvictorydesign.com for the latest version. *
************************************************************/



/******************************* Addressing variable declarations *****************************/

#include <EEPROM.h>
#define NUMBER_OF_CHANNELS 8
//the number of channels we want to receive (8 by default).

#define SWITCH_PIN_0 11 //the pin number of our "0" switch
#define SWITCH_PIN_1 12 //the pin number of our "1" switch

unsigned int dmxaddress = 1;
/* The dmx address we will be listening to.  The value of this will be set in the Addressing()
*  function and read from EEPROM addresses 510 and 511.

/******************************* MAX485 variable declarations *****************************/

#define RECEIVER_OUTPUT_ENABLE 2
/* receiver output enable (pin2) on the max485.  
*  will be left low to set the max485 to receive data. */

#define DRIVER_OUTPUT_ENABLE 3
/* driver output enable (pin3) on the max485.  
*  will left low to disable driver output. */

#define RX_PIN 0   // serial receive pin, which takes the incoming data from the MAX485.
#define TX_PIN 1   // serial transmission pin

/******************************* DMX variable declarations ********************************/

volatile byte i = 0;              //dummy variable for dmxvalue[]
volatile byte dmxreceived = 0;    //the latest received value
volatile unsigned int dmxcurrent = 0;     //counter variable that is incremented every time we receive a value.
volatile byte dmxvalue[NUMBER_OF_CHANNELS];     
/*  stores the DMX values we're interested in using-- 
 *  keep in mind that this is 0-indexed. */
volatile boolean dmxnewvalue = false; 
/*  set to 1 when updated dmx values are received 
 *  (even if they are the same values as the last time). */

/******************************* Timer2 variable declarations *****************************/

volatile byte zerocounter = 0;          
/* a counter to hold the number of zeros received in sequence on the serial receive pin.  
*  When we've received a minimum of 11 zeros in a row, we must be in a break.  */




void setup() {
  
  /******************************* Max485 configuration ***********************************/
  
  pinMode(RECEIVER_OUTPUT_ENABLE, OUTPUT);
  pinMode(DRIVER_OUTPUT_ENABLE,   OUTPUT);
  digitalWrite(RECEIVER_OUTPUT_ENABLE, LOW);
  digitalWrite(DRIVER_OUTPUT_ENABLE,   LOW);    //sets pins 3 and 4 to low to enable reciever mode on the MAX485.

  pinMode(RX_PIN, INPUT);  //sets serial pin to receive data

//  /******************************* Addressing subroutine *********************************/
//
//  pinMode(SWITCH_PIN_0, INPUT);           //sets pin for '0' switch to input
//  digitalWrite(SWITCH_PIN_0, HIGH);       //turns on the internal pull-up resistor for '0' switch pin
//  pinMode(SWITCH_PIN_1, INPUT);           //sets pin for '1' switch to input  
//  digitalWrite(SWITCH_PIN_1, HIGH);       //turns on the internal pull-up resistor for '1' switch pin
//  
//  /* Call the addressing subroutine.  Three behaviors are possible:
//  *  1. Neither switch is pressed, in which case the value previously stored in EEPROM
//  *  510 and 511 is recalled,
//  *  2. Both switches are pressed, in which case the address is reset to 1.
//  *  3. Either switch is pressed (but not both), in which case the new address may 
//  *  be entered by the user.
//  */
//  //set this equal to a constant value if you just want to hardcode the address.
//  dmxaddress = Addressing();
  
  /******************************* USART configuration ************************************/
  
  Serial1.begin(250000);
  /* Each bit is 4uS long, hence 250Kbps baud rate */
  
  cli(); //disable interrupts while we're setting bits in registers
  
  bitClear(UCSR1B, RXCIE1);  //disable USART reception interrupt
  
  /******************************* Timer2 configuration ***********************************/
  
  //NOTE:  this will disable PWM on pins TODO WHICH PINS??
  bitClear(TCCR3A, COM3A1);
  bitClear(TCCR3A, COM3A0); //disable compare match output A mode
  bitClear(TCCR3A, COM3B1);
  bitClear(TCCR3A, COM3B0); //disable compare match output B mode
  bitClear(TCCR3A, COM3C1);
  bitClear(TCCR3A, COM3C0); //disable compare match output C mode
  bitClear(TCCR3A, WGM31);
  bitClear(TCCR3A, WGM30);  //set mode 2, CTC.  TOP will be set by OCRA.
  
  bitClear(TCCR3B, ICNC3);  //disable the input capture noise canceler

  bitClear(TCCR3B, WGM33);
  bitSet(  TCCR3B, WGM32);  //set mode 2, CTC.  TOP will be set by OCRA.
  bitClear(TCCR3B, CS32);
  bitClear(TCCR3B, CS31);
  bitSet(  TCCR3B, CS30);   // no prescaler means the clock will increment every 62.5ns (assuming 16Mhz clock speed).
  
  OCR3A = 64;                
  /* Set output compare register to 64, so that the Output Compare Interrupt will fire
  *  every 4uS.  */
  
  bitClear(TIMSK3, OCIE3B);  //Disable Timer/Counter2 Output Compare Match B Interrupt
  bitSet(  TIMSK3, OCIE3A);    //Enable Timer/Counter2 Output Compare Match A Interrupt
  bitClear(TIMSK3, TOIE3);   //Disable Timer/Counter2 Overflow Interrupt Enable          
  
  sei();                     //reenable interrupts now that timer2 has been configured. 


  /******************************* ActionLoop configuration ***********************************/
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  
}  //end setup()


void loop()  {
  // the processor gets parked here while the ISRs are doing their thing. 
  
  if (dmxnewvalue == 1) {    //when a new set of values are received, jump to action loop...
    action();
    dmxnewvalue = 0;
    dmxcurrent = 0;
    zerocounter = 0;      //and then when finished reset variables and enable timer2 interrupt
    i = 0;
    bitSet(TIMSK3, OCIE3A);    //Enable Timer/Counter2 Output Compare Match A Interrupt
  }
} //end loop()


//Timer2 compare match interrupt vector handler
ISR(TIMER3_COMPA_vect) {
  if (bitRead(PIND, PIND2)) {  // if a one is detected, we're not in a break, reset zerocounter.
    zerocounter = 0;
    }
  else {
    zerocounter++;             // increment zerocounter if a zero is received.
    if (zerocounter == 20)     // if 20 0's are received in a row (80uS break)
      {   
      bitClear(TIMSK3, OCIE3A);    //disable this interrupt and enable reception interrupt now that we're in a break.
      bitSet(  UCSR1B, RXCIE1);
      }
  }
} //end Timer2 ISR


ISR(USART1_RX_vect){
  dmxreceived = UDR1;
  /* The receive buffer (UDR1) must be read during the reception ISR, or the ISR will just 
  *  execute again immediately upon exiting. */
 
  dmxcurrent++;                        //increment address counter
 
  if(dmxcurrent > dmxaddress) {         //check if the current address is the one we want.
    dmxvalue[i] = dmxreceived;
    i++;
    if(i == NUMBER_OF_CHANNELS) {
      bitClear(UCSR1B, RXCIE1); 
      dmxnewvalue = 1;                        //set newvalue, so that the main code can be executed.
    } 
  }
} // end ISR
