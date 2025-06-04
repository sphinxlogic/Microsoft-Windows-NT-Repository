#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/xrn.c,v 1.7 1993/02/04 18:22:42 ricks Exp $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * xrn.c: set up the main screens
 *
 */

#include "copyright.h"
#include "config.h"
#include <stdio.h>
#include "utils.h"
#ifndef VMS
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Shell.h>
#else
#include <decw$include/Intrinsic.h>
#include <decw$include/Xutil.h>
#include <decw$include/StringDefs.h>
#include <decw$include/Xatom.h>
#include <decw$include/Shell.h>
#endif
#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/PanedW.h>
#include <Xm/Label.h>
#include <Xm/TextP.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/ScrolledW.h>
#if (XmVERSION == 1) && (XmREVISION == 0)
#define MOTIF_V10
#endif
#else
#ifndef VMS
#include <X11/DECwDwtWidgetProg.h>
#include "Pane.h"
#else
#include <decw$include/DECwDwtWidgetProg.h>
#include "Pane.h"
#endif	/* VMS */
#endif	/* MOTIF */

#include "slist.h"
#include "xthelper.h"
#include "xmisc.h"
#include "resources.h"
#include "news.h"
#include "internals.h"
#include "error_hnds.h"
#include "buttons.h"
#include "xrn.h"

#ifdef __STDC__
#include <stdlib.h>
#endif

void PaneSetMinMax();

#ifndef MOTIF
#ifndef DwtNmenuExtendLastRow
#define DwtNmenuExtendLastRow "menuExtendLastRow"
#endif
#endif

#ifdef XFILESEARCHPATH
static void AddPathToSearchPath();
#endif

extern void makeMenus();
extern void makeArtArtMenus();
extern void makePopup();
extern void makeArtPopup();
extern XtCallbackProc artQuitFunction();


/* global variables that represent the widgets that are dynamically changed */

Widget TopLevel;
Widget MainWindow;	/* Main Window				     */
Widget ArtTopLevel;	/* Article Shell       			     */
Widget ArtMainWindow;	/* Article Window			     */
Widget ArtWindow;	/* Article pane				     */
Widget Frame;		/* Pane widget 				     */
Widget TopButtonBox;	/* button box containing the command buttons */
Widget BottomButtonBox;	/* button box containing the article buttons */
Widget TopInfoLine;	/* top button info line                      */
Widget BottomInfoLine;	/* bottom button info line                   */
Widget TopList;		/* newsgroup and article subject display     */
Widget ScrollWindow;	/* Scrolled window for top list		     */
Widget ArticleText;	/* article display                           */
Widget PopupMenu;	/* the popup menus 			     */
Widget MenuBar;		/* the menu bar				     */
Widget ArtMenuBar;	/* the menu bar				     */
#ifdef MOTIF
Widget TopShadowFrame;	/* Frame around list Widget		     */
Widget BotShadowFrame;	/* Frame around text Widget		     */
#endif
#ifdef TITLEBAR
Widget TitleBar;	/* The titlebar				     */
#endif
int XRNState;		/* XRN status: news and x                    */
static Boolean oneTime = True;	/* Set true if resize tried	     */

static Arg frameArgs[] = {			/* main window description */
						/* args set from geometry and */
	{XtNx,			(XtArgVal) 10},	/* must stay in x/y/w/h order */
	{XtNy, 			(XtArgVal) 10},
#ifdef MOTIF
	{XtNwidth, 		(XtArgVal) 10},
	{XtNheight, 		(XtArgVal) 10},
#else
	{DwtNwidth,		(XtArgVal) 600},
	{DwtNheight,		(XtArgVal) 700},
#endif
};

