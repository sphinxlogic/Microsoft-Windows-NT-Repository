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
 *
 *      This is the file xless.h for XLess, a file browsing utility
 *      built upon Xlib and the XToolkit.
 *      It contains: Lots of good definitions.
 *
 *	$Header: /usr/sww/share/src/X11R5/local/clients/xless-1.4/RCS/xless.h,v 1.33 1993/04/30 21:39:15 dglo Exp dglo $
 *
 */

/* Std system and C include files */

#include <stdio.h>
#include <X11/Xos.h>
#include <sys/dir.h>
#include <sys/stat.h>

/* X include files */

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>

/* X toolkit Include files */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>

#  include <X11/Xaw/AsciiText.h>
#  include <X11/Xaw/Text.h>
#  include <X11/Xaw/Command.h>
#  include <X11/Xaw/Viewport.h>
#  include <X11/Xaw/Box.h>
#  include <X11/Xaw/Label.h>
#  include <X11/Xaw/Paned.h>
#  include <X11/Xaw/Form.h>

#include <malloc.h>

/* Useful defines */

#define MEMBUF		512000
#define BOXWIDTH	500
#define BOXHEIGHT	50
#define BUTTONWIDTH	100
#define BUTTONHEIGHT	18
#define DIALOGWIDTH	350
#define DIALOGHEIGHT	150
#define MAX_INPUT	256
#define DEF_INPUT_W	200
#define INPUT_HEIGHT	6

/* handle differences between K&R and ANSI C */
#if __STDC__
#define ARGS(a)	a
#else
#define ARGS(a)	()
#define const
#endif

/*
 * The environment variable containing the favorite editor.
 */

#define EDITORVAR "EDITOR"

/*
 * The default editor; make sure the file exists.
 * (Used if the environment variable EDITOR is not set)
 */

#ifndef DEFEDITOR
#define DEFEDITOR "/usr/ucb/vi"
#endif

/*
 * This is the default helpfile and is site specific,
 * make sure you change this.
 */

#ifndef HELPFILE
#define HELPFILE "/usr/lib/X11/xless.help"
#endif

/*
 * This is the default print command and is OS specific.
 * It should probably be "lpr" for BSD, "lp" for SysV
 */

#ifndef PRINTCMD
#define PRINTCMD "/usr/ucb/lpr"
#endif

/*
 * Application class.  You shouldn't change this.
 */
#define XLESS_CLASS "XLess"

/* Default fonts. */
#define STANDARDCUR "left_ptr"

#define STANDARDFONT "fixed"
#define TEXTFONT "-adobe-helvetica-medium-r-normal--12-120-*-*-p-67-iso8859-1"
#define LABELFONT "-adobe-times-bold-i-normal--24-240-*-*-p-128-iso8859-1"
#define BUTTONFONT "-adobe-times-bold-r-normal--14-140-*-*-p-77-iso8859-1"

/* Typedefs */

/*
 * Values for WindowInfo flag
 */
typedef enum _XLessFlagBits {
  XLessClearFlag = 0x00,		/* turn off all bits */
  XLessFreeFilename = 0x01,		/* filename uses malloc'd memory */
} XLessFlag;

/*
 * This structure holds everything xless needs to know about
 * each of its windows.
 */
typedef struct _WindowInfo {
  struct _WindowInfo *next;
  Widget base;
  Widget text;
  Widget toolbox;
  Widget search_popup;
  Widget newwin_popup;
  Widget chgfile_popup;
  Widget editor_button;
  Widget reload_button;
  const char *memory;
  const char *file;
  XLessFlag flag;
} WindowInfo;

typedef struct _XLessFonts {
  XFontStruct
    *standard,	/* The font used if no fonts specied */
    *text,			/* Font used for XLess text */
    *label,			/* Font used for labels in dialog boxes */
    *button;		/* Font used for commandbuttons */
} XLessFonts;

