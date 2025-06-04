/* $XConsortium: TableauP.h,v 1.6 91/03/13 20:12:07 rws Exp $ */

/* Copyright	Massachusetts Institute of Technology	1987, 1988
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef _TableauP_h
#define _TableauP_h

#include "Tableau.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRTableauResource "TableauResource"

typedef struct {
    int empty;
} TableauClassPart;

typedef struct _TableauClassRec {
    CoreClassPart	core_class;
    TableauClassPart	tableau_class;
} TableauClassRec;

extern TableauClassRec tableauClassRec;

typedef struct {
    /* resources */
    String messagefile;
    String keyboardfile;
    String xpmdir;
    String cardset;
    String rules;
    int faceup;
    int facedown;
    int decks;
    int slots;
    int jokers;
    int tmps;
    int param0, param1, param2, param3;
    Pixel cbcolor;
    Pixmap cbpixmap; /* still unused */
    Pixel markcolor;
    Pixel arrowcolor;
    Pixel redcolor;
    Dimension xgap;
    Dimension ygap;
    Dimension linewidth;
    Dimension arrowwidth;
    Dimension arrowheight;
    Dimension markwidth;
    int rng;
} TableauPart;

typedef struct _TableauRec {
    CorePart core;
    TableauPart tableau;
} TableauRec;

#endif /* _TableauP_h */
