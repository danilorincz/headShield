// HEADSHIELD_V3.7 PCB_V10
//? SETTINGS
#define SOUND_ACTIVE true
//? LIBRARIES
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <analogWrite.h>
#include <pitches.h>
#include <Tone32.h>
#include "Wire.h"
#include "DFRobot_BME280.h"
#include <DFRobot_ENS160.h>
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\webpage.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\timer.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\fan.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\beeper.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\battery.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\reed.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\powerLed.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\touchInput.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\infraredSensor.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\audioEN.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V9_ESP\sensor_data.h"

//?  SENSOR
DFRobot_BME280_IIC BME280(&Wire, 0x76);
DFRobot_ENS160_I2C ENS160(&Wire, 0x53);

SensorData perkData;

//? FAN
const int fanPin = 5;
const int tachoPin = 17;
Fan fan(fanPin);

//? IR
const int infraredPin = 35;
infraredSensor IR(infraredPin);

//? POWER LED
const int powerLedPin = 19;
powerLED lamp(powerLedPin);

//? TOUCH INPUT
const int touchRightPin = 33;
const int touchLeftPin = 15;
touchInput touchRight(touchRightPin, 30);
touchInput touchLeft(touchLeftPin, 30);

//? REED SWITCH
const int reedSwitchPin = 18;
ReedSwitch reed(reedSwitchPin);

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

//? WIFI
const char *ssid = "headShield";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

//? TIMERS
Timer serviceModeTimer(3000);
//? GLOBAL
serviceMode = false;

void setup()
{
  //* BEGIN
  Serial.begin(115200);
  fan.begin();
  battery.begin();
  beeper.begin(SOUND_ACTIVE);
  reed.begin();
  audio.begin();
  lamp.begin();

  //* WIFI
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  //* BME280
  BME280.reset();
  if (BME280.begin() != DFRobot_BME280_IIC::eStatusOK)
    Serial.println("inside pressure sensor faild");

  //* ENS160
  if (ENS160.begin() != NO_ERR)
  {
    Serial.println("Communication with device failed, please check connection");
    delay(50);
  }
  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  ENS160.setTempAndHum(/*temperature=*/25.0, /*humidity=*/50.0);

  //* HANDLERS
  server.on("/", handle_root);
  server.on("/readPressure", handle_readPressure);
  server.on("/readTemperature", handle_readTemperature);
  server.on("/readHumidity", handle_readHumidity);
  server.on("/readPPM", handle_readPPM);
  server.on("/readTVOC", handle_readTVOC);
  server.begin();

  //* BOOT
  beeper.playStartupTone();
  delay(100);
  audio.on();
  
  while (touchLeft.readAtTheMoment() && touchRight.readAtTheMoment())
  {
    if (serviceModeTimer.timeElapsed())
    {
      serviceMode = true;
      break;
    }
  }
}

void getSensorData()
{
  perkData.press = BME280.getPressure();
  perkData.temp = BME280.getTemperature();
  perkData.humi = BME280.getHumidity();

  perkData.status = ENS160.getENS160Status();
  perkData.AQI = ENS160.getAQI();
  perkData.TVOC = ENS160.getTVOC();
  perkData.ECO2 = ENS160.getECO2();

  Serial.print("Sensor operating status : ");
  Serial.println(Status);
  Serial.print("Air quality index : ");
  Serial.println(AQI);
  Serial.print("Concentration of total volatile organic compounds : ");
  Serial.print(TVOC);
  Serial.println(" ppb");
  Serial.print("Carbon dioxide equivalent concentration : ");
  Serial.print(ECO2);
  Serial.println(" ppm");
  Serial.println();
}

//? HANDLERs
void handle_root()
{
  server.send(200, "text/html", webpageCode);
}
void handle_readPressure()
{
  String pressureString = (String)perkData.press;
  server.send(200, "text/plane", pressureString);
}
void handle_readTemperature()
{
  String tempString = (String)perkData.temp;
  server.send(200, "text/plane", tempString);
}
void handle_readHumidity()
{
  String humidityString = (String)perkData.humi;
  server.send(200, "text/plane", humidityString);
}
void handle_readPPM()
{
  String PPMString = (String)perkData.ECO2;
  server.send(200, "text/plane", PPMString);
}
void handle_readTVOC()
{
  String TVOCString = (String)perkData.TVOC;
  server.send(200, "text/plane", TVOCString);
}

// 1-> bekapcsol a ventillátor, beveszi a touch inputokat
// 2-> kikapcsol a ventillátor, nem veszi be a touch inputokat, kikapcsol az audio
// 3-> kikapcsol a ventillátor, beveszi a touch inputokat, kikapcsol az audio, kikapcsolnak a lámpák
// 4-> kikapcsol a ventillátor, nem veszi be a touch inputokat, kikapcsol az audio, a lámpák maradnak
int mode = 0;
void loop()
{
  if (presence && visorState) // wearing and active visor
  {
    mode = 1;
  }
  else if (presence && !visorState) // wearing but deactivated visor
  {
    mode = 2;
  }
  else if (!presence && visorState) // not wearing but active visor
  {
    mode = 3;
  }
  else if (!presence && !visorState) // not wearing and deactivated visor
  {
    mode = 4;
  }

  switch (mode)
  {
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    break;
  default:
  }
}

void presenceDetection()
{
}

void touchDetection()
{

  touchRight.singleTap();
  touchRight.doubleTap();

  if (touchRight.longTap())
  {
    fan.toggleLevel();
    fan.setLevel(fan.level);
  }

  touchLeft.singleTap();
  touchLeft.doubleTap();
  touchLeft.longTap();
}

void visorDetection()
{
}

void handleReedSwitch

    // old loop
    /*

  if (IR.active())
  {
    if (!IRactive)
    {
      IRactive = true;
    }
    if (touchLeft.doubleTap())
    {
      audio.toggle();
      if (audio.state)
        true;
      else
        true;
    }
    if (touchLeft.singleTap())
    {
      lamp.increaseLevel();
      lamp.setLevel(lamp.level);
      if (lamp.level == 0)
        true;
      else
        true;
    }

    if (touchRight.singleTap())
    {
      fan.increaseLevel();
      fan.setLevel(fan.level);
      if (fan.level == 0)
        true;
      else
        true;
    }
  }
  else
  {
    if (IRactive)
    {
      IRactive = false;
    }
    fan.setSpeed(0);
    lamp.setLevel(0);
  }
  if (handleClientTimer.timeElapsedMillis())
    server.handleClient();

  if (readSensorDataTimer.timeElapsedMillis())
  {
    pressure = BME280.getPressure();
    temperature = BME280.getTemperature();
    humidity = BME280.getHumidity();
    CO2 = carbonSensor.getCO2PPM();
    TVOC = carbonSensor.getTVOCPPB();
  }
*/