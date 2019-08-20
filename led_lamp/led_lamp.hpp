
#define NOT_IMPLEMENTED printf("function is not implemented")

enum mode
{
    lamp, lamp_colored, candle, rainbow, disco, ambient
};



class Lamp
{
  private:
    int red_level[16] = {};    // Current 
    int green_level[16] = {};  // ring
    int blue_level[16] = {};   // color
    
    int mode = lamp;
    
  public:
    // setting all pixels in one color
    int setColor(int red, int green, int blue);
    int setColor(char color[]);
    
    // setting one pixel in some color
    int setPixel(int red, int green, int blue, int pixel_number);
    int setPixel(char color[], int pixel_number);
    
    // changing lamp working mode
    int changeMode(mode new_mode);
    
    // getting next color for candle, rainbow, disco and ambient modes
    char** getNextColor();
    
    void lampInfo();
};

int Lamp::setColor(int red, int green, int blue)
{
    for (int counter = 0; counter < 16; counter++)
    {
        red_level[counter] = red;
        green_level[counter] = green;
        blue_level[counter] = blue;
    }
    return 0;
}

int Lamp::setPixel(int red, int green, int blue, int pixel_number)
{
    red_level[pixel_number] = red;
    green_level[pixel_number] = green;
    blue_level[pixel_number] = blue;
    return 0;
}

void Lamp::lampInfo()
{
    printf("\n------\n");
    for (int counter = 0; counter < 16; counter++)
    {
        printf("pixel %d - red: %d, green: %d, blue: %d\n", counter, red_level[counter], green_level[counter], blue_level[counter]);
    }
    printf("------\n");
}




// end;

