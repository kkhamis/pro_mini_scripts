//This script is used to test thermistors and uses a real time colock so measurments can be synced with other devices


#include "RTClib.h"
#include <Wire.h>
#include <virtuabotixRTC.h> //Library used




//Wiring SCLK -> 6, I/O -> 7, CE -> 8
//Or CLK -> 6 , DAT -> 7, Reset -> 8

virtuabotixRTC myRTC(6, 7, 8); //If you change the wiring change the pins here also
uint32_t next_sample;
uint32_t now_u;

//******USER EDIT***************//

uint32_t frequency_r = 60; ///sampling frequency in seconds 30 min = 1800, 15 min = 900, 10 min =600

//******USER EDIT***************//

///setup temperature sensor parameters
   // which analog pin for Temp measurement      
#define THERMISTORNOMINAL 10000   // resistance at 25 degrees C      
#define TEMPERATURENOMINAL 25   // temp. for nominal resistance (almost always 25 C)
#define BCOEFFICIENT 3435  // The beta coefficient of the thermistor (usually 3000-4000)r
#define SERIESRESISTOR 10000    // the value of the 'other' resisto
#define THERMISTORPIN1 A0
#define THERMISTORPIN2 A1 //thermistor 2
#define BCOEF2  3950 //other B coefficient

//introduce second thermometer
//////------------------------------------------------------------
float MeasureResist(int NUMSAMPLES, int THERMISTORPIN){
int samples[NUMSAMPLES];
  uint8_t i;
  float average;
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
  
  
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
    // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  
  return average;
}

float MeasureTemp(int NUMSAMPLES, int THERMISTORPIN){
int samples[NUMSAMPLES];
  uint8_t i;
  float average;
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
  
  
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
  
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
//  Serial.print(THERMISTORPIN);
// Serial.print(",  ");
return steinhart;
}

///------------------------------------------------------------------------------
///function to create a clean datatime string

void PrintTime() {
 // This allows for the update of variables for time or accessing the individual elements.
 myRTC.updateTime();
// Start printing elements as individuals
 //Serial.print("Current Date / Time: ");
 Serial.print(myRTC.dayofmonth); //You can switch between day and month if you're using American system
 Serial.print("/");
 Serial.print(myRTC.month);
 Serial.print("/");
 Serial.print(myRTC.year);
 Serial.print(" ");
 Serial.print(myRTC.hours);
 Serial.print(":");
 Serial.print(myRTC.minutes);
 Serial.print(":");
 Serial.print(myRTC.seconds);

// Delay so the program doesn't print non-stop
}
//--------------------------------------------------------------------------------   

void setup() {
 Serial.begin(9600);

// Set the current date, and time in the following format:
 // seconds, minutes, hours, day of the week, day of the month, month, year
 //myRTC.setDS1302Time(30, 21, 15, 5, 26, 2, 2021); //Here you write your actual time/date as shown above 
 //but remember to "comment/remove" this function once you're done
 //The setup is done only one time and the module will continue counting it automatically
  Serial.print("Timestamp, ");
 Serial.println("T1_rsist_25, T1_Temp_25, T1_rsist_25, T2_Temp_25");
}

void loop() {
 //   int timeDiff, lastRead=0;
 //   myRTC.updateTime(); 
 PrintTime();
int NUMSAMPLES_25 = 25;
  float T1_Rsist_25= MeasureResist(NUMSAMPLES_25, THERMISTORPIN1);
 float T1_Temp_25 = MeasureTemp( NUMSAMPLES_25, THERMISTORPIN1);
      Serial.print(",  ");
      Serial.print(T1_Rsist_25);
       Serial.print(",  ");
      Serial.print(T1_Temp_25);
  float T2_Rsist_25= MeasureResist(NUMSAMPLES_25, THERMISTORPIN2);
 float T2_Temp_25 = MeasureTemp( NUMSAMPLES_25, THERMISTORPIN2);
       Serial.print(",  ");
      Serial.print(T2_Rsist_25);
       Serial.print(",  ");
      Serial.print(T2_Temp_25);
Serial.println(",");
  delay(5000);

}
