/*
   ESP8266 WasisTech One 4Channel Relay w Fade and Terminal
   1MB flash sizee
   WASISTech header
   1 - vcc 3v3
   2 - rx
   3 - tx
   4 - gnd
   5 - gpio 14
   esp8266 connections
   gpio  0 - button 1
   gpio 12 - relay 1 Kabel Kuning / Orange
   gpio 10 - button 2 
   gpio 13 - relay 2 Kabel Putih
   gpio  4 - button 3
   gpio 14 - relay 3 Kabel Ungu
   gpio  5 - button 4
   gpio 16 - relay 4 Kabel Hijau

   Restart   V0
   Reset     V1

   -----Timer On-----
   Timer 1   V2 On 1
   Timer 2   V3 On 2
   Timer 3   V4 On 3
   Timer 4   V5 On 4
   
   -----Timer Off-----
   Timer 1   V6 Off 1
   Timer 2   V7 Off 2
   Timer 3   V8 Off 3
   Timer 4   V9 Off 4
    
   Terminal  V10
   
   Off 1     V20
   On 1      V21
   Toggle 1  V22
   Off 2     V23
   On 2      V24
   Toggle 2  V25
   Off 3     V26
   On 3      V27
   Toggle 3  V28
   Off 4     V29
   On 4      V30
   Toggle 4  V31

   All On    V32
   All Off   V33

   LED Widget1 V90
   LED Widget2 V91
   LED Widget3 V92
   LED Widget4 V93
   LED Widget5 V94
   LED Widget6 V95
   LED Widget7 V96
   LED Widget8 V97
   
*/

#define WASISTech_BUTTON1    0
#define WASISTech_RELAY1    12
#define WASISTech_LED1       2

#define WASISTech_BUTTON2   10
#define WASISTech_RELAY2    13

#define WASISTech_BUTTON3    4
#define WASISTech_RELAY3    14

#define WASISTech_BUTTON4    5
#define WASISTech_RELAY4    16


#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

WidgetTerminal terminal(V10);

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

static bool BLYNK_ENABLED = true;

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <EEPROM.h>

#define EEPROM_SALT 12663
typedef struct {
  int   salt = EEPROM_SALT;
  char  blynkToken[33]  = "";
  char  blynkServer[33] = "blynk-cloud.com";
  char  blynkPort[6]    = "8442";
} WMSettings;

WMSettings settings;

#include <ArduinoOTA.h>


//for LED status
#include <Ticker.h>
Ticker ticker;


const int CMD_WAIT1 = 0;
const int CMD_BUTTON_CHANGE1 = 1;

const int CMD_WAIT2 = 0;
const int CMD_BUTTON_CHANGE2 = 1;

const int CMD_WAIT3 = 0;
const int CMD_BUTTON_CHANGE3 = 1;

const int CMD_WAIT4 = 0;
const int CMD_BUTTON_CHANGE4 = 1;

int cmd1 = CMD_WAIT1;
int cmd2 = CMD_WAIT2;
int cmd3 = CMD_WAIT3;
int cmd4 = CMD_WAIT4;

int relayState1 = LOW;
int relayState2 = LOW;
int relayState3 = LOW;
int relayState4 = LOW;

//inverted button state
int buttonState1 = HIGH;
int buttonState2 = HIGH;
int buttonState3 = HIGH;
int buttonState4 = HIGH;

static long startPress1 = 0;
static long startPress2 = 0;
static long startPress3 = 0;
static long startPress4 = 0;

void tick()
{
  //toggle state 1
  int state1 = digitalRead(WASISTech_LED1);  // get the current state of GPIO1 pin
  digitalWrite(WASISTech_LED1, !state1);     // set pin to the opposite state

  //toggle state 2
  //int state2 = digitalRead(WASISTech_LED2);  // get the current state of GPIO1 pin
  //digitalWrite(WASISTech_LED2, !state2);     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setState1(int s1) {
  digitalWrite(WASISTech_RELAY1, s1);
  //digitalWrite(WASISTech_LED1, (s1 + 1) % 2); // led is active low
  Blynk.virtualWrite(90, !s1*255);
  Blynk.virtualWrite(94, !s1*255);
}

