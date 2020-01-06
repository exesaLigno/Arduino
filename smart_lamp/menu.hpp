class Setting
{
private:
  int screens_count = 5;    // Lamp / Point / Color / Torch / Ambient
  int lines_count[5] = {1, 2, 3, 0, 1};
  
public:
  int screen = 0;
  int line = 0;

  int brightness = 0; // Value in range from 0 to 100

  int warmth = 0; // Value in range from -100 to 100
  int spotlight_position = 0; // Value in range from -50 to 50

  int red = 90; // Value in range from 0 to 255
  int green = 90;
  int blue = 90;

  int ambient_mode = 0;

  Setting();

  bool nextLine();
  bool nextScreen();
  bool previousScreen();

  bool brightnessUp();
  bool brightnessDown();

  bool warmthUp();
  bool warmthDown();

  bool positionUp();
  bool positionDown();

  bool colorUp(int color);
  bool colorDown(int color);
  
  ~Setting();
};


Setting::Setting()
{}


Setting::~Setting()
{}


bool Setting::nextLine()
{
  int last = this -> line;
  
  if ((this -> lines_count)[this -> screen])
    this -> line = (this -> line + 1) % (this -> lines_count)[this -> screen];
    
  return (this -> line != last);
}


bool Setting::nextScreen()
{  
  this -> screen = (this -> screen + 1) % this -> screens_count;

  this -> line = 0;
  
  return true;
}


bool Setting::previousScreen()
{
  if (this -> screen)
    this -> screen -= 1;
  else
    this -> screen = this -> screens_count - 1;

  this -> line = 0;
    
  return true;
}


bool Setting::brightnessUp()
{
  if (this -> brightness < 100)
  {
    (this -> brightness) += 5;
    return true;
  }
  else
    return false;
}


bool Setting::brightnessDown()
{
  if (this -> brightness > 0)
  {
    (this -> brightness) -= 5;
    return true;
  }
  else
    return false;
}


bool Setting::warmthUp()
{
  if (this -> warmth < 100)
  {
    (this -> warmth) += 5;
    return true;
  }
  else
    return false;
}


bool Setting::warmthDown()
{
  if (this -> warmth > -100)
  {
    (this -> warmth) -= 5;
    return true;
  }
  else
    return false;
}


bool Setting::positionUp()
{
  if (this -> spotlight_position < 50)
  {
    (this -> spotlight_position) += 5;
    return true;
  }
  else
    return false;
}


bool Setting::positionDown()
{
  if (this -> spotlight_position > -50)
  {
    (this -> spotlight_position) -= 5;
    return true;
  }
  else
    return false;
}



bool Setting::colorUp(int color)
{
  switch(color)
  {
    case(0):
    {
      if (this -> red < 255)
      {
        (this -> red) += 15;
        return true;
      }
      else
        return false;
    }

    case(1):
    {
      if (this -> green < 255)
      {
        (this -> green) += 15;
        return true;
      }
      else
        return false;
    }

    case(2):
    {
      if (this -> blue < 255)
      {
        (this -> blue) += 15;
        return true;
      }
      else
        return false;
    }
  }
}


bool Setting::colorDown(int color)
{
  switch(color)
  {
    case(0):
    {
      if (this -> red > 0)
      {
        (this -> red) -= 15;
        return true;
      }
      else
        return false;
    }

    case(1):
    {
      if (this -> green > 0)
      {
        (this -> green) -= 15;
        return true;
      }
      else
        return false;
    }

    case(2):
    {
      if (this -> blue > 0)
      {
        (this -> blue) -= 15;
        return true;
      }
      else
        return false;
    }
  }
}
