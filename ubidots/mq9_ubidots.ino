#include <ESP8266WiFi.h>
#include <Ubidots.h>

//ubidots
const char* UBIDOTS_TOKEN = "";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = ""; // Put here your Wi-Fi SSID
const char* WIFI_PASS = ""; // Put here your Wi-Fi password
char const * HTTPSERVER = "things.ubidots.com";

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);


#define LED D0
#define DO D1

void setup() { 
 Serial.begin(9600); 
 ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
 ubidots.setDebug(true);
 
 pinMode(LED, OUTPUT); 
 pinMode(DO, INPUT); 
 
} 

void loop() { 
 int alarm = 0;
 const int thresh=50;//threshold value can be changed. 
 float sensor_volt; 
 float RS_gas; 
 float ratio; 
//-Replace the name "R0" with the value of R0 in the demo of First Test -/ 
 float R0 = 2.30; 
 int sensorValue = analogRead(A0); 
 sensor_volt = ((float)sensorValue / 1024) * 5.0; 
RS_gas = (5.0 - sensor_volt) / sensor_volt; // Depend on RL on yor module 
 ratio = RS_gas / R0; // ratio = RS/R0 
//------------------------------------------------------------/ 
 Serial.print("sensor_volt = "); 
 Serial.println(sensor_volt); 
 Serial.print("RS_ratio = "); 
 Serial.println(RS_gas); 
 Serial.print("Rs/R0 = "); 
 Serial.println(ratio); 
 Serial.print("\n\n");
 Serial.print("DO="); 
 Serial.println(digitalRead(DO));   
 alarm = digitalRead(DO);
  Serial.print("alarm="); 
  Serial.println(alarm);  
  double ppm = 3.027*exp(1.0698*(sensorValue*3.3/4095));
 Serial.print("CO concentration in ppm=");
 Serial.println(ppm);
 if (ppm>thresh) digitalWrite(LED, HIGH); 
 else  digitalWrite(LED, LOW); 
  delay(1000); 
  //ubidots
ubidots.add("concentration_resistance", ratio);
ubidots.add("CO_ppm", ppm);
ubidots.add("Alarm", alarm);
    bool bufferSent = false;
  bufferSent = ubidots.send();
  if (bufferSent) {
    // Do something if values were sent properly
    Serial.println("Values sent by the device");
}
}
