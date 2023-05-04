// HEADSHIELD_V3.7 PCB_V10
//? SETTINGS
#define SOUND_ACTIVE true
//? LIBRARIES

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <analogWrite.h>
#include <pitches.h>
#include <Tone32.h>
#include "Wire.h"
#include "DFRobot_BME280.h"
#include <DFRobot_ENS160.h>
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\webpage.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\timer.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\fan.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\beeper.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\battery.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\reed.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\powerLed.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\touchInput.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\infraredSensor.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\audioEN.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\sensor_data.h"

//?  SENSOR
DFRobot_ENS160_I2C ENS160(&Wire, 0x53);
DFRobot_BME280_IIC BME280(&Wire, 0x76);

SensorData perkData;

//? FAN
const int fanPin = 5;
const int tachoPin = 17;
Fan fan(fanPin);

//? IR
const int infraredPin = 35;
infraredSensor IR(infraredPin);

//? POWER LED
const int powerLedPin = 19;
powerLED lamp(powerLedPin);

//? TOUCH INPUT
const int touchRightPin = 33;
const int touchLeftPin = 15;
touchInput touchRight(touchRightPin, 30);
touchInput touchLeft(touchLeftPin, 30);

//? REED SWITCH
const int reedSwitchPin = 18;
ReedSwitch visor(reedSwitchPin);

//? PIEZO
const int piezoPin = 23;
const int piezoChannel = 8;
Beeper beeper(piezoPin, piezoChannel);

//? BATTERY
const int batteryPin = 32;
Battery battery(batteryPin);

//? AUDIO
const int audioEnPin = 16;
Audio audio(audioEnPin);

//? TIMERS
Timer serviceModeTimer(3000);

//? GLOBAL
bool serviceMode = false;

void setup()
{
  //* BEGIN
  Serial.begin(115200);
  fan.begin();
  battery.begin();
  beeper.begin(SOUND_ACTIVE);
  visor.begin();
  audio.begin();
  lamp.begin();

  //* BME280
  BME280.reset();
  if (BME280.begin() != DFRobot_BME280_IIC::eStatusOK)
    Serial.println("inside pressure sensor faild");

  //* ENS160
  if (ENS160.begin() != NO_ERR)
  {
    Serial.println("Communication with device failed, please check connection");
    delay(50);
  }
  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  ENS160.setTempAndHum(25.0, 50.0);

  //* BOOT
  beeper.playStartupTone();
  delay(100);

  while (touchLeft.readAtTheMoment() && touchRight.readAtTheMoment())
  {
    if (serviceModeTimer.timeElapsedMillis())
    {
      serviceMode = true;
      break;
    }
  }
  fan.prevLevel = 2;
}

int mode = 0;
int prevMode = 0;

void loop()
{

  if (IR.scan() && visor.scan())
    mode = 1;
  else if (IR.scan() && !visor.scan())
    mode = 2;
  else if (!IR.scan() && visor.scan())
    mode = 3;
  else if (!IR.scan() && !visor.scan())
    mode = 4;

  if (prevMode != mode)
  {

    switch (mode)
    {
    case 1: //? NORMAL
      beeper.playVisorDownTone();
      fan.setLevel(fan.prevLevel);
      lamp.setLevel(lamp.prevLevel);
      audio.off();
      break;
    case 2: //? VISOR OFF
      beeper.playVisorUpTone();

      fan.setLevel(0);
      audio.off();
      break;
    case 3: //? IR OFF
      beeper.playVisorUpTone();
      fan.setLevel(0);
      lamp.setLevel(0);
      audio.off();
      break;
    case 4: //? ALL OFF
      beeper.playVisorUpTone();
      fan.setLevel(0);
      lamp.setLevel(0);
      audio.off();
      break;
    }

    prevMode = mode;
  }

  serveTouch();
}

void serveTouch()
{
  if (touchLeft.singleTap())
  {
    lamp.toggle();
    lamp.prevLevel = lamp.level;
    if (lamp.level == 0)
      beeper.playLampOffTone();
    else
      beeper.playLampOnTone();
  }

  if (touchRight.singleTap())
  {
    if (fan.level < 3)
      fan.level++;
    else
      fan.level = 1;

    fan.setLevel(fan.level);
    fan.prevLevel = fan.level;

    switch (fan.level)
    {
    case 1:
      beeper.playFanSpeedDownTone();
      break;
    case 2:
    case 3:
      beeper.playFanSpeedUpTone();
      break;
    }
  }
  if (touchRight.longTap())
  {

    if (fan.level > 1)
      fan.level--;
    else
      fan.level = 3;

    fan.setLevel(fan.level);
    fan.prevLevel = fan.level;

    audio.toggle();
    audio.prevState = audio.state;
    if (audio.state)
      beeper.playVisorUpTone();
    else
      beeper.playVisorDownTone();
  }
}
