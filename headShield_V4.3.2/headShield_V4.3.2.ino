/* jól működő:
Normal
  MAX: 3621
  MIN: 3617
  AVE: 3617

        int P_4 = 39;
        int P_3 = 29;
        int P_2 = 26;
        int P_1 = 18;

        int base;

        int N_1 = 30;
        int N_2 = 38;
        int N_3 = 65;
        int N_4 = 69;
        int N_5 = 93;
        int N_6 = 95;

        StatData newLimits = tachoAnalysis(400, 20);

        base = newLimits.average;

        noAir.setMax(base + P_4);
        noAir.setMin(base + P_3);
        noAirSingle.setMax(base + P_2);
        noAirSingle.setMin(base + P_1);

        normal.setLimit(newLimits);

        noFilterSingle.setMax(base - N_1);
        noFilterSingle.setMin(base - N_2);
        noFilter.setMax(base - N_3);
        noFilter.setMin(base - N_4);
        faultFan1.setMax(base - N_5);
        faultFan1.setMin(base - N_6);

*/

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

//? GLOBAL
std::vector<int> values;
cond::conditionNumber fanErrorNumber;

FanCondition noAir;
FanCondition noAirSingle;

FanCondition normal;

FanCondition noFilterSingle;
FanCondition noFilter;
FanCondition faultFan1;

