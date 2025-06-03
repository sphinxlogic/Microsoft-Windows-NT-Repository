/* libXi : import.h
 * indirection defines
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived partially from work of Thomas Roell
 *
 * $XFree86: mit/extensions/lib/xinput/import.h,v 1.3 1993/05/04 14:57:56 dawes Exp $
 */

#ifndef _libXi_import
#define _libXi_import
#if defined(SVR3SHLIB) && !defined(SVR3SHDAT)

/* prevent name conflict with member free of XFontSetMethods */
#define free(ptr) (*_libXi_free)(ptr)
#define malloc  (*_libXi_malloc)
#define printf  (*_libXi_printf)
#define sprintf  (*_libXi_sprintf)


/* Imported functions declarations
 * Why declaring some imported functions here?
 * This should be done gracefully through including of the systems
 * header files. Unfortunatly there are some source files don't
 * include all headers they should include, there are also some functions
 * nowhere declared in the systems headers and some are declared
 * without extern and cause problems since the names are redefined
 * and these declarations then become undesired false pointer definitions.
 */

/* This section needs editing! It's only an example for the X libs. */

#ifdef __STDC__
#ifndef SCO
extern int printf(char const *, ...);
extern int sprintf(char *, char const *, ...);
#endif
#else
#ifndef SCO
extern int printf();
extern int sprintf();
#endif
#endif

/* use char * also for __STDC__, the sources want it */ 
extern char *malloc();

/* Functions with ambiguous names */

#ifdef __STDC__
extern void free(void *);
#else
extern void (*_libXi_free)();
#endif

#endif
#endif

