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
 * $Header: /usr/sww/share/src/X11R5/local/clients/xless-1.4/RCS/callbacks.c,v 1.22 1993/03/02 01:57:10 dglo Exp dglo $
 */

#include "xless.h"

/* buffers for all dialog boxes */
static char search_buffer[MAX_INPUT];
static char newwin_buffer[MAX_INPUT];
static char chgfile_buffer[MAX_INPUT];

/*
 * Button callback functions.
 */

void
Quit(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  QuitFunction();
}

void
Cancel(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  XtPopdown((Widget)closure);
}

void
CallEditor(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;
  const char *editor;
  int cmdlen = 0;
  char *cmd;

  /* figure out which editor they want */
  if ((editor = getenv(EDITORVAR)) == NULL)
    editor = DEFEDITOR;

  /* get a buffer long enough for the entire command */
  if (!resources.editorDoesWindows)
    cmdlen = 9;			/* "xterm -e " */
  cmdlen += strlen(editor) + 1 + strlen(wi->file) + 2;
  if (cmd = (char *)malloc(cmdlen)) {

    /* don't start from an xterm if the editor has its own window */
    if (resources.editorDoesWindows)
      strcpy(cmd, editor);
    else {
      strcpy(cmd, "xterm -e ");
      strcat(cmd, editor);
    }
    strcat(cmd, " ");
    strcat(cmd, wi->file);
    strcat(cmd, "&");
    system(cmd);
    free(cmd);
  }
}

void
Reload(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;
  FILE *file;
  char *new_data;

  if ((file = fopen(wi->file, "r")) == NULL)
    CouldntOpen(wi->base, wi->file);
  else {
    new_data = InitData(file);
    free(wi->memory);
    SetReadText(wi->text, new_data);
    wi->memory = new_data;
  }
}

void
SearchNext(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;

  DoSearch(wi->text, wi->memory, search_buffer, wi->base);
}

static void
popdownAndSearch(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;

  XtPopdown((Widget)wi->search_popup);
  DoSearch(wi->text, wi->memory, search_buffer, wi->base);
}

void
Search(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;

  /* build dialog box if it doesn't exist */
  if (!wi->search_popup)
    wi->search_popup = DialogBox(wi->base, popdownAndSearch, (XtPointer )wi,
				 "Search for:", "Search", search_buffer);

  /* popup search dialog box */
  SetPopup(wi->base, wi->search_popup);
}

static void
popdownAndChange(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;
  FILE *file;
  char *new_data;
  const char *filename;
  XLessFlag flag;
  Arg args[1];

  XtPopdown((Widget)wi->chgfile_popup);

  /* make sure there's a filename there */
  filename = chgfile_buffer;
  if (*filename == 0)
    return;

#ifdef TILDE_EXPANSION
    /* see if we need to do tilde expansion */
    if (filename && *filename == '~') {
      filename = TildeExpand(filename);
      if (*filename != '~')
	flag |= XLessFreeFilename;
    }
#endif /* TILDE_EXPANSION */

  if ((file = fopen(filename, "r")) == NULL)
    CouldntOpen(wi->base, filename);
  else {

    /* read in new file */
    new_data = InitData(file);
    free(wi->memory);
    if (wi->flag & XLessFreeFilename) {
      free(wi->file);
      wi->flag &= ~XLessFreeFilename;
    }
    SetReadText(wi->text, new_data);
    wi->memory = new_data;
    wi->file = filename;
    wi->flag |= flag;

    /* sensitize buttons if previous file was STDIN */
    XtSetArg(args[0], XtNsensitive, True);
    if (wi->editor_button) {
      XtSetValues(wi->editor_button, args, 1);
      wi->editor_button = 0;
    }
    if (wi->reload_button) {
      XtSetValues(wi->reload_button, args, 1);
      wi->editor_button = 0;
    }

    /* set title & icon name */
    SetXNames(wi->base, filename);
  }
}

void
ChangeFile(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;

  if (!wi->chgfile_popup)
    wi->chgfile_popup = DialogBox(wi->base, popdownAndChange, (XtPointer)wi,
				  "Enter filename:", "Ok", chgfile_buffer);

  SetPopup(wi->base, wi->chgfile_popup);	/* Change file dialog box */
}

static void
popdownAndCreate(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;

  XtPopdown((Widget)wi->newwin_popup);
  CreateWindow(wi->base, newwin_buffer);
}

void
NewWindow(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;

  if (!wi->newwin_popup)
    wi->newwin_popup = DialogBox(wi->base, popdownAndCreate, (XtPointer)wi,
			"Enter filename:", "Ok", newwin_buffer);
  SetPopup(wi->base, wi->newwin_popup);	/* New window dialog box */
}

void
CloseWindow(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *sd = (WindowInfo *)closure;

  DestroyWindowInfo(sd);
}

void
Print(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  WindowInfo *wi = (WindowInfo *)closure;
  FILE *file;
  const char *printfile;
  char *cmd;
#ifdef WIMPY_STDIO
  char *line, *l;
#endif

  if (wi->file == NULL) {

    /* create a temporary file */
    printfile = tmpnam(0);
    file = fopen(printfile, "w");

#ifdef WIMPY_STDIO
    /* write stdin (line by line) to temporary file */
    line = l = wi->memory;
    while (line && *line) {
      if (l = strchr(line, '\n'))
	*l = 0;
      fprintf(file, "%s\n", line);
      if (l)
	*l = '\n';
      line = l+1;
    }
#else /* !WIMPY_STDIO */
    /* write stdin (in one manly hunk) to temp file */
    fputs(wi->memory, file);
#endif /* WIMPY_STDIO */
    fclose(file);
  } else
    printfile = wi->file;

  /* create the print command string */
  if (cmd = (char *)malloc(strlen(resources.printCmd) + 1 +
			   strlen(printfile) + 1)) {
    strcpy(cmd, resources.printCmd);
    strcat(cmd, " ");
    strcat(cmd, printfile);
    system(cmd);
    free(cmd);
  }

  /* unlink the file if we created it */
  if (wi->file == NULL)
    unlink(printfile);
}
