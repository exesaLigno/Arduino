#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


class Dino
{
 public:
  double vspeed = 0;
  double vposition = 0;

  long unsigned int last_update_time = 0;

  Dino();
  ~Dino();
  void draw();
  void jump();
};


Dino dino = Dino();


void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
}


void loop()
{
  if (digitalRead(3))
    dino.jump();

   dino.draw();
}



Dino::Dino() {}

Dino::~Dino() {}


void Dino::draw()
{
  double delta = (millis() - this -> last_update_time) / 1000.0;

  display.clearDisplay();

  this -> last_update_time = millis();

  if (this -> vposition > 0 or this -> vspeed > 0)
  {
    this -> vposition += this -> vspeed * delta;
    this -> vspeed -= 22 * delta;
  }

  if (this -> vposition < 0)
  {
    this -> vposition = 0;
    this -> vspeed = 0;
  }

  display.fillRect(30, 60 - this -> vposition * 11, 3, 3, SSD1306_INVERSE);

  display.display();
}


void Dino::jump()
{
  if (this -> vspeed == 0)
  {
    this -> last_update_time = millis();
    this -> vspeed = 10;
  }
}
