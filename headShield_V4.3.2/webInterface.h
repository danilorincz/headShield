

void handle_root()
{
    server.send_P(200, "text/html", webpageCode);
}
void handler_helmetData()
{
    server.send_P(200, "text/html", helmetDataPage);
}

void handler_getHelmetData()
{
    static Timer intervalTimer(1000);
    if (intervalTimer.timeElapsedMillis())
    {

        StaticJsonDocument<200> doc;

        doc["visorState"] = visor.state;
        doc["IRState"] = headSensor.state;
        doc["fanState"] = fan.state;
        doc["lampLevel"] = lamp.level;
        doc["batteryLevel"] = battery.percent;
        doc["audioState"] = audio.state;
        doc["fanRPM"] = fanErrorNumber;

        String jsonData;
        serializeJson(doc, jsonData);

        server.send(200, "application/json", jsonData);
    }
}


void handler_sensorData()
{
    server.send_P(200, "text/html", sensorDataPage);
}
/*
void handler_controlPage()
{
    server.send_P(200, "text/html", controlPage);
}
void handler_debugPage()
{
    server.send_P(200, "text/html", DEBUG_PAGE);
}
*/
void handler_getSensorData()
{
    StaticJsonDocument<200> doc;

    char tempStr[6]; // Buffer big enough for "-XX.X" and a null terminator
    char humiStr[6]; // Buffer big enough for "-XX.X" and a null terminator

    // Format temperature and humidity as strings with 1 decimal place
    dtostrf(perkData.temp, 5, 1, tempStr);
    dtostrf(perkData.humi, 5, 1, humiStr);

    doc["temp"] = tempStr;
    doc["press"] = perkData.press;
    doc["humi"] = humiStr;
    doc["AQI"] = perkData.AQI;
    doc["TVOC"] = perkData.TVOC;
    doc["ECO2"] = perkData.ECO2;
    doc["status"] = perkData.status;

    String jsonData;
    serializeJson(doc, jsonData);

    server.send(200, "application/json", jsonData);
}

/*
void handler_getDebugData()
{
    StaticJsonDocument<200> doc;
    doc["dummy1"] = touchRight.getAnalog();
    doc["dummy2"] = touchLeft.getAnalog();
    doc["dummy3"] = headSensor.read();
    String jsonData;
    serializeJson(doc, jsonData);
    server.send(200, "application/json", jsonData);
}

void handler_setLampLevel()
{
    int newLampLevel = server.arg("level").toInt();
    lamp.setLevel(newLampLevel);
    server.send(200, "text/plain", "Lamp level set");
}
void handler_setAudioState()
{
    String stateParam = server.arg("state"); // Get the state parameter from the request

    if (stateParam == "true")
    {
        audio.on();
    }
    else
    {
        audio.off();
    }

    server.send(200, "text/plain", "OK");
}

*/

void serverOn()
{
    server.on("/", handle_root);
    server.on("/helmetData", handler_helmetData);
    server.on("/getHelmetData", handler_getHelmetData);

    server.on("/sensorData", handler_sensorData);
    server.on("/getSensorData", handler_getSensorData);
    /*
    server.on("/control", handler_controlPage);
    server.on("/debugData", handler_debugPage);

    server.on("/debugdata", handler_getDebugData);

    server.on("/setLampLevel", handler_setLampLevel);
    server.on("/setAudioState", handler_setAudioState);*/
}