/**************************************************************
 * timeinput.ino Demonstrate interaction of Time library with
 * Blynk's TimeInput widget.
 * App project setup:
 * RTC widget (no pin required!!!)
 * V1 : Manual/Auto button
 * V2: On-off button
 * Terminal on V3 // Label will be the clock + wifi signal!!!!
 * 
 * Time Input widget on V4 (Monday-Friday)
 * Button selection for Time Input (Monday-Friday) on V5
 * 
 * Time Input widget on V6 (Saturday-Sunday)
 * Button selection for Time Input (Saturday-Sunday on V7
 * 
 * Time Input widget on V8 (All days)
 * Button selection for Time Input (All days) on V9
 * 
 * Time Input widget on V10 (Up to you)
 * Button selection for Time Input (Up to you) on V11
 * 
 **************************************************************/

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


SimpleTimer timer;

WidgetRTC rtc;
WidgetTerminal terminal(V3);

#define server "XXX.XXX.XXX.XXX"   // or "blynk.cloud-com" for Blynk's cloud server
#define TestLED 2                 // on board LED pin assignment
char Date[16];
char Time[16];
char auth[] = "XXXXXXXXXXXXXXXXXXXXXXX";
char ssid[] = "XXXXXXX";
char pass[] = "XXXXXXXXXXXXXX";
long startsecondswd;            // weekday start time in seconds
long stopsecondswd;             // weekday stop  time in seconds
long nowseconds;                // time now in seconds
bool isFirstConnect = true;


String displaycurrenttimepluswifi;
int wifisignal;
int manual=0;
int oldstatus;

int mondayfriday;
int saturdaysunday;
int alldays;
int uptoyou;


void setup()
{
  pinMode(TestLED, OUTPUT);
  digitalWrite(TestLED, LOW); // set LED OFF
  Serial.begin(115200);
  Serial.println("\Starting");
  Blynk.begin(auth, ssid, pass, server);
  int mytimeout = millis() / 1000;
  while (Blynk.connect() == false) { // try to connect to server for 10 seconds
    if((millis() / 1000) > mytimeout + 8){ // try local server if not connected within 9 seconds
       break;
    }
  }
  rtc.begin();
  timer.setInterval(10000L, activetoday);  // check every 10 SECONDS if schedule should run today 
  timer.setInterval(30000L, reconnectBlynk);  // check every 30s if still connected to server 
  timer.setInterval(5000L, clockvalue);  // check value for time
  timer.setInterval(5000L, sendWifi);    // Wi-Fi singal
}


BLYNK_CONNECTED() {
if (isFirstConnect) {
  Blynk.syncAll();
  Blynk.notify("TIMER STARTING!!!!");
isFirstConnect = false;
}
}


void sendWifi() {
  wifisignal = map(WiFi.RSSI(), -105, -40, 0, 100);
}

void clockvalue() // Digital clock display of the time
{

 int gmthour = hour();
  if (gmthour == 24){
     gmthour = 0;
  }
  String displayhour =   String(gmthour, DEC);
  int hourdigits = displayhour.length();
  if(hourdigits == 1){
    displayhour = "0" + displayhour;
  }
  String displayminute = String(minute(), DEC);
  int minutedigits = displayminute.length();  
  if(minutedigits == 1){
    displayminute = "0" + displayminute;
  }  

  displaycurrenttimepluswifi = "                                          Clock:  " + displayhour + ":" + displayminute + "               Signal:  " + wifisignal +" %";
  Blynk.setProperty(V3, "label", displaycurrenttimepluswifi);
  
}



void activetoday(){        // check if schedule should run today
  if(year() != 1970){

   if (mondayfriday==1) {  
    Blynk.syncVirtual(V4); // sync timeinput widget  
   }
   if (saturdaysunday==1) { 
    Blynk.syncVirtual(V6); // sync timeinput widget  
   }
   if (alldays==1) { 
    Blynk.syncVirtual(V8); // sync timeinput widget  
   }
   if (uptoyou==1) { 
    Blynk.syncVirtual(V10); // sync timeinput widget  
   }
  }
}

