/* $XFree86: mit/server/ddx/x386/bdm2/mfb.banked/mfbcustom.h,v 2.0 1993/08/30 15:23:02 dawes Exp $ */

/*
 * This file customizes mfb for a banked dumb mono graphic card server.
 * Included very early by mfb.h and therefore by every mfb*.c file
 */

/*
 * This macro maps function names by adding the hga prefix in order to avoid 
 * collisions and allow coexistance with other mfb libraries within the server.
 */
#define CUSTOMNAME(name)   CATNAME(bdm,name)

#include "bdmBank.h"

/*
 * BANKING_MODS is #defined in files which have had banking macros added.
 * This #define will be removed when all files are done.
*/

#ifdef BANKING_MODS

/* With the banking macros in place, these are simple */

#define MAPRW(p) ((unsigned int*)(p))
#define MAPR(p)  MAPRW(p)
#define MAPW(p)  MAPRW(p)
#define CHECKRWONEXT(x) { x++; CHECKRWO(x); x--; }
#define CHECKRWD(of,uf,x) { CHECKRWOF(of, x); CHECKRWUF(uf, x); }
#define BANKED_MFB

/* mfbbres.c and mfbbresd.c use vgaWriteFlag directly instead of the
 * banking macros. */
#define vgaWriteFlag bdmWriteFlag

#else

/* These are *very* inefficient */

#define MAPRW(p) \
  (((unsigned long)(p)>=BDMBASE) ? \
             (unsigned int *)bdmSetReadWrite(p) : (unsigned int *)(p))
#define MAPR(p) \
  ((unsigned int *)(((unsigned long)(p)>=BDMBASE) ? \
             (unsigned int *)bdmSetRead(p) : (unsigned int *)(p)))
#define MAPW(p) \
  ((unsigned int *)(((unsigned long)(p)>=BDMBASE) ? \
             (unsigned int *)bdmSetWrite(p) : (unsigned int *)(p)))


/* Turn off the bdmBank.h macros */

#undef CHECKSCREEN
#undef SETRWF
#undef CHECKRWOF
#undef CHECKRWUF
#undef BANK_FLAG
#undef BANK_FLAG_BOTH
#undef SETR
#undef SETW
#undef SETRW
#undef CHECKRO
#undef CHECKWO
#undef CHECKRWO
#undef CHECKRU
#undef CHECKWU
#undef CHECKRWU
#undef NEXTR
#undef NEXTW
#undef PREVR
#undef PREVW
#undef SAVE_BANK
#undef RESTORE_BANK
#undef PUSHR
#undef POPR

#define CHECKSCREEN(x)		FALSE
#define SETRWF(f,x)		/**/
#define CHECKRWOF(f,x)		/**/
#define CHECKRWUF(f,x)		/**/
#define BANK_FLAG(a)		/**/
#define BANK_FLAG_BOTH(a,b)	/**/
#define SETR(a)			/**/
#define SETW(a)			/**/
#define SETRW(a)		/**/
#define CHECKRO(a)		/**/
#define CHECKWO(a)		/**/
#define CHECKRWO(a)		/**/
#define CHECKRU(a)		/**/
#define CHECKWU(a)		/**/
#define CHECKRWU(a)		/**/
#define NEXTR(a)		/**/
#define NEXTW(a)		/**/
#define PREVR(a)		/**/
#define PREVW(a)		/**/
#define SAVE_BANK()		/**/
#define RESTORE_BANK()		/**/
#define PUSHR()			/**/
#define POPR()			/**/
#define CHECKRWONEXT(a)		/**/
#define CHECKRWD(a,b,c)		/**/
#endif
