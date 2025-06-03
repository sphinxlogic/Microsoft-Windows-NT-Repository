/*
 * 	$XFree86: mit/clients/xterm/data.h,v 1.4 1993/03/20 06:12:24 dawes Exp $
 *	$XConsortium: data.h,v 1.9 91/02/05 19:44:30 gildea Exp $
 */
/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

extern TekLink *TekRefresh;
extern XPoint T_box2[];
extern XPoint T_box3[];
extern XPoint T_boxlarge[];
extern XPoint T_boxsmall[];
extern XPoint VTbox[];
extern Char *Tbptr;
extern Char *Tbuffer;
extern Char *Tpushb;
extern Char *Tpushback;
extern Char *bptr;
extern char log_def_name[];
extern char *ptydev;
extern char *ttydev;
extern char *xterm_name;
extern Char buffer[];
#ifndef _MINIX
extern int Select_mask;
#endif
extern int T_lastx;
extern int T_lasty;
extern int Tbcnt;
extern int Ttoggled;
#ifndef _MINIX
extern int X_mask;
#endif
extern int am_slave;
extern int bcnt;
#ifdef DEBUG
extern int debug;
#endif	/* DEBUG */
extern int errno;
#ifndef _MINIX
extern int max_plus1;
extern int pty_mask;
#endif
#if _MINIX
extern int read_inprogress;
extern struct v_buf
{
	char *v_buffer;		/* pointer to physical buffer */
	int v_bufbeg;		/* Index of the first data byte in the buffer */
	int v_bufend;		/* Index of the last byte in the buffer + 1 */
	int v_bufsiz;		/* Size of the buffer */
} v_prim;
extern int v_inprogress;
extern int v_fd;
#endif
extern int switchfb[];

extern int waitingForTrackInfo;

extern EventMode eventMode;

extern GC visualBellGC;

extern int VTgcFontMask;
extern int TEKgcFontMask;

extern XtermWidget term;
extern TekWidget tekWidget;
