#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include <HTTPClient.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "Display.h"
#include "utils.h"

#define JSON_CONFIG_FILE "/test_config.json"

AsyncWebServer server(80);
Display display = Display();

const char *ssid = "OpenWRT-WIFI-2.4G";
const char *password = "FtYpp86z";
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void initWiFi()
{
  // WiFi.mode(WIFI_STA);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void handle_OnConnect(AsyncWebServerRequest *request)
{
  Serial.println("handle_OnConnect");
  display.setRequestInfo("handle_OnConnect()");
  request->send(200, "text/html", SendHTML("handle_OnConnect"));
}

void handle_info(AsyncWebServerRequest *request)
{
  Serial.println("handle_info");
  display.setRequestInfo("handle_info()");
  request->send(200, "text/html", SendHTML("handle_info"));
}

void handle_NotFound(AsyncWebServerRequest *request)
{
  display.setRequestInfo("handle_NotFound()");
  request->send(404, "text/plain", "{\"error\": \"Error 404\"}");
}

void check_auth(AsyncWebServerRequest *request)
{
  //if (!request->authenticate(www_username, www_password))
  //  request->requestAuthentication();

  request->send(404, "text/plain", "{\"error\": \"Error 404\"}");
}

void setup()
{

  Serial.begin(115200);
  Serial.println("It Works!");
  initWiFi();

  display.init();
  display.setStateInfo(WiFi.localIP().toString(), WiFi.macAddress());

  server.on("/auth", check_auth);
  server.on("/", handle_OnConnect);
  server.on("/info", handle_info);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("MD5: " + ESP.getSketchMD5());

  Serial.print(F("Inizializing FS..."));
  if (SPIFFS.begin())
  {
    Serial.println(F("done."));
  }
  else
  {
    Serial.println(F("fail."));
  }
}

void loop()
{

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
