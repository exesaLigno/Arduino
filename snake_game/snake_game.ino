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
 * KEYBOARD (HW-136)
 *    GND -> GND
 *    VCC -> 5V
 *    SCL -> D2
 *    SDO -> D3
 */


#include <TTP229.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>


// define pins for display  (VCC -> 5V, BL -> 3V3, GND -> GND)
#define DISPLAY_CLOCK 4   // CLK pin on display
#define DISPLAY_DATA  5   // DIN pin on display
#define DISPLAY_CS    7   // CE pin on display
#define DISPLAY_RESET 8   // RST pin on display


// define pins for keyboard (VCC -> 5V, GND -> GND)
#define SCL_PIN 2         // SCL pin on keyboard
#define SDO_PIN 3         // SDO pin on keyboard


#define ASC_BUTTON 6
#define RESTART_BUTTON 1
#define UP_BUTTON 2
#define DOWN_BUTTON 10
#define LEFT_BUTTON 5
#define RIGHT_BUTTON 7

#define UP_VECTOR 1
#define LEFT_VECTOR 2
#define DOWN_VECTOR 3
#define RIGHT_VECTOR 4


struct SnakeBodySeg
{
  int x_coord = -1;
  int y_coord = -1;
};

struct Food
{
  bool exist = 0;
  int x_coord = 0;
  int y_coord = 0;
};


SnakeBodySeg Snake[100] = {};
int SnakeLength = 3;
Food current_food;
bool SnakeAlive = 1;
int current_head_vector = 0;
int timer = 0; // Тупа высер
int Score = 0;



int changeVector(int target_vector, int current_vector);
void drawFrame();
void generateFood();
void drawBlock(int x, int y);
void updateSnake(SnakeBodySeg new_segment);
bool checkMovement(int x, int y);



U8G2_HX1230_96X68_F_3W_SW_SPI display(U8G2_R0, DISPLAY_CLOCK, DISPLAY_DATA, DISPLAY_CS, DISPLAY_RESET);
TTP229 keyboard(SCL_PIN, SDO_PIN);


void setup() 
{
  Serial.begin(9600);
  display.begin();
  randomSeed(analogRead(0));
  Snake[0].x_coord = 48;
  Snake[0].y_coord = 35;
}


void loop() 
{  
  int key = keyboard.GetKey16();

  if(SnakeAlive)  // Change vector
  {
    if(key == UP_BUTTON) current_head_vector = changeVector(UP_VECTOR, current_head_vector);
    else if(key == DOWN_BUTTON) current_head_vector = changeVector(DOWN_VECTOR, current_head_vector);
    else if(key == LEFT_BUTTON) current_head_vector = changeVector(LEFT_VECTOR, current_head_vector);
    else if(key == RIGHT_BUTTON) current_head_vector = changeVector(RIGHT_VECTOR, current_head_vector);
  }

  if(!SnakeAlive and current_head_vector) // Stopping snake if it dies
  {
    current_head_vector = 0;
    updateSnake();
    drawFrame();
  }

  if(!SnakeAlive and !current_head_vector and key == RESTART_BUTTON)  // Restarting if snake dead
  {
    for(int counter = 0; counter < SnakeLength; counter++)
    {
      Snake[counter].x_coord = -1;
      Snake[counter].y_coord = -1;
    }
    SnakeLength = 3;
    Snake[0].x_coord = 48;
    Snake[0].y_coord = 35;
    SnakeAlive = 1;
  }

  if(!current_food.exist) generateFood(); // Regenerate food

  if((timer > 5 and SnakeAlive) or key == ASC_BUTTON) // Update and redraw snake
  {
    updateSnake();
    drawFrame();
    timer = 0;
  }
  timer++;
}


int changeVector(int target_vector, int current_vector) // Change movement vector
{
  if (current_vector == 0)
    return target_vector;
  else if (target_vector % 2 != current_vector % 2)
    return target_vector;
  else
    return current_vector;
}


void drawFrame()
{
  display.clearBuffer();
  display.drawLine(1, 0, 1, 67);
  display.drawLine(1, 67, 95, 67);
  display.drawLine(95, 67, 95, 0);
  display.drawLine(95, 0, 1, 0);

  if(SnakeAlive)  // Drawing snake
  {
    for(int counter = 0; counter < SnakeLength; counter++)
    {
      if(Snake[counter].x_coord != -1 and Snake[counter].y_coord != -1) drawBlock(Snake[counter].x_coord, Snake[counter].y_coord);
    }
    drawBlock(current_food.x_coord, current_food.y_coord);
    if(!current_head_vector)
    {
      display.setFont(u8g2_font_ncenB08_tr);
      display.drawStr(12, 63, "Move to start");
    }
  }
  
  else  // Drawing death screen
  {
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(19, 25, "YOU DEAD");
    display.drawStr(18, 40, "Score:");
    display.setCursor(57, 40);
    display.print(Score);
    display.drawStr(12, 63, "\"1\" to restart");
  }
  
  display.sendBuffer();
}


void drawBlock(int x, int y)
{
  display.drawPixel(x, y);
  display.drawPixel(x-1, y);
  display.drawPixel(x, y-1);
  display.drawPixel(x+1, y);
  display.drawPixel(x, y+1);
  display.drawPixel(x+1,y+1);
  display.drawPixel(x-1,y-1);
  display.drawPixel(x+1,y-1);
  display.drawPixel(x-1,y+1);
}


void updateSnake()
{
  SnakeBodySeg new_segment;   // Head after movement
  new_segment.x_coord = Snake[0].x_coord;
  new_segment.y_coord = Snake[0].y_coord;
  if(current_head_vector == UP_VECTOR) new_segment.y_coord = new_segment.y_coord - 3;
  else if(current_head_vector == DOWN_VECTOR) new_segment.y_coord = new_segment.y_coord + 3;
  else if(current_head_vector == RIGHT_VECTOR) new_segment.x_coord = new_segment.x_coord + 3;
  else if(current_head_vector == LEFT_VECTOR) new_segment.x_coord = new_segment.x_coord - 3;
  if((new_segment.x_coord != Snake[0].x_coord) or (new_segment.y_coord != Snake[0].y_coord))
  {
    SnakeAlive = checkMovement(new_segment.x_coord, new_segment.y_coord);

    if(SnakeAlive)
    {
      if((abs(new_segment.x_coord - current_food.x_coord) < 3) and (abs(new_segment.y_coord - current_food.y_coord) < 3))
      {
        current_food.exist = 0; // Food is not exist now, need to regenerate in other place
        SnakeLength++;
        Score = Score + 10;
      }
      
      for(int counter = SnakeLength - 1; counter > 0; counter--)
      {
        Snake[counter] = Snake[counter - 1];    // Move segments to create one empty slot for head
      }
      Snake[0] = new_segment;
    }
  }
}


void generateFood() // Generating food in random place of field
{
  current_food.exist = 1;
  current_food.x_coord = random(3, 93);
  current_food.y_coord = random(2, 65);
}


bool checkMovement(int x, int y) // Validation of movement
{
  bool validity = 1;
  if(x < 3 or x > 93 or y < 2 or y > 65) validity = 0; // Check that snake is on the field
  else
  {
    for(int counter = 0; counter < SnakeLength; counter++)
    {
      if(x == Snake[counter].x_coord and y == Snake[counter].y_coord) validity = 0; // Check that snake not touching itself
    }
  }
  return validity;
}

//end;
