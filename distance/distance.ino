#include <Ultrasonic.h>
#include <TM74HC595Display.h>

#define SCLK 2
#define RCLK 4
#define DIO 3

#define TRIG 6
#define ECHO 5

#define CM 1
#define INC 0

TM74HC595Display disp(SCLK, RCLK, DIO);
Ultrasonic ultrasonic(TRIG, ECHO);

void setup(){}

void loop() 
{  
  disp.digit4(ultrasonic.Ranging(CM), 100);
}
