#include "RTClib.h"
#include <Wire.h>
#include <SPI.h>
#include "ClosedCube_HDC1080.h"
#include "LowPower.h"
///----------------------------------------------------------------------////////////



//******USER EDIT***************//

///setup temperature sensor parameters
   // which analog pin for Temp measurement      
#define THERMISTORNOMINAL 10000   // resistance at 25 degrees C      
#define TEMPERATURENOMINAL 25   // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 5 // N for average
#define BCOEFFICIENT 3950  // The beta coefficient of the thermistor (usually 3000-4000)r
#define SERIESRESISTOR 10000    // the value of the 'other' resisto
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


 //--------------------------------------------------------------------------------   

void setup() {
  Serial.begin(9600);
 // hdc1080.begin(0x40);
  Serial.print("Thermistor_pin, ");
  Serial.print("TW_deg, ");
  Serial.print("Thermistor_pin, ");
  Serial.print("TW_deg, ");
  Serial.print("Thermistor_pin, ");
  Serial.print("TW_deg");
  //Serial.print("Tair_degC");
 // Serial.println("Humidity_%");
}

void loop() {
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
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
    //measuring and displaying results
 // Serial.print(hdc1080.readTemperature()); // Acquisition of temperature
 // Serial.print(",");
 // Serial.print(hdc1080.readHumidity()); // Acquisition of humidity
  //Serial.print(",");
  Serial.println( );
   delay(500);
  }
