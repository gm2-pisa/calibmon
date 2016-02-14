////////////////////////////////////////////////////////////////////////////////
//
// TermIoHandler defs
//

////////////////////////////////////////////////////////////////////////////////
//
// headers
//

extern "C" {
#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
}

#include "TermIoHandler.h"

//--- default constructor
TermIoHandler::TermIoHandler()
{
  isTerminal = false;
  fn = fileno(stdin);
  //--- init terminal attributes struct private member for stdin
  if (tcgetattr(fn, &attr) == 0) {
    //--- annotate whether the file handler corresponds to a terminal
    isTerminal = true;
  }
  // printf("isTerminal %d, fn=%d\n", isTerminal?1:0, fn);
}

//--- constructor with file handler argument
TermIoHandler::TermIoHandler(FILE *input)
{
  isTerminal = false;
  fn = fileno(input);
  //--- init terminal attributes struct private member for stdin
  if (tcgetattr(fn, &attr) == 0) {
    //--- annotate whether the file handler corresponds to a terminal
    isTerminal = true;
  }
  // printf("isTerminal %d, fn=%d\n", isTerminal?1:0, fn);
}

TermIoHandler::~TermIoHandler()
{
  //--- restore terminal with initial configuration
  if (isTerminal) {
    tcsetattr(fn, TCSAFLUSH, &attr);
  }
}

//--- setup input terminal to get immediately key-presses, with no echo
void TermIoHandler::SetImmedNoecho()
{
  struct termios attrNew;

  if (isTerminal) {
    memcpy(&attrNew, &attr, sizeof(attr));
    //--- echo off
    attrNew.c_lflag &= ~(ECHO);
    //--- do not wait for newline
    attrNew.c_lflag &= ~(ICANON);
    //--- read at least one char
    attrNew.c_cc[VMIN] = 1;
    // set new terminal attrs
    tcsetattr(fn, TCSAFLUSH, &attrNew);
  }
}

void TermIoHandler::Reset()
{
  //--- restore terminal with initial configuration
  if (isTerminal) {
    tcsetattr(fn, TCSAFLUSH, &attr);
  }
}

#if 0
//
// usage
//

//--- allocate object to setup terminal input
labFisTermIoHandler tioh(stdin);
//--- setup terminal for immediate keypress input with no echo
tioh.SetImmedNoecho();

unsigned char ch = fgetc(stdin);
if (ch == 25) {
  fprintf(stderr,"  --- <control-Y> received, will quit! --- \n");
  quit = 1;
 } else {
  fprintf(stderr,"  --- Press <control-Y> to quit --- \n");
 }

#endif
