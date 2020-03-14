float sensor=A0;
float gas_value;
void setup()
{

pinMode(sensor,INPUT);
Serial.begin(9600);

}

void loop()
{

gas_value=analogRead(sensor);
Serial.print(" gas concentration methane in %" );
Serial.println (gas_value);
}
