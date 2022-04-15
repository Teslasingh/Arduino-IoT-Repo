
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "Board not found"
#endif

#include "Arduino.h"

#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>


class credentials {
  public:
  bool credentials_get();
  void setupAP(char* softap_ssid, char* softap_pass);
  void server_loops();
  String EEPROM_Config();
  void Erase_eeprom();
  private:
  bool _testWifi(void);
  void _launchWeb(void);
  void _createWebServer(void);
  String ssid = "";
  String pass = "";
};