void setState2(int s2) {
  digitalWrite(WASISTech_RELAY2, s2);
  //digitalWrite(WASISTech_LED2, (s2 + 1) % 2); // led is active low
  Blynk.virtualWrite(91, !s2*255);
  Blynk.virtualWrite(95, !s2*255);
}

void setState3(int s3) {
  digitalWrite(WASISTech_RELAY3, s3);
  //digitalWrite(WASISTech_LED3, (s3 + 1) % 2); // led is active low
  Blynk.virtualWrite(92, !s3*255);
  Blynk.virtualWrite(96, !s3*255);
}

void setState4(int s4) {
  digitalWrite(WASISTech_RELAY4, s4);
  //digitalWrite(WASISTech_LED4, (s4 + 1) % 2); // led is active low
  Blynk.virtualWrite(93, !s4*255);
  Blynk.virtualWrite(97, !s4*255);
}

void turnOn1() {
  relayState1 = LOW;
  setState1(relayState1);
}

void turnOn2() {
  relayState2 = LOW;
  setState2(relayState2);
}

void turnOn3() {
  relayState3 = LOW;
  setState3(relayState3);
}

void turnOn4() {
  relayState4 = LOW;
  setState4(relayState4);
}

void turnOff1() {
  relayState1 = HIGH;
  setState1(relayState1);
}

void turnOff2() {
  relayState2 = HIGH;
  setState2(relayState2);
}

void turnOff3() {
  relayState3 = HIGH;
  setState3(relayState3);
}

void turnOff4() {
  relayState4 = HIGH;
  setState4(relayState4);
}

void toggleState1() {
  cmd1 = CMD_BUTTON_CHANGE1;
}

void toggleState2() {
  cmd2 = CMD_BUTTON_CHANGE2;
}

void toggleState3() {
  cmd3 = CMD_BUTTON_CHANGE3;
}

void toggleState4() {
  cmd4 = CMD_BUTTON_CHANGE4;
}

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


void toggle1() {
  Serial.println("toggle state1");
  Serial.println(relayState1);
  terminal.println("Toogle 1");
  terminal.println(relayState1);
  // Ensure everything is sent
  terminal.flush();
  relayState1 = relayState1 == HIGH ? LOW : HIGH;
  setState1(relayState1);
}

void toggle2() {
  Serial.println("toggle state2");
  Serial.println(relayState2);
  terminal.println("Toogle 2");
  terminal.println(relayState2);
  // Ensure everything is sent
  terminal.flush();
  relayState2 = relayState2 == HIGH ? LOW : HIGH;
  setState2(relayState2);
}

void toggle3() {
  Serial.println("toggle state3");
  Serial.println(relayState3);
  terminal.println("Toogle 3");
  terminal.println(relayState3);
  // Ensure everything is sent
  terminal.flush();
  relayState3 = relayState3 == HIGH ? LOW : HIGH;
  setState3(relayState3);
}

void toggle4() {
  Serial.println("toggle state4");
  Serial.println(relayState4);
  terminal.println("Toogle 4");
  terminal.println(relayState4);
  // Ensure everything is sent
  terminal.flush();
  relayState4 = relayState4 == HIGH ? LOW : HIGH;
  setState4(relayState4);
}

void restart() {
  ESP.reset();
  delay(1000);
}

void reset() {
  //reset settings to defaults
  /*
    WMSettings defaults;
    settings = defaults;
    EEPROM.begin(512);
    EEPROM.put(0, settings);
    EEPROM.end();
  */
  //reset wifi credentials
  WiFi.disconnect();
  delay(1000);
  ESP.reset();
  delay(1000);
}


//off - button 1
BLYNK_WRITE(20) {
  int a = param.asInt();
  if (a != 0) {
    turnOff1();
  }
}

//on - button 1
BLYNK_WRITE(21) {
  int a = param.asInt();
  if (a != 0) {
    turnOn1();
  }
}

//toggle - button1
BLYNK_WRITE(22) {
  int a = param.asInt();
  if (a != 0) {
    toggle1();
  }
}

//off - button2
BLYNK_WRITE(23) {
  int a = param.asInt();
  if (a != 0) {
    turnOff2();
  }
}

//on - button2
BLYNK_WRITE(24) {
  int a = param.asInt();
  if (a != 0) {
    turnOn2();
  }
}

