
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>;

int l1 = 13;
int fedback = 12;
const byte interruptPin = 14;
int ledblue = 4;
int ledgreen = 5;
bool setval = 0;
bool rest = 0;
volatile bool isrloop = 0;
bool doOnce=1;

volatile bool sat = 0;
volatile bool sata = 1;
volatile unsigned long RiseTime;
volatile unsigned long FallTime;
long delta;

unsigned long timennow;
int i = 0;
int statusCode;
const char *ssid = "text";
const char *passphrase = "text";
String st;
String content;
String esid;
String eauths;

String output;

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
        digitalWrite(ledblue, HIGH);
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
    if (isrloop == 1)
    {
        Serial.println("   /// blinker indiator /// ");
        timennow = millis();
        if ((timennow > RiseTime + 8000UL) && (timennow < RiseTime + 11000UL))
        {
            // digitalWrite(l1, LOW);
            digitalWrite(ledgreen, HIGH);
            digitalWrite(ledblue, HIGH);
        }
    }

    if ((WiFi.status() == WL_CONNECTED))
    {

        delay(2000);
        Serial.println("//////////////////////////////////////////////////////////////////////////////////");
        digitalWrite(ledblue, LOW);
        Serial.println("wifi connected");
        WiFiClient client;
        HTTPClient http;
        if(sata == 0 && doOnce == 1){
         String server = "http://tesla001.ddns.net/d/";
        String serverName = server + eauths + "/" + "?s=1";
        Serial.println(serverName);
        http.begin(client, serverName);
         int httpResponseCode = http.GET();
                if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
        }
        doOnce = 0;
      }
      if (sata == 1 && doOnce == 0){
        String server = "http://tesla001.ddns.net/d/";
        String serverName = server + eauths + "/" + "?s=0";
        Serial.println(serverName);
        http.begin(client, serverName);
         int httpResponseCode = http.GET();
                if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
        }
        doOnce = 1;
      }

        String server = "http://tesla001.ddns.net/d/";
        String serverName = server + eauths + "/";
        Serial.println(serverName);
        http.begin(client, serverName);
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);

            StaticJsonDocument<32> doc;

            DeserializationError error = deserializeJson(doc, payload);

            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }

            int s = doc["s"]; // 1

            Serial.print("state = ");
            Serial.println(s);

            if (s == 1)
            {
                setval = 1;
                digitalWrite(l1, HIGH);
                digitalWrite(ledgreen, LOW);
            }
            if (s == 0)
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

        ///..................Updateing s on the server if timenf is True
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
        isrloop = 1;
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

    if ((delta > 7000UL) && (delta < 13000UL))
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
    WiFiClient client;
    HTTPClient http;
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
    http.end();
}
void doFalling()
{
    isrloop = 0;
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
              if (isrloop == 1)
       {
        Serial.println("   /// blinker indiator /// ");
        timennow = millis();
        if ((timennow > RiseTime + 8000UL) && (timennow < RiseTime + 11000UL))
        {
            // digitalWrite(l1, LOW);
            digitalWrite(ledgreen, HIGH);
            digitalWrite(ledblue, HIGH);
        }
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
