#ifndef TermIoHandler_h
#define TermIoHandler_h

extern "C" {
#include <stdio.h>
#include <termios.h>
}

//
// class TermIoHandler
// - used to modify termilan I/O for non-blocking I/O with automatic
//   restoration of initial data when the object is destroyed
//

class TermIoHandler {
 private:
  bool isTerminal;
  int fn;
  struct termios attr;
 public:
  TermIoHandler();
  TermIoHandler(FILE *input);
  ~TermIoHandler();
  
  void SetImmedNoecho();
  void Reset();
};

#endif
