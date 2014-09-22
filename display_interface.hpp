#ifndef DISPLAY_INTERFACE_H_
#define DISPLAY_INTERFACE_H_

#include "common_includes.hpp"

/*
** Interface for the display screen. This interface allows the screen to
** be managed and for individual pixels to be flipped.
*/
class DisplayInterface
{
public:
  virtual ~DisplayInterface() {}
  virtual void FlipPixel( int width, int height ) = 0;
  virtual void RefreshScreen() = 0;
  virtual void ClearScreen() = 0;
  virtual void InitializeScreen() = 0;
  virtual void ShutdownScreen() = 0;
  virtual bool PixelFlipped() = 0;
};

#endif //DISPLAY_INTERFACE_H_