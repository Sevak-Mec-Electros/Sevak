int ledpin = 5; // D1(gpio5)
int button = 4; //D2(gpio4)
int buttonState=0;

#include <Ubidots.h>

//ubidots
const char* UBIDOTS_TOKEN = "";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = ""; // Put here your Wi-Fi SSID
const char* WIFI_PASS = ""; // Put here your Wi-Fi password
char const * HTTPSERVER = "things.ubidots.com";

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);
void setup() {
 Serial.begin(9600);
 pinMode(ledpin, OUTPUT);
 pinMode(button, INPUT);
 ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
 ubidots.setDebug(true);
 

}
void loop() {
 buttonState=digitalRead(button); // put your main code here, to run repeatedly:
 if (buttonState == 1)
 {
 digitalWrite(ledpin, HIGH); 
 delay(200);
 }
 if (buttonState==0)
 {
 digitalWrite(ledpin, LOW); 
 delay(200);
 }

 //ubidots
ubidots.add("buttonState", buttonState);
    bool bufferSent = false;
  bufferSent = ubidots.send();
  if (bufferSent) {
    // Do something if values were sent properly
    Serial.println("Values sent by the device");
}
}
