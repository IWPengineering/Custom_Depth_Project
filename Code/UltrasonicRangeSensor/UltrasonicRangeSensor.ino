#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>

/*
* The Circuit:
* Ultrasonic Range Sensor: Pin 5 - Arduino Pin 6
* Adafruit Data Logger Shield: MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
*  and pin #10 (SS) must be an output
*/
#define STORAGE_INTERVAL 5


SoftwareSerial sensorSerial(6, 7, true); // RX, TX
File myFile;
// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 10;

RTC_DS1307 RTC;
DateTime now;

const int rangeIn = 0;
int lastSaveMinute = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  sensorSerial.begin(9600);
  
  //initialize SD card
  pinMode(SS, OUTPUT);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work.
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  Wire.begin();
  RTC.begin();
  
if (! RTC.isrunning()) {
  Serial.println("RTC is NOT running!");
  // following line sets the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}




void loop() {
  // put your main code here, to run repeatedly:
  now = RTC.now();
  
  /*
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  Serial.println("distance: " + getRange() + "mm");
  Serial.println();
  */



  if((now.minute()%5)==0){
    if(lastSaveMinute != now.minute()){
      Serial.println("******Running Periodic Sequence******");
      Serial.println(timeStamp());
      Serial.println("distance: " + getRange() + "mm");
      writeToFile("test.txt", timeStamp(), getRange());
      
      lastSaveMinute = now.minute();
    }
  }
  delay(1000);
  
  /* Use for analog read pin of Ultrasonic Range Sensor
  double inch = analogRead(rangeIn) * 0.1981 - 1.9613;
  Serial.println( (String)inch + " inches");
  delay(250);
  */
}


/**
* getRange
* modified 18 April 2016
* by David MacBride
* 
* Construncts and returns the distance from the Ultrasontic Range Sensor (mySerial) in the form of a String
* Will return "FAIL" in the case of an error (this is rare, but should be fixed in the future)
* 
*  Note:
* Not all pins on the Mega and Mega 2560 support change interrupts,
* so only the following can be used for RX:
* 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
*
* Not all pins on the Leonardo support change interrupts,
* so only the following can be used for RX:
* 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
**/
String getRange(){
  String distance = "";
  while(sensorSerial.available())                //clear serial buffer
    sensorSerial.read();
  while(true){
    if (sensorSerial.available()){
      if (sensorSerial.read()== 'R'){            //Search for begining char
        for(int i = 0; i<4; i++){                //construct distance String
          while(!sensorSerial.available()){};
          char nextChar = (char)sensorSerial.read();
          if(nextChar != 'R'){                   //add next char
            distance += nextChar;
          }else{                                 //communication error detection
            return "FAIL";
          }
        }
        break;
      }
    }
  }
  return distance;
}

/**
* timeStamp
* 
* Returns the time and date in the form of a String
* 
* modified 21 April 2016
* by Paul Zwart
*/

String timeStamp(){ 
  return (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " "
          + (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
}

/**
* writeToFile
* modified 21 April 2016
* by David MacBride
*  
* Saves a String to specified file
* Will create a header on the first use of a file (needs to be modified)
* 
* Note:
* * SD card attached to SPI bus as follows:
* ** UNO:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
*  and pin #10 (SS) must be an output
* ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
*  and pin #52 (SS) must be an output
* ** Leonardo: Connect to hardware SPI via the ICSP header
**/
void writeToFile(String fileNameString, String time, String range){
  char fileName[100]; // Or something long enough to hold the longest file name you will ever use.
   fileNameString.toCharArray(fileName, sizeof(fileName));
  boolean firstUse = false;
   
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
    if(firstUse){//create information in this header
      myFile.println("******************** LOG ********************");
      myFile.print("This log was created ");
      myFile.print(now.year(), DEC);
      myFile.print('/');
      myFile.print(now.month(), DEC);
      myFile.print('/');
      myFile.print(now.day(), DEC);
      myFile.print(' ');
      myFile.print(now.hour(), DEC);
      myFile.print(':');
      myFile.print(now.minute(), DEC);
      myFile.print(':');
      myFile.println(now.second(), DEC);
      myFile.println("Time Stamp            Level");
    }
    myFile.print(time);
    myFile.print("     ");
    myFile.print(range);
    myFile.println("mm");
    
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileNameString);
  }
}

