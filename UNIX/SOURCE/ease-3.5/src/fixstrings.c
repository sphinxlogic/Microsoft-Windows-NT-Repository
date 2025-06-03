/*
 * $Revision: 2.2 $
 * Check-in $Date: 1991/05/16 10:45:25 $
 *
 * $Author: barnett $
 *
 * $Log: fixstrings.c,v $
 * Revision 2.2  1991/05/16  10:45:25  barnett
 * Better support for System V machines
 * Support for machines with read only text segments
 *
 * Revision 2.1  1990/01/30  14:25:14  jeff
 * Comment changes only.
 *
 * Revision 2.0  88/06/15  14:41:19  root
 * Baseline release for net posting. ADR.
 * 
 * Version 1.2  87/02/25  16:55:13  jeff
 * Add RCS header lines.  No code changes.
 * 
 */
#ifdef FLUKE
# ifndef LINT
    static char RCSid[] = "@(#)FLUKE  $Header: /home/kreskin/u0/barnett/Src/ease/src/RCS/fixstrings.c,v 2.2 1991/05/16 10:45:25 barnett Exp $";
# endif LINT
#endif FLUKE

/*  FLUKE jps 16-apr-86 - special hacks for NULL pointers.
 *
 *  The author of ease used a *lot* of NULL pointers.  This isn't much
 *  of a problem on a vax, where NULL pointers look like "".  Not so on a Sun.
 *
 *  We hack around the problem by defining a set of wrappers for the
 *  standard string functions, making it appear as though they accept NULL
 *  pointers.  In the other C files, cpp macros are used to revector the
 *  standard string functions to this file.
 */
#ifdef SYSV
#include <string.h>
#define index strchr
#define rindex strrchr
#else
#include <strings.h>
#endif
#define fix(s) ((s) ? (s) : "")

char *Xstrcat (s1, s2)
char *s1, *s2; 
{
	return (strcat (s1, fix (s2)));
}

char *Xstrncat (s1, s2, n)
char *s1, *s2; 
{
	return (strncat (s1, fix (s2), n));
}

Xstrcmp (s1, s2)
char *s1, *s2; 
{
	return (strcmp (fix (s1), fix (s2)));
}

Xstrncmp (s1, s2, n)
char *s1, *s2; 
{
	return (strncmp (fix (s1), fix (s2), n));
}

char *Xstrcpy (s1, s2)
char *s1, *s2; 
{
	return (strcpy (s1, fix (s2)));
}

char *Xstrncpy (s1, s2, n)
char *s1, *s2; 
{
	return (strncpy (s1, fix (s2), n));
}

Xstrlen (s)
char *s; 
{
	return (strlen (fix (s)));
}

char *Xindex (s, c)
char *s, c; 
{
	return (index (fix (s), c));
}

char *Xrindex (s, c)
char *s, c; 
{
	return (rindex (fix (s), c));
}
