#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <FastLED.h>
#include "encoder.hpp"
#include "menu.hpp"


#define ENCODER_S1_PIN  4
#define ENCODER_S2_PIN  3
#define ENCODER_KEY_PIN 2


#define LED_PIN 12
#define CURRENT_LIMIT 400


static const int NUM_LEDS_RING = 16;
static const int NUM_LEDS_SETTINGS_STRIP = 0;
static const int NUM_LEDS_LAMP = 0;

#define NUM_LEDS  NUM_LEDS_RING+NUM_LEDS_SETTINGS_STRIP+NUM_LEDS_LAMP


enum MODES
{
  LAMP,
  SPOTLIGHT,
  COLOR,
  CANDLE,
  AMBIENT,
  COUNT_OF_MODES
};


Encoder encoder(ENCODER_S1_PIN, ENCODER_S2_PIN, ENCODER_KEY_PIN, 10);
Settings settings = Settings();

CRGB leds[NUM_LEDS];
CRGB target_leds[NUM_LEDS];

int brightness = 0;
int color_changing = 0;

unsigned long last_updated = 0;

void setup() 
{
  //Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  FastLED.setBrightness(255);
}


void loop() 
{
  bool parameter_changed = read_command();

  if (parameter_changed)
  {
    if (!settings.parameter_changing)
      draw_ring(Settings::BRIGHTNESS);
    else
      draw_ring(settings.current_line);
  }
  
}

/*
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
*/


int draw_ring(int parameter)
{
  switch(parameter)
  {
    case Settings::BRIGHTNESS:
    {
      for (int counter = 0; counter < NUM_LEDS_RING; counter++)
      {
        if (counter * 15 < settings.parameters[Settings::BRIGHTNESS])
        {
          leds[NUM_LEDS_RING - counter - 1].r = 255 * ((counter + 1.0)/(NUM_LEDS_RING - 0.0));
          leds[NUM_LEDS_RING - counter - 1].g = 255 * ((counter + 1.0)/(NUM_LEDS_RING - 0.0));
          leds[NUM_LEDS_RING - counter - 1].b = 255 * ((counter + 1.0)/(NUM_LEDS_RING - 0.0));
        }
        else
        {
          leds[NUM_LEDS_RING - counter - 1].r = 0;
          leds[NUM_LEDS_RING - counter - 1].g = 0;
          leds[NUM_LEDS_RING - counter - 1].b = 0;
        }
      }
      break;
    }

    case Settings::TEMPERATURE:
    {
      leds[0].r = 0;
      leds[0].g = 0;
      leds[0].b = 0;
      leds[NUM_LEDS_RING - 1].r = 0;
      leds[NUM_LEDS_RING - 1].g = 0;
      leds[NUM_LEDS_RING - 1].b = 0;
      for (int counter = 0; counter < NUM_LEDS_RING - 2; counter++)
      {
        if (1600 + (counter * 430) <= settings.parameters[Settings::TEMPERATURE])
        {
          leds[NUM_LEDS_RING - counter - 2].r = get_red(settings.parameters[parameter]);
          leds[NUM_LEDS_RING - counter - 2].g = get_green(settings.parameters[parameter]);
          leds[NUM_LEDS_RING - counter - 2].b = get_blue(settings.parameters[parameter]);
        }
        else
        {
          leds[NUM_LEDS_RING - counter - 2].r = 0;
          leds[NUM_LEDS_RING - counter - 2].g = 0;
          leds[NUM_LEDS_RING - counter - 2].b = 0;
        }
      }
      break;
    }

    case Settings::SPOTLIGHT:
    {
      leds[NUM_LEDS_RING/2].r = 100;
      leds[NUM_LEDS_RING/2].g = 100;
      leds[NUM_LEDS_RING/2].b = 100;
      break;
    }

    case Settings::COLOR:
    {
      leds[NUM_LEDS_RING/2].r = 100;
      leds[NUM_LEDS_RING/2].g = 0;
      leds[NUM_LEDS_RING/2].b = 0;
      break;
    }

    case Settings::CANDLE:
    {
      leds[NUM_LEDS_RING/2].r = 80;
      leds[NUM_LEDS_RING/2].g = 80;
      leds[NUM_LEDS_RING/2].b = 0;
      break;
    }

    case Settings::AMBIENT:
    {
      leds[NUM_LEDS_RING/2].r = 80;
      leds[NUM_LEDS_RING/2].g = 0;
      leds[NUM_LEDS_RING/2].b = 80;
      break;
    }
  }

  FastLED.show();
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
  int event = encoder.checkEvent();

  bool smth_changed = false;

  if (event == LEFT_RELEASED)
    smth_changed = settings.changeParameter(Settings::BRIGHTNESS, -1);
  else if (event == RIGHT_RELEASED)
    smth_changed = settings.changeParameter(Settings::BRIGHTNESS, 1);
  else if (event == RIGHT_KEY_PRESSED or event == LEFT_KEY_PRESSED)
  {
    settings.parameter_changing = true;
    smth_changed = true;
  }
  else if (event == KEY_RELEASED)
  {
    settings.nextMode();
    present_mode();
    settings.parameter_changing = false;
    smth_changed = true;
  }
  else if (event == KEY_RELEASED_AFTER_ROTATING)
  {
    settings.parameter_changing = false;
    smth_changed = true;
  }
  else if (event == LEFT_KEY_RELEASED)
  {
    smth_changed = settings.changeParameter(settings.current_line, -1);
  }
  else if (event == RIGHT_KEY_RELEASED)
  {
    smth_changed = settings.changeParameter(settings.current_line, 1);
  }
  
  return smth_changed;
}


