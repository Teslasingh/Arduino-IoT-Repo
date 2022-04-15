/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecureBearSSL.h>
// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date
const uint8_t fingerprint[20] = {0x7e, 0x0e, 0x1d, 0x7a, 0x98, 0xf0, 0x88, 0x31, 0x70, 0x36, 0x7c, 0xe4, 0x41, 0xb7, 0x44, 0x28, 0x76, 0x7f, 0x00, 0xfc};

ESP8266WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("TES2","tesla458266");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    // client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://api.coindesk.com/v1/bpi/currentprice.json")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
      
      StaticJsonDocument<1024> doc;

DeserializationError error = deserializeJson(doc, payload);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject root_0 = doc[0];

JsonObject root_0_time = root_0["time"];
const char* root_0_time_updated = root_0_time["updated"]; // "Dec 6, 2021 12:33:00 UTC"
const char* root_0_time_updatedISO = root_0_time["updatedISO"]; // "2021-12-06T12:33:00+00:00"
const char* root_0_time_updateduk = root_0_time["updateduk"]; // "Dec 6, 2021 at 12:33 GMT"

String root_0_disclaimer = root_0["disclaimer"]; // "This data was produced from the CoinDesk ...
const char* root_0_chartName = root_0["chartName"]; // "Bitcoin"

for (JsonPair root_0_bpi_item : root_0["bpi"].as<JsonObject>()) {
  const char* root_0_bpi_item_key = root_0_bpi_item.key().c_str(); // "USD", "GBP", "EUR"

  const char* root_0_bpi_item_value_code = root_0_bpi_item.value()["code"]; // "USD", "GBP", "EUR"
  const char* root_0_bpi_item_value_symbol = root_0_bpi_item.value()["symbol"]; // "&#36;", "&pound;", ...
 String root_0_bpi_item_value_rate = root_0_bpi_item.value()["rate"]; // "48,651.5283", ...
String root_0_bpi_item_value_description = root_0_bpi_item.value()["description"]; // "United ...
  double root_0_bpi_item_value_rate_float = root_0_bpi_item.value()["rate_float"]; // 48651.5283, ...

          Serial.println(root_0_bpi_item_value_rate);
           Serial.println(root_0_bpi_item_value_description);
}
    
         Serial.println(root_0_disclaimer);


            

            }
        }
      } else {
      //  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  Serial.println("Wait 10s before next round...");
  delay(10000);

  }