void checklastbuttonpressed (){
    if((mondayfriday==1)&&(saturdaysunday==0)){ oldstatus=1; }
    if((mondayfriday==0)&&(saturdaysunday==1)){ oldstatus=2; }
    if((mondayfriday==1)&&(saturdaysunday==1)){ oldstatus=3; }
    if(alldays==1){ oldstatus=4; }
    if(uptoyou==1){ oldstatus=5; }
    if((mondayfriday==0)&&(saturdaysunday==0)&&(alldays==0)&&(uptoyou==0)){ oldstatus=6; }  
}


void restorelastbuttonpressed (){
    if(oldstatus==1){ mondayfriday=1; Blynk.virtualWrite(V5, 1); }
    if(oldstatus==2){ saturdaysunday=1 ; Blynk.virtualWrite(V7, 1); }
    if(oldstatus==3){ saturdaysunday=1; mondayfriday=1;Blynk.virtualWrite(V5, 1);Blynk.virtualWrite(V7, 1); }
    if(oldstatus==4){ alldays=1; Blynk.virtualWrite(V9, 1);}
    if(oldstatus==5){ uptoyou=1; Blynk.virtualWrite(V11, 1);}   
    if(oldstatus==6){ 
      mondayfriday=0; 
      saturdaysunday=0;
      alldays=0;
      uptoyou=0;
      Blynk.virtualWrite(V5, 0);
      Blynk.virtualWrite(V7, 0);
      Blynk.virtualWrite(V9, 0);
      Blynk.virtualWrite(V11, 0);
      }
 }


BLYNK_WRITE(V1)  // Manual/Auto selection
{
  if (param.asInt()==1) {
    manual=1;
    terminal.println();
    terminal.println();
    terminal.println();
    terminal.println("Manual MODE is ON");
    terminal.println("Press ON/OFF button if required");
    terminal.println();
    terminal.println();
    terminal.println();
    terminal.flush();

    checklastbuttonpressed ();
  
    alldays=0;
    uptoyou=0;
    mondayfriday=0;
    saturdaysunday=0;
    Blynk.virtualWrite(V5, 0);
    Blynk.virtualWrite(V7, 0);
    Blynk.virtualWrite(V9, 0);
    Blynk.virtualWrite(V11, 0);
    
  } else {
    restorelastbuttonpressed ();
    manual=0;
    terminal.println();
    terminal.println();
    terminal.println();
    terminal.println("Manual MODE is OFF");
    terminal.println("Auto MODE restored from last status");
    terminal.println("Wait for update (10 seconds as maximum)");
    terminal.println();
    terminal.println();
    terminal.flush();
}
}

void resetTerminal()
{
    terminal.println();
    terminal.println();
    terminal.println();
    terminal.println("New MODE has been selected");
    terminal.println("Wait for update (10 seconds as maximum)");
    terminal.println();
    terminal.println();
    terminal.println();
    terminal.flush();
}

void resetManual()
{
Blynk.virtualWrite(V1, 0);   //Turn OFF Manual Mode Widget
Blynk.virtualWrite(V2, 0);   //Turn OFF Button Widget Device
digitalWrite(TestLED, LOW); // set LED OFF
}


