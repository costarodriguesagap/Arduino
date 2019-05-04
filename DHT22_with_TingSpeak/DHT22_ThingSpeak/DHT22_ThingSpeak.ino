#include <DHT.h>  // Including library for dht

#include <ESP8266WiFi.h>
 
String apiKey = "xxxxxx";

const char* ssid = "xxxxx";
const char* password = "xxxxxx";
const char* server = "api.thingspeak.com";

#define DHTPIN 4          //pino D2 do esp8266
DHT dht(DHTPIN, DHT22);

WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);
       delay(10);
       dht.begin();
       pinMode(2, OUTPUT);
       digitalWrite(2, HIGH);
 
       //Serial.println("Connecting to ");
       //Serial.println(ssid);
 
 
       WiFi.begin(ssid, password);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            //Serial.print(".");
     }
      //Serial.println("");
      //Serial.println("WiFi connected");
 
}
 
void loop() 
{
  
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      //Serial.print("Temperature - ");//Serial.print(t);//Serial.print(" | Humidade - ");//Serial.println(h);
      
              if (isnan(h) || isnan(t)) 
                 {
                     //Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

                         if (client.connect(server,80))
                      {  
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                             digitalWrite(2, LOW);
                             delay(100);
                             digitalWrite(2, HIGH );
                             
                             //Serial.print("Temperature: ");
                             //Serial.print(t);
                             //Serial.print(" degrees Celcius, Humidity: ");
                             //Serial.print(h);
                             //Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          //Serial.println("Waiting...");
  delay(20000);
}
