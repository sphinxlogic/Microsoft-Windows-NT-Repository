/*
 * $Revision: 3.2 $
 * Check-in $Date: 1991/05/16 10:45:25 $
 * $State: Exp $
 *
 * $Author: barnett $
 *
 * $Log: fixstrings.h,v $
 * Revision 3.2  1991/05/16  10:45:25  barnett
 * Better support for System V machines
 * Support for machines with read only text segments
 *
 * Revision 3.1  1991/02/25  22:09:52  barnett
 * Fixed some portability problems
 *
 * Revision 2.1  1990/01/30  14:28:33  jeff
 * Bring RCS version number in line with netwide version 2.1.  No code
 * changes.
 *
 * Revision 2.0  88/06/15  14:41:57  root
 * Baseline release for net posting. ADR.
 */

/* FLUKE jps 16-apr-86 - revector the string routines to custom-coded ones
 *  which handle NULL pointers.
 */
#ifdef SYSV
#define strchr Xindex
#define strrchr Xrindex
#else
#define index Xindex
#define rindex Xrindex
#endif
#define strcat	Xstrcat
#define strncat	Xstrncat
#define strcmp	Xstrcmp
#define strncmp	Xstrncmp
#define strcpy	Xstrcpy
#define strncpy	Xstrncpy
#define strlen	Xstrlen

extern char *Xstrcpy();
extern char *Xindex();
extern char *Xstrcat();
