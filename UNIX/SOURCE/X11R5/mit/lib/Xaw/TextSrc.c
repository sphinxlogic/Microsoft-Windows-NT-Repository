/* $XFree86: mit/lib/Xaw/TextSrc.c,v 1.3 1993/03/27 09:10:52 dawes Exp $ */
/* $XConsortium: TextSrc.c,v 1.11 91/02/20 17:58:08 converse Exp $ */

/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
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
 * Author:  Chris Peterson, MIT X Consortium.
 *
 * Much code taken from X11R3 String and Disk Sources.
 */

/*
 * TextSrc.c - TextSrc object. (For use with the text widget).
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/TextSrcP.h>
#include <X11/Xmu/CharSet.h>

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffsetOf(TextSrcRec, textSrc.field)
static XtResource resources[] = {
    {XtNeditType, XtCEditType, XtREditMode, sizeof(XawTextEditType), 
        offset(edit_mode), XtRString, "read"},
};

static void ClassInitialize(), ClassPartInitialize(), SetSelection();
static void CvtStringToEditMode();
static Boolean ConvertSelection();
static XawTextPosition Search(), Scan(), Read();
static int Replace();

#ifndef SVR3SHLIB
#define SuperClass		(&objectClassRec)
#else
#define SuperClass		NULL
#endif
TextSrcClassRec textSrcClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) SuperClass,
    /* class_name	  	*/	"TextSrc",
    /* widget_size	  	*/	sizeof(TextSrcRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	NULL,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	NULL,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	FALSE,
    /* compress_exposure  	*/	FALSE,
    /* compress_enterleave	*/	FALSE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	NULL,
    /* resize		  	*/	NULL,
    /* expose		  	*/	NULL,
    /* set_values	  	*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	NULL,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	NULL,
    /* extension		*/	NULL
  },
/* textSrc_class fields */
  {
    /* Read                     */      Read,
    /* Replace                  */      Replace,
    /* Scan                     */      Scan,
    /* Search                   */      Search,
    /* SetSelection             */      SetSelection,
    /* ConvertSelection         */      ConvertSelection
  }
};

#ifndef SVR3SHLIB
WidgetClass textSrcObjectClass = (WidgetClass)&textSrcClassRec;
#else
extern WidgetClass textSrcObjectClass;
#endif

static void 
ClassInitialize ()
{
    XawInitializeWidgetSet ();
    XtAddConverter(XtRString, XtREditMode,   CvtStringToEditMode,   NULL, 0);
}


static void
ClassPartInitialize(wc)
WidgetClass wc;
{
  register TextSrcObjectClass t_src, superC;

  t_src = (TextSrcObjectClass) wc;
  superC = (TextSrcObjectClass) t_src->object_class.superclass;

/* 
 * We don't need to check for null super since we'll get to TextSrc
 * eventually.
 */

    if (t_src->textSrc_class.Read == XtInheritRead) 
      t_src->textSrc_class.Read = superC->textSrc_class.Read;

    if (t_src->textSrc_class.Replace == XtInheritReplace) 
      t_src->textSrc_class.Replace = superC->textSrc_class.Replace;

    if (t_src->textSrc_class.Scan == XtInheritScan) 
      t_src->textSrc_class.Scan = superC->textSrc_class.Scan;

    if (t_src->textSrc_class.Search == XtInheritSearch) 
      t_src->textSrc_class.Search = superC->textSrc_class.Search;

    if (t_src->textSrc_class.SetSelection == XtInheritSetSelection) 
      t_src->textSrc_class.SetSelection = superC->textSrc_class.SetSelection;

    if (t_src->textSrc_class.ConvertSelection == XtInheritConvertSelection) 
      t_src->textSrc_class.ConvertSelection =
	                               superC->textSrc_class.ConvertSelection;
}

/************************************************************
 *
 * Class specific methods.
 *
 ************************************************************/

/*	Function Name: Read
 *	Description: This function reads the source.
 *	Arguments: w - the TextSrc Object.
 *                 pos - position of the text to retreive.
 * RETURNED        text - text block that will contain returned text.
 *                 length - maximum number of characters to read.
 *	Returns: The number of characters read into the buffer.
 */

/* ARGSUSED */
static XawTextPosition
Read(w, pos, text, length)
Widget w;
XawTextPosition pos;
XawTextBlock *text;	
int length;		
{
  XtAppError(XtWidgetToApplicationContext(w), 
	     "TextSrc Object: No read function is defined.");
}

/*	Function Name: Replace.
 *	Description: Replaces a block of text with new text.
 *	Arguments: src - the Text Source Object.
 *                 startPos, endPos - ends of text that will be removed.
 *                 text - new text to be inserted into buffer at startPos.
 *	Returns: XawEditError.
 */