BLYNK_WRITE(V2)  // ON-OFF Manual 
{
    if (param.asInt()==1) {  // boton encendido  
    terminal.println();
    terminal.println();
    terminal.println();
    terminal.println("Manual MODE is ON");
    terminal.println("Press ON/OFF button if required");
    terminal.println("Device is ON");
    terminal.println();
    terminal.println();
    terminal.flush();
    
        if (manual==0){  //está en modo automático     
         checklastbuttonpressed ();
         manual=1;
         mondayfriday=0;
         saturdaysunday=0;
         alldays=0;
         uptoyou=0;
         Blynk.virtualWrite(V1, 1);
         Blynk.virtualWrite(V5, 0);
         Blynk.virtualWrite(V7, 0);
         Blynk.virtualWrite(V9, 0);
         Blynk.virtualWrite(V11, 0);
         digitalWrite(TestLED, HIGH); // set LED ON 
         Blynk.virtualWrite(V2, 1);   //Turn ON Button Widget
         
         } else {             //está en modo manual 
         mondayfriday=0;
         saturdaysunday=0;
         alldays=0;
         uptoyou=0;
         Blynk.virtualWrite(V1, 1);
         Blynk.virtualWrite(V5, 0);
         Blynk.virtualWrite(V7, 0);
         Blynk.virtualWrite(V9, 0);
         Blynk.virtualWrite(V11, 0);
         digitalWrite(TestLED, HIGH); // set LED ON 
         Blynk.virtualWrite(V2, 1);   //Turn ON Button Widget
         }                           
    }else {
      
    terminal.println();
    terminal.println();
    terminal.println();
    terminal.println("Manual MODE is ON");
    terminal.println("Press ON/OFF button if required");
    terminal.println("Device is OFF");
    terminal.println();
    terminal.println();
    terminal.flush();

        if (manual==0){      //modo automático
         checklastbuttonpressed ();
         manual=1;
         mondayfriday=0;
         saturdaysunday=0;
         alldays=0;
         uptoyou=0;
         Blynk.virtualWrite(V1, 1);
         Blynk.virtualWrite(V5, 0);
         Blynk.virtualWrite(V7, 0);
         Blynk.virtualWrite(V9, 0);
         Blynk.virtualWrite(V11, 0);
         digitalWrite(TestLED, LOW); // set LED OFF
         Blynk.virtualWrite(V2, 0);   //Turn OFF Button Widget
         } else {  
         mondayfriday=0;
         saturdaysunday=0;
         alldays=0;
         uptoyou=0;   
         Blynk.virtualWrite(V1, 1);
         Blynk.virtualWrite(V5, 0);
         Blynk.virtualWrite(V7, 0);
         Blynk.virtualWrite(V9, 0);
         Blynk.virtualWrite(V11, 0);
         digitalWrite(TestLED, LOW); // set LED OFF
         Blynk.virtualWrite(V2, 0);   //Turn OFF Button Widget
         }  
         }
}

BLYNK_WRITE(V5)  // Monday-Friday selected
{
  if (param.asInt()==1 && (V1==1)) {
    timer.setTimeout(50, resetTerminal);
    timer.setTimeout(50, resetManual);
    timer.setTimeout(50, checklastbuttonpressed);
    mondayfriday=1;
    alldays=0;
    uptoyou=0;
    Blynk.virtualWrite(V9, 0);
    Blynk.virtualWrite(V11, 0);
  } else {
    mondayfriday=0;
}
}


BLYNK_WRITE(V7)  // Saturday-Sunday selected
{
  if (param.asInt()==1) {
    timer.setTimeout(50, resetTerminal);
    timer.setTimeout(50, resetManual);
    timer.setTimeout(50, checklastbuttonpressed);
    saturdaysunday=1;
    alldays=0;
    uptoyou=0;
    Blynk.virtualWrite(V9, 0);
    Blynk.virtualWrite(V11, 0);
  } else {
    saturdaysunday=0;
}
}

BLYNK_WRITE(V9)  // All days selected
{
  if (param.asInt()==1) {
    timer.setTimeout(50, resetTerminal);
    timer.setTimeout(50, resetManual);
    timer.setTimeout(50, checklastbuttonpressed);
    alldays=1;
    mondayfriday=0;
    saturdaysunday=0;
    uptoyou=0; 
    Blynk.virtualWrite(V5, 0);
    Blynk.virtualWrite(V7, 0);
    Blynk.virtualWrite(V11, 0);
  } else {
    alldays=0;
}
}

BLYNK_WRITE(V11)  // Up to you selected
{
  if (param.asInt()==1) {
    timer.setTimeout(50, resetTerminal);
    timer.setTimeout(50, resetManual);
    timer.setTimeout(50, checklastbuttonpressed);
    uptoyou=1;
    mondayfriday=0;
    saturdaysunday=0;
    alldays=0;
    Blynk.virtualWrite(V5, 0);
    Blynk.virtualWrite(V7, 0);
    Blynk.virtualWrite(V9, 0);
  } else {
    uptoyou=0;
}
}

