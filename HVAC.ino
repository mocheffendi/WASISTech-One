    /**************************************************************************** 
     *  ThermoX v0.15 HVAC control for a "2-pipe" radiator system.
     *  
     *  Compares readings from a DHT11 temperature sensor with desired temperature
     *  from the Blynk application, and runs the fan unit as necessary to heat or 
     *  cool.  Hysteresis levels for both Summer and Winter are independently 
     *  adjustable from 2 to 6 degrees. The temperature sensor readings can be 
     *  offset up or down by up to 5 degrees. All settings are saved to EEPROM, and 
     *  automatically reloaded on startup.
     *  
     *  "Home" setting is triggered by IFTTT iOS location channel, and results in an
     *  action on the Maker channel. The Maker channel parameters are as follows:
     *       URL: http://blynk-cloud.com:8080/YOUR_TOKEN/pin/V31
     *       Method: PUT
     *       Content Type: application/json
     *       Body: ["1"]    
     *  "Away" mode requires an identical IFTTT recipe, but with
     *       Body: ["0"]
     *  
     *  Added color coding (red/blue) for heating and cooling modes to dashboard widgets.
     *  
     *  Added IFTTT / Amazon Echo integration for operating with voice commands. Uses 
     *  the IFTTT Maker Channel in the same way as above, but with an Amazon Alexa voice 
     *  trigger. To manually run the fan, use the following Maker Channel parameters: 
     *       URL: http://blynk-cloud.com:8080/YOUR_TOKEN/pin/V6
     *       Method: PUT
     *       Content Type: application/json
     *       Body: ["1"]    
     *  Make identical recipes (now called applets) for Temperature Up and Temperature 
     *  Down, substituting Body values of ["2"] and ["3"], repsectively.  
     *  
     *  Added a press-and-hold requirement to enter the settings menu, as well as
     *  a Menu timeout and reset after a period of inactivity. 
     *  Added manual overrides to force system run or halt, independent of other factors.
     *  Added a diagnostic mode that outputs temperature readings to the SETTINGS widget.
     *  Extended lower limit of temperature correction to -10 degrees.
     *  Fixed arithmetic bug that was causing Blynk to show ESP as "offline"
     *  
     *  WiFi connection is now simplified with Tapzu's WiFiManager. Wifi automatically
     *  reconnects to last working credentials. If the last SSID is unavailable, it
     *  creates an access point ("BlynkAutoConnect"). Connect any wifi device to the
     *  access point, and a captive portal pop up to receive new wifi credentials.
     *  
     *  The hardware is minimal: an ESP-01, a single relay on GPIO 0, and a DHT11
     *  temperature sensor on GPIO 2.
     *  
    *****************************************************************************
    */
    #include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
    #include <BlynkSimpleEsp8266.h>
    #include <ESP8266WebServer.h>
    #include <DNSServer.h>
    #include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager
    #include <SimpleTimer.h>
    #include "DHT.h"
    #include <EEPROM.h>

    #define UpdateFrequency 10000 //How often a new temperature will be read
    #define MenuTimeOut 7000 //Menu timeout from inactivity
    #define LongPress 650 //How long SETTINGS button needs to be pressed to enter menu
    #define RelayPin 2


    #define BLYNK_BLUE      "#04C0F8"
    #define BLYNK_RED       "#D3435C"
    #define BLYNK_WHITE     "#FFFFFF"

    DHT dht(0,DHT11); //Initialize the sensor. Use pin 0. Sensor type is DHT11.

    // Timer for temperature updates
    SimpleTimer timer;

    //WiFi and Blynk connection variables
    char auth[] = "TOKEN"; // Blynk token "YourAuthToken"

    //Thermostat variables
    int TempDes = 70;             //Desired temperature setting
    int PreviousTempDes;
    int TempAct = 70;             //Actual temperature, as measured by the DHT11 sensor
    int BadRead = 0;              //Counts consecutive failed readings of the DHT11 sensor
    float LastRead = 70; 

    // Preference variables
    int Hysteresis_W = 2;         //Summer and Winter hysteresis levels
    int Hysteresis_S = 2;
    int TempCorrection = 0;       //Used to adjust readings, if the sensor needs calibration

    // Current condition variables
    boolean Winter = true; 
    boolean Home = true;
    boolean ManualRun = false;    // used to run fan, overriding thermostat algorithm
    boolean ManualStop = false;   // used to stop fan, overriding thermostat algorithm
    boolean DigitalTemp = false;  // used to show temperature in SETTINGS value widget
    int MenuItem = 0;             // Settings menu selection variable
    long buttonRelease;           // time button was released
    long buttonPress;             // time button was last pressed
    boolean ButtonDown = false;   // Settings button state (pressed = true)
    boolean FanState = 0;         // is the fan on or off?
    String Response = "";         // Text output to SETTINGS value widget

    String myHostname = "ThermoX";

    void setup() {
      
      // Create an access point if no wifi credentials are stored
      WiFi.hostname(myHostname);
      WiFiManager wifi;
      wifi.autoConnect("ThermoX"); 
      Blynk.config(auth);
      
      dht.begin(); //Start temperature sensor
      delay(1500);

      //Initialize the fan relay. Mine is "off" when the relay is set HIGH.
      pinMode(RelayPin,OUTPUT); 
      digitalWrite(RelayPin,HIGH);
     
      Serial.begin(115200);
      delay(10);
      
      //Load any saved settings from the EEPROM
      EEPROM.begin(20);  
      Serial.println(F("STARTUP : LOADING SETTINGS FROM MEMORY"));
      Serial.println(F(""));
      GetPresets();

      PreviousTempDes = TempDes; 
      
      MenuReset();

      timer.setInterval(UpdateFrequency, TempUpdate); // Update temp reading and relay state
      timer.setInterval(200, ButtonCheck);
    }


    // Main loop
    void loop() {
      Blynk.run();
      timer.run();
    }

    // Checks for long press condition on SETTINGS button
    void ButtonCheck(){
      if (ButtonDown){
        // Enter or exit the SETTINGS menu, if it was a long press
        if (millis() - buttonPress > LongPress){ 
          if (MenuItem == 0){
            NextMenuItem(); // Enter the SETTINGS menu
          }
          else MenuReset(); // Exit the SETTINGS menu 

          ButtonDown = false; // Prevent repeat triggering
        }
      }
    }


    // This is the decision algorithm for turning the HVAC on and off
    void TempUpdate (){
      OtherUpdates(); //Refeshes dashboard information

      float ReadF = dht.readTemperature(true); //Get a new reading from the temp sensor
        
      if (isnan(ReadF)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        BadRead++;
        return;
      }

      //To compensate for some instability in the DHT11, the corrected temperature is
      //averaged with previous read, and any change is limited to 1 degree at a time. 
      else   { 
        int TempAvg = int((ReadF + LastRead + (2 * TempCorrection))/2);
        if (TempAvg > TempAct){
          TempAct += 1;
        }
        if (TempAvg < TempAct){
          TempAct -= 1;
        }

        LastRead = ReadF;
        BadRead = 0;        // Reset counter for failed sensor reads
      }
      
      Blynk.virtualWrite(V0,TempAct); //Report the corrected temperature in app
      Serial.print("Actual temperature: ");
      Serial.println(TempAct);

      if (DigitalTemp){ //Report the temperature to the SETTINGS widget
        Blynk.virtualWrite(V10,"-----------------------");
        delay(50);
        Blynk.virtualWrite(V10,String("TEMPERATURE: ") + TempAct);
      }

      // Decision algorithm for running HVAC
      if (!ManualRun && !ManualStop){   // Make sure it's not in one of the manual modes
        // If I'm home, run the algorithm
        if (Home){
          if (Winter){
            //If I'm home, it's Winter, and the temp is too low, turn the relay ON
            if (TempAct < TempDes){
              FanState = 1;
              Fan();
            }
            //Turn it off when the space is heated to the desired temp + a few degrees
            else if ((TempAct >= (TempDes + Hysteresis_W)) && FanState) {
              FanState = 0;
              Fan();
            }
          }
          else if (!Winter){
            //If I'm home, it's Summer, and the temp is too high, turn the relay ON
            if (TempAct > TempDes){
              FanState = 1;
              Fan();
            }
            //Turn it off when the space is cooled to the desired temp - a few degrees
            else if ((TempAct <= (TempDes - Hysteresis_S)) && FanState){
              FanState = 0;
              Fan();
            }
         }
        }
        // If I'm not home, turn the relay OFF
        else {
          FanState = 0;
          Fan();
        }
      }
    }


    //Match temp gauge to slider in Blynk app 
    BLYNK_WRITE(V3){
      TempDes = param.asInt();
      Blynk.virtualWrite(V1,TempDes);
    }

    //Get location (home or away) from the IFTTT iOS location and Maker channels
    BLYNK_WRITE(V31)
    {   
      Home = param.asInt();
      
      if (Home){ //Turn the HOME LED widget on or off
        Blynk.virtualWrite(V29,1023);
      }
      else Blynk.virtualWrite(V29,0);
    }

       
    // Dashboard SETTINGS button. Press-and-hold to enter menu. Short press for next item.
    BLYNK_WRITE(V4) {
      // Check for a button press
      ButtonDown = param.asInt();
      if (ButtonDown){ 
         buttonPress = millis();
         DigitalTemp = false;
       }
        // check for button release
        else {
          buttonRelease = millis();
          if (buttonRelease - buttonPress < LongPress){  // It was a short press.
            if (MenuItem == 0){
            MenuReset(); // Remind user to hold 2 seconds to enter menu
            }
          else NextMenuItem(); // Advance to next menu item
          }
       }
    }


    //Cycles through the Settings Menu in the Labeled Value widget
    void NextMenuItem(){
      
      MenuItem += 1;
      if (MenuItem > 9){
        MenuItem = 1;
      }
        
      switch(MenuItem){
          case 1:
            if (ManualRun){
              Response = "END MANUAL RUN?";
            }
            else{
              Response = "RUN MANUALLY?";
            }
            break;

          case 2:
            if (ManualStop){
              Response = "END SYSTEM HALT?";
            }
            else{
              Response = "HALT SYSTEM?";
            }
            break;
            
         case 3:
          if (Home){
            Response = "LOCATION: HOME";
          }
          else Response = "LOCATION: AWAY";
          break;


        case 4:
          if (Winter){
            Response = "MODE : WINTER";
          }
          else Response = "MODE : SUMMER";
          break;

        case 5:
          if (Winter){
            Response = "HYSTERESIS: ";
            Response +=  Hysteresis_W;
            Response += " DEG";   
          }
          else{
            Response = "HYSTERESIS: ";
            Response += Hysteresis_S;
            Response += " DEG";
          }
          break;

        case 6:
          Response = "TEMP CORRECTION: ";
          Response += TempCorrection;
          Response += " DEGREES";
          break;
          
        case 7:
          Response = "DIAGNOSTIC MODE?";
          break;

        case 8:
          Response = "CLEAR WIFI SETTINGS?";
          break;

        case 9:
           Response = "RESET ALL DEFAULTS?";
           break;
      }
      Blynk.virtualWrite(V10,Response);
    }


    //Dashboard MODIFY button. Executes change of selected menu item 
    BLYNK_WRITE(V5){
      
      buttonRelease = millis(); //Resets menu timeout for inactivity
      
      if ((MenuItem > 0) && (param.asInt())){
        switch(MenuItem){

            //Forced on
          case 1:
            if (ManualRun){
              ManualRun = false;
              Response = "RUN MANUALLY?";
            }
            else{
              ManualRun = true;
              ManualStop = false;
              FanState = 1;
              Fan();
              Response = "MANUAL RUN: ON";
            }   
            break;

            //Forced halt
          case 2:
            if (ManualStop){
              ManualStop = false;
              Response = "HALT SYSTEM?";
            }
            else {
              ManualStop = true;
              ManualRun = false;
              FanState = 0;
              Fan();
              Response = "SYSTEM HALTED";
            }
            break;

             //Change location manually
          case 3:
            if (Home){
              Home = false;
              Response = "LOCATION : AWAY";
            }
            else {
              Home = true;
              Response = "LOCATION : HOME";
            }
            break;
            
          //Change season
          case 4:
            if (Winter){
              Response = "MODE : SUMMER";
              Winter = false;
              EEPROM.write(4,0);
              EEPROM.commit();
            }
            else {
              Response = "MODE : WINTER";
              Winter = true;
              EEPROM.write(4,1);
              EEPROM.commit();
            } 
            break;
            
          //Change hysteresis level of currently selected season
          case 5:
            if (Winter){
              Hysteresis_W += 1;
              if (Hysteresis_W > 6){
                Hysteresis_W = 1;
              }
              EEPROM.write(1,(Hysteresis_W));
              EEPROM.commit();
              Response = "WINTER HYSTERESIS: ";
              Response += Hysteresis_W;
              Response += " DEG";
            }
            else{
              Hysteresis_S += 1;
              if (Hysteresis_S > 6){
                Hysteresis_S = 1;
              }
              EEPROM.write(2,(Hysteresis_S));
              EEPROM.commit();
              Response = "SUMMER HYSTERESIS: ";
              Response += Hysteresis_S;
              Response += " DEG";
              }
            break;

          // Correct faulty DHT11 readings
          case 6:
            TempCorrection +=1;
            if (TempCorrection > 5){
              TempCorrection = -10;
            }
            EEPROM.write(0,(TempCorrection + 10));
            EEPROM.commit();
            Response = "TEMPERATURE CORRECTION: ";
            Response += TempCorrection;
            Response += " DEG";
            break;

           // Send temperature readings to the SETTINGS value widget
          case 7:
            if (!DigitalTemp){
              Response = "ENTERING DIAGNOSTIC MODE";
              DigitalTemp = true;
              MenuItem = 0; //Prevents menu reset
              delay(400);
            }
            else {
              Response = ("DIAGNOSTIC MODE?");
              DigitalTemp = false;
            }
            break;
            

          //Clear stored SSID and password
          case 8:
            Response = "ERASING WIFI CREDENTIALS";
            WiFi.begin("FakeSSID","FakePW"); //replace current WiFi credentials with fake ones
            delay(1000);
            ESP.restart();
            break;

          //Clear current temperature settings
          case 9:
            Response = "All settings reset to default!";
            Winter = true;
            Hysteresis_W = 2;
            Hysteresis_S = 2;
            break;
        }
        Blynk.virtualWrite(V10, Response);
      }
    }

    // Receives commands from IFTTT Maker Channel via the Amazon Echo. 
    // Pin V6 is not actually associated with a Blynk dashboard widget.
    BLYNK_WRITE(V6){
      int AlexaCommand = param.asInt();

      switch(AlexaCommand){
        // Turn manual running on / off
        case 1:
          Serial.println("Manual run");
          if (ManualRun){
              ManualRun = false;
            }
          else{
            ManualRun = true;
            ManualStop = false;
            FanState = 1;
            Fan();
          }   
          break;
        // Increase desired temperature by 2 degrees
        case 2: 
          Serial.println("Raise temperature 2 degrees.");
          TempDes += 2;
          break;
        // Decrease desired temperature by 2 degrees
        case 3:
          Serial.println("Lower temperature 2 degrees.");
          TempDes -= 2;
          break;
      }
    }


    // Turn the HVAC on or off
    void Fan(){

      // Set the proper color for the Desired Temp gauge and ON/OFF LED
      //(red = heating, blue = cooling, white gauge or LED off = within desired range)
      if (Winter && FanState){
          Blynk.setProperty(V1, "color", BLYNK_RED);
          Blynk.setProperty(V7, "color", BLYNK_RED);
        }
        else if (!Winter && FanState){
          Blynk.setProperty(V1, "color", BLYNK_BLUE);
          Blynk.setProperty(V7, "color", BLYNK_BLUE);
        }
        else{
          Blynk.setProperty(V1, "color", BLYNK_WHITE);
        }
        
      digitalWrite(RelayPin,!FanState); // Relay turns fan on with LOW input, off with HIGH
      Blynk.virtualWrite(V7,FanState * 1023);// fan "ON" LED on dashboard
      Serial.print(F("Fan state: "));
      Serial.println(FanState);
    }


    // Reset the Menu at startup or after timing out from inactivity
    void MenuReset(){
      MenuItem = 0;
      Blynk.virtualWrite(V10, String("-----------------------"));
      delay(50);
      Blynk.virtualWrite(V10, String("HOLD 2 SEC TO ENTER/EXIT MENU"));
    }


    // Updates dashboard information on the Blynk app
    void OtherUpdates(){
      Blynk.virtualWrite(V29,Home * 1023);  // Update "home" LED on dashboard
      Blynk.virtualWrite(V1,TempDes);       //Update desired temp on the dashboard
      
      //Reset the Settings Menu if there's been no activity for a while
       if (MenuItem > 0 && (millis() - buttonRelease > MenuTimeOut)){
         MenuReset();
       }
       
       // Notify when the temperature sensor fails repeatedly, and turn off the fan.
       if (BadRead > 10){
         Blynk.virtualWrite(V10, String("<<< SENSOR MALFUNCTION >>>"));
         BadRead = 0;
         if (!ManualRun){ //Manual mode supersedes a malfunction condition
          FanState = 0;
          Fan();
         }
       }
       
       if (TempDes != PreviousTempDes){   //update the EEPROM if desired temperature had changed.
        EEPROM.write(3,TempDes);
        EEPROM.commit();
        Serial.print(F("New desired temperature saved: "));
        Serial.println(TempDes);
        PreviousTempDes = TempDes;  
       }
    }

    //Retrieves saved values from EEPROM
    void GetPresets(){
      TempCorrection = EEPROM.read(0);
      if ((TempCorrection < 0) || (TempCorrection > 15)){
        TempCorrection = 0;
        Serial.println(F("No saved temperature correction."));
      }
      else{
        TempCorrection -= 10; // 10 was added at EEPROM save to account for negative values
        Serial.print(F("Temperature correction: "));
        Serial.print(TempCorrection);
        Serial.println(F(" degrees."));      
      }

      Winter = EEPROM.read(4);
      Hysteresis_W = EEPROM.read(1);
      Hysteresis_S = EEPROM.read(2);

      if ((Hysteresis_W < 2) || (Hysteresis_W > 6)){
          Hysteresis_W = 2;
      }
      if ((Hysteresis_S < 2) || (Hysteresis_S > 6)){
          Hysteresis_S = 2;
      }
      
      if (Winter){
        Serial.println(F("Season setting: Winter / heating"));
        Serial.print(F("Winter hysteresis: "));
        Serial.print(Hysteresis_W);
        Serial.println(F(" degrees."));   
      }
      else {
        Serial.println(F("Season setting: Summer / cooling"));
        Serial.print(F("Summer hysteresis: "));
        Serial.print(Hysteresis_S);
        Serial.println(F(" degrees."));      
      } 
     
      TempDes = EEPROM.read(3);
      if ((TempDes < 50) || (TempDes > 80)){
        TempDes = 70;
        Serial.println(F("No saved temperature setting."));
      }
      else {
        Serial.print(F("Desired temperature: "));
        Serial.print(TempDes);
        Serial.println(F(" degrees."));   
      }
      Serial.println("");
    }
