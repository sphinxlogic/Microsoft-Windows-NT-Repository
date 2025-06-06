#ifndef DEBUG_H
#define DEBUG_H

#include "boolean.h"

extern boolean DEBUG;

#ifdef DEBUGGING
#  include <stdio.h>
#  define Debug(str,val) if (DEBUG) { fprintf(stderr,str,val); }
#  define DebugGSplusPrint(gop,str) if (DEBUG) { GSplusPrint(gop,str); }

#else 

#  define Debug(str,val)
#  define DebugGSplusPrint(gop,str)

#endif

#endif /* DEBUG_H */
