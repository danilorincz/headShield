// HEADSHIELD_V4.3.1 PCB_V10
//? SETTINGS
#define SOUND_ACTIVE true
// right not tocuhing: 37 / 22 -> 30
// left not touching 65 / 37 -> 45
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

//? CUSTOM LIBRARIEs
#include "Device.h"
#include "Audio.h"
#include "Fan.h"
#include "Lamp.h"
#include "FanCondition.h"

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
FanCondition normal(3582, 3590);
FanCondition noFilter(3546 - 5, 3558 + 5);
FanCondition faultFan1(3531 - 5, 3538 + 5);
FanCondition notEnoughAirflow(3591, 3617 + 100);

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

//* TACHOMETER
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

//! UPDATE -> PARSE -> DO ACTION

//* TACHO
void updateTacho()
{
  if (fan.active())
  {
    tacho.getAverage();
  }
}
void parseAndAction_tacho() // working with: tacho.finalValue
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

void loop()
{
  server.handleClient();

  //! UPDATES
  static Timer tachoTimer(1000);
  updateTacho();
  if (tachoTimer.timeElapsedMillis())
    parseAndAction_tacho();

  static Timer batteryTimer(3000);
  updateBattery();
  if (batteryTimer.timeElapsedMillis())
    parseAndAction_battery();

  static Timer visorTimer(100);
  updateVisor();
  if (visorTimer.timeElapsedMillis())
    parseAndAction_visor();

  static Timer headSensorTimer(100);
  updateHeadSensor();
  if (headSensorTimer.timeElapsedMillis())
    parseAndAction_headSensor();

  touchInputHandler();

  sensorConnectRequest();
  sensorDisconnectRequest();
  sensorReconnectingRequest();

  static Timer sensorTimer(200);
  if (sensorTimer.timeElapsedMillis())
    readSensorData();

  static char serialInput = 'X';
  if (Serial.available() > 0)
  {
    serialInput = (char)Serial.read(); // Cast the read byte to a character
    for (int i = 0; i < 64; i++)
    {
      Serial.read();
    }
  }



  switch (serialInput)
  {
  case 'T':
    Serial.println(tacho.finalValue);
    break;
  case 'B':
    Serial.println(battery.percent);
    break;
  case 'H':
    Serial.println(headSensor.state);
    break;
  case 'V':
    Serial.println(visor.state);
    break;
  case 'E':
    serialEnabled = true;
    break;
  case 'X':
    serialEnabled = false;
    break;
  }
}
