
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>;

int l1 = 13;
int fedback = 12;
const byte interruptPin = 14;
int ledblue = 5;
int ledgreen = 4;
bool setval = 0;
bool rest = 0;

volatile bool sat = 0;
volatile bool sata = 1;
unsigned long RiseTime;
unsigned long FallTime;
long delta;

int i = 0;
int statusCode;
const char *ssid = "text";
const char *passphrase = "text";
String st;
String content;
String esid;
String eauths;
String timenf;
String timest;
String timend;
String timet;
String output;
String timeg;

bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void doReset();
void doRising();
void doFalling();

ESP8266WebServer server(80);

void setup()
{
  Serial.begin(115200);
  pinMode(l1, OUTPUT);
  pinMode(ledblue, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(fedback, INPUT);
  pinMode(interruptPin, INPUT);

  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); // Initialasing EEPROM
  delay(10);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, CHANGE);
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
    if (sata == 1)
    {
      digitalWrite(ledblue, HIGH);
      digitalWrite(ledgreen, LOW);
    }

    if (sata == 0)
    {
      digitalWrite(ledblue, LOW);
      digitalWrite(ledgreen, LOW);
    }
    Serial.print(".");
    server.handleClient();
    delay(100);
    if (rest == 1)
    {
      rest = 0;
      delay(200);
      ESP.reset();
    }
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
    String server = "http://tesla001.ddns.net:5001/api/public/";
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
      String time = doc["time"];      // "0,1300,1305"
      Serial.print("State = ");
      Serial.println(state);
      Serial.print("Time = ");
      Serial.println(time);
      timeg = time.substring(0, 11);
      timenf = time.substring(0, 1);
      timest = time.substring(2, 6);
      timend = time.substring(7, 11);

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
      // Serial.println(input);
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
      // Serial.println(timethh);
      // Serial.println(timetmm);
      timet = timethh + timetmm;
      Serial.print("Time = ");
      Serial.println(timet);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode2);
    }
    http2.end();

    ///..................Updateing state on the server if timenf is True
    if (timenf.toInt() == 1)
    {
      if (timest == timet)
      {
        http.addHeader("Content-Type", "application/json");
        String output = {"{\"state\": 1,\"time\": \"" + timeg + "\"}"};
        int httpResponseCode = http.PUT(output);
        String response = http.getString();

        Serial.println(httpResponseCode);
        Serial.println(response);
        http.end();
      }
      if (timend == timet)
      {
        http.addHeader("Content-Type", "application/json");
        String output = {"{\"state\": 0,\"time\": \"" + timeg + "\"}"};
        int httpResponseCode = http.PUT(output);
        String response = http.getString();

        Serial.println(httpResponseCode);
        Serial.println(response);
        http.end();
      }
    }
  }

  else
  {
    digitalWrite(ledblue, HIGH);
    digitalWrite(ledgreen, LOW);
    delay(200);
    digitalWrite(ledblue, LOW);
    digitalWrite(ledgreen, LOW);
    delay(200);
    Serial.println("wifi not connected");
  }
}
// .............end of loop

ICACHE_RAM_ATTR void ISR()
{
  Serial.println("isr loop..............................");
  if (sat == 0)
  {
    doRising();
    RiseTime = millis();
    Serial.print("riseTime = ");
    Serial.println(RiseTime);
    sat = 1;
  }
  else
  {
    doFalling();
    FallTime = millis();
    Serial.print("fallTime = ");
    Serial.println(FallTime);
    sat = 0;
  }

  delta = FallTime - RiseTime;
  Serial.print("delta = ");
  Serial.println(delta);

  if ((delta > 7000UL) && (delta < 20000UL))
  {
    doReset();
  }
}

void doReset()
{
  Serial.println("reseting...................");
  for (int i = 0; i < 160; ++i)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
  Serial.println("EEPROM reset");
  digitalWrite(l1, LOW);
  digitalWrite(ledgreen, HIGH);
  digitalWrite(ledblue, HIGH);
  ESP.reset();
}
void doRising()
{
  Serial.println("rising loop");

  if (sata == 1)
  {
    digitalWrite(l1, HIGH); // Interrupt occurred, so turn On LED
    digitalWrite(ledgreen, LOW);
    digitalWrite(ledblue, LOW);
    sata = 0;
  }
  else
  {
    digitalWrite(l1, LOW);
    digitalWrite(ledgreen, HIGH);
    sata = 1;
  }
}
void doFalling()
{

  Serial.println("falling loop");
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
  while (c < 240)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
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
      rest=1; });
  }
}
