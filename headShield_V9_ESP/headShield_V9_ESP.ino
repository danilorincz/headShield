// HEADSHIELD_V3.7 PCB_V10
//? SETTINGS
#define piezoActive false

//? LIBRARIES
#include <Arduino.h>
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

#include <WiFi.h>
#include <WebServer.h>

//? INSIDE SENSOR
typedef DFRobot_BME280_IIC BME_I2C;
typedef DFRobot_CCS811 CCS_I2C;
BME_I2C pressureSensorIn(&Wire, 0x77);
CCS_I2C carbonSensor(&Wire, 0x5A);

#define SEA_LEVEL_PRESSURE 1015.0f
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
const int touchFrontPin = 15;
const int touchBackPin = 33;
touchInput touchFront(touchFrontPin, 30);
touchInput touchBack(touchBackPin, 30);

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

float temp;
uint32_t press;
float alti;
float humi;
float CO2;
float TVOC;

void setup()
{
  Serial.begin(115200);
  Serial.println("START");
  fan.begin();
  battery.begin();
  beeper.begin();
  reed.begin();
  audio.begin();
  pinMode(powerLedPin, OUTPUT);
  audio.off();

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  pressureSensorIn.reset();
  if (pressureSensorIn.begin() != BME_I2C::eStatusOK)
    Serial.println("inside pressure sensor faild");

  if (carbonSensor.begin() != 0)
  {
    Serial.println("carbon sensor faild");
  }

  server.on("/", handle_root);

  server.on("/readInsidePressure", handle_readInsidePressure);
  server.on("/readInsideTemp", handle_readInsideTemp);
  server.on("/readInsideHum", handle_readInsideHum);
  server.on("/readInsidePPM", handle_readInsidePPM);
  server.on("/readInsideTVOC", handle_readInsideTVOC);

  server.begin();
  delay(100);
}

//? HANDLERs
void handle_root()
{
  server.send(200, "text/html", webpageCode);
}

void handle_readInsidePressure()
{
  String pressureString = (String)press;
  server.send(200, "text/plane", pressureString);
}
void handle_readInsideTemp()
{
  String tempString = (String)temp;
  server.send(200, "text/plane", tempString);
}
void handle_readInsideHum()
{
  String humidityString=(String)humi;
  server.send(200, "text/plane", humidityString);
}
void handle_readInsidePPM()
{
  String PPMString=(String)CO2;
  server.send(200, "text/plane", PPMString);
}
void handle_readInsideTVOC()
{
  String TVOCString=(String)TVOC;
  server.send(200, "text/plane", TVOCString);
}

void loop()
{
  server.handleClient();

  CO2 = carbonSensor.getCO2PPM();
  TVOC = carbonSensor.getTVOCPPB();
  temp = pressureSensorIn.getTemperature();
  press = pressureSensorIn.getPressure();
  alti = pressureSensorIn.calAltitude(SEA_LEVEL_PRESSURE, press);
  humi = pressureSensorIn.getHumidity();

  
  if (touchFront.read())
  {
    lamp.increaseLevel();
    lamp.setLevel(lamp.level);
    if (lamp.level == 0)
    {
      beeper.lampOff();
    }
    else
    {
      beeper.lampIncriase();
    }
    delay(50);
  }
}

/* 
  //? CARBON
  Serial.println("CARBON_____________");

  if (carbonSensor.checkDataReady() == true)
  {
    CO2 = carbonSensor.getCO2PPM();
    TVOC = carbonSensor.getTVOCPPB();
    Serial.print("CO2: ");
    Serial.print(CO2);
    Serial.print("ppm, TVOC: ");
    Serial.print(TVOC);
    Serial.println("ppb");
  }
  else
  {
    Serial.println("Data is not ready!");
    CO2 = carbonSensor.getCO2PPM();
    TVOC = carbonSensor.getTVOCPPB();
    Serial.print("CO2: ");
    Serial.print(CO2);
    Serial.print("ppm, TVOC: ");
    Serial.print(TVOC);
    Serial.println("ppb");
  }

  temp = pressureSensorIn.getTemperature();
  press = pressureSensorIn.getPressure();
  alti = pressureSensorIn.calAltitude(SEA_LEVEL_PRESSURE, press);
  humi = pressureSensorIn.getHumidity();

  Serial.println("INSIDE PRESSURE_____________");
  Serial.print("temperature (unit Celsius): ");
  Serial.println(temp);
  Serial.print("pressure (unit pa):         ");
  Serial.println(press);

  Serial.print("humidity (unit percent):    ");
  Serial.println(humi);
  Serial.println(" ");
  Serial.println(" ");

*/
