/* libXext : import.h
 * indirection defines
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived partially from work of Thomas Roell
 *
 * $XFree86: mit/extensions/lib/import.h,v 1.3 1993/05/04 14:57:41 dawes Exp $
 */

#ifndef _libXext_import
#define _libXext_import
#ifdef SVR3SHLIB

#define _iob  (*_libXext__iob)
#define calloc  (*_libXext_calloc)
#define exit  (*_libXext_exit)
#define fprintf  (*_libXext_fprintf)
/* prevent name conflict with member free of XFontSetMethods */
#define free(ptr) (*_libXext_free)(ptr)
#define malloc  (*_libXext_malloc)
#define printf  (*_libXext_printf)
#define sprintf  (*_libXext_sprintf)


/* Imported functions declarations
 * Why declaring some imported functions here?
 * This should be done gracefully through including of the systems
 * header files. Unfortunatly there are some source files don't
 * include all headers they should include, there are also some functions
 * nowhere declared in the systems headers and some are declared
 * without extern and cause problems since the names are redefined
 * and these declarations then become undesired false pointer definitions.
 */

#ifdef __STDC__
extern void exit(int);
#ifndef SCO
extern int printf(char const *, ...);
extern int sprintf(char *, char const *, ...);
#endif
#else
extern void exit();
#ifndef SCO
extern int printf();
extern int sprintf();
#endif
#endif

/* use char * also for __STDC__, the sources want it */ 
extern char *malloc(), *calloc();

/* Functions with ambiguous names */

#ifdef __STDC__
extern void free(void *);
#else
extern void (*_libXext_free)();
#endif

#endif
#endif

