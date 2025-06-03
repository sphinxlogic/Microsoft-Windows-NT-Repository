/*
 * $Id: os_unix.h,v 4.18 1993/11/08 19:13:10 mikes Exp $
 *
 * Program:	Operating system dependent routines - Ultrix 4.1
 *
 *
 * Michael Seibel
 * Networks and Distributed Computing
 * Computing and Communications
 * University of Washington
 * Administration Builiding, AG-44
 * Seattle, Washington, 98195, USA
 * Internet: mikes@cac.washington.edu
 *
 * Please address all bugs and comments to "pine-bugs@cac.washington.edu"
 *
 * Copyright 1991-1993  University of Washington
 *
 *  Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee to the University of
 * Washington is hereby granted, provided that the above copyright notice
 * appears in all copies and that both the above copyright notice and this
 * permission notice appear in supporting documentation, and that the name
 * of the University of Washington not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.  This software is made available "as is", and
 * THE UNIVERSITY OF WASHINGTON DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND IN
 * NO EVENT SHALL THE UNIVERSITY OF WASHINGTON BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE) OR STRICT LIABILITY, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Pine and Pico are trademarks of the University of Washington.
 * No commercial use of these trademarks may be made without prior
 * written permission of the University of Washington.
 *
 */

#ifndef	OSDEP_H
#define	OSDEP_H

#include	<string.h>

#undef	CTRL
#include	<signal.h>

#include	<ctype.h>
#include	<types.h>

#include	<stat.h>

/*
 * type qsort() expects
 */
#if	defined(nxt)
#define	QSType	  void
#define QcompType const void
#else
#define	QSType	  int
#define QcompType void
#endif

/*
 * File name separator, as a char and string
 */
#define	C_FILESEP	'/'
#define	S_FILESEP	"/"

/*
 * Place where mail gets delivered (for pico's new mail checking)
 */
#define	MAILDIR		"SYS$LOGIN:"


/*
 * What and where the tool that checks spelling is located.  If this is
 * undefined, then the spelling checker is not compiled into pico.
 */
#define SPELLER         "$ SPELL"

/* memcpy() is no good for overlapping blocks.  If that's a problem, use
 * the memmove() in ../c-client
 */
#define bcopy(a,b,s) memmove (b, a, s)

struct  KBSTREE {
	char	value;
        int     func;              /* Routine to handle it         */
	struct	KBSTREE *down; 
	struct	KBSTREE	*left;
};
extern struct KBSTREE *kpadseqs;
extern int kbseq();


extern char *getcwd(char *, int);

#ifdef ANSI_DRIVER
#define	NROW	24
#define	NCOL	80
#endif

#endif	/* OSDEP_H */
