/*
 * This file is an incredible crock to get the normally-inline functions
 * built into the server so that things can be debugged properly.
 */

/* $XFree86: mit/server/ddx/x386/os-support/misc/xf86_IlHack.c,v 2.0 1993/07/12 16:04:49 dawes Exp $ */

#define static /**/
#define __inline__ /**/
#undef NO_INLINE
#include "compiler.h"
