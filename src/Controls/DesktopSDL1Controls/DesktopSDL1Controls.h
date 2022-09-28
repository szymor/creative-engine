#ifdef DESKTOP_SDL1

#ifndef DESKTOP_SDL1_CONTROLS_H
#define DESKTOP_SDL1_CONTROLS_H
#include "Controls.h"
#include <SDL/SDL.h>

class DesktopSDL1Controls : public Controls {
public:
  DesktopSDL1Controls();
  ~DesktopSDL1Controls();

  TBool Poll() final;
};


#endif //DESKTOP_SDL1_CONTROLS_H

#endif

