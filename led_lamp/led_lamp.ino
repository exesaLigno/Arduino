#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>

#define ENCODER_S1 2
#define ENCODER_S2 3
#define ENCODER_KEY 4

#define LED_RING_IN 6

#define DISPLAY_RESET 8
#define DISPLAY_CS 9
#define DISPLAY_DATA 10
#define DISPLAY_CLOCK 11


unsigned int brightness = 20;
unsigned int lamp_temperature = 4000;
unsigned long int color = 16777215;

int lines_count[6] = {3, 2, 2, 2, 2, 5};
int current_mode = 0;
int current_line = 0;


U8G2_HX1230_96X68_F_3W_SW_SPI display(U8G2_R0, DISPLAY_CLOCK, DISPLAY_DATA, DISPLAY_CS, DISPLAY_RESET);


class Ring
{
  const int DISABLED = 0;
  const int LAMP = 1;
  const int CANDLE = 2;
  const int AMBIENT = 3;
  const int STROBE = 4;
  const int RAINBOW = 5;
  const int COLOR = 6;
  
  int mode = DISABLED;
  int updated = false;
  unsigned long int current_color[16] = {};
  unsigned long int target_color[16] = {};
  
  int changeMode();
  int getNextColor();
  int writeColor();
};


void setup() 
{
  display.begin();  
}

void loop() 
{
  int command = checkEncoder();
  if (command == 2)
  {
    current_line = (current_line + 1) % lines_count[current_mode];
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.setCursor(10,10);
    display.print(current_line);
    display.setCursor(10,30);
    display.print(current_mode);
    display.sendBuffer();
  }
  else if (command == 1 and current_line == 0)
  {
    current_mode = (current_mode + 1) % 6;
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.setCursor(10,10);
    display.print(current_line);
    display.setCursor(10,30);
    display.print(current_mode);
    display.sendBuffer();
  }
  else if (command == -1 and current_line == 0)
  {
    current_mode = (current_mode - 1) % 6;
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.setCursor(10,10);
    display.print(current_line);
    display.setCursor(10,30);
    display.print(current_mode);
    display.sendBuffer();
  }
}


int Ring::getNextColor()
{
  return 0;
}


int Ring::changeMode()
{
  return 0;
}


int Ring::writeColor()
{
  return 0;
}


int checkEncoder()
{
  int encoder_command = 0;
  
  if(!digitalRead(ENCODER_S1))
  {
    encoder_command = -1;
    while(!digitalRead(ENCODER_S1));
  }
  
  else if(!digitalRead(ENCODER_S2))
  {
    encoder_command = 1;
    while(!digitalRead(ENCODER_S2));
  }

  if(!digitalRead(ENCODER_KEY)) 
  {
    encoder_command = 2;
    while(!digitalRead(ENCODER_KEY));
  }

  return encoder_command;
}
