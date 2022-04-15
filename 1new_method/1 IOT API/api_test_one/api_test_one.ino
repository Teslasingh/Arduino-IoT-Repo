/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  Code compatible with ESP8266 Boards Version 3.0.0 or above 
  (see in Tools > Boards > Boards Manager > ESP8266)
*/
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>;

const char* ssid = "TES2";
const char* password = "tesla458266";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://152.44.45.247:3000/users/login";

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
  //Send an HTTP POST request every 10 minutes
  String eauth ="kissamsingh@gmail.com";
  String epassa ="458266458266";
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/json");
      // Data to send with HTTP POST
      String httpRequestData = "{\"email\":\""+eauth+"\",\"password\":\""+epassa+"\"}";          
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); 
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
      Serial.printf("[HTTP] GET... code: %d\n", httpResponseCode);
    //  Serial.print("HTTP Response code: ");
     // Serial.println(httpResponseCode);
      Serial.println(payload);
      Serial.println(httpRequestData);
       StaticJsonDocument<512> doc;

DeserializationError error = deserializeJson(doc, payload);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject user = doc["user"];
const char* user_id = user["_id"]; // "61a99c760d2cef9e3a5d0d2c"
String user_country = user["country"]; // "India"
const char* user_email = user["email"]; // "kissamsingh@gmail.com"
const char* user_createdAt = user["createdAt"]; // "2021-12-03T04:26:30.232Z"
const char* user_updatedAt = user["updatedAt"]; // "2021-12-06T11:56:23.633Z"
int user_v = user["__v"]; // 1640

const char* token = doc["token"];
Serial.println(user_country);
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
