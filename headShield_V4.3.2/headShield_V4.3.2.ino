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
#include "webpage.h"
#include "timer.h"
#include "fan.h"
#include "beeper.h"
#include "battery.h"
#include "reed.h"
#include "LED.h"
#include "touchInput.h"
#include "infraredSensor.h"
#include "audioEN.h"
#include "sensor_data.h"
#include "tacho.h"
#include "movingAverage.h"
#include "TimeManager.h"
//? WIFI
const char *ssid = "headShield_";
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

//? TACHOMETER
const int tachometerPin = 39;
Tachometer tacho(tachometerPin);
int tachoFinalValue = -1;

//? POWER LED
const int LEDPin = 19;
LED lamp(LEDPin);

//? HEAD SENSOR
const int infraredPin = 35;
infraredSensor headSensor(infraredPin);

//? TOUCH INPUT
const int touchRightPin = 33;
const int touchLeftPin = 15;
Touch touchRight(touchRightPin, 0, 24);
Touch touchLeft(touchLeftPin, 18, 34);

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

void setup()
{
  //* BEGIN
  Serial.begin(115200);
  fan.begin();
  lamp.begin();
  audio.begin();
  beeper.begin(SOUND_ACTIVE);
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
  beeper.playStartup();
  delay(100);

  //* INITIALIZE STARTING VALUES
  lamp.level = 0;
  fan.level = 0;
  audio.state = LOW;

  //* SERVICE MODE
  while (touchLeft.getDigital() && touchRight.getDigital())
  {
    static Timer serviceModeTimer(3000);
    if (serviceModeTimer.timeElapsedMillis())
    {
      while (true)
      {
        static Timer serviceModePrint(1000);
        if (serviceModePrint.timeElapsedMillis())
        {
          Serial.println("SERVICE MODE ENABLED");
        }
      }
    }
  }
}

