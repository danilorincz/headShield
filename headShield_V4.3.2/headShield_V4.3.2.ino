//? SETTINGS
#define SOUND_ACTIVE true

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

//? FAN CONDITIONS
FanCondition noAir;
FanCondition normal;    // N
FanCondition noFilter;  // I
FanCondition faultFan1; // Q

//? GLOBAL
bool serialEnabled = false;

void putData(StatData putThis, Preferences &here, String mapName)
{
  here.begin(mapName.c_str(), false);
  here.putInt("max", putThis.max);
  here.putInt("min", putThis.min);
  here.end();
}
#include "webInterface.h"
void setup()
{
  //* BEGIN
  Serial.begin(115200);
  fan.begin();
  lamp.begin();
  audio.begin();
  piezo.begin(SOUND_ACTIVE);
  visor.begin();
  battery.begin();
  tacho.begin();

  //* RETRIEVE DATA
  data.begin("normalLimits", false);

  StatData loadedNormal;
  loadedNormal.max = data.getInt("max", -1);
  loadedNormal.min = data.getInt("min", -1);
  normal.setLimit(loadedNormal);

  data.end();

  data.begin("noAirLimits");

  StatData loaded_noAir;
  loaded_noAir.max = data.getInt("max", -1);
  loaded_noAir.min = data.getInt("min", -1);
  noAir.setLimit(loaded_noAir);
  data.end();

  data.begin("noFilterLimits", false);

  StatData loadedNoFilter;
  loadedNoFilter.max = data.getInt("max", -1);
  loadedNoFilter.min = data.getInt("min", -1);

  noFilter.setLimit(loadedNoFilter);
  data.end();

  data.begin("faultFan1Limits", false);
  StatData loadedFaultFan1;
  loadedFaultFan1.max = data.getInt("max", -1);
  loadedFaultFan1.min = data.getInt("min", -1);

  faultFan1.setLimit(loadedFaultFan1);
  data.end();

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

//* HANDLERS

//* BASIC
void serailPrintIf(String inputText)
{
  if (serialEnabled)
    Serial.println(inputText);
}

//* TOUCH
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

//* SENSOR DATA
bool BME280Connected()
{
  if (BME280.begin() != DFRobot_BME280_IIC::eStatusOK)
    return false;
  else
    return true;
}
bool ENS160Connected()
{
  if (ENS160.begin() != NO_ERR)
    return false;
  else
    return true;
}
void readSensorData()
{
  static Timer readData(100);
  if (readData.timeElapsedMillis())
  {
    perkData.temp = BME280.getTemperature();
    perkData.press = BME280.getPressure();
    perkData.humi = BME280.getHumidity();

    perkData.status = ENS160.getENS160Status();
    perkData.AQI = ENS160.getAQI();
    perkData.TVOC = ENS160.getTVOC();
    perkData.ECO2 = ENS160.getECO2();
  }
}
bool sensorConnectRequest()
{
  static Timer connectingSensorMax(3000);
  if (connectCommand)
  {
    if (!connectingSensorMax.timeElapsedMillis()) // itt még nézi hogy van-e szenzor
    {
      static bool BME_ok = false;
      static bool ENS_ok = false;
      if (BME280Connected() && !BME_ok)
        BME_ok = true;
      if (ENS160Connected() && !ENS_ok)
        ENS_ok = true;

      if (BME_ok && ENS_ok)
      {
        sensorConnected = true;
        connectCommand = false;
        BME_ok = false;
        ENS_ok = false;
        piezo.playSuccess();
        serailPrintIf("Sensor connected OK");
        return true;
      }
    }
    else // letelt az ido
    {
      connectCommand = false;
    }
  }
  else
  {
    connectingSensorMax.preTime = millis();
  }
  return false;
}
bool isSensorDisconnecting()
{
  if (perkData.AQI == 0 && perkData.TVOC == 252 && perkData.ECO2 == 252)
    return true;
  else
    return false;
}
void sensorDisconnectRequest()
{
  if (sensorConnected)
  {
    if (isSensorDisconnecting())
    {
      sensorConnected = false;
      piezo.playShutdown();
      serailPrintIf("disconnecting sensor");
    }
  }
}
bool isSensorReconnecting()
{
  if (perkData.AQI == 0 && perkData.TVOC == 0 && perkData.ECO2 == 0)
    return true;
  else
    return false;
}
void sensorReconnectingRequest()
{
  if (!sensorConnected)
  {
    if (isSensorReconnecting())
    {
      connectCommand = true;
      serailPrintIf("Reconnecting sensor");
    }
  }
}

//* MEASURE PROTOCOL
void tacho_logMeasureProtocol()
{
  if (!fan.active())
    return;
  static const int measureQuantity = 500;
  static int tachoPrevValue;
  static int monitorCounter = 0;
  static int monitorSessionCounter = 0;
  static unsigned long sessionDuration = 0;
  static unsigned long sessionStartTime = 0;
  static int lastSessionValues[measureQuantity];

  if (monitorCounter == 0)
  {
    sessionStartTime = millis();
    Serial.println("________MONITORING STARTED_________");
    Serial.print("Monitor session: ");
    Serial.print(monitorSessionCounter);
    Serial.println("VALUES START");
    Serial.println(perkData.temp);
    Serial.println(perkData.press);
    Serial.println(perkData.humi);
    Serial.println(perkData.AQI);
    Serial.println(perkData.TVOC);
    Serial.println(perkData.ECO2);

    monitorSessionCounter++;
  }
  lastSessionValues[monitorCounter] = tacho.finalValue;
  monitorCounter++;
  Serial.println(tacho.finalValue);

  if (monitorCounter == measureQuantity)
  {

    piezo.playStartup();
    Serial.println("**********MONITORING ENDED**********");

    monitorCounter = 0;
    Serial.println(" ");
    Serial.println(" ");
    Serial.println(" ");
    Serial.println(" ");
    Serial.println(" ");
    delay(2000);
  }
}

//* TACHO
void updateTacho()
{
  if (fan.active())
  {
    tacho.getAverage();
  }
}
void parseAndAction_tacho() // working with: latestTachoStat.average
{

  if (!fan.active())
    return;

  if (normal.inRange(tacho.finalValue, 3))
  {
    serailPrintIf("Normál működés, szűrők fent, ventillátorok jók");
    fanErrorNumber = 1;
  }
  else if (noAir.inRange(tacho.finalValue, 5))
  {
    serailPrintIf("Nincs elég térfogatáram! Ellenőrizze a szűrők állapotát!");
    fanErrorNumber = 0;
  }
  else if (noFilter.inRange(tacho.finalValue, 5))
  {
    serailPrintIf("Nincs felhelyezve szűrő!");
    fanErrorNumber = 2;
  }
  else if (faultFan1.inRange(tacho.finalValue, 5))
  {
    serailPrintIf("Az egyik ventillátor leállt!");
    fanErrorNumber = 3;
  }
  else
  {
    serailPrintIf("Lehetséges hogy valami akadályozza a levegő kiáramlását!");
    fanErrorNumber = 4;
  }
}

//* BATTERY
void updateBattery()
{
  battery.getPercent();
}
void parseAndAction_battery() // working with: battery.percent
{
}

//* HEAD SENSOR
void updateHeadSensor()
{
  headSensor.scan();
}
void parseAndAction_headSensor() // working with: headSensor.state
{
  static bool onDone = false;
  static bool offDone = false;

  switch (headSensor.state)
  {
  case 1:
    if (!onDone)
    {
      piezo.playHelmetPutOn();
      serailPrintIf("HEAD SENSOR ON");
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
      serailPrintIf("HEAD SENSOR OFF");
      offDone = true;
      onDone = false;
      if (!visor.state)
        fan.off();
    }
    break;
  }
}

//* VISOR
void updateVisor()
{
  visor.scan();
}
void parseAndAction_visor() // working with: visor.state
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
      serailPrintIf("VISOR ON");
      onDone = true;
      offDone = false;
    }
    break;
  case 0:
    if (!offDone)
    {
      piezo.playVisorFoldedUp();
      fan.off();
      serailPrintIf("VISOR OFF");
      offDone = true;
      onDone = false;
    }
    break;
  }
}

