#define BLYNK_PRINT Serial 
/* Comment this out to disable prints and save space */ 
#include <UIPEthernet.h> 
#include <BlynkSimpleUIPEthernet.h> 
BlynkTimer timer; 
char auth[] = "YourAuthToken"; 
unsigned int myEthernetTimeout = 5000; // 5.0s Ethernet Connection Timeout (ECT) 
unsigned int blynkInterval = 25000; // 25.0s Check Server Frequency (CSF) 
void setup() { 
Serial.begin(115200); // Debug console, Mega is OK at 115200 
Serial.println(); 
timer.setInterval(myEthernetTimeout, myfunction); 
timer.setInterval(blynkInterval, checkBlynk); // check connection to server per blynkInterval 
Blynk.config(auth, "blynk-cloud.com"); // this is OK Blynk.connect(); } 

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
Serial.print("\tUnable to connect to server. "); 
break; 
} 
} 
Serial.println("Checking again in 25s."); 
} 
void loop() { 
if (Blynk.connected()) { 
Blynk.run(); 
} 

timer.run(); 
}

Edit: ignore the sketch above and use the one below as it was missing Ethernet connection.