BLYNK_WRITE(V4)//Monday-Friday
{  
  if (mondayfriday==1) {         
    sprintf(Date, "%02d/%02d/%04d",  day(), month(), year());
    sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());
  
    TimeInputParam t(param);
  
    terminal.print("M-F Checked schedule at: ");
    terminal.println(Time);
    terminal.flush();
    int dayadjustment = -1;  
    if(weekday() == 1){
      dayadjustment =  6; // needed for Sunday, Time library is day 1 and Blynk is day 7
    }
    if(t.isWeekdaySelected(weekday() + dayadjustment)){ //Time library starts week on Sunday, Blynk on Monday
    terminal.println("Monday-Friday ACTIVE today");
    terminal.flush();
    if (t.hasStartTime()) // Process start time
    {
      terminal.println(String("Start: ") + t.getStartHour() + ":" + t.getStartMinute());
      terminal.flush();
    }
    if (t.hasStopTime()) // Process stop time
    {
      terminal.println(String("Stop : ") + t.getStopHour() + ":" + t.getStopMinute());
      terminal.flush();
    }
    // Display timezone details, for information purposes only 
    terminal.println(String("Time zone: ") + t.getTZ()); // Timezone is already added to start/stop time 
  //  terminal.println(String("Time zone offset: ") + t.getTZ_Offset()); // Get timezone offset (in seconds)
    terminal.flush();
  
     for (int i = 1; i <= 7; i++) {  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)
        if (t.isWeekdaySelected(i)) {
        terminal.println(String("Day ") + i + " is selected");
        terminal.flush();
        }
      } 
    nowseconds = ((hour() * 3600) + (minute() * 60) + second());
    startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
    //Serial.println(startsecondswd);  // used for debugging
    if(nowseconds >= startsecondswd){    
      terminal.print("Monday-Friday STARTED at");
      terminal.println(String(" ") + t.getStartHour() + ":" + t.getStartMinute());
      terminal.flush();
      if(nowseconds <= startsecondswd + 90){    // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, HIGH); // set LED ON
        Blynk.virtualWrite(V2, 1);
        // code here to switch the relay ON
      }      
    }
    else{
      terminal.println("Monday-Friday Device NOT STARTED today");
      terminal.flush();
   
    }
    stopsecondswd = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
    //Serial.println(stopsecondswd);  // used for debugging
    if(nowseconds >= stopsecondswd){
      digitalWrite(TestLED, LOW); // set LED OFF
      Blynk.virtualWrite(V2, 0);
      terminal.print("Monday-Friday STOPPED at");
      terminal.println(String(" ") + t.getStopHour() + ":" + t.getStopMinute());
      terminal.flush();
      if(nowseconds <= stopsecondswd + 90){   // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, LOW); // set LED OFF
        Blynk.virtualWrite(V2, 0);
        // code here to switch the relay OFF
      }              
    }
    else{
      if(nowseconds >= startsecondswd){  
        digitalWrite(TestLED, HIGH); // set LED ON    test
        Blynk.virtualWrite(V2, 1);
        terminal.println("Monday-Friday is ON");
        terminal.flush();
      
      }          
    }
  }
  else{
    terminal.println("Monday-Friday INACTIVE today");
    terminal.flush();
    // nothing to do today, check again in 30 SECONDS time    
  }
  terminal.println();
}
}

 BLYNK_WRITE(V6) //Saturday-Sunday
 {  
  if (saturdaysunday==1) { 
    sprintf(Date, "%02d/%02d/%04d",  day(), month(), year());
    sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());
  
    TimeInputParam t(param);
  
    terminal.print("S-S Checked schedule at: ");
    terminal.println(Time);
    terminal.flush();
    int dayadjustment = -1;  
    if(weekday() == 1){
    dayadjustment =  6; // needed for Sunday, Time library is day 1 and Blynk is day 7
   }
    if(t.isWeekdaySelected(weekday() + dayadjustment)){ //Time library starts week on Sunday, Blynk on Monday
    terminal.println("Saturday-Sunday ACTIVE today");
    terminal.flush();
    if (t.hasStartTime()) // Process start time
    {
      terminal.println(String("Start: ") + t.getStartHour() + ":" + t.getStartMinute());
      terminal.flush();
    }
    if (t.hasStopTime()) // Process stop time
    {
      terminal.println(String("Stop : ") + t.getStopHour() + ":" + t.getStopMinute());
      terminal.flush();
    }
    // Display timezone details, for information purposes only 
    terminal.println(String("Time zone: ") + t.getTZ()); // Timezone is already added to start/stop time 
   // terminal.println(String("Time zone offset: ") + t.getTZ_Offset()); // Get timezone offset (in seconds)
    terminal.flush();
  
     for (int i = 1; i <= 7; i++) {  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)
        if (t.isWeekdaySelected(i)) {
        terminal.println(String("Day ") + i + " is selected");
        terminal.flush();
        }
      } 
    nowseconds = ((hour() * 3600) + (minute() * 60) + second());
    startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
    //Serial.println(startsecondswd);  // used for debugging
    if(nowseconds >= startsecondswd){    
      terminal.print("Saturday-Sunday STARTED at");
      terminal.println(String(" ") + t.getStartHour() + ":" + t.getStartMinute());
      terminal.flush();
      if(nowseconds <= startsecondswd + 90){    // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, HIGH); // set LED ON
        Blynk.virtualWrite(V2, 1);
        // code here to switch the relay ON
      }      
    }
    else{
      terminal.println("Saturday-Sunday NOT STARTED today");
      terminal.flush();
     
    }
    stopsecondswd = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
    //Serial.println(stopsecondswd);  // used for debugging
    if(nowseconds >= stopsecondswd){
      digitalWrite(TestLED, LOW); // set LED OFF
      Blynk.virtualWrite(V2, 0);
      terminal.print("Saturday-Sunday STOPPED at");
      terminal.println(String(" ") + t.getStopHour() + ":" + t.getStopMinute());
      terminal.flush();
      if(nowseconds <= stopsecondswd + 90){   // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, LOW); // set LED OFF
        Blynk.virtualWrite(V2, 0);
        // code here to switch the relay OFF
      }              
    }
    else{
      if(nowseconds >= startsecondswd){  
        digitalWrite(TestLED, HIGH); // set LED ON  TEST
        Blynk.virtualWrite(V2, 1);
        terminal.println("Saturday-Sunday is ON");
        terminal.flush();
      
      }          
    }
  }
  else{
    terminal.println("Saturday-Sunday INACTIVE today");
    terminal.flush();
    // nothing to do today, check again in 30 SECONDS time    
  }
  terminal.println();
}
}


