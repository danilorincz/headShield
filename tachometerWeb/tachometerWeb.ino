#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32_AP";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

#define inputPin 39
#define outputPin 5
const size_t numValues = 1000;
int values[numValues];
size_t valueIndex = 0;

bool fanState = false;

const char *webpageCode = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Real-time Analog Value</title>
    <script>
        function fetchValues() {
            fetch('/values')
            .then(response => response.json())
            .then(values => {
                var list = document.getElementById('values');
                list.innerHTML = '';  // Clear the list
                for (var i = 0; i < values.length; i++) {
                    var li = document.createElement('li');
                    li.appendChild(document.createTextNode('Value: ' + values[i]));
                    list.appendChild(li);
                }
            });
        }

        function toggleFan() {
            fetch('/toggleFan');
        }

        setInterval(fetchValues, 5000);  // Fetch values every 5 seconds
    </script>
</head>
<body>
    <h1>ESP32 Real-time Analog Value</h1>
    <button onclick="toggleFan()">Toggle Fan</button>
    <ul id="values"></ul>
</body>
</html>
)rawliteral";

void handle_root()
{
    server.send_P(200, "text/html", webpageCode);
}

void handle_value()
{
    server.send(200, "text/plain", String(analogRead(inputPin)).c_str());
}

void handle_toggleFan()
{
    fanState = !fanState;
    digitalWrite(outputPin, fanState ? HIGH : LOW);
    server.send(200, "text/plain", "Toggled");
}

void handle_values()
{
    String response = "[";
    for (size_t i = 0; i < numValues; i++)
    {
        response += String(values[i]);
        if (i < numValues - 1)
        {
            response += ",";
        }
    }
    response += "]";
    server.send(200, "application/json", response);
}

void setup()
{
    pinMode(inputPin, INPUT);
    pinMode(outputPin, OUTPUT);

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    server.on("/", handle_root);
    server.on("/value", handle_value);
    server.on("/toggleFan", handle_toggleFan);
    server.on("/values", handle_values);
    delay(100);
    server.begin();
    delay(100);
}

void loop()
{
    values[valueIndex] = analogRead(inputPin);
    valueIndex = (valueIndex + 1) % numValues;

    server.handleClient();
}
