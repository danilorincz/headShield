

void handle_root()
{
    server.send_P(200, "text/html", webpageCode);
}
void handle_getData()
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
    doc["memoryHealth"] = String(filterTracker.get_memoryHealth());
    String jsonData;
    serializeJson(doc, jsonData);

    server.send(200, "application/json", jsonData);
}

void handle_restart()
{
    filterTracker.clearLatestData();
    Serial.println("RESET FILTER");
    server.send_P(200, "text/plain", "OK");
}
void handle_toggleWarning()
{
    tacho.warning = !tacho.warning;
    server.send(200, "text/plain", "OK");
}
void serverOn()
{
    server.on("/", handle_root);
    server.on("/helmetData", handle_getData);
    server.on("/resetTime", HTTP_POST, handle_restart);
    server.on("/toggleWarning", HTTP_POST, handle_toggleWarning);
}