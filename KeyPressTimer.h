#ifndef KeyPressTimer_h
#define KeyPressTimer_h
////////////////////////////////////////////////////////////////////////
//
//	KeyPressTimer decl
//

#include "Rtypes.h"
#include <TApplication.h>
#include <TTimer.h>

#include "KeyPress.h"

class KeyPressTimer : public TTimer
{
private:
  Bool_t fKeepGoing;
  Char_t fQuitKey;
  TApplication* fApp;
  
public:
  KeyPressTimer(Long_t millisec=100, Char_t pQuitKey='!', TApplication* pApp=0);
  
  Bool_t Notify();
  
  inline Bool_t KeepGoing() {return fKeepGoing;}
};
#endif
