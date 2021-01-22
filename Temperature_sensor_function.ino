#include "RTClib.h"
#include <Wire.h>

///----------------------------------------------------------------------////////////

RTC_DS1307 rtc; 
DateTime now;
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
#define SERIESRESISTOR 10000    // the value of the 'other' resisto
 
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
void PrintTime(){
  DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(",");
 //--------------------------------------------------------------------------------   
}

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
rtc.begin();                                    // start clock
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // set clock to time laptop time
     // January 21, 2014 at 3am you would call:
    rtc.adjust(DateTime(2019, 5, 14, 17, 38, 0));
   if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  }
  Serial.print("Timestamp, ");
  Serial.print("Thermistor_pin, ");
  Serial.print("TW_deg, ");
  Serial.print("Thermistor_pin, ");
  Serial.print("TW_deg, ");
  Serial.print("Thermistor_pin, ");
  Serial.println("TW_deg");
}

void loop() {
  DateTime now = rtc.now();
  if(now.second() == 00){
  PrintTime();
  int THERMISTORPIN;
  THERMISTORPIN = A0;
  MeasureTemp(THERMISTORPIN);
  delay(500);
  THERMISTORPIN = A1;
  MeasureTemp(THERMISTORPIN);
  delay(500);
  THERMISTORPIN = A2;
  MeasureTemp(THERMISTORPIN);
  delay(500);
  Serial.println( );
  }

}