//toggle - button2
BLYNK_WRITE(25) {
  int a = param.asInt();
  if (a != 0) {
    toggle2();
  }
}

//off - button3
BLYNK_WRITE(26) {
  int a = param.asInt();
  if (a != 0) {
    turnOff3();
  }
}

//on - button3
BLYNK_WRITE(27) {
  int a = param.asInt();
  if (a != 0) {
    turnOn3();
  }
}

//toggle - button3
BLYNK_WRITE(28) {
  int a = param.asInt();
  if (a != 0) {
    toggle3();
  }
}

//off - button4
BLYNK_WRITE(29) {
  int a = param.asInt();
  if (a != 0) {
    turnOff4();
  }
}

//on - button4
BLYNK_WRITE(30) {
  int a = param.asInt();
  if (a != 0) {
    turnOn4();
  }
}

//toggle - button4
BLYNK_WRITE(31) {
  int a = param.asInt();
  if (a != 0) {
    toggle4();
  }
}

BLYNK_WRITE(32) {
  int a = param.asInt();
  if (a != 0) {
    turnOn1();
    turnOn2();
    turnOn3();
    turnOn4();    
  }
}

BLYNK_WRITE(33) {
  int a = param.asInt();
  if (a != 0) {
    turnOff1();
    turnOff2();
    turnOff3();
    turnOff4();    
  }
}

//restart - button
BLYNK_WRITE(0) {
  int a = param.asInt();
  if (a != 0) {
    restart();
  }
}

//reset - button
BLYNK_WRITE(1) {
  int a = param.asInt();
  if (a != 0) {
    reset();
  }
}

//status - display
BLYNK_READ(2) {
  Blynk.virtualWrite(5, relayState1);
}

void setup()
{
  Serial.begin(115200);

  //set led pin as output
  pinMode(WASISTech_LED1, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);


  const char *hostname = "WASISTech InspireOne";

  WiFiManager wifiManager;
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //timeout

  //Configuration Portal Timeout
  wifiManager.setConfigPortalTimeout(180);
  //custom params
  EEPROM.begin(512);
  EEPROM.get(0, settings);
  EEPROM.end();

  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }

  Serial.println(settings.blynkToken);
  Serial.println(settings.blynkServer);
  Serial.println(settings.blynkPort);

  WiFiManagerParameter custom_blynk_text("Blynk config. <br/> No token to disable.");
  wifiManager.addParameter(&custom_blynk_text);

  WiFiManagerParameter custom_blynk_token("blynk-token", "blynk token", settings.blynkToken, 33);
  wifiManager.addParameter(&custom_blynk_token);

  WiFiManagerParameter custom_blynk_server("blynk-server", "blynk server", settings.blynkServer, 33);
  wifiManager.addParameter(&custom_blynk_server);

  WiFiManagerParameter custom_blynk_port("blynk-port", "blynk port", settings.blynkPort, 6);
  wifiManager.addParameter(&custom_blynk_port);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect(hostname)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //Serial.println(custom_blynk_token.getValue());
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("Saving config");

    strcpy(settings.blynkToken, custom_blynk_token.getValue());
    strcpy(settings.blynkServer, custom_blynk_server.getValue());
    strcpy(settings.blynkPort, custom_blynk_port.getValue());

    Serial.println(settings.blynkToken);
    Serial.println(settings.blynkServer);
    Serial.println(settings.blynkPort);

    EEPROM.begin(512);
    EEPROM.put(0, settings);
    EEPROM.end();
  }

  //config blynk
  if (strlen(settings.blynkToken) == 0) {
    BLYNK_ENABLED = false;
  }
  if (BLYNK_ENABLED) {
    Blynk.config(settings.blynkToken, settings.blynkServer, atoi(settings.blynkPort));
  }

  //OTA
  ArduinoOTA.onStart([]() {
    Serial.println("Start OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.begin();

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  terminal.println("connected...yeey :)");
  // Ensure everything is sent
  terminal.flush();
  ticker.detach();

  //setup button
  pinMode(WASISTech_BUTTON1, INPUT_PULLUP);
  attachInterrupt(WASISTech_BUTTON1, toggleState1, CHANGE);

  //setup button2
  pinMode(WASISTech_BUTTON2, INPUT_PULLUP);
  attachInterrupt(WASISTech_BUTTON2, toggleState2, CHANGE);

  //setup button3
  pinMode(WASISTech_BUTTON3, INPUT_PULLUP);
  attachInterrupt(WASISTech_BUTTON3, toggleState3, CHANGE);

  //setup button4
  pinMode(WASISTech_BUTTON4, INPUT_PULLUP);
  attachInterrupt(WASISTech_BUTTON4, toggleState4, CHANGE);

  //setup relay
  pinMode(WASISTech_RELAY1, OUTPUT);

  //setup relay2
  pinMode(WASISTech_RELAY2, OUTPUT);

  //setup relay3
  pinMode(WASISTech_RELAY3, OUTPUT);
  
  //setup relay4
  pinMode(WASISTech_RELAY4, OUTPUT);
  
  turnOff1();
  turnOff2();
  turnOff3();
  turnOff4();
  
  Serial.println("done setup");

  while (Blynk.connect() == false) {
    // Wait until connected
  }
  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println("Ready");
  terminal.print("IP address: ");
  terminal.println(WiFi.localIP());
  terminal.flush();
}

// Keep this flag not to re-sync on every reconnection
bool isFirstConnect = true;

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  if (isFirstConnect) {
    // Request Blynk server to re-send latest values for all pins
    //Blynk.syncAll();

    // You can also update an individual Virtual pin like this:
    //Blynk.syncVirtual(V0);
    //Blynk.syncVirtual(V80);
    //Blynk.syncVirtual(V81);
    //Blynk.syncVirtual(V82);
    //Blynk.syncVirtual(V83);

    isFirstConnect = false;
  }

  // Let's write your hardware uptime to Virtual Pin 2
  int value = millis() / 1000;
  Blynk.virtualWrite(V60, value);
}

BLYNK_WRITE(V80)
{
  int value = param.asInt();
  Blynk.virtualWrite(V60, value);
}

BLYNK_WRITE(V2)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  int a = param.asInt();
  if (a != 0) {
    turnOn1();
  }
}

