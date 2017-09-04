// W5100NoBlock.ino for https://community.blynk.cc/t/brown-outs-or-reboots/16909
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#define W5100_CS  10
#define SDCARD_CS 4

char server[] = "blynk-cloud.com";
byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };  // Normally any MAC is OK

char auth[] = "YourAuthToken";                // ENTER YOUR TOKEN
IPAddress arduino_ip ( 192, 168,  10, 120);   // SET UP A STATIC IP
IPAddress dns_ip     ( 192, 168,  10,  90);   // ADD ROUTER IP FOR DNS
IPAddress gateway_ip ( 192, 168,  10,  90);   // ADD ROUTER IP FOR GATEWAY

IPAddress subnet_mask( 255, 255, 255,   0);   // Standard subnet mask

unsigned int myEthernetTimeout =   25000;     // Ethernet Connection Timeout (ECT)
unsigned int blynkInterval     =   30000;     // Check Server Frequency      (CSF)
unsigned long startConnecting = millis();

BlynkTimer timer;

void setup()
{ 
  Serial.begin(115200); // Debug console
  Serial.println();
  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card
  timer.setInterval(myEthernetTimeout, myfunction);
  timer.setInterval(blynkInterval, checkBlynk);   // check connection to server per blynkInterval
  //Blynk.begin(auth);  // blocking routine, not to be used
  Ethernet.begin(arduino_mac, arduino_ip, dns_ip, gateway_ip, subnet_mask );
  Serial.println("\tBeginning Blynk...");
  Blynk.config(auth, server);
  Blynk.connect();
  Serial.println("\tSetup complete.");  
}

void myfunction() {
  Serial.println("\tLook, no Blynk block.");
  if (Blynk.connected()) {
    Serial.println("\tEthernet still connected.");
    Blynk.virtualWrite(V121, millis() / 1000);   // SELECT A VIRTUAL PIN TO SEND MILLIS() TO
  }
}

void checkBlynk() {
  while (!Blynk.connected()) {
    Blynk.connect();
    if (millis() > startConnecting + myEthernetTimeout) {
      Serial.println("\tUnable to connect to server. ");
      break;
    }
  }
  Serial.println("\tChecking again in 25s.");
}

void loop()
{
  if(Blynk.connected()){  // this ensures it doesn't just loop trying to reconnect
    Blynk.run();
  }
  timer.run();
}
