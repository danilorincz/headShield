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

#include "Webpage.h"

//? DATA STORAGE
Preferences data;

//? WIFI
const char *ssid = "headShield";
const char *password = "123456789";
IPAddress local_ip(192, 168, 23, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

//?  SENSOR
SensorData perkData;
#define SEA_LEVEL_PRESSURE 1015.0f
DFRobot_ENS160_I2C ENS160(&Wire, 0x53);
DFRobot_BME280_IIC BME280(&Wire, 0x76);
bool connectCommand = false;
bool sensorConnected = false;

//? FAN
const int fanPin = 5;
Fan fan(fanPin);
FanCondition noAir;
FanCondition normal;
FanCondition noFilter;
FanCondition faultFan1;

//? POWER LED
const int LEDPin = 19;
LED lamp(LEDPin);

//? TACHOMETER
const int tachometerPin = 39;
Tachometer tacho(tachometerPin, 600);
int fanErrorNumber = 0;

//? BATTERY
const int batteryPin = 32;
Battery battery(batteryPin, 300);

//? HEAD SENSOR
const int infraredPin = 35;
InfraredSensor headSensor(infraredPin, 5);

//? VISOR
const int reedSwitchPin = 18;
ReedSwitch visor(reedSwitchPin, 5);

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

  //* RETRIEVE DATA
  restore(normal, data, "normalLimits");
  restore(noAir, data, "noAirLimits");
  restore(noFilter, data, "noFilterLimits");
  restore(faultFan1, data, "faultFan1Limits");

  //* WIFI
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
  serverOn();
  server.begin();

  //* BME280
  BME280.reset();
  connectCommand = true;
  static Timer connection(3000);
  while (!connection.timeElapsedMillis())
  {
    if (sensorConnectRequest())
      break;
  }

  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  ENS160.setTempAndHum(25.0, 50.0);

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
void updateTacho()
{
  if (fan.active())
  {
    tacho.getAverage();
  }
}
void updateBattery()
{
  battery.getPercent();
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
    return;

  if (normal.inRange(tacho.finalValue, 3))
  {
    serialPrintIf("Normál működés, szűrők fent, ventillátorok jók");
    fanErrorNumber = 1;
  }
  else if (noAir.inRange(tacho.finalValue, 5))
  {
    serialPrintIf("Nincs elég térfogatáram! Ellenőrizze a szűrők állapotát!");
    fanErrorNumber = 0;
  }
  else if (noFilter.inRange(tacho.finalValue, 5))
  {
    serialPrintIf("Nincs felhelyezve szűrő!");
    fanErrorNumber = 2;
  }
  else if (faultFan1.inRange(tacho.finalValue, 5))
  {
    serialPrintIf("Az egyik ventillátor leállt!");
    fanErrorNumber = 3;
  }
  else
  {
    serialPrintIf("Lehetséges hogy valami akadályozza a levegő kiáramlását!");
    fanErrorNumber = 4;
  }
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
      if (visor.state)
        fan.on();
      onDone = true;
      offDone = false;
    }
    break;
  case 0:
    if (!offDone)
    {
      piezo.playHelmetTakenOff();
      serialPrintIf("HEAD SENSOR OFF");
      offDone = true;
      onDone = false;
      if (!visor.state)
        fan.off();
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
      if (headSensor.state)
        fan.on();
      piezo.playVisorFoldedDown();
      serialPrintIf("VISOR ON");
      onDone = true;
      offDone = false;
    }
    break;
  case 0:
    if (!offDone)
    {
      piezo.playVisorFoldedUp();
      fan.off();
      serialPrintIf("VISOR OFF");
      offDone = true;
      onDone = false;
    }
    break;
  }
}
FunctionRunner tachoRunner(parseAndAction_tacho, 3000);
FunctionRunner batteryRunner(parseAndAction_battery, 4000);
FunctionRunner visorRunner(parseAndAction_visor, 100);
FunctionRunner headSensorRunner(parseAndAction_headSensor, 200);
FunctionRunner readSensorRunner(updateSensor, 200);
void loop()
{
  server.handleClient();

  updateTacho();
  updateBattery();
  updateHeadSensor();
  updateVisor();
  tachoRunner.takeAction();
  batteryRunner.takeAction();
  visorRunner.takeAction();
  headSensorRunner.takeAction();
  readSensorRunner.takeAction();

  touchInputHandler();

  if (Serial.available())
  {
    interpreter::command = interpreter::getCommand();
  }

  printTachoValue.refresh(interpreter::command);
  printPeriferial.refresh(interpreter::command);
  printLimits.refresh(interpreter::command);

  analyseNoAir.refresh(interpreter::command);
  analyseNormal.refresh(interpreter::command);
  analysenoFilter.refresh(interpreter::command);
  analysefaultFan1.refresh(interpreter::command);
  clearLimits.refresh(interpreter::command);
  toggleSerial.refresh(interpreter::command);
  doRecalculation.refresh(interpreter::command);

  sensorConnectRequest();
  sensorDisconnectRequest();
  sensorReconnectingRequest();
}
