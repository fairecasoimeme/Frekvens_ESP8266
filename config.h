#include <Arduino.h>

#define VERSION "v0.1a"

struct ConfigSettingsStruct
{
    String ssid;
    String password;
    String ipAddress;
    String ipMask;
    String ipGW;
    
};

struct ConfigPanel
{
    String text;
    String sizeText;
    String scrollText;
    String xText;
    String yText;
    String light;
    
};

IPAddress parse_ip_address(const char *str);
bool loadConfig();
bool loadConfigPanel();
