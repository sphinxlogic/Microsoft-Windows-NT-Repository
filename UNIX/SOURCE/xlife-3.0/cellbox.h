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

typedef struct box {
    unsigned long x,y,live1,live2,olive1,olive2,dead;
    unsigned long on[8];
    struct box *up, *dn, *lf, *rt, *fore, *next,*hfore,*hnext;
}
cellbox;

#define HASHSIZE	32768
#define MAXON		8192	/* max # cells we can change to on per move */
#define MAXOFF		8192	/* max # cells we can change to off per move */
#define BOXSIZE         8

#define link LOCAL_LINK
extern cellbox *link();
extern cellbox *head;
extern cellbox *freep;
extern cellbox *boxes[HASHSIZE];
extern XPoint onpoints[MAXON],offpoints[MAXOFF];
extern XRectangle onrects[MAXON],offrects[MAXOFF];
