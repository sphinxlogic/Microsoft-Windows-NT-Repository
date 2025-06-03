/*
  Include declarations
*/
#include <stdio.h>
#if defined(__STDC__) || defined(sgi) || defined(AIXV3)
#include <stdlib.h>
#include <unistd.h>
#else
#ifndef vms
#include <malloc.h>
#include <memory.h>
#endif
#endif
#include <ctype.h>
#include <string.h>
#include <math.h>
#undef index

/*
  Define declarations for the Display program.
*/
#if __STDC__ || defined(sgi) || defined(AIXV3)
#define _Declare(formal_parameters) formal_parameters
#else
#define const 
#define _Declare(formal_parameters) ()
#endif
#define DownShift(x) ((int) ((x)+(1L << 15)) >> 16)
#define False  0
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#define MinInfoSize (1 << 18)
#define True  1
#define UpShift(x) ((x) << 16)
#define UpShifted(x) ((int) ((x)*(1L << 16)+0.5))
#define Warning(message,qualifier)  \
{  \
  (void) fprintf(stderr,"%s: %s",client_name,message);  \
  if (qualifier != (char *) NULL)  \
    (void) fprintf(stderr," (%s)",qualifier);  \
  (void) fprintf(stderr,".\n");  \
}
#ifdef vms
#define pclose(file)  exit(1)
#define popen(command,mode)  exit(1)
#define unlink(file)  remove(file)
#endif

/*
  Variable declarations.
*/
#ifndef lint
static char 
  Version[]="@(#)ImageMagick 2.3 93/01/10 cristy@dupont.com";
#endif
