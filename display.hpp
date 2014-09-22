#ifndef DISPLAY_H_
#define DISPLAY_H_

//SFML libraries
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

// Interface
#include "display_interface.hpp"

/*
** Implements the Display Interface. Allows the management of the screen
** and the setting of individual pixels.
** TODO: 1) Allow constructor to specify some screen resolutions
**       2) Constants for width and height of screen
*/
class Display: public DisplayInterface
{
public:
  Display();
  ~Display();
  void FlipPixel( int width, int height );
  void RefreshScreen();
  void ClearScreen();
  void InitializeScreen();
  void ShutdownScreen();
  bool PixelFlipped();

private:
  sf::Texture texture;
  sf::Sprite sprite;
  sf::RenderWindow renderWindow;
  sf::Uint8 *pixelArray;
  bool is_initialized;
  int screen_width;
  int screen_height;
  char black_pixel;
  char white_pixel;
  bool pixel_flipped;
};

#endif //DISPLAY_H_