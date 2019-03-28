#include <Wire.h>        //Biblioteca para manipulação do protocolo I2C
#include <DS3231.h>      //Biblioteca para manipulação do DS3231

DS3231  rtc(A4,A5);              //Criação do objeto do tipo DS3231
//RTCDateTime dataehora;   //Criação do objeto do tipo RTCDateTime
 
void setup()
{
  String str_mes;
  int dia;int mes;int ano;
  int hora, minuto, segundo;
  rtc.begin();            //Inicialização do RTC DS3231
  String horario = __TIME__;
  hora = ( horario.substring(0,2)).toInt();
  minuto = ( horario.substring(3,5)).toInt();
  segundo = ( horario.substring(6,8)).toInt() + 11;
  rtc.setTime(hora,minuto,segundo);

  String data = __DATE__;

  str_mes = data.substring(0,3);
  dia = (data.substring(4,6)).toInt();
  ano = (data.substring(7,11)).toInt();
  
  if     ( str_mes == "Jan" ) mes = 1;
  else if( str_mes == "Feb" ) mes = 2;
  else if( str_mes == "Mar" ) mes = 3;
  else if( str_mes == "Apr" ) mes = 4;
  else if( str_mes == "May" ) mes = 5;
  else if( str_mes == "Jun" ) mes = 6;
  else if( str_mes == "Jul" ) mes = 7;
  else if( str_mes == "Aug" ) mes = 8;
  else if( str_mes == "Sep" ) mes = 9;
  else if( str_mes == "Oct" ) mes = 10;
  else if( str_mes == "Nov" ) mes = 11;
  else if( str_mes == "Dec" ) mes = 12;
  else exit(-1);
  rtc.setDate(dia,mes,ano);
  
}
 
void loop()
{
//  dataehora = rtc.getDateTime();     //Atribuindo valores instantâneos de 
//                                     //data e hora à instância dataehora
// 
// 
//  Serial.print(dataehora.year);     //Imprimindo o Ano   
//  Serial.print("-");
//  Serial.print(dataehora.month);    //Imprimindo o Mês
//  Serial.print("-");
//  Serial.print(dataehora.day);      //Imprimindo o Dia
//  Serial.print(" ");
//  Serial.print(dataehora.hour);     //Imprimindo a Hora
//  Serial.print(":");
//  Serial.print(dataehora.minute);   //Imprimindo o Minuto
//  Serial.print(":");
//  Serial.print(dataehora.second);   //Imprimindo o Segundo
//  Serial.println("");
 
  delay(1000);     //Tempo p atualização do valor enviado pela porta
                   //serial
}
