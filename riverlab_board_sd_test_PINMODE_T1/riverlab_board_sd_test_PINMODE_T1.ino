//This script is used to test thermistors and uses a real time colock so measurments can be synced with other devices


#include "RTClib.h"
#include <Wire.h>
#include  <SPI.h>
#include "LowPower.h"     // from https://github.com/rocketscream/Low-Power
#include <SD.h>
///----------------------------------------------------------------------////////////

RTC_DS1307 rtc; 
DateTime now;
//#define LED 3 // bright light when recording

///--------------------------------------DATA headers for SD card-------------------------------------------------
const char codebuild[] PROGMEM = __FILE__;  // loads the compiled source code directory & filename into a varaible
const char header[] PROGMEM = "Timestamp,Resis_T6,Temp_T6,Res_T7,Temp_T7,Res_T8,Temp_T8,Res_T9,Temp_T9,Res_T10,Temp_T10, "; //gets written to second line datalog.txt in setup
char CycleTimeStamp[ ] = "0000/00/00,00:00"; //16 ascii characters (without seconds)
const int chipSelect = 14; ///AO on riverlab board is 14

//******USER EDIT***************//

///setup temperature sensor parameters
   // which analog pin for Temp measurement      
#define THERMISTORNOMINAL 10000   // resistance at 25 degrees C      
#define TEMPERATURENOMINAL 25   // temp. for nominal resistance (almost always 25 C)
#define BCOEFFICIENT 3435  // The beta coefficient of the thermistor (usually 3000-4000)r
#define SERIESRESISTOR 10000    // the value of the 'other' resisto

#define THERMISTORPIN1 A1 //T4
#define THERMISTORPIN2 A2 //T5
#define THERMISTORPIN6 A6 //T0


///------------------------------------------------------------------------------
///function to create a clean datatime string

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
  // the following pullup resistors only needs to be enabled for the ProMini builds - not the UNO loggers
  pinMode(chipSelect, OUTPUT); digitalWrite(chipSelect, HIGH); //ALWAYS pullup the ChipSelect pin with the SD library
  pinMode(3, OUTPUT);  digitalWrite(3, HIGH);///POWER UP 3.3v out
  pinMode(A6,INPUT);
  
  delay(500);
  Serial.begin(9600);
  rtc.begin();                                    // start clock
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // set clock to time laptop time
  // January 21, 2014 at 3am you would call:
  //rtc.adjust(DateTime(2019, 5, 14, 17, 38, 0));
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
 while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  DateTime now = rtc.now();
  sprintf(CycleTimeStamp, "%04d/%02d/%02d %02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());
  
  Serial.print(F("Initializing SD card…"));
// print lines in the setup loop only happen once
// see if the card is present and can be initialized
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    // don’t do anything more:
    return;
  }
  Serial.println(F("card initialized."));
  
  // You must already have a plain text file file named ‘datalog.txt’ on the SD already for this to work!
  
  //————-print a header to the data file———- OPTIONAL!
  File dataFile = SD.open("DATA00.CSV", FILE_WRITE);
  if (dataFile) { // if the file is available, write to it:
    dataFile.println((__FlashStringHelper*)codebuild); // writes the entire path + filename to the start of the data file
    dataFile.println((__FlashStringHelper*)header);
    dataFile.close();
  }
  else {
     Serial.println("error opening datalog.txt"); // if the file isn’t open, pop up an error:
  }
}

//replace if below with
//if ((now.minute()==0) |  (now.minute()==15)| (now.minute()==30)| (now.minute()==35)|(now.minute()==40)|(now.minute()==45)|(now.minute()==50)){
 // PrintTime();
//if ((now.second()==0){

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
    // convert the va
//measure to resistance
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

void loop() {
  DateTime now = rtc.now();
  PrintTime();
  sprintf(CycleTimeStamp, "%04d/%02d/%02d %02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());

int NUMSAMPLES_25 = 25;
  float T6_Rsist_25= MeasureResist(NUMSAMPLES_25, THERMISTORPIN6);
 float T6_Temp_25 = MeasureTemp( NUMSAMPLES_25, THERMISTORPIN6);
  Serial.print(",  ");
 Serial.print(T6_Rsist_25);
   Serial.print(",  ");
 Serial.print(T6_Temp_25);
Serial.println(",");
 delay(10);
//=========concatenate data into a string =====================
// Add each piece of information to the string that gets written to the SD card with:dataFile.println(dataString);
String dataString = ""; //this line simply erases the string
dataString = dataString + CycleTimeStamp;
dataString = dataString + ", ";     //separate data with a comma so they can be imported to separate collumns in a spreadsheet
dataString = dataString + String(T6_Rsist_25);
dataString = dataString + ", ";
dataString = dataString + String(T6_Temp_25);
dataString = dataString + ", ";
//========== Now write the data to the SD card ===========
// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
  File dataFile = SD.open("Data00.CSV", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println(F("error opening DATA00.CSV"));

  }

  
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  } 
