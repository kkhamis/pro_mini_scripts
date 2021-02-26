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
#define NUMSAMPLES 5 // N for average
#define BCOEFFICIENT 3950  // The beta coefficient of the thermistor (usually 3000-4000)r
#define SERIESRESISTOR 10000    // the value of the 'other' resistor
#define InternalReferenceConstant 1126400L /// need to change based on the board rail voltage use the rail_Volt_cal 
///script to find the board specific value https://github.com/kkhamis/pro_mini_scripts/blob/main/Rail_Volt_cal.ino

 
int samples[NUMSAMPLES];

//////------------------------------------------------------------
void MeasureTemp(int  THERMISTORPIN){
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
  Serial.print(THERMISTORPIN);
  Serial.print(",  ");
  Serial.print(steinhart);
  Serial.print(",");
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
 Serial.println(myRTC.seconds);

// Delay so the program doesn't print non-stop
}
//--------------------------------------------------------------------------------   

void setup() {
 Serial.begin(9600);

// Set the current date, and time in the following format:
 // seconds, minutes, hours, day of the week, day of the month, month, year
 myRTC.setDS1302Time(30, 11, 17, 4, 25, 2, 2021); //Here you write your actual time/date as shown above 
 //but remember to "comment/remove" this function once you're done
 //The setup is done only one time and the module will continue counting it automatically
  Serial.print("Timestamp, ");
  Serial.print("Thermistor_pin, ");
  Serial.print("TW_deg, ");
  Serial.print("Thermistor_pin, ");
  Serial.print("TW_deg, ");
  Serial.print("Thermistor_pin, ");
  Serial.println("TW_deg");
}

void loop() {
 //   int timeDiff, lastRead=0;
 //   myRTC.updateTime(); 
 PrintTime();
  int THERMISTORPIN;
  THERMISTORPIN = A0;
  MeasureTemp(THERMISTORPIN);
  delay(500);
 // THERMISTORPIN = A1;
 // MeasureTemp(THERMISTORPIN);
  delay(500);
  //THERMISTORPIN = A2;
  //MeasureTemp(THERMISTORPIN);
  delay(500);
  Serial.println( );
  }
