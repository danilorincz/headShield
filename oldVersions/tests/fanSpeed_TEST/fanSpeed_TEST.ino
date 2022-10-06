/*const int tachoPin = 5;
bool tachoVal = 0;
bool prevTachoVal = 0;
int counter = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(tachoPin, INPUT);
}

void loop()
{
  tachoVal = digitalRead(tachoPin);
  if (tachoVal != prevTachoVal)
  {
    Serial.print("CHANGE: ");
    Serial.println(counter);
    counter++;
    prevTachoVal = tachoVal;
  }
}
*/

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\webpage.h"
#include "C:\Users\user\Desktop\headShield\3_programming\headShield_ESP\timer.h"

#include <Arduino.h>
#include <analogWrite.h>


const int tachoPin = 5;
bool tachoVal = 0;
bool prevTachoVal = 0;
int counter = 0;

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
  pinMode(tachoPin, INPUT);


  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  //! HANDLERS_SETUP
  server.on("/", handle_root);
  //? USER STUFF

  server.on("/readInsidePressure", handle_readInsidePressure);

  server.begin();

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
  server.send(200, "text/plane", (String)counter);
}



void loop()
{
  server.handleClient();

  tachoVal = digitalRead(tachoPin);
  if (tachoVal != prevTachoVal)
  {
    Serial.print("CHANGE: ");
    Serial.println(counter);
    counter++;
    prevTachoVal = tachoVal;
  }
}
