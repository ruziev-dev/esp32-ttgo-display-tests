#include "Config.h"

const char READ_FILE_ERROR[] PROGMEM = "Failed to read file, using default configuration";

void Config::load()
{

    File file = SPIFFS.open(filename);

    StaticJsonDocument<4096> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, file);

    if (error)
        Serial.println(READ_FILE_ERROR);

    this->_admin_login = jsonDoc["admin_login"].as<char *>();
    this->_admin_password = jsonDoc["admin_password"].as<char *>();
    this->_ssid = jsonDoc["ssid"].as<char *>();
    this->_ap_password = jsonDoc["ap_password"].as<char *>();
    this->_ip_addr = jsonDoc["ip_addr"].as<char *>();
    this->_gateway = jsonDoc["gateway"].as<char *>();
    this->_subnet = jsonDoc["subnet"].as<char *>();
    this->_ipfs_node = jsonDoc["ipfs_node"].as<char *>();

    file.close();
}

void Config::set(String key, String value)
{
    File file = SPIFFS.open(filename, "w");

    StaticJsonDocument<2048> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, file);
    if (error)
        Serial.println(READ_FILE_ERROR);

    jsonDoc[key] = value;
    if (serializeJson(jsonDoc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }
    file.close();
    this->load();
}

String Config::get_admin_login()
{
    return String(this->_admin_login);
}
void Config::set_admin_login(String value)
{
    this->set("admin_login", value.c_str());
}

String Config::get_admin_password()
{
    return String(this->_admin_password);
}
void Config::set_admin_password(String value)
{
    this->set("admin_password", value);
}

String Config::get_ssid()
{
    return String(this->_ssid);
}
void Config::set_ssid(String value)
{
    this->set("ssid", value);
}

String Config::get_ap_password()
{
    return String(this->_ap_password);
}
void Config::set_ap_password(String value)
{
    this->set("ap_password", value);
}

String Config::get_ip_address()
{
    return String(this->_ip_addr);
}
void Config::set_ip_address(String value)
{
    this->set("ip_addr", value);
}

String Config::get_gateway()
{
    return String(this->_gateway);
}
void Config::set_gateway(String value)
{
    this->set("gateway", value);
}

String Config::get_subnet()
{
    return String(this->_subnet);
}
void Config::set_subnet(String value)
{
    this->set("subnet", value);
}

String Config::get_ipfs_node()
{
    return String(this->_ipfs_node);
}
void Config::set_ipfs_node(String value)
{
    this->set("ipfs_node", value);
}