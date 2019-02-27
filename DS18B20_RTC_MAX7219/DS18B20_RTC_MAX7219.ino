// Use the Parola library to scroll text on the display
//
// Demonstrates the use of the scrolling function to display text received
// from the serial interface
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
// Speed for the display is controlled by a pot on SPEED_IN analog in.
// Scrolling direction is controlled by a switch on DIRECTION_SET digital in.
// Invert ON/OFF is set by a switch on INVERT_SET digital in.
//
// UISwitch library can be found at https://github.com/MajicDesigns/MD_UISwitch
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// Turn on debug statements to the serial output
#define DEBUG 0

#if DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CLK_PIN   11
#define DATA_PIN  12
#define CS_PIN    10

#define ONE_WIRE_BUS 3

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffectIn = PA_SCROLL_DOWN;
textEffect_t scrollEffectOut = PA_SCROLL_DOWN;
textPosition_t scrollAlign = PA_CENTER;
uint16_t scrollPause = 2000; // in milliseconds

// Global message buffers shared by Serial and Scrolling functions
#define  BUF_SIZE  75
char curMessage[BUF_SIZE] = { " " };
char newMessage[BUF_SIZE] = { " " };
bool newMessageAvailable = true;
bool existe_sensores = false;
float temp_Ant = 0.00;
float temp_Act = 0.00;

void setup()
{
  Serial.begin(57600);
  
  P.begin();
  for (uint8_t i=0 ; i<MAX_DEVICES ; i++){P.setIntensity(i,0);}
//  P.setFont(BigFont);
  //P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffectIn, scrollEffectOut);
  sensors.begin();
  if(!sensors.getAddress(sensor1, 0))
  {
    //Serial.println("Sensores nao encontrados !");
    scrollSpeed = 20;
    scrollEffectIn = PA_SCROLL_LEFT;scrollEffectOut = PA_SCROLL_LEFT;
    strcpy(curMessage, "Sensores nao encontrados !");
    P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffectIn, scrollEffectOut);
  }
  if (!existe_sensores){exit;} // termina execução script
  delay(1000);
  P.setTextAlignment(scrollAlign);
  scrollEffectIn = PA_SCROLL_UP;scrollPause = 0;
}

void loop()
{
  if (P.displayAnimate())
  {
    sensors.requestTemperatures();temp_Act = sensors.getTempC(sensor1);
    //Serial.print("Temp : ");Serial.print(temp_Act);Serial.println("");
    if (temp_Act != temp_Ant)
    {
      strcpy(curMessage, "Temp: ");
      dtostrf(temp_Act , 2, 2, newMessage);
      strcat( curMessage, newMessage );strcat( curMessage, String("\xF7").c_str() );
      
//      if (temp_Act < temp_Ant){scrollEffectIn = PA_SCROLL_DOWN;} // Temperatura Baixou
//      else{scrollEffectIn = PA_SCROLL_UP;} // Temperatura Subiu 
//      if (temp_Act < temp_Ant){scrollEffectIn = PA_SLICE;} // Temperatura Baixou
//      else{scrollEffectIn = PA_SLICE;} // Temperatura Subiu
//      if (temp_Act < temp_Ant){scrollEffectIn = PA_SCAN_HORIZ;} // Temperatura Baixou
//      else{scrollEffectIn = PA_SCAN_HORIZ;} // Temperatura Subiu
      if (temp_Act < temp_Ant){scrollEffectIn = PA_SCROLL_DOWN_LEFT;} // Temperatura Baixou
      else{scrollEffectIn = PA_SCROLL_UP_LEFT;} // Temperatura Subiu
      scrollPause = 0;scrollSpeed = 30;
      P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffectIn);P.displayReset();
      temp_Ant=temp_Act;
    }
    else{delay(60000);}
  }
}
