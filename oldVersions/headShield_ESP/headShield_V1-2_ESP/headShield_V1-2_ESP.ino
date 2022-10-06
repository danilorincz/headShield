
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\webpage.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\timer.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\tones.h"
#include <Arduino.h>
#include <analogWrite.h>
#include <pitches.h>
#include <Tone32.h>
#include "DFRobot_BME280.h"
#include "Wire.h"
#include "SPI.h"
#include <BME280I2C.h>
#include "DFRobot_CCS811.h"
#include "DFRobot_BME680_SPI.h"

//? SENSOR
typedef BME280I2C BME_I2C;
typedef DFRobot_CCS811 CCS_I2C;
typedef DFRobot_BME680_SPI BME_SPI;
const int CS_pin = 5;

BME280I2C::Settings settings(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_Off,
    BME280::SpiEnable_False,
    BME280I2C::I2CAddr_0x77);
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);

BME_I2C pressureSensorIn(settings);
CCS_I2C carbonSensor(&Wire, 0x5A);
// OUTSIDE
BME_SPI pressureSensorOut(CS_pin);

struct SensorData
{
public:
  float temp;
  float pressure;
  float humi;
  int ppm;  //? Parts Per Million
  int tovc; //? Total Volatile Organic Compounds
};

SensorData insideData;
SensorData outsideData;

//? WIFI
const char *ssid = "headShield";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

//? GPIO
const int fanPin = 33;
const int ledPin = 15;
const int batteryPin = 35;
const int piezoPin = 32;
const int piezoChannel = 1;
const int tiltPin = 39;

//? GLOBAL VARIABLES
unsigned long int millisecondsWhenStart = 0;
int fanSpeed = 0;
int prevFanSpeed = 0;
int prevPWMValue = 0;
bool tiltValue;

Timer getPressureTimer(150000);
Timer firstphaseTimer(800000);
Timer secondPhaseTimer(1000000);

int phase = 0;
bool startFirstPhaseTimer = true;
bool startSecondPhaseTimer = true;
bool remainTilted = false;
bool activationState = false;
bool prevActivationState = true;

