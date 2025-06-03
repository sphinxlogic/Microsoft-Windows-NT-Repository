#include "xless.h"

static void QuitAction ARGS((Widget, XButtonEvent *, String *, Cardinal *));
static void HelpAction ARGS((Widget, XButtonEvent *, String *, Cardinal *));

XtActionsRec actions[] = {
  { "quit",		(XtActionProc )QuitAction },
/* { "search",		(XtActionProc )SearchAction }, */
/* { "search_next",		(XtActionProc )SearchNextAction }, */
/* { "editor",		(XtActionProc )EditorAction }, */
/* { "reload",		(XtActionProc )ReloadAction }, */
/* { "change_file",		(XtActionProc )ChangeFileAction }, */
/* { "new_window",		(XtActionProc )NewWindowAction }, */
  { "help",		(XtActionProc )HelpAction },
};
Cardinal numactions = XtNumber(actions);

static void
QuitAction(w, e, p, n)
Widget w;
XButtonEvent *e;
String *p;
Cardinal *n;
{
  QuitFunction();
}

static void
HelpAction(w, e, p, n)
Widget w;
XButtonEvent *e;
String *p;
Cardinal *n;
{
  PopupHelp(w, NULL, NULL);
}
