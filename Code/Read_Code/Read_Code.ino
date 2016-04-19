/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11, true); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.println("Coms Started");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
}

void loop() // run over and over
{
  
  Serial.println("distance: " + getRange() + "mm");
}

/**
* getRange
* modified 18 April 2016
* by David MacBride
* 
* Construncts and returns the distance from the Ultrasontic Range Sensor (mySerial) in the form of a String
* Will return "FAIL" in the case of an error (this should be fixed in the near future)
**/
String getRange(){
  String distance = "";
  while(true){
    if (mySerial.available()){
      if (mySerial.read()== 'R'){
        for(int i = 0; i<4; i++){
          while(!mySerial.available()){};
          char nextChar = (char)mySerial.read();
          if(nextChar != 'R'){
            distance += nextChar;
          }else{
            return "FAIL";
          }
          //Serial.println(distance);
        }
        break;
      }
    }
  }
  return distance;
}
