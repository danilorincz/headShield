

void handle_root()
{
    server.send_P(200, "text/html", webpageCode);
}
void handle_getData()
{
    StaticJsonDocument<200> doc;
    doc["state"] = fanErrorNumber;
    doc["battery"] =   battery.percent;
    doc["temp"] = perkData.temp;
    doc["press"] = perkData.press;
    doc["humi"] = perkData.humi;
    doc["AQI"] = perkData.AQI;
    doc["TVOC"] = perkData.TVOC;
    doc["ECO2"] = perkData.ECO2;
    doc["filterTime"] = millisToTimeString(filterTracker.get_timeOn());
    String jsonData;
    serializeJson(doc, jsonData);

    server.send(200, "application/json", jsonData);
}

void serverOn()
{
    server.on("/", handle_root);
    server.on("/helmetData", handle_getData);
}