//* INTERPRETER
void printPeriferial_inter()
{
  Serial.print("Left touch raw: ");
  Serial.println(touchLeft.getAnalog());

  Serial.print("Right touch raw: ");
  Serial.println(touchRight.getAnalog());

  Serial.print("Visor: ");
  Serial.println(visor.state);

  Serial.print("Head sensor: ");
  Serial.println(headSensor.state);
}
void printTacho_inter()
{
  Serial.println(tacho.finalValue);
}
void printLimits_inter()
{
  Serial.println("noAir");
  Serial.print("  MAX: ");
  Serial.println(noAir.getMax());
  Serial.print("  MIN: ");
  Serial.println(noAir.getMin());

  Serial.println("Normal");
  Serial.print("  MAX: ");
  Serial.println(normal.getMax());
  Serial.print("  MIN: ");
  Serial.println(normal.getMin());

  Serial.println("noFilter");
  Serial.print("  MAX: ");
  Serial.println(noFilter.getMax());
  Serial.print("  MIN: ");
  Serial.println(noFilter.getMin());

  Serial.println("faultFan1");
  Serial.print("  MAX: ");
  Serial.println(faultFan1.getMax());
  Serial.print("  MIN: ");
  Serial.println(faultFan1.getMin());
}

StatData tachoAnalysis(int numberOfMeasurements, int measurementsMultiplier)
{
  fan.on();
  int counter = 0;
  int allTimeMax = 0;
  int allTimeMin = 9999;
  StatData dataNow;
  Statistics statisticsNow(numberOfMeasurements);

  while (true)
  {
    tacho.getAverage();

    if (statisticsNow.addValue(tacho.finalValue))
    {
      Serial.print("new set of value added_");
      Serial.println(counter);
      counter++;
      dataNow = statisticsNow.getStats();
      if (counter > 5) // skip the first three
      {
        if (dataNow.min < allTimeMin)
          allTimeMin = dataNow.min;
        if (dataNow.max > allTimeMax)
          allTimeMax = dataNow.max;
      }
    }
    if (counter > measurementsMultiplier)
    {
      break;
    }
  }
  fan.off();
  return dataNow;
}