BLYNK_WRITE(V8)//All days
{  
  if (alldays==1) {         
    sprintf(Date, "%02d/%02d/%04d",  day(), month(), year());
    sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());
  
    TimeInputParam t(param);
  
    terminal.print("All Days Checked schedule at: ");
    terminal.println(Time);
    terminal.flush();
    int dayadjustment = -1;  
    if(weekday() == 1){
      dayadjustment =  6; // needed for Sunday, Time library is day 1 and Blynk is day 7
    }
    if(t.isWeekdaySelected(weekday() + dayadjustment)){ //Time library starts week on Sunday, Blynk on Monday
    terminal.println("ALL DAYS ACTIVE today");
    terminal.flush();
    if (t.hasStartTime()) // Process start time
    {
      terminal.println(String("Start: ") + t.getStartHour() + ":" + t.getStartMinute());
      terminal.flush();
    }
    if (t.hasStopTime()) // Process stop time
    {
      terminal.println(String("Stop : ") + t.getStopHour() + ":" + t.getStopMinute());
      terminal.flush();
    }
    // Display timezone details, for information purposes only 
    terminal.println(String("Time zone: ") + t.getTZ()); // Timezone is already added to start/stop time 
  //  terminal.println(String("Time zone offset: ") + t.getTZ_Offset()); // Get timezone offset (in seconds)
    terminal.flush();
  
     for (int i = 1; i <= 7; i++) {  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)
        if (t.isWeekdaySelected(i)) {
        terminal.println(String("Day ") + i + " is selected");
        terminal.flush();
        }
      } 
    nowseconds = ((hour() * 3600) + (minute() * 60) + second());
    startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
    //Serial.println(startsecondswd);  // used for debugging
    if(nowseconds >= startsecondswd){    
      terminal.print("ALL DAYS STARTED at");
      terminal.println(String(" ") + t.getStartHour() + ":" + t.getStartMinute());
      terminal.flush();
      if(nowseconds <= startsecondswd + 90){    // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, HIGH); // set LED ON
        Blynk.virtualWrite(V2, 1);
        // code here to switch the relay ON
      }      
    }
    else{
      terminal.println("All Day Device NOT STARTED today");
      terminal.flush();
        
    }
    stopsecondswd = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
    //Serial.println(stopsecondswd);  // used for debugging
    if(nowseconds >= stopsecondswd){
      digitalWrite(TestLED, LOW); // set LED OFF
      Blynk.virtualWrite(V2, 0);
      terminal.print("All day STOPPED at");
      terminal.println(String(" ") + t.getStopHour() + ":" + t.getStopMinute());
      terminal.flush();
      if(nowseconds <= stopsecondswd + 90){   // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, LOW); // set LED OFF
        Blynk.virtualWrite(V2, 0);
        // code here to switch the relay OFF
      }              
    }
    else{
      if(nowseconds >= startsecondswd){  
        digitalWrite(TestLED, HIGH); // set LED ON  TEST!!!!!
        Blynk.virtualWrite(V2, 1);
        terminal.println("All day is ON");
        terminal.flush();
 
      }          
    }
  }
  else{
    terminal.println("All day INACTIVE today");
    terminal.flush();
    // nothing to do today, check again in 30 SECONDS time    
  }
  terminal.println();
}
}

