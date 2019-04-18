#include<stdio.h>
#include <DS3231.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16
#define DATA_PIN_H  12
#define CS_PIN_H    11
#define CLK_PIN_H   10

#define buzzer_pin 5  // buzzer pin
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN_H, CLK_PIN_H, CS_PIN_H, MAX_DEVICES);

DS3231  rtc(A4,A5);

uint8_t SPEED_SCROLL = 25;
char my_array_H[75];            // Character array for printing something.
char my_array_D[75];            // Character array for printing something.
uint8_t cnt = 0;
uint8_t intensidade = 1;
uint8_t minuto = 0;

textEffect_t textEffect_in_D = PA_PRINT;textEffect_t textEffect_out_D = PA_NO_EFFECT;
textEffect_t textEffect_in_H = PA_PRINT;textEffect_t textEffect_out_H = PA_NO_EFFECT;

textPosition_t textPosition = PA_CENTER;
boolean ind_tocou;

void setup()
{
  inicializa_Sistema();
}

void inicializa_Sistema()
{
  cnt = 0;
  rtc.begin();
  P.begin(2);
  P.setZone(0, 0, 7);
  P.setZone(1, 8, 15);
  P.setIntensity(0);
  pinMode(buzzer_pin,OUTPUT);
  digitalWrite(buzzer_pin,HIGH);
  ind_tocou = false;

}

void loop()
{ 
  if (cnt == 0){inicializa_Sistema();cnt++;}

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
  textEffect_in_D = PA_PRINT;textEffect_out_D = PA_NO_EFFECT;
  textEffect_in_H = PA_PRINT;textEffect_out_H = PA_NO_EFFECT;
  SPEED_SCROLL = 25;
  
  minuto = rtc.getTime().min;
  char *str_data = rtc.getDateStr(FORMAT_LONG,FORMAT_LITTLEENDIAN,'-');

  sprintf(my_array_H, "%s  %i\xF7",rtc.getTimeStr(),int(rtc.getTemp()));
  sprintf(my_array_D, " %s",str_data);

  if (minuto%5 == 0 && minuto != 30)
  {
    switch(rtc.getTime().dow-1)
    {
      case 6:
        strcpy(my_array_D,"Domingo");break;
      case 0:
        strcpy(my_array_D,"Segunda");break;
      case 1:
        strcpy(my_array_D,"\x54\x65\x72\x87\x61");break;
      case 2:
        strcpy(my_array_D,"Quarta");break;
      case 3:
        strcpy(my_array_D,"Quinta");break;
      case 4:
        strcpy(my_array_D,"Sexta");break;
      case 5:
        strcpy(my_array_D,"Sabado");break;
    }
  }


  if (minuto%30 == 0 && verifica_aniversario(str_data))
  {
    SPEED_SCROLL = 15;
    textEffect_in_H = PA_SCROLL_LEFT;textEffect_out_H = PA_SCROLL_LEFT;
  }

  if(minuto == 0)
  {
    uint8_t n_hora = rtc.getTime().hour;
    if(n_hora < 7 || n_hora > 23){ind_tocou = true;}
    if(!ind_tocou)
    { 
      ind_tocou = true;
      if(n_hora > 12){n_hora = n_hora - 12;}
      while(n_hora > 0){digitalWrite(5,LOW);delay(70);digitalWrite(5,HIGH);delay(140);digitalWrite(5,LOW);delay(70);digitalWrite(5,HIGH);delay(700);n_hora--;}
      
    }
  }
  else {ind_tocou = false;}
}

boolean verifica_aniversario(char *data)
{
  boolean existe = false;
    if(str_ncmp(data,"01-01-1981\0",5) == 0){str_cpy(my_array_D,"Aniversario\0");str_cpy(my_array_H,"Andriolina\0");existe = true;}

  return existe;
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
