#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#define D3 0
#define D4 2 
#define D5 14 
#define D7 13 
#define D8 15 
#define TFT_CS     D8
#define TFT_RST    D4  
#define TFT_DC     D3
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST); // Instância do objeto tipo Adafruit_ST7735 que recebe como argumentos os pinos de controle
#define TFT_SCLK D5   
#define TFT_MOSI D7

uint8_t n_minutos = 0;
bool first_inq = true;

const char* password;

HTTPClient http; //Object of class HTTPClient

void setup() 
{
  //Serial.begin(115200);
  //delay(100);
  first_inq = true;
  if(ScanAndConnect)
  {
    char wifi_ssid[20];
    char wifi_ip[20];
    WiFi.SSID().toCharArray(wifi_ssid,20);
    WiFi.localIP().toString().toCharArray(wifi_ip,20);
    
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
    printText("Wifi_SSID", ST7735_GREEN,10,10,1);printText(wifi_ssid, ST7735_RED,10,20,1);
    printText("Wifi_IP", ST7735_GREEN,10,40,1);printText(wifi_ip, ST7735_RED,10,50,1);
    delay(5000);

    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
    printText("Nota:", ST7735_YELLOW,1,10,2);
    printText("A recolha de dados da\n API OpenWeathermap\n ocorre a cada 10\n minutos!", ST7735_WHITE,1,40,1);
    delay(10000);

  }
  else{return;}
}

bool ScanAndConnect()
{
   int n = WiFi.scanNetworks();
   if(n==0)
   {
      return false;
   }
   else
   {
      int i=0;
      int SigStrength[10][30]; //maximum of 10 access points. 
      for ( i = 0; i<n; ++i) 
      {
         //2 columms column 0 is the signal Strength
         SigStrength[1][i] = i;
         SigStrength[0][i] = WiFi.RSSI(i);
      }
      for (int i = 1; i < n; ++i)
      {// insert sort into strongest signal 
         int j = SigStrength[0][i]; //holding value for signal strength
         int l = SigStrength[1][i];
         int k;
         for (k = i - 1; (k >= 0) && (j > SigStrength[0][k]); k--)
         {
          SigStrength[0][k + 1] = SigStrength[0][k];
          l = SigStrength[1][k+1];
          SigStrength[1][k+1]=SigStrength[1][k];
          SigStrength[1][k]=l; //swap index values here. 
         }
         SigStrength[0][k + 1] = j;
         SigStrength[1][k + 1] = l; //swap index values here to re-order. 
      }
      int j = 0;
      while ((j < n)  && (WiFi.status() != WL_CONNECTED) )
      {
         //Serial.print("Connecting: "); //debugging
         //Serial.println(WiFi.SSID(SigStrength[1][j])); //debugging
         int k =0;
         if (WiFi.SSID(SigStrength[1][j])== "<wifi_SSID_1>" ||
             WiFi.SSID(SigStrength[1][j])== "<wifi_SSID_2>"){password = "<wifi_pass>";}
         WiFi.begin(WiFi.SSID(SigStrength[1][j]), password); //conect to the j'th value in our new array
         while ((WiFi.status() !=  WL_CONNECTED) && k < 15 )
         {
            Serial.print("k");
            delay(500);
            k++; //counter for up to 8 seconds - why 8 ? who knows
         }
         if (k > 15) 
            Serial.println(F("NEXT Wifi Accest Point"));
         j++; //choose the next SSID
      }
      if (WiFi.status() ==  WL_CONNECTED) 
      {
         Serial.print(F("Connected:"));
         Serial.println(WiFi.SSID());
         return true;
      }
      else
      {
         Serial.println(F("No Connection :( (Sad face)"));
         return false;
      }
   }
}
void init_screen()
{
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  printText("HUMIDITY", ST7735_YELLOW,40,10,1);
  printText("TEMPERATURE", ST7735_GREEN,30,65,1);
  printText("MAX", ST7735_RED,18,130,1);
  printText("MIN", ST7735_BLUE,95,130,1);  
}
void readReponseContent() {
 
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
      Serial.println(F("erro Api .... "));
      Serial.print(F("   > Cod = "));Serial.println(cod);
      Serial.print(F("   > Msg = "));Serial.println(msg);
  }
  else
  { 
    char humidityChar[20];char temperatureChar[20];char charMinTemperature[20];char charMaxTemperature[20];
    double temp = root["main"]["temp"].as<double>();
    dtostrf(temp,2,1,temperatureChar);  
    tft.fillRect(10,80,128,30,ST7735_BLACK);
    printText(temperatureChar, ST7735_WHITE,10,80,3);
    printText("o", ST7735_WHITE,90,75,2);printText("C", ST7735_WHITE,105,80,3);
    
    double temp_min = root["main"]["temp_min"].as<double>();
    dtostrf(temp_min,2,1,charMinTemperature);
    tft.fillRect(75,140,36,18,ST7735_BLACK);
    printText(charMinTemperature, ST7735_WHITE,80,145,1);
    printText("o", ST7735_WHITE,112,140,1);printText("C", ST7735_WHITE,118,145,1);

    double temp_max = root["main"]["temp_max"].as<double>();
    dtostrf(temp_max,2,1,charMaxTemperature);
    tft.fillRect(3,142,33,20,ST7735_BLACK);
    printText(charMaxTemperature, ST7735_WHITE,3,145,1);
    printText("o", ST7735_WHITE,35,140,1);printText("C", ST7735_WHITE,41,145,1);

    int humidity = root["main"]["humidity"].as<int>();
    itoa(humidity, humidityChar,20);
    tft.fillRect(10,22,128,30,ST7735_BLACK);
    printText(humidityChar, ST7735_WHITE,40,22,3);
    printText("%", ST7735_WHITE,80,22,2);
    
    Serial.println(F("api.openweathermap"));
    Serial.print(F("   > Temp     = "));Serial.println(temp);
    Serial.print(F("   > Humidity = "));Serial.println(humidity);
    Serial.print(F("   > Temp Min = "));Serial.println(temp_min);
    Serial.print(F("   > Temp Max = "));Serial.println(temp_max);
    Serial.println();
  }
}

void printText(char *text, uint16_t color, int x, int y,int textSize)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

void loop() 
{
  if (n_minutos < 10 && !first_inq)
  {
    ++n_minutos;
  }
  else
  {
    if(first_inq){init_screen();}
    first_inq = false; 
    Serial.println(F("Invocar API ..."));
    if (WiFi.status() == WL_CONNECTED) 
    {
      http.begin("http://api.openweathermap.org/data/2.5/weather?q=Lisbon,pt&APPID=c90a783983bb396f8d33177987d4c7ca&units=metric");
      int httpCode = http.GET();
  
      if (httpCode > 0) 
      {
        readReponseContent();
      }
      http.end(); //Close connection
    }
    n_minutos = 0;
  }
  delay(60000);
}
