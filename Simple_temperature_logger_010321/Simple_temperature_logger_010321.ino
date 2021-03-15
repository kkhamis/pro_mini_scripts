//This script is used to test thermistors and uses a real time colock so measurments can be synced with other devices


#include "RTClib.h"
#include <Wire.h>
#include  <SPI.h>
#include "LowPower.h"     // from https://github.com/rocketscream/Low-Power
#include <SD.h>
///----------------------------------------------------------------------////////////

RTC_DS1307 rtc; 
DateTime now;


///--------------------------------------DATA headers for SD card-------------------------------------------------
const char codebuild[] PROGMEM = __FILE__;  // loads the compiled source code directory & filename into a varaible
const char header[] PROGMEM = "Timestamp,Temp_thermistor(C),Analog(A6),BatteryV,Tair, Humidity% "; //gets written to second line datalog.txt in setup
char CycleTimeStamp[ ] = "0000/00/00,00:00"; //16 ascii characters (without seconds)
const int chipSelect = 10; 





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
  Serial.begin(9600);
  rtc.begin();                                    // start clock
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // set clock to time laptop time
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
  File dataFile = SD.open("DATALOG.TXT", FILE_WRITE);
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

void loop() {
  DateTime now = rtc.now();
  PrintTime();
  sprintf(CycleTimeStamp, "%04d/%02d/%02d %02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());

//meaSURE HERE


//=========concatenate data into a string =====================
// Add each piece of information to the string that gets written to the SD card with:dataFile.println(dataString);
String dataString = ""; //this line simply erases the string
dataString = dataString + CycleTimeStamp;
dataString = dataString + ", ";     //separate data with a comma so they can be imported to separate collumns in a spreadsheet
dataString = dataString + String(XXX);  //this only gives you two decimal places
dataString = dataString + ", "; 
dataString = dataString + String(valA6);
dataString = dataString + ", ";
dataString = dataString + String(batteryVoltage);
dataString = dataString + ", ";
dataString = dataString + String(Tair);
dataString = dataString + ", ";
dataString = dataString + String(humidity);
dataString = dataString + ", ";
//========== Now write the data to the SD card ===========
// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
  File dataFile = SD.open("DATALOG.TXT", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println(F("error opening datalog.txt"));
  }

  
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  } 
