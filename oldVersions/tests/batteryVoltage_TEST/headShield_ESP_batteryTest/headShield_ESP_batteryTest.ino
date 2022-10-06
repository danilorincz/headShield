
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\webpage.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\timer.h"
#include <Arduino.h>
#include <analogWrite.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <pitches.h>
#include <Tone32.h>
#include "DFRobot_BME680_SPI.h"
#include "SPI.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

Adafruit_MPU6050 mpu;
const int outsideSPI = 5;
const int insideSPI = 17;

#define CALIBRATE_PRESSURE 114
float insidePressure = 0;
float outsidePressure = 0;
String insidePressureString = "";
String outsidePressureString = "";
float pressureDifference = 0;
float seaLevel_1;
float seaLevel_2;

const char *ssid = "headShieldPremet";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

const int fanPin = 32;
//const int boardLedPin = 17;
const int ledPin = 15;
const int batteryPin = 39;
const int piezoPin = 33;

unsigned long int millisecondsWhenStart = 0;

//? GLOBAL VARIABLES
int fanSpeed = 0;
int prevFanSpeed = 0;
int prevPWMValue = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(fanPin, OUTPUT);
  //pinMode(boardLedPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(piezoPin, OUTPUT);
    adc1_config_width(ADC_WIDTH_11Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_6);
  /*
  if (!mpu.begin())
    Serial.println("MPU begin failed");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
*/
  //printMPUdata();
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", handle_root);

  //? PRESSURE

  server.on("/readInsidePressure", handle_readInsidePressure);
  server.on("/readOutsidePressure", handle_readOutsidePressure);
  server.on("/readDeltaPressure", handle_readDeltaPressure);

  //? USER DATA
  server.on("/readCharge", handle_readCharge);
  server.on("/readSinceStart", handle_readSinceStart);
  server.on("/readFanSpeed", handle_readFanSpeed);

  //? CONTROL PANEL
  server.on("/led_set", handle_led);
  server.on("/slider", handle_setSpeed);

  server.begin();
  bootSound();
  millisecondsWhenStart = millis();
}
void bootSound()
{
  tone(piezoPin, NOTE_A4, 100, 0);
  noTone(piezoPin, 0);
  tone(piezoPin, NOTE_B4, 100, 0);
  noTone(piezoPin, 0);
  tone(piezoPin, NOTE_C4, 100, 0);
  noTone(piezoPin, 0);
  tone(piezoPin, NOTE_D4, 100, 0);
  noTone(piezoPin, 0);
  tone(piezoPin, NOTE_E4, 100, 0);
  noTone(piezoPin, 0);
  tone(piezoPin, NOTE_F4, 100, 0);
  noTone(piezoPin, 0);
}
void printMPUdata()
{
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange())
  {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void handle_readFanSpeed()
{
  server.send(200, "text/plane", (String)fanSpeed);
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

void handle_readInsidePressure()
{
  server.send(200, "text/plane", insidePressureString);
}
void handle_readOutsidePressure()
{
  server.send(200, "text/plane", outsidePressureString);
}

void handle_readDeltaPressure()
{

  server.send(200, "text/plane", (String)pressureDifference);
}

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
void handle_root()
{
  server.send(200, "text/html", webpageCode);
}

void handle_readCharge()
{ /*
  int lowCounter = 0;
  int mediumCounter = 0;
  int highCounter = 0;
  for (int i = 0; i < 10; i++)
  {
    String batChargeRawString = String(analogRead(batteryPin));
    int batChargeRaw = batChargeRawString.toInt();
    int batChargeInt = map(batChargeRaw, 2925, 4096, 0, 100);

    if (batChargeInt > 1 && batChargeInt < 33)
      lowCounter++;
    if (batChargeInt >= 33 && batChargeInt < 66)
      mediumCounter++;
    if (batChargeInt >= 66 && batChargeInt <= 100)
      highCounter++;
  }

  String batCharge;
  if (lowCounter > mediumCounter && lowCounter > highCounter)
    batCharge = "LOW";
  if (mediumCounter > lowCounter && mediumCounter > highCounter)
    batCharge = "MEDIUM";
  if (highCounter > lowCounter && highCounter > mediumCounter)
    batCharge = "HIGH";
*/
  int batteryVoltage_test = adc1_get_raw(ADC1_CHANNEL_3);
  Serial.println(batteryVoltage_test);
  String batCharge = (String)batteryVoltage_test;
  server.send(200, "text/plane", batCharge);
}

void measureInsidePressure()
{
  DFRobot_BME680_SPI insideSensor(insideSPI);
  insideSensor.begin();
  insideSensor.startConvert();
  insideSensor.update();
  insidePressure = insideSensor.readPressure();
  insidePressureString = (String)insidePressure;
}
void measureOutsidePressure()
{
  DFRobot_BME680_SPI outsideSensor(outsideSPI);
  outsideSensor.begin();
  outsideSensor.startConvert();
  outsideSensor.update();
  outsidePressure = outsideSensor.readPressure() - 25;
  outsidePressureString = (String)outsidePressure;
}

int fanSpeedCorrection(int deltaPressure)
{
  int deltaMin = 30;  // nyomás
  int speedMax = 100; // sebesség

  int deltaMax = 150; // nyomás
  int speedMin = 30;  // sebesség

  int returnValue;
  if (deltaPressure < deltaMin)
  {
    Serial.println("First if");
    setFanSpeed(speedMax);

    returnValue = speedMax;
  }
  else if (deltaPressure > deltaMax)
  {
    Serial.println("Else if");
    setFanSpeed(speedMin);
    returnValue = speedMin;
  }
  else
  {
    Serial.println("Else");
    int period = 10;
    int pressureResolution = (deltaMax - deltaMin) / period;
    int speedResolution = (speedMax - speedMin) / period;

    int speedValues[10];
    int pressureValues[10];

    int finalSpeedValue;
    for (int i = 0; i < 10; i++)
    {
      speedValues[i] = speedMin + i * speedResolution;
      pressureValues[i] = deltaMax - i * pressureResolution;
    }
    for (int i = 0; i < 9; i++)
    {
      if (deltaPressure < pressureValues[i] &&
          deltaPressure > pressureValues[i + 1])
      {
        finalSpeedValue = speedValues[i];
        returnValue = finalSpeedValue;
      }
    }
    setFanSpeed(finalSpeedValue);
  }
  Serial.print("Final speed value: ");
  Serial.println(returnValue);
  Serial.print("Delta pressure value: ");
  Serial.println(deltaPressure);
  return returnValue;
}

void setFanSpeed(int percent)
{
  fanSpeed = percent;
  int PWMvalue = map(percent, 0, 100, 0, 255);

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

void printGyroData()
{
  /* Get new sensor events*/
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /*Acceleration*/
  Serial.print("X: ");
  Serial.print(a.acceleration.x);
  Serial.print("Y: ");
  Serial.println(a.acceleration.y);
  Serial.print("Z: ");
  Serial.println(a.acceleration.z);
  Serial.println(" m/s^2");

  /*Rotation*/
  Serial.print("X: ");
  Serial.print(g.gyro.x);
  Serial.print("Y: ");
  Serial.print(g.gyro.y);
  Serial.print("Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  /*Temperature*/
  Serial.print(temp.temperature);
  Serial.println(" C");

  Serial.println("");
  delay(500);
}
void loop()
{
  server.handleClient();
  /*
  measureOutsidePressure();
  measureInsidePressure();
  pressureDifference = insidePressure - outsidePressure;
  fanSpeedCorrection(pressureDifference);*/
}
