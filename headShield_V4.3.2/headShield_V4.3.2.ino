//? SETTINGS
bool serialEnabled = false;
bool soundEnabled = true;

//? DOWNLOADED LIBRARIEs
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <analogWrite.h>
#include <pitches.h>
#include <Tone32.h>
#include "Wire.h"
#include "DFRobot_BME280.h"
#include <DFRobot_ENS160.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <nvs_flash.h>

//? CUSTOM LIBRARIEs
#include "hardware\Audio.h"
#include "hardware\Device.h"
#include "hardware\Fan.h"
#include "hardware\Lamp.h"
#include "hardware\Battery.h"
#include "hardware\Piezo.h"
#include "hardware\reed.h"
#include "hardware\touchInput.h"
#include "hardware\infraredSensor.h"
#include "hardware\tacho.h"

#include "Timer.h"
#include "movingAverage.h"
#include "Interpreter.h"
#include "FunctionRunner.h"
#include "stat_data_struct.h"
#include "sensor_data_struct.h"
#include <algorithm>
#include "Webpage.h"

#include "onTimeTracker.h"

//? DATA STORAGE
Preferences data;

//? WIFI
const char *ssid = "headShield";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

//? SENSOR
SensorData perkData;
#define SEA_LEVEL_PRESSURE 1015.0f
DFRobot_ENS160_I2C ENS160(&Wire, 0x53);
DFRobot_BME280_IIC BME280(&Wire, 0x76);
bool connectCommand = false;
bool sensorConnected = false;

//? FAN
const int fanPin = 5;
Fan fan(fanPin);

unsigned long autoUpdateFilterTime = 10 * 60 * 1000;
unsigned long saveMaxPeriodTime = 4 * 60 * 1000;
OnTimeTracker filterTracker(saveMaxPeriodTime);
unsigned long previousOnTime = 0;
String onTimeString = "";

//? POWER LED
const int LEDPin = 19;
LED lamp(LEDPin);

//? TACHOMETER
const int tachometerPin = 39;
Tachometer tacho(tachometerPin, 400);

//? BATTERY
const int batteryPin = 32;
Battery battery(batteryPin, 300);

//? HEAD SENSOR
const int infraredPin = 35;
InfraredSensor headSensor(infraredPin, 30);

//? VISOR
const int reedSwitchPin = 18;
ReedSwitch visor(reedSwitchPin, 40);

//? TOUCH INPUT
const int touchRightPin = 33;
const int touchLeftPin = 15;
Touch touchRight(touchRightPin, 0, 24);
Touch touchLeft(touchLeftPin, 0, 50);

//? PIEZO
const int piezoPin = 23;
const int piezoChannel = 8;
Piezo piezo(piezoPin, piezoChannel);

//? AUDIO
const int audioEnPin = 16;
Audio audio(audioEnPin);

//? GLOBAL
FanCondition normal;
cond::conditionNumber fanErrorNumber;

#include "basic.h"
#include "webInterface.h"
#include "interpreterCommands.h"
#include "sensorConnection.h"

void setup()
{
  //* BEGIN
  Serial.begin(115200);
  fan.begin();
  lamp.begin();
  audio.begin();
  piezo.begin(soundEnabled);
  visor.begin();
  battery.begin();
  tacho.begin();
  filterTracker.begin();
  //* RETRIEVE DATA
  restore(normal, data, "normal");

  //* WIFI
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, local_ip, subnet);
  WiFi.softAP(ssid, password);
  serverOn();
  server.begin();

  //* SENSOR
  BME280.reset();
  connectCommand = true;
  Timer connection(3000);
  while (!connection.timeElapsedMillis())
  {
    if (sensorConnectRequest())
      break;
  }

  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  ENS160.setTempAndHum(25.0, 50.0);

  //* start up sound
  piezo.playStartup();
}

bool multiTouch()
{
  bool returnValue = false;
  const unsigned int timeMax = 2000;
  const unsigned int timeMin = 1000;
  static unsigned long timeWhenStart = 0;
  static bool timeStarted = false;

  if (touchLeft.getDigital() && touchRight.getDigital())
  {
    if (!timeStarted)
    {
      timeWhenStart = millis();
      timeStarted = true;
    }

    if (millis() - timeWhenStart > timeMin)
    {
      timeStarted = false;
      returnValue = true;
    }
  }
  else
  {
    timeWhenStart = 0;
    timeStarted = false;
  }

  return returnValue;
}
void touchInputHandler()
{
  if (touchLeft.longTap() && !touchRight.getDigital()) //? FAN TOGGLE
  {
    fan.toggle();
    if (fan.active())
      piezo.playFanOn();
    else
      piezo.playFanOff();
  }

  if (touchRight.singleTap() && !touchLeft.getDigital()) //? LED CONTROL
  {
    lamp.toggleBetween(0, 3);

    if (lamp.level == 0)
      piezo.playLampOff();
    else
      piezo.playLampOn();
  }

  if (touchRight.longTap() && !touchLeft.getDigital()) //? AUDIO CONTROL
  {
    audio.toggle();

    if (audio.state)
      piezo.playVisorUp();
    else
      piezo.playVisorDown();
  }

  if (touchLeft.singleTap() && !touchRight.getDigital()) //? EMPTY
  {
  }
  if (multiTouch()) //? EMPTY
  {
  }
}

