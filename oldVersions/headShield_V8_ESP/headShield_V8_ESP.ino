// HEADSHIELD_V11 PCB_V8

//? LIBRARIES

#include <Arduino.h>
#include <analogWrite.h>
#include <pitches.h>
#include <Tone32.h>

#include "Wire.h"
#include "DFRobot_BME280.h"
#include "DFRobot_CCS811.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\webpage.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\timer.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\fan.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\beeper.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\battery.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\reed.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\powerLed.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\touchInput.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\infraredSensor.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\audioEN.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V8_ESP\sensor_data.h"

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>

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
const int piezoPin = 21;
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

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  pressureSensorIn.reset();
  if (pressureSensorIn.begin() != BME_I2C::eStatusOK)
    Serial.println("inside pressure sensor faild");

  if (carbonSensor.begin() != 0)
    Serial.println("carbon sensor faild");

  server.on("/", handle_root);

  server.on("/readInsidePressure", handle_readInsidePressure);
  server.on("/readInsideTemp", handle_readInsideTemp);
  server.on("/readInsideHum", handle_readInsideHum);
  server.on("/readInsidePPM", handle_readInsidePPM);
  server.on("/readInsideTVOC", handle_readInsideTVOC);
}

void handle_root()
{
  server.send(200, "text/html", webpageCode);
}
//? INSIDE
void handle_readInsidePressure()
{
  server.send(200, "text/plane", (String)press);
}
void handle_readInsideTemp()
{
  server.send(200, "text/plane", (String)temp);
}
void handle_readInsideHum()
{
  server.send(200, "text/plane", (String)humi);
}
void handle_readInsidePPM()
{
  server.send(200, "text/plane", (String)CO2);
}

void handle_readInsideTVOC()
{
  server.send(200, "text/plane", (String)TVOC);
}


void loop()
{

    server.handleClient();
  digitalWrite(fanPin, LOW);
  digitalWrite(audioEnPin, LOW);
  digitalWrite(powerLedPin, LOW);

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
  }
  //? INSIDE PRESSURE
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


  if (reed.read())
  {
    Serial.println("reed on");
    audio.on();
  }
  else
  {
    Serial.println("reed off");
    audio.off();
  }

  while (touchBack.readAtTheMoment())
  {
    fan.setSpeed(100);
  }
  fan.setSpeed(0);
  Serial.println(digitalRead(reedSwitchPin));
  if (digitalRead(reedSwitchPin))
  {
    digitalWrite(audioEnPin, LOW);
  }
  else
  {
    digitalWrite(audioEnPin, HIGH);
  }

  //* Lamp
  if (touchFront.readAtTheMoment())
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
