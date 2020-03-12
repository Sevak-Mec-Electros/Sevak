#include <ESP8266WiFi.h>//include ubidots and ESP8266 libraries
#include "Ubidots.h"

const char* UBIDOTS_TOKEN = "";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = ""; // Put here your Wi-Fi SSID
const char* WIFI_PASS = ""; // Put here your Wi-Fi password
char const * HTTPSERVER = "things.ubidots.com";

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);


void setup() {
  Serial.begin(9600);
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);

   ubidots.setDebug(true);  // Uncomment this line for printing debug messages
}

void loop() {
  
  ubidots.add("Variable_Name_One", value);  // Change for your variable name
  bool bufferSent = false;
  bufferSent = ubidots.send();  // Will send data to a device label that matches the device Id
  if (bufferSent) {
    // Do something if values were sent properly
    Serial.println("Values sent by the device");
  }

  delay(10000);
}
