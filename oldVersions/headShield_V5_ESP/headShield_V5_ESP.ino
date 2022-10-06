// HEADSHIELD_V11 PCB_V5

//? LIBRARIES
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <analogWrite.h>
#include <pitches.h>
#include <Tone32.h>
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V5_ESP\webpage.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V5_ESP\RGB.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V5_ESP\timer.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V5_ESP\fan.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V5_ESP\beeper.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V5_ESP\battery.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V5_ESP\tachometer.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_V5_ESP\reed.h"

//? WIFI
const char *ssid = "headShield";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

//? RGB
const int RGB_redPin = 22;
const int RGB_greenPin = 21;
const int RGB_bluePin = 19;
RGB led(RGB_redPin, RGB_greenPin, RGB_bluePin);

//? FAN
const int fanPin = 15;
const int tachoPin1 = 5;
const int tachoPin2 = 17;
const int tachoPin3 = 16;
Fan fan(fanPin);
Tachometer tacho1(tachoPin1);
Tachometer tacho2(tachoPin2);
Tachometer tacho3(tachoPin3);

//? IR
const int infraReadPin = 34;

//? REED SWITCH
const reedSwitchPin1 = 23;
const reedSwitchPin2 = 18;
ReedSwitch reedFront(reedSwitchPin1);
ReedSwitch reedBack(reedSwitchPin2);

//? PIEZO
const int piezoPin = 32;
const int piezoChannel = 8;
Beeper beeper(piezoPin, piezoChannel);

//? BATTERY
const int batteryPin = 33;
Battery battery(batteryPin);

//? TIMERS
Timer measureBatteryCharge(10000000); // 10 sec

//? GLOBAL VARIABLES
unsigned long int millisecondsWhenStart = 0;
bool activationState = false;

void setup()
{
  Serial.begin(115200);

  fan.begin();
  tacho1.begin();
  tacho2.begin();
  tacho3.begin();
  battery.begin();
  led.begin();
  beeper.begin();
  reedFront.begin();
  reedBack.begin();

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  //! HANDLERS_SETUP
  server.on("/", handle_root);

  server.on("/readSinceStart", handle_readSinceStart);
  server.on("/readCharge", handle_readCharge);
  server.on("/readRPM", handle_readRPM);

  server.on("/slider", handle_setSpeed);
  server.on("/led_set", handle_led);
  server.begin();

  //! BOOT
  beeper.boot();
  led.green();
  millisecondsWhenStart = millis();
}

//! 1_HANDLERS
void handle_root()
{
  server.send(200, "text/html", webpageCode);
}
void handle_readCharge()
{
  server.send(200, "text/plane", (String)battery.chargePercent);
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

//! LOOP
void loop()
{
  server.handleClient();
}
