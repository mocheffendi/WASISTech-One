/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to synchronize Button widget
  and physical button state.

  WARNING :
  For this example you'll need SimpleTimer library:
    https://github.com/jfturcot/SimpleTimer

  App project setup:
    Button widget attached to V1 (Switch mode)
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

const byte pwmLED = 13;
 
// define directions for LED fade
#define UP 0
#define DOWN 1
 
// constants for min and max PWM
const int minPWM = 0;
const int maxPWM = 255;
 
// State Variable for Fade Direction
byte fadeDirection = UP;
 
// Global Fade Value
// but be bigger than byte and signed, for rollover
int fadeValue = 0;
 
// How smooth to fade?
byte fadeIncrement = 5;
 
// millis() timing Variable, just for fading
unsigned long previousFadeMillis;
 
// How fast to increment?
int fadeInterval = 50;
 

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "ba894acb3c884bf2ad41ff4cad816c82";

// Set your Relay and physical button pins here
const int RelayPin1 = 22;
const int RelayPin2 = 23;
const int RelayPin3 = 24;
const int RelayPin4 = 25;
const int RelayPin5 = 26;
const int RelayPin6 = 27;
const int RelayPin7 = 28;
const int RelayPin8 = 29;
const int RelayPin9 = 30;
const int RelayPin10 = 31;
const int RelayPin11 = 32;
const int RelayPin12 = 33;
const int RelayPin13 = 34;
const int RelayPin14 = 35;
const int RelayPin15 = 36;
const int RelayPin16 = 37;

const int btnPin1 = 2;
const int btnPin2 = 3;
const int btnPin3 = 5;
const int btnPin4 = 6;
const int btnPin5 = 7;
const int btnPin6 = 8;
const int btnPin7 = 9;
const int btnPin8 = 14;
const int btnPin9 = 15;
const int btnPin10 = 16;
const int btnPin11 = 17;
const int btnPin12 = 18;
const int btnPin13 = 19;
const int btnPin14 = 20;
const int btnPin15 = 21;
const int btnPin16 = 1;



BlynkTimer timer;
void checkPhysicalButton();

int RelayState1 = HIGH;
int RelayState2 = HIGH;
int RelayState3 = HIGH;
int RelayState4 = HIGH;
int RelayState5 = HIGH;
int RelayState6 = HIGH;
int RelayState7 = HIGH;
int RelayState8 = HIGH;
int RelayState9 = HIGH;
int RelayState10 = HIGH;
int RelayState11 = HIGH;
int RelayState12 = HIGH;
int RelayState13 = HIGH;
int RelayState14 = HIGH;
int RelayState15 = HIGH;
int RelayState16 = HIGH;

int btnState1 = HIGH;
int btnState2 = HIGH;
int btnState3 = HIGH;
int btnState4 = HIGH;
int btnState5 = HIGH;
int btnState6 = HIGH;
int btnState7 = HIGH;
int btnState8 = HIGH;
int btnState9 = HIGH;
int btnState10 = HIGH;
int btnState11 = HIGH;
int btnState12 = HIGH;
int btnState13 = HIGH;
int btnState14 = HIGH;
int btnState15 = HIGH;
int btnState16 = HIGH;

// Every time we connect to the cloud...
BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V11);
  Blynk.syncVirtual(V12);
  Blynk.syncVirtual(V13);
  Blynk.syncVirtual(V14);
  Blynk.syncVirtual(V15);
  Blynk.syncVirtual(V16);

  // Alternatively, you could override server state using:
  //Blynk.virtualWrite(V1, RelayState1);
}

// When App button is pushed - switch the state
BLYNK_WRITE(V1) {
  RelayState1 = param.asInt();
  digitalWrite(RelayPin1, RelayState1);
}

BLYNK_WRITE(V2) {
  RelayState2 = param.asInt();
  digitalWrite(RelayPin2, RelayState2);
}

BLYNK_WRITE(V3) {
  RelayState3 = param.asInt();
  digitalWrite(RelayPin3, RelayState3);
}

BLYNK_WRITE(V4) {
  RelayState4 = param.asInt();
  digitalWrite(RelayPin4, RelayState4);
}

