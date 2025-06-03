/*-
 * micache.h --
 * 	Header file for users of the MI Pixmap cache scheme
 *
 */

/* $XFree86: mit/server/ddx/mi/micache.h,v 2.0 1993/07/24 07:13:58 dawes Exp $ */

#ifndef _MICACHE_H
#define _MICACHE_H

/*
 * Functionpointer for the Pixmap cache of the accelerated servers
 */

extern void (* miCacheFreeSlot)();

#endif /* _MICACHE_H */
