////////////////////////////////////////////////////////////////////////
//
//	calibmonrun
//

#include <stdio.h>

#include "KeyPressTimer.h"

KeyPressTimer::KeyPressTimer(Long_t millisec, Char_t pQuitKey, TApplication* pApp) :
    TTimer(millisec), fQuitKey(pQuitKey), fApp(pApp), fKeepGoing(true)
{
  //--- for some reason the key input needs this initialization
  KeyPress::Get();
  //--- start the timer
  TurnOn();
}

Bool_t KeyPressTimer::Notify()
{
  int ch;
  
  //--- stop timer
  TurnOff();
  
  if (KeyPress::Hit()) {
    ch = KeyPress::Get();
    if ((char) ch == fQuitKey) {
      // printf("quit key pressed, quitting\n");
      KeyPress::Get(true);
      fKeepGoing = false;
      if (fApp) fApp->Terminate(0);
    }
  }
  
  TurnOn();
  return true;
}