BLYNK_WRITE(V5) {
  RelayState5 = param.asInt();
  digitalWrite(RelayPin5, RelayState5);
}

BLYNK_WRITE(V6) {
  RelayState6 = param.asInt();
  digitalWrite(RelayPin6, RelayState6);
}

BLYNK_WRITE(V7) {
  RelayState7 = param.asInt();
  digitalWrite(RelayPin7, RelayState7);
}

BLYNK_WRITE(V8) {
  RelayState8 = param.asInt();
  digitalWrite(RelayPin8, RelayState8);
}

BLYNK_WRITE(V9) {
  RelayState9 = param.asInt();
  digitalWrite(RelayPin9, RelayState9);
}

BLYNK_WRITE(V10) {
  RelayState10 = param.asInt();
  digitalWrite(RelayPin10, RelayState10);
}

BLYNK_WRITE(V11) {
  RelayState11 = param.asInt();
  digitalWrite(RelayPin11, RelayState11);
}

BLYNK_WRITE(V12) {
  RelayState12 = param.asInt();
  digitalWrite(RelayPin12, RelayState12);
}

BLYNK_WRITE(V13) {
  RelayState13 = param.asInt();
  digitalWrite(RelayPin13, RelayState13);
}

BLYNK_WRITE(V14) {
  RelayState14 = param.asInt();
  digitalWrite(RelayPin14, RelayState14);
}

BLYNK_WRITE(V15) {
  RelayState15 = param.asInt();
  digitalWrite(RelayPin15, RelayState15);
}

BLYNK_WRITE(V16) {
  RelayState16 = param.asInt();
  digitalWrite(RelayPin16, RelayState16);
}


