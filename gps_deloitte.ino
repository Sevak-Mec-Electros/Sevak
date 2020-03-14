#include "UbidotsMicroESP8266.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define TOKEN  "BBFF-Q6dsWbWpIEYHL1X5dHw12gYcXZrXxK"  // Put here your Ubidots TOKEN
#define WIFISSID "Airtel_Zerotouch" // Put here your Wi-Fi SSID
#define PASSWORD "12345678" // Put here your Wi-Fi password

Ubidots client(TOKEN);

static const int RXPin = D1, TXPin = D2;
static const uint32_t GPSBaud = 9600; // Change according to your device

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

double _lat, _lng;
int dummySensorValue = 22;
char context[25];

unsigned long previousMillis = 0;     // last time data was send
const long interval = 5000;           // data transfer interval

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  client.wifiConnection(WIFISSID, PASSWORD);
}

void loop()
{
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (gps.location.isValid())
        {
          _lat = gps.location.lat();
          _lng = gps.location.lng();
          sprintf(context, "lat=%.2f$lng=%.2f", _lat, _lng);

          client.add("Dummy Sensor", dummySensorValue, context);
          client.sendAll(true);

          Serial.print(F("Location: "));
          Serial.print(_lat);
          Serial.print(F(","));
          Serial.print(_lng);
        }
        else
        {
          Serial.print(F("INVALID"));
        }

        Serial.println();
      }
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
}



/*#include <TinyGPS++.h>
  #include <SoftwareSerial.h>
  #include <ESP8266WiFi.h>

  TinyGPSPlus gps;  // The TinyGPS++ object

  SoftwareSerial ss(4, 5); // The serial connection to the GPS device

  const char* ssid = "Airtel_Zerotouch";
  const char* password = "12345678";

  float latitude , longitude;
  int year , month , date, hour , minute , second;
  String date_str , time_str , lat_str , lng_str;
  int pm;

  WiFiServer server(80);
  void setup()
  {
  Serial.begin(115200);
  ss.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  }

  void loop()
  {

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
      }

      if (gps.date.isValid())
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
          date_str = '0';
        date_str += String(date);

        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month);

        date_str += " / ";

        if (year < 10)
          date_str += '0';
        date_str += String(year);
      }

      if (gps.time.isValid())
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();

        minute = (minute + 30);
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;

        if (hour >= 12)
          pm = 1;
        else
          pm = 0;

        hour = hour % 12;

        if (hour < 10)
          time_str = '0';
        time_str += String(hour);

        time_str += " : ";

        if (minute < 10)
          time_str += '0';
        time_str += String(minute);

        time_str += " : ";

        if (second < 10)
          time_str += '0';
        time_str += String(second);

        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";

      }

    }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GPS Interfacing with NodeMCU</title> <style>";
  s += "a:link {background-color: YELLOW;text-decoration: none;}";
  s += "table, th, td {border: 1px solid black;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER> GPS Interfacing with NodeMCU</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += lat_str;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += lng_str;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += date_str;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += time_str;
  s += "</td>  </tr> </table> ";


  if (gps.location.isValid())
  {
     s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += lat_str;
    s += "+";
    s += lng_str;
    s += """ target=""_top"">Click here!</a> To check the location in Google maps.</p>";
  }

  s += "</body> </html> \n";

  client.print(s);
  delay(100);

  }
*/


