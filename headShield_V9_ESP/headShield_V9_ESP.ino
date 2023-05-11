// HEADSHIELD_V3.7 PCB_V10
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
#include "tachometer.h"

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

//? FAN
const int fanPin = 5;
const int tachoPin = 17;
Fan fan(fanPin);

//? TACHOMETER
const int tachometerPin = 17;
Tachometer tachometer(tachometerPin);

//? POWER LED
const int LEDPin = 19;
LED lamp(LEDPin);

//? IR
const int infraredPin = 35;
infraredSensor IR(infraredPin);

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

Timer chechTachometerTimer(1000);
Timer checkBatteryTimer(5000);

Timer refreshSensorDataTimer(1000);
Timer connectSensorTimer(2000);
//? GLOBAL
int mode = 1;
bool newSensorConnection = false;

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
  tachometer.begin();

  //* WIFI
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  //* HANDLERS
  server.on("/", handle_root);
  server.on("/helmetData", handler_helmetData);
  server.on("/getHelmetData", handler_getHelmetData);
  server.on("/sensorData", handler_sensorData);
  server.on("/getSensorData", handler_getSensorData);
  server.on("/setFanSpeed", handler_setFanSpeed);
  server.on("/control", HTTP_GET, handler_controlPage);
  server.begin();

  //* BME280
  BME280.reset();
  if (BME280.begin() != DFRobot_BME280_IIC::eStatusOK)
  {
    perkData.initializeBME280 = false;
    Serial.println("BME280 faild");
  }
  else
  {
    perkData.initializeBME280 = true;
    Serial.println("BME280 sucess");
  }

  //* ENS160
  if (ENS160.begin() != NO_ERR)
  {
    perkData.initializeENS160 = false;
    Serial.println("ENS160 failed");
  }
  else
  {
    perkData.initializeENS160 = true;
    Serial.println("ENS160 sucess");
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
  while (multiTouch())
  {
    if (serviceModeTimer.timeElapsedMillis())
    {
      mode = 0;
      break;
    }
  }
}