void updateSensor()
{
  perkData.temp = BME280.getTemperature();
  perkData.press = BME280.getPressure();
  perkData.humi = BME280.getHumidity();
  perkData.status = ENS160.getENS160Status();
  perkData.AQI = ENS160.getAQI();
  perkData.TVOC = ENS160.getTVOC();
  perkData.ECO2 = ENS160.getECO2();
}

bool fanTurnedOnLongEnough()
{
}

void updateTacho()
{
  if (fan.active())
  {
    tacho.getAverage();
  }

  accountBattery(tacho.finalValue);
}
void accountBattery(int &modifyThis)
{
  int offset = map(battery.percent, 100, 0, 0, 10);
  modifyThis -= offset;
}
void updateBattery()
{
  battery.getPercent();
  if (battery.percent > 100)
    battery.percent = 100;
}
void updateHeadSensor()
{
  headSensor.scan();
}
void updateVisor()
{
  visor.scan();
}

void parseAndAction_tacho()
{
  if (!fan.active())
  {
    fanErrorNumber = cond::off;
    return;
  }

  int value = tacho.finalValue;

  if (normal.inRange(value, 15, 12)) //* NORMAL
    fanErrorNumber = cond::normal;
  else if (normal.getMax() < value && value < normal.getMax() + 100) //* OVER NORMAL
    fanErrorNumber = cond::overNormal;
  else if (normal.getMin() - 100 < value && value < normal.getMin()) //* UNDER NORMAL
    fanErrorNumber = cond::underNormal;
  else
    fanErrorNumber = cond::other; //* UNUSUAL

  switch (fanErrorNumber)
  {
  case cond::normal:
    serialPrintIf("NORMAL");
    break;
  case cond::underNormal:
    serialPrintIf("CHECK FILTERS OR FANS");
    break;
  case cond::overNormal:
    serialPrintIf("NOT ENOUGH AIRFLOW");
    break;
  case cond::other:
    serialPrintIf("AIRFLOW SYSTEM MALFUNCTION");
    break;
  }

  if (tacho.warning)
  {
    Serial.println("Warning cycle is turned ON");
  }
  else
  {
    Serial.println("Warning cycle is turned OFF");
  }
/*
  if (fan.getCurrentSessionOn() < 5000)
    return;

  static Timer signalingTimer(3000);
  if (fanErrorNumber != cond::normal)
  {
    if (signalingTimer.timeElapsedMillis())
    {
      piezo.playFanError();
    }
  }*/


}

void parseAndAction_battery()
{
}
void parseAndAction_headSensor()
{
  static bool onDone = false;
  static bool offDone = false;

  switch (headSensor.state)
  {
  case 1:
    if (!onDone)
    {
      piezo.playHelmetPutOn();
      serialPrintIf("HEAD SENSOR ON");
      onDone = true;
      offDone = false;

      if (visor.state)
        fan.on();
    }
    break;
  case 0:
    if (!offDone)
    {
      piezo.playHelmetTakenOff();
      serialPrintIf("HEAD SENSOR OFF");
      offDone = true;
      onDone = false;
      fan.off();
      lamp.off(); //! CHECK

      filterTracker.save();
    }
    break;
  }
}
void parseAndAction_visor()
{
  static bool onDone = false;
  static bool offDone = false;

  switch (visor.state)
  {
  case 1:
    if (!onDone)
    {
      piezo.playVisorFoldedDown();
      serialPrintIf("VISOR ON");
      onDone = true;
      offDone = false;

      fan.on();
    }
    break;
  case 0:
    if (!offDone)
    {
      piezo.playVisorFoldedUp();
      serialPrintIf("VISOR OFF");
      offDone = true;
      onDone = false;

      fan.off();
    }
    break;
  }
}

void updateFilterTracker()
{
  filterTracker.update(fan.state);
  onTimeString = millisToTimeString(filterTracker.get_timeOn());
}

FunctionRunner tachoRunner(parseAndAction_tacho, 1000);
FunctionRunner batteryRunner(parseAndAction_battery, 4000);
FunctionRunner visorRunner(parseAndAction_visor, 100);
FunctionRunner headSensorRunner(parseAndAction_headSensor, 200);
FunctionRunner readSensorRunner(updateSensor, 200);
FunctionRunner readFilterTrack(updateFilterTracker, 1000);

void loop()
{

  readFilterTrack.takeAction();

  if (filterTracker.get_timeOn() - previousOnTime > autoUpdateFilterTime)
  {
    filterTracker.save();
    previousOnTime = filterTracker.get_timeOn();
  }

  server.handleClient();
  touchInputHandler();

  updateTacho();
  updateBattery();
  updateHeadSensor();
  updateVisor();

  tachoRunner.takeAction();
  batteryRunner.takeAction();
  visorRunner.takeAction();
  headSensorRunner.takeAction();
  readSensorRunner.takeAction();
  sensorConnectRequest();
  sensorDisconnectRequest();
  sensorReconnectingRequest();

  using namespace interpreter;

  if (Serial.available())
    command = interpreter::getCommand();

  toggleSerial.refresh(command);
  printTachoValue.refresh(command);
  printPeriferial.refresh(command);
  printLimits.refresh(command);

  printSensorValues.refresh(command);
  analyse_normal.refresh(command);
  clearLimits.refresh(command);

  printFilterTime.refresh(command);
  toggleFan.refresh(command);
  printMemoryWear.refresh(command);
}
