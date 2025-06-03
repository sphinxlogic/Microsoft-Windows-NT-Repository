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
 * $Header: /usr/sww/share/src/X11R5/local/clients/xless-1.4/RCS/init.c,v 1.27 1993/04/30 18:36:56 dglo Exp $
 */

#include "xless.h"

/*
 *	Function Name: InitData
 *	Description:   This function reads a file and sets it up for
 *		       display in the asciiTextWidget.
 *	Arguments:     file - a file pointer to the file that we are opening.
 *	Returns:       data - a pointer to the data read from the file
 *			      and stored in dynamic memory.
 */

static int dataheight, datawidth;

const char *
InitData(file)
FILE *file;
{
  char *data;		/* pointer to the data stored
			   in dynamic memory */
  struct stat fileinfo;	/* file information from fstat */
  int sz, newsz;		/* # of chars fread has written into data */
  char *tmp, *where;
  int i, w;

  /*
   * Get file size and allocate a chunk of memory for the file to be
   * copied into.
   */

  if (file == stdin) {

    /* create an initial memory buffer */
    if ((data = (char *) malloc(MEMBUF)) == NULL) {
      fprintf(stderr, "xless: not enough memory available.\n");
      exit(1);
    }

    /* read STDIN a chunk at a time */
    fileinfo.st_size = 0;
    where = data;
    while ((sz = fread(where, sizeof(char), MEMBUF, file)) != 0) {
      fileinfo.st_size += sz;
      /* remember old data in case of realloc failure */
      tmp = data;
      newsz = fileinfo.st_size+MEMBUF;
      if ((data = (char *)realloc(data, (unsigned)newsz)) == NULL) {
	fprintf(stderr, "xless: not enough memory available.\n");
	data = tmp;
	break;
      }
      where = data + fileinfo.st_size;
    }

    /* see if file ends with a NEWLINE */
    if (*(data + fileinfo.st_size - 1) != '\n')
      fileinfo.st_size++;

    /* allocate final chunk of memory */
    tmp = data;
    newsz = fileinfo.st_size + 1;
    if ((data = (char *)realloc(data, (unsigned)newsz)) == NULL) {
      fprintf(stderr, "xless: not enough memory available.\n");
      data = tmp;
    }

    /* make sure file ends with a NEWLINE */
    if (*(data + fileinfo.st_size - 1) != '\n')
      *(data + fileinfo.st_size - 1) = '\n';

  } else {

    /* get size of file */
    if (fstat(fileno(file), &fileinfo)) {
      fprintf(stderr, "xless: can't get file size.\n");
      exit(1);
    }

    /* leave space for the NULL (and possibly a final NEWLINE) */
    if ((data = (char *) malloc(fileinfo.st_size + 2)) == NULL) {
      fprintf(stderr, "xless: not enough memory available.\n");
      exit(1);
    }

    /* read file in one swoop */
    fread(data, sizeof(char), fileinfo.st_size, file);
    fclose(file);

    /* make sure file ends with a NEWLINE */
    if (*(data + fileinfo.st_size - 1) != '\n') {
      *(data + fileinfo.st_size) = '\n';
      fileinfo.st_size++;
    }
  }

  /* NUL-terminate file string */
  *(data + fileinfo.st_size) = '\0';

  /* lose all underlined and bold characters in the file */
  i = w = 0;
  dataheight = datawidth = 0;
  tmp = data;
  while (i < fileinfo.st_size) {
    if ((*(data + i) == '_') && (*(data + i + 1) == '\b')) {
      i += 2;
    } else if ((*(data + i) == *(data + i + 2)) &&
	       (*(data + i + 1) == '\b')) {
      i += 2;
    } else {
      *tmp = *(data + i++);
      if (*tmp == '\n') {
	if (w > datawidth)
	  datawidth = w;
	w = 0;
	dataheight++;
      } else
	w++;
      tmp++;
    }
  }
  *tmp = 0;

  return(data);
}

static Widget
makeButton(parent, sensitive, vertdist, prevwidg, procptr, wininfo,
	   accelstr, label)
Widget parent;
Boolean sensitive;
int vertdist;
Widget prevwidg;
XtCallbackProc procptr;
WindowInfo *wininfo;
String accelstr;
const char *label;
{
  Arg args[14];
  Cardinal i;
  XtCallbackRec callback[2];
  Widget w;
  XtAccelerators accel;

  i = 0;
  if (sensitive) {
    XtSetArg(args[i], XtNsensitive, False); i++;
  }
  XtSetArg(args[i], XtNfont, buttonfont); i++;
  XtSetArg(args[i], XtNcursor, stdcur); i++;
  XtSetArg(args[i], XtNwidth, BUTTONWIDTH); i++;
  XtSetArg(args[i], XtNvertDistance, vertdist); i++;
  XtSetArg(args[i], XtNhorizDistance, 4); i++;
  XtSetArg(args[i], XtNfromVert, prevwidg); i++;
  XtSetArg(args[i], XtNfromHoriz, NULL); i++;
  XtSetArg(args[i], XtNtop, XtChainTop); i++;
  XtSetArg(args[i], XtNbottom, XtChainTop); i++;
  XtSetArg(args[i], XtNright, XtChainLeft); i++;
  XtSetArg(args[i], XtNleft, XtChainLeft); i++;
  callback[0].callback = procptr;
  callback[0].closure = (XtPointer )wininfo;
  callback[1].callback = (XtCallbackProc) NULL;
  callback[1].closure = (XtPointer) NULL;
  XtSetArg(args[i], XtNcallback, callback); i++;
  accel = XtParseAcceleratorTable(accelstr);
  XtSetArg(args[i], XtNaccelerators, accel); i++;
  w = XtCreateManagedWidget(label, commandWidgetClass, parent, args, i);
  XtInstallAccelerators(wininfo->text, w);
  return(w);
}