//* HANDLERS
// ROOT HANDLERS
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
    doc["fanLevel"] = fan.level;
    doc["lampLevel"] = lamp.level;
    doc["batteryLevel"] = battery.level;
    doc["audioState"] = audio.state;
    doc["fanRPM"] = tachoFinalValue;

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
// SET HANDLERS
void handler_setFanSpeed()
{
  int newFanLevel = server.arg("value").toInt();

  fan.setLevel(newFanLevel);
  server.send(200, "text/plain", "OK");
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
void doFunction(void (*function)(), unsigned long interval)
{
  static Timer timer(interval);
  if (timer.timeElapsedMillis())
    function();
}
bool isStableInput(bool actualState, unsigned long stableTime)
{
  static bool prevState = false;
  static unsigned long stableStartTime = 0;

  if (actualState != prevState)
  {                             // If input state changes
    stableStartTime = millis(); // reset timer
    prevState = actualState;
  }

  if (millis() - stableStartTime >= stableTime)
  {
    return true;
  }
  else
  {
    return false;
  }
}
bool isStableInput_forHeadSensor(bool actualState, unsigned long stableTime)
{
  static bool prevState = false;
  static unsigned long stableStartTime = 0;

  if (actualState != prevState)
  {                             // If input state changes
    stableStartTime = millis(); // reset timer
    prevState = actualState;
  }

  if (millis() - stableStartTime >= stableTime)
  {
    return true;
  }
  else
  {
    return false;
  }
}
bool isAnyTouch()
{
  if (touchLeft.getDigital() || touchRight.getDigital())
    return true;
  else
    return false;
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
  if (touchLeft.singleTap() && !touchRight.getDigital()) //? LEFT SINGLE -> LAMP CONTROL
  {
    lamp.toggle(0, 3);
    if (lamp.level == 0)
      beeper.playLampOff();
    else
      beeper.playLampOn();
  }

  if (touchLeft.longTap() && !touchRight.getDigital()) //? LEFT LONG -> SENSOR CONNECTION
  {
    connectCommand = true;
  }

  if (touchRight.singleTap() && !touchLeft.getDigital()) //? RIGHT SINGLE -> FAN CONTROL
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

  if (touchRight.longTap() && !touchLeft.getDigital()) //? RIGHT LONG -> AUDIO CONTROL
  {
    audio.toggle();

    if (audio.state)
      beeper.playVisorUp();
    else
      beeper.playVisorDown();
  }

  if (multiTouch()) //? MULTI TOUCH -> TURN OFF FANS
  {
    fan.setLevel(0);
  }
}

//* VISOR
bool visorStateChange()
{
  const unsigned int minimumDuration = 400;

  static unsigned long timeWhenStart = 0;
  static bool timeStart = false;

  bool newVisorState = visor.scan();

  static bool prevVisorState = !newVisorState;

  if (newVisorState != prevVisorState)
  {
    if (!timeStart)
    {
      timeWhenStart = millis();
      timeStart = true;
    }
    if (millis() - timeWhenStart > minimumDuration)
    {
      prevVisorState = newVisorState;
      timeStart = false;
      timeWhenStart = 0;
      return true;
    }
  }
  else
  {
    timeStart = false;
    timeWhenStart = 0;
  }

  return false;
}
void visorStateHandler()
{
  if (visorStateChange())
  {
    switch (visor.state)
    {
    case 1: //* ACTIVE
      fan.setLevel(fan.level);
      Serial.println("back to active");
      beeper.playVisorFoldedDown();
      break;
    case 0: //* INACTIVE
      Serial.println("deactivating");
      fan.suspend();
      beeper.playVisorFoldedUp();
      break;
    }
  }
}

//* HEAD SENSOR
float getHeadSensorAverage()
{
  static MovingAverage headSensorAverage;
  headSensorAverage.add(headSensor.read());
  return headSensorAverage.average();
}
void headSensorStateHandler()
{
  static bool averageState;

  if (getHeadSensorAverage() != 4095)
    averageState = true;
  else
    averageState = false;

  static bool onDone = false;
  static bool offDone = false;

  if (isStableInput_forHeadSensor(averageState, 1000))
  {
    switch (averageState)
    {
    case 1:
      if (!onDone)
      {
        beeper.playHelmetPutOn();
        Serial.println("HELMET ON");
        onDone = true;
        offDone = false;
      }
      break;
    case 0:
      if (!offDone)
      {
        beeper.playHelmetTakenOff();
        Serial.println("HELMET OFF");
        offDone = true;
        onDone = false;
      }
      break;
    }
  }
}

//* BATTERY
void batteryLevelHandling()
{
  battery.getLevel();
  // do someting according to the new value
}

//* TACHOMETER
void checkFanError()
{
  if (tachoFinalValue < 3575)
  {
    if (isTimePassed(2000))
    {
      beeper.playFanError();
    }
  }
}
void updateTachometer()
{
  if (fan.level == 3 && !fan.suspended)
  {
    static MovingAverage tachoValueAverage;
    static MovingAverage tachoValueSuperAverage;
    static int tachoFinalValuePre;
    unsigned long valueToAdd = tacho.measureAverageDutyCycle(5, 60, isAnyTouch);
    if (valueToAdd != 0)
      tachoValueAverage.add(valueToAdd);
    tachoFinalValuePre = tachoValueAverage.average();
    tachoValueSuperAverage.add(tachoFinalValuePre);
    tachoFinalValue = tachoValueSuperAverage.average();

    checkFanError();
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
        beeper.playSuccess();
        Serial.println("Sensor connected OK");
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
      beeper.playShutdown();
      Serial.println("disconnecting sensor");
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
      Serial.println("Reconnecting sensor");
    }
  }
}

void loop()
{
  server.handleClient();
  updateTachometer();
  Serial.println(tachoFinalValue);
  Serial.println(headSensor.read());
  touchInputHandler();
  visorStateHandler();

  headSensorStateHandler();
  sensorConnectRequest();
  sensorDisconnectRequest();

  sensorReconnectingRequest();

  doFunction(readSensorData, 200);
  doFunction(batteryLevelHandling, 4000);
}
