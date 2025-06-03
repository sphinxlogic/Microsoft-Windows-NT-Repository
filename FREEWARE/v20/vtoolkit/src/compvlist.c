/* compvlist.c - Show files in a VList widget

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program is a brief example of how to use the VList widget to display the
contents of one or more files in a simple list.

MODIFICATION HISTORY:

05-Sep-1993 (sjk/nfc) Run through the Motif porting tools, all functions changed
	to prototype style.

10-Feb-1993 (sjk) Initial entry.
*/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <unixio.h>
#include <file.h>
#include <stdlib.h>

#include "vtoolkit.h"
#include "vlist.h"
#include "vframe.h"
#include "vheader.h"

#define appName		"compvlist"
#define appClass	"compvlist_defaults"
#define uidFile         "compvlist.uid"

static MrmHierarchy mrmHierarchy;		/* UID file "channel" */
static char *infileNameP;

static Widget widgetA[32];
#define appW		widgetA[0]
#define vListW		widgetA[2]
#define mainW		widgetA[3]

#define ERROR_EXIT_STATUS       44                              /* %SYSTEM-F-ABORT, abort */

#define intab	8

static void Retab(unsigned char *inbufP, unsigned char *outbufP)
{
    unsigned char inChar, *inP, *outP;

    /* First, untab it. */

    for (inP = inbufP, outP = outbufP; inChar = (*inP); inP++) {
	if (inChar != '\t') *(outP++) = inChar;
	else do {*(outP++) = ' ';} while (((outP - outbufP) % intab) != 0);
    }

    /* Trim trailing blanks. */

    while (*(outP - 1) == ' ') outP--;

    *outP = 0;
}

static void FileErrorExit(char *messageP, void *parmP)
{
    printf (messageP, parmP);
    perror ("compvlist");
    printf ("usage:   compvlist filename\n");
    exit (ERROR_EXIT_STATUS);
}

static void LoadCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    int infileDesc, recLength, i;
    unsigned char inBufA[512], retabbedInBufA[512];
    VListLine vLine;

    vLine.mask = VListLineTextMask;
    vLine.textP = (char *)retabbedInBufA;

    if ((infileDesc = open (infileNameP, O_RDONLY, 0)) == (-1))
	FileErrorExit ("compvlist: failed opening input file %s\n", infileNameP);

    VListStartBatchUpdate (vListW);
    VListRemoveAllLines (vListW);

    while ((recLength = read (infileDesc, inBufA, sizeof (inBufA) - 1)) > 0) {	/* leave room for null terminator */
	if (recLength && (inBufA[recLength - 1] == '\n')) recLength--;		/* trim trailing newline, if any */
	inBufA[recLength] = 0;                                         		/* add null terminator */

	Retab (inBufA, retabbedInBufA);

	VListAddLine (vListW, &vLine, VListAddAtBottom);
    }
    VListEndBatchUpdate (vListW);
}

static void QuitCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (1);
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"LoadCallback",		(char *)LoadCallback},
    {"QuitCallback",		(char *)QuitCallback},
};

main(int argc, char **argv)
{
    MrmInitialize ();			/* register toolkit widgets */
    VListInitializeForMRM ();		/* register VList widget */
    VFrameInitializeForMRM ();		/* register VFrame widget */
    VHeaderInitializeForMRM ();		/* register VHeader widget */

    /* Initialize the application. */

    if (!VtkInitialize (appName, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
	return;

    /* Load the main window and its children. */

    (void)VtkFetchWidget (mrmHierarchy, "compVListMain", appW, widgetA, 0);
    XtManageChild (mainW);

    if (argc < 2) {
	printf ("usage:   compvlist filename\n");
	exit (ERROR_EXIT_STATUS);
    }

    infileNameP = (argv[argc - 1]);		/* last input argument is file name */

    /* Work around a (DECwindows V2) toolkit bug to position the application shell based on the main window's position. */

    XtVaSetValues (XtParent (mainW),
	XtNx, mainW->core.x,
	XtNy, mainW->core.y,
	0);

    /* Realize the application shell widget and its children. */

    XtRealizeWidget (appW);

    LoadCallback (0, 0, 0);

    XtMainLoop ();			/* Handle events.  Never returns. */
}
