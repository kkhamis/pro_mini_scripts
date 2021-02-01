#include <SPI.h>
#include <SD.h>
File myFile;
const int chipSelect = 10;
void setup() {
// Open serial communications and wait for port to open:
Serial.begin(9600);
while (!Serial) {
; // wait for serial port to connect. Needed for native USB port only
}
Serial.print("Initializing SD card...");
if (!SD.begin(chipSelect)) {
Serial.println("initialization failed!");
while (1);
}
Serial.println("initialization done.");
// open the file for reading:
myFile = SD.open("DATALOG.TXT");
if (myFile) {
Serial.println("DATALOG.TXT:");
// read from the file until there's nothing else in it:
while (myFile.available()) {
Serial.write(myFile.read());
}
// close the file:
myFile.close();
} else {
// if the file didn't open, print an error:
Serial.println("error opening DATALOG.TXT");
}
}
void loop() {
// nothing happens after setup
}
