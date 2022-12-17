#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

class Config
{
public:
    void load();

    String get_admin_login();
    void set_admin_login(String value);

    String get_admin_password();
    void set_admin_password(String value);

    String get_ssid();
    void set_ssid(String value);

    String get_ap_password();
    void set_ap_password(String value);

    String get_ip_address();
    void set_ip_address(String value);

    String get_gateway();
    void set_gateway(String value);

    String get_subnet();
    void set_subnet(String value);

    String get_ipfs_node();
    void set_ipfs_node(String value);

private:
    String filename = "/config.json";
    const char *_admin_login;
    const char *_admin_password;
    const char *_ssid;
    const char *_ap_password;
    const char *_ip_addr;
    const char *_gateway;
    const char *_subnet;
    const char *_ipfs_node;

    void set(String key, String value);
};