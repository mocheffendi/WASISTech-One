#define BLYNK_PRINT Serial /* Comment this out to disable prints and save space */
#include <UIPEthernet.h>
#include <BlynkSimpleUIPEthernet.h>
BlynkTimer timer;
char auth[] = "xxxxxxxxxxxxxx";
unsigned int myEthernetTimeout =   5000;  // Ethernet Connection Timeout (ECT)
unsigned int blynkInterval     =  25000;  // Check Server Frequency      (CSF)

byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };    //DE:ED:BA:FE:FE:ED
char server[]=    "blynk-cloud.com"; // can use "blynk-cloud.com" or an IP address
IPAddress arduino_ip ( 192,   168,   10,  120);
IPAddress dns_ip     (  192,   168,   10,   90);
IPAddress gateway_ip ( 192,   168,   10,   90);
IPAddress subnet_mask(255, 255, 255,   0);

void setup()
{
  Serial.begin(115200);      // Debug console, Mega is OK at 115200
  Serial.println();
  timer.setInterval(myEthernetTimeout, myfunction);
  timer.setInterval(blynkInterval, checkBlynk);   // check connection to server per blynkInterval    
  Ethernet.begin(arduino_mac, arduino_ip, dns_ip, gateway_ip, subnet_mask );
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
