#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include "encoder.hpp"
#include "menu.hpp"

#define BAR_INDENT 1


#define FIRST_ENCODER_S1_PIN  2
#define FIRST_ENCODER_S2_PIN  3
#define FIRST_ENCODER_KEY_PIN 4
#define SECOND_ENCODER_S1_PIN  7
#define SECOND_ENCODER_S2_PIN  6
#define SECOND_ENCODER_KEY_PIN 5

#define CHECK_S1_1  !digitalRead(FIRST_ENCODER_S1_PIN)
#define CHECK_S2_1  !digitalRead(FIRST_ENCODER_S2_PIN)
#define CHECK_KEY_1 !digitalRead(FIRST_ENCODER_KEY_PIN)
#define CHECK_S1_2  !digitalRead(SECOND_ENCODER_S1_PIN)
#define CHECK_S2_2  !digitalRead(SECOND_ENCODER_S2_PIN)
#define CHECK_KEY_2 !digitalRead(SECOND_ENCODER_KEY_PIN)

#define DISPLAY_CLOCK 8
#define DISPLAY_DATA  9
#define DISPLAY_CS    10
#define DISPLAY_RESET 11

#define DRAW_MENU                       \
        if(menu.drawingRequred()){      \
          menu.drawed();                \
          draw_menu();                  \
        }


enum MODES
{
  LAMP,
  SPOTLIGHT,
  COLOR,
  CANDLE,
  AMBIENT,
  COUNT_OF_MODES
};


U8G2_HX1230_96X68_F_3W_SW_SPI display(U8G2_R0, DISPLAY_CLOCK, DISPLAY_DATA, DISPLAY_CS, DISPLAY_RESET);
Encoder enc1(FIRST_ENCODER_S1_PIN, FIRST_ENCODER_S2_PIN, FIRST_ENCODER_KEY_PIN);
Encoder enc2(SECOND_ENCODER_S1_PIN, SECOND_ENCODER_S2_PIN, SECOND_ENCODER_KEY_PIN);
Setting menu = Setting();




void setup() 
{
  display.begin();
}

void loop() 
{
  int cmd1 = enc1.check();
  int cmd2 = enc2.check();

  if (cmd1 == BUTTON)
    menu.nextLine();

  else if (cmd1 == RIGHT_KEY)
    menu.nextScreen();

  else if (cmd1 == LEFT_KEY)
    menu.previousScreen();

  else if (cmd1 == LEFT or cmd1 == RIGHT)
  {
    if (menu.screen == LAMP or (menu.screen == SPOTLIGHT and menu.line == 0))
    {
      if (cmd1 == LEFT)
        menu.warmthDown();
      else
        menu.warmthUp();
    }

    else if (menu.screen == SPOTLIGHT and menu.line == 1)
    {
      if (cmd1 == LEFT)
        menu.positionDown();
      else
        menu.positionUp();
    }

    else if (menu.screen == COLOR)
    {
      if (cmd1 == LEFT)
        menu.colorDown(menu.line);
      else
        menu.colorUp(menu.line);
    }
  }

  if (cmd2 == RIGHT)
    menu.brightnessUp();

  else if (cmd2 == LEFT)
    menu.brightnessDown();

  DRAW_MENU;
}


int draw_menu()
{
  display.clearBuffer();

  display.drawLine(0,13,96,13);
  display.drawLine(0,57,96,57);

  display.setFont(u8g2_font_unifont_t_symbols);
  display.drawUTF8(4, 71, "☀");
  display.setFont(u8g2_font_ncenB08_tr);
  
  display.drawFrame(20,59,75,9);
  display.drawBox(21 + BAR_INDENT, 60 + BAR_INDENT, (menu.brightness / 100.0 * (73 - 2*BAR_INDENT)), 7 - 2*BAR_INDENT);

  switch(menu.screen)
  {
    case (LAMP):
    {
      display.drawStr(32,10,"LAMP");
      display.drawStr(5, 25, "Warmth");
      display.drawFrame(4, 28, 88, 10);
      display.drawBox((46 + menu.warmth/100.0 * (41 - BAR_INDENT)), 29 + BAR_INDENT, 4, 8 - 2*BAR_INDENT);
      display.setFont(u8g2_font_4x6_tr);
      display.drawStr(5, 44, "2700K");
      display.drawStr(72, 44, "6500K");
      display.setFont(u8g2_font_ncenB08_tr);
      break;
    }

    case (SPOTLIGHT):
    {
      display.drawStr(16, 10, "SPOTLIGHT");
      if (menu.line == 0)
      {
        display.drawStr(5, 25, "Warmth");
        display.drawFrame(4, 28, 88, 10);
        display.drawBox((46 + menu.warmth/100.0 * (41 - BAR_INDENT)), 29 + BAR_INDENT, 4, 8 - 2*BAR_INDENT);
        display.setFont(u8g2_font_4x6_tr);
        display.drawStr(5, 44, "2700K");
        display.drawStr(72, 44, "6500K");
        display.setFont(u8g2_font_ncenB08_tr);
        display.drawStr(40, 55, "1/2");
      }
      
      else if (menu.line == 1)
      {
        display.drawStr(5, 25, "Position");
        display.drawFrame(4, 28, 88, 10);
        display.drawBox((44 + menu.spotlight_position/50.0 * (39 - BAR_INDENT)), 29 + BAR_INDENT, 8, 8 - 2*BAR_INDENT);
        display.drawStr(40, 55, "2/2");
      }
      break;
    }

    case (COLOR):
    {
      display.drawStr(28, 10, "COLOR");
      
      display.drawStr(15, 25, "R");
      display.drawStr(15, 39, "G");
      display.drawStr(15, 53, "B");
      
      display.drawFrame(25, 16, 70, 10);
      display.drawFrame(25, 30, 70, 10);
      display.drawFrame(25, 44, 70, 10);

      display.drawBox(26 + BAR_INDENT, 17 + BAR_INDENT, menu.red / 255.0 * (68 - 2*BAR_INDENT), 8 - 2*BAR_INDENT);
      display.drawBox(26 + BAR_INDENT, 31 + BAR_INDENT, menu.green / 255.0 * (68 - 2*BAR_INDENT), 8 - 2*BAR_INDENT);
      display.drawBox(26 + BAR_INDENT, 45 + BAR_INDENT, menu.blue / 255.0 * (68 - 2*BAR_INDENT), 8 - 2*BAR_INDENT);

      display.drawStr(5, 24 + menu.line * 14, ">");
      
      break;
    }

    case (CANDLE):
    {
      display.drawStr(24, 10, "CANDLE");
      display.drawLine(0,13,96,57);
      display.drawLine(0,57,96,13);
      break;
    }

    case (AMBIENT):
    {
      display.drawStr(20, 10, "AMBIENT");
      display.drawStr(5, 35, "Mode:");
      display.setCursor(50, 35);
      display.print(menu.ambient_mode + 1);
      break;
    }

    default:
    {
      display.drawStr(26, 10, "ERROR");
      break;
    }
  }

  display.sendBuffer();
}
