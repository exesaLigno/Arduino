////////////////////////////////////////////////////////////////
//        Pre-alpha version of LED lights controller          //
//     Work with display and stick (controlling modes)        //
//        Modes: Static clor, softly changing colors,         //
//              playing colors and stroboscope                //
//                                            by V. Karcev    //
////////////////////////////////////////////////////////////////

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <colors.h>

LiquidCrystal_I2C lcd(0x27,16,2);
#define STICK_HILEVEL 1000          //Critical high level when signal reading
#define STICK_LOLEVEL 100           //Critical low level when signal reading

#define POWERPIN 3 //NOT IMPLEMENTED
#define GREENPIN 5   
#define REDPIN 6
#define BLUEPIN 9

int CurrentRed = 0;       //Number of red color pin
int CurrentGreen = 0;     //Number of green color pin
int CurrentBlue = 0;      //Number of blue color pin

const int switchPin = 8;  //Stick button
const int pinX = A1;      //X axis
const int pinY = A2;      //Y axis

int CurrentPage = 0;      //Saving number of current page in menu
int CursorPosition = 0;   //Saving number of current cursor position
int Mode = 1;             //Default work mode
int Variant = 1;          //Default variation of work mode
int Brightness = 100;     //NOT IMPLEMENTED
int AlarmHour1 = 0;       
int AlarmHour2 = 0;
int AlarmMinute1 = 0;
int AlarmMinute2 = 0;
int AlarmEnabled = 0;

int Variants [5] = {0, 20, 8, 20, 1};   //Number of variations for every mode 

int setColor (char hexCode [6], int timer = 25);  //Setting color from HEX-code ("ff0000" - red, "ffffff" - white)
 
void setup() 
{
  lcd.init();                         
  lcd.backlight();                  
  lcd.setCursor(0,0);
  lcd.print("LCD Lights");
  lcd.setCursor(0,1);
  lcd.print("by V. Karcev");
  setColor ("ff0000");
  setColor ("ffff00");
  setColor ("00ff00");
  setColor ("000000");
  lcd.clear();
  lcd.setCursor(0,0);                
  lcd.print(" MODE: ");               
  if (Mode == 1) lcd.print("StatColor");
  if (Mode == 2) lcd.print("ColorPlay");
  if (Mode == 3) lcd.print("SoftColor");
  if (Mode == 4) lcd.print("Strobo   ");
  lcd.setCursor(0,1);                
  lcd.print(" VAR: ");               
  lcd.print(Variant);
  lcd.setCursor (0, CursorPosition % 2);
  lcd.print (">");
  lcd.setCursor(0,0);
  pinMode(ledPin, OUTPUT);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH);
  analogWrite(POWERPIN, 0); //NOT IMPLEMENTED
  Serial.begin(9600);
}
 
