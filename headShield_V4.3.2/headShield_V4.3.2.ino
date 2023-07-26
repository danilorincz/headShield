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

//? CUSTOM LIBRARIEs
#include "Device.h"
#include "Audio.h"
#include "Fan.h"
#include "Lamp.h"
#include "FanCondition.h"
#include "StatData.h"
#include "Battery.h"
#include "Piezo.h"
#include "Webpage.h"
#include "Timer.h"
#include "reed.h"
#include "touchInput.h"
#include "infraredSensor.h"
#include "sensor_data.h"
#include "tacho.h"
#include "movingAverage.h"
#include "TimeManager.h"
#include "Interpreter.h"
#include "FunctionRunner.h"
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
FanCondition notEnoughAirflow;
FanCondition normal;    // N
FanCondition noFilter;  // I
FanCondition faultFan1; // Q

Statistics tachoStat(1000);
StatData latestTachoStat;

//? GLOBAL
bool serialEnabled = false;
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
  int normalMax = data.getInt("max", -1);
  int mormalMin = data.getInt("min", -1);

  normal.setLimit(mormalMin, normalMax);
  notEnoughAirflow.setLimit(normalMax + 1, 4000);
  data.end();
  delay(10);

  data.begin("noFilterLimits", false);
  int noFilterMax = data.getInt("max", -1);
  int noFilterMin = data.getInt("min", -1);
  noFilter.setLimit(noFilterMin, noFilterMax);
  data.end();
  delay(10);

  data.begin("faultFan1Limits", false);
  int faultFanMax = data.getInt("max", -1);
  int faultFanMin = data.getInt("min", -1);
  faultFan1.setLimit(faultFanMin, faultFanMax);
  data.end();
  delay(10);

  adcAttachPin(infraredPin);
  //* WIFI

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
  //* HANDLERS
  // ROOT HANDLERS
  server.on("/", handle_root);
  server.on("/helmetData", handler_helmetData);
  server.on("/getHelmetData", handler_getHelmetData);

  //server.on("/sensorData", handler_sensorData);
  //server.on("/control", handler_controlPage);
  //server.on("/debugData", handler_debugPage);
  // GET GANDLERS
  /*
  server.on("/getSensorData", handler_getSensorData);
  server.on("/debugdata", handler_getDebugData);
  // SET HANDLERS
  server.on("/setFanSpeed", handler_setFanSpeed);
  server.on("/setLampLevel", handler_setLampLevel);
  server.on("/setAudioState", handler_setAudioState);*/
  server.begin();
  delay(100);

  //* BME280
  BME280.reset();
  connectCommand = true;
  while (!isTimePassed(3000))
  {
    if (sensorConnectRequest())
      break;
  }

  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  ENS160.setTempAndHum(25.0, 50.0);

  //* BOOT
  piezo.playStartup();
}

//* HANDLERS
void handler_helmetData()
{
  server.send_P(200, "text/html", helmetDataPage);
}

