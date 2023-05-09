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

//? GLOBAL
int mode = 1;

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
  /*
  //* ENS160
  if (ENS160.begin() != NO_ERR)
  {
    while (ENS160.begin() != NO_ERR)
    Serial.println("ENS160 failed");
    perkData.initializeEN160 = false;
  }
  else
  {
    Serial.println("ENS160 sucess");
    perkData.initializeEN160 = false;
  }
*/

  while (NO_ERR != ENS160.begin())
  {
    perkData.initializeEN160 = true;
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }
  Serial.println("Begin ok!");
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
void handler_helmetData()
{
  server.send(200, "text/html", helmetDataPage);
}

void handler_getHelmetData()
{
  String jsonData = "{";
  jsonData += "\"visorState\":\"" + String(visor.state) + "\",";
  jsonData += "\"IRState\":\"" + String(IR.state) + "\",";
  jsonData += "\"fanLevel\":" + String(fan.level) + ","; // Remove quotation marks around fan.level value
  jsonData += "\"lampLevel\":\"" + String(lamp.level) + ",";
  jsonData += "\"batteryLevel\":\"" + String(battery.level) + "\",";
  jsonData += "\"audioState\":\"" + String(audio.state) + "\",";
  jsonData += "\"fanRPM\":\"" + String(tachometer.speed_rpm) + "\"";
  jsonData += "}";

  server.send(200, "application/json", jsonData);
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

void modeSelector()
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

//* TOUCH
void serveTouch()
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
void reconsiderServiceMode()
{
  if (triggerServiceMode() || mode == 0)
  {
    while (true)
    {
      //* service mode stuff
    }
  }
}
Timer checkBatteryTimer(5000);
//* BATTERY
void checkBattery()
{
  if (checkBatteryTimer.timeElapsedMillis())
    battery.getLevel();

  switch (battery.level)
  {
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  default:
    Serial.println("SWITCH_ERROR_BATTERY");
  }
}

//* TACHOMETER
Timer chechTachometerTimer(1000);
void checkTachometer()
{
  if (chechTachometerTimer.timeElapsedMillis())
    tachometer.getRPM();
}

//* SENSOR DATA
Timer refreshSensorDataTimer(2000);

void refreshSensorData()
{
  if (refreshSensorDataTimer.timeElapsedMillis())
  {
    if (perkData.initializeBME280)
    {
      Serial.println("Read BME280");
      perkData.temp = BME280.getTemperature();
      perkData.press = BME280.getPressure();
      perkData.humi = BME280.getHumidity();
    }
    else
    {
      Serial.println("Read BME280 failed");
    }
    if (true)
    {
      Serial.println("Read ENS160");
      perkData.status = ENS160.getENS160Status();
      perkData.AQI = ENS160.getAQI();
      perkData.TVOC = ENS160.getTVOC();
      perkData.ECO2 = ENS160.getECO2();
    }
    else
    {
      Serial.println("Read ENS160 failed");
    }
    perkData.log();
  }
}
Timer getSensorTimer(1000);
//* LOOP
void loop()
{
  refreshSensorData();
  server.handleClient();
  checkBattery();
  checkTachometer();
  refreshSensorData();
  modeSelector();
  serveTouch();

  reconsiderServiceMode();
}

// data shown on website

//1 helmet data
// visor state (up/down) {visor.state}
// weather the user wearing the helmet or not (active/deactive) {IR.state}
// fan speed (low/medium/high/OFF) {fan.level}
// lamp sensitivity (low/medium/high/OFF) {lamp.level}
// battery level (low/medium/high) {battrey.level}
// audio state (on/off) {audio.state}
// fan RPM (the number with RPM dimension) {tachometer.speed_rpm}

//2 sensor data
//2.1 BMS280
// temperature {perkData.temp}
// pressure {perkData.press}
// humidity {perkData.humi}
//2.2 ENS160
// status (0-> normal, 1-> warm up up, 2-> start up) {perkdata.status}
// AQI (air quality index) {perkdata.AQI}
// TVOC (concentration of total volatile organic compounds) {perkdata.TVOC}
// ECO2 (carbon dioxide equivalent concentration) {perkdata.EOC2}

//3 control
//3.1 fan control
// four button in a row called low, medium, high, and OFF, where the currently acive states button is green and the other ones are grey
//3.2 lamp control
// four button in a row called low, medium, high, and OFF, where the currently acive states button is green and the other ones are grey
//3.3 audio toggle
// a single button which is called as the current state of the audio (on or off) and if the user press it, it will toggle between the two state