void setup()
{
  Serial.begin(115200);
  pinMode(fanPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(piezoPin, OUTPUT);
  pinMode(tiltPin, INPUT);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  Wire.begin();
  if (!pressureSensorIn.begin())
    Serial.println("inside pressure sensor faild");

  settings.tempOSR = BME280::OSR_X4;
  pressureSensorIn.setSettings(settings);

  if (pressureSensorOut.begin() != 0)
    Serial.println("outside pressure sensor faild");

  if (carbonSensor.begin() != 0)
    Serial.println("carbon sensor faild");

  //! HANDLERS_SETUP
  server.on("/", handle_root);
  //? USER STUFF
  server.on("/readSinceStart", handle_readSinceStart);
  server.on("/readCharge", handle_readCharge);
  server.on("/readFanSpeed", handle_readFanSpeed);
  server.on("/readDeltaPressure", handle_readDeltaPressure);
  server.on("/readStatusState", handle_statusState);
  //? INSIDE
  server.on("/readInsidePressure", handle_readInsidePressure);
  server.on("/readInsideTemp", handle_readInsideTemp);
  server.on("/readInsideHum", handle_readInsideHum);
  server.on("/readInsidePPM", handle_readInsidePPM);
  server.on("/readInsideTVOC", handle_readInsideTVOC);
  //? OUTSIDE
  server.on("/readOutsidePressure", handle_readOutsidePressure);
  server.on("/readOutsideTemp", handle_readOutsideTemp);
  server.on("/readOutsideHum", handle_readOutsideHum);
  //? CONTROL PANEL
  server.on("/slider", handle_setSpeed);
  server.on("/led_set", handle_led);
  server.begin();

  bootSound();
  millisecondsWhenStart = millis();
}

//! 1_HANDLERS
void handle_root()
{
  server.send(200, "text/html", webpageCode);
}
//? INSIDE
void handle_readInsidePressure()
{
  server.send(200, "text/plane", (String)insideData.pressure);
}
void handle_readInsideTemp()
{
  server.send(200, "text/plane", (String)insideData.temp);
}
void handle_readInsideHum()
{
  server.send(200, "text/plane", (String)insideData.humi);
}
void handle_readInsidePPM()
{
  server.send(200, "text/plane", (String)insideData.ppm);
}
void handle_readInsideTVOC()
{
  server.send(200, "text/plane", (String)insideData.tovc);
}
//? OUTSIDE
void handle_readOutsidePressure()
{
  server.send(200, "text/plane", (String)outsideData.pressure);
}
void handle_readOutsideTemp()
{
  server.send(200, "text/plane", (String)outsideData.temp);
}
void handle_readOutsideHum()
{
  server.send(200, "text/plane", (String)outsideData.humi);
}
//? RESULT
void handle_readDeltaPressure()
{
  float pressureDifference = insideData.pressure - outsideData.pressure;

  server.send(200, "text/plane", (String)pressureDifference);
}
//? USER DATA
void handle_readCharge()
{
  int lowCounter = 0;
  int mediumCounter = 0;
  int highCounter = 0;

  int batChargeRaw = analogRead(batteryPin);
  int batChargeInt = map(batChargeRaw, 2925, 4096, 0, 100);

  String batCharge = (String)batChargeInt;
  server.send(200, "text/plane", batCharge);
}
void handle_statusState()
{
  String statusStateString;
  if (activationState)
    statusStateString = "active";
  else
    statusStateString = "OFF";

  server.send(200, "text/plane", statusStateString);
}
void handle_readSinceStart()
{
  unsigned long int sinceStartMillis = millis() - millisecondsWhenStart;
  unsigned long int sinceStartAllSeconds = sinceStartMillis / 1000;

  unsigned long int hours = 0;

  if (sinceStartAllSeconds >= 3600)
    hours = sinceStartAllSeconds / 3600;

  unsigned long int minutes = sinceStartAllSeconds / 60 - hours * 60;
  unsigned long int seconds = sinceStartAllSeconds % 60;

  String hoursString = "";
  String minutesString = "";
  String secondsString = "";

  if (hours < 10)
    hoursString = "0" + (String)hours;
  else
    hoursString = (String)hours;

  if (minutes < 10)
    minutesString = "0" + (String)minutes;
  else
    minutesString = (String)minutes;

  if (seconds < 10)
    secondsString = "0" + (String)seconds;
  else
    secondsString = (String)seconds;

  String exactTime = hoursString + ":" + minutesString + ":" + secondsString;

  server.send(200, "text/plane", exactTime);
}
void handle_readFanSpeed()
{
  server.send(200, "text/plane", (String)fanSpeed);
}
//? CONTROL PANEL
void handle_setSpeed()
{
  String message = server.arg("state");
  fanSpeed = message.toInt();
  setFanSpeed(fanSpeed);
  server.send(200, "text/html", webpageCode);
}
void handle_led()
{
  String state = "OFF";
  String actualState = server.arg("state");
  if (actualState == "1")
  {
    digitalWrite(ledPin, HIGH); //LED ON
    state = "ON";
  }
  else
  {
    digitalWrite(ledPin, LOW); //LED OFF
  }
  server.send(200, "text/plane", state);
}

//! 2_SENSOR
void getDataFromInside()
{

  float actualTemp, actualHumi, actualPressure;

  pressureSensorIn.read(actualPressure, actualTemp, actualHumi, tempUnit, presUnit);

  if ((actualTemp > -10 && actualTemp < 60) && actualTemp != 0)
    insideData.temp = actualTemp;

  if (actualPressure > 95000 && actualPressure < 105000)
    insideData.pressure = actualPressure;

  if (actualHumi > 10 && actualHumi < 90)
    insideData.humi = actualHumi;

  if (carbonSensor.checkDataReady() == true)
  {
    float actualCO2 = carbonSensor.getCO2PPM();
    if (actualCO2 < 10000)
      insideData.ppm = actualCO2;

    float actualTVOC = carbonSensor.getTVOCPPB();
    if (actualTVOC < 7000)
      insideData.tovc = actualTVOC;
  }
}
void getDataFromOutside()
{

  pressureSensorOut.startConvert();
  delay(100);
  pressureSensorOut.update();
  outsideData.temp = pressureSensorOut.readTemperature() / 100;
  outsideData.pressure = pressureSensorOut.readPressure();
  outsideData.humi = pressureSensorOut.readHumidity() / 1000;
}
void printSensorData()
{
  Serial.print("INSIDE");
  Serial.print("pressure: ");
  Serial.println(insideData.pressure);
  Serial.print("Temp: ");
  Serial.println(insideData.temp);
  Serial.print("hum: ");
  Serial.println(insideData.humi);
  Serial.print("PPM: ");
  Serial.println(insideData.ppm);
  Serial.print("TVOC: ");
  Serial.println(insideData.tovc);

  Serial.print("OUTSIDE");
  Serial.print("pressure: ");
  Serial.println(outsideData.pressure);
  Serial.print("Temp: ");
  Serial.println(outsideData.temp);
  Serial.print("hum: ");
  Serial.println(outsideData.humi);
}

//! 3_FAN_CONTROL
void setFanSpeed(int percent)
{
  fanSpeed = percent;
  int PWMvalue = map(percent, 0, 100, 0, 255);

  analogWrite(fanPin, PWMvalue);
  return;
  if (prevPWMValue >= PWMvalue)
  {
    analogWrite(fanPin, PWMvalue);
  }
  else
  {
    if (prevFanSpeed > 30)
    {
      analogWrite(fanPin, PWMvalue);
    }
    else
    {
      for (int i = prevPWMValue; i <= PWMvalue; i++)
      {
        analogWrite(fanPin, i);
        delay(70);
      }
    }
  }

  prevFanSpeed = fanSpeed;
  prevPWMValue = PWMvalue;
}

//! 4_TILT_TRIGGER
bool headTilted()
{
  if (digitalRead(tiltPin))
  {
    return false;
  }
  else
  {
    return true;
  }
}
void getTiltTrigger()
{
  switch (phase)
  {
  case 0:
    if (remainTilted)
    {
      if (!headTilted())
        remainTilted = false;
    }
    else
    {
      if (headTilted()) //! trigger start
      {
        phase = 1;
      }
    }
    break;
  case 1:
    if (startFirstPhaseTimer)
    {
      firstphaseTimer.preTime = micros();
      startFirstPhaseTimer = false;
    }
    if (firstphaseTimer.timeElapsed())
    {
      if (headTilted()) //! tovább
      {
        beepForward();
        phase = 2;
        startFirstPhaseTimer = true;
      }
    }
    else
    {
      if (!headTilted()) //! mégse
      {
        phase = 0;
        startFirstPhaseTimer = true;
      }
    }
    break;
  case 2:
    if (startSecondPhaseTimer)
    {
      secondPhaseTimer.preTime = micros();
      startSecondPhaseTimer = false;
    }
    if (!secondPhaseTimer.timeElapsed())
    {
      if (!headTilted()) //! tovább
      {
        phase = 3;
        startSecondPhaseTimer = true;
      }
    }
    else
    {
      if (headTilted()) //! mégse
      {
        beepTurnOff();
        phase = 0;
        startSecondPhaseTimer = true;
        remainTilted = true;
      }
    }
    break;
  case 3:
    activationState = !activationState;
    phase = 0;
    break;
  }
}



void loop()
{


  server.handleClient();
  if (getPressureTimer.timeElapsed())
  {
    getDataFromOutside();
    getDataFromInside();
    printSensorData();
  }

  getTiltTrigger();
  if (activationState != prevActivationState)
  {
    if (activationState)
    {
      beepTurnOn();
      setFanSpeed(100);
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      beepTurnOff();
      setFanSpeed(0);
      digitalWrite(ledPin, LOW);
    }
    prevActivationState = activationState;
  }
}
