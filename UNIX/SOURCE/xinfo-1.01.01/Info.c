#ifndef lint
static char *rcsid = "$Header: /usr3/xinfo/RCS/Info.c,v 1.9 91/01/07 14:01:06 jkh Exp Locker: jkh $";
#endif

#include "InfoP.h"

#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Viewport.h>

#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>

/*
 *
 *                   Copyright 1989, 1990
 *                    Jordan K. Hubbard
 *
 *                PCS Computer Systeme, GmbH.
 *                   Munich, West Germany
 *
 *
 * This file is part of GNU Info widget.
 *
 * The GNU Info widget is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 */

/*
 * $Log:	Info.c,v $
 * Revision 1.9  91/01/07  14:01:06  jkh
 * Now handles compressed files, resource structure now properly declared
 * (XtRInt -> XtRImmediate where appropriate). Node name saved for goto
 * attempts.
 * 
 * Revision 1.8  90/11/12  18:06:46  jkh
 * Removed aggregate initializations. GCC likes them, noone else does.
 * 
 * Revision 1.7  90/11/12  13:46:11  jkh
 * Fixed bug with bell_volume resource
 * 
 * Revision 1.6  90/11/11  23:22:59  jkh
 * Last minute fixes.
 * 
 * Revision 1.5  90/11/11  22:24:05  jkh
 * Added option to enable/disable retention of arg text.
 * 
 * Revision 1.4  90/11/11  21:19:39  jkh
 * Release 1.01
 * 
 * Revision 1.3  90/11/07  01:28:30  jkh
 * Tweaked dialog popup to accept <return> as fast confirm.
 * 
 * Revision 1.2  90/11/06  15:12:47  jkh
 * Fixed memory leaks
 * 
 * Revision 1.1  90/11/06  01:47:28  jkh
 * Initial revision
 * 
 */

#define offset(name)	XtOffset(InfoWidget, info.name)
#define CDT(name)	(XtPointer)(name)

Local XtResource resources[] = {
     { XpNinfoPath, XpCInfoPath, XtRString, sizeof(String),
	    offset(path), XtRString, CDT(XpDefaultInfoPath)		},
     { XpNinfoFile, XpCInfoFile, XtRString, sizeof(String),
	    offset(file), XtRString, CDT(XpDefaultInfoFile)		},
     { XpNinfoNode, XpCInfoNode, XtRString, sizeof(String),
	    offset(node), XtRString, CDT(XpDefaultInfoNode)		},
     { XpNbellVolume, XpCBellVolume, XtRInt, sizeof(int),
	    offset(bell_volume), XtRImmediate, CDT(XpDefaultBellVolume)	},
     { XpNretainArg, XpCRetainArg, XtRBoolean, sizeof(Boolean),
	    offset(retain_arg), XtRImmediate, CDT(False)		},
     { XpNprintCommand, XpCPrintCommand, XtRString, sizeof(String),
	    offset(printCmd), XtRString, CDT(XpDefaultPrintCommand)	},
     { XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t),
	    offset(callback), XtRCallback, CDT(NULL)			},
};

#undef offset
#undef CDT

Local Boolean SetValues();
Local XtGeometryResult GeometryManager();
Local void Destroy();
Local void Initialize();
Local void Realize();
Local void Resize();

/* Routines called directly by actions */
Local void Abort();
Local void ButtonSelection();
Local void Confirm();
Local void NodeDir();
Local void NodeGoto();
Local void NodeHelp();
Local void NodeLast();
Local void NodeMenuSelectByNumber();
Local void NodeNext();
Local void NodePrev();
Local void NodePrint();
Local void NodeQuit();
Local void NodeSearch();
Local void NodeTop();
Local void NodeTutorial();
Local void NodeUp();
Local void NodeXRef();

/* Routines called directly from callbacks or indirectly by actions */
Local void do_dialog_abort();
Local void do_dialog_confirm();
Local void do_goto();
Local void do_menu();
Local void do_menu_sel();
Local void do_next();
Local void do_popdown();
Local void do_prev();
Local void do_quit();
Local void do_search();
Local void do_up();
Local void do_xref();
Local void do_xref_sel();

/* Utility routines */
Local Boolean getNode();
Local Boolean parseTags();
Local InfoWidget find_top();
Local NodeInfo *popNode();
Local NodeInfo *pushNode();
Local String downcase();
Local String eat_whitespace();
Local String file_name();
Local String find_file();
Local String getFile();
Local String get_arg();
Local String normalize_whitespace();
Local String offsetToString();
Local String reverse();
Local String search();
Local String search_back();
Local String strconcat();
Local String substr();
Local String trueName();
Local int findNode();
Local int iindex();
Local int strcomp();
Local int strncomp();
Local void clear_arg();
Local void dialog();
Local void displayHeader();
Local void displayNode();
Local void feep();
Local void getXY();
Local void message();
Local void parseHeader();
Local void parseIndirect();
Local void parseMenu();
Local void parseNode();
Local void parseXRefs();
Local void showStatus();
Local void strccpy();

Local XtActionsRec actionTable[] =
{
     {	"abort",		Abort			},
     {	"confirm",		Confirm			},
     {	"info_click",		ButtonSelection		},
     {	"info_dir",		NodeDir			},
     {	"info_goto",		NodeGoto		},
     {	"info_last",		NodeLast		},
     {	"info_menusel",		NodeMenuSelectByNumber	},
     {	"info_next",		NodeNext		},
     {	"info_nodeSearch",	NodeSearch		},
     {	"info_popupHelp",	NodeHelp		},
     {	"info_prev",		NodePrev		},
     {	"info_print",		NodePrint		},
     {	"info_quit",		NodeQuit		},
     {	"info_top",		NodeTop			},
     {	"info_tutorial",	NodeTutorial		},
     {	"info_up",		NodeUp			},
     {	"info_xref",		NodeXRef		},
     {	NULL,			NULL			}
};

Export InfoClassRec infoClassRec = {
     {	/* core fields */
	/* superclass		*/	(WidgetClass)&compositeClassRec,
	/* class_name		*/	"Info",
	/* widget_size		*/	sizeof(InfoRec),
	/* class_initialize	*/	NULL,
	/* class_part_initialize*/	NULL,
	/* class_inited		*/	FALSE,
	/* initialize		*/	Initialize,
	/* initialize_hook	*/	NULL,
	/* realize		*/	Realize,
	/* actions		*/	actionTable,
	/* num_actions		*/	XtNumber(actionTable),
	/* resources		*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class		*/	NULLQUARK,
	/* compress_motion	*/	TRUE,
	/* compress_exposure	*/	TRUE,
	/* compress_enterleave	*/	TRUE,
	/* visible_interest	*/	FALSE,
	/* destroy		*/	Destroy,
	/* resize		*/	Resize,
	/* expose		*/	XtInheritExpose,
	/* set_values		*/	SetValues,
	/* set_values_hook	*/	NULL,
	/* set_values_almost	*/	XtInheritSetValuesAlmost,
	/* get_values_hook	*/	NULL,
	/* accept_focus		*/	XtInheritAcceptFocus,
	/* version		*/	XtVersion,
	/* callback_private	*/	NULL,
	/* tm_table		*/	NULL,
	/* query_geometry	*/	XtInheritQueryGeometry,
	/* display_accelerator	*/	XtInheritDisplayAccelerator,
	/* extension		*/	NULL
     },
     {	/* composite fields	*/
	/* geometry_manager	*/	GeometryManager,
	/* change_managed	*/	NULL,
	/* insert_child		*/	XtInheritInsertChild,
	/* delete_child		*/	XtInheritDeleteChild,
	/* extension		*/	NULL,
     },
     { /* info fields		*/
	/* empty		*/	0
     }
};

Export WidgetClass infoWidgetClass = (WidgetClass)&infoClassRec;

#ifndef tolower
#include <X11/Xos.h>
#define TOLOWER(c) (tolower(c))
#else
#define TOLOWER(c) (isupper(c) ? tolower(c) : (c))
#endif

Local XtCallbackRec cb[2];
#define XtSetCbk(argarray, rtn, arg) \
     cb[0].callback = rtn; \
     cb[0].closure = (caddr_t)arg; \
     XtSetArg(argarray, XtNcallback, cb)

/*****************************************************************************
 * Widget manipulation routines.                                             *
 *****************************************************************************/

Local Boolean SetValues(current, request, new)
Widget current, request, new;
{
     InfoWidget cw = (InfoWidget)current;
     InfoWidget nw = (InfoWidget)new;
     
     if (cw->info.file != nw->info.file
	 || strcomp(cw->info.file, nw->info.file)
	 || cw->info.node != nw->info.node
	 || strcomp(cw->info.node, nw->info.node)) {
	  XtFree(cw->info.file);
	  XtFree(cw->info.node);
	  getNode(nw, nw->info.file, nw->info.node, NULL);
     }
     /* getNode() does the redisplay implicitly */
     return FALSE;
}