/*ARGSUSED*/
static int 
Replace (w, startPos, endPos, text)
Widget w;
XawTextPosition startPos, endPos;
XawTextBlock *text;
{
  return(XawEditError);
}

/*	Function Name: Scan
 *	Description: Scans the text source for the number and type
 *                   of item specified.
 *	Arguments: w - the TextSrc Object.
 *                 position - the position to start scanning.
 *                 type - type of thing to scan for.
 *                 dir - direction to scan.
 *                 count - which occurance if this thing to search for.
 *                 include - whether or not to include the character found in
 *                           the position that is returned. 
 *	Returns: EXITS WITH AN ERROR MESSAGE.
 *
 */

/* ARGSUSED */
static 
XawTextPosition 
Scan (w, position, type, dir, count, include)
Widget                w;
XawTextPosition       position;
XawTextScanType       type;
XawTextScanDirection  dir;
int     	      count;
Boolean	              include;
{
  XtAppError(XtWidgetToApplicationContext(w), 
	     "TextSrc Object: No SCAN function is defined.");
}

/*	Function Name: Search
 *	Description: Searchs the text source for the text block passed
 *	Arguments: w - the TextSource Object.
 *                 position - the position to start scanning.
 *                 dir - direction to scan.
 *                 text - the text block to search for.
 *	Returns: XawTextSearchError.
 */

/* ARGSUSED */
static XawTextPosition 
Search(w, position, dir, text)
Widget                w;
XawTextPosition       position;
XawTextScanDirection  dir;
XawTextBlock *        text;
{
  return(XawTextSearchError);
}

/*	Function Name: ConvertSelection
 *	Description: Dummy selection converter.
 *	Arguments: w - the TextSrc object.
 *                 selection - the current selection atom.
 *                 target    - the current target atom.
 *                 type      - the type to conver the selection to.
 * RETURNED        value, length - the return value that has been converted.
 * RETURNED        format    - the format of the returned value.
 *	Returns: TRUE if the selection has been converted.
 *
 */

/* ARGSUSED */
static Boolean
ConvertSelection(w, selection, target, type, value, length, format)
Widget w;
Atom * selection, * target, * type;
XtPointer * value;
unsigned long * length;
int * format;
{
  return(FALSE);
}

/*	Function Name: SetSelection
 *	Description: allows special setting of the selection.
 *	Arguments: w - the TextSrc object.
 *                 left, right - bounds of the selection.
 *                 selection - the selection atom.
 *	Returns: none
 */

/* ARGSUSED */
static void
SetSelection(w, left, right, selection)
Widget w;
XawTextPosition left, right;
Atom selection;
{
  /* This space intentionally left blank. */
}


#define done(address, type) \
        { toVal->size = sizeof(type); toVal->addr = (caddr_t) address; }

/* ARGSUSED */
static void 
CvtStringToEditMode(args, num_args, fromVal, toVal)
XrmValuePtr args;		/* unused */
Cardinal	*num_args;	/* unused */
XrmValuePtr	fromVal;
XrmValuePtr	toVal;
{
  static XawTextEditType editType;
  static  XrmQuark  QRead, QAppend, QEdit;
  XrmQuark    q;
  char        lowerName[BUFSIZ];
  static Boolean inited = FALSE;
    
  if ( !inited ) {
    QRead   = XrmPermStringToQuark(XtEtextRead);
    QAppend = XrmPermStringToQuark(XtEtextAppend);
    QEdit   = XrmPermStringToQuark(XtEtextEdit);
    inited = TRUE;
  }

  XmuCopyISOLatin1Lowered (lowerName, (char *)fromVal->addr);
  q = XrmStringToQuark(lowerName);

  if       (q == QRead)          editType = XawtextRead;
  else if (q == QAppend)         editType = XawtextAppend;
  else if (q == QEdit)           editType = XawtextEdit;
  else {
    done(NULL, 0);
    return;
  }
  done(&editType, XawTextEditType);
  return;
}



/************************************************************
 *
 * Public Functions.
 *
 ************************************************************/

/*	Function Name: XawTextSourceRead
 *	Description: This function reads the source.
 *	Arguments: w - the TextSrc Object.
 *                 pos - position of the text to retreive.
 * RETURNED        text - text block that will contain returned text.
 *                 length - maximum number of characters to read.
 *	Returns: The number of characters read into the buffer.
 */

XawTextPosition
#if NeedFunctionPrototypes
XawTextSourceRead(Widget w, XawTextPosition pos, XawTextBlock *text,
		  int length)
#else
XawTextSourceRead(w, pos, text, length)
Widget w;
XawTextPosition pos;
XawTextBlock *text;	
int length;
#endif
{
  TextSrcObjectClass class = (TextSrcObjectClass) w->core.widget_class;

  return((*class->textSrc_class.Read)(w, pos, text, length));
}

