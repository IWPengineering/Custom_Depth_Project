#include <RTClib.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
RTC_DS1307 RTC;
void setup () {
  Serial.begin(57600);
  Wire.begin();
  RTC.begin();
  
if (! RTC.isrunning()) {
  Serial.println("RTC is NOT running!");
  // following line sets the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}
void loop () {
  DateTime now = RTC.now();
  
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

  int x=now.minute();
  while(x!=now.minute()&&(now.minute()%5)==0);
    
  //Serial.println();
  //Serial.print(" since 1970 = ");
  //Serial.print(now.unixtime());
  //Serial.print("s = ");
  //Serial.print(now.unixtime() / 86400L);
  //Serial.println("d");
 
  
  Serial.println();
  delay(3000);
}