/* We only manage one widget (the pane) directly */
Local XtGeometryResult GeometryManager(w, request, reply)
InfoWidget w;
XtWidgetGeometry *request;
XtWidgetGeometry *reply; /* RETURN */
{
     XtGeometryResult res;
     Dimension width, height;
     
     width = w->core.width;
     height = w->core.height;
     
     /* We don't really care; see what daddy says */
     res = XtMakeGeometryRequest(XtParent(w), request, reply);
     if (res == XtGeometryNo)
	  return res;
     else if (res == XtGeometryAlmost) {
	  if (reply->request_mode & CWWidth)
	       width = reply->width;
	  if (reply->request_mode & CWHeight)
	       height = reply->height;
     }
     else { /* Has to be XtGeometryYes */
	  if (request->request_mode & CWWidth)
	       width = request->width;
	  if (request->request_mode & CWHeight)
	       height = request->height;
     }
     XtResizeWidget(w, width, height, w->core.border_width);
}

Local void Destroy(w)
Widget w;
{
     InfoWidget iw = (InfoWidget)w;
     
     if (INDIRECT(iw).table)
	  FREE_TAG_TABLE(INDIRECT(iw));
     if (TAGTABLE(iw).table)
	  FREE_TAG_TABLE(TAGTABLE(iw));
     while (popNode(iw));	/* popNode will free all but last */
     /* now free the last one */
     if (CURNODE(iw)) {
	  XtFree(CURNODE(iw)->file);
	  XtFree(CURNODE(iw)->node);
	  FREE_LIST(CURNODE(iw)->menu);
	  FREE_LIST(CURNODE(iw)->xref);
	  XtFree(CURNODE(iw));
     }
     if (DATA(iw))
	  XtFree(DATA(iw));
     XtFree(iw->info.file);
     XtFree(iw->info.node);
}

Local void Initialize(request, new)
Widget request;
Widget new;
{
     Arg args[15];
     Cardinal i;
     InfoWidget iw = (InfoWidget)new;
     Widget top, box1, box2, vport, vport2;
     char blanks[MAXSTR], *cp;
     Import char *bzero();
     
     /* create a blank filled string as a placeholder for certain labels */
     for (i = 0; i < MAXSTR - 1; i++)
	  blanks[i] = ' ';
     blanks[i] = '\0';
     
     /* Pick some desperation defaults */
     if (new->core.width == 0)
	  new->core.width = 100;
     if (new->core.height == 0)
	  new->core.height = 50;
     
     /* Prevent later confusion */
     iw->info.arg[0] = '\0';
     
     /* Create outer pane */
     i = 0;
     top = XtCreateManagedWidget("pane1", panedWidgetClass, new, args, i);
     
     /* Create top row of "main control" buttons and labels. */
     i = 0;
     box1 = XtCreateManagedWidget("box1", boxWidgetClass, top, args, i);
     
     if (iw->info.callback) {
	  Widget q;
	  
	  i = 0;
	  q = XtCreateManagedWidget("quit", commandWidgetClass,
				    box1, args, i);
	  XtAddCallback(q, XtNcallback, do_quit, iw);
     }
     i = 0;
     XtSetArg(args[i], XtNlabel, "File: ");				i++;
     iw->info.fileLabel = XtCreateManagedWidget("file", labelWidgetClass,
						box1, args, i);
     i = 0;
     XtSetArg(args[i], XtNlabel, "Node: ");				i++;
     iw->info.nodeLabel = XtCreateManagedWidget("node", labelWidgetClass,
						box1, args, i);
     i = 0;
     XtSetArg(args[i], XtNlabel, "Prev: ");				i++;
     XtSetCbk(args[i], do_prev, iw);					i++;
     iw->info.prevCmd = XtCreateManagedWidget("prev", commandWidgetClass,
					      box1, args, i);
     i = 0;
     XtSetArg(args[i], XtNlabel, "Up: ");				i++;
     XtSetCbk(args[i], do_up, iw);					i++;
     iw->info.upCmd = XtCreateManagedWidget("up", commandWidgetClass,
					    box1, args, i);
     i = 0;
     XtSetArg(args[i], XtNlabel, "Next: ");				i++;
     XtSetCbk(args[i], do_next, iw);					i++;
     iw->info.nextCmd = XtCreateManagedWidget("next", commandWidgetClass,
					      box1, args, i);
     
     /* Create the menu pane */
     i = 0;
     XtSetArg(args[i], XtNallowVert, TRUE);				i++;
     vport = XtCreateManagedWidget("vport1", viewportWidgetClass,
				   top, args, i);
     
     i = 0;
     XtSetCbk(args[i], do_menu_sel, iw);				i++;
     iw->info.menuList = XtCreateManagedWidget("menu", listWidgetClass,
					       vport, args, i);
     /*
      * Create the text area for displaying node contents.
      */
     i = 0;
     XtSetArg(args[i], XtNstring, blanks);				i++;
     XtSetArg(args[i], XtNlength, MAXSTR);				i++;
     XtSetArg(args[i], XtNeditType, XawtextRead);			i++;
     XtSetArg(args[i], XtNuseStringInPlace, TRUE);			i++;
     XtSetArg(args[i], XtNtype, XawAsciiString);			i++;
     iw->info.nodeText = XtCreateManagedWidget("nodeText",
					       asciiTextWidgetClass,
					       top, args, i);
     i = 0;
     XtSetArg(args[i], XtNallowVert, TRUE);				i++;
     vport2 = XtCreateManagedWidget("vport2", viewportWidgetClass,
				    top, args, i);
     
     /* Create the xref pane */
     i = 0;
     XtSetCbk(args[i], do_xref_sel, iw);				i++;
     iw->info.xrefList = XtCreateManagedWidget("xref", listWidgetClass,
					       vport2, args, i);
     
     /*
      * Create the bottom "auxilliary" command button group.
      */
     i = 0;
     box2 = XtCreateManagedWidget("box2", boxWidgetClass,
				  top, args, i);
     i = 0;
     XtSetCbk(args[i], do_menu, iw);					i++;
     iw->info.xrefCmd = XtCreateManagedWidget("menu", commandWidgetClass,
					      box2, args, i);
     i = 0;
     XtSetCbk(args[i], do_xref, iw);					i++;
     iw->info.xrefCmd = XtCreateManagedWidget("xref", commandWidgetClass,
					      box2, args, i);
     i = 0;
     XtSetCbk(args[i], do_goto, iw);					i++;
     iw->info.gotoCmd = XtCreateManagedWidget("goto", commandWidgetClass,
					      box2, args, i);
     i = 0;
     XtSetCbk(args[i], do_search, iw);					i++;
     iw->info.searchCmd = XtCreateManagedWidget("search", commandWidgetClass,
						box2, args, i);
     i = 0;
     bzero(iw->info.arg, ARGLEN);
     XtSetArg(args[i], XtNstring, iw->info.arg);			i++;
     XtSetArg(args[i], XtNlength, ARGLEN);				i++;
     XtSetArg(args[i], XtNuseStringInPlace, TRUE);			i++;
     XtSetArg(args[i], XtNeditType, XawtextEdit);			i++;
     iw->info.argText = XtCreateManagedWidget("arg", asciiTextWidgetClass,
					      box2, args, i);
     
     /*
      * Create the status and message area labels.
      */
     i = 0;
     XtSetArg(args[i], XtNresize, FALSE);				i++;
     XtSetArg(args[i], XtNlabel, blanks);				i++;
     XtSetArg(args[i], XtNborderWidth, 0);				i++;
     iw->info.statusLabel = XtCreateManagedWidget("status", labelWidgetClass,
						  top, args, i);
     i = 0;
     XtSetArg(args[i], XtNresize, FALSE);				i++;
     XtSetArg(args[i], XtNlabel, blanks);				i++;
     XtSetArg(args[i], XtNborderWidth, 0);				i++;
     iw->info.messageLabel = XtCreateManagedWidget("message", labelWidgetClass,
						   top, args, i);
     
     /* set the initial node information */
     ZERO_TABLE(INDIRECT(iw));
     ZERO_TABLE(TAGTABLE(iw));
     DATA(iw) = NULL;
     CURNODE(iw) = NULL;
     
     iw->info.file = XtNewString(iw->info.file);
     iw->info.node = XtNewString(iw->info.node);
     
     if (getNode(iw, iw->info.file, iw->info.node, NULL) == FALSE)
	  message(iw, "?Can't find initial file/node.");
}

Local void Realize(w, value_mask, attributes)
InfoWidget w;
Mask *value_mask;
XSetWindowAttributes *attributes;
{
     if (w->composite.num_children < 1)
	  XtError("No children?!?");
     else {
	  /* Create window with which to manage child */
	  XtCreateWindow(w, (unsigned int)InputOutput,
			 (Visual *)CopyFromParent, *value_mask, attributes);
	  XtResizeWidget(w->composite.children[0], w->core.width,
			 w->core.height, 0);
	  /*
	   * Install accelerators onto widgets we know will need them.
	   * Note that Volume 4 of the O'Reilly "X Toolkit Intrinsics
	   * Programming Manual" (page 204, paragraph 5) says that widgets
	   * should never do this. I disagree: here's a case in point.
	   */
	  XtInstallAllAccelerators(w, w);
	  XtInstallAccelerators(w->info.nodeText, w);
     }
}

