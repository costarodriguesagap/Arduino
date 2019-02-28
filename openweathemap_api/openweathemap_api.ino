#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "BTXPS1_2.4G";
const char* password = "#Pentagrama24968#";

uint8_t n_minutos = 0;

struct clientData {
  char temp[8];
  char temp_min[8];
  char temp_max[8];
  char humidity[8];
};

String result;

HTTPClient http; //Object of class HTTPClient

void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);Serial.println("Connecting...");
  }
  Serial.println("Connected...");
}

void readReponseContent(struct clientData* clientData) {
 
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 260;
  DynamicJsonDocument doc(capacity);

  DeserializationError error = deserializeJson(doc,http.getString().c_str());
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  JsonObject root = doc.as<JsonObject>();
  if (root["cod"] != 200)
  {   
      int cod = root["cod"];
      String msg = root["message"];
      Serial.println("erro Api .... ");
      Serial.print("   > Cod = ");Serial.println(cod);
      Serial.print("   > Msg = ");Serial.println(msg);
  }
  else
  {
    strcpy(clientData->temp, root["temp"]);
    strcpy(clientData->temp_min, root["temp_min"]);
    strcpy(clientData->temp_max, root["temp_max"]);
    strcpy(clientData->humidity, root["humidity"]);
    printclientData(clientData);
  }
}

// Print the data extracted from the JSON
void printclientData(const struct clientData* clientData) {
  Serial.println("###############################################");
  Serial.print("Temp = ");
  Serial.println(clientData->temp);
  Serial.print("Temp_Min = ");
  Serial.println(clientData->temp_min);
  Serial.print("Temp_Max = ");
  Serial.println(clientData->temp_max);
  Serial.print("Humidity = ");
  Serial.println(clientData->humidity);
  Serial.println("###############################################");
}

void loop() 
{
  if (n_minutos < 1)
  {
    ++n_minutos;
  }
  else
  {
    Serial.println("Invocar API ...");
    if (WiFi.status() == WL_CONNECTED) 
    {
      http.begin("http://api.openweathermap.org/data/2.5/weather?q=Lisbon,pt&APPID=c90a783983bb396f8d33177987d4c7ca&units=metric");
      int httpCode = http.GET();
  
      if (httpCode > 0) 
      {
        //Serial.println(http.getString());
        clientData clientData;
        readReponseContent(&clientData);
      }
      http.end(); //Close connection
    }
    n_minutos = 0;
  }
  delay(60000);
}
