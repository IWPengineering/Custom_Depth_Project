#include <RTClib.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
RTC_DS1307 RTC;

//DateTime now = RTC.now();


void setup () {
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  DateTime now = RTC.now();
  Serial.println("RTC is NOT running!");
  // following line sets the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));
  Serial.print("The time has been changed: ");
  
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
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.println("Do not restard Arduino (this will reset the time again to the compilation time)");
  Serial.println("Reprogram the Arduino with the proper code");
  Serial.println();
  Serial.println("Additional information:");
  
  Serial.print(" since 1970 = ");
  Serial.print(now.unixtime());
  Serial.print("s = ");
  Serial.print(now.unixtime() / 86400L);
  Serial.println("d");
 
  Serial.println();
  
}

void loop () {
  delay(1000);
}
