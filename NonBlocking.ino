/* NoBlynkBlock.ino by Costas for https://community.blynk.cc/t/blynk-is-blocking-if-internet-is-down/16809
will recover from server or router going down   

*/

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[]   = "xxx";
char pass[]   = "xxx";
char auth[]   = "xxx";
char server[] = "blynk-cloud.com";

BlynkTimer timer;
unsigned int myServerTimeout  =  3500;  //  3.5s server connection timeout (SCT)
unsigned int myWiFiTimeout    =  3200;  //  3.2s WiFi connection timeout   (WCT)
unsigned int functionInterval =  7500;  //  7.5s function call frequency   (FCF)
unsigned int blynkInterval    = 25000;  // 25.0s check server frequency    (CSF)

void setup()
{
  Serial.begin(115200);
  Serial.println();
  if(WiFi.status() == 6){
    Serial.println("\tWiFi not connected yet.");
  }
  timer.setInterval(functionInterval, myfunction);// run some function at intervals per functionInterval
  timer.setInterval(blynkInterval, checkBlynk);   // check connection to server per blynkInterval
  unsigned long startWiFi = millis();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    if(millis() > startWiFi + myWiFiTimeout){
      Serial.println("\tCheck the WiFi router. ");
      break;
    }       
  }
  Blynk.config(auth, server);
  checkBlynk();
}

void myfunction(){
  Serial.println("\tLook, no Blynk  block.");
  if(WiFi.status()== 3){
    Serial.println("\tWiFi still  connected.");
  }
  if(Blynk.connected()){
    Blynk.virtualWrite(V11, millis() / 1000);
  }
}

void checkBlynk() {
  if (WiFi.status() == WL_CONNECTED)  
  {
    unsigned long startConnecting = millis();    
    while(!Blynk.connected()){
      Blynk.connect();  
      if(millis() > startConnecting + myServerTimeout){
        Serial.print("Unable to connect to server. ");
        break;
      }
    }
  }
  if (WiFi.status() != 3) {
    Serial.print("\tNo WiFi. ");
  } 
  Serial.printf("\tChecking again in %is.\n", blynkInterval / 1000);
  Serial.println(); 
}

void loop()
{
  if (Blynk.connected()) {Blynk.run();}
  timer.run();
}
