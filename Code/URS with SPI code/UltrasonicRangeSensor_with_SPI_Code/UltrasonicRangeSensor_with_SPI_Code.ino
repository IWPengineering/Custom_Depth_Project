#include <Adafruit_GFX.h>
#include <Adafruit_HX8357.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <Wire.h> // Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#define STORAGE_INTERVAL 1 //time in minutes
#define SENSOR_SERIAL_RX 5 //pin for sensor
#define SENSOR_SERIAL_TX 6 //pin (unused)
#define MAX_FILE_NAME_LENGTH 13 //max length of total file name that Arduino allowes (format 8.3)

// These are 'flexible' lines that can be changed
#define TFT_CS 2
#define TFT_DC 9
#define TFT_RST 8 // RST can be set to -1 if you tie it to Arduino's reset

// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
#define SD_CHIP_SELECT 10

/*
 * Note:
 * See line 68 for instructions on manually reseting the time and date on the dataloging shield.
 * 
 * The Circuit:
 * Ultrasonic Range Sensor: Pin 5 - Arduino Pin 6 (SoftSerial also requires a TX pin, we are using 7)
 * Adafruit Data Logger Shield: MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
 *  and pin #10 (SS) must be an output
 */
 
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

SoftwareSerial sensorSerial(SENSOR_SERIAL_RX, SENSOR_SERIAL_TX, true); // RX, TX
File dataFile;
RTC_DS1307 RTC;
DateTime now;
const int rangeIn = 0;
int lastSaveMinute = 0;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Serial.println("HX8357D Test!"); 

  

  /*  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(HX8357_RDPOWMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDCOLMOD);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDIM);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDSDR);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); */

  
  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }*/
  sensorSerial.begin(9600);
  
  
  //initialize SD card
  pinMode(SS, OUTPUT);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work.
  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  Wire.begin();
  RTC.begin();

  /* To set the time on the data logging sheild to the time on your computer,
   * comment out the if and upload, than uncoment it and upload again. It should 
   * automatically set the time for the first use
  */
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  tft.begin(HX8357D);
  tft.fillScreen(HX8357_BLACK);
  tft.setRotation(3);
}
void loop() {
  // put your main code here, to run repeatedly:
  now = RTC.now();

  tft.fillScreen(HX8357_BLACK);
  tft.setCursor(50, 0);
  tft.setTextColor(HX8357_WHITE);
  tft.setTextSize(3);
  tft.println(timeStamp());
  tft.setCursor(50, 30);
  tft.println("Distance: " + getRange() + "mm");
  /*
   * date functions
   * now.year()
   * now.month()
   * now.day()
   * now.hour()
   * now.minute()
   * now.second()
   * 
   * Serial.println("distance: " + getRange() + "mm");
  */



  if((now.minute() % STORAGE_INTERVAL) == 0){//
    if(lastSaveMinute != now.minute()){
      Serial.println("******Running Periodic Sequence******");
      Serial.println(timeStamp());
      Serial.println("distance: " + getRange() + "mm");
      writeToFile((String)now.year() + "-" + (String)now.month() + ".txt", timeStamp(), getRange());
      // + (String)now.year() + "-" + (String)now.month() + "-" + (String)now.day()
      
      lastSaveMinute = now.minute();
    }
  }
  delay(5000);
  
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
* Name must be shorter than 9 bytes with a a 3 byte extention. Anything
* too large will fail to create or open the file
* 
* Circuit info:
* * SD card attached to SPI bus as follows:
* ** UNO:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
*  and pin #10 (SS) must be an output
* ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
*  and pin #52 (SS) must be an output
* ** Leonardo: Connect to hardware SPI via the ICSP header
**/
void writeToFile(String fileNameString, String timeText, String range){
  char fileName[MAX_FILE_NAME_LENGTH]; // The max file name length alowed by Arduino.
  fileNameString.toCharArray(fileName, sizeof(fileName));
  //Serial.println(fileName);
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
  dataFile = SD.open(fileName, FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (dataFile) {
    Serial.print("Writing to " + fileNameString + "...");
    if(firstUse){//create information in this header
      dataFile.println("******************** LOG ********************");
      dataFile.print("This log was created ");
      dataFile.println(timeText);
      dataFile.println("Time Stamp,\t\tLevel [mm]");
    }
    dataFile.print(timeText);
    dataFile.print(",\t");
    dataFile.println(range);
    //dataFile.println("mm");
    
	// close the file:
    dataFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileNameString);
  }
}