/*ARGSUSED*/
main(argc, argv)
int argc;
char **argv;
{
    int i;
    Arg resizeArg[3];
    Dimension width, height, borderWidth;
    extern char title[];
    static Arg fontArgs[2];
    static Arg labelArgs[1];
    static Dimension fontHeight, fontWidth, listHeight;
    static Dimension paneHeight = 0, paneWidth;
#ifdef MOTIF
    static XmString labelString;
#else
    static DwtCompString labelString;
#endif

#ifndef MOTIF
    static Arg mainArgs[] = {
	{DwtNacceptFocus,	(XtArgVal) True},
	{XtNsensitive,		(XtArgVal) True},
    };
#endif
    static Arg infoArgs[] = {
	{XtNheight,		(XtArgVal) 40},
#ifdef MOTIF
	{XmNallowResize,	(XtArgVal) True},
	{XmNskipAdjust,		(XtArgVal) False},
#endif
    };
#ifndef MOTIF
    static Arg menuArgs[] = {
	{DwtNx,		    0},
	{DwtNy,		    0},
	{DwtNspacing,	    20},
	{DwtNorientation,   DwtOrientationHorizontal},
    };
#endif

    static Arg topBoxArgs[] = {
#ifdef MOTIF
	{XmNresizeHeight,	(XtArgVal) True},
	{XmNresizeWidth,	(XtArgVal) False},
	{XmNnumColumns,		(XtArgVal) 3},
	{XmNadjustLast,		(XtArgVal) False},
	{XmNorientation, 	(XtArgVal) XmHORIZONTAL},
	{XmNpacking,		(XtArgVal) XmPACK_TIGHT},
	{XmNheight,		(XtArgVal) 80},
#else
	{DwtNorientation, 	(XtArgVal) DwtOrientationHorizontal},
	{DwtNmenuPacking,	(XtArgVal) DwtMenuPackingTight},
	{DwtNmenuNumColumns,	(XtArgVal) 4},
	{DwtNmenuExtendLastRow, (XtArgVal) False},
	{DwtNborderWidth,	(XtArgVal) 3},
	{DwtNentryBorder,	(XtArgVal) 2},
	{DwtNspacing,		(XtArgVal) 2},
	{DwtNchildOverlap,	(XtArgVal) False},
#endif
    };
    static Arg bottomBoxArgs[] = {
#ifdef MOTIF
	{XmNresizeHeight,	(XtArgVal) False},
	{XmNresizeWidth,	(XtArgVal) True},
	{XmNadjustLast,		(XtArgVal) False},
	{XmNorientation, 	(XtArgVal) XmHORIZONTAL},
	{XmNpacking,		(XtArgVal) XmPACK_TIGHT},
#else
	{DwtNorientation, 	(XtArgVal) DwtOrientationHorizontal},
	{DwtNmenuPacking,	(XtArgVal) DwtMenuPackingTight},
	{DwtNmenuExtendLastRow, (XtArgVal) False},
	{DwtNborderWidth,	(XtArgVal) 3},
	{DwtNentryBorder,	(XtArgVal) 2},
	{DwtNspacing,		(XtArgVal) 2},
	{DwtNchildOverlap,	(XtArgVal) False},
	{DwtNmenuNumColumns,	(XtArgVal) 1},
#endif
   };
    static Arg listArgs[] = {			/* newsgroup/subject list window */
	{SListNrows, 		(XtArgVal) 10},
	{SListNcols,		(XtArgVal) 83},
    };
#ifdef MOTIF
    Arg articleTextArgs[10];
#else
    static Arg articleTextArgs[] = {		/* article/all text window */
	{DwtNrows,		(XtArgVal) 24},
	{DwtNcols,		(XtArgVal) 80},
	{DwtNwordWrap, 		(XtArgVal) TRUE},
	{DwtNscrollVertical,	(XtArgVal) TRUE},
	{DwtNeditable,		(XtArgVal) FALSE},
    };
#endif

#ifdef MOTIF
    static Arg scrollWinArgs[] = {
	{XmNscrollBarDisplayPolicy,	(XtArgVal) XmSTATIC},
	{XmNscrollBarPlacement,		(XtArgVal) XmBOTTOM_RIGHT},
	{XmNscrollingPolicy,		(XtArgVal) XmAPPLICATION_DEFINED},
    };
#else
    static Arg scrollWinArgs[] = {
	{DwtNshownValueAutomaticVert,	(XtArgVal) False},
	{DwtNshownValueAutomaticHoriz,	(XtArgVal) False},
    };
#endif

    static Arg artTopArgs[] = {
	{XtNinput,			(XtArgVal) True},
	{XtNallowShellResize,		(XtArgVal) True},
#ifdef MOTIF
	{XmNiconName,			(XtArgVal) "mxrn article"},
	{XmNtitle,			(XtArgVal) "mxrn Article Text"},
	{XmNtransient,			(XtArgVal) False},
	{XmNdeleteResponse,		(XtArgVal) XmUNMAP},
#else
#ifndef DwtNiconName
#define DwtNiconName "iconName"
#endif
	{DwtNiconName,			(XtArgVal) "dxrn article"},
	{DwtNtitle,			(XtArgVal) "dxrn Article Text"},
#endif
    };

    static Arg shellArgs[] = {
	{XtNallowShellResize,		(XtArgVal) True},
    };

    XRNState = 0;

#ifdef XFILESEARCHPATH
    AddPathToSearchPath(XFILESEARCHPATH);
#endif
    
    TopLevel = XrnInitialize(argc, argv);
    
    ehInstallSignalHandlers();
    ehInstallErrorHandlers();
#ifdef DEBUG
    XSynchronize(XtDisplay(TopLevel), True);
#endif

    if (app_resources.twoWindows) {
	if (app_resources.indexLayout) {
	    xthTranslateGeometry(app_resources.indexLayout, frameArgs);
	    XtSetValues(TopLevel, frameArgs, XtNumber(frameArgs));
	}
    } else {
	if (app_resources.mainLayout) {
	    xthTranslateGeometry(app_resources.mainLayout, frameArgs);
	    XtSetValues(TopLevel, frameArgs, XtNumber(frameArgs));
	}
    }
    
    if (app_resources.twoWindows) {
	XtSetValues(TopLevel, shellArgs, XtNumber(shellArgs));
    }
#ifdef MOTIF
    MainWindow = XmCreateMainWindow(TopLevel, "main", NULL, 0);
#else
    MainWindow = DwtMainWindowCreate(TopLevel, "main",
					mainArgs, XtNumber(mainArgs));
#endif
    XtManageChild(MainWindow);

    if (app_resources.twoWindows) {
	ArtTopLevel = XtCreatePopupShell("articleView",
			topLevelShellWidgetClass, TopLevel,
			artTopArgs, XtNumber(artTopArgs));
	if (app_resources.artLayout) {
	    xthTranslateGeometry(app_resources.artLayout, frameArgs);
	    XtSetValues(ArtTopLevel, frameArgs, XtNumber(frameArgs));
	}
#ifdef MOTIF
	ArtMainWindow = XmCreateMainWindow(ArtTopLevel, "article", NULL, 0);
	XtManageChild(ArtMainWindow);
	ArtWindow = XtCreateManagedWidget("artpane", xmPanedWindowWidgetClass,
			ArtMainWindow, NULL, 0);
#else
	ArtMainWindow = DwtMainWindowCreate(ArtTopLevel, "article", NULL, 0);
	XtManageChild(ArtMainWindow);
	ArtWindow = DwtPaneCreate(ArtMainWindow, "artpane", NULL, 0);
	XtManageChild(ArtWindow);
	DwtPaneAllowResizing(ArtWindow, DwtResizeShrinkWrap);
#endif
    }
    /* create the pane and its widgets */
    
#ifdef MOTIF
    Frame = XtCreateManagedWidget("vpane", xmPanedWindowWidgetClass, MainWindow,
				  frameArgs, XtNumber(frameArgs));
#else
    Frame = (Widget) DwtPaneCreate(MainWindow, "vpane", frameArgs,
					XtNumber(frameArgs));
    XtManageChild(Frame);
    DwtPaneAllowResizing(Frame, DwtResizeShrinkWrap);
#endif
    
#ifdef MOTIF
    MenuBar = XmCreateMenuBar(MainWindow,"MenuBar", NULL, 0);
    XtManageChild(MenuBar);
    makeMenus();
    XmMainWindowSetAreas(MainWindow, MenuBar, NULL, NULL, NULL, Frame);
    if (app_resources.twoWindows) {
	ArtMenuBar = XmCreateMenuBar(ArtMainWindow,"MenuBar", NULL, 0);
	XtManageChild(ArtMenuBar);
	makeArtArtMenus(ArtMenuBar);
	XmMainWindowSetAreas(ArtMainWindow, ArtMenuBar, NULL, NULL, NULL,
				ArtWindow);
    }
#else
    MenuBar = DwtMenuBarCreate(MainWindow, "MenuBar",
	menuArgs, XtNumber(menuArgs));
    XtManageChild(MenuBar);
    makeMenus();
    DwtMainSetAreas(MainWindow, MenuBar, Frame, NULL, NULL, NULL);
    if (app_resources.twoWindows) {
	ArtMenuBar = DwtMenuBarCreate(ArtMainWindow, "MenuBar",
		menuArgs, XtNumber(menuArgs));
	XtManageChild(ArtMenuBar);
	makeArtArtMenus(ArtMenuBar);
	DwtMainSetAreas(ArtMainWindow, ArtMenuBar, ArtWindow, NULL, NULL, NULL);
    }
#endif

#ifdef TITLEBAR
#ifdef MOTIF
    labelString = XmStringLtoRCreate(title, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(labelArgs[0], XmNlabelString, labelString);
    TitleBar = XtCreateManagedWidget("titlebar", xmLabelWidgetClass, Frame,
				     labelArgs, XtNumber(labelArgs));
    XmStringFree(labelString);
    XtSetArg(fontArgs[0], XmNheight, &fontHeight);
    XtGetValues(TitleBar, fontArgs, 1);
    PaneSetMinMax(TitleBar, fontHeight, fontHeight);
    paneHeight = fontHeight * 6;
#else /* MOTIF */
    labelString = DwtLatin1String(title);
    XtSetArg(labelArgs[0], DwtNlabel, labelString);
    TitleBar = DwtLabelCreate(Frame, "titlebar", 
				     labelArgs, XtNumber(labelArgs));
    XtFree(labelString);
    XtManageChild(TitleBar);
    XtSetArg(fontArgs[0], DwtNheight, &fontHeight);
    XtGetValues(TitleBar, fontArgs, 1);
    PaneSetMinMax(TitleBar, fontHeight, fontHeight);
    paneHeight = fontHeight * 6;
#endif /* MOTIF */
#endif /* TITLEBAR */

    listArgs[0].value = (XtArgVal) app_resources.topLines;
#ifdef MOTIF
    TopShadowFrame = XtCreateManagedWidget("topFrame", xmFrameWidgetClass,
					   Frame, NULL, 0);
    ScrollWindow = (Widget) XtCreateManagedWidget("topScroll",
		xmScrolledWindowWidgetClass, TopShadowFrame,
		scrollWinArgs, XtNumber(scrollWinArgs));
#else
    ScrollWindow = (Widget) DwtScrollWindowCreate(Frame, "topScroll",
			    scrollWinArgs, XtNumber(scrollWinArgs));
    XtManageChild(ScrollWindow);
#endif
    TopList = SListCreate(ScrollWindow, "index", listArgs, XtNumber(listArgs));
    XtManageChild(TopList);

#ifdef MOTIF
    fitFrameToContents(TopShadowFrame, TopList);
    XtSetArg(fontArgs[0], XmNheight, &listHeight);
    XtSetArg(fontArgs[1], XmNwidth,  &fontWidth);
    XtGetValues(TopShadowFrame, fontArgs, 2);
    PaneSetMinMax(TopShadowFrame, listHeight+2, listHeight+2);
#else
    XtSetArg(fontArgs[0], DwtNheight, &listHeight);
    XtSetArg(fontArgs[1], DwtNwidth,  &fontWidth);
    XtGetValues(TopList, fontArgs, 2);
    PaneSetMinMax(ScrollWindow, listHeight, listHeight);
#endif
#ifdef TITLEBAR
    paneHeight = paneHeight + listHeight + 6;
    if (fontWidth > paneWidth)
	paneWidth = fontWidth;
#else
    paneHeight = listHeight + 6;
#endif

#ifdef MOTIF
    TopInfoLine = XtCreateManagedWidget("indexinfo", xmLabelWidgetClass, Frame,
					infoArgs, XtNumber(infoArgs));

#ifndef TITLEBAR
    XtSetArg(fontArgs[0], XmNheight, &fontHeight);
    XtGetValues(TopInfoLine, fontArgs, 1);
    fontHeight = fontHeight/2;
#endif
    PaneSetMinMax(TopInfoLine, fontHeight, fontHeight);

    TopButtonBox = XtCreateManagedWidget("indexbuttons",
					  xmRowColumnWidgetClass, Frame,
					  topBoxArgs, XtNumber(topBoxArgs));
    PaneSetMinMax(TopButtonBox, fontHeight*2, fontHeight*2);

    /* article display text window */

    if (app_resources.twoWindows) {
        BotShadowFrame = XtCreateManagedWidget("botFrame", xmFrameWidgetClass,
					   ArtWindow, NULL, 0);
    } else {
        BotShadowFrame = XtCreateManagedWidget("botFrame", xmFrameWidgetClass,
					   Frame, NULL, 0);
    }
    i = 0;
    XtSetArg(articleTextArgs[i], XmNrows, app_resources.textLines);i++;
    XtSetArg(articleTextArgs[i], XmNcolumns, 83);i++;
    XtSetArg(articleTextArgs[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
    XtSetArg(articleTextArgs[i], XmNwordWrap, FALSE);i++;
    XtSetArg(articleTextArgs[i], XmNscrollHorizontal,
	!app_resources.autoWrap);i++;
    XtSetArg(articleTextArgs[i], XmNeditable, FALSE); i++;
    XtSetArg(articleTextArgs[i], XmNcursorPositionVisible, FALSE); i++;
    ArticleText = XmCreateScrolledText(BotShadowFrame, "text", 
	articleTextArgs, i);
    XtManageChild(ArticleText);
    XtSetValues(ArticleText, articleTextArgs, i);

    fitFrameToContents(BotShadowFrame, ArticleText);
    
    if (app_resources.twoWindows) {
	BottomInfoLine = XtCreateManagedWidget("textinfo", xmLabelWidgetClass,
					   ArtWindow, infoArgs,
					   XtNumber(infoArgs));
    } else {
	BottomInfoLine = XtCreateManagedWidget("textinfo", xmLabelWidgetClass,
					   Frame, infoArgs, XtNumber(infoArgs));
    } 
    PaneSetMinMax(BottomInfoLine, fontHeight, fontHeight);
    if (app_resources.twoWindows) {
	BottomButtonBox = XtCreateManagedWidget("textbuttons",
					     xmRowColumnWidgetClass, ArtWindow,
					     bottomBoxArgs,
					     XtNumber(bottomBoxArgs));
    } else {
	BottomButtonBox = XtCreateManagedWidget("textbuttons",
					     xmRowColumnWidgetClass, Frame,
					     bottomBoxArgs,
					     XtNumber(bottomBoxArgs));
    }

    XtSetArg(fontArgs[0], XmNheight, &listHeight);
    XtGetValues(BottomButtonBox, fontArgs, 1);
    PaneSetMinMax(BottomButtonBox, listHeight, listHeight);
#else
    TopInfoLine = DwtLabelCreate(Frame, "indexinfo", 
					infoArgs, XtNumber(infoArgs));

    XtManageChild(TopInfoLine);
#ifndef TITLEBAR
    XtSetArg(fontArgs[0], DwtNheight, &fontHeight);
    XtGetValues(TopInfoLine, fontArgs, 1);
    paneHeight = paneHeight + fontHeight * 3;
    if (fontWidth > paneWidth)
	paneWidth = fontWidth;
#endif
    PaneSetMinMax(TopInfoLine, fontHeight, fontHeight);
    TopButtonBox = DwtMenuBarCreate(Frame, "indexbuttons",
					  topBoxArgs, XtNumber(topBoxArgs));
    XtSetArg(fontArgs[0], DwtNheight, &listHeight);
    XtGetValues(TopButtonBox, fontArgs, 1);
    if (app_resources.twoWindows) {
	paneHeight = paneHeight + listHeight * 3; /* one button box, total 3 lines*/
    } else {
	paneHeight = paneHeight + listHeight * 5; /* two button boxes, total 5 lines*/
    }
    /* article display text window */

    XtManageChild(TopButtonBox);
    articleTextArgs[0].value = app_resources.textLines;
    if (app_resources.twoWindows) {
	ArticleText = DwtSTextCreate(ArtWindow, "text",
		articleTextArgs, XtNumber(articleTextArgs));
    } else {
	ArticleText = DwtSTextCreate(Frame, "text",
		articleTextArgs, XtNumber(articleTextArgs));
    }
    XtSetValues(ArticleText, articleTextArgs, XtNumber(articleTextArgs));
    XtManageChild(ArticleText);
    
    XtSetArg(fontArgs[0], DwtNheight, &listHeight);
    XtSetArg(fontArgs[1], DwtNwidth,  &fontWidth);
    XtGetValues(ArticleText, fontArgs, 2);
    if (app_resources.twoWindows) {
	BottomInfoLine = DwtLabelCreate(ArtWindow, "textinfo", 
					   infoArgs, XtNumber(infoArgs));
    } else {
	paneHeight = paneHeight + listHeight;
	if (fontWidth > paneWidth)
	    paneWidth = fontWidth;
	BottomInfoLine = DwtLabelCreate(Frame, "textinfo", 
					   infoArgs, XtNumber(infoArgs));
    }
    XtManageChild(BottomInfoLine);
    PaneSetMinMax(BottomInfoLine, fontHeight, fontHeight);
    if (app_resources.twoWindows) {
	BottomButtonBox = DwtMenuBarCreate(ArtWindow, "textbuttons",
					     bottomBoxArgs,
					     XtNumber(bottomBoxArgs));
    } else {
	BottomButtonBox = DwtMenuBarCreate(Frame, "textbuttons",
					     bottomBoxArgs,
					     XtNumber(bottomBoxArgs));
    }
    XtManageChild(BottomButtonBox);
    DwtPaneSetMinMax(BottomButtonBox, fontHeight, fontHeight);

    paneWidth += 20;
    paneHeight += 50;
    if (paneHeight > 860) paneHeight = 860;
    XtSetArg(fontArgs[0], DwtNheight, paneHeight);
    XtSetArg(fontArgs[1], DwtNwidth,  paneWidth);
    XtSetValues(Frame, fontArgs, 2);
    XtSetValues(XtParent(Frame), fontArgs, 2);
#endif

    /* initialize the news system, read the newsrc file */
    initializeNews();
    XRNState |= XRN_NEWS_UP;

    /* create the icon */
    xmIconCreate(TopLevel);
    if (app_resources.twoWindows) {
	xmIconCreate(ArtTopLevel);
#ifdef MOTIF
	xmAddWMHandler(ArtTopLevel, (XtCallbackProc)artQuitFunction);
#endif
    }
    /*
     * This next call doesn't do anything by default, unless you
     * modify the application defaults file, because there are no
     * accelerators for Text in it.  However, it makes it possible for
     * users to add accelerators to their own resources, so that, for
     * example, they can use the arrow keys to scroll through the
     * index window rather than through the article text.  To do that,
     * they would put the following in their resources:
     *
     * xrn*index.accelerators: #override \n\
     * 		<Key>Down:	next-line() \n\
     * 		<Key>Up:	previous-line()
     */
#ifdef notdef
    XtInstallAccelerators(ArticleText, Text);
#endif
    
#if defined(MOTIF) && defined(EDITRES) && XtSpecificationRelease == 5
    /* Enable editres interaction (needs X11R5 Xmu; Xaw already has this) */
    {
	extern void _XEditResCheckMessages();

	XtAddEventHandler(TopLevel, (EventMask)0, True,
			  _XEditResCheckMessages, NULL);
    }
#endif

    XtRealizeWidget(TopLevel);
    makePopup(Frame);
    if (app_resources.twoWindows)
	makeArtPopup(ArtWindow);

    XChangeProperty(XtDisplay(TopLevel), XtWindow(TopLevel),
		    XA_WM_NAME, XA_STRING, 8, PropModeReplace,
			(unsigned char *) title, strlen(title));

#ifdef MOTIF
    XChangeProperty(XtDisplay(TopLevel), XtWindow(TopLevel),
		    XA_WM_ICON_NAME, XA_STRING, 8, PropModeReplace,
			(unsigned char *) "mxrn", 4);
#else
    XChangeProperty(XtDisplay(TopLevel), XtWindow(TopLevel),
		    XA_WM_ICON_NAME, XA_STRING, 8, PropModeReplace,
			(unsigned char *) "dxrn", 4);
#endif

    /* set up the text window, mode buttons, and question */
    determineMode();

    unbusyCursor();
    addTimeOut();
    XRNState |= XRN_X_UP;

#ifdef MOTIF_V10
    XtSetArg(resizeArg[0], XmNrefigureMode, False);
    XtSetValues(XtParent(TopButtonBox), resizeArg, 1);
    XtSetArg(resizeArg[0], XmNrefigureMode, True);
    XtSetValues(XtParent(TopButtonBox), resizeArg, 1);
#endif

    XtMainLoop();
    exit(0);
}       
#ifdef MOTIF
static Boolean resetLimits(w)
Widget w;
{
    Arg minmax[5];
    int i = 0;
    if (w == BottomButtonBox || w == BottomInfoLine) return True;
#ifndef MOTIF_V10
    XtSetArg(minmax[i], XmNpaneMinimum, 1);i++;
    XtSetArg(minmax[i], XmNpaneMaximum, 5000);i++;
    XtSetArg(minmax[i], XmNskipAdjust, False);i++;
    XtSetArg(minmax[i], XmNallowResize, (XtArgVal) True);i++;
    XtSetValues(w, minmax, i);
#endif
    return True;
}
void PaneSetMinMax(w, min, max)
Widget w;
Dimension min;
Dimension max;
{
    Arg minmax[5];
    int i = 0;
#ifdef MOTIF_V10
    if (max <= min) max = min + 1;
    XtSetArg(minmax[i], XmNminimum, (XtArgVal) min);i++;
    XtSetArg(minmax[i], XmNmaximum, (XtArgVal) max);i++;
    XtSetArg(minmax[i], XmNskipAdjust, (XtArgVal) True);i++;
#else
    if (max <= min) max = min + 1;
    XtSetArg(minmax[i], XmNpaneMinimum, (XtArgVal) min);i++;
    XtSetArg(minmax[i], XmNpaneMaximum, (XtArgVal) max);i++;
    XtSetArg(minmax[i], XmNskipAdjust, (XtArgVal) True);i++;
#endif
    XtSetArg(minmax[i], XmNheight,  (XtArgVal) min);i++;
    XtSetValues(w, minmax, i);
#ifndef MOTIF_V10
    XtAddWorkProc((XtWorkProc)resetLimits, w);
#endif
}
static Boolean resetBox(height)
int height;
{
    Arg minmax[5];
    int i;
    Dimension current = 0;
    Dimension width = 0;
    Dimension curHeight = 0;
    Dimension borderWidth = 0;
 
    XtSetArg(minmax[0], XmNheight, &current);
    XtSetArg(minmax[1], XmNwidth, &width);
    XtSetArg(minmax[2], XmNborderWidth, &borderWidth);

    XtGetValues(TopButtonBox, minmax, 1);
    if (oneTime || (current < height)) {
	oneTime = False;
	XtGetValues(MainWindow, minmax, 3);
	if (app_resources.twoWindows) {
	    if (app_resources.indexLayout) {
		width = frameArgs[2].value;
	    }
	} else {
	    if (app_resources.mainLayout) {
		width = frameArgs[2].value;
	    }
	}
	XtResizeWidget(MainWindow, width, current + 1, borderWidth);
    }
    i = 0;
#ifndef MOTIF_V10
    XtSetArg(minmax[i], XmNpaneMinimum, (XtArgVal) 1);i++;
    XtSetArg(minmax[i], XmNpaneMaximum, (XtArgVal) 5000);i++;
#else
    XtSetArg(minmax[i], XmNminimum, (XtArgVal) 1);i++;
    XtSetArg(minmax[i], XmNmaximum, (XtArgVal) 5000);i++;
#endif
    XtSetArg(minmax[i], XmNskipAdjust, (XtArgVal) False);i++;
    XtSetArg(minmax[i], XmNallowResize, (XtArgVal) True);i++;
    XtSetArg(minmax[i], XmNheight, (XtArgVal) (height+2));i++;
    XtSetValues(TopButtonBox, minmax, i);
#ifndef MOTIF_V10
    XtSetArg(minmax[0], XmNskipAdjust, False);
    XtSetArg(minmax[0], XmNallowResize, True);
    if (!app_resources.twoWindows)
	XtSetValues(BottomButtonBox, minmax, 1);
    XtSetValues(TopInfoLine, minmax, 1);
    if (!app_resources.twoWindows)
	XtSetValues(BottomInfoLine, minmax, 1);
    XtSetValues(TopList, minmax, 1);
    if (!app_resources.twoWindows) {
	XtSetValues(TopShadowFrame, minmax, 2);
	XtSetValues(BotShadowFrame, minmax, 2);
	XtSetValues(TopInfoLine, minmax, 2);
    }
#ifdef TITLEBAR
    XtSetValues(TitleBar, minmax, 1);
#endif
#endif /* MOTIF_V10 */
    return True;
}
void SetButtonBoxSize(min, max)
Dimension min, max;
{
    Arg minmax[5];
    int i;
    Dimension height = 0;					    /* ggh */
    Dimension width = 0;					    /* ggh */
    Dimension borderWidth = 0;					    /* ggh */

    XtSetArg(minmax[0], XmNheight,  (XtArgVal) &height);
    XtGetValues(TopButtonBox, minmax, 1);
    if (!oneTime) {
	if (height >= min) return;
	if ((min - height) < 4) return;
    }
#ifndef MOTIF_V10
    XtSetArg(minmax[0], XmNskipAdjust, True);
    if (!app_resources.twoWindows)
	XtSetValues(BottomButtonBox, minmax, 1);
    XtSetValues(TopInfoLine, minmax, 1);
    if (!app_resources.twoWindows)
	XtSetValues(BottomInfoLine, minmax, 1);
    if (!app_resources.twoWindows)
	XtSetValues(TopShadowFrame, minmax, 1);
#ifdef TITLEBAR
    XtSetValues(TitleBar, minmax, 1);
#endif
    if (app_resources.twoWindows) {
	XtSetValues(TopShadowFrame, minmax, 1);
	XtSetValues(TopInfoLine, minmax, 1);
    }
#endif /* MOTIF_V10 */

    i = 0;
#ifdef MOTIF_V10
    if (max <= min) max = min + 1;
    XtSetArg(minmax[i], XmNminimum, (XtArgVal) min);i++;
    XtSetArg(minmax[i], XmNmaximum, (XtArgVal) max);i++;
    XtSetArg(minmax[i], XmNskipAdjust, (XtArgVal) True);i++;
#else
    if (max < min) max = min + 1;
    XtSetArg(minmax[i], XmNallowResize, (XtArgVal) True);i++;
    XtSetArg(minmax[i], XmNskipAdjust, (XtArgVal) True);i++;
    XtSetArg(minmax[i], XmNpaneMinimum, (XtArgVal) min);i++;
    XtSetArg(minmax[i], XmNpaneMaximum, (XtArgVal) max);i++;
#endif
    XtSetArg(minmax[i], XmNheight,  (XtArgVal) min);i++;
    XtSetValues(TopButtonBox, minmax, i);
    XtAddWorkProc((XtWorkProc)resetBox, (void *) min);
}
#else /* MOTIF */
static Boolean resetLimits(w)
Widget w;
{
    Arg minmax[5];
    XtSetArg(minmax[0], DwtNresizable, True);
    XtSetArg(minmax[1], DwtNmin, 1);
    XtSetArg(minmax[2], DwtNmax, 5000);
    XtSetValues(w, minmax, 3);
    return True;
}
void PaneSetMinMax(w, min, max)
Widget w;
Dimension min;
Dimension max;
{
    Arg minmax[5];

    if (max <= min) max = min + 1;
    XtSetArg(minmax[0], DwtNresizable, (XtArgVal) False);
    XtSetArg(minmax[1], DwtNmin, (XtArgVal) min);
    XtSetArg(minmax[2], DwtNmax, (XtArgVal) max);
    XtSetValues(w, minmax, 3);
}
static Boolean resetBox(height)
int height;
{
    Arg minmax[5];
    int i;

    DwtPaneSetConstraints(TopButtonBox, 1, 5000, height, True);

    i = 0;
    XtSetArg(minmax[i], DwtNmin, (XtArgVal) 1);i++;
    XtSetArg(minmax[i], DwtNmax, (XtArgVal) 5000);i++;
    XtSetArg(minmax[i], DwtNresizable, (XtArgVal) True);i++;
    XtSetValues(TopInfoLine, minmax, i);
    XtSetValues(TopList, minmax, i);
    if (!app_resources.twoWindows)
	XtSetValues(ScrollWindow, minmax, i);
#ifdef TITLEBAR
    XtSetValues(TitleBar, minmax, i);
#endif
    return True;
}
void SetButtonBoxSize(min, max)
Dimension min, max;
{
    Arg minmax[5];
    int i;

    if (max <= min) max = min + 1;
    XtSetArg(minmax[0], DwtNresizable, False);
    XtSetValues(TopInfoLine, minmax, 1);
    XtSetValues(TopList, minmax, 1);
    if (!app_resources.twoWindows)  
	XtSetValues(ScrollWindow, minmax, 1);
#ifdef TITLEBAR
    XtSetValues(TitleBar, minmax, 1);
#endif
    DwtPaneSetConstraints(TopButtonBox, min, max, min, False);
    XtAddWorkProc(resetBox, min);
}
#endif /* MOTIF */
#ifdef XFILESEARCHPATH
static void
AddPathToSearchPath(path)
char *path;
{
     char *old, *new;

     old = getenv("XFILESEARCHPATH");
     if (old) {
#ifdef USE_PUTENV
	  /* +1 for =, +2 for :, +3 for null */
	  new = XtMalloc((Cardinal) (strlen("XFILESEARCHPATH") +
				     strlen(old) +
				     strlen(path) + 3));
	  (void) strcpy(new, "XFILESEARCHPATH");
	  (void) strcat(new, "=");
	  (void) strcat(new, old);
	  (void) strcat(new, ":");
	  (void) strcat(new, path);
	  putenv(new);
#else
	  /* +1 for colon, +2 for null */
	  new = XtMalloc((Cardinal) (strlen(old) + strlen(path) + 2));
	  (void) strcpy(new, old);
	  (void) strcat(new, ":");
	  (void) strcat(new, path);
	  setenv("XFILESEARCHPATH", new, 1);
#endif
     }
     else {
#ifdef USE_PUTENV
	  new = XtMalloc((Cardinal) (strlen("XFILESEARCHPATH") +
				     strlen(path) + 2));
	  (void) strcpy(new, "XFILESEARCHPATH");
	  (void) strcat(new, "=");
	  (void) strcat(new, path);
	  putenv(new);
#else
	  setenv("XFILESEARCHPATH", path, 1);
#endif
     }
}
#endif
