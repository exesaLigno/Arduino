#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <FastLED.h>
#include "encoder.hpp"
#include "menu.hpp"

#define BAR_INDENT 1


#define FIRST_ENCODER_S1_PIN  2
#define FIRST_ENCODER_S2_PIN  3
#define FIRST_ENCODER_KEY_PIN 4
#define SECOND_ENCODER_S1_PIN  7
#define SECOND_ENCODER_S2_PIN  6
#define SECOND_ENCODER_KEY_PIN 5

#define DISPLAY_CLOCK 8
#define DISPLAY_DATA  9
#define DISPLAY_CS    10
#define DISPLAY_RESET 11

#define LED_PIN 12


#define NUM_LEDS 16
#define CURRENT_LIMIT 450
#define SPOTLIGHT_LEDS 5


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

CRGB leds[NUM_LEDS];
CRGB target_leds[NUM_LEDS];
int brightness = 0;
int color_changing = 0;

unsigned long last_updated = 0;

void setup() 
{
  display.begin();
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  draw_menu();
  FastLED.setBrightness(255);
}


void loop() 
{
  bool command_received = read_command();
  
  if(command_received)
    draw_menu();

  if(menu.screen == LAMP or menu.screen == SPOTLIGHT or menu.screen == COLOR)
  {
    if(command_received)
    {
      set_target_color();
    }

    if(color_changing and (abs(millis() - last_updated) > 25))
    {
      change_color();
      //FastLED.setBrightness(brightness * 2.55);
      FastLED.show();
      last_updated = millis();
    }
  }
}


int set_target_color()
{
  color_changing = 15;
  
  int temp = (4100 + (menu.warmth * 24));
  float red = get_red(temp);
  float green = get_green(temp);
  float blue = get_blue(temp);
  
  for (int counter = 0; counter < NUM_LEDS; counter++)
  {
    if (menu.screen == LAMP)
    {
      target_leds[counter].r = red;
      target_leds[counter].g = green;
      target_leds[counter].b = blue;
    }
    
    else if (menu.screen == SPOTLIGHT)
    {
      int spotlight_position = ((menu.spotlight_position + 50.0) / 100.0) * (NUM_LEDS - 1);

      if (abs(counter - spotlight_position) <= (SPOTLIGHT_LEDS-1)/2)
      {
        target_leds[counter].r = red;
        target_leds[counter].g = green;
        target_leds[counter].b = blue;
      }
      else if(abs(counter - spotlight_position) <= (SPOTLIGHT_LEDS+3)/2)
      {
        target_leds[counter].r = red / (2 * (abs(counter - spotlight_position) - (SPOTLIGHT_LEDS-1)/2));
        target_leds[counter].g = green / (2 * (abs(counter - spotlight_position) - (SPOTLIGHT_LEDS-1)/2));
        target_leds[counter].b = blue / (2 * (abs(counter - spotlight_position) - (SPOTLIGHT_LEDS-1)/2));
      }
      else
      {
        target_leds[counter].r = 0;
        target_leds[counter].g = 0;
        target_leds[counter].b = 0;
      }
    }

    else if (menu.screen == COLOR)
    {
      target_leds[counter].r = menu.red;
      target_leds[counter].g = menu.green;
      target_leds[counter].b = menu.blue;
    }

    target_leds[counter].r *= (menu.brightness/100.0);
    target_leds[counter].g *= (menu.brightness/100.0);
    target_leds[counter].b *= (menu.brightness/100.0);
  }  
}


int change_color()
{
  int divider = color_changing;
  color_changing--;
  for (int counter = 0; counter < NUM_LEDS; counter++)
  {
    if (color_changing)
    {
      leds[counter].r = leds[counter].r + (target_leds[counter].r - leds[counter].r) / divider;
      leds[counter].g = leds[counter].g + (target_leds[counter].g - leds[counter].g) / divider;
      leds[counter].b = leds[counter].b + (target_leds[counter].b - leds[counter].b) / divider;
    }
    else
    {
      leds[counter].r = target_leds[counter].r;
      leds[counter].g = target_leds[counter].g;
      leds[counter].b = target_leds[counter].b;
    }
  }
  return 0;
}