void refreshLimits(int condition) // {0 -> no air flow} {1 -> normal}  {2 -> no filter} {3 -> fan fault}
{
  StatData newLimits = tachoAnalysis(300, 15);

  bool writeToFlash = false;
  StatData oldLimitsClone;

  switch (condition)
  {
  case 0:
    oldLimitsClone = noAir.getStatData();
    break;
  case 1:
    oldLimitsClone = normal.getStatData();
    break;
  case 2:
    oldLimitsClone = noFilter.getStatData();
    break;
  case 3:
    oldLimitsClone = faultFan1.getStatData();
    break;
  }

  StatData movingLimits = oldLimitsClone;
  if (newLimits.max > movingLimits.max)
  {
    writeToFlash = true;
    movingLimits.max = newLimits.max;
  }
  if (newLimits.min < movingLimits.min)
  {
    writeToFlash = true;
    movingLimits.min = newLimits.min;
  }

  if (writeToFlash)
  {
    switch (condition)
    {
    case 0:
      newLimits.max += 100;
      noAir.setLimit(newLimits);
      putData(newLimits, data, "noAirLimits");
      break;
    case 1:
      newLimits.max += 5;
      newLimits.min -= 5;
      normal.setLimit(newLimits);
      putData(newLimits, data, "normalLimits");
      break;
    case 2:
      newLimits.max += 5;
      newLimits.min -= 5;
      noFilter.setLimit(newLimits);
      putData(newLimits, data, "noFilterLimits");
      break;
    case 3:
      newLimits.max += 5;
      newLimits.min -= 5;
      faultFan1.setLimit(newLimits);
      putData(newLimits, data, "faultFan1Limits");
      break;
    }
  }

  if (writeToFlash)
  {
    Serial.println("Values changed! (min and max)");

    Serial.print("Prev limits: ");
    Serial.print("   ");
    Serial.println(oldLimitsClone.max);
    Serial.print("   ");
    Serial.println(oldLimitsClone.min);

    Serial.print("New limits: ");
    Serial.print("   ");
    Serial.println(newLimits.max);
    Serial.print("   ");
    Serial.println(newLimits.min);
  }
  else
  {
    Serial.println("Old limits NOT changed to new limits!");

    Serial.print("Old limits: ");
    Serial.print("   ");
    Serial.println(oldLimitsClone.max);
    Serial.print("   ");
    Serial.println(oldLimitsClone.min);

    Serial.print("Results: ");
    Serial.print("   ");
    Serial.println(newLimits.max);
    Serial.print("   ");
    Serial.println(newLimits.min);
  }
}
void refreshNoAirLimit()
{
  refreshLimits(0);
}
void refreshNormalLimit()
{
  refreshLimits(1);
}
void refreshNoFilterLimits()
{
  refreshLimits(2);
}
void refreshFaultFan1Limits()
{
  refreshLimits(3);
}

