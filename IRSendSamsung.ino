//Turn Samsung TV on/off repeatedly to test program. IR LED connected to pin 3.

#include <IRremote.h>

#define POWER         0xE0E040BF            //from IRrecvDemo, Ken Shirriff
#define SAMSUNG_BITS  32 

IRsend irsend;

void setup()
{
  pinMode (3, OUTPUT);  //output as used in library
}

void loop() {

  irsend.sendSAMSUNG(POWER, SAMSUNG_BITS); 

  delay (5000);

}//end of loop
