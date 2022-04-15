#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>;
const char* ssid = "Tesla";
const char* password = "458266458266"; 
void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
}
 
void loop() {
 
 if(WiFi.status()== WL_CONNECTED){
  WiFiClient client;
   HTTPClient http;   
 
   http.begin(client,"http://tesla001.ddns.net/d/e0302935-d3ce-4d47-9890-7e1334540f47/");
   http.addHeader("Content-Type", "application/json");
                String output = {"{\"state\": 0\"}"};
                int httpResponseCode = http.PUT(output);
                String response = http.getString();

                Serial.println(httpResponseCode);
                Serial.println(response);
                http.end();
 
  
 
   http.end();
 
 }else{
    Serial.println("Error in WiFi connection");
 }
 
  delay(5000);
}
