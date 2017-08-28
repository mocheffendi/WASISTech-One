/*

 There is a bug in blynk library (v0.3.4), which hangs the Blynk.begin(auth) function (keeps restarting the board every minute), if there is no network access during startup. To bypass this problem, edit this file in:
  libraries\Blynk\Adapters\BlynkEthernet.h, at line 58:

  comment out the: BLYNK_FATAL("DHCP Failed!"); line,

  and insert these lines:
  BLYNK_LOG("DHCP failed. check your network connection!");
  BLYNK_LOG("bypassing blynk...");
  return;

*/

// global variables
bool blynkAllowed = true;
bool firstConnect = true;

void setup()
{
  //Serial.begin(9600);
  
  Blynk.begin(auth);       // configuring ethernet. timeout: 60 seconds

  if (millis() < 50000) {  // if Blynk.begin(auth) finished under 50 seconds == connected to network
    beep(500, 0);
    //Serial.println("connected to local network!");

    tryConnecting();                          // try to connect to blynk server
    tryConnecting();                          // i put this twice, because very rarely it fails the first connection

    timer.setInterval(1000, updateBlynk);     // set how often send data to the blynk server
    timer.setInterval(60000, tryConnecting);  // if lost connection, try to connect time by time

    //Serial.println("reconnect attempts to blynk server allowed in main loop");
  }
  else {                   // if Blynk.begin(auth) took more than 50 seconds == no network connection
    blynkAllowed = false;
    beep(2000, 0);

    //Serial.println("no network access. no reconnect attempts will be allowed in main loop");
  }
}

void loop()
{
  checkBlynk();
  timer.run();  // initiates SimpleTimer
}

void checkBlynk()
{
  if (blynkAllowed) {
    if (firstConnect) tryConnecting();
    if (Blynk.connected()) Blynk.run();
  }
}

void tryConnecting()
{
  if (!Blynk.connected()) {
    Blynk.connect();  // try to connect for 15 seconds, every time, as set  in: timer.setInterval(60000, tryConnecting) in void setup
                      // but let the main loop run

    if (Blynk.connected()) {
      if (firstConnect) {
        // do some none relevant stuff here, and
        firstConnect = false;
      }
      
      //Serial.println("connected to blynk server!");
      beep(500, 0);
    }
  }
}