BLYNK_WRITE(V3)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  int a = param.asInt();
  if (a != 0) {
    turnOn2();
  }
}

BLYNK_WRITE(V4)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  int a = param.asInt();
  if (a != 0) {
    turnOn3();
  }
}

BLYNK_WRITE(V5)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  int a = param.asInt();
  if (a != 0) {
    turnOn4();
  }
}

BLYNK_WRITE(V6)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  int a = param.asInt();
  if (a != 0) {
    turnOff1();
  }
}

BLYNK_WRITE(V7)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  int a = param.asInt();
  if (a != 0) {
    turnOff2();
  }
}

BLYNK_WRITE(V8)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  int a = param.asInt();
  if (a != 0) {
    turnOff3();
  }
}

BLYNK_WRITE(V9)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  int a = param.asInt();
  if (a != 0) {
    turnOff4();
  }
}


// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V10)
{

  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
    terminal.println("You said: 'Marco'") ;
    terminal.println("I said: 'Polo'") ;
  } else {

    // Send it back
    terminal.print("You said:");
    terminal.write(param.getBuffer(), param.getLength());
    terminal.println();
  }

  // Ensure everything is sent
  terminal.flush();
}

BLYNK_WRITE(V11) {
  TimeInputParam t(param);

  // Process start time

  if (t.hasStartTime())
  {
    Serial.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
                   
    terminal.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
      
  }
  else if (t.isStartSunrise())
  {
    Serial.println("Start at sunrise");
    terminal.println("Start at sunrise");
    
  }
  else if (t.isStartSunset())
  {
    Serial.println("Start at sunset");
    terminal.println("Start at sunset");
    
  }
  else
  {
    // Do nothing
  }

  // Process stop time

  if (t.hasStopTime())
  {
    Serial.println(String("Stop: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());
                   
    terminal.println(String("Stop: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());
    
  }
  else if (t.isStopSunrise())
  {
    Serial.println("Stop at sunrise");
    terminal.println("Stop at sunrise");
    
  }
  else if (t.isStopSunset())
  {
    Serial.println("Stop at sunset");
    terminal.println("Stop at sunset");
    
  }
  else
  {
    // Do nothing: no stop time was set
  }

  // Process timezone
  // Timezone is already added to start/stop time

  Serial.println(String("Time zone: ") + t.getTZ());
  terminal.println(String("Time zone: ") + t.getTZ());
  

  // Get timezone offset (in seconds)
  Serial.println(String("Time zone offset: ") + t.getTZ_Offset());
  terminal.println(String("Time zone offset: ") + t.getTZ_Offset());
  

  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)

  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      Serial.println(String("Day ") + i + " is selected");
      terminal.println(String("Day ") + i + " is selected");
      
    }
  }

  Serial.println();
  terminal.flush();
}

void loop()
{

  //ota loop
  ArduinoOTA.handle();
  //blynk connect and run loop
  if (BLYNK_ENABLED) {
    Blynk.run();
  }
  //delay(200);
  //Serial.println(digitalRead(WASISTech_BUTTON));
  
  switch (cmd1) {
    case CMD_WAIT1:
      break;
    case CMD_BUTTON_CHANGE1:
      int currentState1 = digitalRead(WASISTech_BUTTON1);
      
      if (currentState1 != buttonState1) {
        if (buttonState1 == LOW && currentState1 == HIGH) {
          long duration1 = millis() - startPress1;
          if (duration1 < 3000) {
            Serial.println("short press - toggle relay1");
            toggle1();
          } else if (duration1 < 10000) {
            Serial.println("medium press - reset1");
            //restart();
          } else if (duration1 < 60000) {
            Serial.println("long press - reset settings1");
            //reset();
          }
        } else if (buttonState1 == HIGH && currentState1 == LOW) {
          startPress1 = millis();
        }
        buttonState1 = currentState1;
      }
      break;
      
  }
  switch (cmd2) {
    case CMD_WAIT2:
      break;
    case CMD_BUTTON_CHANGE2:
      int currentState2 = digitalRead(WASISTech_BUTTON2);
      
      if (currentState2 != buttonState2) {
        if (buttonState2== LOW && currentState2 == HIGH) {
          long duration2 = millis() - startPress2;
          if (duration2 < 3000) {
            Serial.println("short press - toggle relay2");
            toggle2();
          } else if (duration2 < 10000) {
            Serial.println("medium press - reset2");
            //restart();
          } else if (duration2 < 60000) {
            Serial.println("long press - reset settings2");
            //reset();
          }
        } else if (buttonState2 == HIGH && currentState2 == LOW) {
          startPress2 = millis();
        }
        buttonState2 = currentState2;
      }
      break;
      
  }
  switch (cmd3) {
    case CMD_WAIT3:
      break;
    case CMD_BUTTON_CHANGE3:
      int currentState3 = digitalRead(WASISTech_BUTTON3);
      
      if (currentState3 != buttonState3) {
        if (buttonState3== LOW && currentState3 == HIGH) {
          long duration3 = millis() - startPress3;
          if (duration3 < 3000) {
            Serial.println("short press - toggle relay3");
            toggle3();
          } else if (duration3 < 10000) {
            Serial.println("medium press - reset3");
            //restart();
          } else if (duration3 < 60000) {
            Serial.println("long press - reset settings3");
            //reset();
          }
        } else if (buttonState3 == HIGH && currentState3 == LOW) {
          startPress3 = millis();
        }
        buttonState3 = currentState3;
      }
      break;
      
  }
  switch (cmd4) {
    case CMD_WAIT4:
      break;
    case CMD_BUTTON_CHANGE4:
      int currentState4 = digitalRead(WASISTech_BUTTON4);
      
      if (currentState4 != buttonState4) {
        if (buttonState4== LOW && currentState4 == HIGH) {
          long duration4 = millis() - startPress4;
          if (duration4 < 3000) {
            Serial.println("short press - toggle relay4");
            toggle4();
          } else if (duration4 < 10000) {
            Serial.println("medium press - reset4");
            //restart();
          } else if (duration4 < 60000) {
            Serial.println("long press - reset settings4");
            //reset();
          }
        } else if (buttonState4 == HIGH && currentState4 == LOW) {
          startPress4 = millis();
        }
        buttonState4 = currentState4;
      }
      break;
      
  }
  // set the brightness of pin 2:
  analogWrite(WASISTech_LED1, brightness);
  //analogWrite(WASISTech_LED2, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness == 0 || brightness == 1020) {
    fadeAmount = -fadeAmount ;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(10);

}
