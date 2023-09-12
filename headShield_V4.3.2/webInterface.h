

void handle_root(AsyncWebServerRequest *request)
{
    request->send_P(200, "text/html", webpageCode);
}

void handle_getData(AsyncWebServerRequest *request)
{
    StaticJsonDocument<200> doc;
    doc["state"] = fanErrorNumber;
    doc["battery"] = batteryPercentage;
    doc["temp"] = perkData.temp;
    doc["press"] = perkData.press;
    doc["humi"] = perkData.humi;
    doc["AQI"] = perkData.AQI;
    doc["TVOC"] = perkData.TVOC;
    doc["ECO2"] = perkData.ECO2;
    doc["filterTime"] = millisToTimeString(filterTracker.get_timeOn());
    doc["warningSystemStatus"] = tacho.warning;
    String jsonData;
    serializeJson(doc, jsonData);
    request->send(200, "application/json", jsonData);
}

void handle_restart(AsyncWebServerRequest *request)
{
    filterTracker.clearLatestData();
    Serial.println("RESET FILTER");
    request->send_P(200, "text/plain", "OK");
}
void handle_toggleWarning(AsyncWebServerRequest *request)
{
    tacho.warning = !tacho.warning;
    request->send(200, "text/plain", "OK");
}

void serverOn()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { handle_root(request); });
    server.on("/helmetData", HTTP_GET, [](AsyncWebServerRequest *request)
              { handle_getData(request); });
    server.on("/resetTime", HTTP_POST, [](AsyncWebServerRequest *request)
              { handle_restart(request); });
    server.on("/toggleWarning", HTTP_POST, [](AsyncWebServerRequest *request)
              { handle_toggleWarning(request); });
}
