enum COMMANDS
{
  NOTHING_HAPPENS = 0,
  LEFT_PRESSED = -1,
  RIGHT_PRESSED = 1,
  LEFT_KEY_PRESSED = -2,
  RIGHT_KEY_PRESSED = 2,
  LEFT_FAST_PRESSED = -3,
  RIGHT_FAST_PRESSED = 3,
  KEY_PRESSED = 4,

  LEFT_RELEASED = -11,
  RIGHT_RELEASED = 11,
  LEFT_KEY_RELEASED = -12,
  RIGHT_KEY_RELEASED = 12,
  LEFT_FAST_RELEASED = -13,
  RIGHT_FAST_RELEASED = 13,
  KEY_RELEASED = 14,

  ERR = 100
};

enum cmd
{
  LEFT = -1,
  RIGHT = 1,
  LEFT_KEY = -2,
  RIGHT_KEY = 2,
  BUTTON = 3,
  NOTHING = 0
};


class Encoder
{
private:
  int last_command = 0;
  bool rotated = 0;
  int S1 = 0;
  int S2 = 0;
  int KEY = 0;

public:
  Encoder(int S1_PIN, int S2_PIN, int KEY_PIN);
  int checkEvent();
  int check();

  ~Encoder();
};

Encoder::Encoder(int S1_PIN, int S2_PIN, int KEY_PIN)
{
  this -> S1 = S1_PIN;
  this -> S2 = S2_PIN;
  this -> KEY = KEY_PIN;
}

int Encoder::checkEvent()
{
  bool s1_signal = !digitalRead(this -> S1);
  bool s2_signal = !digitalRead(this -> S2);
  bool key_signal = !digitalRead(this -> KEY);

  if(key_signal)
  {
    if (s1_signal)
    {
      this -> rotated = true;
      if (this -> last_command == LEFT_KEY_PRESSED or this -> last_command == RIGHT_KEY_PRESSED)
      {
        return NOTHING_HAPPENS;
      }
      else
      {
        this -> last_command = LEFT_KEY_PRESSED;
        return LEFT_KEY_PRESSED;
      }
    }

    else if (s2_signal)
    {
      this -> rotated = true;
      if (this -> last_command == LEFT_KEY_PRESSED or this -> last_command == RIGHT_KEY_PRESSED)
      {
        return NOTHING_HAPPENS;
      }
      else
      {
        this -> last_command = RIGHT_KEY_PRESSED;
        return RIGHT_KEY_PRESSED;
      }
    }

    else
    {
      if (this -> last_command == KEY_PRESSED or this -> last_command == LEFT_KEY_PRESSED or this -> last_command == RIGHT_KEY_PRESSED)
      {
        this -> last_command = KEY_PRESSED;
        return NOTHING_HAPPENS;
      }

      else
      {
        this -> last_command = KEY_PRESSED;
        return KEY_PRESSED;
      }
    }
  }

  else if(s1_signal)
  {
    if (this -> last_command == RIGHT_PRESSED or this -> last_command == LEFT_PRESSED)
    {
      return NOTHING_HAPPENS;
    }

    else
    {
      this -> last_command = LEFT_PRESSED;
      return LEFT_PRESSED;
    }
  }

  else if(s2_signal)
  {
    if (this -> last_command == RIGHT_PRESSED or this -> last_command == LEFT_PRESSED)
    {
      return NOTHING_HAPPENS;
    }

    else
    {
      this -> last_command = RIGHT_PRESSED;
      return RIGHT_PRESSED;
    }
  }

  else
  {
    if (this -> last_command == KEY_PRESSED)
    {
      this -> last_command = NOTHING_HAPPENS;
      return KEY_RELEASED;
    }

    else if (this -> last_command == LEFT_PRESSED)
    {
      this -> last_command = NOTHING_HAPPENS;
      return LEFT_RELEASED;
    }

    else if (this -> last_command == RIGHT_PRESSED)
    {
      this -> last_command = NOTHING_HAPPENS;
      return RIGHT_RELEASED;
    }

    else if (this -> last_command == LEFT_KEY_PRESSED)
    {
      this -> last_command = NOTHING_HAPPENS;
      return LEFT_KEY_RELEASED;
    }

    else if (this -> last_command == RIGHT_KEY_PRESSED)
    {
      this -> last_command = NOTHING_HAPPENS;
      return RIGHT_KEY_RELEASED;
    }

    else
    {
      return NOTHING_HAPPENS;
    }
  }
}


int Encoder::check()
{
  int event = this -> checkEvent();

  if (event == KEY_RELEASED)
  {
    if (this -> rotated)
    {
      this -> rotated = false;
      return NOTHING;
    }

    else
    {
      this -> rotated = false;
      return BUTTON;
    }
  }

  else if (event == LEFT_RELEASED)
    return LEFT;

  else if (event == RIGHT_RELEASED)
    return RIGHT;

  else if (event == LEFT_KEY_PRESSED)
    return LEFT_KEY;

  else if (event == RIGHT_KEY_PRESSED)
    return RIGHT_KEY;

  else if (event == NOTHING_HAPPENS)
    return NOTHING;

  else
    return NOTHING;
}


Encoder::~Encoder()
{
  
}
