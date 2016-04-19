#include <SPI.h>
#include <SD.h>

File rangeLog;

const int rangeIn = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  rangeLog = SD.open("Range Log.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (rangeLog) {
    Serial.print("Writing to test.txt...");
    rangeLog.println("testing 1, 2, 3.");
    // close the file:
    rangeLog.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  rangeLog = SD.open("test.txt");
  if (rangeLog) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (rangeLog.available()) {
      Serial.write(rangeLog.read());
    }
    // close the file:
    rangeLog.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  double inch = analogRead(rangeIn) * 0.1981 - 1.9613;
  Serial.println( (String)inch + " inches");
  delay(250);
}
