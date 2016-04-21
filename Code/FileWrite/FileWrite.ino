/*
  SD card read/write
 
 This example shows how to read and write data to and from an SD card file 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** UNO:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
  and pin #10 (SS) must be an output
 ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
  and pin #52 (SS) must be an output
 ** Leonardo: Connect to hardware SPI via the ICSP header

 
 created   Nov 2010  by David A. Mellis
 modified 9 Apr 2012  by Tom Igoe
 
 This example code is in the public domain.
 	 
 */
 
#include <SPI.h>
#include <SD.h>

File myFile;

// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 10;

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  //******************for main code******************
  writeToFile("test2.txt","data 1 2 3");
  //****************end for main code****************
  
}

void loop()
{
	// nothing happens after setup
}
void writeToFile(String fileNameString, String data){
  char fileName[100]; // Or something long enough to hold the longest file name you will ever use.
   fileNameString.toCharArray(fileName, sizeof(fileName));
  boolean firstUse = false;
   
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(SS, OUTPUT);
   
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
  
  //check file exsistance
  if (SD.exists(fileName)) {
    Serial.println(fileNameString + " exists.");
  }
  else {
    Serial.println(fileNameString + " doesn't exist.");
    firstUse = true;
  }
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(fileName, FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to " + fileNameString + "...");
    if(firstUse) myFile.println("*****header******");
    myFile.println(data);
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileNameString);
  }
}