//* HANDLERs
void handle_root()
{
  server.send(200, "text/html", webpageCode);
}
void handler_sensorData()
{
  server.send(200, "text/html", sensorDataPage);
}
void handler_helmetData()
{
  server.send(200, "text/html", helmetDataPage);
}
void handler_getHelmetData()
{
  StaticJsonDocument<200> doc;

  doc["visorState"] = visor.state;
  doc["IRState"] = IR.state;
  doc["fanLevel"] = fan.level;
  doc["lampLevel"] = lamp.level;
  doc["batteryLevel"] = battery.level;
  doc["audioState"] = audio.state;
  doc["fanRPM"] = tachometer.speed_rpm;

  String jsonData;
  serializeJson(doc, jsonData);

  server.send(200, "application/json", jsonData);
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
void handler_controlPage()
{
  server.send(200, "text/html", controlPage);
}
void handler_setFanSpeed()
{
  int value = server.arg("value").toInt();
  // Here, you can update the fan speed value on the helmet
  // ...
  Serial.print("New fan value: ");
  Serial.println(value);

  server.send(200, "text/plain", "OK");
}
//* MODE
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

//* TOUCH

bool multiTouch()
{
  if (touchLeft.readAtTheMoment() && touchRight.readAtTheMoment())
    return true;
  else
    return false;
}

//* SERVICE MODE
bool triggerServiceMode()
{
  if (multiTouch())
  {
    while (multiTouch())
    {
      if (serviceModeTimer.timeElapsedMillis())
      {
        return true;
      }
    }
  }
  else
  {
    serviceModeTimer.preTime = millis();
    return false;
  }
  return false;
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
bool sensorConnected()
{
  if (!BME280Connected() || !ENS160Connected())
    return false;
  else
    return true;
}
bool connectSensor()
{
  BME280.reset();
  if (BME280Connected())
  {
    perkData.initializeBME280 = true;
    Serial.println("BME280 sucess");
  }
  else
  {
    perkData.initializeBME280 = false;
    Serial.println("BME280 failed");
  }

  //* ENS160
  if (ENS160Connected())
  {
    perkData.initializeENS160 = true;
    Serial.println("ENS160 sucess");
  }
  else
  {
    perkData.initializeENS160 = false;
    Serial.println("ENS160 failed");
  }

  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  ENS160.setTempAndHum(25.0, 50.0);

  if (perkData.initializeENS160 && perkData.initializeBME280)
    return true;
  else
    return false;
}

//* MAIN FUNCTIONS
void main_sensorConnection(unsigned long _loopTime)
{
  static unsigned long sinceStart = millis();
  if (millis() - sinceStart > _loopTime)
  {
    if (connectSensor())
    {
      if (newSensorConnection)
      {
        Serial.println("connection established");
        beeper.playSuccess();
        newSensorConnection = false;
      }
    }
    else
    {
      if (!newSensorConnection)
        beeper.playError();
      newSensorConnection = true;
    }
    sinceStart = millis();
  }
}

void main_battery(unsigned long _loopTime)
{
  static unsigned long sinceStart = millis();
  if (millis() - sinceStart > _loopTime)
    battery.getLevel();

  switch (battery.level)
  {
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
    sinceStart = millis();
  }
}
void main_tachometer(unsigned long _loopTime)
{
  static unsigned long sinceStart = millis();
  if (millis() - sinceStart > _loopTime)
  {
    tachometer.getRPM();
    sinceStart = millis();
  }
}
void main_readSensorData(unsigned long _loopTime)
{
  static unsigned long sinceStart = millis();
  if (millis() - sinceStart > _loopTime)
  {
    perkData.temp = BME280.getTemperature();
    perkData.press = BME280.getPressure();
    perkData.humi = BME280.getHumidity();

    perkData.status = ENS160.getENS160Status();
    perkData.AQI = ENS160.getAQI();
    perkData.TVOC = ENS160.getTVOC();
    perkData.ECO2 = ENS160.getECO2();
    //perkData.log();
    sinceStart = millis();
  }
}
void main_mode()
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
      fan.suspend();
      audio.suspend();
      break;

    case 3: //? IR OFF
      beeper.playVisorUp();

      fan.suspend();
      lamp.suspend();
      audio.suspend();
      break;

    case 4: //? ALL OFF
      beeper.playVisorUp();

      fan.suspend();
      lamp.suspend();
      audio.suspend();
      break;
    }
  }
}
void main_touchInput()
{
  if (touchLeft.singleTap()) //! LAMP INPUT
  {
    lamp.toggle(0, 3);
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
  else if (touchRight.longTap()) //! AUDIO INPUT
  {
    audio.toggle();

    if (audio.state)
      beeper.playVisorUp();
    else
      beeper.playVisorDown();
  }
}
void main_serviceMode()
{
  if (triggerServiceMode() || mode == 0)
  {
    while (true)
    {
      //* service mode stuff
    }
  }
}

void main_handleClient(unsigned long _loopTime)
{
  static unsigned long timeSince = millis();
  if (millis() - timeSince > _loopTime)
  {
    server.handleClient();
    timeSince = millis();
  }
}


void loop()
{
  main_mode();
  main_touchInput();
  main_serviceMode();

  main_handleClient(500);
  main_sensorConnection(4000);
  main_readSensorData(1000);
  main_battery(5000);
  main_tachometer(400);
}



/*

void loop()
{
  unsigned long startTime;  // Variable to store the start time of each function

  startTime = millis();
  main_mode();
  Serial.print("main_mode: ");
  Serial.println(millis() - startTime);

  startTime = millis();
  main_touchInput();
  Serial.print("main_touchInput: ");
  Serial.println(millis() - startTime);

  startTime = millis();
  main_serviceMode();
  Serial.print("main_serviceMode: ");
  Serial.println(millis() - startTime);

  startTime = millis();
  main_handleClient(0);
  Serial.print("main_handleClient: ");
  Serial.println(millis() - startTime);

  startTime = millis();
  main_sensorConnection(0);
  Serial.print("main_sensorConnection: ");
  Serial.println(millis() - startTime);

  startTime = millis();
  main_readSensorData(0);
  Serial.print("main_readSensorData: ");
  Serial.println(millis() - startTime);

  startTime = millis();
  main_battery(0);
  Serial.print("main_battery: ");
  Serial.println(millis() - startTime);

  startTime = millis();
  main_tachometer(0);
  Serial.print("main_tachometer: ");
  Serial.println(millis() - startTime);

  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  
}
*/
