class Setting
{
private:
  int screens_count = 5;    // Lamp / Point / Color / Torch / Ambient
  int lines_count[5] = {1, 2, 3, 0, 1};
  bool drawing_required = true;
  
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

  int nextLine();
  int nextScreen();
  int previousScreen();

  int brightnessUp();
  int brightnessDown();

  int warmthUp();
  int warmthDown();

  int positionUp();
  int positionDown();

  int colorUp(int color);
  int colorDown(int color);

  bool drawingRequred();
  bool drawed();
  
  ~Setting();
};


Setting::Setting()
{}


Setting::~Setting()
{}


int Setting::nextLine()
{
  int last = this -> line;
  
  if ((this -> lines_count)[this -> screen])
    this -> line = (this -> line + 1) % (this -> lines_count)[this -> screen];

  if (this -> line != last)
    this -> drawing_required = true;
    
  return this -> line;
}


int Setting::nextScreen()
{  
  this -> screen = (this -> screen + 1) % this -> screens_count;

  this -> line = 0;

  this -> drawing_required = true;
  
  return this -> screen;
}


int Setting::previousScreen()
{
  if (this -> screen)
    this -> screen -= 1;
  else
    this -> screen = this -> screens_count - 1;

  this -> drawing_required = true;

  this -> line = 0;
    
  return this -> screen;
}


bool Setting::drawingRequred()
{
  return this -> drawing_required;
}

bool Setting::drawed()
{
  this -> drawing_required = false;
  return 0;
}


int Setting::brightnessUp()
{
  if (this -> brightness < 100)
  {
    this -> drawing_required = true;
    (this -> brightness) += 5;
    return 5;
  }
  else
    return 0;
}


int Setting::brightnessDown()
{
  if (this -> brightness > 0)
  {
    this -> drawing_required = true;
    (this -> brightness) -= 5;
    return -5;
  }
  else
    return 0;
}


int Setting::warmthUp()
{
  if (this -> warmth < 100)
  {
    this -> drawing_required = true;
    (this -> warmth) += 5;
    return 5;
  }
  else
    return 0;
}


int Setting::warmthDown()
{
  if (this -> warmth > -100)
  {
    this -> drawing_required = true;
    (this -> warmth) -= 5;
    return -5;
  }
  else
    return 0;
}


int Setting::positionUp()
{
  if (this -> spotlight_position < 50)
  {
    this -> drawing_required = true;
    (this -> spotlight_position) += 5;
    return 5;
  }
  else
    return 0;
}


int Setting::positionDown()
{
  if (this -> spotlight_position > -50)
  {
    this -> drawing_required = true;
    (this -> spotlight_position) -= 5;
    return -5;
  }
  else
    return 0;
}



int Setting::colorUp(int color)
{
  switch(color)
  {
    case(0):
    {
      if (this -> red < 255)
      {
        this -> drawing_required = true;
        (this -> red) += 15;
        return 15;
      }
      else
        return 0;
    }

    case(1):
    {
      if (this -> green < 255)
      {
        this -> drawing_required = true;
        (this -> green) += 15;
        return 15;
      }
      else
        return 0;
    }

    case(2):
    {
      if (this -> blue < 255)
      {
        this -> drawing_required = true;
        (this -> blue) += 15;
        return 15;
      }
      else
        return 0;
    }
  }
}


int Setting::colorDown(int color)
{
  switch(color)
  {
    case(0):
    {
      if (this -> red > 0)
      {
        this -> drawing_required = true;
        (this -> red) -= 15;
        return -15;
      }
      else
        return 0;
    }

    case(1):
    {
      if (this -> green > 0)
      {
        this -> drawing_required = true;
        (this -> green) -= 15;
        return -15;
      }
      else
        return 0;
    }

    case(2):
    {
      if (this -> blue > 0)
      {
        this -> drawing_required = true;
        (this -> blue) -= 15;
        return -15;
      }
      else
        return 0;
    }
  }
}
