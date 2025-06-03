/* +-------------------------------------------------------------------+ */
/* | Copyright 1993, David Koblas (koblas@netcom.com)                  | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include "Paint.h"
#include "xpaint.h"
#include "Misc.h"
#include "Palette.h"

extern void *GraphicGetData(Widget);

void StdCopyCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget		paint = (Widget)paintArg;
	Pixmap		pix, mask;
	Window		root;
	int		x, y;
	unsigned int	width, height, bw, depth;

	if (!PwGetRegion(paint, &pix, &mask))
		return;

	XGetGeometry(XtDisplay(paint), pix,
			&root, &x, &y, &width, &height, &bw, &depth);

	if (Global.region.pix != None)
		XFreePixmap(XtDisplay(paint), Global.region.pix);
	if (Global.region.mask != None)
		XFreePixmap(XtDisplay(paint), Global.region.mask);

	Global.region.pix    = pix;
	Global.region.mask   = mask;
	Global.region.width  = width;
	Global.region.height = height;

	XtVaGetValues(paint, XtNcolormap, &Global.region.cmap, NULL);
}

void StdPasteCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget		paint = (Widget)paintArg;
	Boolean		flag = False;
	void		*data;
	XRectangle	rect;
	Pixmap		pix;
	Colormap	cmap;
	extern  void	SelectAreaAdd();
	extern  void	SelectBoxAdd();

	if (Global.region.pix == None)
		return;

	if ((int)CurrentOp[0] == (int)SelectAreaAdd ||
	    (int)CurrentOp[0] == (int)SelectBoxAdd)
		flag = True;

	rect.x = 0;
	rect.y = 0;
	rect.width = Global.region.width;
	rect.height = Global.region.height;

	if (flag && (data = GraphicGetData(paint)) != NULL)
		SelectPasteSave(paint, data);

	XtVaGetValues(paint, XtNcolormap, &cmap, NULL);
	if (cmap != Global.region.cmap) {
		pix = PixmapXForm(paint, Global.region.cmap, 
					Global.region.pix, cmap);

		PwPutRegion(paint, pix, Global.region.mask);

		XFreePixmap(XtDisplay(paint), pix);
	} else {
		PwPutRegion(paint, Global.region.pix, Global.region.mask);
	}

	if (flag && (data = GraphicGetData(paint)) != NULL)
		SelectPasteStart(paint, data, &rect);
}

void StdClearCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget	paint = (Widget)paintArg;

	PwUndoStart(paint);
	PwClearRegion(paint, True);
	PwUpdate(paint, NULL, True);
}

void StdCutCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget	paint = (Widget)paintArg;

	StdCopyCallback(w, paint, junk2);
	StdClearCallback(w, paint, junk2);
}
