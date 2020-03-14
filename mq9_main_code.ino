const int LED = 4; 
const int DO = 8; 
void setup() { 
 Serial.begin(9600); 
 pinMode(LED, OUTPUT); 
 pinMode(DO, INPUT); 
} 
void loop() { 
 int alarm = 0; 
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
 if (ratio>9.92) digitalWrite(LED, HIGH); 
 else  digitalWrite(LED, LOW); 
 delay(1000); 
}
