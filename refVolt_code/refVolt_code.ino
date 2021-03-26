#define VrefPin 2     // pin A0 - the pin our reference is connected to
#define VxPin 6       // use will use analog pin 1 for voltage x
#define VyPin 1       // use will use analog pin 1 for voltage y

// for voltage conversion
float vRef = 2.048; // the accurate value of your reference eg 4.096
/* *** change the value to match the reference voltage you are using  ***  */
long nX,nRef; //the value we read from the ADC
float vX;  //measured voltage values after calibration correction

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

//

void setup()
{
  Serial.begin(9600);
  analogReference(DEFAULT);    // program is being tested on a 5V Arduino Nano so this will be ROUGHLY 5.0V; as measured on mine 4.65V
  //*** The chip on your Arduino - depending on type -  is provided with SOME of the following reference voltages
  //DEFAULT: the default analog reference of "5 volts" (on 5V Arduino boards) or "3.3 volts" (on 3.3V Arduino boards)
  //INTERNAL: a built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328 and 2.56 volts on the ATmega8 and 32U4 chip boards.
  //EXTERNAL: the voltage applied to the AREF pin.  The Arduino Micro Pro does not have a pin to let you do this

  nRef = analogRead(VrefPin);     // dummy read to settle ADC Mux
  delay(5);
}

void loop()
{
  nX=0; nRef=0; //initialise values to prepare for aggregation
  for(int m=0; m<nTimes; m++){
    nX += readInput(VxPin, nSamp, interval);
   nRef += readInput(VrefPin, nSamp, interval);
  }
  // at this point we have the numbers corresponding to all the input readings * count * nTimes
  vX = convReadings(nX, nRef, vRef);
 Serial.print("Vx is  ");
  Serial.print(vX, 3); 
  Serial.println(" ");
  delay(2000);
}
