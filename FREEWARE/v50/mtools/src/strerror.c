#include "sysincludes.h"

char *strerror(int errno)
{
  return sys_errlist[errno];
}
