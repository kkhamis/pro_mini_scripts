/*
 * Wire voltage reference to A1, Thermistor to A2, 1k resistor between A6 and 3.3v
 * V0 =A2 = VxPin 
 * vcc = A6 = VyPin
 * Vo = R / (R + 10K) * Vcc
 * R=10k/(vcc/vo)
 */
#include <SPI.h>
#include "SdFat.h"

// SD chip select pin.  A0 on riverlabs board = 14
const uint8_t chipSelect = 14;

//==============================================================================
// User functions.  Edit writeHeader() and logData() for your requirements.
///setup temperature sensor parameters
 // which analog pin for Temp measurement      
#define THERMISTORNOMINAL 10000   // resistance at 25 degrees C      
#define TEMPERATURENOMINAL 25   // temp. for nominal resistance (almost always 25 C)
#define BCOEFFICIENT 3435  // The beta coefficient of the thermistor (usually 3000-4000)r
#define SERIESRESISTOR 10000    // the value of the 'other' resisto
#define THERMISTORPIN1 A1 
#define THERMISTORPIN2 A2
#define THERMISTORPIN6 A6
int NUMSAMPLES_25 = 25;
//Analog input pin assignments

#define VrefPin A1     // pin A0 - the pin our reference is connected to
#define VxPin A2       // use will use analog pin 1 for voltage x
#define VyPin A6       // use will use analog pin 2 for voltage y

// for voltage conversion
float vRef = 2.05; // the accurate value of your reference eg 4.096
/* *** change the value to match the reference voltage you are using  ***  */
long nX, nY, nRef; //the value we read from the ADC
float vX, vY, T,R;  //measured voltage values after calibration correction

//---------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
// sampling parameters

int nSamp = 5;   // Number of Samples to take for each reading
int nTimes = 7; //number of times to aggregate samples
int interval = 7; //milliseconds interval between successive readings

//take a set of "count" readings from selected analog input, with a delay of "pause" between readings.
long readInput(int pin, int count, int pause) {  
  int n = analogRead( pin); // dummy read
  n = 0; // initialise for aggregation
  delay(pause);
  for (int i = 0; i < count; i++ ) {
   n += analogRead( pin);
    delay(pause); //short delay between readings - could use millis instead
  }
  return n ; //returns a number count * reading
}

// Convert the number reading to voltage
float convReadings(long nZ, long nR, float vR)  
{
 float ztrue = (vR * nZ) / nR; //apply correction
 // provided Vref and Vx .. Vy are measured the SAME number of times they cancel out. Vx*10 / Vref*10 is the same as Vx/Vref
  return (ztrue); //return the result as a voltage
}
//------------------
float convert2Temp(float vy, float vx){
float average;
  
  // convert the value to resistance
 
  average = (SERIESRESISTOR * vx) / (vy - vx) ;
  
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



//------------------------------------------------------------------------------
void setup() {

  pinMode(3, OUTPUT); 
  digitalWrite(3, HIGH);
  delay(500);
  
  Serial.begin(9600);
  nRef = analogRead(VrefPin); 
  delay(10);
  Serial.println(nRef);
}
//------------------------------------------------------------------------------
void loop() {
 nX=0; nY=0; nRef=0; T=0; R=0; //initialise values to prepare for aggregation
  for(int m=0; m<nTimes; m++){
    nX += readInput(VxPin, nSamp, interval);
    nY += readInput(VyPin, nSamp, interval);
    nRef += readInput(VrefPin, nSamp, interval);
  }
  // at this point we have the numbers corresponding to all the input readings * count * nTimes
  vX = convReadings(nX, nRef, vRef);
  vY = convReadings(nY, nRef, vRef);
  Serial.print("Vx is  ");
  Serial.print(vX, 3);
  Serial.print(";  Vy is  ");
  Serial.print(vY, 3);
  Serial.println(" ");
  Serial.print(";  ref is  ");
  Serial.println(nRef);
  delay(2000);
  T = convert2Temp(vY,vX);
  Serial.print(";  Temp is  ");
  Serial.println(T);
  R =  (SERIESRESISTOR * vX) / (vY - vX);
  Serial.print(";  Resistance is  ");
  Serial.println(R);
}