Local void Resize(w)
InfoWidget w;
{
     XtResizeWidget(w->composite.children[0], w->core.width, w->core.height,
		    0);
}

/*****************************************************************************
 * Info file manipulation routines.                                          *
 *****************************************************************************/

/* Here is the main guy. Handles all navigation within the info tree. */
Local Boolean getNode(iw, file, node, pushTo)
InfoWidget iw;
String file, node;
NodeInfo *pushTo;
{
     NodeInfo *cur;
     int offset;
     Boolean status = FALSE, needfile;
     
     if (node && index(node, '(') && index(node, ')')) {
	  file = substr(node, iindex(node, '(') + 1,
			iindex(node, ')') - 1);
	  node = index(node, ')') + 1;
     }
     if (!node || !*node)
	  node = "Top";
     
     if (!file) {
	  file = iw->info.file;
	  needfile = !DATA(iw);
     }
     else
	  needfile = !DATA(iw) ||
	       strcomp(file_name(file), file_name(iw->info.file));
     if (needfile) {
	  /* get a new file */
	  if ((file = getFile(iw, file, FALSE)) != NULL) {
	       if (file && iw->info.file != file) {
		    XtFree(iw->info.file);
		    iw->info.file = XtNewString(file);
	       }
	       iw->info.subFile = NULL;
	  }
     }
     else if (!strcomp(node, iw->info.node))
	  return TRUE;	/* we're already there */
     else {
	  XtFree(iw->info.node);
	  iw->info.node = XtNewString(node);
     }
     if (file && (offset = findNode(iw, node)) >= 0) {
	  if (!pushTo) {
	       cur = pushNode(iw, iw->info.file, iw->info.node, offset);
	       parseNode(iw, cur, offset);
	  }
	  else
	       cur = pushTo;
	  displayNode(iw, cur);
	  message(iw, NULL);
	  showStatus(iw, cur);
	  if (!iw->info.retain_arg)
	       clear_arg(iw);
	  status = TRUE;
     }
     else {
	  /* Failed to get the new node, go back (but only once) */
	  if (!pushTo && CURNODE(iw))
	       getNode(iw, CURNODE(iw)->file, CURNODE(iw)->node, CURNODE(iw));
     }
     return status;
}

#ifdef	UNCOMPRESS

Local Inline int iscompressed(fname)
String fname;
{
     int len = strlen(fname);
     
     return len >= 2 && !strcmp(".Z", fname + len - 2);
}

Local FILE* iopen(fname, dir)
String fname;
String dir;
{
     int len = strlen(fname);
     
     if (iscompressed(fname)) {
	  /* This is a compressed file. */
	  char cmd[MAXPATHLEN + sizeof(UNCOMPRESS) + 1];
	  
	  sprintf(cmd, "%s %s", UNCOMPRESS, fname);
	  return popen(cmd, dir);
     }
     else
	  return fopen(fname, dir);
}

#else	/* !UNCOMPRESS */

#define iscompressed(fname)	(0)
#define iopen(fname, dir)	fopen(fname, dir)

#endif	/* UNCOMPRESS */

/* Loads in file "name" and tag/indirect info, if any. */
Local String getFile(iw, name, subfilep)
InfoWidget iw;
String name;
Boolean subfilep;
{
     String ret;
     
     FILE *fp;
     
     ret = find_file(iw->info.path, name);
     if (ret) {
	  Import int stat();
	  struct stat sb;

	  if (!stat(ret, &sb) && (fp = iopen(ret, "r"))) {
	       int expected, total;
	       Boolean compressed = iscompressed(ret);

	       if (DATA(iw)) {
		    XtFree(DATA(iw));
		    DATA(iw) = NULL;
	       }
	       /*
		* If the file is compressed, we make a worst-case guess
		* (though space-wise it's certainly the BEST case) that 
		* the uncompressed data will be 3 times larger than the
		* original file. This is REALLY KLUDGE but there's really
		* not much else to do, short of uncompressing to a temp file
		* which is even less reliable (what if you haven't got enough
		* tmp space?). Oh well. We do realloc it later, once we know
		* how much was really read.
		*/
	       expected = compressed ? 3 * sb.st_size : sb.st_size;
	       DATA(iw) = XtMalloc(expected + 1);
	       if (compressed) {
		    int left;
		    char *next;
		    
		    next = DATA(iw);
		    left = expected;
		    while (left >= 0) {
			 int nread;
			 extern int errno;
		    
			 errno = 0;
			 nread = fread(next, 1, left, fp);
			 if (nread > 0) {
			      next += nread;
			      left -= nread;
			 }
			 else if (errno) {
			      message(iw, "?Read error on pipe for %s.", name);
			      pclose(fp);
			      fp = NULL;
			      XtFree(DATA(iw));
			      ret = NULL;
			      break;
			 }
			 else {
			      pclose(fp);
			      fp = NULL;
			      total = next - DATA(iw);
			      break;
			 }
		    }
		    if (fp) {
			 message(iw, "?Underestimated expansion of %s.", ret);
			 pclose(fp);
			 fp = NULL;
			 XtFree(DATA(iw));
			 ret = NULL;
		    }
		    else /* reclaim excess space */
			 DATA(iw) = XtRealloc(DATA(iw), total + 1);
	       }
	       else {
		    /* Not compressed, just slurp it in */
		    if (fread(DATA(iw), 1, sb.st_size, fp) == sb.st_size) {
			 fclose(fp);
			 total = sb.st_size;
		    }
		    else {
			 message(iw, "?Read error on %s.", name);
			 fclose(fp);
			 XtFree(DATA(iw));
			 ret = NULL;
		    }
	       }
	       if (ret) {
		    DATA(iw)[DATASIZE(iw) = total] = '\0';
		    if (!subfilep) {
			 Boolean needIndirect;
			 needIndirect = parseTags(iw);
			 parseIndirect(iw, needIndirect);
		    }
	       }
	  }
	  else
	       ret = NULL;
     }
     return ret;
}

/* Look through tag table (and/or current buffer) for a node */
Local int findNode(iw, name)
InfoWidget iw;
String name;
{
     ID_P i;
     int offset = -1;
     String s, srch;
     
     /* A node name of "*" means the whole file */
     if (!strcomp(name, "*"))
	  return 0;
     
     if (TAGTABLE(iw).table) {
	  for (i = TAGTABLE(iw).table; I_NAME(*i); i++) {
	       if (!strcomp(I_NAME(*i), name)) {
		    offset = I_OFFSET(*i);
		    break;
	       }
	  }
	  /* if we found the tag and there's an indirect table, adjust */
	  if (offset > 0 && INDIRECT(iw).table) {
	       String sub;
	       
	       for (i = INDIRECT(iw).table; I_NAME(*i); i++) {
		    if (I_OFFSET(*i) > offset)	/* got it */
			 break;
	       }
	       sub = I_NAME(*(--i));
	       if (strcomp(sub, iw->info.subFile)) {
		    if (!getFile(iw, sub, TRUE))
			 return 0;
		    else
			 iw->info.subFile = sub;
	       }
	       offset -= I_OFFSET(*i);
	       /* compensate for header */
	       offset += HDRSIZE(iw);
	  }
     }
     /*
      * Now search forward for the node name. Note that this will
      * work whether or not we found the tag in the tag table. Having
      * found the tag only insures that we search a little less.
      */
     s = START(iw);
     if (offset > 0)
	  s += offset;
     
     /*
      * since bogus tags can leave us *after* the node start as well as
      * before it, we risk a little extra searching and back up to the
      * closest node marker above. Es tut mir leid, but this is what you
      * get with out-of-date tags!
      */
     while (s > START(iw) && !INFO_CHAR(*s))
	  --s;
     srch = strconcat(NODE_TOKEN, name);
     while (s) {
	  if ((s = search(iw, s, END(iw), srch, TRUE)) != NULL) {
	       /* If not an exact match, keep looking */
	       if (!index(NAME, *s))
		    continue;
	       offset = INTOFF(START(iw), s);
	       /* found it, move to the beginning */ 
	       while(!INFO_CHAR(START(iw)[offset - 1]))
		    offset--;
	       s = NULL;
	  }
	  else
	       offset = -1;
     }
     return offset;
}

/* Push a node onto the history list */
Local NodeInfo *pushNode(iw, file, node, offset)
InfoWidget iw;
String file, node;
int offset;
{
     NodeInfo *tmp;
     
     tmp = XtNew(NodeInfo);
     bzero(tmp, sizeof(NodeInfo));
     tmp->file = XtNewString(file);
     tmp->node = XtNewString(node);
     tmp->start = offset;
     tmp->nextNode = CURNODE(iw);
     CURNODE(iw) = tmp;
     return tmp;
}

