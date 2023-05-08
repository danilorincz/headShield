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
int mode = 1;

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
  beeper.playStartup();
  delay(100);

  //* INITIALIZE STARTING VALUES
  lamp.level = 0;
  fan.level = 2;
  audio.state = LOW;

  //* SERVICE MODE
  while (touchLeft.readAtTheMoment() && touchRight.readAtTheMoment())
  {
    if (serviceModeTimer.timeElapsedMillis())
    {
      mode = 0;
      break;
    }
  }
}

int scanMode()
{
  int newMode;
  if (IR.scan() && visor.scan())
    newMode = 1;
  else if (IR.scan() && !visor.scan())
    newMode = 2;
  else if (!IR.scan() && visor.scan())
    newMode = 3;
  else if (!IR.scan() && !visor.scan())
    newMode = 4;
  return newMode;
}

void modeSelector()
{
  int newMode = scanMode();

  if (newMode != mode)
  {
    mode = newMode;
    switch (mode)
    {
    case 1: //? NORMAL
      beeper.playVisorDown();

      fan.setLevel(fan.level);
      lamp.setLevel(lamp.level);
      audio.turn(audio.state);
      break;

    case 2: //? VISOR OFF
      beeper.playVisorUp();
      lamp.setLevel(lamp.level);
      fan.temporaryOff();
      audio.temporaryOff();
      break;

    case 3: //? IR OFF
      beeper.playVisorUp();

      fan.temporaryOff();
      lamp.temporaryOff();
      audio.temporaryOff();
      break;

    case 4: //? ALL OFF
      beeper.playVisorUp();

      fan.temporaryOff();
      lamp.temporaryOff();
      audio.temporaryOff();
      break;
    }
  }
}

void serveTouch()
{
  if (touchLeft.singleTap()) //! LAMP INPUT
  {
    lamp.toggle();
    if (lamp.level == 0)
      beeper.playLampOff();
    else
      beeper.playLampOn();
  }

  if (touchRight.singleTap()) //! FAN INPUT
  {
    fan.toggle(1, 3);

    switch (fan.level)
    {
    case 1:
      beeper.playFanSpeedDown();
      break;
    case 2:
    case 3:
      beeper.playFanSpeedUp();
      break;
    }
  }

  if (touchRight.longTap()) //! AUDIO INPUT
  {
    audio.toggle();

    if (audio.state)
      beeper.playVisorUp();
    else
      beeper.playVisorDown();
  }
}
bool multiTouch()
{
  if (touchLeft.readAtTheMoment() && touchRight.readAtTheMoment())
    return true;
  else
    return false;
}
void loop()
{
  if (multiTouch())
  {
    while (multiTouch())
    {
      if (serviceModeTimer.timeElapsedMillis())
      {
        mode = 0;
        break;
      }
    }
  }
  else
    serviceModeTimer.preTime = millis();

  if (mode == 0)
  {
    while (true)
    {
      //* the stuff to do when service mode is activeated
    }
  }
  modeSelector();
  serveTouch();
}
