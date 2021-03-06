#include "RTClib.h"
#include<stdio.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ArduinoJson.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16
#define DATA_PIN_H  13
#define CS_PIN_H    12
#define CLK_PIN_H   14

// Board : Wemos D1 R1

#define buzzer_pin 16
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN_H, CLK_PIN_H, CS_PIN_H, MAX_DEVICES);

RTC_DS3231 rtc;

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;
WiFiClient client;

uint8_t SPEED_SCROLL = 25;
char my_array_H[75];            // Character array for printing something.
char my_array_D[75];
char data_dt[15];
char hora_dt[15];
// Character array for printing something.
uint8_t cnt = 0;
uint8_t intensidade = 0;
const uint8_t n_networks = 10;
uint8_t ids[n_networks];


textEffect_t textEffect_in_D = PA_PRINT;textEffect_t textEffect_out_D = PA_NO_EFFECT;
textEffect_t textEffect_in_H = PA_PRINT;textEffect_t textEffect_out_H = PA_NO_EFFECT;

textPosition_t textPosition = PA_CENTER;
boolean ind_tocou;boolean acertar_relogio=false;

void setup(){inicializa_Sistema();}

void inicializa_Sistema()
{
  Serial.begin(9600);
  cnt = 0;
  if (! rtc.begin()){while (1);}
  P.begin(2);P.setZone(0, 0, 7);P.setZone(1, 8, 15);P.setIntensity(intensidade);
  pinMode(buzzer_pin,OUTPUT);digitalWrite(buzzer_pin,HIGH);ind_tocou = false;
}

void loop()
{ 
  if (cnt == 0){inicializa_Sistema();actializa_hora();cnt++;}
  if (P.displayAnimate())
  {

    boolean bAllDone = true;
    for (uint8_t i=0; i<2 && bAllDone; i++)
    bAllDone = bAllDone && P.getZoneStatus(i);

    if (bAllDone)
    { // Fazer o que se pretende 
      obtem_info();
      P.displayZoneText(0, my_array_H, textPosition, SPEED_SCROLL, 0, textEffect_in_H, textEffect_out_H);
      P.displayZoneText(1, my_array_D, textPosition, SPEED_SCROLL, 0, textEffect_in_D, textEffect_out_D);
    }
  }
}

void obtem_info()
{ 
  uint8_t minuto = 0;uint8_t hora = 0;
  textEffect_in_D = PA_PRINT;textEffect_out_D = PA_NO_EFFECT;
  textEffect_in_H = PA_PRINT;textEffect_out_H = PA_NO_EFFECT;
  SPEED_SCROLL = 25;

  DateTime now = rtc.now();
  
  minuto = now.minute();
  hora = now.hour();
  sprintf(my_array_H, "%02d:%02d:%02d  %i\xF7",now.hour(),now.minute(),now.second(),int(rtc.getTemperature()));
  sprintf(my_array_D, "%02d-%02d-%04d",now.day(),now.month(),now.year());
  
  if (minuto%5 == 0 && minuto != 30)
  {
    switch(now.dayOfTheWeek())
    {
      case 0:
        strcpy(my_array_D,"Domingo");break;
      case 1:
        strcpy(my_array_D,"Segunda");break;
      case 2:
        strcpy(my_array_D,"\x54\x65\x72\x87\x61");break;
      case 3:
        strcpy(my_array_D,"Quarta");break;
      case 4:
        strcpy(my_array_D,"Quinta");break;
      case 5:
        strcpy(my_array_D,"Sexta");break;
      case 6:
        strcpy(my_array_D,"Sabado");break;
    }
  }

  if (minuto%30 == 0 && verifica_aniversario(my_array_D))
  {
    SPEED_SCROLL = 25;
    textEffect_in_H = PA_SCROLL_LEFT;textEffect_out_H = PA_SCROLL_LEFT;
  }

  if(minuto == 0)
  {
    if(hora < 7 || hora > 23){ind_tocou = true;}
    if(!ind_tocou)
    { 
      ind_tocou = true;
      if(hora > 12){hora = hora - 12;}
      
      while(hora > 0){Serial.println("Toca");digitalWrite(buzzer_pin,LOW);delay(70);digitalWrite(buzzer_pin,HIGH);delay(140);digitalWrite(buzzer_pin,LOW);delay(70);digitalWrite(buzzer_pin,HIGH);delay(700);hora--;}
    }
  }
  else {ind_tocou = false;}
  if(hora == 02 && minuto == 16){acertar_relogio = false;}
  if(hora == 02 && minuto == 18 && !acertar_relogio){actializa_hora();}
}
//
boolean verifica_aniversario(char *data)
{
  boolean existe = false;
    if(str_ncmp(data,"25-12-1946\0",5) == 0){str_cpy(my_array_D,"Aniversario\0");str_cpy(my_array_H,"Menino Jesus\0");existe = true;}
  return existe;
}

