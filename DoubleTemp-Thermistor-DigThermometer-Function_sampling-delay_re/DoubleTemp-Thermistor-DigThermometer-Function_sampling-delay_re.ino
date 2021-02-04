#include "RTClib.h"
#include <Wire.h>
#include <DallasTemperature.h>
#include <advancedSerial.h>
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
#define BCOEFFICIENT 3435  // The beta coefficient of the thermistor (usually 3000-4000)r
#define SERIESRESISTOR 10000    // the value of the 'other' resisto
#define THERMISTORPIN A0

//introduce second thermometer
#define ONE_WIRE_BUS 13 //digital thermometer
//////------------------------------------------------------------
float MeasureTemp(int NUMSAMPLES, int Wait){
int samples[NUMSAMPLES];
  uint8_t i;
  float average;
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(Wait);
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

// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 
void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
 // Start up the library 
 sensors.begin(); 
 Serial.println("Thermistor5, Thermistor5_wait, Thermistor50, Thermistor50_wait, DigitalTherm,");
} 

void loop() {
//thermistor
//  Serial.print(" Thermistor_pin, ");
  int NUMSAMPLES_5 = 5;
  int Wait_10 = 10;
  float Tmistor5= MeasureTemp( NUMSAMPLES_5, Wait_10);
      Serial.print(Tmistor5);
       Serial.print(",  ");
 //Thermistor 15 samples
 int Wait_100 = 100;
  float Tmistor5w = MeasureTemp( NUMSAMPLES_5, Wait_100);
      Serial.print(Tmistor5w);
       Serial.print(",  ");
 //Thermistor with 50 samples
int NUMSAMPLES_50 = 50;
float Tmistor50 = MeasureTemp(NUMSAMPLES_50,Wait_10);
   Serial.print(Tmistor50);
      Serial.print(",  ");
 // Thermistor with 100 samples
float Tmistor50w = MeasureTemp(NUMSAMPLES_50,Wait_100);
  Serial.print(Tmistor50w);
     Serial.print(",  ");
//digital thermometer
// Serial.print(" Digital Thermometer, "); 
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"?  
Serial.println(",");
  delay(5000);
}
