/* libXext : libXext.c
 * exported data
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived partially from work of Thomas Roell
 *
 * $XFree86: mit/extensions/lib/libXext.c,v 1.2 1993/03/27 08:56:50 dawes Exp $
 */

#ifdef SVR3SHLIB

#define VoidInit(var) void *var = 0

VoidInit(_libXext__iob);
VoidInit(_libXext_calloc);
VoidInit(_libXext_exit);
VoidInit(_libXext_fprintf);
VoidInit(_libXext_free);
VoidInit(_libXext_malloc);
VoidInit(_libXext_printf);
VoidInit(_libXext_sprintf);


#ifndef __GNUC__
/*
 * If we are working with floating point aritmetic, stock AT&T cc generates
 * an unresolved reference to __fltused. But we want to make a shared lib from
 * this here and don't want to reference /lib/libc_s.a, just define this sym as
 * (shared lib) static.
 * The trick is that while building the shared lib all references to this
 * symbol are resolved internally. But the symbol will be outside only visible
 * as a static one, so preventing a name conflict with other shared libs.
 */
long __fltused = 0;
#endif

/* A dummy function for free branchtab-slots.
 * This would be a great place for an error-check mechanism for shared libs.
 * (Print error message if not existent function is called.)
 */
extern void _libXext_dummy() {};

#endif