void actializa_hora()
{ 
  acertar_relogio = true;
  WiFi.mode(WIFI_STA);WiFi.disconnect();delay(100);scan_wifi();
}
void scan_wifi()
{
  int n = WiFi.scanNetworks();
  if (n == 0) {str_cpy(my_array_D,"Sem Redes\0");str_cpy(my_array_H,":(\0");} 
  else 
  {
    uint8_t id=0;
    for (uint8_t j = 0 ; (j < n_networks && j < n) ; ++j)
    {
      long rssi=-999;
      for (int i = 0; i < n; ++i) 
      {
        if (rssi<=WiFi.RSSI(i))
        {
          bool existe = false;
          for (uint8_t z = 0 ; z < j; ++z){if(i == ids[z]){existe = true;break;}}
          if (!existe){rssi = WiFi.RSSI(i);id = i;}
        }     
      }
      ids[j] = id;
    }
    //Serial.println("________________Lista Ordenada ___________________");
    for (uint8_t j =0 ; (j < n_networks && j < n) ; ++j)
    {
      if(ligar_rede(WiFi.SSID(ids[j]))) // neste momento o ESP8266 esta ligado a uma rede...
      {
        str_cpy(my_array_D,"WiFi\0");
        char rede_id[25];
        char nrede[75];
        WiFi.SSID(ids[j]).toCharArray(rede_id,25);
        strcpy(nrede, rede_id);strcat(nrede, "        \0");strcat(nrede, rede_id);strcat(nrede, "        \0");strcat(nrede, rede_id);
        str_cpy(my_array_H,nrede);
        
        SPEED_SCROLL = 40;
        textEffect_in_H = PA_SCROLL_LEFT;textEffect_out_H = PA_SCROLL_LEFT;
        mostra_mensagem1();
        obter_data_hora();break;
      }
    }
  }
}

boolean ligar_rede(String ssid)
{
  boolean ligacao_ok = false;
  char* password = "";
  uint8_t n_tentativas = 0;
  if (ssid == "rede1"){password = "pass_rede1";WiFi.begin(ssid, password);}
  else{WiFi.begin("","");}
  while (WiFi.status() != WL_CONNECTED && n_tentativas < 6) {
    delay(500);
    //Serial.print(".");
    ++n_tentativas;
  }
  return (WiFi.status() == WL_CONNECTED);
}

void obter_data_hora()
{
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      if (http.begin(client, "http://worldtimeapi.org/api/timezone/Europe/Lisbon")) {  // HTTP
      //if (http.begin(client, "http://worldtimeapi.org/api/timezone/Australia/Eucla")) {  // HTTP + 08:45
      //if (http.begin(client, "http://worldtimeapi.org/api/timezone/Canada/Newfoundland")) {  // HTTP -02:30
      //if (http.begin(client, "http://worldtimeapi.org/api/timezone/Asia/Tokyo")) {
        int httpCode = http.GET();
        if (httpCode > 0) {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            readReponseContent();
          }
        } 
        else {Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());}
        http.end();
      } 
      else {Serial.printf("[HTTP} Unable to connect\n");}
  }
}

void readReponseContent() {
  // https://arduinojson.org/v6/assistant/
  const size_t capacity = JSON_OBJECT_SIZE(11) + 1024;
  DynamicJsonDocument doc(capacity);
  
  DeserializationError error = deserializeJson(doc,client);
  if (error){return;}
  long longUnixTime = doc["unixtime"].as<long>();
  String longutcoffset = doc["utc_offset"].as<char*>();
  long offset = (longutcoffset.substring(1,3).toInt()*3600) + (((longutcoffset.substring(4).toInt())*3600)/60);
  
  long res;
  
  if (longutcoffset[0] == '+'){res = longUnixTime + offset;}
  else{res = longUnixTime - offset;}
  rtc.adjust(DateTime(res));
}

void mostra_mensagem1()
{
  while(true)
  {
    P.displayAnimate();
    if (P.getZoneStatus(0) && P.getZoneStatus(1))
    {
      P.displayClear();
      P.displayZoneText(0, my_array_H, textPosition, SPEED_SCROLL, 0, textEffect_in_H, textEffect_out_H);
      P.displayZoneText(1, my_array_D, textPosition, SPEED_SCROLL, 0, textEffect_in_D, textEffect_out_D);
      P.synchZoneStart();
      break;
    }
  }
}


// *****************************************************************************************************************************************************************************************
// > Função que copia um array de carracteres (prt_source) para outro (prt_dest) e devolve um apontador para o array de copia
// *****************************************************************************************************************************************************************************************
char* str_cpy(char *prt_dest,char *prt_source)
{
  int print_len = str_len(prt_source);
  for (int x=0;x< print_len;x++){*(prt_dest) = *(prt_source);prt_source++;prt_dest++;}
  *(prt_dest) = '\0';prt_dest -= print_len;
  return prt_dest;
}

// *****************************************************************************************************************************************************************************************
// > Função que compara dois arrays de caracteres segundo sua ordem alfabética para as primeiras x posições
//   Retorna  valor negativo se 1º array menor que 2º array;
//   Retorna  zero se 1º array igual ao 2º array;
//   Retorna  valor positivo se 1º array maior que 2º array;
// *****************************************************************************************************************************************************************************************
int str_ncmp(char *prt_str1,char *prt_str2, int num_chars){
  int res = 0;int str1_len = str_len(prt_str1);int str2_len = str_len(prt_str2);
  for (int x = 0;((x < num_chars) && ((x <= str1_len) || (x <= str2_len)));x++){
    res = *(prt_str1) - *(prt_str2);
    if (res != 0){break;}
    prt_str1++;prt_str2++;
  }
  return res;
}

// *****************************************************************************************************************************************************************************************
// > Funcao para retornar um inteiro com o comprimento de um array de caracteres (String)
// *****************************************************************************************************************************************************************************************
int str_len(char *prt_str){
  int size_str = 0;
  while (*(prt_str) != '\0'){size_str +=1;prt_str++;}
  return size_str;
}
