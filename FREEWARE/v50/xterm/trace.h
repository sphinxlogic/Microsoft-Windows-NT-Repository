/*
 * $XFree86: xc/programs/xterm/trace.h,v 3.6 1999/04/29 09:14:08 dawes Exp $
 */

/************************************************************

Copyright 1997 by Thomas E. Dickey <dickey@clark.net>

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the above listed
copyright holder(s) not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

THE ABOVE LISTED COPYRIGHT HOLDER(S) DISCLAIM ALL WARRANTIES WITH REGARD
TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL THE ABOVE LISTED COPYRIGHT HOLDER(S) BE
LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/*
 * Common/useful definitions for XTERM application
 */
#ifndef	included_trace_h
#define	included_trace_h

#ifdef HAVE_CONFIG_H
#include <xtermcfg.h>
#endif

#include <ptyx.h>

extern	void	Trace ( char *, ... )
#ifdef GCC_PRINTF
	__attribute__ ((format(printf,1,2)))
#endif
	;
#define TRACE(p) Trace p;

extern	char *	visibleChars (PAIRED_CHARS(Char *buf, Char *buf2), unsigned len);
extern	char *	visibleIChar (IChar *, unsigned);

extern	char	*trace_who;
#define TRACE_CHILD int tracing_child = (trace_who = "child") != 0;

#endif	/* included_trace_h */