/*
 *	Function Name: MakeToolbox
 *	Description:   This function create all the widgets necessary
 *		       to build the toolbox.
 *	Arguments:     parent   - the parent widget
 *		       cd       - pointer to the data structure containing
 *			          the application data.
 *		       notmain  - TRUE if this is not the main window
 *		       new_wdg  - the shell widget that is the new window
 *		       filename - the name of the file displayed in the window
 *	Returns:       None.
 */

Widget
MakeToolbox(parent, wi, filename)
Widget parent;
WindowInfo *wi;
const char *filename;
{
  Arg args[15];
  Cardinal i;
  Widget box, button;
  const String AccSearch = "#override Meta<Key>S:   set() notify() unset()\n";
  const String AccNext = "#override Meta<Key>N:   set() notify() unset()\n";
  const String AccReload = "#override Meta<Key>R:   set() notify() unset()\n";
  const String AccEdit = "#override Meta<Key>E:   set() notify() unset()\n";
  const String AccChange = "#override Meta<Key>C:   set() notify() unset()\n";
  const String AccWind = "#override Meta<Key>W:   set() notify() unset()\n";
  const String AccPrint = "#override Meta<Key>P: set() notify() unset()\n";
  const String AccHelp = "#override <Key>?: set() notify() unset()\n";
  const String AccClose = "#override <Key>Q: set() notify() unset()\n";
  const String AccQuit = "#override Meta<Key>Q: set() notify() unset()\n";

  /* create the ButtonBox which contains the toolbox */
  i = 0;
  XtSetArg(args[i], XtNallowResize, True); i++;
  XtSetArg(args[i], XtNskipAdjust, True); i++;
  XtSetArg(args[i], XtNhorizDistance, 1); i++;
  XtSetArg(args[i], XtNfromHoriz, wi->text); i++;
  XtSetArg(args[i], XtNtop, XtChainTop); i++;
  XtSetArg(args[i], XtNbottom, XtChainTop); i++;
  XtSetArg(args[i], XtNright, XtChainRight); i++;
  XtSetArg(args[i], XtNleft, XtChainRight); i++;
  XtSetArg(args[i], XtNorientation, XtorientVertical); i++;
  XtSetArg(args[i], XtNshowGrip, False); i++;
  box = XtCreateManagedWidget("toolbox", boxWidgetClass, parent,
				  args, i);

  button = (Widget )NULL;
  button = makeButton(box, FALSE, 1, button, PopupHelp, wi, AccHelp, "Help");
  button = makeButton(box, FALSE, 1, button, Search, wi, AccSearch, "Search");
  button = makeButton(box, FALSE, 1, button, SearchNext, wi,
		      AccNext, "Search Next");
  button = makeButton(box, (filename == NULL ? TRUE : FALSE), 1, button,
		      CallEditor, wi, AccEdit, "Editor");
  if (filename == NULL)
    wi->editor_button = button;
  else
    wi->editor_button = 0;
  button = makeButton(box, (filename == NULL ? TRUE : FALSE), 1, button,
		      Reload, wi, AccReload, "Reload");
  if (filename == NULL)
    wi->reload_button = button;
  else
    wi->reload_button = 0;
  button = makeButton(box, FALSE, 1, button, ChangeFile, wi,
		      AccChange, "Change file");
  button = makeButton(box, FALSE, 1, button, NewWindow, wi,
		      AccWind, "New window");
  button = makeButton(box, FALSE, 1, button, Print, wi, AccPrint, "Print");
  button = makeButton(box, FALSE, 1, button, CloseWindow, wi, AccClose,
		      "Close window");
  if (resources.quitButton)
    button = makeButton(box, FALSE, 1, button, Quit, wi, AccQuit, "Quit");

  return(box);
}

/*
 *	Function Name: MakeText
 *	Description:   This function creates the text widget necessary
 *		       to display the data.
 *	Arguments:     parent - the parent widget
 *		       data   - pointer to data in memory.
 *	Returns:       tmp    - the text widget so created.
 */

