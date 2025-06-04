/* Handle Solaris 2.4.  */

#include "sol2-3.h"

#define SOLARIS2_4

/* Solaris 2.4 has a broken vfork.  So we don't use it;
   we use the alternate definition in sysdep.c.
   But a header file has a declaration 
   that conflicts with the definition of vfork in sysdep.c.
   This kludge should prevent the conflict.  */
#define pid_t int

/* Get rid of -traditional and let const really do its thing.  */

#ifdef __GNUC__
#undef C_SWITCH_SYSTEM
#undef const
#endif /* __GNUC__ */
