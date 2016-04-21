#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>

SoftwareSerial sensorSerial(6, 7, true); // RX, TX
RTC_DS1307 RTC;
DateTime now;

const int rangeIn = 0;
int lastRunMinute = 0;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  sensorSerial.begin(9600);
  
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
  
  /*Serial.print(now.year(), DEC);
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
  Serial.println();*/

  Serial.println(timeStamp());
  Serial.println("distance: " + getRange() + "mm");
  
  //int x=now.minute();
  if((now.minute()%2)==0){
    if(lastRunMinute != now.minute()){
      Serial.println("Running");
      lastRunMinute = now.minute();
    }
  }
  //x==now.minute()&&
  delay(3000);
  
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

String timeStamp(){
  //Serial.print(now.year(), DEC);
  //Serial.print('/');
  //Serial.print(now.month(), DEC);
  //Serial.print('/');
  //Serial.print(now.day(), DEC);
  //Serial.print(' ');
  //Serial.print(now.hour(), DEC);
  //Serial.print(':');
  //Serial.print(now.minute(), DEC);
  //Serial.print(':');
  //Serial.println(now.second(), DEC);
  //Serial.println("distance: " + getRange() + "mm");
  //Serial.println();  
  return (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " "
          + (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
}