bool present_mode()
{
  switch(settings.current_line)
  {
    case Settings::TEMPERATURE:
    {
      for (int counter = 0; counter < 20; counter++)
      {
        for (int k = 0; k < NUM_LEDS_RING; k++)
        {
          leds[k].r = counter * 10;
          leds[k].g = counter * 10;
          leds[k].b = counter * 10;
        }
        FastLED.show();
        delay(16);
      }
      //delay(190);
      for (int counter = 19; counter >= 0; counter--)
      {
        for (int k = 0; k < NUM_LEDS_RING; k++)
        {
          leds[k].r = counter * 10;
          leds[k].g = counter * 10;
          leds[k].b = counter * 10;
        }
        FastLED.show();
        delay(16);
      }
      break;
    }

    case Settings::SPOTLIGHT:
    {
      for (int counter = 0; counter < NUM_LEDS_RING; counter++)
      {
        for (int k = 0; k < NUM_LEDS_RING; k++)
        {
          if (k == counter)
          {
            leds[k].r = counter * 10;
            leds[k].g = counter * 10;
            leds[k].b = counter * 10;
          }
          else
          {
            leds[k].r = 0;
            leds[k].g = 0;
            leds[k].b = 0;
          }
        }
        FastLED.show();
        delay(20);
      }
      for (int counter = NUM_LEDS_RING - 1; counter >= 0; counter--)
      {
        for (int k = 0; k < NUM_LEDS_RING; k++)
        {
          if (k == counter)
          {
            leds[k].r = counter * 10;
            leds[k].g = counter * 10;
            leds[k].b = counter * 10;
          }
          else
          {
            leds[k].r = 0;
            leds[k].g = 0;
            leds[k].b = 0;
          }
        }
        FastLED.show();
        delay(20);
      }
      break;
    }

    case Settings::COLOR:
    {
      for (int counter = 0; counter < 20; counter++)
      {
        for (int k = 0; k < NUM_LEDS_RING; k++)
        {
          leds[k].r = counter * 10;
          leds[k].g = counter * 10;
          leds[k].b = counter * 10;
        }
        FastLED.show();
        delay(16);
      }
      //delay(190);
      for (int counter = 19; counter >= 0; counter--)
      {
        for (int k = 0; k < NUM_LEDS_RING; k++)
        {
          leds[k].r = counter * 10;
          leds[k].g = counter * 10;
          leds[k].b = counter * 10;
        }
        FastLED.show();
        delay(16);
      }
      break;
    }

    case Settings::AMBIENT:
    {
      for (int counter = 0; counter < 20; counter++)
      {
        for (int k = 0; k < NUM_LEDS_RING; k++)
        {
          leds[k].r = counter * 10;
          leds[k].g = counter * 2.5;
          leds[k].b = counter * 12.5;
        }
        FastLED.show();
        delay(30);
      }
      //delay(190);
      for (int counter = 19; counter >= 0; counter--)
      {
        for (int k = 0; k < NUM_LEDS_RING; k++)
        {
          leds[k].r = counter * 10;
          leds[k].g = counter * 2.5;
          leds[k].b = counter * 12.5;
        }
        FastLED.show();
        delay(30);
      }
      break;
    }
  }
  return true;
}
