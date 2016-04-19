#include <SPI.h>
#include <SD.h>

File rangeLog;

const int rangeIn = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  double inch = analogRead(rangeIn) * 0.1981 - 1.9613;
  Serial.println( (String)inch + " inches");
  delay(250);
}
