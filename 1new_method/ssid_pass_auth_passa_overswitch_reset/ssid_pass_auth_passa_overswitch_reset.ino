
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>


//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

int l1 = 13;
int resetpin =12;
int overswitch=14;
int ledblue=5;
int ledgreen=4;



#define API_KEY "AIzaSyAAfShlueTplHUOvJA0l-mlVWkljxOD5U4"
/* 3. Define the RTDB URL */
#define DATABASE_URL "smart-home-9928c-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "subham@gmail.com"
#define USER_PASSWORD "saumyatiluu"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;


boolean rest=0;

//Variables
int i = 0;
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;


//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);

//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);

void setup()
{
pinMode(l1,OUTPUT);
pinMode(ledblue,OUTPUT);
pinMode(ledgreen,OUTPUT);
pinMode(resetpin,INPUT);
pinMode(overswitch,INPUT);
  Serial.begin(9600); //Initialising if(DEBUG)Serial Monitor
  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  pinMode(LED_BUILTIN, OUTPUT);
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
  for (int i = 32; i < 64; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);

  String eauth = "";
  for (int i = 64; i < 96; ++i)
  {
    eauth += char(EEPROM.read(i));
  }
  Serial.print("AUTH: ");
  Serial.println(eauth);

    String epassa = "";
  for (int i = 96; i < 128; ++i)
  {
    epassa += char(EEPROM.read(i));
  }
  Serial.print("PASSA: ");
  Serial.println(epassa);


  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
    Serial.println("Succesfully Connected!!!");
      Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  //Or use legacy authenticate method
  //config.database_url = DATABASE_URL;
  //config.signer.tokens.legacy_token = "<database secret>";

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
 Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);
  delay(10);
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
    digitalWrite(ledblue,HIGH);
    digitalWrite(ledgreen,LOW);
    Serial.print(".*");
    delay(100);
    server.handleClient();
    delay(200);
    if(rest==1){
    rest=0;
    ESP.reset();
    }

    if (digitalRead(overswitch)==LOW){
      
    for (uint8_t t = 4; t > 0; t--) {
    Serial.println(t);
    delay(1000);                                       ///override switch clicked for max of 4 sec then restart the ddevice
    }
    if (digitalRead(overswitch)==LOW){
      Serial.println("restarting the device");
      ESP.reset();
    }
  }
    
  }




  

}
void loop() {
  if ((WiFi.status() == WL_CONNECTED))
  {

    
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
     digitalWrite(ledblue,LOW);
     Serial.println("wifi connected");

  }
  if (digitalRead(resetpin)== LOW){                                   //press rest pin more then 3 sec to erase the eeprom and reset the device
      for (uint8_t t = 3; t > 0; t--) {
    Serial.println(t);
    delay(1000);
  }
  if (digitalRead(resetpin)== LOW){
    for (int i = 0; i < 160; ++i) {
          EEPROM.write(i, 0);
        }
      EEPROM.end();
      Serial.println("EEPROM reset");
      delay(500);
      ESP.reset();
  }
  }

  if (digitalRead(overswitch)==LOW){               //overSwitch clicked for more than 4 sec restart the device
    for (uint8_t t = 4; t > 0; t--) {
    Serial.println(t);
    delay(1000);
    }
    if (digitalRead(overswitch)==LOW){
      ESP.reset();
    }
  }

  Serial.println(Firebase.ready());
 
  if (Firebase.ready())
  {
 
    String light1=Firebase.RTDB.getString(&fbdo, "/light1") ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    Serial.println(light1);
    
      if(light1=="on"){
       digitalWrite(l1,HIGH);
       digitalWrite(ledgreen,LOW);
    }else{
       digitalWrite(ledgreen,HIGH);
       digitalWrite(l1,LOW);
    }
  }
}


//----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connection timed out, opening AP");
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
      content = "<!DOCTYPE HTML>\r\n<html>Put you WiFi name and Password here.";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=32><input name='auth' length=32><input name='passa' length=32><input type='submit'></form>";
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
      String qauth = server.arg("auth");
      String qpassa = server.arg("passa");
      if (qsid.length() > 0 && qpass.length() > 0 && qauth.length() > 0 && qpassa.length() > 0) {
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
        Serial.println(qpassa);

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
        Serial.println("writing eeprom passa:");
        for (int i = 0; i < qpassa.length(); ++i)
        {
          EEPROM.write(96 + i, qpassa[i]);
          Serial.print("Wrote: ");
          Serial.println(qpassa[i]);
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
      delay(1000);
      rest=1;
    });
  } 
}
