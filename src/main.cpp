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

#define APPLICATION_JSON "application/json"

AsyncWebServer server(80);
Display display = Display();

IPAddress local_ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const char *ssid = "OpenWRT-WIFI-2.4G";
const char *password = "FtYpp86z";
const char *www_username = "admin";
const char *www_password = "admin";

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void initWiFi()
{
  // WiFi.mode(WIFI_STA);
  WiFi.mode(WIFI_AP_STA);
  // WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void handle_NotFound(AsyncWebServerRequest *request)
{
  display.setRequestInfo("handle_NotFound()");
  String path = request->pathArg(0);
  request->send(404, APPLICATION_JSON, "{\"error\": \"Error 404: " + path + " not found\"}");
}

void handleLogin(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  AsyncWebServerResponse *resp = request->beginResponse(401, APPLICATION_JSON);

  if (!index)
  {
    Serial.printf("BodyStart: %u B\n", total);
  }
  for (size_t i = 0; i < len; i++)
  {
    Serial.write(data[i]);
  }
  if (index + len == total)
  {
    Serial.printf("BodyEnd: %u B\n", total);
  }

  Serial.println("Handle login");
  String msg;
  if (request->hasHeader("Cookie"))
  {
    // Print cookies
    Serial.print("Found cookie: ");
    String cookie = request->header("Cookie");
    Serial.println(cookie);
  }
  if (request->hasArg("username") && request->hasArg("password"))
  {
    Serial.print("Found parameter: ");
    if (request->arg("username") == String(www_username) && request->arg("password") == String(www_password))
    {

      resp->addHeader("Cache-Control", "no-cache");

      Serial.println("getLocalAddress: " + String(request->client()->localIP()));

      String token = sha1(String(www_username) + ":" + String(www_password) + ":" + String(request->client()->getLocalAddress()));

      resp->addHeader("Set-Cookie", "ESPSESSIONID=" + token);
      request->send(resp);
      Serial.println("Log in Successful. New TOKEN: ESPSESSIONID=" + token);
      return;
    }
    Serial.println("Log in Failed");
    resp->addHeader("Cache-Control", "no-cache");
    request->send(resp);
    return;
  }
  else
  {
    Serial.println("There arent arguments username & password!");
    request->send(401, "application/json", "{\"error\":\"Set username & password!\"}");
    return;
  }
}

void handleLogout(AsyncWebServerRequest *request)
{

  AsyncWebServerResponse *resp = request->beginResponse(301);

  Serial.println("Disconnection");
  resp->addHeader("Cache-Control", "no-cache");
  resp->addHeader("Set-Cookie", "ESPSESSIONID=0");
  request->send(resp);
  return;
}

void setup()
{

  Serial.begin(115200);

  initWiFi();

  Serial.println(F("Inizializing FS..."));
  if (!SPIFFS.begin())
  {
    if (SPIFFS.format())
      Serial.println(("WARN", "sys", F("Filesystem formatted"), ""));
    else
      Serial.println(F("Filesystem couldn't be formatted"));
  }
  else
    Serial.print(F("FS DONE"));

  display.init();
  display.setStateInfo(WiFi.localIP().toString(), WiFi.macAddress());

  server.on(
      "/login", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleLogin);
  server.on("/logout", HTTP_GET, handleLogout);

  /* server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/www/index.html", "text/html"); });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/www/style.css", "text/css"); });
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/www/index.js", "text/javascript"); }); */

  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");

  server.onNotFound(handle_NotFound);

  server.begin();

  Serial.println(F("[ INFO ] ESP v"));
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Serial.println("Flash ide  size: " + String(ideSize));
  Serial.println("Flash ide speed: " + String(ESP.getFlashChipSpeed()));
  Serial.println("MD5: " + ESP.getSketchMD5());
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

      http.addHeader("Content-type", "application/json");
      int httpResponseCode = http.POST("{ \"jsonrpc\": \"2.0\",\"id\": \"dontcare\", \"method\": \"query\", \"params\": {\"request_type\": \"view_account\",\"finality\":\"final\",\"account_id\":\"ruziev.poolv1.near\"}}");

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
