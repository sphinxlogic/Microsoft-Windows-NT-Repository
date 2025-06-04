/* File: cedit.o
 *
 *	The XColorEdit program.
 */

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>

#include "cedit.h"
#include "appwin.h"


/*** custom resources ***/
static CustomResources my_res;

static XtResource res_list[] = {
	{ "rgbFile", "RGBFile", XmRString, sizeof(char *),
#ifndef VMS
	  XtOffsetOf(CustomResources, rgb_file), XmRImmediate, (char *)"/usr/lib/X11/rgb.txt" }
#else
	  XtOffsetOf(CustomResources, rgb_file), XmRImmediate, (char *)"sys$common:[sysmgr]decw$rgb.dat" }
#endif
};

/*** command line options ***/
static XrmOptionDescRec cmd_options[] = {
	{ "-rgb", "rgbFile", XrmoptionSepArg, NULL }
};

/*** default resources ***/
static String def_resources[] = {
	"XColorEdit*.background:		#f0e0d0",
	"XColorEdit*.foreground:		black",
	"XColorEdit*.XmPushButton.background:	#a07030",
	"XColorEdit*.XmPushButton.foreground:	white",
	"XColorEdit*.XmPushButton.fontList:	-*-helvetica-bold-r-normal--12-*=button_font",
	"XColorEdit*.XmScale.fontList:		-*-helvetica-bold-r-normal--12-*",
	"XColorEdit*.XmTextField.background:	white",
	"XColorEdit*.mainForm.background:	#e0c0b0",
	"XColorEdit*.nameLabel.fontList:	-*-new century schoolbook-bold-r-normal--14-*",
	NULL
};


int main(int argc, char **argv)
{
	XtAppContext		app;
	Widget			top_widget;
	AppWin			*win;

	/*** initialize Xt ***/
	top_widget = XtVaAppInitialize(&app, "XColorEdit",
				       cmd_options, XtNumber(cmd_options),
				       &argc, argv,
				       def_resources, 
				       NULL);

	/*** load custom resources ***/
	XtGetApplicationResources(top_widget, &my_res, res_list, XtNumber(res_list), NULL, 0);

	/*** create the application window ***/
	win = AppWin_create(top_widget, my_res.rgb_file);
	XtRealizeWidget(top_widget);

	/*** standard Xt event loop ***/
	XtAppMainLoop(app);
}
