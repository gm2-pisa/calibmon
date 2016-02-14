////////////////////////////////////////////////////////////////////////
//
//	calibmonrun
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "KeyPress.h"

/*------------------------------------------------------------------*/
bool KeyPress::Hit()
/********************************************************************\

  Routine: Hit()

  Purpose: Returns TRUE if a key is pressed

  Input:
    none

  Output:
    none

  Function value:
    FALSE                 No key has been pressed
    TRUE                  Key has been pressed

  copied from ss_kbhit() in MIDAS system.c

\********************************************************************/
{
  int n;
  
  ioctl(0, FIONREAD, &n);
  return (n > 0);
}

/*------------------------------------------------------------------*/
int KeyPress::Get(bool reset)
/********************************************************************\

  Routine: getchar

  Purpose: Read a single character

  Input:
    Bool_t   reset            Reset terminal to standard mode

  Output:
    <none>

  Function value:
    int             0       for no character available
                    KeyPress::Char  for special character
                    n       ASCII code for normal character

  copied from ss_getchar() in MIDAS system.c

\********************************************************************/
{
  static bool init(false);
  static struct termios save_termios;
  struct termios buf;
   int i, fd;
   char c[3];

   fd = fileno(stdin);

   if (reset) {
      if (init)
         tcsetattr(fd, TCSAFLUSH, &save_termios);
      init = false;
      return 0;
   }

   if (!init) {
      tcgetattr(fd, &save_termios);
      memcpy(&buf, &save_termios, sizeof(buf));

      buf.c_lflag &= ~(ECHO | ICANON | IEXTEN);

      buf.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON);

      buf.c_cflag &= ~(CSIZE | PARENB);
      buf.c_cflag |= CS8;
      /* buf.c_oflag &= ~(OPOST); */
      buf.c_cc[VMIN] = 0;
      buf.c_cc[VTIME] = 0;

      tcsetattr(fd, TCSAFLUSH, &buf);
      init = true;
   }

   memset(c, 0, 3);
   i = read(fd, c, 1);

   if (i == 0)
      return 0;

   /* check if ESC */
   if (c[0] == 27) {
      i = read(fd, c, 2);
      if (i == 0)               /* return if only ESC */
         return 27;

      /* cursor keys return 2 chars, others 3 chars */
      if (c[1] < 65) {
         i = read(fd, c, 1);
      }

      /* convert ESC sequence to KeyPress::Char */
      switch (c[1]) {
      case 49:
	return KeyPress::Char::HOME;
      case 50:
         return KeyPress::Char::INSERT;
      case 51:
         return KeyPress::Char::DELETE;
      case 52:
         return KeyPress::Char::END;
      case 53:
         return KeyPress::Char::PUP;
      case 54:
         return KeyPress::Char::PDOWN;
      case 65:
         return KeyPress::Char::UP;
      case 66:
         return KeyPress::Char::DOWN;
      case 67:
         return KeyPress::Char::RIGHT;
      case 68:
         return KeyPress::Char::LEFT;
      }
   }

   /* BS/DEL -> BS */
   if (c[0] == 127)
      return KeyPress::Char::BS;

   return c[0];
}