/* Pop a node off the history list */
Local NodeInfo *popNode(iw)
InfoWidget iw;
{
     NodeInfo *tmp = NULL;
     
     if (CURNODE(iw) && CURNODE(iw)->nextNode) {
	  tmp = CURNODE(iw)->nextNode;
	  XtFree(CURNODE(iw)->file);
	  XtFree(CURNODE(iw)->node);
	  FREE_LIST(CURNODE(iw)->menu);
	  FREE_LIST(CURNODE(iw)->xref);
	  XtFree(CURNODE(iw));
	  CURNODE(iw) = tmp;
     }
     return tmp;
}

/* Parse out all the header/menu/xref information for a node. */
Local void parseNode(iw, n, offset)
InfoWidget iw;
NodeInfo *n;
int offset;
{
     register String start = START(iw) + offset;
     
     /* was the whole file ("*") selected? */
     if (offset == 0) {
	  n->length = DATASIZE(iw);
	  I_START(n->name) = I_LEN(n->name) = 0;
	  I_START(n->prev) = I_LEN(n->prev) = 0;
	  I_START(n->up) = I_LEN(n->up) = 0;
	  I_START(n->next) = I_LEN(n->next) = 0;
	  I_START(n->text) = 0;
	  I_LEN(n->text) = n->length;
     }
     else {
	  /* find the end of the node */
	  n->length = 0;
	  start = START(iw) + offset;
	  while (start < END(iw) && !INFO_CHAR(*start)) {
	       n->length++;
	       start++;
	  }
     }
     /* get the header */
     parseHeader(iw, n);
     /* get the menu items */
     parseMenu(iw, n);
     /* get the cross reference entries */
     parseXRefs(iw, n);
}

Local void parseHeader(iw, n)
InfoWidget iw;
NodeInfo *n;
{
     String strpbrk(), tmp;
     
     /* first, get the node name offset */
     I_START(n->name) = INTOFF(START(iw), NSEARCH(iw, n, NODE_TOKEN));
     I_LEN(n->name) = INTOFF(START(iw), strpbrk(START(iw) + I_START(n->name),
						NAME_END_TOKEN)) -
						     I_START(n->name);
     
     /* now the prev, if any */
     if ((I_START(n->prev) = INTOFF(START(iw),
				    NSEARCH(iw, n, PREV_TOKEN))) > 0)
	  I_LEN(n->prev) = INTOFF(START(iw),
				  strpbrk(START(iw) + I_START(n->prev),
					  NAME_END_TOKEN)) -
					       I_START(n->prev);
     else
	  I_LEN(n->prev) = I_START(n->prev) = 0;
     
     /* and the up, if any */
     if ((I_START(n->up) = INTOFF(START(iw),
				  NSEARCH(iw, n, UP_TOKEN))) > 0)
	  I_LEN(n->up) = INTOFF(START(iw), strpbrk(START(iw) + I_START(n->up),
						   NAME_END_TOKEN)) -
							I_START(n->up);
     else
	  I_LEN(n->up) = I_START(n->up) = 0;
     
     /* the next, if any */
     if ((I_START(n->next) = INTOFF(START(iw),
				    NSEARCH(iw, n, NEXT_TOKEN))) > 0)
	  I_LEN(n->next) = INTOFF(START(iw),
				  strpbrk(START(iw) + I_START(n->next),
					  NAME_END_TOKEN)) -
					       I_START(n->next);
     else
	  I_LEN(n->next) = I_START(n->next) = 0;
     
     /* And finally skip over the header and set the text offset there */
     tmp = START(iw) + I_START(n->name);
     while (*tmp != '\n')
	  tmp++;
     I_START(n->text) = INTOFF(START(iw), tmp + 1);
     I_LEN(n->text) = n->length - (I_START(n->text) - n->start);
}

Local void parseMenu(iw, n)
InfoWidget iw;
NodeInfo *n;
{
     register String mstart;
     String strpbrk();
     
     /* start clean */
     FREE_LIST(n->menu);
     
     /* Does node have a menu? */
     if ((mstart = NSEARCH(iw, n, MENU_TOKEN)) != NULL) {
	  /* Initialize string list */
	  ALLOC_LIST(n->menu);
	  
	  /* go looking for menu items */
	  while (mstart = search(iw, mstart, NEND(iw, n), MENU_SEP_TOKEN,
				 FALSE)) {
	       MAYBE_BUMP_LIST(n->menu);
	       /* put an offset entry in the table */
	       I_LEN(TPOS(n->menu.t)) = 0;
	       I_START(TPOS(n->menu.t)) = INTOFF(START(iw), mstart);
	       while (*(mstart++) != ':')
		    I_LEN(TPOS(n->menu.t))++;
	       /* now save the menu name as a string in the list */
	       LPOS(n->menu) = XtMalloc(I_LEN(TPOS(n->menu.t)) + 1);
	       strncpy(LPOS(n->menu), START(iw) + I_START(TPOS(n->menu.t)),
		       I_LEN(TPOS(n->menu.t)));
	       LPOS(n->menu)[I_LEN(TPOS(n->menu.t))] = '\0';
	       normalize_whitespace(LPOS(n->menu));
	       /* Is the menu name not the node name? */
	       if (*mstart != ':') {
		    int plev = 0;
		    
		    mstart = eat_whitespace(mstart);
		    I_START(TPOS(n->menu.t)) = INTOFF(START(iw), mstart);
		    while (*mstart != '\0' && !(plev == 0 &&
						index(NAME_END_TOKEN,
						      *mstart) != NULL)) {
			 if (*mstart == '(')
			      ++plev;
			 else if (*mstart == ')')
			      --plev;
			 mstart++;
 		    }
		    I_LEN(TPOS(n->menu.t)) =
			 INTOFF(START(iw), mstart) - I_START(TPOS(n->menu.t));
	       }
	       INCP(n->menu.t);
	  }
	  ROUND_LIST(n->menu);
     }
}

Local void parseXRefs(iw, n)
InfoWidget iw;
NodeInfo *n;
{
     register String nstart;
     String strpbrk();
     
     /* start clean */
     FREE_LIST(n->xref);
     
     /* Do we have any cross-reference entries? */
     if ((nstart = search(iw, NSTART(iw, n), NEND(iw, n), NOTE_TOKEN, TRUE))
	 != NULL) {
	  ALLOC_LIST(n->xref);
	  nstart = NSTART(iw, n);
	  
	  /*
	   * Go looking for cross-references (including the one we just
	   * found; wasteful, but avoiding it would make for grotty code).
 	   */
	  while (nstart = search(iw, nstart, NEND(iw, n), NOTE_TOKEN, TRUE)) {
	       /* skip over whitespace */
	       nstart = eat_whitespace(nstart);
	       MAYBE_BUMP_LIST(n->xref);
	       I_LEN(TPOS(n->xref.t)) = 0;
	       I_START(TPOS(n->xref.t)) = INTOFF(START(iw), nstart);
	       while (*(nstart++) != ':')
		    I_LEN(TPOS(n->xref.t))++;
	       /* save the note name as a string */
	       LPOS(n->xref) = XtMalloc(I_LEN(TPOS(n->xref.t)) + 1);
	       strncpy(LPOS(n->xref), START(iw) + I_START(TPOS(n->xref.t)),
		       I_LEN(TPOS(n->xref.t)));
	       LPOS(n->xref)[I_LEN(TPOS(n->xref.t))] = '\0';
	       normalize_whitespace(LPOS(n->xref));
	       /* Is the note name not the first part? */
	       if (*nstart != ':') {
		    nstart = eat_whitespace(nstart + 1);
		    I_START(TPOS(n->xref.t)) = INTOFF(START(iw), nstart);
		    I_LEN(TPOS(n->xref.t)) =
			 INTOFF(START(iw), strpbrk(nstart, NAME_END_TOKEN)) -
			      I_START(TPOS(n->xref.t));
	       }
	       INCP(n->xref.t);
	  }
	  ROUND_LIST(n->xref);
     }
}	       