void loop() 
{
  int button = !digitalRead(switchPin);
  if (button && CurrentPage == 0) enableMode (Mode, Variant);
  else if (button && CursorPosition == 3) setupAlarm ();
  int X = analogRead(pinX);
  int Y = analogRead(pinY);
  
  if (X > STICK_HILEVEL || X < STICK_LOLEVEL)
  {
    if (X > STICK_HILEVEL) CursorPosition = ((CursorPosition + 1) + 4) % 4;
    if (X < STICK_LOLEVEL) CursorPosition = ((CursorPosition - 1) + 4) % 4;
    if ((CursorPosition == 0 || CursorPosition == 1) && (CurrentPage != 0)) drawPage (0);
    if ((CursorPosition == 2 || CursorPosition == 3) && (CurrentPage != 1)) drawPage (1);
    lcd.setCursor (0, 0);
    lcd.print (" ");
    lcd.setCursor (0, 1);
    lcd.print (" ");
    lcd.setCursor (0, CursorPosition % 2);
    lcd.print (">");
    lcd.setCursor (0, CursorPosition % 2);
    delay (300);
  }

  if (Y < STICK_LOLEVEL && CursorPosition == 0)
  {
    Mode += 1;
    if (Mode > 4) Mode = 1;
    lcd.setCursor (7, 0);
    if (Mode == 1) lcd.print("StatColor");
    if (Mode == 2) lcd.print("ColorPlay");
    if (Mode == 3) lcd.print("SoftColor");
    if (Mode == 4) lcd.print("Strobo   ");
    Variant = 1;
    lcd.setCursor (6, 1);
    lcd.print ("    ");
    lcd.setCursor (6, 1);
    lcd.print (Variant);
    lcd.setCursor (0, 0);
    delay (300);
  }

  if (Y > STICK_HILEVEL && CursorPosition == 0)
  {
    Mode -= 1;
    if (Mode < 1) Mode = 4;
    lcd.setCursor (7, 0);
    if (Mode == 1) lcd.print("StatColor");
    if (Mode == 2) lcd.print("ColorPlay");
    if (Mode == 3) lcd.print("SoftColor");
    if (Mode == 4) lcd.print("Strobo   ");
    Variant = 1;
    lcd.setCursor (6, 1);
    lcd.print ("    ");
    lcd.setCursor (6, 1);
    lcd.print (Variant);
    lcd.setCursor (0, 0);
    delay (300);
  }

  if (Y < STICK_LOLEVEL && CursorPosition == 1)
  {
    Variant += 1;
    if (Variant > Variants[Mode]) Variant = 1;
    lcd.setCursor (6, 1);
    lcd.print ("    ");
    lcd.setCursor (6, 1);
    lcd.print (Variant);
    lcd.setCursor (0, 1);
    delay (300);
  }

  if (Y > STICK_HILEVEL && CursorPosition == 1)
  {
    Variant -= 1;
    if (Variant < 1) Variant = Variants[Mode];
    lcd.setCursor (6, 1);
    lcd.print ("    ");
    lcd.setCursor (6, 1);
    lcd.print (Variant);
    lcd.setCursor (0, 1);
    delay (300);
  }

  if (Y < STICK_LOLEVEL && CursorPosition == 2)
  {
    Brightness += 10;
    if (Brightness > 100) Brightness = 0;
    lcd.setCursor (9, 0);
    lcd.print ("    ");
    lcd.setCursor (9, 0);
    lcd.print (Brightness);
    lcd.print ("%");
    lcd.setCursor (0, 0);
    delay (300);
  }

  if (Y > STICK_HILEVEL && CursorPosition == 2)
  {
    Brightness -= 10;
    if (Brightness < 0) Brightness = 100;
    lcd.setCursor (9, 0);
    lcd.print ("    ");
    lcd.setCursor (9, 0);
    lcd.print (Brightness);
    lcd.print ("%");
    lcd.setCursor (0, 0);
    delay (300);
  }
}


int drawPage (int pageNum)
{
  if (pageNum == 0)
  {
    CurrentPage = 0;
    lcd.clear();
    lcd.setCursor(0,0);                
    lcd.print(" MODE: ");  
    if (Mode == 1) lcd.print("StatColor");
    if (Mode == 2) lcd.print("ColorPlay");
    if (Mode == 3) lcd.print("SoftColor");
    if (Mode == 4) lcd.print("Strobo   ");             
    lcd.setCursor(0,1);                
    lcd.print(" VAR: ");               
    lcd.print(Variant);
    lcd.setCursor(0,0);
  }
  if (pageNum == 1)
  {
    CurrentPage = 1;
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print(" INTENS: ");
    lcd.setCursor (9, 0);
    lcd.print (Brightness);
    lcd.print ("%");
    lcd.setCursor(0,1); 
    lcd.print(" ALARM: ");
    lcd.print("  :  ");
    lcd.setCursor (8,1);
    lcd.print(AlarmHour1);
    lcd.print(AlarmHour2);
    lcd.setCursor (11,1);
    lcd.print(AlarmMinute1);
    lcd.print(AlarmMinute2);
    lcd.setCursor(14, 1);
    if (AlarmEnabled) lcd.print ("V");
    else lcd.print ("X");
    lcd.setCursor(0,0); 
  }
  /*
  if (pageNum == 2)
  {
    CurrentPage = 2;
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print(" ALARM: ");
    lcd.setCursor(0,0); 
  }
  */
  return 0;
}