BLYNK_WRITE(V10)//Up to you 
{  
  if (uptoyou==1) {         
    sprintf(Date, "%02d/%02d/%04d",  day(), month(), year());
    sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());
  
    TimeInputParam t(param);
  
    terminal.print("Up to you Checked schedule at: ");
    terminal.println(Time);
    terminal.flush();
    int dayadjustment = -1;  
    if(weekday() == 1){
      dayadjustment =  6; // needed for Sunday, Time library is day 1 and Blynk is day 7
    }
    if(t.isWeekdaySelected(weekday() + dayadjustment)){ //Time library starts week on Sunday, Blynk on Monday
    terminal.println("Up to you ACTIVE today");
    terminal.flush();
    if (t.hasStartTime()) // Process start time
    {
      terminal.println(String("Start: ") + t.getStartHour() + ":" + t.getStartMinute());
      terminal.flush();
    }
    if (t.hasStopTime()) // Process stop time
    {
      terminal.println(String("Stop : ") + t.getStopHour() + ":" + t.getStopMinute());
      terminal.flush();
    }
    // Display timezone details, for information purposes only 
    terminal.println(String("Time zone: ") + t.getTZ()); // Timezone is already added to start/stop time 
    terminal.println("At least ONE day MUST be selected");
   // terminal.println(String("Time zone offset: ") + t.getTZ_Offset()); // Get timezone offset (in seconds)
    terminal.flush();
  
     for (int i = 1; i <= 7; i++) {  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)
        if (t.isWeekdaySelected(i)) {
        terminal.println(String("Day ") + i + " is selected");
        terminal.flush();
        }
      } 
    nowseconds = ((hour() * 3600) + (minute() * 60) + second());
    startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
    //Serial.println(startsecondswd);  // used for debugging
    if(nowseconds >= startsecondswd){    
      terminal.print("Up to you STARTED at");
      terminal.println(String(" ") + t.getStartHour() + ":" + t.getStartMinute());
      terminal.flush();
      if(nowseconds <= startsecondswd + 90){    // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, HIGH); // set LED ON
        Blynk.virtualWrite(V2, 1);
        // code here to switch the relay ON
      }      
    }
    else{
      terminal.println("UP to you Device NOT STARTED today");
      terminal.flush();
         
    }
    stopsecondswd = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
    //Serial.println(stopsecondswd);  // used for debugging
    if(nowseconds >= stopsecondswd){
      digitalWrite(TestLED, LOW); // set LED OFF
      Blynk.virtualWrite(V2, 0);
      terminal.print("Up to you STOPPED at");
      terminal.println(String(" ") + t.getStopHour() + ":" + t.getStopMinute());
      terminal.flush();
      if(nowseconds <= stopsecondswd + 90){   // 90s on 60s timer ensures 1 trigger command is sent
        digitalWrite(TestLED, LOW); // set LED OFF
        Blynk.virtualWrite(V2, 0);
        // code here to switch the relay OFF
      }              
    }
    else{
      if(nowseconds >= startsecondswd){  
        digitalWrite(TestLED, HIGH); // set LED ON 
        Blynk.virtualWrite(V2, 1);
        terminal.println("Up to you is ON");
        terminal.flush();
       
      }          
    }
  }
  else{
    terminal.println("Up to you INACTIVE today");
    terminal.flush();
    // nothing to do today, check again in 30 SECONDS time    
  }
  terminal.println();
}
}

void reconnectBlynk() {
  if (!Blynk.connected()) {
    if(Blynk.connect()) {
     BLYNK_LOG("Reconnected");
    } else {
      BLYNK_LOG("Not reconnected");
    }
  }
}

void loop()
{

  if (Blynk.connected()) {
    Blynk.run();
  }
  timer.run();
}
