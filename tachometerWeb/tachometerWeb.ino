#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "headShield";
const char *password = "123456789";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

#define inputPin 39
#define outputPin 5

const char *webpageCode = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Test Page</title>
    <script>
        function fetchValue() {
            fetch('/value')
            .then(response => response.text())
            .then(value => {
                document.getElementById('analogValue').innerText = "Analog value: " + value;
            });
        }

        setInterval(fetchValue, 1000);
    </script>
</head>
<body>
    <h1>Hello World</h1>
    <p id="analogValue">Analog value: </p>
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

void setup()
{
    pinMode(inputPin, INPUT);
    pinMode(outputPin, OUTPUT);

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    server.on("/", handle_root);
    server.on("/value", handle_value);
    server.begin();
}

void loop()
{
    server.handleClient();
}