int setupAlarm ()
{
  int position = 0;
  lcd.setCursor(0,1);
  lcd.print(" ");
  lcd.setCursor(8,1);
  lcd.cursor();
  lcd.blink();
  delay (300);

  while (true)
  {
    int X = analogRead(pinX);
    int Y = analogRead(pinY);
    int button = !digitalRead(switchPin);
    
    if (Y < STICK_LOLEVEL) 
    {
      position++;
      if (position == 2) position++;
      if (position == 5) position++;
      if (position == 7) position = 0;
      lcd.setCursor(8 + position, 1);
      delay (300);
    }
    
    else if (Y > STICK_HILEVEL) 
    {
      position--;
      if (position == 5) position--;
      if (position == 2) position--;
      if (position == -1) position = 6;
      lcd.setCursor(8 + position, 1);
      delay (300);
    }

    if (X < STICK_LOLEVEL) 
    {
      if (position == 0) 
      {
        AlarmHour1 = (AlarmHour1 + 1) % 3;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmHour1);
        lcd.setCursor (8 + position, 1);
        if (AlarmHour2 > 4 && AlarmHour1 == 2)
        {
          AlarmHour2 = 3;
          lcd.setCursor (8 + position + 1, 1);
          lcd.print (AlarmHour2);
          lcd.setCursor (8 + position, 1);
        }
      }
      if (position == 1 && AlarmHour1 != 2)
      {
        AlarmHour2 = (AlarmHour2 + 1) % 10;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmHour2);
        lcd.setCursor (8 + position, 1);
      }
      if (position == 1 && AlarmHour1 == 2)
      {
        AlarmHour2 = (AlarmHour2 + 1) % 4;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmHour2);
        lcd.setCursor (8 + position, 1);
      }
      if (position == 3)
      {
        AlarmMinute1 = (AlarmMinute1 + 1) % 6;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmMinute1);
        lcd.setCursor (8 + position, 1);
      }
      if (position == 4)
      {
        AlarmMinute2 = (AlarmMinute2 + 1) % 10;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmMinute2);
        lcd.setCursor (8 + position, 1);
      }
      if (position == 6)
      {
        AlarmEnabled = !AlarmEnabled;
        lcd.setCursor (8 + position, 1);
        if (AlarmEnabled) lcd.print ("V");
        else lcd.print ("X");
        lcd.setCursor (8 + position, 1);
      }
      delay (300);
    }
    
    if (X > STICK_HILEVEL) 
    {
      if (position == 0) 
      {
        AlarmHour1 = (AlarmHour1 - 1 + 3) % 3;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmHour1);
        lcd.setCursor (8 + position, 1);
        if (AlarmHour2 > 4 && AlarmHour1 == 2)
        {
          AlarmHour2 = 3;
          lcd.setCursor (8 + position + 1, 1);
          lcd.print (AlarmHour2);
          lcd.setCursor (8 + position, 1);
        }
      }
      if (position == 1 && AlarmHour1 != 2)
      {
        AlarmHour2 = (AlarmHour2 - 1 + 10) % 10;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmHour2);
        lcd.setCursor (8 + position, 1);
      }
      if (position == 1 && AlarmHour1 == 2)
      {
        AlarmHour2 = (AlarmHour2 - 1 + 4) % 4;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmHour2);
        lcd.setCursor (8 + position, 1);
      }
      if (position == 3)
      {
        AlarmMinute1 = (AlarmMinute1 - 1 + 6) % 6;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmMinute1);
        lcd.setCursor (8 + position, 1);
      }
      if (position == 4)
      {
        AlarmMinute2 = (AlarmMinute2 - 1 + 10) % 10;
        lcd.setCursor (8 + position, 1);
        lcd.print (AlarmMinute2);
        lcd.setCursor (8 + position, 1);
      }
      if (position == 6)
      {
        AlarmEnabled = !AlarmEnabled;
        lcd.setCursor (8 + position, 1);
        if (AlarmEnabled) lcd.print ("V");
        else lcd.print ("X");
        lcd.setCursor (8 + position, 1);
      }
      delay (300);
    }

    if (button) break;
  }

  lcd.setCursor(0,1);
  lcd.print(">");
  lcd.setCursor(0,0);
  lcd.noBlink();
  lcd.noCursor();
  delay (300);
  return 0;
}


