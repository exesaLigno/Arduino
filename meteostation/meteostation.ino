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
 * BME SENSOR (BME280)
 *    VCC -> 3V3
 *    GND -> GND
 *    SCL -> D13
 *    SDA -> D11
 *    SCB -> D10
 *    SDO -> D12
 *    
 * TOUCH KEY
 *    5V -> 5V
 *    GND -> GND
 *    KEY -> D2
 */


#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// define pins for sensor   (VCC -> 3V3, GND -> GND)
#define BME_SCK   13      // SCL pin on sensor
#define BME_MISO  12      // SDO pin on sensor
#define BME_MOSI  11      // SDA pin on sensor
#define BME_CS    10      // SCB pin on sensor

// define pins for display  (VCC -> 5V, BL -> 3V3, GND -> GND)
#define DISPLAY_CLOCK 4   // CLK pin on display
#define DISPLAY_DATA  5   // DIN pin on display
#define DISPLAY_CS    7   // CE pin on display
#define DISPLAY_RESET 8   // RST pin on display

// define pins for key   (5V -> 5V, GND -> GND)
#define TOUCH_KEY 2

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 sensor(BME_CS, BME_MOSI, BME_MISO, BME_SCK);
U8G2_HX1230_96X68_F_3W_SW_SPI display(U8G2_R0, DISPLAY_CLOCK, DISPLAY_DATA, DISPLAY_CS, DISPLAY_RESET);


void sensorError();
void drawFrame(int frame);
int checkKey();
int next(int frame);


int FrameNumber = 0;
int Time_from_last_update = 0;
int SensorError = 0;


void setup(void) 
{
  display.begin();
  Serial.begin(9600);

  bool status;
  status = sensor.begin();
  if (!status) SensorError = 1;
  delay(100); //Sensor bootup
  drawFrame(FrameNumber);
}

void loop(void) 
{
  int frame = FrameNumber;
  int command = checkKey();
  
  if (command == 1) FrameNumber = next(FrameNumber);
  
  if ((frame != FrameNumber) or (Time_from_last_update > 1000))
  {
    Time_from_last_update = 0;
    drawFrame(FrameNumber);
    Serial.println(Time_from_last_update);
  }
  
  delay(1);
  Time_from_last_update++;
}


void drawFrame(int frame) 
{
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);

  display.drawLine(0,13,96,13);
  display.drawLine(0,58,96,58);

  drawBar();

  switch(frame)
  {
    case(0):
    {
      display.drawStr(0, 10, "Temp/P/A/H");
      float temp_celsius = sensor.readTemperature();
      display.setCursor(10, 27);
      display.print(temp_celsius, 2); // Temperature in Celsius
      display.drawCircle(50, 18, 2);
      display.drawStr(52, 27, " C");
      display.setCursor(10, 40);
      display.print(temp_celsius + 273.15, 2); // Temperature in Kelvins
      display.drawCircle(50, 31, 2);
      display.drawStr(52, 40, " K");
      display.setCursor(10, 53);
      display.print(temp_celsius * 1.8 + 32, 2); // Temperature in Farengeith
      display.drawCircle(50, 44, 2);
      display.drawStr(52, 53, " F");
      display.drawLine(72,13,72,58);
      break;
    }
    case(1):
    {
      display.drawStr(0, 10, "T/Pressure/A/H");
      float pressure_pa = sensor.readPressure();
      display.setCursor(10, 27);
      display.print(pressure_pa / 1000.0F, 2);
      display.drawStr(50, 27, "kPa");
      display.setCursor(10, 40);
      display.print(pressure_pa / 133.332, 2);
      display.drawStr(50, 40, "mm hg");
      break;
    }
    case(2):
    {
      display.drawStr(0, 10, "T/P/Altitude/H");
      display.setCursor(10, 27);
      display.print(sensor.readAltitude(SEALEVELPRESSURE_HPA),2);
      display.drawStr(50, 27, "m");
      display.drawStr(10, 40, "Approximate");
      display.drawStr(10, 53, "Error ~20m");
      break;
    }
    case(3):
    {
      display.drawStr(0, 10, "T/P/A/Humidity");
      display.setCursor(10, 27);
      display.print(sensor.readHumidity(), 2);
      display.drawStr(40, 27, "%");
      display.drawStr(10, 40, "Normal: 40-60%");
      break;
    }
  }

  display.sendBuffer();
}


int checkKey()
{
  int command = 0;
  
  if(digitalRead(TOUCH_KEY))
  {
    command = 1;
    while(digitalRead(TOUCH_KEY));
  }

  return command;
}


int next(int frame)
{
  int NewFrame = frame + 1;
  if(NewFrame > 3) NewFrame = 0;
  return NewFrame;
}


void drawBar()
{
  if(SensorError) display.drawStr(0, 67, "err");
  if(sensor.readHumidity() < 40) display.drawStr(55, 67, "too dry");
  else if (sensor.readHumidity() > 60) display.drawStr(55, 67, "too wet");
}


//end;
