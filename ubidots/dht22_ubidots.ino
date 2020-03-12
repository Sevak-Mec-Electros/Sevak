#include <Adafruit_Sensor.h>
#include <DHT.h>;
#include <ESP8266WiFi.h>
#include "Ubidots.h"

//Constants
#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


const char* UBIDOTS_TOKEN = "BBFF-u9dgLGNPoT5EhDebDQbxqSLEwzUDsI";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "anjana"; // Put here your Wi-Fi SSID
const char* WIFI_PASS = "12345678"; // Put here your Wi-Fi password
char const * HTTPSERVER = "things.ubidots.com";

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);

//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

void setup()
{
  Serial.begin(9600);
  dht.begin();
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  ubidots.setDebug(true);
}

void loop()
{
  
  temp = dht.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  ubidots.add("Temperature", temp);
  bool bufferSent = false;
  bufferSent = ubidots.send();
  if (bufferSent) {
    // Do something if values were sent properly
    Serial.println("Values sent by the device");
  }
 
  delay(1000); //Delay 1 sec.
}

