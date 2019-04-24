/*
 * DISPLAY  (HX1230)
 *    GND -> GND
 *    BL -> 3V3
 *    VCC -> 5V
 *    CLK -> D4
 *    DIN -> D5
 *    N/C is not connected
 *    CE -> D7
 *    RST -> D8
 *    
 * ENCODER
 *    5V -> 5V
 *    KEY -> D9
 *    S1 -> D3
 *    S2 -> D2
 *    
 * LED
 *    short pin -> GND
 *    tall pin -> D6
 */


#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>

// define pins for display  (VCC -> 5V, BL -> 3V3, GND -> GND)
#define DISPLAY_CLOCK 4   // CLK pin on display
#define DISPLAY_DATA  5   // DIN pin on display
#define DISPLAY_CS    7   // CE pin on display
#define DISPLAY_RESET 8   // RST pin on display

// define pins for encoder    (5V -> 5V, GND -> GND)
#define ENCODER_KEY 9       // KEY pin on encoder
#define ENCODER_S1  3       // S1 pin on sensor (Left rotation)
#define ENCODER_S2  2       // S2 pin on sensor (Right rotation)


#define LED 6


U8G2_HX1230_96X68_F_3W_SW_SPI display(U8G2_R0, DISPLAY_CLOCK, DISPLAY_DATA, DISPLAY_CS, DISPLAY_RESET);
int TimerPosition = 0;
int timer = 0;
bool TimerStarted = 0;
bool LedEnabled = 0;


int checkEncoder();


void setup(void) 
{
  Serial.begin(9600);
  display.begin();
  drawTime(TimerPosition);
}

void loop(void) 
{
  bool kostil = 0;
  int prev_state = TimerStarted;
  int prev_time = TimerPosition;
  int cmd = checkEncoder();

  if(!TimerStarted)
  {
    if(cmd == 1) TimerPosition++;
    if(cmd == -1) 
    {
      TimerPosition--;
      if(TimerPosition < 0)
      {
        TimerPosition++;
        kostil = 1;
      }
    }
  }
  
  if(cmd == 2)
  {
    if(TimerStarted)
    {
      if(LedEnabled)
      {
        LedEnabled = 0;
        analogWrite(LED, 0);
      }
    }
    TimerStarted = !TimerStarted;
  }

  if(timer > 1000)
  {
    if(TimerStarted and TimerPosition)
      TimerPosition--;
    timer = 0;
  }
  
  if(timer > 200 and TimerStarted and !TimerPosition)
  {
    if(LedEnabled)
    {
      analogWrite(LED, 0);
      LedEnabled = 0;
    }
    else
    {
      analogWrite(LED, 700);
      LedEnabled = 1;
    }
    timer = 0;
  }


  bool smth_changed = (prev_time != TimerPosition) or (prev_state != TimerStarted) or (kostil);

  if(smth_changed)
  {
    drawTime(TimerPosition);
    timer = timer + 40;
  }
  
  delay(1);
  timer++;
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


void drawTime(int timestamp)
{
  int seconds = timestamp % 60;
  int minutes = (timestamp - seconds) / 60;
  int sec2 = seconds % 10;
  int sec1 = (seconds - sec2) / 10;
  int min2 = minutes % 10;
  int min1 = (minutes - min2) / 10;
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);
  display.setCursor(35, 40);
  display.print(min1);
  display.setCursor(42, 40);
  display.print(min2);
  display.drawStr(50, 39, ":");
  display.setCursor(55, 40);
  display.print(sec1);
  display.setCursor(62, 40);
  display.print(sec2);
  if(TimerStarted and TimerPosition == 0)
    display.drawStr(15, 55, "Press to stop");
  if(TimerStarted and TimerPosition != 0)
    display.drawStr(13, 55, "Press to pause");
  if(!TimerStarted and TimerPosition == 0)
    display.drawStr(15, 55, "Rotate to set");
  if(!TimerStarted and TimerPosition != 0)
    display.drawStr(15, 55, "Press to start");
  display.sendBuffer();
}




//end;
