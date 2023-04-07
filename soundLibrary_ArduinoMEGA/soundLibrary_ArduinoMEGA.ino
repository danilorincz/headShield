#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\beeper.h"
#define piezoPin 23
#define channel 8
Beeper myBeeper(piezoPin, channel);

void setup()
{
  Serial.begin(115200);
  myBeeper.begin();
  myBeeper.setSoundState(true);
  myBeeper.setSpeed(75);
}

void loop()
{

  myBeeper.playTouchTone();
  delay(2000);
  myBeeper.playVisorUpTone();
  delay(2000);
  myBeeper.playVisorDownTone();
  delay(2000);
  myBeeper.playLampOnTone();
  delay(2000);
  myBeeper.playLampOffTone();
  delay(2000);
  myBeeper.playBatteryLowTone();//
  delay(2000);
  myBeeper.playBatteryFullTone();
  delay(2000);
  myBeeper.playFanOnTone();
  delay(2000);
  myBeeper.playFanOffTone();
  delay(2000);
  myBeeper.playFanSpeedUpTone();
  delay(2000);
  myBeeper.playFanSpeedDownTone();
  delay(2000);
  myBeeper.playErrorTone();
  delay(2000);
  myBeeper.playSuccessTone();
  delay(2000);
  myBeeper.playStartupTone();
  delay(2000);
  myBeeper.playShutdownTone();
  delay(4000);
}
