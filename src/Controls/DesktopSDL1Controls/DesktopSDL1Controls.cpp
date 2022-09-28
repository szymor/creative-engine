#ifdef DESKTOP_SDL1

#include "DesktopSDL1Controls.h"

#define LDK_A_BUTTON        SDLK_LCTRL
#define LDK_B_BUTTON        SDLK_SPACE
#define LDK_X_BUTTON        SDLK_LALT
#define LDK_Y_BUTTON        SDLK_LSHIFT

#define LDK_SELECT_BUTTON   SDLK_ESCAPE
#define LDK_START_BUTTON    SDLK_RETURN
#define LDK_L_BUTTON        SDLK_TAB
#define LDK_R_BUTTON        SDLK_BACKSPACE
#define LDK_UP_BUTTON       SDLK_UP
#define LDK_DOWN_BUTTON     SDLK_DOWN
#define LDK_RIGHT_BUTTON    SDLK_RIGHT
#define LDK_LEFT_BUTTON     SDLK_LEFT

// unused
#define LDK_MUTE_BUTTON     SDLK_0
#define LDK_SCREEN_BUTTON   SDLK_RCTRL


DesktopSDL1Controls::DesktopSDL1Controls() : Controls() {}

DesktopSDL1Controls::~DesktopSDL1Controls() {
}

int pollNum = 0;
TBool DesktopSDL1Controls::Poll()  {
  SDL_Event e;


  while (SDL_PollEvent(&e)) {
    if (pollNum > 100) {
//      printf("POLL %i\n", pollNum);
      fflush(stdout);
      pollNum = 0;
    }

    TUint16 keys = bKeys;

    if (e.type == SDL_QUIT) {
      keys |= BUTTONQ;
    }



    // Keyboard
    if (e.type == SDL_KEYDOWN) {
//      printf("KEYDOWN %i\n", e.key.keysym.scancode);
      fflush(stdout);

      switch (e.key.keysym.sym) {
        // QUIT button, will never be set on target
        case LDK_R_BUTTON:// Not mapped on Odroid
          keys |= BUTTONR;
          break;
        case LDK_L_BUTTON:
          keys |= BUTTONL;
          break;
        case LDK_X_BUTTON:
          keys |= BUTTONX;
          break;
        case LDK_Y_BUTTON:
          keys |= BUTTONY;
          break;
        case LDK_SELECT_BUTTON: // SELECT on ODROID
          keys |= BUTTON3;
          break;
        case LDK_START_BUTTON: // START on oDROID
          keys |= BUTTON4;
          break;
        case LDK_A_BUTTON:
          keys |= BUTTONA;
          break;
        case LDK_B_BUTTON:
          keys |= BUTTONB;
          break;
        case LDK_UP_BUTTON:
          keys |= JOYUP;
          break;
        case LDK_DOWN_BUTTON:
          keys |= JOYDOWN;
          break;
        case LDK_LEFT_BUTTON:
          keys |= JOYLEFT;
          break;
        case LDK_RIGHT_BUTTON:
          keys |= JOYRIGHT;
          break;
        case LDK_MUTE_BUTTON:
          keys |= BUTTONQ;
          break;
        default:
          break;
      }
    }

    if (e.type == SDL_KEYUP) {
      switch (e.key.keysym.sym) {
        // QUIT button, will never be set on target
        case LDK_R_BUTTON:// Not mapped on Odroid
          keys &= ~BUTTONR;
          break;
        case LDK_L_BUTTON: // MENU on ODROID
          keys &= ~BUTTONL;
          break;
        case LDK_X_BUTTON:
          keys &= ~BUTTONX;
          break;
        case LDK_Y_BUTTON:
          keys &= ~BUTTONY;
          break;
        case LDK_SELECT_BUTTON: // SELECT on ODROID
          keys &= ~BUTTON3;
          break;
        case LDK_START_BUTTON: // START on oDROID
          keys &= ~BUTTON4;
          break;
        case LDK_A_BUTTON:
          keys &= ~BUTTONA;
          break;
        case LDK_B_BUTTON:
          keys &= ~BUTTONB;
          break;
        case LDK_UP_BUTTON:
          keys &= ~JOYUP;
          break;
        case LDK_DOWN_BUTTON:
          keys &= ~JOYDOWN;
          break;
        case LDK_LEFT_BUTTON:
          keys &= ~JOYLEFT;
          break;
        case LDK_RIGHT_BUTTON:
          keys &= ~JOYRIGHT;
          break;
        case LDK_MUTE_BUTTON:
          keys &= ~BUTTONQ;
          break;
        default:
          break;
      }
    }
    // cKeys are journaled if journaling is on!
    dKeys |= (keys ^ cKeys) & keys;
    cKeys        = keys;
    bKeys        = keys;
//    if (e.type == SDL_MOUSEBUTTONDOWN) {
//      quit = true;
//    }
  }
  return false;
}

#endif
