
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>;

int l1 = 13;
int fedback = 12;
int overswitch = 14;
int ledblue = 5;
int ledgreen = 4;
int setval = 0;
unsigned long inTime = 0;
long interval = 7000;
bool onetime = 1;
boolean rest = 0;
bool secpass = 1;

int i = 0;
int statusCode;
const char *ssid = "text";
const char *passphrase = "text";
String st;
String content;

bool testWifi(void);
void launchWeb(void);
void setupAP(void);
String esid;
String eauths;
ESP8266WebServer server(80);
void setup()
{
  Serial.begin(115200);
  pinMode(l1, OUTPUT);
  pinMode(ledblue, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(fedback, INPUT);
  pinMode(overswitch, INPUT);

  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); // Initialasing EEPROM
  delay(10);
  // pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println();
  Serial.println("Startup");

  //---------------------------------------- Read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");

  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");

  String epass = "";
  for (int i = 32; i < 64; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);

  String eauth = "";
  for (int i = 64; i < 128; ++i)
  {
    eauth += char(EEPROM.read(i));
  }
  Serial.print("AUTH: ");
  Serial.println(eauth);
  eauths = eauth.c_str();
  Serial.println(eauths);

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
    setupAP(); // Setup HotSpot
  }

  Serial.println();
  Serial.println("Waiting.");

  while ((WiFi.status() != WL_CONNECTED))
  {
    if (setval == 0)
    {
      digitalWrite(ledblue, HIGH);
      digitalWrite(ledgreen, LOW);
    }
    Serial.print(".");
    // delay(100);
    server.handleClient();
    delay(100);
    if (rest == 1)
    {
      rest = 0;
      delay(200);
      ESP.reset();
    }

    /*    if (digitalRead(overswitch)==LOW && setval==0){
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
      */
  }
}

void loop()
{

  if ((WiFi.status() == WL_CONNECTED))
  {
    delay(200);
    Serial.println("//////////////////////////////////////////////////////////////////////////////////");
    digitalWrite(ledblue, LOW);
    Serial.println("wifi connected");
    WiFiClient client;
    HTTPClient http;
    String server = "http://152.44.45.247:5001/api/public/";
    String serverName = server + eauths;
    Serial.println(serverName);
    http.begin(client, serverName);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);

      StaticJsonDocument<192> doc;

      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      int id = doc["id"];             // 1
      const char *name = doc["name"]; // "61c2fe72a96fb14482dc66ce"
      int state = doc["state"];       // 1
      String time = doc["time"]; // "0,1300,1305"
      Serial.print("State = ");
      Serial.println(state);
      Serial.print("Time = ");
      Serial.println(time);
      String timenf = time.substring(0, 1);
      String timest = time.substring(2,6);
      String timend = time.substring(7,11);

      Serial.print("timenf = ");
      Serial.println(timenf);
      Serial.print("timest = ");
      Serial.println(timest);
      Serial.print("timend = ");
      Serial.println(timend);

      if (state == 1)
      {
        setval = 1;
        digitalWrite(l1, HIGH);
        digitalWrite(ledgreen, LOW);
      }
      if (state == 0)
      {
        setval = 0;
        digitalWrite(ledgreen, HIGH);
        digitalWrite(l1, LOW);
      }
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    // .............................................now fetching date and time
    WiFiClient client2;
    HTTPClient http2;
    String serverPath = "http://worldtimeapi.org/api/timezone/Asia/Kolkata";
    http2.begin(client2, serverPath);
    int httpResponseCode2 = http2.GET();
    if (httpResponseCode2 > 0)
    {
      Serial.print("HTTP Response code2: ");
      Serial.println(httpResponseCode2);
      String input = http2.getString();
      //Serial.println(input);
      StaticJsonDocument<16> filter;
      filter["datetime"] = true;

      StaticJsonDocument<96> doc;

      DeserializationError error = deserializeJson(doc, input, DeserializationOption::Filter(filter));

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      const char *datetime = doc["datetime"];
      Serial.println(datetime);
      String one = datetime;
      String timethh = one.substring(11, 13);
      String timetmm = one.substring(14, 16);
      //Serial.println(timethh);
      //Serial.println(timetmm);
      String timet = timethh + timetmm;
      Serial.print("Time = ");
      Serial.println(timet);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode2);
    }
    http2.end();
  }

  else
  {
    digitalWrite(ledblue, HIGH);
    delay(200);
    digitalWrite(ledblue, LOW);
    delay(200);
    Serial.println("wifi not connected");
  }
}
// .............end of loop
void fun(void)
{
  unsigned long currentTime = millis();
  if (digitalRead(overswitch) == LOW)
  {
    if (onetime == 1)
    {
      inTime = currentTime;
      onetime = 0;
    }

    if (inTime + 3000UL < currentTime && inTime + 3000UL + 1500UL > currentTime)
    {
      secpass = 0;
    }

    if (inTime + interval < currentTime && inTime + interval + 1500UL > currentTime && secpass == 0)
    {
      Serial.println("reseting...................");
      for (int i = 0; i < 160; ++i)
      {
        EEPROM.write(i, 0);
      }
      EEPROM.end();
      Serial.println("EEPROM reset");
      delay(500);
      ESP.reset();
      delay(600);
    }
  }
  if (inTime + interval + 1500UL < currentTime)
  {
    onetime = 1;
    secpass = 1;
  }
  Serial.print("onetime =");
  Serial.println(onetime);
  Serial.print("secpass =");
  Serial.println(secpass);

  if (digitalRead(overswitch) == LOW && setval == 0)
  {
    setval = 1;
    digitalWrite(l1, HIGH);
    digitalWrite(ledgreen, LOW);
    delay(300);
  }
  else if (digitalRead(overswitch) == LOW && setval == 1)
  {

    setval = 0;
    digitalWrite(l1, LOW);
    digitalWrite(ledgreen, HIGH);
    delay(300);
  }
}
//----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  if (esid == 0)
  {
    Serial.println("esid is zero loop in testWifi");
    return false;
  }
  while (c < 60)
  {
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
    server.on("/", []()
              {

      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=32><input type='submit'><input name='auth' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content); });
    server.on("/scan", []()
              {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content); });

    server.on("/setting", []()
              {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      String qauth = server.arg("auth");
      if (qsid.length() > 0 && qpass.length() > 0 && qauth.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 128; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
        Serial.println(qauth);
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
        Serial.println("writing eeprom auth:");
        for (int i = 0; i < qauth.length(); ++i)
        {
          EEPROM.write(64 + i, qauth[i]);
          Serial.print("Wrote: ");
          Serial.println(qauth[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
             //ESP.reset();
      } 
      else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
      Serial.println("should reset?");
      digitalWrite(ledblue,LOW);
      delay(1000);
      rest=1; });
  }
}