/* Put the node information on the screen */
Local void displayNode(iw, n)
InfoWidget iw;
NodeInfo *n;
{
     Arg args[5];
     Cardinal i, lst_size;
     String *lst;
     Local char *nolist[] = { "", NULL };     /* make the list widget happy */
     Local char tmpfile[256];
     
     /* Make sure it doesn't try anything cute until we're ready */
     XawTextDisableRedisplay(iw->info.nodeText);
     
     /*
      * There exists a strange bug in the text widget that causes text
      * to be erroneously selected when we're mousing selections. Since
      * we don't want to keep things selected while we're navigating
      * anyway, this is a satisfactory workaround.
      */
     XawTextUnsetSelection(iw->info.nodeText);
     
     /* show the header */
     displayHeader(iw, n);
     
     /* show the menu */
     if (!n->menu.l) {
	  lst = nolist;
	  lst_size = 1;
     }
     else {
	  lst = n->menu.l;
	  lst_size = IDX(n->menu.t);
     }
     XawListChange(iw->info.menuList, lst, lst_size, 0, TRUE);
     
     /* change the xref list */
     if (!n->xref.l) {
	  lst = nolist;
	  lst_size = 1;
     }
     else {
	  lst = n->xref.l;
	  lst_size = IDX(n->xref.t);
     }
     XawListChange(iw->info.xrefList, lst, lst_size, 0, TRUE);
     
     /* Show the new text */
     i = 0;
     if (I_START(n->text)) {
	  char *addr = (START(iw) + I_START(n->text) + I_LEN(n->text));
	  
	  XtSetArg(args[i], XtNstring, START(iw) + I_START(n->text));	i++;
	  XtSetArg(args[i], XtNlength, I_LEN(n->text));			i++;
	  if (INFO_CHAR(*addr))
	       *addr = '\0';
	  else {
	       char msg[256];
	       
	       sprintf(msg, "Encountered bad terminator (%d) for node '%s'",
		       *addr, n->name);
	       XtWarning(msg);
	  }
     }
     else {
	  XtSetArg(args[i], XtNstring, START(iw));			i++;
	  XtSetArg(args[i], XtNlength, DATASIZE(iw));			i++;
     }
     XtSetValues(iw->info.nodeText, args, i);
     
     /* Go for redisplay */
     XawTextEnableRedisplay(iw->info.nodeText);
     
     /* Stick the insertion marker at the top where it's out of the way */
     XawTextSetInsertionPoint(iw->info.nodeText, 0);
}

/* display the header information */
Local void displayHeader(iw, n)
InfoWidget iw;
NodeInfo *n;
{
     Arg args[5];
     Cardinal i;
     String tmp;
     int sensitive;
     
     /* set the file name */
     tmp = strconcat("File: ", file_name(iw->info.file));
     i = 0;
     XtSetArg(args[i], XtNlabel, tmp);					i++;
     XtSetValues(iw->info.fileLabel, args, i);
     
     /* set the node name */
     i = 0;
     if ((tmp = offsetToString(iw, n->name)) != NULL)
	  sensitive = TRUE;
     else
	  sensitive = FALSE;
     XtSetArg(args[i], XtNlabel, strconcat("Node: ", tmp));		i++;
     XtSetArg(args[i], XtNsensitive, sensitive);			i++;
     XtSetValues(iw->info.nodeLabel, args, i);
     
     /* set the prev */
     i = 0;
     if ((tmp = offsetToString(iw, n->prev)) != NULL)
	  sensitive = TRUE;
     else
	  sensitive = FALSE;
     XtSetArg(args[i], XtNlabel, strconcat("Prev: ", tmp));		i++;
     XtSetArg(args[i], XtNsensitive, sensitive);			i++;
     XtSetValues(iw->info.prevCmd, args, i);
     
     /* set the up */
     i = 0;
     if ((tmp = offsetToString(iw, n->up)) != NULL)
	  sensitive = TRUE;
     else
	  sensitive = FALSE;
     XtSetArg(args[i], XtNlabel, strconcat("Up: ", tmp));		i++;
     XtSetArg(args[i], XtNsensitive, sensitive);			i++;
     XtSetValues(iw->info.upCmd, args, i);
     
     /* set the next */
     i = 0;
     if ((tmp = offsetToString(iw, n->next)) != NULL)
	  sensitive = TRUE;
     else
	  sensitive = FALSE;
     XtSetArg(args[i], XtNlabel, strconcat("Next: ", tmp));		i++;
     XtSetArg(args[i], XtNsensitive, sensitive);			i++;
     XtSetValues(iw->info.nextCmd, args, i);
}

/*
 * Look for tag table information in the current buffer. If tag table
 * is indirect, return TRUE, else return false.
 */
Local Boolean parseTags(iw)
InfoWidget iw;
{
     String start, s1;
     char tmp[MAXSTR];
     Boolean indirect = FALSE;
     int i;
     
     /*
      * go back about 8 lines. I don't know if this will always back up
      * past the end marker, but Emacs info seems to think so.
      */
     start = END(iw);
     i = 0;
     while (i < 8)
	  if (*(--start) == '\n')
	       i++;
     
     start = search(iw, start, END(iw), TAGEND_TOKEN, TRUE);
     if (start && (start = search_back(iw, start, START(iw),
				       TAGTABLE_TOKEN, TRUE))) {
	  ALLOC_TABLE(TAGTABLE(iw));
	  /* we were searching backward so move over the token */
	  start += strlen(TAGTABLE_TOKEN);
	  if ((s1 = search(iw, start, start + strlen(ITAGTABLE_TOKEN) + 10,
			   ITAGTABLE_TOKEN, TRUE)) != NULL) {
	       indirect = TRUE;
	       start = s1;
	  }
	  while ((start = search(iw, start, END(iw), NODE_TOKEN, FALSE))
		 != NULL) {
	       MAYBE_BUMP_TABLE(TAGTABLE(iw));
	       strccpy(tmp, start, DEL_CHAR);
	       I_NAME(TPOS(TAGTABLE(iw))) = XtNewString(tmp);
	       start += strlen(tmp) + 1;
	       sscanf(start, "%d", &I_OFFSET(TPOS(TAGTABLE(iw))));
	       INCP(TAGTABLE(iw));
	  }
	  ROUND_TABLE(TAGTABLE(iw));
     }
     else if (TAGTABLE(iw).table)
	  FREE_TAG_TABLE(TAGTABLE(iw));
     return indirect;
}

/* Look for indirect file information in the current buffer */
Local void parseIndirect(iw, needIndirect)
InfoWidget iw;
Boolean needIndirect;
{
     String start;
     char tmp[MAXSTR], *s1;
     
     if (start = search(iw, START(iw), END(iw), INDIRECT_TOKEN, TRUE)) {
	  /* move backwards looking for the INFO_CHAR */
	  for (s1 = start; s1 >= START(iw) && !INFO_CHAR(*s1); s1--);
	  if (s1 < START(iw)) {
	       message(iw, "?Invalid indirect table for %s!", iw->info.file);
	       return;
	  }
	  else
	       HDRSIZE(iw) = INTOFF(START(iw), s1);
	  ALLOC_TABLE(INDIRECT(iw));
	  for (IDX(INDIRECT(iw)) = 0; !INFO_CHAR(*start); INCP(INDIRECT(iw))){
	       MAYBE_BUMP_TABLE(INDIRECT(iw));
	       strccpy(tmp, start, ':');
	       I_NAME(TPOS(INDIRECT(iw))) = XtNewString(tmp);
	       start += strlen(tmp) + 1;
	       sscanf(start, "%d", &I_OFFSET(TPOS(INDIRECT(iw))));
	       start = index(start, '\n') + 1;
	  }
	  ROUND_TABLE(INDIRECT(iw));
     }
     else if (needIndirect)
	  message(iw, "?Indirect table not found for %s! Hilfe!",
		  iw->info.file);
     else if (INDIRECT(iw).table)
	  FREE_TAG_TABLE(INDIRECT(iw));
}

/*****************************************************************************
 * Text display functions.                                                   *
 *****************************************************************************/

/* display a message in the message area */
Local void message(iw, s, p1, p2, p3)
InfoWidget iw;
String s;
caddr_t p1, p2, p3;
{
     char msgbuf[MAXSTR];
     Arg args[5];
     Cardinal i;
     
     i = 0;
     if (s) {
	  sprintf(msgbuf, s, p1, p2, p3);
	  XtSetArg(args[i], XtNlabel, msgbuf);	i++;
	  XtSetValues(iw->info.messageLabel, args, i);
	  feep(iw);
	  if (*s == '?')	/* a dire warning */
	       XtWarning(msgbuf);
     }
     else {	/* clear the message area */
	  XtSetArg(args[i], XtNlabel, " ");	i++;
	  XtSetValues(iw->info.messageLabel, args, i);
     }
}

/* display the current node/file */
Local void showStatus(iw, n)
InfoWidget iw;
NodeInfo *n;
{
     char statbuf[MAXSTR];
     Arg args[5];
     Cardinal i;
     String sub = iw->info.subFile;
     
     sprintf(statbuf, "(%s)%s, %d characters%s", file_name(iw->info.file),
	     iw->info.node, n->length,
	     sub ? strconcat(", subfile: ", sub) : ".");
     i = 0;
     XtSetArg(args[i], XtNlabel, statbuf);	i++;
     XtSetValues(iw->info.statusLabel, args, i);
}

/*****************************************************************************
 * Functions used by actions                                                 *
 *****************************************************************************/

Local void Abort(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     InfoWidget iw = find_top(w);
     
     feep(iw);
     do_dialog_abort(w, iw, NULL);
}

Local void Confirm(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     InfoWidget iw = find_top(w);
     
     if (w == iw->info.argText)
	  (*(iw->info.requester))(w, iw, NULL);
     else
	  do_dialog_confirm(w, iw, NULL);
}

Local void NodeDir(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     InfoWidget iw = find_top(w);
     
     if (getNode(iw, "dir", "Top", NULL) == FALSE)
	  message(iw, "?Yow! The directory seems to have disappeared!\n");
}

Local void NodeNext(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     do_next(NULL, find_top(w), NULL);
}

Local void NodePrev(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     do_prev(NULL, find_top(w), NULL);
}