bool calibrateCommand = false;

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
  Serial.println("SHIT");
  //* RETRIEVE DATA
  restore(noAir, data, "noAir");
  restore(noAirSingle, data, "noAirSingle");

  restore(normal, data, "normal");

  restore(noFilter, data, "noFilter");
  restore(noFilterSingle, data, "noFilterSingle");
  restore(faultFan1, data, "faultFan1");

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

  fan.on();
  delay(1000);
  tacho.updateThreshold();
  fan.off();

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
  if (multiTouch()) //? CALIBRATE
  {
    calibrateCommand = true;
    piezo.playStartup();
  }
}
bool calibrate()
{
  if (!fan.state)
    fan.on();

  if (fan.getOnTime() > 4000)
  {
    piezo.playStartup();
    interpretCommand::recalculateFromNormal();
    calibrateCommand = false;
    return true;
  }
  else
  {
    return false;
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
    values.clear();
    return;
  }

  int value = tacho.finalValue;

  FanCondition copy_noAir = noAir;
  FanCondition copy_noAirSingle = noAirSingle;
  FanCondition copy_normal = normal;
  FanCondition copy_noFilterSingle = noFilterSingle;
  FanCondition copy_noFilter = noFilter;
  FanCondition copy_faultFan1 = faultFan1;

  copy_faultFan1.setMax(copy_faultFan1.getMax() + 8);
  copy_faultFan1.setMin(copy_faultFan1.getMin() - 10);

  copy_noFilterSingle.setMax(copy_noFilterSingle.getMax() + 12);
  copy_noFilterSingle.setMin(copy_noFilterSingle.getMin() - 10);

  copy_noFilter.setMax(copy_noFilter.getMax() + 15);
  copy_noFilter.setMin(copy_noFilter.getMin() - 10);

  copy_noAir.setMax(copy_noAir.getMax() + 8);
  copy_noAir.setMin(copy_noAir.getMin() - 5);

  copy_normal.setMin(copy_normal.getMin() - 15);
  copy_normal.setMax(copy_normal.getMax() + 8);

  copy_noAirSingle.setMin(copy_normal.getMax() + 1);
  copy_noAirSingle.setMax(copy_noAirSingle.getMin() + copy_noAirSingle.getRangeSize());

  copy_noAir.setMin(copy_noAirSingle.getMax() + 1);
  copy_noAir.setMax(copy_noAir.getMin() + 80);

  if (copy_normal.inRange(value, 0, 0)) //* NORMAL
  {
    serialPrintIf("Normál működés, szűrők fent, ventillátorok jók");
    fanErrorNumber = cond::normal;
  }

  else if (copy_noAirSingle.inRange(value, 0, 0)) //* NO AIR SINGLE
  {
    serialPrintIf("Az egyik szűrőtől nem jön levegő, ezért nincs elég térfogatáram!");
    fanErrorNumber = cond::noAirSingle;
  }

  else if (copy_noAir.inRange(value, 0, 0)) //* NO AIR
  {
    serialPrintIf("Nem jön levegő a szűrőktől, ezért nincs elég térfogatáram!");
    fanErrorNumber = cond::noAir;
  }

  else if (copy_noFilter.inRange(value, 0, 0)) //* NO FILTER
  {
    serialPrintIf("Nincs felhelyezve szűrő, vagy valami akadályozza a levegő kiáramlását!");
    fanErrorNumber = cond::noFilter;
  }
  else if (copy_noFilterSingle.inRange(value, 0, 0)) //* NO FILTER SINGLE
  {
    serialPrintIf("Az egyik szűrő nincs felhelyezve, vagy valami akadályozza a levegő kiáramlását!");
    fanErrorNumber = cond::noFilterSingle;
  }

  else if (copy_faultFan1.inRange(value, 0, 0)) //* FAULT FAN 1
  {
    serialPrintIf("Az egyik ventillátor leállt!");
    fanErrorNumber = cond::faultFan1;
  }
  else if (copy_faultFan1.getMin() - 100 < value || value < copy_noAir.getMax() + 100)
  {
    serialPrintIf("Lehetséges hogy valami akadályozza a levegő kiáramlását!");
    fanErrorNumber = cond::other;
  }
  else
  {
    serialPrintIf("Nincs elég mérés, vagy a térfogatárammérő meghibásodott!");
  }

  Serial.print("Tacho final: ");
  Serial.println(value);

  int majority = getMajority(fanErrorNumber);

  if (majority == -1)
    return;

  static Timer signalingTimer(3000);
  if (!isInValues(cond::normal) && fanErrorNumber != cond::normal)
  {
    if (signalingTimer.timeElapsedMillis())
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
}

int getMajority(int newValue)
{
  // Store the new value in the vector
  values.push_back(newValue);

  // Limit the size of the vector to the last 5 values
  if (values.size() > 4)
  {
    values.erase(values.begin());
  }
  else
  {
    return -1;
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

bool isInValues(int input)
{
  // Use std::find to search for the input value in the vector
  auto it = std::find(values.begin(), values.end(), input);

  // If the iterator returned by std::find is not equal to the end of the vector,
  // it means the value was found in the vector
  return it != values.end();
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
FunctionRunner tachoRunner(parseAndAction_tacho, 1000);
FunctionRunner batteryRunner(parseAndAction_battery, 4000);
FunctionRunner visorRunner(parseAndAction_visor, 100);
FunctionRunner headSensorRunner(parseAndAction_headSensor, 200);
FunctionRunner readSensorRunner(updateSensor, 200);

void loop()
{
  //Serial.println("LOOP");
  server.handleClient();
  //Serial.println(analogRead(tachometerPin));

  updateTacho();
  updateBattery();
  updateHeadSensor();
  updateVisor();

  //Serial.println("1");
  tachoRunner.takeAction();
  //Serial.println("2");
  batteryRunner.takeAction();
  //Serial.println("3");
  visorRunner.takeAction();
  //Serial.println("4");
  headSensorRunner.takeAction();
  //Serial.println("5");
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

  analyse_noAir.refresh(command);
  analyse_noAirSingle.refresh(command);

  analyse_normal.refresh(command);

  analyse_noFilterSingle.refresh(command);
  analyse_noFilter.refresh(command);
  analyse_faultFan1.refresh(command);

  clearLimits.refresh(command);
  doRecalculation.refresh(command);

  sensorConnectRequest();
  sensorDisconnectRequest();
  sensorReconnectingRequest();

  if (calibrateCommand)
  {
    if (calibrate())
    {
      Serial.println("Sikeres kalibrálás");
    }
    else
    {
      Serial.println("Még nem megy elég ideje a ventillátor");
    }
  }
}
