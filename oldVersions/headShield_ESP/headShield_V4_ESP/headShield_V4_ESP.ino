//? LIBRARIES
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
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
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\headShield_V4_ESP\webpage.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\headShield_V4_ESP\RGB.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\headShield_V4_ESP\timer.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\headShield_V4_ESP\fan.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\headShield_V4_ESP\sensor_data.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\headShield_V4_ESP\beeper.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\headShield_V4_ESP\battery.h"

//? WIFI
const char *ssid = "headShield";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

//? INSIDE SENSOR
typedef BME280I2C BME_I2C;
typedef DFRobot_CCS811 CCS_I2C;
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
SensorData insideData;

//? OUTSIDE SENSOR
typedef DFRobot_BME680_SPI BME_SPI;
const int CS_pin = 5;
BME_SPI pressureSensorOut(CS_pin);
SensorData outsideData;

//? RGB
const int RGB_redPin = 15;
const int RGB_greenPin = 16;
const int RGB_bluePin = 17;
RGB led(RGB_redPin, RGB_greenPin, RGB_bluePin);

//? FAN
const int fanPin = 32;
const int tachoPin = 36;
Fan fan(tachoPin, fanPin);

//? PIEZO
const int piezoPin = 33;
const int piezoChannel = 8;
Beeper beeper(piezoPin, piezoChannel);

//? BATTERY
const int batteryPin = 35;
Battery battery(batteryPin);

//? GPIO
const int vibrationPin = 34;
const int tiltPin = 39;

//? TIMERS
Timer measureBatteryCharge(10000000); // 10 sec
Timer measureInsideSensor(150000);    // 0.15 sec
Timer measureOutsideSensor(500000);   // 0.5 sec
Timer measureRPM(1000000);            // 1 sec
Timer scanVibrationTrigger(50000);    // 0.05 sec
Timer scanTiltTrigger(1000000);       // 1 sec

//? GLOBAL VARIABLES
unsigned long int millisecondsWhenStart = 0;
bool activationState = false;
int fanRPM = 0;

void setup()
{
  Serial.begin(115200);

  pinMode(vibrationPin, INPUT);
  pinMode(tiltPin, INPUT);

  battery.begin();
  fan.begin();
  led.begin();
  beeper.begin();

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
  server.on("/readDeltaPressure", handle_readDeltaPressure);
  server.on("/readStatusState", handle_statusState);
  server.on("/readRPM", handle_readRPM);
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

  beeper.boot();
  led.green();
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
  server.send(200, "text/plane", (String)battery.chargePercent);
}
void handle_statusState()
{
  String statusStateString;
  if (activationState)
    statusStateString = "ON";
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
void handle_readRPM()
{
  server.send(200, "text/plane", (String)fanRPM);
}

//? CONTROL PANEL
void handle_setSpeed()
{
  String message = server.arg("state");
  fan.setSpeed(message.toInt());
  server.send(200, "text/html", webpageCode);
}
void handle_led()
{
  String state;
  String actualState = server.arg("state");
  if (actualState == "1")
  {
    led.white();
    state = "ON";
  }
  else
  {
    state = "OFF";
    led.off();
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
void controlFan()
{
}
//! 4_VIBRATION_TRIGGER
bool scanVibration()
{
  return true;
}
//! 5_TILT_TRIGGER
bool scanTilt()
{
  return true;
}
int count = 0;
//! LOOP
void loop()
{
  server.handleClient();

  //? SENSOR READING
  if (measureInsideSensor.timeElapsed())
    getDataFromInside();

  if (measureOutsideSensor.timeElapsed())
    getDataFromOutside();

  //? BATTERY
  if (measureBatteryCharge.timeElapsed())
    battery.getCharge();

  //? MEASURE RPM
  if (measureRPM.timeElapsed() && fan.orederedPercent > 0)
    fanRPM=fan.getPeriodTime();
}