Local void NodeUp(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     do_up(NULL, find_top(w), NULL);
}

Local void NodeTop(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     InfoWidget iw = find_top(w);
     
     if (getNode(iw, NULL, "Top", NULL) == FALSE)
	  message(iw, "?This node has no top! Bad joss!");
}

Local void NodeLast(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     NodeInfo *tmp;
     InfoWidget iw = find_top(w);
     
     if ((tmp = popNode(iw)) != NULL) {
	  if (getNode(iw, tmp->file, tmp->node, tmp) == FALSE)
	       message(iw, "?Can't pop back to node (%s)%s! We're hosed!",
		       tmp->file, tmp->node);
     }
     else
	  message(iw, "No further history.");
}

Local void NodeXRef(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     do_xref(NULL, find_top(w), NULL);
}

Local void NodeGoto(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     do_goto(NULL, find_top(w), NULL);
}

Local void NodeSearch(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     do_search(NULL, find_top(w), NULL);
}

Local void NodeQuit(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     do_quit(NULL, find_top(w), NULL);
}

Local void NodeTutorial(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     InfoWidget iw = find_top(w);
     
     if (getNode(iw, "info", "Help", NULL) == FALSE)
	  message(iw, "?Hmmm. I can't seem to find the info tutorial!");
}

Local void NodeHelp(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     Cardinal i;
     Arg args[10];
     InfoWidget iw = find_top(w);
     
     if (!iw->info.helpPopup) {
	  Widget hpane, htext;
	  Local XtCallbackRec cb[2];
	  
	  /* create the help popup */
	  i = 0;
	  iw->info.helpPopup = XtCreatePopupShell("help",
						  transientShellWidgetClass,
						  iw, args, i);
	  i = 0;
	  hpane = XtCreateManagedWidget("pane", panedWidgetClass,
					iw->info.helpPopup, args, i);
	  i = 0;
	  cb[0].callback = do_popdown;
	  cb[0].closure = (caddr_t)iw->info.helpPopup;
	  XtSetArg(args[i], XtNcallback, cb);			i++;
	  XtCreateManagedWidget("Close", commandWidgetClass,
				hpane, args, i);
	  i = 0;
	  XtSetArg(args[i], XtNtype, XawAsciiString);		i++;
	  XtSetArg(args[i], XtNeditType, XawtextRead);		i++;
	  htext = XtCreateManagedWidget("text", asciiTextWidgetClass,
					hpane, args, i);
     }
     
     i = 0;
     XtSetArg(args[i], XtNx, event->xbutton.x);			i++;
     XtSetArg(args[i], XtNy, event->xbutton.y);			i++;
     XtSetValues(iw->info.helpPopup, args, i);
     
     XtPopup(iw->info.helpPopup, XtGrabNonexclusive);
}

Local void ButtonSelection(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     char tmp[MAXTOKEN], *idx;
     XawTextPosition beg, end, nlen;
     XawTextBlock ret, asterisk, colon;
     InfoWidget iw = find_top(w);
     
     SET_BLOCK(asterisk, 0, 1, "*");
     SET_BLOCK(colon, 0, 1, ":");
     
     /* Next, try and get a complete "item" selected */
     if ((beg = XawTextSearch(w, XawsdLeft, &asterisk)) != XawTextSearchError)
	  XawTextSetInsertionPoint(w, beg);
     else
	  return;	/* Bomb out */
     if ((end = XawTextSearch(w, XawsdRight, &colon)) != XawTextSearchError &&
	 end > beg) {
	  long len = end - beg;
	  
	  /* Victory! Now try and figure out what it is */
	  if (!XawTextSourceRead(XawTextGetSource(w), beg, &ret, len))
	       return;	/* If can't read, forget it */
	  else while (ret.length && *(ret.ptr) == '*' || isspace(*(ret.ptr)))
	       --ret.length, ++ret.ptr;
	  if (!ret.length || ret.length >= MAXTOKEN) {
	       feep(iw);
	       return;	/* Nothing left, forget it */
          }
	  else {
	       strncpy(tmp, ret.ptr, ret.length);
	       tmp[ret.length] = '\0';
	       normalize_whitespace(tmp);
	       if (!strncomp(tmp, "note ", 5)) {
		    if (!(idx = trueName(iw, CURNODE(iw)->xref, tmp + 5)))
			 feep(iw);
		    else if (getNode(iw, NULL, idx, NULL) == FALSE)
			 message(iw, "?Can't find cross reference for '%s'!",
				 idx);
	       }
	       else {
		    if (!(idx = trueName(iw, CURNODE(iw)->menu, tmp)))
			 feep(iw);
		    else if (getNode(iw, NULL, idx, NULL) == FALSE)
			 message(iw, "?Can't find menu entry for '%s'!",
				 idx);
	       }
	  }		    
     }
}

Local void NodeMenuSelectByNumber(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     Import int atoi();
     int menunum;
     int nitems;
     InfoWidget iw = find_top(w);
     
     nitems = IDX(CURNODE(iw)->menu.t);
     menunum = atoi(*params);
     /* menu number of zero means get menu from arg area */
     if (!menunum)
	  do_menu(NULL, iw, NULL);
     else if (!nitems)
	  message(iw, "No menu for this node.");
     else if (menunum > nitems)
	  message(iw, "There are only %d menu items.", nitems);
     else {
	  XawListHighlight(iw->info.menuList, menunum - 1);
	  if (getNode(iw, NULL,
		      offsetToString(iw,CURNODE(iw)->menu.t.table[menunum-1]),
		      NULL) == FALSE)
	       message(iw, "?Can't find node for menu item #%s", *params);
     }
}

Local void NodePrint(w, event, params, num_params)
Widget   w;
XEvent   *event;
String   *params;
Cardinal *num_params;
{
     Import int unlink();
     String tmp;
     FILE *out;
     InfoWidget iw = find_top(w);
     
     /* if you don't have this routine in your stdlib, make one up */
     tmp = tmpnam(NULL);
     
     if (!CURNODE(iw))
	  message(iw, "?No current node?");
     else if ((out = fopen(tmp, "w")) == NULL)
	  message(iw, "?Can't open temporary file '%s'.", tmp);
     else {
	  String s1 = NSTART(iw, CURNODE(iw));
	  String s2 = NEND(iw, CURNODE(iw));
	  char syscmd[MAXSTR];
	  int stat;
	  
	  fwrite(s1, s2 - s1, 1, out);
	  fclose(out);
	  
	  message(iw, "Sending '%s' to the printer, please wait..",
		  iw->info.node);
	  
	  sprintf(syscmd, "%s %s", iw->info.printCmd, tmp);
	  if ((stat = system(syscmd)) != 0)
	       message(iw, "?'%s' failed with exit status %d. Help!",
		       syscmd, stat);
	  else
	       message(iw, "Finished printing.");
	  unlink(tmp);
     }
     
     
}

/*****************************************************************************
 * Functions used from callback lists.                                       *
 *****************************************************************************/

/* Abort the dialog operation */
Local void do_dialog_abort(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     
     if (w == iw->info.argText)
	  clear_arg(iw);
     else
	  XtDestroyWidget(iw->info.argPopup);
}

/* Confirm the dialog operation */
Local void do_dialog_confirm(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     XawTextBlock blk;
     
     XtDestroyWidget(iw->info.argPopup);
     
     SET_BLOCK(blk, 0, 0, NULL);
     if ((blk.ptr = XawDialogGetValueString(XtParent(w))) &&
	 (blk.length = strlen(blk.ptr))) {
	  if (blk.length > ARGLEN)	/* truncate if necessary */
	       blk.ptr[blk.length = ARGLEN] = '\0';
	  XawTextReplace(iw->info.argText, 0, blk.length, &blk);
	  (*(iw->info.requester))(w, iw, NULL);
     }
}

/*
 * Seems there should be a better way of doing this. Methinks the
 * XtCallbackPopdown() stuff isn't general enough. Should be a way of
 * doing this (and only this).
 */
Local void do_popdown(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     XtPopdown((Widget)client_data);
}

Local void do_prev(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     String tmp;
     
     if ((tmp = offsetToString(iw, CURNODE(iw)->prev))) {
	  if (getNode(iw, NULL, tmp, NULL) == FALSE)
	       message(iw, "?Can't find the previous (%s) for this node.",
		       tmp);
     }
     else
	  message(iw, "Node has no previous");
}

Local void do_quit(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     
     if (XtHasCallbacks(iw, XtNcallback) != XtCallbackHasSome)
	  message(iw, "Sorry, I just don't know how to quit.");
     else
	  XtCallCallbacks(iw, XtNcallback, NULL);
}

Local void do_up(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     String tmp;
     
     if ((tmp = offsetToString(iw, CURNODE(iw)->up))) {
	  if (getNode(iw, NULL, tmp, NULL) == FALSE)
	       message(iw, "?Can't find the up (%s) for this node.", tmp);
     }
     else
	  message(iw, "Node has no up");
}