void handler_getHelmetData()
{
  static Timer intervalTimer(1000);
  if (intervalTimer.timeElapsedMillis())
  {

    StaticJsonDocument<200> doc;

    doc["visorState"] = visor.state;
    doc["IRState"] = headSensor.state;
    doc["fanLevel"] = fan.state;
    doc["lampLevel"] = lamp.level;
    doc["batteryLevel"] = battery.level;
    doc["audioState"] = audio.state;
    doc["fanRPM"] = fanErrorNumber;

    String jsonData;
    serializeJson(doc, jsonData);

    server.send(200, "application/json", jsonData);
  }
}
void handle_root()
{
  server.send_P(200, "text/html", webpageCode);
}
/*
void handler_sensorData()
{
  server.send_P(200, "text/html", sensorDataPage);
}
void handler_controlPage()
{
  server.send_P(200, "text/html", controlPage);
}
void handler_debugPage()
{
  server.send_P(200, "text/html", DEBUG_PAGE);
}




void handler_getSensorData()
{
  StaticJsonDocument<200> doc;

  char tempStr[6]; // Buffer big enough for "-XX.X" and a null terminator
  char humiStr[6]; // Buffer big enough for "-XX.X" and a null terminator

  // Format temperature and humidity as strings with 1 decimal place
  dtostrf(perkData.temp, 5, 1, tempStr);
  dtostrf(perkData.humi, 5, 1, humiStr);

  doc["temp"] = tempStr;
  doc["press"] = perkData.press;
  doc["humi"] = humiStr;
  doc["AQI"] = perkData.AQI;
  doc["TVOC"] = perkData.TVOC;
  doc["ECO2"] = perkData.ECO2;
  doc["status"] = perkData.status;

  String jsonData;
  serializeJson(doc, jsonData);

  server.send(200, "application/json", jsonData);
}
void handler_getDebugData()
{
  StaticJsonDocument<200> doc;
  doc["dummy1"] = touchRight.getAnalog();
  doc["dummy2"] = touchLeft.getAnalog();
  doc["dummy3"] = headSensor.read();
  String jsonData;
  serializeJson(doc, jsonData);
  server.send(200, "application/json", jsonData);
}

void handler_setLampLevel()
{
  int newLampLevel = server.arg("level").toInt();
  lamp.setLevel(newLampLevel);
  server.send(200, "text/plain", "Lamp level set");
}
void handler_setAudioState()
{
  String stateParam = server.arg("state"); // Get the state parameter from the request

  if (stateParam == "true")
  {
    audio.on();
  }
  else
  {
    audio.off();
  }

  server.send(200, "text/plain", "OK");
}
*/

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
  /*
  tachoStat.addValue(tacho.finalValue);
  latestTachoStat = tachoStat.getStats();*/
}
void parseAndAction_tacho() // working with: latestTachoStat.average
{
  if (!fan.active())
    return;
  if (normal.inRange(tacho.finalValue))
  {
    serailPrintIf("Normál működés, szűrők fent, ventillátorok jók");
    fanErrorNumber = 1;
  }
  else if (noFilter.inRange(tacho.finalValue))
  {
    serailPrintIf("Nincs felhelyezve szűrő!");
    fanErrorNumber = 2;
  }
  else if (faultFan1.inRange(tacho.finalValue))
  {
    serailPrintIf("Az egyik ventillátor leállt!");
    fanErrorNumber = 3;
  }
  else if (notEnoughAirflow.inRange(tacho.finalValue))
  {
    serailPrintIf("Nincs elég térfogatáram! Ellenőrizze a szűrők állapotát!");
    fanErrorNumber = 4;
  }
  else
  {
    serailPrintIf("Lehetséges hogy valami akadályozza a levegő kiáramlását!");
    fanErrorNumber = 5;
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
  Serial.println("notEnoughAirflow");
  Serial.print("  MAX: ");
  Serial.println(notEnoughAirflow.getMax());
  Serial.print("  MIN: ");
  Serial.println(notEnoughAirflow.getMin());

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

void performTachoAnalysis(int condition)
{
  fan.on();
  int counter = 0;
  int allTimeMax = 0;
  int allTimeMin = 9999;
  Serial.println("PERFORM TACHO ANALYSIS STARTED!");
  while (true)
  {
    tacho.getAverage();

    if (tachoStat.addValue(tacho.finalValue))
    {
      Serial.println("new set of value added");
      counter++;
      latestTachoStat = tachoStat.getStats();
      if (counter > 3)
      {
        if (latestTachoStat.min < allTimeMin)
          allTimeMin = latestTachoStat.min;
        if (latestTachoStat.max > allTimeMax)
          allTimeMax = latestTachoStat.max;
      }
    }
    if (counter > 40)
    {
      break;
    }
  }
  latestTachoStat.max = allTimeMax + 5;
  latestTachoStat.min = allTimeMin - 2;

  Serial.println("PERFORM TACHO ANALYSIS ENDED!");
  Serial.print("Max limit: ");
  Serial.println(latestTachoStat.max);
  Serial.print("Min limit: ");
  Serial.println(latestTachoStat.min);

  switch (condition)
  {
  case 1: // normal
    data.begin("normalLimits", false);
    data.putInt("max", latestTachoStat.max);
    data.putInt("min", latestTachoStat.min);
    data.end();
    normal.setLimit(latestTachoStat.min, latestTachoStat.max);
    break;
  case 2: // noFilter
    data.begin("noFilterLimits", false);
    data.putInt("max", latestTachoStat.max);
    data.putInt("min", latestTachoStat.min);
    data.end();
    noFilter.setLimit(latestTachoStat.min, latestTachoStat.max);
    break;
  case 3: // faultFan1
    data.begin("faultFan1Limits", false);
    data.putInt("max", latestTachoStat.max);
    data.putInt("min", latestTachoStat.min);
    data.end();
    faultFan1.setLimit(latestTachoStat.min, latestTachoStat.max);
    break;
  }

  fan.off();
}

FunctionRunner tachoRunner(parseAndAction_tacho, 3000);
FunctionRunner batteryRunner(parseAndAction_battery, 4000);
FunctionRunner visorRunner(parseAndAction_visor, 100);
FunctionRunner headSensorRunner(parseAndAction_headSensor, 200);
FunctionRunner readSensorRunner(readSensorData, 200);

Interpreter printTachoValue("print tacho", printTacho_inter);
Interpreter printPeriferial("print periferial", printPeriferial_inter);
Interpreter printLimits("print limit", printLimits_inter);
//Interpreter analyseNormal("analyse normal", analyseTacho_inter);

void loop()
{
  if (Serial.available())
  {
    interpreter::command = interpreter::getCommand();
  }

  printTachoValue.refresh(interpreter::command);
  printPeriferial.refresh(interpreter::command);
  printLimits.refresh(interpreter::command);
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