/*	Function Name: XawTextSourceReplace.
 *	Description: Replaces a block of text with new text.
 *	Arguments: src - the Text Source Object.
 *                 startPos, endPos - ends of text that will be removed.
 *                 text - new text to be inserted into buffer at startPos.
 *	Returns: XawEditError or XawEditDone.
 */

/*ARGSUSED*/
int
#if NeedFunctionPrototypes
XawTextSourceReplace (Widget w, XawTextPosition startPos, 
		      XawTextPosition endPos, XawTextBlock *text)
#else
XawTextSourceReplace (w, startPos, endPos, text)
Widget w;
XawTextPosition startPos, endPos;
XawTextBlock *text;
#endif
{
  TextSrcObjectClass class = (TextSrcObjectClass) w->core.widget_class;

  return((*class->textSrc_class.Replace)(w, startPos, endPos, text));
}

/*	Function Name: XawTextSourceScan
 *	Description: Scans the text source for the number and type
 *                   of item specified.
 *	Arguments: w - the TextSrc Object.
 *                 position - the position to start scanning.
 *                 type - type of thing to scan for.
 *                 dir - direction to scan.
 *                 count - which occurance if this thing to search for.
 *                 include - whether or not to include the character found in
 *                           the position that is returned. 
 *	Returns: The position of the text.
 *
 */

XawTextPosition
#if NeedFunctionPrototypes
XawTextSourceScan(Widget w, XawTextPosition position,
#if NeedWidePrototypes
		  int type, int dir,
#else
		  XawTextScanType type, XawTextScanDirection dir,
#endif
		  int count,
#if NeedWidePrototypes
		  int include)
#else
		  Boolean include)
#endif
#else
XawTextSourceScan(w, position, type, dir, count, include)
Widget                w;
XawTextPosition       position;
XawTextScanType       type;
XawTextScanDirection  dir;
int     	      count;
Boolean	              include;
#endif
{
  TextSrcObjectClass class = (TextSrcObjectClass) w->core.widget_class;

  return((*class->textSrc_class.Scan)(w, position, type, dir, count, include));
}

/*	Function Name: XawTextSourceSearch
 *	Description: Searchs the text source for the text block passed
 *	Arguments: w - the TextSource Object.
 *                 position - the position to start scanning.
 *                 dir - direction to scan.
 *                 text - the text block to search for.
 *	Returns: The position of the text we are searching for or
 *               XawTextSearchError.
 */

XawTextPosition 
#if NeedFunctionPrototypes
XawTextSourceSearch(Widget w, XawTextPosition position,
#if NeedWidePrototypes
		    int dir,
#else
		    XawTextScanDirection dir,
#endif
		    XawTextBlock *text)
#else
XawTextSourceSearch(w, position, dir, text)
Widget                w;
XawTextPosition       position;
XawTextScanDirection  dir;
XawTextBlock *        text;
#endif
{
  TextSrcObjectClass class = (TextSrcObjectClass) w->core.widget_class;

  return((*class->textSrc_class.Search)(w, position, dir, text));
}

/*	Function Name: XawTextSourceConvertSelection
 *	Description: Dummy selection converter.
 *	Arguments: w - the TextSrc object.
 *                 selection - the current selection atom.
 *                 target    - the current target atom.
 *                 type      - the type to conver the selection to.
 * RETURNED        value, length - the return value that has been converted.
 * RETURNED        format    - the format of the returned value.
 *	Returns: TRUE if the selection has been converted.
 *
 */

Boolean
#if NeedFunctionPrototypes
XawTextSourceConvertSelection(Widget w, Atom *selection, Atom *target, 
			      Atom *type, XtPointer *value,
			      unsigned long *length, int *format)
#else
XawTextSourceConvertSelection(w, selection, 
			      target, type, value, length, format)
Widget w;
Atom * selection, * target, * type;
XtPointer * value;
unsigned long * length;
int * format;
#endif
{
  TextSrcObjectClass class = (TextSrcObjectClass) w->core.widget_class;

  return((*class->textSrc_class.ConvertSelection)(w, selection, target, type,
						  value, length, format));
}

/*	Function Name: XawTextSourceSetSelection
 *	Description: allows special setting of the selection.
 *	Arguments: w - the TextSrc object.
 *                 left, right - bounds of the selection.
 *                 selection - the selection atom.
 *	Returns: none
 */

void
#if NeedFunctionPrototypes
XawTextSourceSetSelection(Widget w, XawTextPosition left, 
			  XawTextPosition right, Atom selection)
#else
XawTextSourceSetSelection(w, left, right, selection)
Widget w;
XawTextPosition left, right;
Atom selection;
#endif
{
  TextSrcObjectClass class = (TextSrcObjectClass) w->core.widget_class;

  (*class->textSrc_class.SetSelection)(w, left, right, selection);
}