void checkPhysicalButton()
{
  if (digitalRead(btnPin1) == LOW) {
    // btnState1 is used to avoid sequential toggles
    if (btnState1 != LOW) {

      // Toggle Relay state
      RelayState1 = !RelayState1;
      digitalWrite(RelayPin1, RelayState1);

      // Update Button Widget
      Blynk.virtualWrite(V1, RelayState1);
    }
    btnState1 = LOW;
  } else {
    btnState1 = HIGH;
  }

  if (digitalRead(btnPin2) == LOW) {
    // btnState2 is used to avoid sequential toggles
    if (btnState2 != LOW) {

      // Toggle Relay state
      RelayState2 = !RelayState2;
      digitalWrite(RelayPin2, RelayState2);

      // Update Button Widget
      Blynk.virtualWrite(V2, RelayState2);
    }
    btnState2 = LOW;
  } else {
    btnState2 = HIGH;
  }

  if (digitalRead(btnPin3) == LOW) {
    // btnState3 is used to avoid sequential toggles
    if (btnState3 != LOW) {

      // Toggle Relay state
      RelayState3 = !RelayState3;
      digitalWrite(RelayPin3, RelayState3);

      // Update Button Widget
      Blynk.virtualWrite(V3, RelayState3);
    }
    btnState3 = LOW;
  } else {
    btnState3 = HIGH;
  }

  if (digitalRead(btnPin3) == LOW) {
    // btnState3 is used to avoid sequential toggles
    if (btnState3 != LOW) {

      // Toggle Relay state
      RelayState3 = !RelayState3;
      digitalWrite(RelayPin3, RelayState3);

      // Update Button Widget
      Blynk.virtualWrite(V3, RelayState3);
    }
    btnState3 = LOW;
  } else {
    btnState3 = HIGH;
  }

  if (digitalRead(btnPin4) == LOW) {
    // btnState4 is used to avoid sequential toggles
    if (btnState4 != LOW) {

      // Toggle Relay state
      RelayState4 = !RelayState4;
      digitalWrite(RelayPin4, RelayState4);

      // Update Button Widget
      Blynk.virtualWrite(V4, RelayState4);
    }
    btnState4 = LOW;
  } else {
    btnState4 = HIGH;
  }

  if (digitalRead(btnPin5) == LOW) {
    // btnState5 is used to avoid sequential toggles
    if (btnState5 != LOW) {

      // Toggle Relay state
      RelayState5 = !RelayState5;
      digitalWrite(RelayPin5, RelayState5);

      // Update Button Widget
      Blynk.virtualWrite(V5, RelayState5);
    }
    btnState5 = LOW;
  } else {
    btnState5 = HIGH;
  }

  if (digitalRead(btnPin6) == LOW) {
    // btnState6 is used to avoid sequential toggles
    if (btnState6 != LOW) {

      // Toggle Relay state
      RelayState6 = !RelayState6;
      digitalWrite(RelayPin6, RelayState6);

      // Update Button Widget
      Blynk.virtualWrite(V6, RelayState6);
    }
    btnState6 = LOW;
  } else {
    btnState6 = HIGH;
  }

  if (digitalRead(btnPin7) == LOW) {
    // btnState7 is used to avoid sequential toggles
    if (btnState7 != LOW) {

      // Toggle Relay state
      RelayState7 = !RelayState7;
      digitalWrite(RelayPin7, RelayState7);

      // Update Button Widget
      Blynk.virtualWrite(V7, RelayState7);
    }
    btnState7 = LOW;
  } else {
    btnState7 = HIGH;
  }

  if (digitalRead(btnPin8) == LOW) {
    // btnState8 is used to avoid sequential toggles
    if (btnState8 != LOW) {

      // Toggle Relay state
      RelayState8 = !RelayState8;
      digitalWrite(RelayPin8, RelayState8);

      // Update Button Widget
      Blynk.virtualWrite(V8, RelayState8);
    }
    btnState8 = LOW;
  } else {
    btnState8 = HIGH;
  }

  if (digitalRead(btnPin9) == LOW) {
    // btnState9 is used to avoid sequential toggles
    if (btnState9 != LOW) {

      // Toggle Relay state
      RelayState9 = !RelayState9;
      digitalWrite(RelayPin9, RelayState9);

      // Update Button Widget
      Blynk.virtualWrite(V9, RelayState9);
    }
    btnState9 = LOW;
  } else {
    btnState9 = HIGH;
  }

  if (digitalRead(btnPin10) == LOW) {
    // btnState10 is used to avoid sequential toggles
    if (btnState10 != LOW) {

      // Toggle Relay state
      RelayState10 = !RelayState10;
      digitalWrite(RelayPin10, RelayState10);

      // Update Button Widget
      Blynk.virtualWrite(V10, RelayState10);
    }
    btnState10 = LOW;
  } else {
    btnState10 = HIGH;
  }

  if (digitalRead(btnPin11) == LOW) {
    // btnState11 is used to avoid sequential toggles
    if (btnState11 != LOW) {

      // Toggle Relay state
      RelayState11 = !RelayState11;
      digitalWrite(RelayPin11, RelayState11);

      // Update Button Widget
      Blynk.virtualWrite(V11, RelayState11);
    }
    btnState11 = LOW;
  } else {
    btnState11 = HIGH;
  }

  if (digitalRead(btnPin12) == LOW) {
    // btnState12 is used to avoid sequential toggles
    if (btnState12 != LOW) {

      // Toggle Relay state
      RelayState12 = !RelayState12;
      digitalWrite(RelayPin12, RelayState12);

      // Update Button Widget
      Blynk.virtualWrite(V12, RelayState12);
    }
    btnState12 = LOW;
  } else {
    btnState12 = HIGH;
  }

  if (digitalRead(btnPin13) == LOW) {
    // btnState13 is used to avoid sequential toggles
    if (btnState13 != LOW) {

      // Toggle Relay state
      RelayState13 = !RelayState13;
      digitalWrite(RelayPin13, RelayState13);

      // Update Button Widget
      Blynk.virtualWrite(V13, RelayState13);
    }
    btnState13 = LOW;
  } else {
    btnState13 = HIGH;
  }

  if (digitalRead(btnPin14) == LOW) {
    // btnState14 is used to avoid sequential toggles
    if (btnState14 != LOW) {

      // Toggle Relay state
      RelayState14 = !RelayState14;
      digitalWrite(RelayPin14, RelayState14);

      // Update Button Widget
      Blynk.virtualWrite(V14, RelayState14);
    }
    btnState14 = LOW;
  } else {
    btnState14 = HIGH;
  }

  if (digitalRead(btnPin15) == LOW) {
    // btnState15 is used to avoid sequential toggles
    if (btnState15 != LOW) {

      // Toggle Relay state
      RelayState15 = !RelayState15;
      digitalWrite(RelayPin15, RelayState15);

      // Update Button Widget
      Blynk.virtualWrite(V15, RelayState15);
    }
    btnState15 = LOW;
  } else {
    btnState15 = HIGH;
  }

  if (digitalRead(btnPin16) == LOW) {
    // btnState16 is used to avoid sequential toggles
    if (btnState16 != LOW) {

      // Toggle Relay state
      RelayState16 = !RelayState16;
      digitalWrite(RelayPin16, RelayState16);

      // Update Button Widget
      Blynk.virtualWrite(V16, RelayState16);
    }
    btnState16 = LOW;
  } else {
    btnState16 = HIGH;
  }
}

