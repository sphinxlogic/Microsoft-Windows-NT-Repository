/*
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
 * $Header: /usr/sww/share/src/X11R5/local/clients/xless-1.4/RCS/util.c,v 1.12 1993/02/26 01:13:12 dglo Exp dglo $
 */

#include "xless.h"

XFontStruct *buttonfont;
XFontStruct *labelfont;
XFontStruct *textfont;
Cursor stdcur;
Cursor dialogcur;

static XawTextPosition Find ARGS((const char *, const char *));

static struct opendata {
  Widget errbox;
  char *errmsg;
} data;

/*
 *	Function name:	SetPopup
 *	Description:	This function pops up the specified dialog box.
 *	Arguments:	top - where to pop up this dialog box
 *	                wdg - the widget to be popped up.
 *	Returns:	None.
 */

void
SetPopup(top, wdg)
Widget top, wdg;
{
  Position x=0, y=0;
  Arg args[5];
  Window rwin;
  Window chwin;
  int rx, ry, wx, wy;
  Dimension wd = 0;
  Dimension he = 0;
  unsigned int mask;

  /* Make the popup shell "wdg" come up at the current pointer position */
  XQueryPointer(XtDisplay(top), XtWindow(top), &rwin, &chwin, &rx, &ry,
		&wx, &wy, &mask);

  XtSetArg(args[0], XtNwidth, &wd);
  XtSetArg(args[1], XtNheight, &he);
  XtGetValues(wdg, args, (Cardinal)2);
  if (wd == 0) wd = DIALOGWIDTH;
  if (he == 0) he = DIALOGHEIGHT;

  x = rx - wd/2;
  y = ry - he/2;

  XtSetArg(args[0], XtNx, x);
  XtSetArg(args[1], XtNy, y);
  XtSetValues(wdg, args, (Cardinal)2);

  /* Popup the widget */
  XtPopup(wdg, XtGrabExclusive);
}

/*
 *	Function Name: Find
 *	Description:   This function search the specified string in
 *		       the specified chunk of memory.
 *	Arguments:     data   - a pointer to the data read from the file
 *			        and stored in dynamic memory.
 *		       string - a pointer to the string to search.
 *	Returns:       #char offset from the current cursor position
 *		       to the beginning of string;
 *		       -1 if string not found.
 */

static XawTextPosition
Find(data, string)
const char *data;
const char *string;
{
  const char *tmpd, *tmps;
  XawTextPosition beg = 0;

  tmpd = data;
  tmps = string;

  while ((*tmpd != '\0')&&(*tmps != '\0')) {
    if (*tmpd == *tmps) {
      tmpd++;
      tmps++;
    } else {
      tmpd++;
      tmps = string;
    }
  }
  if (*tmps == '\0') {
    beg = tmpd - data - strlen(string);
    return(beg);
  }
  else
    return ((XawTextPosition)(-1));
}

/*
 *	Function name:	DoSearch
 *	Description:	This function executes the search for the
 *			specified string.
 *	Arguments:	tex       - the widget to be popped up.
 *			data      - pointer to memory containing the file.
 *			sele      - string to search for.
 *			parent    - the parent widget.
 *	Returns:	None.
 */

void
DoSearch(tex, data, sele, parent)
Widget tex;
const char *data;
const char *sele;
Widget parent;
{
  XawTextPosition beg, end, offset1, offset2;
  int len;
  const char *search_buf;
  static Widget notFound = 0;

  len = strlen(sele);
  offset1 = XawTextGetInsertionPoint(tex);
  search_buf = data + offset1;
  if ((offset2 = Find(search_buf, sele)) != -1) {
    beg = offset1 + offset2;
    end = beg + len;
    XawTextSetInsertionPoint(tex, beg+len);
    XawTextSetSelection(tex, beg, end);
  } else {
    if (!notFound)
      notFound = MessageBox(parent, "String not found .....", "OK", 0, 0);
    if (notFound)
      SetPopup(parent, notFound);
  }
}

/*
 *	Function name:	CheckFonts
 *	Description:	This function checks the resource DB for the
 *			user specified fonts.
 *	Arguments:	None.
 *	Returns:	None.
 */

void
CheckFonts()
{
  if (!(buttonfont = resources.fonts.button))
    buttonfont = resources.fonts.standard;
  if (!(labelfont = resources.fonts.label))
    labelfont = resources.fonts.standard;
  if (!(textfont = resources.fonts.text))
    textfont = resources.fonts.standard;

  dialogcur = resources.cursors.dialog;
  stdcur = resources.cursors.dialog;

  if (!buttonfont || !labelfont || !textfont) {
    fprintf(stderr, "xless: unable to open any of the specified fonts\n");
    exit(1);
  }
}

/*
 *	Function name:	SetReadText
 *	Description:	This function set a new text in a
 *			read-only text widget.
 *	Arguments:	w      - the asciiText widget.
 *			string - the new text source.
 *	Returns:	None.
 */

void
SetReadText(w, string)
Widget w;
char *string;
{
  Arg arg[2];
  Widget source;

  /* Create a new source for the text widget, and put in the new string */
  XtSetArg(arg[0], XtNstring, string);
  XtSetArg(arg[1], XtNeditType, XawtextRead);
  source = XtCreateWidget("Read_Text", asciiSrcObjectClass, w,
			  arg, 2);
  XawTextSetSource(w, source, (XawTextPosition) 0);
}

static void
popdownCouldntOpen(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  XtPopdown(data.errbox);
  XtDestroyWidget(data.errbox);
  free(data.errmsg);
}

/*
 *	Function name:	CouldntOpen
 *	Description:	This function pops up the "Couldn't open"  dialog box.
 *	Arguments:	wdg - the widget to be popped up on.
 *	Returns:	None.
 */

void
CouldntOpen(top, filename)
Widget top;
const char *filename;
{
  const char *msgpart = "Couldn't open file: ";

  if (data.errmsg = (char *)malloc(strlen(msgpart) + strlen(filename) + 1)) {
    strcpy(data.errmsg, msgpart);
    strcat(data.errmsg, filename);

    data.errbox = MessageBox(top, data.errmsg, "OK", popdownCouldntOpen, 0);
    if (data.errbox)
      SetPopup(top, data.errbox);
  }
}

#ifdef TILDE_EXPANSION

#include <pwd.h>

#define USERNAMELEN	9

const char *
TildeExpand(filename)
const char *filename;
{
  struct passwd *pw;
  char username[USERNAMELEN], *bptr = username;
  const char *end;
  int len;

  /* find end of tilde'd name */
  end = strchr(filename, '/');
  if (!end)
    end = filename + strlen(filename);

  /* if it's just '~/...' or '~' */
  if (end == filename + 1) {

    /* look up this UID in passwd file */
    pw = getpwuid(getuid());

  } else {

    /* allocate a buffer if static one is too small */
    len = end - filename;
    if (len > USERNAMELEN)
      bptr = (char *)malloc(len);

    /* copy name into buffer */
    len--;
    strncpy(bptr, filename + 1, len);
    bptr[len] = 0;

    /* look up this user in passwd file */
    pw = getpwnam(bptr);

    /* free allocated memory */
    if (bptr != username)
      free(bptr);
  }

  /* if we found a passwd entry... */
  if (pw) {

    /* get enough memory for expanded string */
    bptr = (char *)malloc(strlen(pw->pw_dir) + strlen(end) + 1);

    /* create new string */
    if (bptr) {
      strcpy(bptr, pw->pw_dir);
      strcat(bptr, end);
      filename = bptr;
    }
  }

  /* return final string */
  return(filename);
}
#endif /* TILDE_EXPANSION */
