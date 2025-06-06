/*
 * $XFree86: mit/extensions/lib/globals.c,v 2.1 1993/09/27 12:22:14 dawes Exp $
 * $XConsortium: globals.c,v 1.1 89/10/03 17:25:16 jim Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 *
 *                                 Global data
 *
 * This file should contain only those objects which must be predefined.
 */
#include <X11/Xlib.h>
#include <X11/extensions/Xext.h>
#if __STDC__ && !defined(SCO)
#include <stdlib.h>
#else
#include <sys/param.h>			/* for definition of NULL */
#endif


/*
 * If possible, it is useful to have the global data default to a null value.
 * Some shared library implementations are *much* happier if there isn't any
 * global initialized data.
 */
#if defined(NULL_NOT_ZERO) || defined(SVR3SHLIB) /* then need to initialize */
#define SetZero(t,var,z) t var = z
#else 
#define SetZero(t,var,z) t var
#endif

#ifdef ATTSHAREDLIB			/* then need extra variables */
/*
 * If we need to define extra variables for each global
 */
#if defined(__STDC__) && !defined(UNIXCPP)  /* then ANSI C concatenation */
#define ZEROINIT(t,var,val) SetZero(t,var,val); \
  SetZero (long, _libX_##var##Flag, 0); \
  SetZero (void *, _libX_##var##Ptr, NULL)
#else /* else pcc concatenation */
#define ZEROINIT(t,var,val) SetZero(t,var,val); \
  SetZero (long, _libX_/**/var/**/Flag, 0); \
  SetZero (void *, _libX_/**/var/**/Ptr, NULL)
#endif /* concat ANSI C vs. pcc */

#else /* else not ATTSHAREDLIB */
/*
 * no extra crud
 */
#define ZEROINIT(t,var,val) SetZero (t, var, val)

#endif /* ATTSHAREDLIB */


/*
 * Error handlers; used to be in XlibInt.c
 */
typedef int (*funcptr)();
ZEROINIT (funcptr, _XExtensionErrorFunction, NULL);

#ifdef SVR3SHLIB      /* otherwise it's in XTestExt1.c */
int                     XTestInputActionType = 0;
int                     XTestFakeAckType   = 1;
#endif
#include "libXext.c"

/*
 * NOTE: any additional external definition NEED
 * to be inserted BELOW this point!!!
 */

/*
 * NOTE: any additional external definition NEED
 * to be inserted ABOVE this point!!!
 */

