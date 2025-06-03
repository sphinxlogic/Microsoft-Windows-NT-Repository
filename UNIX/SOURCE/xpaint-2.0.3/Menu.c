/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
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

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Form.h>

#include "xpaint.h"

Widget MenuBarCreate(Widget parent, int nbar, PaintMenuBar bar[])
{
	static int	checkWidth = 0;
	int	list, item;
	Widget	button = None, menu, entry;
	Widget	prevButton = NULL;
	char 	menuPopupName[80];

	/*
	**  If there is more than one entry in this bar
	**    reparent it.
	*/
	if (nbar > 1) 
		parent = XtVaCreateManagedWidget("menu", 
				formWidgetClass, parent,
				XtNborderWidth, 0,
				XtNbottom, XtChainTop,
				XtNright, XtChainLeft,
				NULL);

	if (checkWidth == 0) {
		checkWidth = 16;	/* XXX */
	}
	
	for (list = 0; list < nbar; list++) {
		strcpy(menuPopupName, bar[list].name);
		strcat(menuPopupName, "Menu");

		button = XtVaCreateManagedWidget(bar[list].name, 
				menuButtonWidgetClass, parent,
				XtNmenuName,  XtNewString(menuPopupName),
				XtNfromHoriz, prevButton,
				NULL);
		prevButton = button;

		menu = XtCreatePopupShell(menuPopupName,
				simpleMenuWidgetClass, button,
				NULL, 0);
			
		for (item = 0; item < bar[list].nitems; item++) {
			if (bar[list].items[item].name[0] == '\0')
				entry = XtVaCreateManagedWidget("seperator",
					smeLineObjectClass, menu,
					NULL);
			else if (bar[list].items[item].flags & MF_CHECK)
				entry = XtVaCreateManagedWidget(
					bar[list].items[item].name,
					smeBSBObjectClass, menu,
					XtNleftMargin, checkWidth + 4,
					NULL);
			else
				entry = XtVaCreateManagedWidget(
					bar[list].items[item].name,
					smeBSBObjectClass, menu,
					NULL);
			
			if (bar[list].items[item].callback != NULL)
				XtAddCallback(entry, XtNcallback, 
						bar[list].items[item].callback, 
						bar[list].items[item].data);
		}
	}

	return (nbar > 1) ? parent : button;
}
