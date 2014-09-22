#include "display.hpp"

Display::Display()
{
  is_initialized = 0;
  screen_width = 64;
  screen_height = 32;
  pixelArray = new sf::Uint8[ 4 * screen_width * screen_height ];
  black_pixel = 0x00;
  white_pixel = 0xff;
  pixel_flipped = 0;
}


Display::~Display()
{
  delete pixelArray;
}


void Display::InitializeScreen()
{
  if ( is_initialized )
  {
    return;
  }

  ClearScreen();

  texture.create(screen_width, screen_height);
  texture.update(pixelArray);

  sprite.setTexture(texture);
  sprite.setScale(20, 20);

  //renderWindow.create(sf::VideoMode(640, 320), "CHIP8 Emulator");
  renderWindow.create(sf::VideoMode(1280, 640), "CHIP8 Emulator");

  is_initialized = true;
}

#include <iostream>

void Display::FlipPixel( int horizontal_offset, int vertical_offset )
{

  if( horizontal_offset > screen_width || vertical_offset > screen_height)
  {
    std::cout << "FlipPixel: hor_off: " << horizontal_offset << " vert_off " << vertical_offset << std::endl;
    getchar();
    return;
  }

  //1 - 64 width??
  //horizontal_offset -= 1;

  //std::cout << "FlipPixel: hor_off: " << horizontal_offset << " vert_off " << vertical_offset << std::endl;

  int index = ( horizontal_offset * 4 ) + ( vertical_offset * screen_width * 4 );

  //std::cout << "pixel value is: " << std::hex << (unsigned int)pixelArray[ index ] << std::endl;
  
  if ( (char)pixelArray[ index ] == white_pixel )
  {
    pixel_flipped = 1;
    //std::cout << "Clearing a pixel" << std::endl;
    pixelArray[index + 0] = black_pixel;
    pixelArray[index + 1] = black_pixel;
    pixelArray[index + 2] = black_pixel;
    pixelArray[index + 3] = black_pixel;
    //std::cout << "FUCK YOU IM FLIPPING!!!" << std::endl;
    //    std::cout << "FUCK YOU IM FLIPPING!!!" << std::endl;
    //        std::cout << "FUCK YOU IM FLIPPING!!!" << std::endl;
  //getchar();
  }
  else
  {
   // std::cout << "Setting a pixel" << std::endl;
    pixelArray[index + 0] = white_pixel;
    pixelArray[index + 1] = white_pixel;
    pixelArray[index + 2] = white_pixel;
    pixelArray[index + 3] = white_pixel;
  }

  texture.update(pixelArray);
}


void Display::RefreshScreen()
{
    renderWindow.clear();
    renderWindow.draw(sprite);
    renderWindow.display();
}


void Display::ClearScreen()
{
  for( int i = 0; i < (4 * screen_width * screen_height); i++)
  {
    pixelArray[i] = black_pixel;
  }

  texture.update(pixelArray);
}

void Display::ShutdownScreen()
{
  if ( !is_initialized )
  {
    return;
  }
  renderWindow.close();
  is_initialized = false;
}

bool Display::PixelFlipped()
{
  bool flipped_flag = pixel_flipped;
  pixel_flipped = 0;
  return flipped_flag;
}