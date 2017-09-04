#define BLYNK_PRINT Serial
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
BlynkTimer timer;
char auth[] = "YourAuthToken";
unsigned int myEthernetTimeout =   5000;  // Ethernet Connection Timeout (ECT)
unsigned int blynkInterval     =  25000;  // Check Server Frequency      (CSF)
char server[]=    "blynk-cloud.com"; // could use "blynk-cloud.com" or an IP address
#define W5100_CS  10
#define SDCARD_CS 4

void setup()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card
  timer.setInterval(myEthernetTimeout, myfunction);
  timer.setInterval(blynkInterval, checkBlynk);   // check connection to server per blynkInterval     
  //Blynk.begin(auth);
  Blynk.config(auth, server);  // this is OK
  Blynk.connect();             // this is OK
  
}

void myfunction(){
  Serial.println("\tLook, no Blynk block.");
  if(Blynk.connected()){
    Serial.println("\tEthernet still connected.");
    Blynk.virtualWrite(V11, millis() / 1000);
  }
}

void checkBlynk() {
  unsigned long startConnecting = millis();    
  while(!Blynk.connected()){
    Blynk.connect();  
    if(millis() > startConnecting + myEthernetTimeout){
      Serial.println("\tUnable to connect to server. ");
      break;
    }
  }
  Serial.print("\tChecking again in ");
  Serial.print(blynkInterval / 1000.0);
  Serial.println("s.");
}

void loop()
{
  if (Blynk.connected()) {
    Blynk.run();
  }
  timer.run();
}
