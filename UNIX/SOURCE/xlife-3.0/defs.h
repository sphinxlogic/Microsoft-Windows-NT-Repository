/*
 * XLife Copyright 1989 Jon Bennett jb7m+@andrew.cmu.edu, jcrb@cs.cmu.edu
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

; /* bogus semi-colon to appease the GNU god */
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

#define DIR "/home/X11R5/lib/X11/app-defaults/"
#define NORMALFONT "8x13"
#define BOLDFONT "8x13bold"
#define FONTHEIGHT 13
#define FONTWIDTH 8
#define FONTWIDTH10 82
#define BORDERWIDTH 2

#define MAINWIN 0

#define INPUTWIN 1
#define INPUTXOFF 2
#define INPUTH 20
#define INPUTYOFF FONTHEIGHT
#define INPUTLEN 125
#define INPUTTEXTLEN (INPUTLEN - INPUTFROMLEN)
#define ICOORDS(x,y) (INPUTXOFF + (x) * FONTWIDTH), (INPUTYOFF + (y) * FONTHEIGHT)


#define LIFEWIN 2
#define LIFEXOF

#define COORDWIN 3
#define COORDXOFF 2
#define COORDYOFF FONTHEIGHT
#define CWCOORDS(x,y) (COORDXOFF + (x) * FONTWIDTH), (COORDYOFF + (y) * FONTHEIGHT)
#define COORDW 120

#define XPOS(x,xpos) ((x >> (scale -1)) + xpos) 
#define YPOS(y,ypos) ((y >> (scale -1)) + ypos )

#define RXPOS(x,xpos) ((x - xpos) << (scale -1))
#define RYPOS(y,ypos) ((y - ypos) << (scale -1))

#define STARTPOS 2100000000
/* global data hack goes here to reduce stress */

#define STOP 	0x0
#define HIDE	0x1
#define RUN	0x2


#define DELAY_FAST 0
#define DELAY_MED 250
#define DELAY_SLOW 500

#define GLOBAL extern

/* Definition for load queue to handle delayed loading of included files. 
   Added by Paul Callahan (callahan@cs.jhu.edu) */

#define PATNAMESIZ 100 /* It's an arbitrary limit, but probably enough */

typedef struct lq {
    long loadtime;
    char patname[PATNAMESIZ];
    int relpath;
    unsigned long hotx,hoty;
    int rxx,rxy,ryx,ryy;
    struct lq  *next;
} LoadReq;

/* definitions for geometric transformations */ 

/* Get around "%" bogosity, and compute mod *correctly*. */
#define mod(m,n) (((m)>=0)?((m)%(n)):((n)-1-(-(m)+(n)-1)%(n)))

/* compute coordinate transform for rotations */
/* yes, macros are equivalent, but this makes the meaning clearer (maybe) */
#define tx(x,y,rxx,rxy) ((rxx)*(x)+(rxy)*(y))
#define ty(x,y,ryx,ryy) ((ryx)*(x)+(ryy)*(y))

#include "data.h"
