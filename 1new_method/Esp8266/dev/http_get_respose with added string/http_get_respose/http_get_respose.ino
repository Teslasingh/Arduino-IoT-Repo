/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  Code compatible with ESP8266 Boards Version 3.0.0 or above 
  (see in Tools > Boards > Boards Manager > ESP8266)
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
const char* ssid = "TES2";
const char* password = "tesla458266";

//Your Domain name with URL path or IP address with path
String serverName = "http://worldtimeapi.org/api/timezone/Asia/Kolkata";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath);
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);

        StaticJsonDocument<768> doc;

DeserializationError error = deserializeJson(doc, payload);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* abbreviation = doc["abbreviation"]; // "IST"
const char* client_ip = doc["client_ip"]; // "14.98.115.2"
const char* datetime = doc["datetime"]; // "2021-12-24T12:00:42.170530+05:30"
int day_of_week = doc["day_of_week"]; // 5
int day_of_year = doc["day_of_year"]; // 358
bool dst = doc["dst"]; // false
// doc["dst_from"] is null
int dst_offset = doc["dst_offset"]; // 0
// doc["dst_until"] is null
int raw_offset = doc["raw_offset"]; // 19800
const char* timezone = doc["timezone"]; // "Asia/Kolkata"
long unixtime = doc["unixtime"]; // 1640327442
const char* utc_datetime = doc["utc_datetime"]; // "2021-12-24T06:30:42.170530+00:00"
const char* utc_offset = doc["utc_offset"]; // "+05:30"
int week_number = doc["week_number"]; // 51

Serial.println(datetime);
String  one=datetime;
String timet = one.substring(11,16);
Serial.println(timet);

        
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
