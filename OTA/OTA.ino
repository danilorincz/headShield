#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "Webpage.h"
#include <ArduinoJson.h>
const char *ssid = "headShield";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void setup()
{
  //* BEGIN
  Serial.begin(115200);

  //* WIFI
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, local_ip, subnet);
  WiFi.softAP(ssid, password);
  serverOn();
  server.begin();
  AsyncElegantOTA.begin(&server);
}

void handle_root(AsyncWebServerRequest *request)
{
  Serial.println("ROOT");
  request->send_P(200, "text/html", webpageCode);
}

void handle_getData(AsyncWebServerRequest *request)
{
  StaticJsonDocument<200> doc;
  doc["state"] = "test";
  doc["battery"] = "test";
  doc["temp"] = "test";
  doc["press"] = "test";
  doc["humi"] = "test";
  doc["AQI"] = "test";
  doc["TVOC"] = "test";
  doc["ECO2"] = "test";
  doc["filterTime"] = "test";
  doc["warningSystemStatus"] = "test";
  String jsonData;
  serializeJson(doc, jsonData);
  request->send(200, "application/json", jsonData);
}

void handle_restart(AsyncWebServerRequest *request)
{
  //Serial.println("RESET FILTER");
  request->send_P(200, "text/plain", "OK");
}
void handle_toggleWarning(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", "OK");
}

void serverOn()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    handle_root(request);
  });
  server.on("/helmetData", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    handle_getData(request);
  });
  server.on("/resetTime", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    handle_restart(request);
  });
  server.on("/toggleWarning", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    handle_toggleWarning(request);
  });
}

void loop()
{
  AsyncElegantOTA.loop();
  delay(10);
}
