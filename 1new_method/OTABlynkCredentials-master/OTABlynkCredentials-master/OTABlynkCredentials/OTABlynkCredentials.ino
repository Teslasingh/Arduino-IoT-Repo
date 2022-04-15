#include "OTABlynkCredentials.h"

#ifdef ESP8266
#include <BlynkSimpleEsp8266.h>
#elif defined(ESP32)
#include <BlynkSimpleEsp32.h>
#else
#error "Board not found"
#endif

credentials Credentials;



//Variables
char auth_token[33];
bool connected_to_internet = 0;
const int Erasing_button = D6;
const int blue = D1;

//Provide credentials for your ESP server
char* esp_ssid = "Socket";
char* esp_pass = "";

//
//
// .     Add your variables 
//
// .             HERE
// .
//
//





void setup()

{

  Serial.begin(115200);
  pinMode(Erasing_button, INPUT);
  pinMode(blue,OUTPUT);

  for (uint8_t t = 3; t > 0; t--) {
    Serial.println(t);
    delay(1000);
  }

  // Press and hold the button to erase all the credentials
  if (digitalRead(Erasing_button) == LOW)
  {
    Credentials.Erase_eeprom();

  }

  String auth_string = Credentials.EEPROM_Config();
  auth_string.toCharArray(auth_token, 33);

  if (Credentials.credentials_get())
  {

  //Blynk.config(auth_token);
    Blynk.begin(auth_token,esp_ssid,esp_pass,"blynk-cloud.com",8080);
    connected_to_internet = 1;

  }
  else
  {
    Credentials.setupAP(esp_ssid, esp_pass);
    connected_to_internet = 0;
  }


  if (connected_to_internet)
  {

    //
    //
    // .  Write the setup part of your code
    //
    // .             HERE
    // .
    //
    //

  }



}



void loop()
{
  Credentials.server_loops();

  if (connected_to_internet)
  {


    //
    //
    // .  Write the loop part of your code
    //
    // .             HERE
    // .
    //
    //
     Serial.println("runing blynk");
    digitalWrite(blue,LOW);
    Blynk.run();
  }
  else{
    digitalWrite(blue,HIGH);
  }
}
