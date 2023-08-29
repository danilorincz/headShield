Problems in the code
//* create a method to turn off the fans completely
//* long touch input registers tap first
//* headSensor optimized
// the sensor must be connected automatically
// optimize the fan speed reading
// do a touch calibration before every start
// do a tachometer calibration
// battery reading

#include <WiFi.h>
#include <WebServer.h>

    //? WIFI
    const char *ssid = "headShield_233";
const char *password = "123456788";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

void setup()
{
    //* BEGIN
    Serial.begin(115200);

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);
    WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
    //* HANDLERS
    // ROOT HANDLERS
    server.on("/", handle_root);

    server.begin();
    delay(100);
}

const char *webpageCode = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Helmet Interface</title>
<style>
  body {font-family: Arial, Helvetica, sans-serif;}
  ul {list-style-type: none; margin: 0; padding: 0; overflow: hidden; background-color: #333;}
  li {float: left;}
  li a {display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none;}
  li a:hover {background-color: #111;}
</style>
</head>
<body>
  <ul>
    <li><a href="/helmetData">Helmet Data</a></li>
    <li><a href="/sensorData">Sensor Data</a></li>
    <li><a href="/control">Control</a></li>
    <li><a href="/debugData">Debug Data</a></li>
  </ul>
</body>
</html>
)rawliteral";

void handle_root()
{
    server.send_P(200, "text/html", webpageCode);
}

void loop()
{
    server.handleClient();
}
