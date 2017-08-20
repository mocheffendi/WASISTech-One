// Energy Meter by Matt Hubbert 14/04/2015
// Designed for use with Siemens S2AS (1000 imps per Kwh)
// LCD switches between three pieces of information:
// i. Current power consumption
// ii. Energy used in the past 24 hours
// iii. Max power recorded.

int flashes = 0;        // Counts the first two flashes to avoid invalid readings when meter starts
float interval;         // The time in seconds between flashes on the meter
int watts;              // The current power consumption in watts
int wattsMax = 0;       // The max recorded watts used at one time
long lastTime;          // The last time the led flashed
int secondsPassed = 0;  // Passing seconds
int minutesPassed = 0;  // Passing minutes
long lastSecond;        // Last measured second
int wattMinute[60];     // Watts measured at each second
int wattHour[60];       // Watts averaged of each minute
int wattDay[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};        // Watts averages of the previous 24 hours
int minuteAverageWatts; // Average watts of the minute
int hourAverageWatts;   // Average watts of the hour
float dayKwh;    // Kwh consumed in the past 24 hours, worked out by the values in wattDay[24]
long lcdCount = 0;       // Counts up and down and changes lcdSwitch
int lcdSwitch = 0;      // Determines what LCD displays

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2);
  attachInterrupt(5, flash, RISING);
  Serial.begin(9600);
}

void flash(){
    interval = (millis() - lastTime) / 1000.0;
    if (flashes > 0){
      // Power (kW) = 3600 (secs in 1hr) divided by (the seconds between flashes * number of Imp/kWh printed on meter
      // Simplified to show watts based on 1000 Imp/Kwh:
      watts = 3600 / (interval * 1000) * 1000;
      if (watts > wattsMax){
         // Update wattsMax if a new record is seen
         wattsMax = watts; 
      }
      //Serial.println(watts);
    }
    flashes++;
    lastTime = millis();
    //Serial.println("flashed");
}

void loop() {
  // Take an average every second
  if (millis() - lastSecond > 1000){ // Adjust this value to perfect time keeping since millis() isn't totally accurate
      wattMinute[secondsPassed] = watts;
      lastSecond = millis();
      secondsPassed++;
      
      Serial.print("Second ");
      Serial.println(secondsPassed);
    
      if (secondsPassed == 60){
         // Take an average of the minute
         int minuteWattSum = 0;
         for(int i=0; i < 60; i++){
           minuteWattSum = minuteWattSum + wattMinute[i];
         }
         minuteAverageWatts = minuteWattSum / 60;
         wattHour[minutesPassed] = minuteAverageWatts;
         secondsPassed = 0;
         minutesPassed++;
         Serial.print("Minute ");
         Serial.println(minutesPassed);
         if (minutesPassed == 60){
             // Take an average of the hour
             int hourWattSum = 0;
             for(int i=0; i < 60; i++){
               hourWattSum = hourWattSum + wattHour[i];
             }
             hourAverageWatts = hourWattSum / 60;
             minutesPassed = 0;
             memcpy(&wattDay[0], &wattDay[1], (23)*sizeof(*wattDay)); // Shift all the hour averages one to the left
             wattDay[23] = hourAverageWatts; // Add the latest average hour to the end of the array
             int dayWattSum = 0;
             for(int k=0; k<24; k++){
               // Get the sum of all 24 hours
               dayWattSum = dayWattSum + wattDay[k];
             }
             dayKwh = dayWattSum / 1000.0;
             //wattMinutes[0] = watts;          // Write watt values to first second and skip it (because timing is short a second)
             //secondsPassed = 1;
          }
      }
  }
  
  lcdCount++;
  if (flashes > 1){
    if (lcdCount == 1){
       //lcdSwitch = 0;
       lcd.setCursor(0, 0);
       lcd.print("Power Usage Now ");
      lcd.setCursor(0, 1);
      if (watts > 1000){
          // Show current consumption in kilowatts
          float kilowatts = watts / 1000.0;
          lcd.print(kilowatts);
          lcd.print(" Kilowatts  ");
      }else{
          // Show current consumption in watts
          lcd.print(watts);
          lcd.print(" watts          ");
      }
    }
    else if (lcdCount == 300000){
       //lcdSwitch = 1;
       lcd.setCursor(0, 0);
       lcd.print("Energy Used <24h");
      lcd.setCursor(0, 1);
      lcd.print(dayKwh);
      lcd.print(" Kwh        ");
    }
    else if (lcdCount == 600000){
       //lcdSwitch = 2;
       lcd.setCursor(0, 0);
       lcd.print("Max Power Seen  ");
      lcd.setCursor(0, 1);
      if (wattsMax > 1000){
          // Show max consumption in kilowatts
          float kilowattsMax = wattsMax / 1000.0;
          lcd.print(kilowattsMax);
          lcd.print(" Kilowatts  ");
      }else{
          // Show max consumption in watts
          lcd.print(wattsMax);
          lcd.print(" watts          ");
      }
    }
    else if (lcdCount == 900000){
       lcdCount = 0;
    }
  }else{
     lcd.setCursor(0, 0);
     lcd.print("   Loading...   ");
     lcdCount = 0;
  }
}