int enableMode (int mode, int variant)
{
  Serial.print ("Mode: ");
  Serial.print (mode);
  Serial.print ("\t\t");
  Serial.print ("Var: ");
  Serial.println (variant);
  lcd.noDisplay();
  lcd.noBacklight();
  delay (300);
  if (mode == 1 && variant == 1) setColor ("ff0000");
  if (mode == 1 && variant == 2) setColor ("00ff00");
  if (mode == 1 && variant == 3) setColor ("0000ff");
  while (true)
  {
    int button = !digitalRead(switchPin);
    if (button)
    {
      lcd.display();
      lcd.backlight();
      delay (300);
      return 0;
    }
  }
  return 0;
}


int setColor (char hexCode [6], int timer)
{
  int decCode [6] = {};
  for (int counter = 0; counter < 6; counter++)
  {
    if (hexCode [counter] > 64 && hexCode [counter] < 71) decCode [counter] = hexCode [counter] - 55;
    if (hexCode [counter] > 47 && hexCode [counter] < 58) decCode [counter] = hexCode [counter] - 48;
    if (hexCode [counter] > 96 && hexCode [counter] < 103) decCode [counter] = hexCode [counter] - 87;
  }
  Serial.print ("Red: ");
  Serial.println (decCode[0]*16 + decCode[1]);
  Serial.print ("Green: ");
  Serial.println (decCode[2]*16 + decCode[3]);
  Serial.print ("Blue: ");
  Serial.println (decCode[4]*16 + decCode[5]);

  int RedTarget = decCode[0]*16 + decCode[1];
  int GreenTarget = decCode[2]*16 + decCode[3];
  int BlueTarget = decCode[4]*16 + decCode[5];

  float RedStep = abs(RedTarget - CurrentRed) / 40;
  float GreenStep = abs(GreenTarget - CurrentGreen) / 40;
  float BlueStep = abs(BlueTarget - CurrentBlue) / 40;

  int RedVector = (RedTarget - CurrentRed)/abs(RedTarget - CurrentRed);
  int GreenVector = (GreenTarget - CurrentGreen)/abs(GreenTarget - CurrentGreen);
  int BlueVector = (BlueTarget - CurrentBlue)/abs(BlueTarget - CurrentBlue);
  
  Serial.print(RedVector);
  Serial.print("\t");
  Serial.print(GreenVector);
  Serial.print("\t");
  Serial.println(BlueVector);
  
  for (int counter = 1; counter < 41; counter++)
  {
    analogWrite(REDPIN, CurrentRed + RedVector * floor(RedStep * counter));
    analogWrite(GREENPIN, CurrentGreen + GreenVector * floor(GreenStep * counter));
    analogWrite(BLUEPIN, CurrentBlue + BlueVector * floor(BlueStep * counter));
    Serial.print(CurrentRed + RedVector * floor(RedStep * counter));
    Serial.print("\t");
    Serial.print(CurrentGreen + GreenVector * floor(GreenStep * counter));
    Serial.print("\t");
    Serial.println(CurrentBlue + BlueVector * floor(BlueStep * counter));
    delay (timer);
  }
  
  analogWrite(REDPIN, RedTarget);
  analogWrite(GREENPIN, GreenTarget);
  analogWrite(BLUEPIN, BlueTarget);

  CurrentRed = RedTarget;
  CurrentGreen = GreenTarget;
  CurrentBlue = BlueTarget;
  
  return 0;
}
