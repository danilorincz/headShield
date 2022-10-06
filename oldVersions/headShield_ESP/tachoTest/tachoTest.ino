//? LIBRARIES
#include <analogWrite.h>
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\headShield_V4_ESP\fan.h"

//? FAN
const int fanPin = 32;
const int tachoPin = 36;
Fan fan(tachoPin, fanPin);

void setup()
{
  Serial.begin(115200);
  fan.begin();
  delay(100);
  fan.setSpeed(50);
  delay(500);
}

//! LOOP
void loop()
{
  unsigned long tachoValue = fan.getPeriodTime();
  Serial.print("Tacho value: ");
  Serial.println(tachoValue);
  delay(100);
}
