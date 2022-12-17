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
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include "Display.h"
#include "utils.h"
#include "Config.h"

Config config;
AsyncWebServer server(80);
Display display = Display();

IPAddress local_ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const char *ssid = "OpenWRT-WIFI-2.4G";
const char *password = "FtYpp86z";

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
  request->send(404);
}

void getDeviceInfo(AsyncWebServerRequest *request)
{
  AsyncJsonResponse *response = new AsyncJsonResponse();
  JsonVariant &root = response->getRoot();

  root["heap"] = ESP.getFreeHeap();
  root["ssid"] = WiFi.SSID();
  root["chip"] = ESP.getChipModel();
  root["ssid"] = config.get_ssid();
  root["ip"] = config.get_ip_address();
  root["subnet"] = config.get_subnet();
  root["cpu_freq"] = ESP.getCpuFreqMHz();

  response->setLength();
  request->send(response);
}

void handleLogin(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{

  String body = "";

  for (size_t i = 0; i < len; i++)
    body += char(data[i]);

  StaticJsonDocument<512> doc;
  deserializeJson(doc, body);
  DeserializationError err = deserializeJson(doc, body);

  String username = doc["username"];
  String password = doc["password"];

  String msg;

  AsyncJsonResponse *response = new AsyncJsonResponse();
  JsonVariant &resp = response->getRoot();
  response->setCode(201);
  if (request->hasHeader("Cookie"))
  {
    Serial.print("Found cookie: ");
    String cookie = request->header("Cookie");
    Serial.println(cookie);
    // TODO: add check SESSIONID with list
    resp["status"] = "Already logged in";
    response->setLength();
    request->send(response);
    return;
  }

  if (username == config.get_admin_login() && password == config.get_admin_password())
  {

    Serial.println("getLocalAddress: " + String(request->client()->localIP()));

    String token = sha1(String(www_username) + ":" + String(www_password) + ":" + String(request->client()->getLocalAddress()));

    response->addHeader("Set-Cookie", "ESPSESSIONID=" + token);
    resp["status"] = "Ok";
    response->setLength();
    request->send(response);
    Serial.println("Log in Successful. New TOKEN: ESPSESSIONID=" + token);
    return;
  }
  Serial.println("Log in Failed");
  request->send(401);
  return;
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

  config.load();
  Serial.println("CONFIG DATA: " + config.get_admin_login());

  display.init();
  display.setStateInfo(WiFi.localIP().toString(), WiFi.macAddress());

  server.on(
      "/login", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleLogin);
  server.on("/logout", HTTP_GET, handleLogout);
  server.on("/dev", HTTP_GET, getDeviceInfo);

  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");

  server.onNotFound(handle_NotFound);
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "http://localhost:3000");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

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
