class Settings
{
public:
  enum PARAMETERS
  {
    BRIGHTNESS = 5,
    TEMPERATURE = 0,
    SPOTLIGHT = 1,
    COLOR = 2,
    CANDLE = 3,
    AMBIENT = 4,
    COUNT_OF_PARAMETERS = 5
  };

  Settings();

  bool changeParameter(int parameter, int sign);
  bool nextMode();
  
  ~Settings();
  
                   //   TEMP  POS  COLOR  CAND  AMB BRIGH   //
  int steps[6] =      { 215,   0,    0,    0,    0,  15   };
  int infinums[6] =   { 1600,  0,    0,    0,    0,  0    };
  int supremums[6] =  { 7620,  200,  255,  0,    0,  255  };
  int parameters[6] = { 1600,  100,  0,    0,    0,  0    };


  byte current_line = 0;
  
  bool parameter_changing = false;
  
};


Settings::Settings()
{}

Settings::~Settings()
{}


bool Settings::changeParameter(int parameter_id, int sign)
{
  if ((((this -> parameters)[parameter_id]) + sign*((this -> steps)[parameter_id]) < ((this -> infinums)[parameter_id])) or (((this -> parameters)[parameter_id]) + sign*((this -> steps)[parameter_id]) > ((this -> supremums)[parameter_id])))
  {  
    return false;
  }
  else
  {
    (this -> parameters)[parameter_id] = (this -> parameters)[parameter_id] + sign*(this -> steps)[parameter_id];
    return true;
  }
}


bool Settings::nextMode()
{
  this -> current_line = ((this -> current_line) + 1) % 5;
  return true;
}
