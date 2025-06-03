/*
 * xless - X Window System file browser.
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
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
 * Author: Carlo Lisa
 *	   MIT Project Athena
 *	   carlo@athena.mit.edu
 *
 * Date: October 11, 1989
 *
 * $Header: /usr/sww/share/src/X11R5/local/clients/xless-1.4/RCS/main.c,v 1.23 1993/04/30 21:40:35 dglo Exp dglo $
 */

#include "xless.h"
#include "XLessTop.icon"

XtAppContext context;
Widget toplevel;
Display *disp;

XLessResources resources;

#define Offset(field) (XtOffset(XLessResources *, field))

static XtResource priv_resources[] = {
  {"helpFile", XtCFile, XtRString, sizeof(char *),
     Offset(help_file), XtRString, HELPFILE},
  {"standardCur", XtCCursor, XtRCursor, sizeof(Cursor),
     Offset(cursors.top), XtRString, STANDARDCUR},
  {"dialogCur", XtCCursor, XtRCursor, sizeof(Cursor),
     Offset(cursors.dialog), XtRString, STANDARDCUR},
  {"standardFont", XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(fonts.standard), XtRString, STANDARDFONT},
  {"textFont", XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(fonts.text), XtRString, TEXTFONT},
  {"labelFont", XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(fonts.label), XtRString, LABELFONT},
  {"buttonFont", XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(fonts.button), XtRString, BUTTONFONT},
  {"editorDoesWindows", "EditorDoesWindows", XtRBoolean, sizeof(Boolean),
     Offset(editorDoesWindows), XtRString, "False"},
  {"printCommand", "PrintCommand", XtRString, sizeof(char *),
     Offset(printCmd), XtRString, PRINTCMD},
  {"printCmd", "PrintCommand", XtRString, sizeof(char *),
     Offset(printCmd), XtRString, PRINTCMD},
  {"maxWindows", "MaxWindows", XtRInt, sizeof(int),
     Offset(maxWindows), XtRString, "0"},
  {"quitButton", "QuitButton", XtRBoolean, sizeof(Boolean),
     Offset(quitButton), XtRString, "False"},
  {"textGeometry", "Geometry", XtRString, sizeof(char *),
     Offset(textGeometry), XtRString, "80x25"},
  {"sizeToFit", "SizeToFit", XtRBoolean, sizeof(Boolean),
     Offset(sizeToFit), XtRString, "True"},
  {"namePrefix", "NamePrefix", XtRString, sizeof(char *),
     Offset(namePrefix), XtRString, "xless:"},
  {"removePath", "RemovePath", XtRBoolean, sizeof(Boolean),
     Offset(removePath), XtRString, "True"},
  {"helpMessage", "HelpMessage", XtRBoolean, sizeof(Boolean),
     Offset(helpMessage), XtRString, "False"},
};

static XrmOptionDescRec options[] = {
  {"-help",	"*helpMessage", XrmoptionNoArg, "True"},
};

void
cleanup()
{
  XtDestroyWidget(toplevel);
  XtDestroyApplicationContext(context);
}

void
main(argc, argv)
unsigned int argc;
char *argv[];
{
  Arg	arg[10];
  Cardinal n;
  int i;
#ifdef _DEBUG_MALLOC_INC
  {
    union dbmalloptarg	moa;

    moa.i = 0;
    dbmallopt(MALLOC_CKCHAIN, &moa);
  }
#endif

  toplevel = XtAppInitialize(&context, XLESS_CLASS, NULL, (Cardinal )0,
			     &argc, argv, NULL, NULL, 0);

  XtGetApplicationResources(toplevel, (XtPointer )&resources, priv_resources,
			    XtNumber(priv_resources), NULL, (Cardinal) 0);

  XtAppAddActions(context, actions, numactions);

  disp = XtDisplay(toplevel);

  n = 0;
  XtSetArg(arg[n], XtNiconPixmap,
	   XCreateBitmapFromData(disp, XRootWindow(disp, 0),
				 XLessTop_bits, XLessTop_width,
				 XLessTop_height)); n++;
  XtSetValues(toplevel, arg, n);

  CheckFonts();

  for (i = 1; resources.helpMessage || i < argc; i++) {

    /* whine if there's still an argument */
    if (resources.helpMessage || *argv[i] == '-') {
      fprintf(stdout, "usage:\txless filename\n");
      fprintf(stdout, "\t(xless also takes input from stdin)\n");
      cleanup();
      exit(0);
    }

    CreateWindow(toplevel, argv[i]);
  }

  /* if we haven't opened a window yet... */
  if (windowcount == 0) {
    /*
     * Not really necessary to call this an error,
     * but if the control terminal (for commands)
     * and the input file (for data) are the same,
     * we get weird results at best.
     */
    if (isatty(fileno(stdin))) {
      fprintf(stderr, "xless: can't take input from terminal\n");
      cleanup();
      exit(1);
    }

    /* bring up controlling window using stdin as input */
    CreateWindow(toplevel, NULL);
  }

  XtAppMainLoop(context);
}
