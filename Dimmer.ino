#include <ESP8266_HardSer.h>
#include <BlynkSimpleShieldEsp8266_HardSer.h>
#define EspSerial Serial1
#include  <TimerOne.h>          // Avaiable from http://www.arduino.cc/playground/Code/Timer1
volatile int i=0;               // Variable to use as a counter volatile as it is in an interrupt
volatile boolean zero_cross=0;  // Boolean to store a "switch" to tell us if we have crossed zero
int AC_pin = 10;                // Output to Opto Triac
int brightness = 128;           // Dimming level (0-128)  0 = on, 128 = 0ff
int freqStep = 75;    // This is the delay-per-brightness step in microseconds.

ESP8266 wifi(EspSerial);

char auth[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

void setup() {                                    
  pinMode(AC_pin, OUTPUT);                          // Set the Triac pin as output
  attachInterrupt(0, zero_cross_detect, RISING);    // Attach an Interupt to Pin 3 (interupt 0) 
  Timer1.initialize(freqStep);                      // Initialize TimerOne library 
  Timer1.attachInterrupt(dim_check, freqStep);   
  
  Serial.begin(9600);
  delay(10);

  EspSerial.begin(9600);
  delay(10);
  
  Blynk.begin(auth, wifi,"xxxxxx","xxxxxxxxxxxx","xxx.xxx.x.xx");
  delay(10);  
                                      
}

BLYNK_WRITE(V1)//      slider brillo
{
 int brillo = param.asInt(); 
 brightness=brillo;
}

void zero_cross_detect() {    
  zero_cross = true;               
  i=0;
  digitalWrite(AC_pin, LOW);       // turn off TRIAC (and AC)
}                                 

// Turn on the TRIAC at the appropriate time
void dim_check() {                   
  if(zero_cross == true) {              
    if(i>=brightness) {                     
      digitalWrite(AC_pin, HIGH); // turn on light       
      i=0;  // reset time step counter                         
      zero_cross = false; //reset zero cross detection
    } 
    else {
      i++; // increment time step counter                     
    }                                
  }                                  
}                                   

void loop() {                        
Blynk.run();
}