Widget
MakeText(parent, data)
Widget parent;
const char *data;
{
  static int width, height;
  Widget txt;
  Arg args[16];
  Cardinal i = 0;

  /* get defaults for text window width&height */
  if (width == 0) {
    int mask, xoffset, yoffset;
    int window_width, window_height;
    int char_width, char_height;

    /* get geometry from resource */
    mask = XParseGeometry(resources.textGeometry, &xoffset, &yoffset,
			  &window_width, &window_height);
    if ((mask & WidthValue) == 0 || (mask & HeightValue) == 0 ||
	window_width <= 0 || window_height <= 0) {
      printf("Bad textGeometry \"%s\"!\n", resources.textGeometry);
      window_width = 80;
      window_height = 24;
    }

    /* get width/height of a text window character */
    char_width = textfont->max_bounds.width;
    char_height = textfont->ascent + textfont->descent;

    /* if actual text is smaller, use it instead */
    if (resources.sizeToFit) {
      if (window_width > datawidth)
	window_width = datawidth + 1;
      if (window_height > dataheight)
	window_height = dataheight;
    }

    /* find width/height of text window in pixels */
    width = window_width * char_width;
    height = window_height * char_height;
  }

  XtSetArg(args[i], XtNstring, data); i++;
  XtSetArg(args[i], XtNscrollVertical, XawtextScrollWhenNeeded); i++;
  XtSetArg(args[i], XtNscrollHorizontal, XawtextScrollWhenNeeded); i++;
  XtSetArg(args[i], XtNwidth, width); i++;
  XtSetArg(args[i], XtNheight, height); i++;
  XtSetArg(args[i], XtNallowResize, True); i++;
  XtSetArg(args[i], XtNskipAdjust, False); i++;
  XtSetArg(args[i], XtNfont, textfont); i++;
  XtSetArg(args[i], XtNhorizDistance, 1); i++;
  XtSetArg(args[i], XtNfromHoriz, NULL); i++;
  XtSetArg(args[i], XtNtop, XtChainTop); i++;
  XtSetArg(args[i], XtNbottom, XtChainBottom); i++;
  XtSetArg(args[i], XtNright, XtChainRight); i++;
  XtSetArg(args[i], XtNleft, XtChainLeft); i++;
  XtSetArg(args[i], XtNdisplayNonprinting, False); i++;
  XtSetArg(args[i], XtNshowGrip, False); i++;
  txt = XtCreateManagedWidget("text", asciiTextWidgetClass,
			      parent, args, i);

  return (txt);
}

void
SetXNames(top, filename)
Widget top;
const char *filename;
{
  static int namelen = 0;
  static char *namestr = 0;
  const char *namePrefix = resources.namePrefix;
  const char *cp;
  int prefixlen, newlen;
  Cardinal i;
  Arg args[2];

  /* make sure there's a filename string */
  if (!filename || *filename == 0)
    return;

  /* figure out how long the prefix is */
  if (namePrefix && *namePrefix)
    prefixlen = strlen(namePrefix);
  else
    prefixlen = 0;

  /* remove path junk if user doesn't want it */
  if (resources.removePath) {
    cp = strrchr(filename, '/');
    if (cp)
      filename = cp + 1;
  }

  /* make sure name buffer is large enough */
  newlen = prefixlen + strlen(filename) + 1;
  if (newlen > namelen) {
    if (namestr)
      free(namestr);
    namestr = (char *)malloc(newlen);
    namelen = newlen;
  }

  /* create title/icon name string */
  strcpy(namestr, namePrefix);
  strcat(namestr, filename);

  /* set window's icon and title names to the file name */
  i = 0;
  XtSetArg(args[i], XtNiconName, namestr); i++;
  XtSetArg(args[i], XtNtitle, namestr); i++;
  XtSetValues(top, args, i);
}

void
SetWMHints(wi)
WindowInfo *wi;
{
  Dimension txt_width, txt_height;
  Dimension win_width, win_height;
  Dimension base_width, base_height;
  int char_width, char_height;
  Cardinal i = 0;
  Arg args[6];

  /* get width/height of entire window */
  XtVaGetValues(wi->base,
		XtNwidth, &win_width,
		XtNheight, &win_height,
		NULL);

  /* get width/height of text window */
  XtVaGetValues(wi->text,
		XtNwidth, &txt_width,
		XtNheight, &txt_height,
		NULL);

  /* get width/height of a text window character */
  char_width = textfont->max_bounds.width;
  char_height = textfont->ascent + textfont->descent;

  /* figure out base width/height */
  base_width = win_width - txt_width;
  base_height = win_height - txt_height;

  /* set base size */
  XtSetArg(args[i], XtNbaseWidth, base_width); i++;
  XtSetArg(args[i], XtNbaseHeight, base_height); i++;

  /* set increment size */
  XtSetArg(args[i], XtNwidthInc, char_width); i++;
  XtSetArg(args[i], XtNheightInc, char_height); i++;

  /* get size of button box */
  XtVaGetValues(wi->toolbox,
		XtNwidth, &win_width,
		XtNheight, &win_height,
		NULL);

  /* guess at size of a 1x1 text window */
  txt_width = base_width + char_width;
  txt_height = base_height + char_height;

  /* set minimum window size */
  XtSetArg(args[i], XtNminWidth,
	   txt_width > win_width ? txt_width : win_width); i++;
  XtSetArg(args[i], XtNminHeight,
	   txt_height > win_height ? txt_height : win_height); i++;

  /* set hints */
  XtSetValues(wi->base, args, i);
}
