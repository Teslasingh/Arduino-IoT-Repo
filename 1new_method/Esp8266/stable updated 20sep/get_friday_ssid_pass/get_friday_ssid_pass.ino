
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>;

String serverName = "http://152.44.45.247:5000/api/public/1";

int l1 = 13;
int fedback =12;
int overswitch=14;
int ledblue=5;
int ledgreen=4;
int setval = 0;
unsigned long inTime=0;
long interval=7000;
bool onetime=1;
boolean rest=0;
bool secpass=1;

int i = 0;
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;

bool testWifi(void);
void launchWeb(void);
void setupAP(void);

ESP8266WebServer server(80);
void setup() {
Serial.begin(115200); 
pinMode(l1,OUTPUT);
pinMode(ledblue,OUTPUT);
pinMode(ledgreen,OUTPUT);
pinMode(fedback,INPUT);
pinMode(overswitch,INPUT);

  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
 // pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println();
  Serial.println("Startup");

  //---------------------------------------- Read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");

  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");

  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);


  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
    Serial.println("Succesfully Connected!!!");
    return;
  }
  else
  {
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup HotSpot
  }

  Serial.println();
  Serial.println("Waiting.");
  
  while ((WiFi.status() != WL_CONNECTED))
  {
    if(setval==0){
    digitalWrite(ledblue,HIGH);
    digitalWrite(ledgreen,LOW);
  }
    Serial.print(".");
    //delay(100);
    server.handleClient();
    delay(100);
    if(rest==1){
    rest=0;
    delay(500);
    ESP.reset();
    }
       
      if (digitalRead(overswitch)==LOW && setval==0){
      setval=1;
      digitalWrite(l1,HIGH);
      digitalWrite(ledblue,LOW);
      delay(300);
  }
  else if(digitalRead(overswitch)==LOW && setval==1){
   setval=0;
   digitalWrite(l1,LOW);
    delay(300);
    }
    
  }

}


void loop() {
/// unsigned long currentTime=millis();
  if ((WiFi.status() == WL_CONNECTED))
  {
   // fun();
     digitalWrite(ledblue,LOW);
     Serial.println("wifi connected");
      WiFiClient client;
      HTTPClient http;
     http.begin(client, serverName);
    int httpResponseCode = http.GET();
    //fun();
     if (httpResponseCode>0) {
        delay(300);
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      StaticJsonDocument<96> doc;

      DeserializationError error = deserializeJson(doc, payload);

          if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
                 return;
                }
        int state = doc["state"];
        Serial.println(state);
      if(state==1){
        setval=1;
       digitalWrite(l1,HIGH);
       digitalWrite(ledgreen,LOW);
    }
    if(state==0){
      setval=0;
       digitalWrite(ledgreen,HIGH);
       digitalWrite(l1,LOW);
    }
     
    }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        fun();
   }
      http.end();
  }
else{
  fun();
Serial.println("wifi not connected");
 }
} 
// .............end of loop
void fun(void){
  unsigned long currentTime=millis();
        if (digitalRead(overswitch)==LOW){
       if(onetime==1){
         inTime=currentTime;
         onetime=0;
       }

        if(inTime + 3000UL < currentTime && inTime + 3000UL + 1500UL > currentTime){
            secpass=0;
        }

        if(inTime + interval < currentTime && inTime + interval + 1500UL > currentTime && secpass==0){
          Serial.println("reseting...................");
        for (int i = 0; i < 160; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.end();
        Serial.println("EEPROM reset");
        delay(500);
        ESP.reset();
          delay(600);
        }

     }
      if(inTime+interval+1500UL<currentTime){
        onetime=1;
        secpass=1;
      }
      Serial.print("onetime =");
      Serial.println(onetime);
      Serial.print("secpass =");
      Serial.println(secpass);
      



      if (digitalRead(overswitch)==LOW && setval==0){
      setval=1;
      digitalWrite(l1,HIGH);
      digitalWrite(ledgreen,LOW);
      delay(300);
       }
  else if(digitalRead(overswitch)==LOW && setval==1){
   setval=0;
   digitalWrite(l1,LOW);
   digitalWrite(ledgreen,HIGH);
    delay(300);
    }
}
//----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 40 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(1000);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("Socket", "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}

void createWebServer()
{
 {
    server.on("/", []() {

      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
             //ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
      Serial.println("should reset?");
      digitalWrite(ledblue,LOW);
      delay(1000);
      rest=1;
    });
  } 
}