float get_red(int temp)
{
  float red = 0;

  temp = temp / 100;
  
  if (temp <= 66)
    red = 255;

  else
  {
    red = temp - 60;
    red = 329.698727446 * pow(red, -0.1332047592);
    if (red < 0) 
      red = 0;
    if (red > 255)
      red = 255;
  }

  return red;
}


float get_green(int temp)
{
  float green = 0;

  temp = temp / 100;
  
  if (temp <= 66)
  {
    green = temp;
    green = 99.4708025861 * log(green) - 161.1195681661;
    if (green < 0) 
      green = 0;
    if (green > 255)
      green = 255;
  }
  
  else
  {
    green = temp - 60;
    green = 288.1221695283 * pow(green, -0.0755148492);
    if (green < 0) 
      green = 0;
    if (green > 255)
      green = 255;
  }

  return green;
}


float get_blue(int temp)
{
  float blue = 0;

  temp = temp / 100;

  if (temp >= 66)
    blue = 255;
  else
  {
    if (temp <= 19)
      blue = 0;
    else
    {
      blue = temp - 10;
      blue = 138.5177312231 * log(blue) - 305.0447927307;
      if (blue < 0) 
        blue = 0;
      if (blue > 255)
        blue = 255;
    }
  }

  return blue;
}


bool read_command()
{
  int cmd1 = enc1.check();
  int cmd2 = enc2.check();

  bool changed = false;

  if (cmd1 == BUTTON)
    changed = menu.nextLine();

  else if (cmd1 == RIGHT_KEY)
    changed = menu.nextScreen();

  else if (cmd1 == LEFT_KEY)
    changed = menu.previousScreen();

  else if (cmd1 == LEFT or cmd1 == RIGHT)
  {
    if (menu.screen == LAMP or (menu.screen == SPOTLIGHT and menu.line == 0))
    {
      if (cmd1 == LEFT)
        changed = menu.warmthDown();
      else
        changed = menu.warmthUp();
    }

    else if (menu.screen == SPOTLIGHT and menu.line == 1)
    {
      if (cmd1 == LEFT)
        changed = menu.positionDown();
      else
        changed = menu.positionUp();
    }

    else if (menu.screen == COLOR)
    {
      if (cmd1 == LEFT)
        changed = menu.colorDown(menu.line);
      else
        changed = menu.colorUp(menu.line);
    }
  }

  if (cmd2 == RIGHT)
    changed = menu.brightnessUp();

  else if (cmd2 == LEFT)
    changed = menu.brightnessDown();

  return changed;
}


int draw_menu()
{
  display.clearBuffer();

  display.drawLine(0,13,96,13);
  display.drawLine(0,57,96,57);

  // Неоптимальное использование памяти //
  // display.setFont(u8g2_font_unifont_t_symbols); // Жрет 9 кБ, 30% флеш памяти (!!!) обмазался гавном ради одного символа
  //display.drawUTF8(4, 71, "☀");                 // Не работает без предыдущей строки
  // Неоптимальное использование памяти //
  
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
      display.drawStr(5, 44, "1700K");
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
        display.drawStr(5, 44, "1700K");
        display.drawStr(72, 44, "6500K");
        display.setFont(u8g2_font_ncenB08_tr);
        display.drawStr(40, 55, "1/2");
      }
      
      else if (menu.line == 1)
      {
        display.drawStr(5, 25, "Position"); // 22 байта
        display.drawFrame(4, 28, 88, 10); // 12 байт
        display.drawBox((44 + menu.spotlight_position/50.0 * (39 - BAR_INDENT)), 29 + BAR_INDENT, 8, 8 - 2*BAR_INDENT); // 72 байта
        display.drawStr(40, 55, "2/2"); // 18 байт
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
      display.drawStr(20, 10, "AMBIENT"); // 20 байт
      display.drawStr(5, 35, "Mode:"); // 16 байт
      display.setCursor(50, 35); // 0 байт ????

      // Неоптимальное использование памяти //
      display.print(menu.ambient_mode + 1); // 292 байта ????????????????
      // Неоптимальное использование памяти //
      
      break;
    }

    default:
    {
      display.drawStr(26, 10, "ERROR"); // 14 байт
      break;
    }
  }

  display.sendBuffer();
}
