/*
 * $Header: /usr/sww/share/src/X11R5/local/clients/xless-1.4/RCS/window.c,v 1.11 1993/03/02 02:07:05 dglo Exp dglo $
 */

#include "xless.h"
#include "XLessWin.icon"

/* list of all xless windows */
WindowInfo *windowlist = 0;
int windowcount = 0;

/* flag for "too many windows" popup */
static int tooManyFlag = 0;

static WindowInfo *
findWindowInfo(w)
Widget w;
{
  extern WindowInfo *windowlist;
  WindowInfo *wi = windowlist;

  while (w) {

    /* if this is the shell widget... */
    if (XtClass(w) == applicationShellWidgetClass) {

      /* find this window in the list */
      while (wi && (wi->base != w))
        wi = wi->next;
      return(wi);
    }

    /* see if parent is the shell widget */
    w = XtParent(w);
  }

  /* couldn't find a window associated with this widget */
  return(0);
}

static void
popdownTooMany(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  XtPopdown((Widget)closure);
  tooManyFlag = 0;
}

int
CreateWindow(top, filename)
Widget top;
const char *filename;
{
  FILE *file;
  Cardinal i;
  Arg args[2];
  const char *memory;
  Pixmap windowicon;
  Widget new, main_frame;
  WindowInfo *wi;
  XLessFlag flag = XLessClearFlag;
  const char *fixedname;
  static Widget badFileMsg = 0;
  static Widget memErrMsg = 0;

  /* make sure we haven't created *too* many windows */
  if (resources.maxWindows && (windowcount >= resources.maxWindows)) {
    static Widget tooManyMsg = 0;
    char message[64];

    if (!tooManyFlag) {
      tooManyFlag = 1;
      sprintf(message, "Can't have more than %d windows!",
	      resources.maxWindows);
      if (!tooManyMsg)
	tooManyMsg = MessageBox(top, message, "OK", popdownTooMany, 0);
      if (tooManyMsg)
	SetPopup(top, tooManyMsg);
    }
    return 1;
  }

  /* get a filehandle (or exit) */
  if (filename) {

#ifdef TILDE_EXPANSION
    /* see if we need to do tilde expansion */
    if (filename && *filename == '~') {
      filename = TildeExpand(filename);
      if (*filename != '~')
	flag |= XLessFreeFilename;
    }
#endif /* TILDE_EXPANSION */

    /* try to open the file */
    if ((file = fopen(filename, "r")) == NULL) {
      CouldntOpen(top, filename);
      return 2;
    }

    fixedname = filename;

  } else if (windowcount == 0) {

    /* first file can come from stdin */
    fixedname = "stdin";
    file = stdin;

  } else {

    /* don't let 'em get away with this!!! */
    if (!badFileMsg)
      badFileMsg = MessageBox(top, "Please specify a file name!",
			      "OK", 0, 0);
    if (badFileMsg)
      SetPopup(top, badFileMsg);
    return 3;

  }

  /* keep track of the new window */
  if ((wi = (WindowInfo *) malloc(sizeof(WindowInfo))) == NULL){
    if (!memErrMsg)
      memErrMsg= MessageBox(top,
			    "Couldn't save internal window information!",
			    "OK", 0, 0);
    if (memErrMsg)
      SetPopup(top, memErrMsg);
    return 4;
  }
  wi->file = fixedname;
  wi->flag = flag;

  /* read the file into memory */
  memory = InitData(file);
  wi->memory = memory;

  /* create a new application shell */
  i = 0;
  windowicon = XCreateBitmapFromData(disp, XRootWindow(disp,0),
				     XLessWin_bits, XLessWin_width,
				     XLessWin_height);
  XtSetArg(args[i], XtNiconPixmap, windowicon); i++;
  XtSetArg(args[i], XtNallowShellResize, TRUE); i++;
  new = XtAppCreateShell("new", XLESS_CLASS, applicationShellWidgetClass,
			 disp, args, i);

  /* set icon & title name for new window */
  SetXNames(new, fixedname);

  /* create the container for the subwindows */
#ifdef FRAME_IS_FORM
  i = 0;
  main_frame = XtCreateManagedWidget("frame", formWidgetClass, new, args, i);
#else
  i = 0;
  XtSetArg(args[i], XtNorientation, XtorientHorizontal); i++;
  main_frame = XtCreateManagedWidget("frame", panedWidgetClass, new, args, i);
#endif

  /* build widgets for new window */
  wi->base = new;
  wi->text = MakeText(main_frame, memory);
  wi->toolbox = MakeToolbox(main_frame, wi, filename);

  /* make sure text window gets all keystrokes */
  XtSetKeyboardFocus(main_frame, wi->text);

  /* no dialog boxes yet */
  wi->search_popup = 0;
  wi->newwin_popup = 0;
  wi->chgfile_popup = 0;

  /* display the window */
  XtPopup(new, XtGrabNone);

  /* add this window to the list */
  wi->next = windowlist;
  windowlist = wi;
  windowcount++;

  SetWMHints(wi);

  return 0;
}

void
DestroyWindowInfo(wi)
WindowInfo *wi;
{
  WindowInfo *prev;

  /* free all window-related widgets */
  XtDestroyWidget(wi->base);

  /* free all existing dialog boxes */
  if (wi->search_popup)
    XtDestroyWidget(wi->search_popup);
  if (wi->newwin_popup)
    XtDestroyWidget(wi->newwin_popup);
  if (wi->chgfile_popup)
    XtDestroyWidget(wi->chgfile_popup);

  /* free text memory */
  free(wi->memory);

  /* free filename string if it was malloc'd */
  if (wi->flag & XLessFreeFilename)
    free(wi->file);

  /* remove from windowlist chain */
  if (windowlist == wi)
    windowlist = wi->next;
  else {
    prev = windowlist;
    while (prev && (prev->next != wi))
      prev = prev->next;
    if (prev)
      prev->next = wi->next;
  }

  /* one less window to worry about... */
  free(wi);
  windowcount--;

  /* nothing else to do if we've closed all the windows */
  if (windowcount == 0) {
    exit(0);
  }
}

void
DestroyAllWindows()
{
  while (windowlist)
    DestroyWindowInfo(windowlist);
}
