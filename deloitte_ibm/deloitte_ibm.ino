#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>

// Watson IoT connection details
#define MQTT_HOST "3vrxae.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:3vrxae:ESP8266:dev01"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "ERJ2HUXBsu4V7eXDFx"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/display/fmt/json"

// Add WiFi connection information
char ssid[] = "";     //  your network SSID (name)
char pass[] = "";  // your network password


// Add GPIO pins used to connect devices
#define DHT_PIN 4 // GPIO pin the data line of the DHT sensor is connected to

// Specify DHT11 (Blue) or DHT22 (White) sensor
#define DHTTYPE DHT22


const int PULSE_INPUT = A0;
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle


PulseSensorPlayground pulseSensor;
DHT dht(DHT_PIN, DHTTYPE);

// MQTT objects
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// variables to hold data
StaticJsonDocument<100> jsonDoc;
JsonObject payload = jsonDoc.to<JsonObject>();
JsonObject status = payload.createNestedObject("d");
static char msg[100];

float h = 0.0;
float t = 0.0;
int myBPM=70;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  
  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char *)payload);
}

void setup() {
  
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) { }
  Serial.println();
  Serial.println("ESP8266 Sensor Application");

  // Start WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  dht.begin();
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.setThreshold(THRESHOLD);

  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_DISPLAY);

  } else {
    Serial.println("MQTT Failed to connect!");
    ESP.reset();
  }
  
}
void loop() {
  mqtt.loop();
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_DISPLAY);
      mqtt.loop();
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }
  
  h = dht.readHumidity();
  t = dht.readTemperature(); 
  myBPM = pulseSensor.getBeatsPerMinute();
  
 if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from sensor!");
  } else {   
           status["temp"] = t;
           status["humidity"] = h;
           
          }

   if (pulseSensor.sawNewSample()) {
      pulseSensor.outputSample();
      if (pulseSensor.sawStartOfBeat()) {
         status["heart"]=myBPM;
         delay(20);
        }
   }
    serializeJson(jsonDoc, msg, 100);
    Serial.println(msg);
    if (!mqtt.publish(MQTT_TOPIC, msg)) {
            Serial.println("MQTT Publish failed");
            }
    
             
  }


/* for (int i = 0; i < 10; i++) {
    mqtt.loop();
    delay(1000);
  }*/