Local void do_next(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     String tmp;
     
     if ((tmp = offsetToString(iw, CURNODE(iw)->next))) {
	  if (getNode(iw, NULL, tmp, NULL) == FALSE)
	       message(iw, "?Can't find the next (%s) for this node.", tmp);
     }
     else
	  message(iw, "Node has no next");
}

Local void do_xref(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     String tmp;
     
     if ((tmp = get_arg(iw)) != NULL) {
	  if ((tmp = trueName(iw, CURNODE(iw)->xref, tmp)) == NULL)
	       message(iw, "No cross reference entry named '%s' in this node.",
		       get_arg(iw));
	  else if (getNode(iw, NULL, tmp, NULL) == FALSE)
	       message(iw, "?Can't find node for xref item '%s'!",
		       get_arg(iw));
     }
     else
	  dialog(iw, "Please specify a cross reference:", do_xref);
}

Local void do_menu(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     String tmp;
     
     if ((tmp = get_arg(iw)) != NULL) {
	  if ((tmp = trueName(iw, CURNODE(iw)->menu, tmp)) == NULL)
	       message(iw, "No menu entry named '%s' in this node.",
		       get_arg(iw));
	  else if (getNode(iw, NULL, tmp, NULL) == FALSE)
	       message(iw, "?Can't find node for menu item '%s'",
		       get_arg(iw));
     }
     else
	  dialog(iw, "Please specify a menu entry:", do_menu);
}

Local void do_goto(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     String tmp;
     
     if ((tmp = get_arg(iw)) != NULL) {
	  char saveit[ARGLEN];
	  
	  /*
           * The string tmp points to will get nuked if getNode() fails
	   * (as part of the cleanup process), so we need to save it if
	   * we want to be able to print a meaningful error message.
	   */
	  strcpy(saveit, tmp);
	  if (getNode(iw, NULL, tmp, NULL) == FALSE)
	       message(iw, "Can't find a node named %s", saveit);
     }
     else
	  dialog(iw, "Please specify the name of a node to goto:", do_goto);
}

/*
 * Implement a somewhat simplistic search strategy. If file has an indirect
 * list, look for a match in the tag table (since just looking in the current
 * file probably wouldn't be very useful). If not, then search the current
 * file. If we're successful in either case, record the position (in the
 * tags table or the file) so that we don't hit it again right away.
 */
Local void do_search(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     String tmp, s;
     char name[MAXSTR];
     Local struct {
	  String file;
	  caddr_t pos;
     } oldPos;
     
     if ((tmp = get_arg(iw)) != NULL) {
	  /* if remembered position is invalid, reset it */
	  if (strcomp(oldPos.file, iw->info.file)) {
	       oldPos.file = iw->info.file;
	       oldPos.pos = NULL;
	  }
	  if (INDIRECT(iw).table) {
	       ID_P i;
	       int len = strlen(tmp);
	       
	       if (oldPos.pos)
		    i = (ID_P)oldPos.pos;
	       else
		    i = TAGTABLE(iw).table;
	       /* do a tags search */
	       while (I_NAME(*i)) {
		    if (!strncomp(I_NAME(*i), tmp, len))
			 break;
		    i++;
	       }
	       /* success? */
	       if (I_NAME(*i)) {
		    oldPos.pos = (caddr_t)(i + 1);
		    if (getNode(iw, iw->info.file, I_NAME(*i), NULL) == FALSE)
			 message(iw, "?Can't find node for tag %s!",
				 I_NAME(*i));
	       }
	       else {
		    message(iw, "Tag search for '%s' failed.", tmp);
		    oldPos.pos = NULL;
	       }
	  }
	  else {
	       if (oldPos.pos)
		    s = (String)oldPos.pos;
	       else
		    s = START(iw);
	       if ((s = search(iw, s, END(iw), 
			       strconcat(NODE_TOKEN, tmp),
			       TRUE)) != NULL) {
		    int i;
		    
		    oldPos.pos = (caddr_t)s;
		    strcpy(name, tmp);
		    i = strlen(name);
		    while (!index(NAME, *s))
			 name[i++] = *s++;
		    name[i] = '\0';
		    if (getNode(iw, iw->info.file, name, NULL) == FALSE)
			 message(iw, "?Can't find node name in search!");
	       }
	       else {
		    message(iw, "Search for '%s' failed.", tmp);
		    oldPos.pos = NULL;
	       }
	  }
     }
     else
	  dialog(iw, "Please enter a string to search for:", do_search);
}

/* These two handle selections from the menu and xref lists */

Local void do_menu_sel(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     XawListReturnStruct *rs = (XawListReturnStruct *)call_data;
     
     if (getNode(iw, NULL, trueName(iw, CURNODE(iw)->menu, rs->string),
		 NULL) == FALSE)
	  message(iw, "?Can't find node for menu item '%s'", rs->string);
}

Local void do_xref_sel(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
     InfoWidget iw = (InfoWidget)client_data;
     XawListReturnStruct *rs = (XawListReturnStruct *)call_data;
     
     if (getNode(iw, NULL, trueName(iw, CURNODE(iw)->xref, rs->string),
		 NULL) == FALSE)
	  message(iw, "?Can't find node for cross reference '%s'", rs->string);
}

/*****************************************************************************
 * Xlib and toolkit utility functions.                                       *
 *****************************************************************************/

/* Clear the argument text */
Local void clear_arg(iw)
InfoWidget iw;
{
     XawTextBlock blk;
     
     SET_BLOCK(blk, 0, 0, "");
     XawTextReplace(iw->info.argText, 0, strlen(iw->info.arg), &blk);
}

/* Put up a dialog to get necessary information */
Local void dialog(iw, msg, callback)
InfoWidget iw;
String msg;
void (*callback)();
{
     Arg args[10];
     Cardinal i;
     int x, y;
     Widget dg, abort, confirm;
     
     /*
      * We create the dialog everytime (rather than just once, followed
      * by Popup/Popdown requests) so that it will be made the proper size
      * for the label each time. Can't seem to get it to resize dynamically,
      * so I don't see any other way.
      */
     iw->info.requester = callback;
     
     /* Don't see any other way of doing this. It seems there should be. */
     getXY(iw, &x, &y);
     
     /* Position nicely */
     i = 0;
     XtSetArg(args[i], XtNx, x - 30 > 0 ? x - 30 : 0);	i++;
     XtSetArg(args[i], XtNy, y - 30 > 0 ? y - 30 : 0);	i++;
     XtSetArg(args[i], XtNallowShellResize, TRUE);	i++;
     iw->info.argPopup = XtCreatePopupShell("need_argument",
					    transientShellWidgetClass,
					    iw, args, i);
     i = 0;
     XtSetArg(args[i], XtNvalue, iw->info.arg);		i++;
     XtSetArg(args[i], XtNlabel, msg);			i++;
     dg = XtCreateManagedWidget("dialog", dialogWidgetClass,
				iw->info.argPopup, args, i);
     
     i = 0;
     abort = XtCreateManagedWidget("abort", commandWidgetClass,
				   dg, args, i);
     XtAddCallback(abort, XtNcallback, do_dialog_abort, iw);
     
     i = 0;
     confirm = XtCreateManagedWidget("confirm", commandWidgetClass,
				     dg, args, i);
     XtAddCallback(confirm, XtNcallback, do_dialog_confirm, iw);
     
     XtPopup(iw->info.argPopup, XtGrabExclusive);
}

/* Toot the horn */
Local void feep(iw)
InfoWidget iw;
{
     XBell(XtDisplay(iw), iw->info.bell_volume);
}

/* Find the info widget in a hierarchy */
Local Inline InfoWidget find_top(w)
Widget w;
{
     register Widget tmp = w;
     
     while (tmp) {
	  if (XtClass(tmp) == infoWidgetClass)
	       return (InfoWidget)tmp;
	  else
	       tmp = XtParent(tmp);
     }
     if (!tmp)
	  XtError("Walked off end of widget hierarchy!");
     return (InfoWidget)NULL;
}

/* Return the arg contents if set, else NULL */
Local String get_arg(w)
InfoWidget w;
{
     if (strlen(w->info.arg))
	  return w->info.arg;
     else
	  return NULL;
}

/* Return the root XY coords of the pointer */
Local void getXY(w, xp, yp)
Widget w;
int *xp, *yp;
{
     Window junkr, junkc;
     int junkx, junky;
     unsigned int mask;
     
     (void) XQueryPointer(XtDisplay(w), XtWindow(w), &junkr, &junkc,
			  xp, yp, &junkx, &junky, &mask);
}

/*****************************************************************************
 * Unix and string utility functions.                                        *
 *****************************************************************************/

