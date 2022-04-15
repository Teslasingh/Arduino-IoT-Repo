#include "ArduinoJson.h"
 
void setup() {
 
  Serial.begin(115200);
  Serial.println();
 
}
 
void loop() {
 
  Serial.println("Parsing start: ");
 
  char JSONMessage[] = " {\"SensorType\": \"Temperature\", \"Value\": 10, \"name\":\"aks\"}"; //Original message
 
  StaticJsonDocument<300> JSONdocument;                         //Memory pool
  DeserializationError error = deserializeJson(JSONdocument, JSONMessage); //Parse message
 
  if (error) {   //Check for errors in parsing
 
    Serial.println("Parsing failed");
    delay(5000);
    return;
 
  }
 
  const char * sensorType = JSONdocument["SensorType"]; //Get sensor type value
  int value = JSONdocument["Value"];                    //Get value of sensor measurement
  Serial.println(JSONMessage);
  Serial.println();
  Serial.print("Sensor type: ");
  Serial.println(sensorType);
  Serial.print("Sensor value: ");
  Serial.println(value);
 
  Serial.println();
  delay(5000);
 
}
