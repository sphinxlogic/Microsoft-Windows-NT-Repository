#ifdef SYS5
#define rindex strrchr
#define index strchr
#ifndef VMS
#include <string.h>
#endif
#endif
