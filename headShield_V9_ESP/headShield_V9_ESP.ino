// HEADSHIELD_V3.7 PCB_V10
//? SETTINGS
#define piezoActive true

//? LIBRARIES
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <analogWrite.h>
#include <pitches.h>
#include <Tone32.h>
#include "Wire.h"
#include "DFRobot_BME280.h"
#include "DFRobot_CCS811.h"
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
DFRobot_BME280_IIC pressureSensor(&Wire, 0x77);
DFRobot_CCS811 carbonSensor(&Wire, 0x5A);
float temperature;
uint32_t pressure;
float humidity;
float CO2;
float TVOC;

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
Beeper beeper(piezoPin, piezoChannel, piezoActive);

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

void setup()
{
  Serial.begin(115200);
  fan.begin();
  battery.begin();
  beeper.begin();
  reed.begin();
  audio.begin();
  lamp.begin();

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  pressureSensor.reset();
  if (pressureSensor.begin() != DFRobot_BME280_IIC::eStatusOK)
    Serial.println("inside pressure sensor faild");

  if (carbonSensor.begin() != 0)
  {
    Serial.println("carbon sensor faild");
  }

  server.on("/", handle_root);
  server.on("/readPressure", handle_readPressure);
  server.on("/readTemperature", handle_readTemperature);
  server.on("/readHumidity", handle_readHumidity);
  server.on("/readPPM", handle_readPPM);
  server.on("/readTVOC", handle_readTVOC);
  server.begin();
  beeper.boot();
  delay(100);
  audio.off();
}

//? HANDLERs
void handle_root()
{
  server.send(200, "text/html", webpageCode);
}
void handle_readPressure()
{
  String pressureString = (String)pressure;
  server.send(200, "text/plane", pressureString);
}
void handle_readTemperature()
{
  String tempString = (String)temperature;
  server.send(200, "text/plane", tempString);
}
void handle_readHumidity()
{
  String humidityString = (String)humidity;
  server.send(200, "text/plane", humidityString);
}
void handle_readPPM()
{
  String PPMString = (String)CO2;
  server.send(200, "text/plane", PPMString);
}
void handle_readTVOC()
{
  String TVOCString = (String)TVOC;
  server.send(200, "text/plane", TVOCString);
}

Timer readSensorDataTimer(800);
Timer handleClientTimer(300);

bool IRactive = false;
void loop()
{

  if (IR.active())
  {
    if (!IRactive)
    {
      IRactive = true;
      beeper.lampIncriase();
    }
    if (touchLeft.doubleTap())
    {
      audio.toggle();
      if (audio.state)
        beeper.visorUp();
      else
        beeper.visorDown();
    }
    if (touchLeft.singleTap())
    {
      lamp.increaseLevel();
      lamp.setLevel(lamp.level);
      if (lamp.level == 0)
        beeper.lampOff();
      else
        beeper.lampIncriase();
    }

    if (touchRight.singleTap())
    {
      fan.increaseLevel();
      fan.setLevel(fan.level);
      if (fan.level == 0)
        beeper.lampOff();
      else
        beeper.lampIncriase();
    }
  }
  else
  {
    if (IRactive)
    {
      IRactive = false;
      beeper.lampOff();
    }
    fan.setSpeed(0);
    lamp.setLevel(0);
  }
  if (handleClientTimer.timeElapsedMillis())
    server.handleClient();

  if (readSensorDataTimer.timeElapsedMillis())
  {
    pressure = pressureSensor.getPressure();
    temperature = pressureSensor.getTemperature();
    humidity = pressureSensor.getHumidity();
    CO2 = carbonSensor.getCO2PPM();
    TVOC = carbonSensor.getTVOCPPB();
  }
}
