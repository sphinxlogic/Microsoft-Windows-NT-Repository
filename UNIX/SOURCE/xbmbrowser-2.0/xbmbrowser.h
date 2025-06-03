/*
*****************************************************************************
** xbmbrowser version 2.0  (c) Copyright Ashley Roll, 1992.
** FILE: xbmbrowser.h 
**
** xbmbrowser is Public Domain. However it, and all the code still belong to me.
** I do, however grant permission for you to freely copy and distribute it on 
** the condition that this and all other copyright notices remain unchanged in 
** all distributions.
**
** This software comes with NO warranty whatsoever. I therefore take no
** responsibility for any damages, losses or problems that the program may 
** cause.
*****************************************************************************
*/

#include <X11/Xlib.h>  /* Xt programming includes */
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <stdio.h>
#ifndef SYSV
#include <strings.h>
#endif
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>      
#include <X11/StringDefs.h>

#include <X11/Xaw/Form.h>      /* widget includes */
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Dialog.h> 
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Viewport.h>


/***************************************************************************
    SITE SPECIFIC STUFF - EDIT TO MATCH YOUR SETUP
 ***************************************************************************/
/*
** The full path to the Help file.
*/
#ifndef HELPFILE
#define HELPFILE "/usr/lib/X11/xbmbrowser.help"
#endif

/*
** The full path to the default menu definition file
*/
#ifndef DEFAULT_RC_FILE
#define DEFAULT_RC_FILE "/usr/lib/X11/xbmbrowser.menu"
#endif

/*
** The name of the menu definition file to look for in the users home directory
*/
#ifndef HOME_DIR_FILE
#define HOME_DIR_FILE ".xbmbrowserrc"
#endif


/***************************************************************************
    PROGRAM STUFF - CHANGE IT AT YOUR OWN RISK
 ***************************************************************************/

typedef struct _file_lst {
  char fname[255];
  char is_dir;
  int bmOK;
  Pixmap bitmap;
  struct _file_lst *next;
} FileList;

typedef struct _widget_lst {
  Widget w;
  char fname[255];
  struct _widget_lst *next;
} WidgetLst;

#ifdef MAIN
#define ext
#else
#define ext extern
#endif

ext Cursor normalCursor,waitCursor;
ext char dname[255];
ext FileList *file_list;
ext WidgetLst *widgetList;
ext Widget toplevel,mainpw,bfw,bw,lw,phw,atw,menu;
ext Widget dirPopup,dirList;
ext int ac;
ext char **av;
ext int dialogs_made;