typedef struct _XLessCursors {
  Cursor
    top,			/* The top cursor, default for XLess */
    dialog;			/* Cursor for dialog boxes */
} XLessCursors;

/* Resource manager sets these */

typedef struct _XLessResources {
  XLessFonts fonts;		/* The fonts used for XLess */
  XLessCursors cursors;		/* The cursors used for XLess */
  char *help_file;		/* The help file name */
  Boolean editorDoesWindows;	/* does editor come up in a window? */
  char *printCmd;		/* command used to print file */
  int maxWindows;		/* maximum number of open windows */
  Boolean quitButton;		/* does user want a QUIT button? */
  char *textGeometry;		/* width/height of text window */
  char *namePrefix;		/* string to prepend to file name */
  Boolean removePath;		/* does user want path junk removed? */
  Boolean sizeToFit;		/* should window be as small as possible? */
  Boolean helpMessage;		/* should we just print a help msg and exit? */
} XLessResources;

/* External prototypes */
char *getenv ARGS((const char *));
#ifdef ALL_EXTERNAL_PROTOTYPES
int fclose ARGS((FILE *));
char *fprintf ARGS((FILE *, const char *, ...));
int fputs ARGS((const char *, FILE *));
int fread ARGS((char *, int, int, FILE *));
void *malloc ARGS((size_t));
void *realloc ARGS((void *, size_t));
int system ARGS((const char *));
struct passwd *getpwnam ARGS((const char *));
char *strchr ARGS((const char *, int));
#endif /* ALL_EXTERNAL_PROTOTYPES */

/* prototypes from help.c */
void PopupHelp ARGS((Widget, XtPointer, XtPointer));

/* prototypes from init.c */
const char *InitData ARGS((FILE *));
Widget MakeToolbox ARGS((Widget, WindowInfo *, const char *));
Widget MakeText ARGS((Widget, const char *));
void SetXNames ARGS((Widget, const char *));

/* prototypes from popup.c */
Widget DialogBox ARGS((Widget, XtCallbackProc, XtPointer, const char *,
		       const char *, char *));
Widget MessageBox ARGS((Widget, const char *, ...));
WindowInfo *MakeDialog ARGS((Widget));

/* prototypes from util.c */
void SetPopup ARGS((Widget, Widget));
void DoSearch ARGS((Widget, const char *, const char *, Widget));
void CheckFonts ARGS((void));
void SetReadText ARGS((Widget, char *));
void CouldntOpen ARGS((Widget, const char *));
const char *TildeExpand ARGS((const char *));
extern XFontStruct *buttonfont;
extern XFontStruct *labelfont;
extern XFontStruct *textfont;
extern Cursor stdcur;
extern Cursor dialogcur;

/* prototypes from callbacks.c */
void Quit ARGS((Widget, XtPointer, XtPointer));
void SearchNext ARGS((Widget, XtPointer, XtPointer));
void Cancel ARGS((Widget, XtPointer, XtPointer));
void CallEditor ARGS((Widget, XtPointer, XtPointer));
void Reload ARGS((Widget, XtPointer, XtPointer));
void Search ARGS((Widget, XtPointer, XtPointer));
void ChangeFile ARGS((Widget, XtPointer, XtPointer));
void NewWindow ARGS((Widget, XtPointer, XtPointer));
void CloseWindow ARGS((Widget, XtPointer, XtPointer));
void Print ARGS((Widget, XtPointer, XtPointer));

/* prototypes from functions.c */
void QuitFunction ARGS((void));

/* prototypes from main.c */
void cleanup ARGS((void));
extern Display *disp;
extern XLessResources resources;

/* prototypes from window.c */
int CreateWindow ARGS((Widget, const char *));
void DestroyWindowInfo ARGS((WindowInfo *));
void DestroyAllWindows ARGS((void));
extern WindowInfo *windowlist;
extern int windowcount;

/* prototypes from actions.c */
extern XtActionsRec actions[];
extern Cardinal numactions;