void doTheFade(unsigned long thisMillis) {
  // is it time to update yet?
  // if not, nothing happens
  if (thisMillis - previousFadeMillis >= fadeInterval) {
    // yup, it's time!
    if (fadeDirection == UP) {
      fadeValue = fadeValue + fadeIncrement;  
      if (fadeValue >= maxPWM) {
        // At max, limit and change direction
        fadeValue = maxPWM;
        fadeDirection = DOWN;
      }
    } else {
      //if we aren't going up, we're going down
      fadeValue = fadeValue - fadeIncrement;
      if (fadeValue <= minPWM) {
        // At min, limit and change direction
        fadeValue = minPWM;
        fadeDirection = UP;
      }
    }
    // Only need to update when it changes
    analogWrite(pwmLED, fadeValue);  
 
    // reset millis for the next iteration (fade timer only)
    previousFadeMillis = thisMillis;
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);
  pinMode(RelayPin6, OUTPUT);
  pinMode(RelayPin7, OUTPUT);
  pinMode(RelayPin8, OUTPUT);
  pinMode(RelayPin9, OUTPUT);
  pinMode(RelayPin10, OUTPUT);
  pinMode(RelayPin11, OUTPUT);
  pinMode(RelayPin12, OUTPUT);
  pinMode(RelayPin13, OUTPUT);
  pinMode(RelayPin14, OUTPUT);
  pinMode(RelayPin15, OUTPUT);
  pinMode(RelayPin16, OUTPUT);
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(btnPin3, INPUT_PULLUP);
  pinMode(btnPin4, INPUT_PULLUP);
  pinMode(btnPin5, INPUT_PULLUP);
  pinMode(btnPin6, INPUT_PULLUP);
  pinMode(btnPin7, INPUT_PULLUP);
  pinMode(btnPin8, INPUT_PULLUP);
  pinMode(btnPin9, INPUT_PULLUP);
  pinMode(btnPin10, INPUT_PULLUP);
  pinMode(btnPin11, INPUT_PULLUP);
  pinMode(btnPin12, INPUT_PULLUP);
  pinMode(btnPin13, INPUT_PULLUP);
  pinMode(btnPin14, INPUT_PULLUP);
  pinMode(btnPin15, INPUT_PULLUP);
  pinMode(btnPin16, INPUT_PULLUP);
  digitalWrite(RelayPin1, RelayState1);
  digitalWrite(RelayPin2, RelayState2);
  digitalWrite(RelayPin3, RelayState3);
  digitalWrite(RelayPin4, RelayState4);
  digitalWrite(RelayPin5, RelayState5);
  digitalWrite(RelayPin6, RelayState6);
  digitalWrite(RelayPin7, RelayState7);
  digitalWrite(RelayPin8, RelayState8);
  digitalWrite(RelayPin9, RelayState9);
  digitalWrite(RelayPin10, RelayState10);
  digitalWrite(RelayPin11, RelayState11);
  digitalWrite(RelayPin12, RelayState12);
  digitalWrite(RelayPin13, RelayState13);
  digitalWrite(RelayPin14, RelayState14);
  digitalWrite(RelayPin15, RelayState15);
  digitalWrite(RelayPin16, RelayState16);

  analogWrite(pwmLED, fadeValue);
  
  // Setup a function to be called every 100 ms
  timer.setInterval(100L, checkPhysicalButton);
}

void loop()
{
  Blynk.run();
  timer.run();

  unsigned long currentMillis = millis();
  doTheFade(currentMillis);
}
