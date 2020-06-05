#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include<Wire.h>
#include<MPU6050.h>



// --------------------------------------------------------------------------------------------
//        UPDATE CONFIGURATION TO MATCH YOUR ENVIRONMENT
// --------------------------------------------------------------------------------------------

// Add GPIO pins used to connect devices

//emergency button
#define ledpin D4
#define button D5
int buttonState=0;

//mq9
#define LED D3
#define DO D8
//mpu6050
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float ax=0, ay=0, az=0, gx=0, gy=0, gz=0;
//mpu6050
#define led D0
int fall_detected;
//int data[STORE_SIZE][5]; //array for saving past data
//byte currentIndex=0; //stores current data array index (0-255)
boolean fall = false; //stores if a fall has occurred
boolean trigger1=false; //stores if first trigger (lower threshold) has occurred
boolean trigger2=false; //stores if second trigger (upper threshold) has occurred
boolean trigger3=false; //stores if third trigger (orientation change) has occurred

byte trigger1count=0; //stores the counts past since trigger 1 was set true
byte trigger2count=0; //stores the counts past since trigger 2 was set true
byte trigger3count=0; //stores the counts past since trigger 3 was set true
int angleChange=0;

//mpu read function
void mpu_read(){
 Wire.beginTransmission(MPU_addr);
 Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
 Wire.endTransmission(false);
 Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
 AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
 AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
 AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
 Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
 GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
 GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
 GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
 }



// Add WiFi connection information
char ssid[] = "AKOO";  // your network SSID (name)
char pass[] = "akshaya123";  // your network password

// Watson IoT connection details

#define MQTT_HOST "3vrxae.messaging.internetofthings.ibmcloud.com"//"orgid.messaging....
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:3vrxae:esp8266:dev02"//orgid-devicetype-deviceid
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "ElectrosSevak"//authentication token
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/display/fmt/json"


// --------------------------------------------------------------------------------------------
//        SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS LINE
// --------------------------------------------------------------------------------------------

// MQTT objects
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// variables to hold data
StaticJsonDocument<100> jsonDoc;
JsonObject payload = jsonDoc.to<JsonObject>();
JsonObject status = payload.createNestedObject("d");
static char msg[70];

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  
  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char *)payload);
}

void setup()
{
  // Start serial console

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);
  //for mq9
  
  pinMode(LED, OUTPUT); 
  pinMode(DO, INPUT); 
 
  //for mpu6050
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  //emergency button
  pinMode(ledpin, OUTPUT);
 pinMode(button, INPUT);

  
 
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

  // Connect to MQTT - IBM Watson IoT Platform
  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_DISPLAY);

  } else {
    Serial.println("MQTT Failed to connect!");
    ESP.reset();
  }

 }

void loop()
{
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

  //emergency button
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

  
  //mq9
  
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

 //mpu6050
 mpu_read();
 //2050, 77, 1947 are values for calibration of accelerometer
 
 ax = (AcX-2050)/16384.00;
 ay = (AcY-77)/16384.00;
 az = (AcZ-1947)/16384.00;

 //270, 351, 136 for gyroscope
 gx = (GyX+270)/131.07;
 gy = (GyY-351)/131.07;
 gz = (GyZ+136)/131.07;

 // calculating Amplitute vactor for 3 axis
 float Raw_AM = pow(pow(ax,2)+pow(ay,2)+pow(az,2),0.5);
 int AM = Raw_AM * 10;  // as values are within 0 to 1, I multiplied 
                        // it by for using if else conditions 
 
 //Serial.println(PM);
 //delay(500);

 if (trigger3==true){
    trigger3count++;
    //Serial.println(trigger3count);
    if (trigger3count>=10){ 
       angleChange = pow(pow(gx,2)+pow(gy,2)+pow(gz,2),0.5);
       //delay(10);
       Serial.println(angleChange); 
       if ((angleChange>=0) && (angleChange<=10)){ //if orientation changes remains between 0-10 degrees
           fall=true; trigger3=false; trigger3count=0;
           Serial.println(angleChange);
             }
       else{ //user regained normal orientation
          trigger3=false; trigger3count=0;
          Serial.println("TRIGGER 3 DEACTIVATED");
       }
     }
  }
  //for creating alert fall_detected used
  if(fall==false)
    fall_detected=0;
    else
    fall_detected=1;
    
 if (fall==true){ //in event of a fall detection
  
  //send data to ibm cloud
  status["co"] = ratio;
    status["CO_ppm"] = ppm;
    status["AM"]=AM;
    status["fall_detected"]=fall_detected;
    serializeJson(jsonDoc, msg, 300);
    Serial.println(msg);
    //
    Serial.println("FALL DETECTED");
   digitalWrite(led, HIGH);
   delay(5000);
   digitalWrite(led, LOW);
   
   fall=false;//resetting value
  // exit(1);
   }
 if (trigger2count>=6){ //allow 0.5s for orientation change
   trigger2=false; trigger2count=0;
   Serial.println("TRIGGER 2 DECACTIVATED");
   }
 if (trigger1count>=6){ //allow 0.5s for AM to break upper threshold
   trigger1=false; trigger1count=0;
   Serial.println("TRIGGER 1 DECACTIVATED");
   }
 if (trigger2==true){
   trigger2count++;
   //angleChange=acos(((double)x*(double)bx+(double)y*(double)by+(double)z*(double)bz)/(double)AM/(double)BM);
   angleChange = pow(pow(gx,2)+pow(gy,2)+pow(gz,2),0.5); Serial.println(angleChange);
   if (angleChange>=30 && angleChange<=400){ //if orientation changes by between 80-100 degrees
     trigger3=true; trigger2=false; trigger2count=0;
     Serial.println(angleChange);
     Serial.println("TRIGGER 3 ACTIVATED");
       }
   }
 if (trigger1==true){
   trigger1count++;
   if (AM>=12){ //if AM breaks upper threshold (3g)
     trigger2=true;
     Serial.println("TRIGGER 2 ACTIVATED");
     trigger1=false; trigger1count=0;
     }
   }
 if (AM<=2 && trigger2==false){ //if AM breaks lower threshold (0.4g)
   trigger1=true;
   Serial.println("TRIGGER 1 ACTIVATED");
   }
//It appears that delay is needed in order not to clog the port
 delay(100);

 Serial.print("am= ");
 Serial.println(AM);
 Serial.print("Status of fall detection= ");
 Serial.println(fall);
 
    
    // Print Message to console in JSON format
   
       
    status["co"] = ratio;
    status["CO_ppm"] = ppm;
    status["AM"]=AM;
    status["fall"]=fall_detected;
    status["button"]=buttonState;
    serializeJson(jsonDoc, msg, 70);
    Serial.println(msg);

    
    if (!mqtt.publish(MQTT_TOPIC, msg)) {
      Serial.println("MQTT Publish failed");
    }

    // Pause - but keep polling MQTT for incoming messages
  for (int i = 0; i < 10; i++) {
    mqtt.loop();
    delay(100);
  }
  }
