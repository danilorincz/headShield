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
const char *ssid = "sisakk";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 11);
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
int fanErrorNumber = 0;
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
Touch touchLeft(touchLeftPin, 0, 50);

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

class FanCondition
{
public:
  int upperLimit;
  int lowerLimit;

  FanCondition(int lowerLimit, int upperLimit)
  {
    this->upperLimit = upperLimit;
    this->lowerLimit = lowerLimit;
  }
  bool inRange(int value)
  {
    if (lowerLimit <= value && value <= upperLimit)
      return true;
    return false;
  }
};

FanCondition normal(3582, 3590);
FanCondition noFilter(3546 - 5, 3558 + 5);
FanCondition faultFan1(3531 - 5, 3538 + 5);
FanCondition notEnoughAirflow(3591, 3617 + 100);

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
bool interruptMeasure()
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
  if (touchLeft.longTap() && !touchRight.getDigital()) //? FAN TOGGLE
  {
    Serial.println("Left long tap");
    fan.toggle();
    if (fan.active())
      beeper.playFanOn();
    else
      beeper.playFanOff();
  }

  if (touchRight.singleTap() && !touchLeft.getDigital()) //? LED CONTROL
  {
    lamp.toggleBetween(0, 3);
    if (lamp.level == 0)
      beeper.playLampOff();
    else
      beeper.playLampOn();
  }

  if (touchRight.longTap() && !touchLeft.getDigital()) //? AUDIO CONTROL
  {
    audio.toggle();

    if (audio.state)
      beeper.playVisorUp();
    else
      beeper.playVisorDown();
  }

  if (touchLeft.singleTap() && !touchRight.getDigital()) //? EMPTY
  {
  }
  if (multiTouch()) //? EMPTY
  {
  }
}

//* VISOR
bool visorStateChange()
{
  const unsigned int minimumDuration = 400;

  static unsigned long timeWhenStart = 0;
  static bool timeStart = false;

  bool newVisorState = visor.scan();

  static bool prevVisorState = LOW;

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
      if (headSensor.state)
      {
        fan.on();
      }
      Serial.println("back to active");
      beeper.playVisorFoldedDown();
      break;
    case 0: //* INACTIVE
      Serial.println("deactivating");
      fan.off();
      beeper.playVisorFoldedUp();
      break;
    }
  }
}

//* HEAD SENSOR
void headSensorStateHandler()
{
  static bool onDone = false;
  static bool offDone = false;

  headSensor.scan();
  if (isStableInput_forHeadSensor(headSensor.state, 1000))
  {
    switch (headSensor.state)
    {
    case 1:
      if (!onDone)
      {
        beeper.playHelmetPutOn();
        Serial.println("HELMET ON");
        if (visor.state)
        {
          fan.on();
        }
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
        if (!visor.state)
        {
          fan.off();
        }
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
int getAverage(int *arr, int size)
{
  int sum = 0;
  for (int i = 0; i < size; i++)
  {
    sum += arr[i];
  }
  return sum / size;
}

void updateTachometer()
{
  if (fan.active())
  {
    static int valueToAdd;
    static MovingAverage smooth_1;
    static MovingAverage smooth_2;
    static MovingAverage smooth_3;
    static int smooth_1_result;
    static int smooth_2_result;

    valueToAdd = tacho.measureAverageDutyCycle(5, 60, interruptMeasure);

    if (valueToAdd != 0)
    {
      smooth_1.add(valueToAdd);
      smooth_1_result = smooth_1.average();

      smooth_2.add(smooth_1_result);
      smooth_2_result = smooth_2.average();

      smooth_3.add(smooth_2_result);
      tachoFinalValue = smooth_3.average();
      //analyzeTachometer();
    }
    getAverageTachoValue();
  }
}
int averageTacho;
void getAverageTachoValue()
{
  if (!fan.active())
    return;

  static int monitorCounter = 0;
  static const int measureQuantity = 100;
  static int monitorSessionCounter = 0;

  static int lastSessionValues[measureQuantity];

  if (monitorCounter == 0)
  {
    monitorSessionCounter++;
  }

  lastSessionValues[monitorCounter] = tachoFinalValue;
  monitorCounter++;

  if (monitorCounter == measureQuantity)
  {
    monitorCounter = 0;
    averageTacho = getAverage(lastSessionValues, 100);
  }
}

void logAirflowError()
{
  if (normal.inRange(averageTacho))
  {
    Serial.println("Normál működés, szűrők fent, ventillátorok jók");
    fanErrorNumber = 1;
  }
  else if (noFilter.inRange(averageTacho))
  {
    Serial.println("Nincs felhelyezve szűrő!");
    fanErrorNumber = 2;
  }
  else if (faultFan1.inRange(averageTacho))
  {
    Serial.println("Az egyik ventillátor leállt!");
    fanErrorNumber = 3;
  }
  else if (notEnoughAirflow.inRange(averageTacho))
  {
    Serial.println("Nincs elég térfogatáram! Ellenőrizze a szűrők állapotát!");
    fanErrorNumber = 4;
  }
  else
  {
    Serial.println("Lehetséges hogy valami akadályozza a levegő kiáramlását!");
    fanErrorNumber = 5;
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

void analyzeTachometer()
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
  lastSessionValues[monitorCounter] = tachoFinalValue;
  monitorCounter++;
  Serial.println(tachoFinalValue);

  if (monitorCounter == measureQuantity)
  {

    beeper.playStartup();
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

bool debuggingTurnOn = false;
void loop()
{
  server.handleClient();

  updateTachometer();

  touchInputHandler();
  visorStateHandler();

  headSensorStateHandler();
  sensorConnectRequest();
  sensorDisconnectRequest();
  sensorReconnectingRequest();

  doFunction(readSensorData, 200);
  doFunction(batteryLevelHandling, 4000);
  static Timer logAirflowTimer(1000);
  if (logAirflowTimer.timeElapsedMillis())
  {
    logAirflowError();
  }

  if (Serial.available() > 0)
  {
    Serial.read();
    debuggingTurnOn = !debuggingTurnOn;
  }
  if (debuggingTurnOn)
  {
    Serial.println(averageTacho);
    /* Serial.print("Left touch value: ");
    Serial.println(touchLeft.getAnalog());*/
  }
}
