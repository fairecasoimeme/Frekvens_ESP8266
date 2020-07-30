#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "config.h"
#include <ArduinoJson.h>

ConfigSettingsStruct ConfigSettings;
ConfigPanel cfgPanel;

IPAddress parse_ip_address(const char *str) {
    IPAddress result;    
    int index = 0;

    result[0] = 0;
    while (*str) {
        if (isdigit((unsigned char)*str)) {
            result[index] *= 10;
            result[index] += *str - '0';
        } else {
            index++;
            if(index<4) {
              result[index] = 0;
            }
        }
        str++;
    }
    
    return result;
}

bool loadConfig() {
  File configFile = SPIFFS.open("/config/config.json", "r+");
  if (!configFile) {
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  //StaticJsonDocument<512> jsonBuffer;
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, buf.get());


  if (doc.isNull()) {
    return false;
  }

  char ssid_[30];
  strcpy(ssid_, doc["ssid"]);
  ConfigSettings.ssid = String(ssid_);
  char pass_[30];
  strcpy(pass_, doc["pass"]);
  ConfigSettings.password = String(pass_);
  char ip_[30];
  strcpy(ip_, doc["ip"]);
  ConfigSettings.ipAddress = String(ip_);
  char mask_[30];
  strcpy(mask_, doc["mask"]);
  ConfigSettings.ipMask = String(mask_);
  char gw_[30];
  strcpy(gw_, doc["gw"]);
  ConfigSettings.ipGW = String(gw_);
  
  configFile.close();
    
    return true;
}

bool loadConfigPanel() {
  File panelFile = SPIFFS.open("/config/panel.json", "r+");
  if (!panelFile) {
    return false;
  }

  size_t size = panelFile.size();
  if (size > 1024) {
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  panelFile.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, buf.get());


  if (doc.isNull()) {
    return false;
  }
  
  char txt_[512];
  strcpy(txt_, doc["text"]);
  cfgPanel.text = String(txt_);
  char sz_[2];
  strcpy(sz_, doc["sizeText"]);
  cfgPanel.sizeText = String(sz_);
  char scroll_[2];
  strcpy(scroll_, doc["scrollText"]);
  cfgPanel.scrollText = String(scroll_);
  char x_[2];
  strcpy(x_, doc["xText"]);
  cfgPanel.xText = String(x_);
  char y_[2];
  strcpy(y_, doc["yText"]);
  cfgPanel.yText = String(y_);
  char light_[6];
  strcpy(light_, doc["light"]);
  cfgPanel.light = String(light_);

    panelFile.close();
   
    return true;
}
