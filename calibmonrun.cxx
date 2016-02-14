////////////////////////////////////////////////////////////////////////
//
//	calibmonrun
//

#include <stdio.h>
#include <cassert>

#include "calibmon.h"

int main(int argc, char **argv)
{  
  int kRootServerPort(9090);

  calibmon::begin();
  calibmon::run();
  calibmon::end();
  
  printf("Quitting after cleanup\n");
  return 0;
}