/* Search for a file along a path, returning the complete path name if found */
Local String find_file(path, name)
String path, name;
{
     String cp = path;
     Boolean more_path = TRUE;
     Local char dir[MAXPATHLEN];
     int status = -1;
     String name_start, z_start;
     
     dir[0] = '\0';
     
     /* absolute path name? */
     if (name[0] == '/') {
	  if (!access(name, R_OK))
	       return name;
	  else
	       name = file_name(name);
     }
     while (status && more_path) {
          if ((cp = index(path, ':')) != NULL) {
               strncpy(dir, path, cp - path);
	       dir[cp - path] = '\0';
               strcat(dir, "/");
               path = cp + 1;
          }
          else {
               strcpy(dir, path);
               strcat(dir, "/");
               more_path = FALSE;
          } 
	  name_start = dir+strlen(dir);
	  strcat(dir, name);
          status = access(dir, R_OK);
#ifdef UNCOMPRESS
	  if (status) {
	       z_start = dir+strlen(dir);
	       strcat(dir,".Z");
	       status = access(dir, R_OK);
	  }
#endif
	  /* if we failed, try again in lower case */
	  if (status) {
	       downcase(name_start);
#ifdef UNCOMPRESS
	       *z_start = '\0';
#endif
	       status = access(dir, R_OK);
#ifdef UNCOMPRESS
	       if (status) {
		    strcat(dir,".Z");
		    status = access(dir, R_OK);
	       }
#endif
	  }
     }
     if (dir[0])
          return dir;
     else
          return NULL;
}

/* return the file part of a path name */
Local Inline String file_name(s)
register String s;
{
     register int i = strlen(s);
     
     while (i) {
	  if (s[i - 1] == '/')
	       return s + i;
	  i--;
     }
     return s;
}

/* strip evil tab/formfeed/newline chars from a string (replacing w/blanks) */
Local Inline String normalize_whitespace(s)
String s;
{
     register String tmp;
     
     if (tmp = s) {
	  while (*tmp) {
	       if (isspace(*tmp))
		    *tmp = ' ';
	       ++tmp;
	  }
     }
     return s;
}

/* Convert from an offset ID to a string. */
Local Inline String offsetToString(iw, blk)
InfoWidget iw;
ID blk;
{
     Local char ret[MAXSTR];
     
     if (I_LEN(blk) != 0) {
	  strncpy(ret, START(iw) + I_START(blk), I_LEN(blk));
	  ret[I_LEN(blk)] = '\0';
	  return normalize_whitespace(ret);
     }
     else
	  return NULL;
}

/* chew through white space */
Local Inline String eat_whitespace(s)
register String s;
{
     while (*s && isspace(*s))
	  s++;
     return s;
}

/* look up the actual name of a list item */
Local String trueName(iw, lst, name)
InfoWidget iw;
IDList lst;
String name;
{
     register int i;
     
     for (i = 0; i < lst.t.idx; i++)
	  if (!strcomp(lst.l[i], name))
	       return offsetToString(iw, lst.t.table[i]);
     return NULL;
}

/* Search for a string */
Local String search(iw, start, end, str, igncase)
InfoWidget iw;
register String start, end, str;
Boolean igncase;
{
     register String ind = str;
     register String stop = str + strlen(str);
     register int comp;
     
     while (start < end) {
	  if (!igncase)
	       comp = (*start == *ind);
	  else
	       comp = (TOLOWER(*start) == TOLOWER(*ind));
	  if (!comp) {
	       if (ind != str)
		    ind = str;
	       else
		    start++;
	  }
	  else {
	       if (++start <= end && ++ind == stop)
		    return start;
	  }
     }
     return NULL;
}

/* Like search(), but in the reverse direction */
Local String search_back(iw, start, end, str, igncase)
InfoWidget iw;
register String start, end, str;
Boolean igncase;
{
     register String ind;
     register String stop;
     register int comp;
     
     ind = str = reverse(str);
     stop = ind + strlen(ind);
     
     while (start > end) {
	  if (!igncase)
	       comp = (*start == *ind);
	  else
	       comp = (TOLOWER(*start) == TOLOWER(*ind));
	  if (!comp) {
	       if (ind != str)
		    ind = str;
	       else
		    start--;
	  }
	  else {
	       start--;
	       if (++ind == stop)
		    return start;
	  }
     }
     return NULL;
}

/*
 * Safe and sane strcmp. Deals with null pointer for either arg and ignores
 * case. All whitespace is considered equivalent.
 */
Local Inline int strcomp(s1, s2)
register String s1, s2;
{
     if (s1 && s2) {
	  if (strlen(s1) != strlen(s2))
	       return -1;
	  
	  while (*s1 && *s2 && (TOLOWER(*s1) == TOLOWER(*s2)))
	       ++s1, ++s2;
	  if (!*s1 && !*s2)
	       return 0;
	  else if (*s1 < *s2)
	       return -1;
	  else
	       return 1;
     }
     else if (!s1 && !s2)
          return 0;
     else if (!s1 && s2)
          return -1;
     else
          return 1;
}

/* like above, but stops after n characters */
Local Inline int strncomp(s1, s2, n)
register String s1, s2;
int n;
{
     register String s3 = s2 + n;
     
     if (s1 && s2) {
	  while (s2 < s3 && *s1 && *s2 && (TOLOWER(*s1) == TOLOWER(*s2)))
	       ++s1, ++s2;
	  if (!*s1 && !*s2 || s2 == s3)
	       return 0;
	  else if (*s1 < *s2)
	       return -1;
	  else
	       return 1;
     }
     else if (!s1 && !s2)
          return 0;
     else if (!s1 && s2)
          return -1;
     else
          return 1;
}

/* Copy s2 to s1 up to (but not including) character c */
Local Inline void strccpy(s1, s2, c)
register String s1, s2;
register char c;
{
     while (*s2 && *s2 != c)
	  *(s1++) = *(s2++);
     *s1 = '\0';
}

/*
 * Return integer subscript of character 'c' in string 's'.
 * (why doesn't this already exist in a library somewhere?).
 */
Local Inline int iindex(s, c)
register char *s, c;
{
     register char *cp;
     
     if (!s)
          return -1;
     cp = index(s, c);
     if (cp)
          return cp - s;
     else
          return -1;
}

Local String substr(s, p1, p2)
register String s;
register int p1, p2;
{
     Local char ret[MAXSTR];
     register int i = 0;
     
     if (p1 > p2) {
	  sprintf(ret, "substr: start %d, end %d. start must be <= end",
		  p1, p2);
	  XtWarning(ret);
          return NULL;
     }
     if (p2 - p1 > MAXSTR) {
          sprintf(ret, "substr: end - start is > max len of %d", MAXSTR);
	  XtWarning(ret);
          return NULL;
     }
     while (p1 <= p2)
          ret[i++] = s[p1++];
     ret[i] = '\0';
     return ret;
}

/*
 * Safely concatenate two strings into static area, returning pointer to
 * result.
 */
Local String strconcat(s1, s2)
register String s1, s2;
{
     Local char ret[MAXSTR];
     int len1;
     
     if (s1) {
	  if ((len1 = strlen(s1)) >= MAXSTR) {
	       sprintf(ret, "strconcat: length of s1 > MAX (%d)", MAXSTR);
	       XtWarning(ret);
	       return NULL;
	  }
	  else
	       strcpy(ret, s1);
	  if (s2) {
	       if (len1 + strlen(s2) > MAXSTR) {
		    sprintf(ret, "strconcat: length of s1 + s2 is > MAX (%d)",
			    MAXSTR);
		    XtWarning(ret);
	       }
	       else
		    strcat(ret, s2);
	  }
	  return ret;
     }
     else
	  return NULL;
}

/* reverse a string so that a simple reverse search may be done on it */
Local String reverse(s)
register String s;
{
     Local char ret[MAXSTR];
     register int i, len;
     
     if ((len = strlen(s)) > MAXSTR) {
	  sprintf(ret, "reverse: string too long to reverse. MAX is %d",
		  MAXSTR);
	  XtWarning(ret);
	  return NULL;
     }
     else {
	  i = 0;
	  while (len)
	       ret[i++] = s[--len];
	  ret[i] = '\0';
	  return ret;
     }
}

/* convert a string to lower case */
Local Inline String downcase(s)
register String s;
{
     String orig = s;
     
     if (s)
	  while (*s) {
	       *s = TOLOWER(*s);
	       s++;
	  }
     return orig;
}

#ifdef BSD
/* BSD users don't have strpbrk() */
/* Routines borrowed from PD libc written by Richard A. O'Keefe. */

#if     CharsAreSigned
#define MaxPosChar      127
#else  ~CharsAreSigned
#define MaxPosChar      255
#endif  CharsAreSigned
#ifndef _AlphabetSize
#define _AlphabetSize   128
#endif

static int  _set_ctr = MaxPosChar;
static char _set_vec[_AlphabetSize];

void _str2set(set)
register String set;
{
     if (set == NULL)
	  return;
     if (++_set_ctr == MaxPosChar+1) {
	  register char *w = &_set_vec[_AlphabetSize];
	  do
	       *--w = '\0';
	  while (w != &_set_vec[0]);
          _set_ctr = 1;
     }
     while (*set)
	  _set_vec[*set++] = _set_ctr;
}

String strpbrk(s1, s2)
register String s1, s2;
{
     _str2set(set);
     while (_set_vec[*str] != _set_ctr)
          if (!*str++)
	       return NULL;
     return str;
}
#endif /* BSD */
