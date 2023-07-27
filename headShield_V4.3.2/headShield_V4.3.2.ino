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
IPAddress local_ip(192, 168, 1, 1);
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
  /*
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);*/
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, local_ip, subnet);
  WiFi.softAP(ssid, password);

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
    fan.off();
    delay(1000);
    piezo.playStartup();
    piezo.playStartup();
    piezo.playStartup();
    delay(1000);
    interpretCommand::recalculateFromNormal();
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
{ /*
  Serial.print("Before change: ");
  Serial.print("   ");
  Serial.println(modifyThis);
*/
  int offset = map(battery.percent, 100, 0, 0, 10);
  modifyThis -= offset;
  /*
  Serial.println("After change: ");
  Serial.print("   ");
  Serial.println(modifyThis);*/
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

  if (normal.inRange(tacho.finalValue, 4, 1))
  {
    serialPrintIf("Normál működés, szűrők fent, ventillátorok jók");
    fanErrorNumber = 1;
  }
  else if (noAir.inRange(tacho.finalValue, 5, 5))
  {
    serialPrintIf("Nincs elég térfogatáram! Ellenőrizze a szűrők állapotát!");
    fanErrorNumber = 0;
  }
  else if (noFilter.inRange(tacho.finalValue, 5,5 ))
  {
    serialPrintIf("Nincs felhelyezve szűrő!");
    fanErrorNumber = 2;
  }
  else if (faultFan1.inRange(tacho.finalValue, 5,5 ))
  {
    serialPrintIf("Az egyik ventillátor leállt!");
    fanErrorNumber = 3;
  }
  else
  {
    serialPrintIf("Lehetséges hogy valami akadályozza a levegő kiáramlását!");
    fanErrorNumber = 4;
  }
  int majority = getMajority(fanErrorNumber);
  if (majority != 1 && majority != -1 && fanErrorNumber != 1)
  {
    piezo.playError();
    for (int i = 0; i < 2; i++)
    {

      digitalWrite(19, HIGH);
      delay(40);
      digitalWrite(19, LOW);
      delay(40);
    }
  }
}

std::vector<int> values;

int getMajority(int newValue)
{
  // Store the new value in the vector
  values.push_back(newValue);

  // Limit the size of the vector to the last 5 values
  if (values.size() > 4)
  {
    values.erase(values.begin());
  }

  // Check if all values are the same
  for (int i = 1; i < values.size(); i++)
  {
    if (values[i] != values[0])
    {
      // If any of the values are different, we return -1
      return -1;
    }
  }

  // If we reached this point, it means all values are the same
  return values[0];
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

  using namespace interpreter;

  if (Serial.available())
    command = interpreter::getCommand();
  toggleSerial.refresh(command);
  printTachoValue.refresh(command);
  printPeriferial.refresh(command);
  printLimits.refresh(command);
  printBareLimits.refresh(command);
  printBattery.refresh(command);
  analyseNoAir.refresh(command);
  analyseNormal.refresh(command);
  analysenoFilter.refresh(command);
  analysefaultFan1.refresh(command);
  clearLimits.refresh(command);
  doRecalculation.refresh(command);

  sensorConnectRequest();
  sensorDisconnectRequest();
  sensorReconnectingRequest();
}
