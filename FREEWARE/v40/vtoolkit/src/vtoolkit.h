/* vtoolkit.h - VToolkit common include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module assists in the portability of the VToolkit widgets across toolkits.
It currently supports the XUI toolkit (the default) and the MOTIF toolkit.

MODIFICATION HISTORY:

14-Sep-1993 (sjk) Add prototypes for VtkAddDeleteWindowCallback and VtkFlushUntilDone.

14-Sep-1993 (sjk) Changed VtkGetOneValue to return XtArgVal rather than Opaque.

09-Sep-1993 (sjk/nfc) Added the condintional definition for VMS to keep Motif 
	handy when compiling with DECC using /STAND=ANSI89

28-Aug-1993 (sjk/nfc) Run through Motif porting tools and added the global function
	prototypes for vtktutils.c

19-Mar-1992 (sjk) Conditionalize #pragma for VMS-only.

03-Mar-1992 (sjk) Add VtkIsWidget macro.

31-Jan-1992 (sjk) Add XtIsWidget XUI equivalent.

08-Nov-1991 (sjk) Add VtkDisplay, VtkWindow and VtkParent macros.

06-Nov-1991 (sjk) Misc changes.

24-Oct-1991 (sjk) Fix DwtFontRec synonym for latest MOTIF version.

27-Sep-1991 (sjk) #pragma nostandard to avoid compiler info messages on VAX.

16-Jul-1991 (sjk) Add more MOTIF synonyms.

17-May-1991 (sjk) Integrate Ken Burechailo's MOTIF updates.  Thanks Ken!

03-Apr-1991 (sjk) Upgrade MOTIF synonym for DwtSFontDefault to MOTIF V1.0.

17-May-1990 (sjk) Add MOTIF synonym for XmNuserData.

10-May-1990 (sjk) Changes for MOTIF BL1.1.

20-Feb-1990 (sjk) Fix capitalization to get ULTRIX version to build.

31-Jan-1990 (sjk) Added MOTIF synonym for XmHORIZONTAL, XmVERTICAL, and xmScrollBarWidgetClass.

24-Jan-1990 (sjk) Added MOTIF synonym for DwtLatin1String.

19-Jan-1990 (sjk) Initial entry.
*/

#ifndef _VToolkit_h
#define _VToolkit_h

#include <Mrm/MrmAppl.h>

#ifdef VAXC
#define external globalref
#else
#define external extern
#endif

/* For maximum portability, use these macros. */

#define VtkIsRealized(w)	((w)->core.window != 0)
#define VtkDisplay(w)           ((w)->core.screen->display)
#define VtkWindow(w)            ((w)->core.window)
#define VtkParent(w)            ((w)->core.parent)
#define VtkIsWidget(w)		XtIsWidget (w)
#define VtkSetValues		XtVaSetValues

/* global functions for vtkutil.c */

void VtkActivatePopupMenu(Widget, XEvent*);
void VtkChangeRadioResource(Widget, int*, XmToggleButtonCallbackStruct*, char*);
int VtkComputeTextWidth(XFontStruct*, char*);
int VtkCopyColorNames(Widget, char**, Pixel, Pixel**);
int VtkCopyColors(Widget, Pixmap, Pixel**);
void VtkGetCharWidthAndHeight(XFontStruct**, int, int*, int*);
Widget VtkGetChildByClass(Widget, WidgetClass);
XmFontList VtkGetDefaultFontList(Widget);
char *VtkGetDefaultsName(Widget);
void VtkGetDrawableWidthAndHeight(Display*, Pixmap, int*, int*);
XFontStruct *VtkGetFirstFont(struct _XmFontListRec*);
void VtkGetFontA(struct _XmFontListRec*, XFontStruct***, int*);
int VtkGetIntResource(XrmDatabase, char*, int);
XtArgVal VtkGetOneValue(Widget, char*);
Pixmap VtkGetStippleBitmap(Widget);
char *VtkGetStringResource(XrmDatabase, char*);
Window VtkGetSubwindow(Display*, Window, int, int, Window, int*, int*);
Widget VtkFetchWidget(MrmHierarchy, char*, Widget, Widget*, Widget*);
int VtkInitialize(char*, char*, int*, char**, char*, MrmRegisterArg*, int, MrmHierarchy*, Widget*);
void VtkMinMax(int, int, int*, int*);
void VtkNullRoutine(void);
XtGeometryResult VtkPermissiveGeometryManager(Widget, XtWidgetGeometry*, XtWidgetGeometry*);
char *VtkRecloneString(char*, char*);
void VtkSaveNumericResource(XrmDatabase*, char*, int);
void VtkSaveNumericResourceN(XrmDatabase*, Widget, char*);
void VtkSaveStringResource(XrmDatabase*, char*, char*);
void VtkSetFieldsToZero(char*, char*);
void VtkSetIdentifierValue(char*, Opaque);
void VtkUnitTypeDefault(Widget, int, XrmValue*);
char *VtkWidgetResourceToName(Widget, char*);
void VtkAddDeleteWindowCallback(Widget w, void (*callbackP)(), Opaque closure);
void VtkFlushUntilDone(Display *dpy);

#endif /* _VToolkit_h */
