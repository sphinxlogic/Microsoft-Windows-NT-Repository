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
#include <stdio.h>
#include <fcntl.h>
#ifndef NOSTDHDRS
#include <unistd.h>
#endif

#define DEFINE_GLOBAL
#include "xpaint.h"

static char *appDefaults[] = {
#include "XPaint.ad.h"
	NULL
};

void BrushInit(Widget);
void OperationInit(Widget);

void main(int argc, char *argv[])
{
	Widget		toplevel;
	int		i;

	/*
	**  Create the application context
	*/
	toplevel = XtAppInitialize(&Global.appContext, "XPaint", NULL, 0, &argc, argv, 
				appDefaults, NULL, 0);

	/*
	**  Call the initializers
	*/

	OperationInit(toplevel);

	/*
	**  A few rogue initaliziers
	*/
	BrushInit(toplevel);

	/*
	**  Realize it
	*/
	XtRealizeWidget(toplevel);

	/*
	**  Now open any file on the command line
	*/
	for (i = 1; i < argc; i++) {
		extern void	GraphicOpenFile(Widget, char *, void *);
		extern void	*ReadMagic(char *);
		void	*v;

		if ((v = (void *)ReadMagic(argv[i])) != NULL) {
			GraphicOpenFile(toplevel, argv[i], v);
		} else {
			fprintf(stderr, "Unable to open image '%s'\n", argv[i]);
		}
	}

	/*
	**  MainLoop
	*/
	XtAppMainLoop(Global.appContext);
}
