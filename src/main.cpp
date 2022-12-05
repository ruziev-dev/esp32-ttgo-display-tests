#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "Display.h"

#define JSON_CONFIG_FILE "/test_config.json"

WebServer server(80);
Display display = Display();

const char *ssid = "OpenWRT-WIFI-2.4G";
const char *password = "FtYpp86z";
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

String SendHTML(String data)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>LED Control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP32 Web Server</h1>\n";
  ptr += "<h3>Using Access Point(AP) Mode</h3>\n";
  ptr += data;
  ptr += "<a class=\"button button-on\" href=\"/led1on\">ON</a>";
  ptr += "<a class=\"button button-off\" href=\"/led1on\">OFF</a>";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void handle_OnConnect()
{
  Serial.println("handle_OnConnect");
  display.setRequestInfo("handle_OnConnect()");
  server.send(200, "text/html", SendHTML("handle_OnConnect"));
}

void handle_info()
{
  Serial.println("handle_info");
  display.setRequestInfo("handle_info()");
  server.send(200, "text/html", SendHTML("handle_info"));
}

void handle_NotFound()
{
  display.setRequestInfo("handle_NotFound()");
  server.send(404, "text/plain", "Not found");
}

void setup()
{

  // WiFi.mode(WIFI_AP_STA);

  Serial.begin(115200);
  Serial.println("It Works!");
  initWiFi();

  display.init();
  display.setStateInfo(WiFi.localIP().toString(), WiFi.macAddress());

  server.on("/", handle_OnConnect);
  server.on("/info", handle_info);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("MD5: " + ESP.getSketchMD5());
}

void loop()
{

  server.handleClient();

  if ((millis() - previousTime) > 5000)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;

      String serverPath = "https://rpc.mainnet.near.org";

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());

      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    previousTime = millis();
  }
}
