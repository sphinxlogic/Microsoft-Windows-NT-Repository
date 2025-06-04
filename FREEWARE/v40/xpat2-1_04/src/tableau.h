/* $XConsortium: Tableau.h,v 1.5 90/12/19 18:46:00 converse Exp $ */

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

#ifndef _Tableau_h
#define _Tableau_h

/****************************************************************
 *
 * Tableau widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 cbcolor	     Background		Pixel		XtDefaultBackground

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */
#define XtNruleset	"rules"
#define XtCRuleset	"Rules"
#define XtNxpmdir	"xpmdir"
#define XtCXpmdir	"Xpmdir"
#define XtNcardset	"cards"
#define XtCCardset	"Cards"
#define XtNfaceup	"faceup"
#define XtCFaceup	"Faceup"
#define XtNfacedown	"facedown"
#define XtCFacedown	"Facedown"
#define XtNdecks	"decks"
#define XtCDecks	"Decks"
#define XtNslots	"slots"
#define XtCSlots	"Slots"
#define XtNjokers	"jokers"
#define XtCJokers	"Jokers"
#define XtNtmps	 	"tmps"
#define XtCTmps 	"Tmps"
#define XtNparam0	"relaxed"
#define XtNparam1	"flips"
#define XtNparam2	"turn"
#define XtNparam3	"rotations"
#define XtCParam	"Param"
#define XtNfaceup	"faceup"
#define XtCFaceup	"Faceup"
#define XtNmessageFile	"messageFile"
#define XtCMessageFile	"MessageFile"
#define XtNkeyboardFile	"keyboardFile"
#define XtCKeyboardFile	"KeyboardFile"

#define XtNgapx		"gapx"
#define XtNgapy		"gapy"
#define XtCGap		"Gap"
#define XtNarrw		"arrowWidth"
#define XtNarrh		"arrowHeight"
#define XtCArr		"Arrowsize"

#define XtNcbColor	"cardbackColor"
#define XtCCbColor	"CardbackColor"
#define XtNcbPixmap	"cardbackPixmap"
#define XtCCbPixmap	"CardbackPixmap"
#define XtNarColor	"arrowColor"
#define XtCArColor	"ArrowColor"
#define XtNmaColor	"markColor"
#define XtCMaColor	"MarkColor"
#define XtNmaWidth	"markWidth"
#define XtCMaWidth	"MarkWidth"
#define XtNredColor	"redColor"
#define XtCRedColor	"RedColor"
#define XtNrng		"rng"
#define XtCRng		"Rng"

/* declare specific TableauWidget class and instance datatypes */

typedef struct _TableauClassRec*	TableauWidgetClass;
typedef struct _TableauRec*		TableauWidget;

/* declare the class constant */

extern WidgetClass tableauWidgetClass;

#endif /* _Tableau_h */
