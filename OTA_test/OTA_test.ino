#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const char *ssid = "headShield_3";
const char *password = "123456789";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

const char *webpageCode = R"rawliteral(
<!DOCTYPE html>
<html>
<body>
Hello World
</body>
</html>
)rawliteral";

void handle_root(AsyncWebServerRequest *request)
{
  request->send_P(200, "text/html", webpageCode);
}

void setup()
{
  Serial.begin(115200);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);

  Serial.println("Server started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", HTTP_GET, handle_root);
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
}

void loop()
{
  AsyncElegantOTA.loop();
}