void clearAllData()
{
  nvs_flash_erase();
  nvs_flash_init();
  /*
  data.begin("normalLimits", false);
  data.clear();
  data.end();
  data.begin("noAirLimits", false);
  data.clear();
  data.end();
  data.begin("noFilterLimits", false);
  data.clear();
  data.end();
  data.begin("faultFan1Limits", false);
  data.clear();
  data.end();*/
  Serial.println("All data cleared!");
}
void toggleSerialEnable()
{
  serialEnabled = !serialEnabled;
}
void recalculateFromNormal()
{
  int base;
  int A = 27;
  int B = 39;
  int C = 56;
  int D = 69;
  StatData newLimits = tachoAnalysis(1000, 20);
  newLimits.max += 5;
  newLimits.min -= 5;
  normal.setLimit(newLimits);
  putData(newLimits, data, "normalLimits");
  base = newLimits.min;

  noAir.setMin(normal.getMax() + 1);
  noAir.setMax(normal.getMax() + 50);
  noFilter.setMax(base - A);
  noFilter.setMin(base - B);
  faultFan1.setMax(base - C);
  faultFan1.setMin(base - D);

  putData(noAir.getStatData(), data, "noAirLimits");
  putData(normal.getStatData(), data, "normalLimits");
  putData(noFilter.getStatData(), data, "noFilterLimits");
  putData(faultFan1.getStatData(), data, "faultFan1Limits");

  Serial.println("_______________");
  Serial.println("No air: ");
  Serial.print("   ");
  Serial.println(noAir.getMax());
  Serial.print("   ");
  Serial.println(noAir.getMin());

  Serial.println("Normal: ");
  Serial.print("   ");
  Serial.println(normal.getMax());
  Serial.print("   ");
  Serial.println(normal.getMin());

  Serial.println("No filter: ");
  Serial.print("   ");
  Serial.println(noFilter.getMax());
  Serial.print("   ");
  Serial.println(noFilter.getMin());

  Serial.println("Fault fan1: ");
  Serial.print("   ");
  Serial.println(faultFan1.getMax());
  Serial.print("   ");
  Serial.println(faultFan1.getMin());

  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
}
void cleanUpLimits()
{
  if (intersect(noAir.limits, normal.limits))
  {
    Serial.println("No air and normal intersecting!");
    int intersectionSize = normal.getMax() - noAir.getMin();
    Serial.print("Interseciton size: ");
    Serial.println(intersectionSize);
    noAir.setMin(normal.getMax() + intersectionSize);
  }
  else
  {
    Serial.println("No intersection between noAir and normal");
    int difference = noAir.getMin() - normal.getMax();
    normal.setMax(normal.getMax() + difference - 1);
    Serial.print("Difference: ");
    Serial.println(difference);
  }
}
FunctionRunner tachoRunner(parseAndAction_tacho, 3000);
FunctionRunner batteryRunner(parseAndAction_battery, 4000);
FunctionRunner visorRunner(parseAndAction_visor, 100);
FunctionRunner headSensorRunner(parseAndAction_headSensor, 200);
FunctionRunner readSensorRunner(readSensorData, 200);

Interpreter printTachoValue("tacho", printTacho_inter);
Interpreter printPeriferial("periferial", printPeriferial_inter);
Interpreter printLimits("limit", printLimits_inter);

Interpreter analyseNoAir("analyse noAir", refreshNoAirLimit);
Interpreter analyseNormal("analyse normal", refreshNormalLimit);
Interpreter analysenoFilter("analyse noFilter", refreshNoFilterLimits);
Interpreter analysefaultFan1("analyse faultFan1", refreshFaultFan1Limits);
Interpreter clearLimits("clear", clearAllData);
Interpreter toggleSerial("enable", toggleSerialEnable);
Interpreter cleanNoAirMin("clean noAir", cleanUpLimits);
Interpreter doRecalculation("recalc", recalculateFromNormal);

bool intersect(StatData range_1, StatData range_2)
{

  if (range_1.max < range_2.min || range_2.max < range_1.min)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void loop()
{
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
  cleanNoAirMin.refresh(interpreter::command);
  doRecalculation.refresh(interpreter::command);
  //! HANDLE CLIENT
  server.handleClient();

  //! TOUCH INPUT
  touchInputHandler();

  //! UPDATES
  updateTacho();
  updateBattery();
  updateHeadSensor();
  updateVisor();

  //! ACTION ACCORDING TO UPDATED VALUES
  tachoRunner.takeAction();
  batteryRunner.takeAction();
  visorRunner.takeAction();
  headSensorRunner.takeAction();

  sensorConnectRequest();
  sensorDisconnectRequest();
  sensorReconnectingRequest();
  readSensorRunner.takeAction();
}
