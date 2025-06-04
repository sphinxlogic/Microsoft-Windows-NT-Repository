#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/buttons.c,v 1.9 1993/02/04 18:22:03 ricks Exp $";
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
 * buttons.c: create and handle the buttons
 *
 */

#include "copyright.h"
#include <stdio.h>
#ifdef __STDC__
#include <time.h>
#ifndef VMS
#include <unistd.h>
#else
#include <unixio.h>
#include <unixlib.h>
extern int sys$getmsg();
#endif
#include <stdlib.h>
#endif
#include "config.h"
#include "utils.h"
#ifndef VMS
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#else
#include <decw$include/Intrinsic.h>
#include <decw$include/StringDefs.h>
#endif
#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/PanedW.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/TextP.h>
#include <Xm/DialogS.h>
#if (XmVERSION == 1) && (XmREVISION == 0) /* Motif V1.0 */
#define MOTIF_V10
#endif
#else
#ifndef VMS
#include <X11/DECwDwtApplProg.h>
#else
#include <decw$include:DECwDwtApplProg.h>
#endif
#endif /* MOTIF */
#include "compose.h"
#include "cursor.h"
#include "mesg.h"
#include "dialogs.h"
#include "modes.h"
#include "resources.h"
#include "internals.h"
#include "save.h"
#include "error_hnds.h"
#include "xthelper.h"
#include "xrn.h"
#include "news.h"
#include "buttons.h"
#include "newsrcfile.h"
#include "xmisc.h"
#include "slist.h"
#include "butdefs.h"

extern void PaneSetMinMax();
extern void SetButtonBoxSize();

#ifndef MOTIF
#define XmNlabelString DwtNlabel
#define XmNactivateCallback DwtNactivateCallback
#define XmNborderWidth DwtNborderWidth
#endif


extern void resetPopupSize();
extern void close_server();
extern void EditKillFile();

#ifdef VMS
#include <descrip.h>
#endif

static Widget UnSubConfirmBox = NULL;
static Widget *AddButtons = NIL(Widget);
static Widget *NgButtons = NIL(Widget);
static Widget *AllButtons = NIL(Widget);
static Widget *ArtButtons = NIL(Widget);
static Widget *ArtSpecButtons = NIL(Widget);
externalref Widget ngMenus[5];
externalref Widget artMenus[7];
externalref Widget allMenus[5];
externalref Widget addMenus[3];
externalref Widget *AddPopupButtons;
externalref Widget *NgPopupButtons;
externalref Widget *AllPopupButtons;
externalref Widget *ArtPopupButtons;
externalref Widget *ArtWinPopupButtons;

static struct newsgroup **AddGroups =  (struct newsgroup **) 0; /* new newsgroups list ...	   */
static ng_num *NewsGroups = (ng_num *) 0; /* newsgroups list that is displayed */
static ng_num *AllGroups = (ng_num *) 0;  /* list of all groups so the user */
					  /* can subscribe/unsubscribe to them */
static art_num *ArticleNumbers = (art_num *) 0; /* Article numbers of displayed arts */

static int  GroupPosition = 0; 	/* cursor position in newsgroup window */
static int  AddGroupPosition = 0;/* cursor position in newsgroup window */
static int  GroupItems = 0;	/* Number of groups */
static int  AddGroupItems = 0;	/* Number of new groups */
static int  ArtPosition = 0;	/* cursor position in article subject window */
static int  SubjectItems = 0;	/* Number of subjects */
static int  AllPosition = 0;	/* position in all groups window */
static int  AllGroupItems = 0;	/* Number of items in All Groups strings */

static char LastGroup[GROUP_NAME_SIZE];	/* last newsgroup accessed; used to */
				/* determine whether or not to move the */
				/* cursor in the newsgroup window */

static art_num CurrentArticle;	/* the number of the article currently */
				/* displayed, used for marking an article */
				/* as saved */

static art_num PrevArticle;	/* the number of the article displayed */
				/* before the current one */

static int ArtStatus = 0;	/* keeps track of what kind of article to */
				/* search for: next, previous, or next */
				/* unread */
static int AllStatus = 0;	/* keeps track of which order to put the */
				/* groups in in all groups mode */
static char *LastRegexp;	/* last regular expression searched for */
static int LastSearch;		/* the direction of the last regular */
				/* expression search */

static int LastWasAuthor = 0;	/* Type of last search */
static char *selList = NULL;	/* Keeps the selected entry array */
static int selSize = 0;		/* current size of the array */
static int selCount = 0;	/* selection count */

static int Action;		/* action to take when a confirmation box */
				/* is clicked in */

static int SelectionItem = -1;	/* List item selected */
static int SelectionCount= 0;	/* Number of items selected */

static int NewsgroupDisplayMode = 0;	/* 0 for unread groups, 1 for all sub */
#define XRN_JUMP 0
#define XRN_GOTO 1
static int NewsgroupEntryMode = XRN_GOTO;

static Boolean oldListed = False;/* TRUE if in LIST OLD mode */
static art_num oldInitial = 0;	/* Saved initial article # */

static Boolean ArtPoppedUp = False;

static char *SaveString = NULL;	/* Last input to save box */
static int ArtEntry = 1;
static int currentTop = 0;	/* Current list top entry */
static int saveTop = 0;		/* saved top entry */

Boolean printPending = 0;	/* True if print file pending */

static Boolean unreadNews = False;	/* True if unread news */

/* article mode "modes" ... determines what to do: jump out of article */
/* mode, change the subject string, or do nothing */
#define art_DONE 0
#define art_CHANGE 1
#define art_NOCHANGE 2

/* keeps track of which type of article to search for: Next, Unread, or */
/* previous */
#define art_NEXT 0
#define art_UNREAD 1
#define art_PREV 2
#define art_CURRENT 3
#define art_FINDUNREAD 4

int Mode = NO_MODE;			/* current mode */
static int PreviousMode = NO_MODE;	/* previous mode, what buttons to */
					/* remove */

#define XRN_NO 0
#define XRN_YES 1

/* the user is in a command - eat type ahead */
int inCommand = 0;
extern Boolean ComposeActive;

static struct _translations {
    Widget widget;
    XtTranslations tables[MAX_MODE];
    char *unparsed[MAX_MODE];
} Translations[7];

static void listSelect();
static void DoubleClick();
static void GetData();
static void subjSetPos();
static void resetSubjectWidget();
static void catchUpNG();
static void unsubscribeNG();
static void catchUpART();
static void catchUpPartART();
static void unsubscribeART();
static void switchToAllMode();
static void fedUpART();
static void setTitleBar();
extern XtActionProc slistForwardPage();
extern XtActionProc slistBackwardPage();
extern XtActionProc slistForwardLine();
extern XtActionProc slistBackwardLine();
static XtActionProc listForwardPage();
static XtActionProc listBackwardPage();
static XtActionProc listForwardLine();
static XtActionProc listBackwardLine();
static XtActionProc artReadCurrent();
static Boolean      artCheckReadCurrent();

/* handle autorescan timeouts */

static XtIntervalId TimeOut = 0;
void autoRescan();
void autoExitAddMode();

/*ARGSUSED*/
static void timerRoutine(data, id)
caddr_t data;
XtIntervalId *id;
{
    if (inCommand || (Mode != NEWSGROUP_MODE && Mode != ADD_MODE)) {
	TimeOut = 0;
	return;
    }
    if (TimeOut != *id) {
	(void) fprintf(stderr, "bad time out, id is %d, expected %d\n", *id, TimeOut);
	TimeOut = 0;
	return;
    }
    if (Mode == NEWSGROUP_MODE) {
	autoRescan(data, id);
    } else {
	if (Mode == ADD_MODE) {
	    autoExitAddMode(data, id);
	}
    }
}
void addTimeOut()
{
#if defined(MOTIF) && !defined(MOTIF_V10)
    XmProcessTraversal(TopList,XmTRAVERSE_CURRENT);
    XmProcessTraversal(ArticleText,XmTRAVERSE_CURRENT);
#endif

    if (app_resources.rescanTime <= 0) {
	return;
    }

    /* do not allow recursive timeouts */
    if (TimeOut) {
	return;
    }

    /* handle race conditions??? */
    TimeOut = 1;

    TimeOut = XtAddTimeOut(app_resources.rescanTime * 1000,
	 (XtTimerCallbackProc) timerRoutine, 0);
    return;
}

void removeTimeOut()
{
    XtIntervalId temp;

    /* handle race conditions??? */
    temp = TimeOut;
    TimeOut = 0;

    /* do not allow recursive timeouts */
    if (temp) {
	XtRemoveTimeOut(temp);
    }
    return;
}

/*
 * The newsgroup and article command buttons
 *
 *    To add a button:
 *
 *      - add the appropriate static Arg[] for describing the button
 *	  label and handler (define the return value of the handler)
 *      - add the variable and size to the apropriate *ButtonList
 *      - add it to the actions table and the keyactions table
 *
 */

BUTTON(addQuit)
BUTTON(addFirst)
BUTTON(addLast)
BUTTON(addAfter)
BUTTON(addUnsub)

BUTTON(ngExit)
BUTTON(ngQuit)
BUTTON(ngRead)
BUTTON(ngOpen)
BUTTON(ngNext)
BUTTON(ngPrev)
BUTTON(ngCatchUp)
BUTTON(ngSubscribe)
BUTTON(ngUnsub)
BUTTON(ngGoto)
BUTTON(ngToggleGroups)
BUTTON(ngAllGroups)
BUTTON(ngRescan)
BUTTON(ngPrevGroup)
BUTTON(ngSelect)
BUTTON(ngMove)
BUTTON(ngCheckPoint)
BUTTON(ngDisconnect)
BUTTON(ngPost)
BUTTON(ngGripe)

BUTTON(allQuit)
BUTTON(allSub)
BUTTON(allFirst)
BUTTON(allLast)
BUTTON(allAfter)
BUTTON(allUnsub)
BUTTON(allGoto)
BUTTON(allOpen)
BUTTON(allSelect)
BUTTON(allMove)
BUTTON(allToggle)
BUTTON(allScroll)
BUTTON(allScrollBack)
BUTTON(allPost)
BUTTON(allSearch)
BUTTON(allContinue)
intBUTTON(allCancelSearch)

BUTTON(artQuit)
BUTTON(artNext)
BUTTON(artNextUnread)
BUTTON(artPrev)
BUTTON(artLast)
BUTTON(artNextGroup)
BUTTON(artGotoArticle)
BUTTON(artCatchUp)
BUTTON(artCatchUpAll)
BUTTON(artFedUp)
BUTTON(artMarkRead)
BUTTON(artMarkUnread)
BUTTON(artUnsub)
BUTTON(artScroll)
BUTTON(artScrollBack)
BUTTON(artScrollEnd)
BUTTON(artScrollBeginning)
BUTTON(artScrollLine)
BUTTON(artScrollBackLine)
BUTTON(artScrollIndex)
BUTTON(artScrollIndexBack)
BUTTON(artSubNext)
BUTTON(artSubPrev)
BUTTON(artKillSession)
BUTTON(artKillLocal)
BUTTON(artKillGlobal)
BUTTON(artKillAuthor)
BUTTON(artEditKillFile)
BUTTON(artEditGlobalKillFile)
BUTTON(artSubSearch)
BUTTON(artAuthorSearch)
BUTTON(artContinue)
intBUTTON(artCancelSearch)
BUTTON(artNonSorted)
BUTTON(artSorted)
BUTTON(artStrictSorted)
BUTTON(artThreadSorted)
BUTTON(artPost)
BUTTON(artExit)
BUTTON(artCheckPoint)
BUTTON(artGripe)
BUTTON(artListOld)

BUTTON(artSave)
BUTTON(artReply)
BUTTON(artForward)
BUTTON(artFollowup)
BUTTON(artFollowupReply)
BUTTON(artCancel)
BUTTON(artRot13)
#ifdef XLATE
BUTTON(artXlate)
#endif /* XLATE */
BUTTON(artHeader)
BUTTON(artPrint)
BUTTON(artFlushQueue)

static void doTheRightThing _ARGUMENTS((Widget, XEvent *,String *,Cardinal *));
static XtActionsRec TopActions[] = {
    {"doTheRightThing",	   (XtActionProc) doTheRightThing},
    {"list-forward-page",  (XtActionProc) listForwardPage},
    {"list-backward-page", (XtActionProc) listBackwardPage},
    {"list-forward-line",  (XtActionProc) listForwardLine},
    {"list-backward-line", (XtActionProc) listBackwardLine},
};

void abortSet();

static XtActionsRec AddActions[] = {
    {"addQuit",		(XtActionProc) addQuitAction},
    {"addFirst",	(XtActionProc) addFirstAction},
    {"addLast",		(XtActionProc) addLastAction},
    {"addAfter",	(XtActionProc) addAfterAction},
    {"addUnsub",	(XtActionProc) addUnsubAction},
};

static XtActionsRec NgActions[] = {
    {"ngQuit",		(XtActionProc) ngQuitAction},
    {"ngRead",		(XtActionProc) ngReadAction},
    {"ngOpen",		(XtActionProc) ngOpenAction},
    {"ngNext",		(XtActionProc) ngNextAction},
    {"ngPrev",		(XtActionProc) ngPrevAction},
    {"ngCatchUp",	(XtActionProc) ngCatchUpAction},
    {"ngSubscribe",	(XtActionProc) ngSubscribeAction},
    {"ngUnsub",		(XtActionProc) ngUnsubAction},
    {"ngGoto",		(XtActionProc) ngGotoAction},
    {"ngRescan",	(XtActionProc) ngRescanAction},
    {"ngAllGroups",	(XtActionProc) ngAllGroupsAction},
    {"ngToggleGroups",	(XtActionProc) ngToggleGroupsAction},
    {"ngPrevGroup",	(XtActionProc) ngPrevGroupAction},
    {"ngSelect",	(XtActionProc) ngSelectAction},
    {"ngMove",		(XtActionProc) ngMoveAction},
    {"ngExit",		(XtActionProc) ngExitAction},
    {"ngGripe",		(XtActionProc) ngGripeAction},
    {"ngPost",		(XtActionProc) ngPostAction},
    {"ngDisconnect",	(XtActionProc) ngDisconnectAction},
    {"ngCheckPoint",	(XtActionProc) ngCheckPointAction},
};    
static XtActionsRec ArtActions[] = {
    {"artQuit",		(XtActionProc) artQuitAction},
    {"artNextUnread",	(XtActionProc) artNextUnreadAction},
    {"artScroll",	(XtActionProc) artScrollAction},
    {"artScrollBack",	(XtActionProc) artScrollBackAction},
    {"artScrollLine",	(XtActionProc) artScrollLineAction},
    {"artScrollBackLine", (XtActionProc) artScrollBackLineAction},
    {"artScrollEnd",	(XtActionProc) artScrollEndAction},
    {"artScrollBeginning",(XtActionProc) artScrollBeginningAction},
    {"artScrollIndex",	(XtActionProc) artScrollIndexAction},
    {"artScrollIndexBack", (XtActionProc) artScrollIndexBackAction},
    {"artNext",		(XtActionProc) artNextAction},
    {"artPrev",		(XtActionProc) artPrevAction},
    {"artLast",		(XtActionProc) artLastAction},
    {"artNextGroup",	(XtActionProc) artNextGroupAction},
    {"artCatchUp",	(XtActionProc) artCatchUpAction},
    {"artCatchUpAll",	(XtActionProc) artCatchUpAllAction},
    {"artFedUp",	(XtActionProc) artFedUpAction},
    {"artGotoArticle",	(XtActionProc) artGotoArticleAction},
    {"artMarkRead",	(XtActionProc) artMarkReadAction},
    {"artMarkUnread",	(XtActionProc) artMarkUnreadAction},
    {"artUnsub",	(XtActionProc) artUnsubAction},
    {"artSubNext",	(XtActionProc) artSubNextAction},
    {"artSubPrev",	(XtActionProc) artSubPrevAction},
    {"artKillSession",	(XtActionProc) artKillSessionAction},
    {"artKillLocal",	(XtActionProc) artKillLocalAction},
    {"artKillGlobal",	(XtActionProc) artKillGlobalAction},
    {"artKillAuthor",	(XtActionProc) artKillAuthorAction},
    {"artEditKillFile", (XtActionProc) artEditKillFileAction},
    {"artEditGlobalKillFile", (XtActionProc) artEditGlobalKillFileAction},
    {"artSubSearch",	(XtActionProc) artSubSearchAction},
    {"artAuthorSearch",	(XtActionProc) artAuthorSearchAction},
    {"artContinue",	(XtActionProc) artContinueAction},
    {"artCancelSearch",	(XtActionProc) artCancelSearchAction},
    {"artNonSorted",	(XtActionProc) artNonSortedAction},
    {"artSorted",	(XtActionProc) artSortedAction},
    {"artStrictSorted",	(XtActionProc) artStrictSortedAction},
    {"artThreadSorted",	(XtActionProc) artThreadSortedAction},
    {"artPost",		(XtActionProc) artPostAction},
    {"artExit",		(XtActionProc) artExitAction},
    {"artCheckPoint",	(XtActionProc) artCheckPointAction},
    {"artSave",		(XtActionProc) artSaveAction},
    {"artReply",	(XtActionProc) artReplyAction},
    {"artForward",	(XtActionProc) artForwardAction},
    {"artFollowup",	(XtActionProc) artFollowupAction},
    {"artFollowupReply",(XtActionProc) artFollowupReplyAction},
    {"artCancel",	(XtActionProc) artCancelAction},
    {"artRot13",	(XtActionProc) artRot13Action},
#ifdef XLATE
    {"artXlate",	(XtActionProc) artXlateAction},
#endif /* XLATE */
    {"artHeader",	(XtActionProc) artHeaderAction},
    {"artPrint",	(XtActionProc) artPrintAction},
    {"artFlushQueue",	(XtActionProc) artFlushQueueAction},
    {"artGripe",	(XtActionProc) artGripeAction},
    {"artListOld",	(XtActionProc) artListOldAction},
    {"artReadCurrent",	(XtActionProc) artReadCurrent},
};

static XtActionsRec AllActions[] = {
    {"allQuit",		(XtActionProc) allQuitAction},
    {"allSub",		(XtActionProc) allSubAction},
    {"allFirst",	(XtActionProc) allFirstAction},
    {"allLast",		(XtActionProc) allLastAction},
    {"allAfter",	(XtActionProc) allAfterAction},
    {"allUnsub",	(XtActionProc) allUnsubAction},
    {"allGoto",		(XtActionProc) allGotoAction},
    {"allOpen",		(XtActionProc) allOpenAction},
    {"allSelect",	(XtActionProc) allSelectAction},
    {"allMove",		(XtActionProc) allMoveAction},
    {"allToggle",	(XtActionProc) allToggleAction},
    {"allScroll",	(XtActionProc) allScrollAction},
    {"allScrollBack",	(XtActionProc) allScrollBackAction},
    {"allPost",		(XtActionProc) allPostAction},
    {"allSearch",	(XtActionProc) allSearchAction},
    {"allContinue",	(XtActionProc) allContinueAction},
    {"allCancelSearch", (XtActionProc) allCancelSearchAction},
};

typedef struct buttonList {
    char *label;
    Arg *buttonArgs;
    unsigned int size;
    char *message;
} ButtonList;

externaldef(addbuttonlist) ButtonList AddButtonList[] = {
    {"Exit", addQuitArgs, XtNumber(addQuitArgs),
   "Exit add mode, unsubscribe remaining groups and append to your .newsrc"},
#ifdef NEWBUTTONS
    {"Prepend Subscribed", addFirstArgs, XtNumber(addFirstArgs),
   "Subscribe to selected group(s), prepend to your .newsrc"},
    {"Append Subscribed", addLastArgs, XtNumber(addLastArgs),
   "Subscribe to selected group(s), append to your .newsrc"},
    {"Position Subscribed", addAfterArgs, XtNumber(addAfterArgs),
   "Subscribe to selected group(s), add to your .newsrc after specific group"},
    {"Append Unsubscribed", addUnsubArgs, XtNumber(addUnsubArgs),
   "Unsubscribe to selected group(s), append to your .newsrc"},
#else
    {"Subscribe First", addFirstArgs, XtNumber(addFirstArgs),
   "Subscribe to selected group(s), add to the beginning of your .newsrc file"},
    {"Subscribe Last", addLastArgs, XtNumber(addLastArgs),
   "Subscribe to selected group(s), add to the end of your .newsrc file"},
    {"Add After Group", addAfterArgs, XtNumber(addAfterArgs),
   "Subscribe to selected group(s), add after a specific group in your .newsrc file"},
    {"Add Unsubscribed", addUnsubArgs, XtNumber(addUnsubArgs),
   "Unsubscribe to selected group(s), append to your .newsrc"},
#endif
};

static int AddButtonListCount = XtNumber(AddButtonList);
externaldef(addpopupcount) int AddPopupCount = XtNumber(AddButtonList);

externaldef(ngbuttonlist) ButtonList NgButtonList[] = {
    {"Quit", ngQuitArgs, XtNumber(ngQuitArgs),
#ifdef MOTIF
   "Quit MXRN"},
#else
   "Quit DXRN"},
#endif
    {"Read Group", ngReadArgs, XtNumber(ngReadArgs),
   "Read the articles in the current group"},
    {"Open Group", ngOpenArgs, XtNumber(ngOpenArgs),
   "Open the current group"},
    {"Next", ngNextArgs, XtNumber(ngNextArgs),
   "Move the cursor to the next group"},
    {"Prev", ngPrevArgs, XtNumber(ngPrevArgs),
   "Move the cursor to the previous group"},
#ifdef NEWBUTTONS
    {"Mark All Read", ngCatchUpArgs, XtNumber(ngCatchUpArgs),
#else
    {"Catch Up", ngCatchUpArgs, XtNumber(ngCatchUpArgs),
#endif
   "Mark all articles in the current group as read"},
    {"Subscribe", ngSubscribeArgs, XtNumber(ngSubscribeArgs),
   "Subscribe to a group"},
    {"Unsubscribe", ngUnsubArgs, XtNumber(ngUnsubArgs),
   "Unsubscribe to the current group"},
    {"Go To Group", ngGotoArgs, XtNumber(ngGotoArgs),
   "Go to a group"},
    {"All Groups", ngAllGroupsArgs, XtNumber(ngAllGroupsArgs),
   "View all available groups, with option to change subscription status"},
    {"Toggle Groups", ngToggleGroupsArgs, XtNumber(ngToggleGroupsArgs),
   "Show/hide groups with no new articles"},
    {"Rescan", ngRescanArgs, XtNumber(ngRescanArgs),
   "Query the news server for new articles and groups"},
    {"Last Group", ngPrevGroupArgs, XtNumber(ngPrevGroupArgs),
   "Return to the group last visited"},
    {"Select Groups", ngSelectArgs, XtNumber(ngSelectArgs),
   "Mark current selections for subsequent move operations"},
    {"Move", ngMoveArgs, XtNumber(ngMoveArgs),
   "Insert previously selected groups in front of the current group"},
    {"Exit", ngExitArgs, XtNumber(ngExitArgs),
#ifdef MOTIF
   "Quit MXRN, leaving your .newsrc unchanged since the last rescan"},
#else
   "Quit DXRN, leaving your .newsrc unchanged since the last rescan"},
#endif
    {"Disconnect", ngDisconnectArgs, XtNumber(ngDisconnectArgs),
   "Disconnect from the news server until next action"},
    {"Checkpoint", ngCheckPointArgs, XtNumber(ngCheckPointArgs),
   "Save changes to your .newsrc"},
    {"Gripe", ngGripeArgs, XtNumber(ngGripeArgs),
   "Mail a gripe to the XRN maintainer"},
    {"Post", ngPostArgs, XtNumber(ngPostArgs),
   "Post an article to a specific group"},
};

static int NgButtonListCount = XtNumber(NgButtonList);
externaldef(ngpopupcount) int NgPopupCount = XtNumber(NgButtonList);

externaldef(allbuttonlist) ButtonList AllButtonList[] = {
    {"Return", allQuitArgs, XtNumber(allQuitArgs),
   "Return to Newsgroup mode, saving changes"},
    {"Subscribe", allSubArgs, XtNumber(allSubArgs),
   "Subscribe to selected group(s)"},
    {"Subscribe First", allFirstArgs, XtNumber(allFirstArgs),
   "Subscribe to selected group(s), move to start of .newsrc"},
    {"Subscribe Last", allLastArgs, XtNumber(allLastArgs),
   "Subscribe to selected group(s), move to end of .newsrc"},
    {"Subscribe After Group", allAfterArgs, XtNumber(allAfterArgs),
   "Subscribe to selected group(s), move to after specific group in .newsrc"},
    {"Unsubscribe", allUnsubArgs, XtNumber(allUnsubArgs),
   "Unsubscribe to the selected group(s)"},
    {"Go To Group", allGotoArgs, XtNumber(allGotoArgs),
   "Go to a specific group"},
    {"Open Group", allOpenArgs, XtNumber(allOpenArgs),
   "Open the current group"},
    {"Select Groups", allSelectArgs, XtNumber(allSelectArgs),
   "Mark current selections for subsequent move operations"},
    {"Move", allMoveArgs, XtNumber(allMoveArgs),
   "Insert previously selected group(s) in front of current group"},
    {"Toggle Order", allToggleArgs, XtNumber(allToggleArgs),
   "Change order of groups: alphabetical/.newsrc order"},
    {"Scroll Forward", allScrollArgs, XtNumber(allScrollArgs),
   "Scroll the All Groups screen forward"},
    {"Scroll Backward", allScrollBackArgs, XtNumber(allScrollBackArgs),
   "Scroll the All Groups screen backward"},
    {"Post", allPostArgs, XtNumber(allPostArgs),
   "Post an article to a specific group"},
    {"Search", allSearchArgs, XtNumber(allSearchArgs),
   "Search for specific group(s)"},
    {"Continue", allContinueArgs, XtNumber(allContinueArgs),
   "Continue group search"},
    {"Cancel Search", allCancelSearchArgs, XtNumber(allCancelSearchArgs),
   "Cancel group search"},
};

static int AllButtonListCount = XtNumber(AllButtonList);
externaldef(allpopupcount) int AllPopupCount = XtNumber(AllButtonList);

externaldef(artbuttonlist) ButtonList ArtButtonList[] = {
    {"Return", artQuitArgs, XtNumber(artQuitArgs),
   "Return to previous mode"},
    {"Next Unread", artNextUnreadArgs, XtNumber(artNextUnreadArgs),
   "Display the next unread article"},
    {"Next", artNextArgs, XtNumber(artNextArgs),
   "Display the next article"},
    {"Scroll Forward", artScrollArgs, XtNumber(artScrollArgs),
   "Scroll displayed article forward"},
    {"Scroll Backward", artScrollBackArgs, XtNumber(artScrollBackArgs),
   "Scroll displayed article backward"},
    {"Scroll to End", artScrollEndArgs, XtNumber(artScrollEndArgs),
   "Scroll to end of displayed article"},
    {"Scroll to Beginning", artScrollBeginningArgs, XtNumber(artScrollBeginningArgs),
   "Scroll to beginning of displayed article"},
    {"Scroll Index", artScrollIndexArgs, XtNumber(artScrollIndexArgs),
   "Scroll index forward one page"},
    {"Scroll Index Back", artScrollIndexBackArgs, XtNumber(artScrollIndexBackArgs),
   "Scroll index back one page"},
    {"Prev", artPrevArgs, XtNumber(artPrevArgs),
   "Display the previous article"},
    {"Last", artLastArgs, XtNumber(artLastArgs),
   "Redisplay the last article displayed"},
    {"Next group", artNextGroupArgs, XtNumber(artNextGroupArgs),
   "*Go to the next group, skipping newsgroup mode"},
    {"Catch Up",artCatchUpArgs, XtNumber(artCatchUpArgs),
   "Mark all articles (up to the current article) in the current group as read"},
    {"Catch Up All",artCatchUpAllArgs, XtNumber(artCatchUpAllArgs),
   "Mark all articles in the current group as read and return to last mode"},
#ifdef NEWBUTTONS
    {"Next Group, Read", artFedUpArgs, XtNumber(artFedUpArgs),
#else
    {"Fed Up", artFedUpArgs, XtNumber(artFedUpArgs),
#endif
   "*Mark all articles in the current group as read and go to next group "},
    {"Go To Article", artGotoArticleArgs, XtNumber(artGotoArticleArgs),
   "Display a specific article number in the current group"},
    {"Mark Read", artMarkReadArgs, XtNumber(artMarkReadArgs),
   "Mark selected article(s) as read"},
    {"Mark Unread", artMarkUnreadArgs, XtNumber(artMarkUnreadArgs),
   "Mark selected article(s) as unread"},
    {"Unsubscribe", artUnsubArgs, XtNumber(artUnsubArgs),
   "Unsubscribe to the current group"},
    {"Subject Next", artSubNextArgs, XtNumber(artSubNextArgs),
   "Search for the next article with the same subject as the displayed article"},
    {"Subject Prev", artSubPrevArgs, XtNumber(artSubPrevArgs),
   "Search for the previous article with the same subject as the displayed article"},
    {"Session Kill", artKillSessionArgs, XtNumber(artKillSessionArgs),
   "Mark all articles in this group with the same subject as the displayed article as read, for this session only"},
    {"Local Kill", artKillLocalArgs, XtNumber(artKillLocalArgs),
   "Mark all articles in this group with the same subject as the displayed article as read, forever"},
    {"Global Kill", artKillGlobalArgs, XtNumber(artKillGlobalArgs),
   "Mark all articles in all groups with the same subject as the displayed article as read, forever"},
    {"Author Local Kill", artKillAuthorArgs, XtNumber(artKillAuthorArgs),
   "Mark all articles in this group with the same author as the displayed article as read, forever"},
    {"Edit Kill File", artEditKillFileArgs, XtNumber(artEditKillFileArgs),
   "Edit the kill file for this group"},
    {"Edit Global Kill File", artEditGlobalKillFileArgs,
     XtNumber(artEditGlobalKillFileArgs), "Edit the global kill file"},
    {"Subject Search", artSubSearchArgs, XtNumber(artSubSearchArgs),
   "Search index subject fields for a regular expression"},
    {"Author Search", artAuthorSearchArgs, XtNumber(artAuthorSearchArgs),
   "Search index author fields for a regular expression"},
    {"Continue", artContinueArgs, XtNumber(artContinueArgs),
   "Continue the regular expression subject/author search"},
    {"Cancel Search", artCancelSearchArgs, XtNumber(artCancelSearchArgs),
   "Cancel subject/author search"},
    {"Post", artPostArgs, XtNumber(artPostArgs),
   "Post an article to this group"},
    {"Article Order", artNonSortedArgs, XtNumber(artNonSortedArgs),
   "Display articles in article number order"},
    {"Subject Order", artSortedArgs, XtNumber(artSortedArgs),
   "Display articles in subject order (subsorted by article number)"},
    {"Strict Order", artStrictSortedArgs, XtNumber(artStrictSortedArgs),
   "Display articles in strict subject order"},
    {"Thread Order", artThreadSortedArgs, XtNumber(artThreadSortedArgs),
   "Display articles in threaded subject order"},
#ifdef NEWBUTTONS
    {"Return, Unread", artExitArgs, XtNumber(artExitArgs),
#else
    {"Exit", artExitArgs, XtNumber(artExitArgs),
#endif
   "Return to previous mode, marking all articles in current group as unread"},
    {"Checkpoint", artCheckPointArgs, XtNumber(artCheckPointArgs),
   "Save changes to your .newsrc"},
    {"Gripe", artGripeArgs, XtNumber(artGripeArgs),
   "Mail a gripe to the XRN maintainer"},
    {"List All", artListOldArgs, XtNumber(artListOldArgs),
#ifdef NEWBUTTONS
   "List all articles in the current group"},
#else
   "List all articles in the current group (may be slow)"},
#endif
/* The following duplicates the artSpecButtonList - be careful */
    {"Save", artSaveArgs, XtNumber(artSaveArgs),
   "Save the displayed article in a file"},
    {"Reply", artReplyArgs, XtNumber(artReplyArgs),
   "Mail a reply to the author of the displayed article"},
    {"Forward", artForwardArgs, XtNumber(artForwardArgs),
   "Forward the displayed article to a user(s)"},
    {"Followup", artFollowupArgs, XtNumber(artFollowupArgs),
   "Post a followup to the displayed article"},
    {"Followup+Reply", artFollowupReplyArgs, XtNumber(artFollowupReplyArgs),
   "Post a followup and reply to the author of the displayed article"},
    {"Cancel", artCancelArgs, XtNumber(artCancelArgs),
   "Cancel the displayed article"},
    {"Rot-13", artRot13Args, XtNumber(artRot13Args),
   "Decrypt an encrypted joke"},
#ifdef XLATE
    {"Translate", artXlateArgs, XtNumber(artXlateArgs),
   "Translate the current article"},
#endif /*XLATE */
    {"Toggle Header", artHeaderArgs, XtNumber(artHeaderArgs),
   "Display the complete/stripped header"},
    {"Print", artPrintArgs, XtNumber(artPrintArgs),
   "Print the displayed article"},
    {"Flush Queue", artFlushQueueArgs, XtNumber(artFlushQueueArgs),
   "Print any pending print jobs now"},
};

static int ArtButtonListCount = XtNumber(ArtButtonList);
externaldef(artpopupcount) int ArtPopupCount = XtNumber(ArtButtonList);
externaldef(artwinpopupcount) int ArtWinPopupCount = XtNumber(ArtButtonList);

externaldef(artspecbuttonlist) ButtonList ArtSpecButtonList[] = {
    {"Save", artSaveArgs, XtNumber(artSaveArgs),
   "Save the selected article(s) in a file"},
    {"Reply", artReplyArgs, XtNumber(artReplyArgs),
   "Mail a reply to the author of the displayed article"},
    {"Forward", artForwardArgs, XtNumber(artForwardArgs),
   "Forward displayed article to user(s)"},
    {"Followup", artFollowupArgs, XtNumber(artFollowupArgs),
   "Post a followup to the displayed article"},
    {"Followup+Reply", artFollowupReplyArgs, XtNumber(artFollowupReplyArgs),
   "Post a followup and reply to the author of the displayed article (as one action)"},
    {"Cancel", artCancelArgs, XtNumber(artCancelArgs),
   "Cancel the displayed article"},
    {"Rot-13", artRot13Args, XtNumber(artRot13Args),
   "Crypt/decrypt the displayed article"},
#ifdef XLATE
    {"Translate", artXlateArgs, XtNumber(artXlateArgs),
   "Translate the displayed article"},
#endif /*XLATE */
    {"Toggle Headers", artHeaderArgs, XtNumber(artHeaderArgs),
   "Display all/selected headers in displayed article"},
    {"Print", artPrintArgs, XtNumber(artPrintArgs),
   "Print selected article(s)"},
};

externaldef(artspecbuttonlistcount) int ArtSpecButtonListCount = XtNumber(ArtSpecButtonList);
externaldef(artspecpopupcount) int ArtSpecPopupCount = XtNumber(ArtButtonList);

static char TopNonButtonInfo[LABEL_SIZE];
static char BottomNonButtonInfo[LABEL_SIZE];
static char BottomNextGroupInfo[LABEL_SIZE];


/*
 * handle the Enter and Leave events for the buttons
 *
 * upon entering a button, get it's info string and put in the Question label
 * upon leaving a button, restore the old info string
 *
 */
/*ARGSUSED*/
void topInfoHandler(widget, client_data, event)
Widget widget;
caddr_t client_data;
XEvent *event;
{
    Arg infoLineArg[1];
#ifdef MOTIF
    XmString infoString;
    XmString defInfoString;
#else
    DwtCompString infoString;
    DwtCompString defInfoString;
#endif
    char *str = (char *) client_data;
    void bottomLabelHandler();

    if (inCommand) return;
    if (str[0] == '*') { /* Magic flag - toggle both */
	str++;
	bottomLabelHandler(BottomInfoLine, client_data, event);
    } 
    if (event->type == LeaveNotify) {
#ifdef MOTIF
	defInfoString = XmStringLtoRCreate(TopNonButtonInfo,
					   XmSTRING_DEFAULT_CHARSET);
	XtSetArg(infoLineArg[0], XmNlabelString, defInfoString);
	XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
	XmStringFree(defInfoString);
#else
	defInfoString = DwtLatin1String(TopNonButtonInfo);
	XtSetArg(infoLineArg[0], DwtNlabel, defInfoString);
	XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
	XtFree(defInfoString);
#endif
    } else if (event->type == EnterNotify) {
#ifdef MOTIF
	infoString = XmStringLtoRCreate(str, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(infoLineArg[0], XmNlabelString, infoString);
	XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
	XmStringFree(infoString);
#else
	infoString = DwtLatin1String(str);
	XtSetArg(infoLineArg[0], DwtNlabel, infoString);
	XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
	XtFree(infoString);
#endif
    }
    return;
}


/*
 * handle the Enter and Leave events for the buttons
 *
 * upon entering a button, get it's info string and put in the Question label
 * upon leaving a button, restore the old info string
 *
 */
/*ARGSUSED*/
void
bottomInfoHandler(widget, client_data, event)
Widget widget;
caddr_t client_data;
XEvent *event;
{
    Arg infoLineArg[1];
#ifdef MOTIF
    XmString infoString;
    XmString defInfoString;
#else
    DwtCompString infoString;
    DwtCompString defInfoString;
#endif

    if (inCommand) return;
    if (event->type == LeaveNotify) {
#ifdef MOTIF
	defInfoString = XmStringLtoRCreate(BottomNonButtonInfo,
					   XmSTRING_DEFAULT_CHARSET);
	XtSetArg(infoLineArg[0], XmNlabelString, defInfoString);
	XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
	XmStringFree(defInfoString);
#else
	defInfoString = DwtLatin1String(BottomNonButtonInfo);
	XtSetArg(infoLineArg[0], DwtNlabel, defInfoString);
	XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
	XtFree(defInfoString);
#endif
    } else if (event->type == EnterNotify) {
#ifdef MOTIF
	infoString = XmStringLtoRCreate(client_data,
					XmSTRING_DEFAULT_CHARSET);
	XtSetArg(infoLineArg[0], XmNlabelString, infoString);
	XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
	XmStringFree(infoString);
#else
	infoString = DwtLatin1String(client_data);
	XtSetArg(infoLineArg[0], DwtNlabel, infoString);
	XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
	XtFree(infoString);
#endif
    }
    return;
}

/*ARGSUSED*/
void
bottomLabelHandler(widget, client_data, event)
Widget widget;
caddr_t client_data;
XEvent *event;
{
    Arg infoLineArg[1];
#ifdef MOTIF
    XmString infoString;
#else
    DwtCompString infoString;
#endif

    if (inCommand) return;
    if (event->type == LeaveNotify) {
#ifdef MOTIF
	infoString = XmStringLtoRCreate(BottomNonButtonInfo,
					   XmSTRING_DEFAULT_CHARSET);
	XtSetArg(infoLineArg[0], XmNlabelString, infoString);
	XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
	XmStringFree(infoString);
#else
	infoString = DwtLatin1String(BottomNonButtonInfo);
	XtSetArg(infoLineArg[0], DwtNlabel, infoString);
	XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
	XtFree(infoString);
#endif
    } else if (event->type == EnterNotify) {
#ifdef MOTIF
	infoString = XmStringLtoRCreate(BottomNextGroupInfo,
					XmSTRING_DEFAULT_CHARSET);
	XtSetArg(infoLineArg[0], XmNlabelString, infoString);
	XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
	XmStringFree(infoString);
#else
	infoString = DwtLatin1String(BottomNextGroupInfo);
	XtSetArg(infoLineArg[0], DwtNlabel, infoString);
	XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
	XtFree(infoString);
#endif
    }
    return;
}


static void
resetSelection()
/*
 * Reset selection once it's used, so the user doesn't accidentally
 * move groups, etc.
 */
{
    selCount = 0;
    SelectionItem = -1;
    SListSelectAll(TopList, 0);
    setTitleBar(NULL);
    return;
}


static void
setTopInfoLineHandler(widget, message)
Widget widget;
char *message;
{
    XtAddEventHandler(widget,
		      (EventMask) (EnterWindowMask|LeaveWindowMask),
		      False,
		      (XtEventHandler) topInfoHandler,
		      (caddr_t) message);
    return;
}


static void
setBottomInfoLineHandler(widget, message)
Widget widget;
char *message;
{
    XtAddEventHandler(widget,
		      (EventMask) (EnterWindowMask|LeaveWindowMask),
		      False,
		      (XtEventHandler) bottomInfoHandler,
		      (caddr_t) message);
    return;
}


static void
setBottomInfoLabelHandler()
{
    XtAddEventHandler(BottomInfoLine,
		      (EventMask) (EnterWindowMask|LeaveWindowMask),
		      False,
		      (XtEventHandler) bottomLabelHandler,
		      (caddr_t) NULL);
    return;
}

#ifdef SWITCH_TOP_AND_BOTTOM
static void setBottomInfoLine(message)
#else
static void setTopInfoLine(message)
#endif
char *message;
{
    Arg infoLineArg[1];
#ifdef MOTIF
    XmString labelString;
#else
    DwtCompString labelString;
#endif

    if (!message) message = "";
    (void) strcpy(TopNonButtonInfo, (char *) message);
#ifdef MOTIF
    labelString = XmStringLtoRCreate(message, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(infoLineArg[0], XmNlabelString, labelString);
    XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
    XmStringFree(labelString);
#else
    labelString = DwtLatin1String(message);
    XtSetArg(infoLineArg[0], DwtNlabel, labelString);
    XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
    XtFree(labelString);
#endif
    return;
}


#ifdef SWITCH_TOP_AND_BOTTOM
static void setTopInfoLine(message)
#else
static void setBottomInfoLine(message)
#endif
char *message;
{
    Arg infoLineArg[1];
    char *p;
#ifdef MOTIF
    XmString labelString;
#else
    DwtCompString labelString;
#endif
    if (!message) message = "";
    if ((p = strchr(message, '\n'))) {
	*p = '\0';
	(void) strcpy(BottomNonButtonInfo, (char *) message);
	(void) strcpy(BottomNextGroupInfo, (char *) p+1);
	*p = '\n';
    } else {
	(void) strcpy(BottomNonButtonInfo, (char *) message);
	(void) strcpy(BottomNextGroupInfo, (char *) message);
    }
#ifdef MOTIF
    labelString = XmStringLtoRCreate(BottomNonButtonInfo,
		XmSTRING_DEFAULT_CHARSET);
    XtSetArg(infoLineArg[0], XmNlabelString, labelString);
    XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
    XmStringFree(labelString);
#else
    labelString = DwtLatin1String(BottomNonButtonInfo);
    XtSetArg(infoLineArg[0], DwtNlabel, labelString);
    XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
    XtFree(labelString);
#endif
    return;
}

#define TOP	0
#define BOTTOM	1

static void
doButtons(resource, box, buttons, buttonList, size, infoLine)
char *resource;
Widget box;
Widget *buttons;
ButtonList *buttonList;
int *size;
int infoLine;
{
    char *ptr, *token, *savePtr;
    int j, i = 0;

    if (resource) {
	savePtr = ptr = XtNewString(resource);

	while ((token = strtok(ptr, ", \t\n")) != NIL(char)) {
	    /* find name */
	    for (j = 0; j < *size; j++) {
		if (STREQ(token, (char *) buttonList[j].buttonArgs[0].value)) {
#ifdef MOTIF
		    if (buttonList[j].buttonArgs[1].value == (XtArgVal) NULL) {
			buttonList[j].buttonArgs[1].value =
			    (XtArgVal)XmStringLtoRCreate(buttonList[j].label,
			    XmSTRING_DEFAULT_CHARSET);
		    }
		    buttons[i] = XmCreatePushButton(box,
			    (char *) buttonList[j].buttonArgs[0].value,
			    buttonList[j].buttonArgs,
			    buttonList[j].size);
#else
		    if (buttonList[j].buttonArgs[1].value == NULL) {
			buttonList[j].buttonArgs[1].value =
			    (XtArgVal)DwtLatin1String(buttonList[j].label);
		    }
		    buttons[i] = DwtPushButtonCreate(box,
			    (char *) buttonList[j].buttonArgs[0].value,
			    buttonList[j].buttonArgs,
			    buttonList[j].size);
#endif
		    if (infoLine == TOP) {
			setTopInfoLineHandler(buttons[i], buttonList[j].message);
		    } else {
			setBottomInfoLineHandler(buttons[i], buttonList[j].message);
		    }
		    i++;
		    break;
		}
	    }
	    if (j == *size) {
		mesgPane(XRN_SERIOUS, "XRN error: bad button name: %s", token);
	    }
	    ptr = NIL(char);
	}
	*size = i;
	XtFree(savePtr);	
    } else {
	for (i = 0; i < *size; i++) {
	    
#ifdef MOTIF
	    if (buttonList[i].buttonArgs[1].value == (XtArgVal) NULL) {
		buttonList[i].buttonArgs[1].value =
		    (XtArgVal)XmStringLtoRCreate(buttonList[i].label,
		    XmSTRING_DEFAULT_CHARSET);
	    }
	    buttons[i] = XmCreatePushButton(box,
				(char *) buttonList[i].buttonArgs[0].value,
				buttonList[i].buttonArgs,
				buttonList[i].size);
#else
	    if (buttonList[i].buttonArgs[1].value == NULL) {
		buttonList[i].buttonArgs[1].value =
	    	    (XtArgVal)DwtLatin1String(buttonList[i].label);
	    }
	    buttons[i] = DwtPushButtonCreate(box,
				(char *) buttonList[i].buttonArgs[0].value,
				buttonList[i].buttonArgs,
				buttonList[i].size);
#endif
	    if (infoLine == TOP) {
		setTopInfoLineHandler(buttons[i], buttonList[i].message);
	    } else {
		setBottomInfoLineHandler(buttons[i], buttonList[i].message);
	    }
	}
    }
    setBottomInfoLabelHandler();
    return;
}


void
createButtons(mode)
int mode;
{
#define SETTRANSLATIONS(w, index, mode, bind) \
    Translations[index].widget = w; \
    Translations[index].unparsed[mode] = bind;

    int i;

    if (mode == -1) {
	SETTRANSLATIONS(MainWindow, 0, ADD_MODE, app_resources.addBindings);
	SETTRANSLATIONS(TopButtonBox, 1, ADD_MODE, app_resources.addBindings);
	SETTRANSLATIONS(BottomButtonBox, 2, ADD_MODE,app_resources.addBindings);
	SETTRANSLATIONS(TopInfoLine, 3, ADD_MODE, app_resources.addBindings);
	SETTRANSLATIONS(BottomInfoLine, 4, ADD_MODE, app_resources.addBindings);
	SETTRANSLATIONS(TopList, 5, ADD_MODE, app_resources.addBindings);
	SETTRANSLATIONS(ArticleText, 6, ADD_MODE, app_resources.addBindings);

	SETTRANSLATIONS(MainWindow, 0, ALL_MODE, app_resources.allBindings);
	SETTRANSLATIONS(TopButtonBox, 1, ALL_MODE, app_resources.allBindings);
	SETTRANSLATIONS(BottomButtonBox, 2, ALL_MODE,app_resources.allBindings);
	SETTRANSLATIONS(TopInfoLine, 3, ALL_MODE, app_resources.allBindings);
	SETTRANSLATIONS(BottomInfoLine, 4, ALL_MODE, app_resources.allBindings);
	SETTRANSLATIONS(TopList, 5, ALL_MODE, app_resources.allBindings);
	SETTRANSLATIONS(ArticleText, 6, ALL_MODE, app_resources.allBindings);

	SETTRANSLATIONS(MainWindow, 0, NEWSGROUP_MODE,app_resources.ngBindings);
	SETTRANSLATIONS(TopButtonBox,1,NEWSGROUP_MODE,app_resources.ngBindings);
	SETTRANSLATIONS(BottomButtonBox,2,NEWSGROUP_MODE,app_resources.ngBindings);
	SETTRANSLATIONS(TopInfoLine,3,NEWSGROUP_MODE,app_resources.ngBindings);
	SETTRANSLATIONS(BottomInfoLine,4,NEWSGROUP_MODE,app_resources.ngBindings);
	SETTRANSLATIONS(TopList,5,NEWSGROUP_MODE,app_resources.ngBindings);
	SETTRANSLATIONS(ArticleText,6,NEWSGROUP_MODE,app_resources.ngBindings);

	SETTRANSLATIONS(MainWindow,0, ARTICLE_MODE, app_resources.artBindings);
	SETTRANSLATIONS(TopButtonBox,1,ARTICLE_MODE, app_resources.artBindings);
	SETTRANSLATIONS(BottomButtonBox,2,ARTICLE_MODE,app_resources.artBindings);
	SETTRANSLATIONS(TopInfoLine,3,ARTICLE_MODE, app_resources.artBindings);
	SETTRANSLATIONS(BottomInfoLine,4,ARTICLE_MODE,app_resources.artBindings);
	SETTRANSLATIONS(TopList, 5, ARTICLE_MODE, app_resources.artBindings);
	SETTRANSLATIONS(ArticleText,6,ARTICLE_MODE,app_resources.artBindings);

	XtAddActions(TopActions, XtNumber(TopActions));

	AddButtons = ARRAYALLOC(Widget, XtNumber(AddButtonList));
	XtAddActions(AddActions, XtNumber(AddActions));

	doButtons(app_resources.addButtonList, TopButtonBox, AddButtons,
		  AddButtonList, &AddButtonListCount, TOP);

	NgButtons = ARRAYALLOC(Widget, XtNumber(NgButtonList));
	XtAddActions(NgActions, XtNumber(NgActions));

	doButtons(app_resources.ngButtonList, TopButtonBox, NgButtons,
		  NgButtonList, &NgButtonListCount, TOP);

	AllButtons = ARRAYALLOC(Widget, XtNumber(AllButtonList));
	XtAddActions(AllActions, XtNumber(AllActions));

	doButtons(app_resources.allButtonList, TopButtonBox, AllButtons,
		  AllButtonList, &AllButtonListCount, TOP);
    
	ArtButtons = ARRAYALLOC(Widget, XtNumber(ArtButtonList));
	XtAddActions(ArtActions, XtNumber(ArtActions));
    
	doButtons(app_resources.artButtonList, TopButtonBox, ArtButtons,
		  ArtButtonList, &ArtButtonListCount, TOP);
    
	ArtSpecButtons = ARRAYALLOC(Widget, XtNumber(ArtSpecButtonList));

	doButtons(app_resources.artSpecButtonList, BottomButtonBox,
		  ArtSpecButtons, ArtSpecButtonList, &ArtSpecButtonListCount,
		  BOTTOM);

	XtAddCallback(TopList, SListNselectCallback, listSelect, 0);
	XtAddCallback(TopList, SListNdoubleClickCallback, DoubleClick, 0);
	XtAddCallback(TopList, SListNgetDataCallback, GetData, 0);
    }

    if (mode == ADD_MODE) {
	XtUnmanageChildren(AddButtons, AddButtonListCount);
	for (i=0; i < AddButtonListCount; i++) {
	    XtDestroyWidget(AddButtons[i]);
	}
	AddButtonListCount = XtNumber(AddButtonList);
	doButtons(app_resources.addButtonList, TopButtonBox, AddButtons,
		AddButtonList, &AddButtonListCount, TOP);
	if (Mode == ADD_MODE) {
	    XtManageChildren(AddButtons, AddButtonListCount);
	}
    }
    if (mode == ALL_MODE) {
	XtUnmanageChildren(AllButtons, AllButtonListCount);
	for (i=0; i < AllButtonListCount; i++) {
	    XtDestroyWidget(AllButtons[i]);
	}
	AllButtonListCount = XtNumber(AllButtonList);
	doButtons(app_resources.allButtonList, TopButtonBox, AllButtons,
		AllButtonList, &AllButtonListCount, TOP);
	if (Mode == ALL_MODE) {
	    XtManageChildren(AllButtons, AllButtonListCount);
	}
    }
    if (mode == ARTICLE_MODE) {
	XtUnmanageChildren(ArtButtons, ArtButtonListCount);
	for (i=0; i < ArtButtonListCount; i++) {
	    XtDestroyWidget(ArtButtons[i]);
	}
	ArtButtonListCount = XtNumber(ArtButtonList);
	doButtons(app_resources.artButtonList, TopButtonBox, ArtButtons,
		ArtButtonList, &ArtButtonListCount, TOP);
	if (Mode == ARTICLE_MODE) {
	    XtManageChildren(ArtButtons, ArtButtonListCount);
	}
    }
    if (mode == NEWSGROUP_MODE) {
	XtUnmanageChildren(NgButtons, NgButtonListCount);
	for (i=0; i < NgButtonListCount; i++) {
	    XtDestroyWidget(NgButtons[i]);
	}
	NgButtonListCount = XtNumber(NgButtonList);
	doButtons(app_resources.ngButtonList, TopButtonBox, NgButtons,
		NgButtonList, &NgButtonListCount, TOP);
	if (Mode == NEWSGROUP_MODE) {
	    XtManageChildren(NgButtons, NgButtonListCount);
	}
    }
    return;
}

static void
setTranslations(mode)
int mode;
{
    Arg args[1];
    Arg targs[2];
    static int init[MAX_MODE] = {0,0,0,0};
    int i;

    if (!init[mode]) {
	/*
	 * first time:
	 *   parse table
	 *   override
	 *   get table back and store
	 */
	for (i = 0; i < sizeof(Translations) / sizeof(struct _translations); i++) {
	    XtTranslations table, table1;

	    if (Translations[i].unparsed[mode] == NIL(char)) {
		table = 0;
	    } else {
		table = XtParseTranslationTable(Translations[i].unparsed[mode]);
	    }
#ifndef MOTIF
	    XtSetArg(args[0], XtNtranslations, &table1);
	    XtGetValues(Translations[i].widget, args, XtNumber(args));
	    if (table1 == (XtTranslations) 0) {
		if (table) {
		    XtSetArg(args[0], XtNtranslations, table);
		    XtSetValues(Translations[i].widget, args, XtNumber(args));
		}
	    }
#endif /* MOTIF */
	    if (table) {
		XtOverrideTranslations(Translations[i].widget, table);
	    }
	    XtSetArg(args[0], XtNtranslations, &table1);
	    XtGetValues(Translations[i].widget, args, XtNumber(args));
	    Translations[i].tables[mode] = table1;
	    /* instead of the previous two lines:
	     * Translations[i].tables[mode] = table;
	     * however, this seems to lose bindings...
	     */
#ifdef MOTIF
	    xthHandleAllPendingEvents();
#endif
	}
	init[mode] = 1;
    } else {
	/*
	 * second and future times:
	 *   install translations
	 */
	for (i = 0; i < sizeof(Translations) / sizeof(struct _translations); i++) {
	    if (Translations[i].tables[mode]) {
#ifndef MOTIF
		XtSetArg(args[0], XtNtranslations,Translations[i].tables[mode]);
		XtSetValues(Translations[i].widget, args, XtNumber(args));
#else
	/*
	 * Work around motif text widget bug.. when a setvalues is done
	 * the text 'value' reverts to some previous value.
	 */
		if (Translations[i].widget == ArticleText) {
		    XtSetArg(targs[0], XtNtranslations,
			Translations[i].tables[mode]);
		    XtSetArg(targs[1], XmNvalue, "");
		    XtSetValues(Translations[i].widget, targs, XtNumber(targs));
		} else {
		    XtSetArg(args[0], XtNtranslations,
			Translations[i].tables[mode]);
		    XtSetValues(Translations[i].widget, args, XtNumber(args));
		}
#endif
	    }
	}
    }
    return;
}

static void
setTitleBar(message)
char *message;
{
#ifdef TITLEBAR
    Arg infoLineArg[1];
    char *p;
#ifdef MOTIF
    XmString labelString;
#else
    DwtCompString labelString;
#endif
    static char *modeTitle[] = {
	"Newsgroup mode - select News Group to read",
	"Article mode - select articles to read",
	"Add groups mode - decide processing for new newsgroups",
	"All groups mode - re-order newsgroups, select groups for subscribe/unsubscribe",
	"No mode"    };

    if (!message) message = modeTitle[Mode];
#ifdef MOTIF
    labelString = XmStringLtoRCreate(message, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(infoLineArg[0], XmNlabelString, labelString);
    XtSetValues(TitleBar, infoLineArg, XtNumber(infoLineArg));
    XmStringFree(labelString);
#else /* MOTIF */
    labelString = DwtLatin1String(message);
    XtSetArg(infoLineArg[0], DwtNlabel, labelString);
    XtSetValues(TitleBar, infoLineArg, XtNumber(infoLineArg));
    XtFree(labelString);
#endif /* MOTIF */
#endif /* TITLEBAR */
    return;
}

static void
swapMode()
/*
 * change the buttons displayed in the TopButtonBox (switch modes)
 */
{
    Arg resizeArg[3];
    Arg testArgs[1];
    Position y;
    Dimension min = 0;
    Dimension max = 0;
    static Dimension buttonHeight = 0;
    Arg labelArgs[1];

    if (PreviousMode == Mode) {
	return;
    }

    setTitleBar(NULL);
#ifdef MOTIF
    XtSetArg(resizeArg[0], XmNrefigureMode, False);
    XtSetValues(XtParent(TopButtonBox), resizeArg, 1);
    XtSetArg(resizeArg[0], XmNheight, &min);
#else
    XtSetArg(resizeArg[0], DwtNheight, &min);
#endif
    XtGetValues(TopButtonBox, resizeArg, 1);
    PaneSetMinMax(TopButtonBox, min, max);
    if (buttonHeight == 0) {
	if (ArtSpecButtonListCount > 0) {
#ifdef MOTIF
	    XtSetArg(resizeArg[0], XmNheight, &buttonHeight);
#else
	    XtSetArg(resizeArg[0], DwtNheight, &buttonHeight);
#endif
	    XtGetValues(ArtSpecButtons[0], resizeArg, 1);
	    buttonHeight += 2;	/* allow for margins */
	} else {
	    if (ArtButtonListCount > 0) {
#ifdef MOTIF
		XtSetArg(resizeArg[0], XmNheight, &buttonHeight);
#else
		XtSetArg(resizeArg[0], DwtNheight, &buttonHeight);
#endif
		XtGetValues(ArtButtons[0], resizeArg, 1);
		buttonHeight += 2;	/* allow for margins */
	    } else {
		buttonHeight = 10;
	    }
	}
	PaneSetMinMax(BottomButtonBox, buttonHeight + 6, buttonHeight + 6);
    }
    
    y = 0;
    /*
     * NONE -> ADD
     *    manage add in top box
     *    manage art in bottom box
     *    desensitize bottom box
     *    install add actions in top box
     */
    if ((PreviousMode == NO_MODE) && (Mode == ADD_MODE)) {
	XtManageChildren(AddButtons, AddButtonListCount);
	XtManageChildren(AddPopupButtons, AddPopupCount);
	XtManageChildren(ArtSpecButtons, ArtSpecButtonListCount);
	XtSetSensitive(BottomButtonBox, False);
	setTranslations(ADD_MODE);
	if (AddButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(AddButtons[AddButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(addMenus, XtNumber(addMenus));
	XtUnmanageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));

    /*    
     * NONE -> NG
     *    manage ng in top box
     *    manage art in bottom box
     *    desensitize bottom box
     *    install ng actions in top box
     */
    } else if ((PreviousMode == NO_MODE) && (Mode == NEWSGROUP_MODE)) {
	XtManageChildren(NgButtons, NgButtonListCount);
	XtManageChildren(NgPopupButtons, NgPopupCount);
	XtManageChildren(ArtSpecButtons, ArtSpecButtonListCount);
	XtSetSensitive(BottomButtonBox, False);
	setTranslations(NEWSGROUP_MODE);
	if (NgButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(NgButtons[NgButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
    /*
     * ADD -> NG
     *    unmanage add in top box
     *    manage ng in top box
     *    install ng actions in top box
     */
    } else if ((PreviousMode == ADD_MODE) && (Mode == NEWSGROUP_MODE)) {
	XtUnmanageChildren(AddButtons, AddButtonListCount);
	XtUnmanageChildren(AddPopupButtons, AddPopupCount);
	resetPopupSize();
	XtManageChildren(NgButtons, NgButtonListCount);
	XtManageChildren(NgPopupButtons, NgPopupCount);
	setTranslations(NEWSGROUP_MODE);
	if (NgButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(NgButtons[NgButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
    /*
     * ADD -> ART
     *    unmanage add in top box
     *    manage art in top box
     *    install art actions in top box
     */
    } else if ((PreviousMode == ADD_MODE) && (Mode == ARTICLE_MODE)) {
	XtUnmanageChildren(AddButtons, AddButtonListCount);
	XtUnmanageChildren(AddPopupButtons, AddPopupCount);
	resetPopupSize();
	XtManageChildren(ArtButtons, ArtButtonListCount);
	XtManageChildren(ArtPopupButtons, ArtPopupCount);
	setTranslations(ARTICLE_MODE);
	if (ArtButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(ArtButtons[ArtButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
	XtUnmanageChildren(ngMenus, XtNumber(ngMenus));
    /*
     * NG -> ART
     *    unmanage ng in top box
     *    manage art in top box
     *    sensitize bottom box
     *    install art actions in top box
     *    install art actions in bottom box
     */
    } else if ((PreviousMode == NEWSGROUP_MODE) && (Mode == ARTICLE_MODE)) {
	XtUnmanageChildren(NgButtons, NgButtonListCount);
	XtUnmanageChildren(NgPopupButtons, NgPopupCount);
	resetPopupSize();
	XtManageChildren(ArtButtons, ArtButtonListCount);
	XtManageChildren(ArtPopupButtons, ArtPopupCount);
	XtSetSensitive(BottomButtonBox, True);
	setTranslations(ARTICLE_MODE);
	if (ArtButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(ArtButtons[ArtButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
	XtUnmanageChildren(ngMenus, XtNumber(ngMenus));
    /*
     * NG -> ADD
     *    unmanage ng in top box
     *    manage add in top box
     *    install add actions in top box
     */
    } else if ((PreviousMode == NEWSGROUP_MODE) && (Mode == ADD_MODE)) {
	XtUnmanageChildren(NgButtons, NgButtonListCount);
	XtUnmanageChildren(NgPopupButtons, NgPopupCount);
	resetPopupSize();
	XtManageChildren(AddButtons, AddButtonListCount);
	XtManageChildren(AddPopupButtons, AddPopupCount);
	setTranslations(ADD_MODE);
	if (AddButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(AddButtons[AddButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(addMenus, XtNumber(addMenus));
	XtUnmanageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));
    /*
     * NG -> ALL
     *    desensitize top box
     *    unmanage art in bottom box
     *    manage all in bottom box
     *    sensitize bottom box
     *    install all actions in bottom box
     */
    } else if ((PreviousMode == NEWSGROUP_MODE) && (Mode == ALL_MODE)) {
	XtSetSensitive(BottomButtonBox, False);
	XtUnmanageChildren(NgButtons, NgButtonListCount);
	XtUnmanageChildren(NgPopupButtons, NgPopupCount);
	resetPopupSize();
	XtManageChildren(AllButtons, AllButtonListCount);
	XtManageChildren(AllPopupButtons, AllPopupCount);
	XtSetSensitive(TopButtonBox, True);
	setTranslations(ALL_MODE);
	if (AllButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(AllButtons[AllButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
    /*     
     * ART -> NG
     *    desensitize bottom box
     *    unmanage art in top box
     *    manage ng in top box
     *    install ng actions in top box
     */
    } else if ((PreviousMode == ARTICLE_MODE) && (Mode == NEWSGROUP_MODE)) {
	XtSetSensitive(BottomButtonBox, False);
	XtUnmanageChildren(ArtButtons, ArtButtonListCount);
	XtUnmanageChildren(ArtPopupButtons, ArtPopupCount);
	resetPopupSize();
	XtManageChildren(NgButtons, NgButtonListCount);
	XtManageChildren(NgPopupButtons, NgPopupCount);
	setTranslations(NEWSGROUP_MODE);
	if (NgButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(NgButtons[NgButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));
    /*
     * ALL -> NG
     *    sensitize top box
     *    unmanage all in bottom box
     *    manage art in bottom box
     *    desensitize bottom box
     */
    } else if ((PreviousMode == ALL_MODE) && (Mode == NEWSGROUP_MODE)) {
	XtSetSensitive(TopButtonBox, True);
	XtUnmanageChildren(AllButtons, AllButtonListCount);
	XtUnmanageChildren(AllPopupButtons, AllPopupCount);
	resetPopupSize();
	XtManageChildren(NgButtons, NgButtonListCount);
	XtManageChildren(NgPopupButtons, NgPopupCount);
	XtSetSensitive(BottomButtonBox, False);
	setTranslations(NEWSGROUP_MODE);
	if (NgButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(NgButtons[NgButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
    /*
     * ART -> ALL (going back to previous ALL_MODE)
     *    unmanage art in bottom box
     *    unmanage art in top box
     *    manage all in bottom box
     *    manage ng in top box
     *    desensitize top box
     *    install all actions in bottom box
     */
    } else if ((PreviousMode == ARTICLE_MODE) && (Mode == ALL_MODE)) {
	XtSetSensitive(BottomButtonBox, False);
	XtUnmanageChildren(ArtButtons, ArtButtonListCount);
	XtUnmanageChildren(ArtPopupButtons, ArtPopupCount);
	resetPopupSize();
	XtManageChildren(AllButtons, AllButtonListCount);
	XtManageChildren(AllPopupButtons, AllPopupCount);
	XtSetSensitive(TopButtonBox, True);
	setTranslations(ALL_MODE);
	if (AllButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(AllButtons[AllButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
    /*	
     * ALL -> ART
     *    unmanage ng in top box
     *    sensitize top box
     *    manage art in top box
     *    unmanage all in bottom box
     *    manage art in bottom box
     *    install art actions in bottom box
     */
    } else if ((PreviousMode == ALL_MODE) && (Mode == ARTICLE_MODE)) {
	XtSetSensitive(TopButtonBox, True);
	XtUnmanageChildren(AllButtons, AllButtonListCount);
	XtUnmanageChildren(AllPopupButtons, AllPopupCount);
	resetPopupSize();
	XtUnmanageChildren(NgButtons, NgButtonListCount);
	XtUnmanageChildren(NgPopupButtons, NgPopupCount);
	XtManageChildren(ArtButtons, ArtButtonListCount);
	XtManageChildren(ArtPopupButtons, ArtPopupCount);
	XtManageChildren(ArtSpecButtons, ArtSpecButtonListCount);
	XtSetSensitive(BottomButtonBox, True);
	setTranslations(ARTICLE_MODE);
	if (ArtButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(ArtButtons[ArtButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(artMenus, XtNumber(artMenus));
	XtUnmanageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(allMenus, XtNumber(allMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));

    /*     
     * ART -> ADD
     *    unmanage art in top box
     *    manage add in top box
     *    install add actions in top box
     */
    } else if ((PreviousMode == ARTICLE_MODE) && (Mode == ADD_MODE)) {
	XtSetSensitive(BottomButtonBox, False);
	XtUnmanageChildren(ArtButtons, ArtButtonListCount);
	XtUnmanageChildren(ArtPopupButtons, ArtPopupCount);
	resetPopupSize();
	XtManageChildren(AddButtons, AddButtonListCount);
	XtManageChildren(AddPopupButtons, AddPopupCount);
	setTranslations(ADD_MODE);
	if (AddButtonListCount > 0) {
	    XtSetArg(testArgs[0], XtNy, &y);
	    XtGetValues(AddButtons[AddButtonListCount-1], testArgs,
			XtNumber(testArgs));
	    min = max = y + buttonHeight + 6;
	} else {
	    min = 1; max = 1;
	}
	XtManageChildren(addMenus, XtNumber(addMenus));
	XtUnmanageChildren(ngMenus, XtNumber(ngMenus));
	XtUnmanageChildren(addMenus, XtNumber(addMenus));
	XtUnmanageChildren(artMenus, XtNumber(artMenus));

    } else {
	(void) sprintf(error_buffer, "unsupported transition: %d to %d\n",
			       PreviousMode, Mode);
	ehErrorExitXRN(error_buffer);
    }
    
#ifdef MOTIF
    if (min == 1 && max == 1) {
	SetButtonBoxSize(min, max);
    } else {
	if (max != 0 && min != 0) {
	    min = MAX(min-6, (buttonHeight + 3) * 2);
	    max = MAX(max-6, (buttonHeight + 3) * 2);
	    
	    SetButtonBoxSize(min ,max);
	}
    }
    XtSetArg(resizeArg[0], XmNrefigureMode, True);
    XtSetValues(XtParent(TopButtonBox), resizeArg, 1);
#else
    if (max != 0 && min != 0) {
	SetButtonBoxSize(min ,max);
    }
#endif
    return;
}


static int XRNAbort = 0;

int
abortP()
{
    xthHandleAllPendingEvents();
    return XRNAbort;
}

void
abortSet()
{
    XRNAbort = 1;
    return;
}

void
abortClear()
{
    XRNAbort = 0;
    return;
}

#ifdef MOTIF
Boolean XmTextPosToXY();
static Boolean ScrollDownOnePage(w)
Widget w;
{
    XmTextWidget widget = (XmTextWidget) w;
    int n;
    Position x, y;

    if (XmTextPosToXY(w, XmTextGetLastPosition(w), &x, &y))
	return TRUE;

#ifdef MOTIF_V10
    XmTextDisableRedisplay(widget, FALSE);
    n = XmTextNumLines(widget);
#else
    _XmTextDisableRedisplay(widget, FALSE);
    n = _XmTextNumLines(widget);
#endif
    if (n > 1) n--;
    XmTextScroll(w, n);
#ifdef MOTIF_V10
    XmTextEnableRedisplay(widget);
#else
    _XmTextEnableRedisplay(widget);
#endif
    return FALSE;
}
static Boolean ScrollUpOnePage(w)
Widget w;
{
    XmTextWidget widget = (XmTextWidget) w;
    int n;
    Position x,y;

    if (XmTextPosToXY(w, 0, &x, &y))
	return TRUE;

#ifdef MOTIF_V10
    XmTextDisableRedisplay(widget, FALSE);
    n = XmTextNumLines(widget);
#else
    _XmTextDisableRedisplay(widget, FALSE);
    n = _XmTextNumLines(widget);
#endif
    if (n > 1) n--;
    XmTextScroll(w, -n);
#ifdef MOTIF_V10
    XmTextEnableRedisplay(widget);
#else
    _XmTextEnableRedisplay(widget);
#endif
    return FALSE;
}
#else
extern Boolean DwtTextPosToXY();
static Boolean ScrollDownOnePage(w)
Widget w;
{
    int n = 0;
    int x, y;
    Arg rowArg[1];
    
    if (DwtTextPosToXY(w, DwtSTextGetLastPosition(w), &x, &y))
	return TRUE;

    XtSetArg(rowArg[0], DwtNrows, (XtArgVal) &n);
    XtGetValues(w, rowArg, 1);
    if (n > 1) n--;
    DwtTextDisableRedisplay(w, FALSE);
    (void) DwtTextScroll(w, n);
    DwtTextEnableRedisplay(w);
    return FALSE;
}
static Boolean ScrollUpOnePage(w)
Widget w;
{
    int n = 0;
    int x, y;
    Arg rowArg[1];
    
    if (DwtTextPosToXY(w, 0, &x, &y))
	return TRUE;

    XtSetArg(rowArg[0], DwtNrows, (XtArgVal) &n);
    XtGetValues(w, rowArg, 1);
    if (n > 1) n--;
    DwtTextDisableRedisplay(w, FALSE);
    (void) DwtTextScroll(w, -n);
    DwtTextEnableRedisplay(w);
    return FALSE;
}
#endif /* MOTIF */

/*ARGSUSED*/
static void
doTheRightThing(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    void ngReadFunction(), ngOpenFunction(), artScrollFunction();
    void artNextFunction(), artNextUnreadFunction(), artSubNextFunction();
    void allScrollFunction();

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    busyCursor();
    switch (Mode) {
	case ALL_MODE:
	allScrollFunction((Widget) 0);
	break;

	case NEWSGROUP_MODE:
	if (string && count && (*count == 1) && 
	    strcmp(string[0], "jump") == 0) {
	    NewsgroupEntryMode = XRN_JUMP;
	}
	if (app_resources.autoRead) {
	   ngReadFunction((Widget) 0);
	} else {
	   ngOpenFunction((Widget) 0);
	}
	break;

	case ARTICLE_MODE:
	if (event &&
	    (event->type == ButtonPress || event->type == ButtonRelease)) {
	    artNextFunction((Widget) 0);
	    break;
	}
	artScrollFunction((Widget) 0);
	break;
    }
    unbusyCursor();
    inCommand = 0;
    return;
}

/*
 *  Redraw the items between start and end in the subject list;
 *  Adjust the window so the cursor is between min and max lines.
 */
void
updateSubjectWidget(start, end, reset)
int	start;
int	end;
Boolean reset;
{
    int i;
    
    if (start < 0) start = 0;
    if (end > SubjectItems) end = SubjectItems;
    for (i=start; i<end+1; i++) {
	SListUpdateLine(TopList, i);
    }
    subjSetPos(TopList, start, reset);
    ArtPosition = start;
    return;
}

/*
 * Get the nearest article to the cursor.  If there is no article on the
 * current line, search forward or backwards for a valid article, depending
 * on the value of status.  Return the text and question of the
 * article obtained.
 */
static int
getNearbyArticle(status, text, question, artNum)
int status;
char **text, **question;
art_num *artNum;
{
    int beginning;
    
    if (status == art_PREV) {
	if (ArtPosition <= 0) {
	    /* no articles remain, jump out of article mode */
	    return art_DONE;
	}
	ArtPosition--;
	*artNum = ArticleNumbers[ArtPosition];
	gotoArticle(*artNum);
	subjSetPos(TopList, ArtPosition, True);
	while (getArticleText(text, question) != XRN_OKAY) {
	    if (ArtPosition > 0)
		ArtPosition--;
	    else
		return art_DONE;
	    *artNum = ArticleNumbers[ArtPosition];
	    gotoArticle(*artNum);
	    subjSetPos(TopList, ArtPosition, True);
	}
	return art_CHANGE;
    }

    if (status == art_NEXT) {
	ArtPosition++;
	if (ArtPosition >= SubjectItems) {
	    ArtPosition = SubjectItems - 1;
	    return art_DONE;
	}
	*artNum = ArticleNumbers[ArtPosition];
	gotoArticle(*artNum);
	subjSetPos(TopList, ArtPosition, True);
	while (getArticleText(text, question) != XRN_OKAY) {
	    ArtPosition++;
	    if (ArtPosition > SubjectItems) {
		return art_DONE;
	    }
	    *artNum = ArticleNumbers[ArtPosition];
	    gotoArticle(*artNum); 
	    subjSetPos(TopList, ArtPosition, True);
	}
	return art_CHANGE;
    }
    if (status == art_CURRENT) {
	if (ArtPosition < 0 || ArtPosition >= SubjectItems) {
	    ArtPosition = 0;
	    return art_DONE;
	}
	*artNum = ArticleNumbers[ArtPosition];
	gotoArticle(*artNum);
	while (getArticleText(text, question) != XRN_OKAY) {
	    ArtPosition++;
	    if (ArtPosition >= SubjectItems) {
		if (ArtPosition == 0) {
		    return art_DONE;
		}
		ArtPosition = 0;
	    }
	    *artNum = ArticleNumbers[ArtPosition];
	    gotoArticle(*artNum); 
	    subjSetPos(TopList, ArtPosition, True);
	}
	subjSetPos(TopList, ArtPosition, True);
	return art_CHANGE;
    }
    if (status == art_UNREAD) {
	if (ArtPosition < 0) ArtPosition = 0;
	if (ArtPosition >= SubjectItems) {
	    if (ArtPosition == 0) {
		return art_DONE;
	    }
	    ArtPosition = 0;
	}
	beginning = ArtPosition;
	if (articleRead(ArticleNumbers[ArtPosition])) {
	    ArtPosition++;
	    if (ArtPosition >= SubjectItems) {
		return art_DONE;
	    }
	    while (articleRead(ArticleNumbers[ArtPosition]) &&
		   ArtPosition != beginning) {
		ArtPosition++;
		if (ArtPosition >= SubjectItems) {
		    return art_DONE;
		}
	    }
	    if (ArtPosition == beginning) {
		return art_DONE;
	    }
	}
	/* we are at an unread article */
	*artNum = ArticleNumbers[ArtPosition];
	gotoArticle(*artNum);
	subjSetPos(TopList, ArtPosition, True);
	while (getArticleText(text, question) != XRN_OKAY) {
	    ArtPosition++;
	    if (ArtPosition >= SubjectItems) {
		if (ArtPosition == 0) {
		    return art_DONE;
		}
		ArtPosition = 0;
	    }
	    while (articleRead(ArticleNumbers[ArtPosition]) &&
		   ArtPosition != beginning) {
		ArtPosition++;
		if (ArtPosition >= SubjectItems) {
		    return art_DONE;
		}
	    }
	    if (ArtPosition == beginning) {
		return art_DONE;
	    }
	    *artNum = ArticleNumbers[ArtPosition];
	    gotoArticle(*artNum); 
	    subjSetPos(TopList, ArtPosition, True);
	}
	return art_CHANGE;
    }
    if (status == art_FINDUNREAD) {
	if (ArtPosition < 0) ArtPosition = 0;
	if (ArtPosition >= SubjectItems) {
	    if (ArtPosition == 0) {
		return art_DONE;
	    }
	    ArtPosition = 0;
	}
	beginning = ArtPosition;
	if (articleRead(ArticleNumbers[ArtPosition])) {
	    ArtPosition++;
	    if (ArtPosition >= SubjectItems) {
		return art_DONE;
	    }
	    while (articleRead(ArticleNumbers[ArtPosition]) &&
		   ArtPosition != beginning) {
		ArtPosition++;
		if (ArtPosition >= SubjectItems) {
		    return art_DONE;
		}
	    }
	    if (ArtPosition == beginning) {
		return art_DONE;
	    }
	}
	/* we are at an unread article */
	*artNum = ArticleNumbers[ArtPosition];
	gotoArticle(*artNum);
	subjSetPos(TopList, ArtPosition, True);
	return art_CHANGE;
    }
    return art_CHANGE;
}


#define CHANGE 0		/* subject window has changed */
#define NOCHANGE 1		/* subject window has not changed */
#define DONE 2			/* no new article was found */
				/* EXIT is already defined, it implies */
				/* there are no articles left at all */

static int
isPrevSubject(subject, text, question, artNum)
char *subject;
char **text, **question;
art_num *artNum;
{
    char *newsubject;
    char *newLine;
    int save;
    int num;
    char statBuf[80];

    save = ArtPosition;
    startSearch();
    abortClear();
    
    for (;;) {
	if (abortP()) {
	    failedSearch();
	    ArtPosition = save;
	    return ABORT;
	}
	if (ArtPosition >= SubjectItems) {
	    ArtPosition = SubjectItems - 1;
	}
	if (ArtPosition > 0) {
	    ArtPosition--;
	    *artNum = ArticleNumbers[ArtPosition];
	    newsubject = getSubject(*artNum);
	    gotoArticle(*artNum);
	    if (utSubjectCompare(newsubject, subject) == 0) {
		gotoArticle(*artNum);
		subjSetPos(TopList, ArtPosition, True);
		if (getArticleText(text, question) != XRN_OKAY) {
		    if (ArtPosition > 0) ArtPosition--;
		    continue;
		}
		if (articleMarkedUnread(*artNum)) {
		    markArticleAsUnread(*artNum);
		}
		return NOCHANGE;
	    }
	    continue;
	} else {
	    gotoFirstArticle();
	    if ((newLine = getPrevSubject()) == NIL(char)) {
		failedSearch();
		ArtPosition = save;
		return DONE;
	    }
	    newLine[0] = '+';
	    num = atol(&newLine[2]);
	    *artNum = num;
	    if ((num % 100) == 0) {
		sprintf(statBuf, "Now searching at article %d", num);
		infoNow(statBuf);
	    }
	    newsubject = getSubject(*artNum);
	    if (utSubjectCompare(newsubject, subject) == 0) {
		/* found a match, go with it*/
		gotoArticle(*artNum);
		subjSetPos(TopList, ArtPosition, True);
		(void) getArticleText(text, question);
		return CHANGE;
	    }
	    continue;
	}
    }
}

static int
isNextSubject(subject, text, question, artNum)
char *subject;
char **text, **question;
art_num *artNum;
{
    char *newsubject;
    int save = ArtPosition;

    abortClear();
    
    if (ArtPosition != 0 ||
	(SubjectItems > 0 && articleRead(ArticleNumbers[ArtPosition]))) {
	ArtPosition++;
    }
    for (;;) {

	if (abortP()) {
	    failedSearch();
	    ArtPosition = save;
	    return ABORT;
	}
	if (ArtPosition >= SubjectItems) {
	    ArtPosition = save;
	    return DONE;
	}
	*artNum = ArticleNumbers[ArtPosition];
	newsubject = getSubject(*artNum);
	if (utSubjectCompare(newsubject, subject) == 0) {
	    gotoArticle(*artNum);
	    subjSetPos(TopList, ArtPosition, True);
	    if (getArticleText(text, question) != XRN_OKAY) {
		if (ArtPosition > 0) ArtPosition--;
	    }
	    return NOCHANGE;
	} else {
	    ArtPosition++;
	}
    }
}


static int
getPrevious(artNum)
art_num *artNum;
/*
 * 
 */
{
    if (getPrevSubject()) {
	resetSubjectWidget(ALL, False);
	ArtPosition = 0;
	*artNum = ArticleNumbers[ArtPosition];
	return TRUE;
    }
    return FALSE;
}

static void
redrawAllWidget(position)
int position;
/*
 * Redraw the all groups window, assuming it has changed
 */
{
    if (Mode != ALL_MODE) {
	return;
    }

    if (AllGroupItems > 0) {
	if (AllGroups != (ng_num *)0) {
	    FREE(AllGroups);
	}
    }
    AllGroups = getStatusList(AllStatus);

    for (AllGroupItems = 0; AllGroups[AllGroupItems] != -1; AllGroupItems++);

    SListLoad(TopList, AllGroupItems);
    currentTop = 0;
    resetSelection();
    subjSetPos(TopList, position, True);
    saveTop = position;
    return;
}


#ifdef WATCH
static int watchcount = -1;
static char **GroupList;
static char *GroupListString = 0;

Boolean   watchingGroup()
{

    char *p;
    int i, j;

    /* if resource not defined or empty */
    if (!app_resources.watchList || (watchcount == -1 && GroupListString))
	return True;
    if (GroupItems == 0)
	return False;
    if (watchcount == -1){
	char *q;
	char * strpbrk();
	int maxcount;

	GroupListString = XtNewString(app_resources.watchList);
	q = GroupListString;
	watchcount = 0;
	while(1){
	    /* skip white space */
	    q = q + strspn(q," \t");
	    if( *q == '\0')
		break;
	    ++watchcount;
	    /* find end of token */
	    if((q = strpbrk(q," \t")) == 0)
		break;
	}
	GroupList = ARRAYALLOC(char *, watchcount);
	q = GroupListString;
	watchcount = -1;
	while(1){
	    /* skip white space */
	    q = q + strspn(q," \t");
	    if( *q == '\0')
		break;
	    GroupList[++watchcount] = q;
	    /* find end of token */
	    if((q = strpbrk(q," \t")) == 0)
		break;
	    *q++ = '\0';
	}

    }	
    for(i = 0; i <= watchcount; i++) {
	char *q;

	for (j = 0; j < GroupItems; j++) {
	    if (strcmp(GroupList[i], groupName(NewsGroups[j])) == 0) {
		return True;
	    }
	}
    }
    return False;
}
#endif

/*
 * Rebuild the newsgroup list window.
 * Find out what groups have articles to read and build up the string.
 * Display it in the list widget.
 */
static void
redrawNewsgroupListWidget()
{
    int i;

    if (Mode != NEWSGROUP_MODE) {
	return;
    }

    if (NewsGroups != NIL(ng_num)) {
	FREE(NewsGroups);
	NewsGroups = (ng_num *)0;
    }

    NewsGroups = unreadGroups(NewsgroupDisplayMode, &unreadNews);
    
    GroupItems = 0;
    for (i=0;NewsGroups[i] >= 0;i++) {
	GroupItems++;
    }

    /* update the info line */

    if (!unreadNews) {
	setTopInfoLine("No more unread articles in the subscribed to newsgroups");
	if (XtIsRealized(TopLevel))
	    xmSetIconAndName(ReadIcon);
    } else {
	if (XtIsRealized(TopLevel)) {
#ifdef WATCH
	    if(watchingGroup())
#endif
		xmSetIconAndName(UnreadIcon);
#ifdef WATCH
	    else
		xmSetIconAndName(ReadIcon);
#endif
	}
    }

    if (GroupItems > 0) {
	if (GroupPosition >= GroupItems) GroupPosition = 0;
	if (NewsGroups[GroupPosition] == -1) {
	    GroupPosition = 0;
	}
	if (STREQ(groupName(NewsGroups[GroupPosition]), LastGroup)) {
	    GroupPosition++;
	    if (GroupPosition >= GroupItems)
		GroupPosition = 0;
	}
    } else {
	setTopInfoLine("No more unread articles in the subscribed to newsgroups");
    }
    
    SListLoad(TopList, GroupItems);
    currentTop = 0;
    subjSetPos(TopList, GroupPosition, True);
    SelectionItem = -1;
    SelectionCount = 0;
    selCount = 0;
    return;
}

/*
 * update the info line and update the newsgroup list window
 */
static void
updateNewsgroupMode()
{
    if (PreviousMode != NEWSGROUP_MODE) {
	setTopInfoLine("Questions apply to current selection or cursor position");
    }
    redrawNewsgroupListWidget();
    subjSetPos(TopList, GroupPosition, True);
    SelectionItem = -1;
    SelectionCount = 0;
    selCount = 0;
    return;
}

/*
 * install the newsgroup mode buttons (and the delete the previous mode buttons)
 * and then go to newsgroup mode
 */
static void
switchToNewsgroupMode()
{
    PreviousMode = Mode;
    Mode = NEWSGROUP_MODE;
    LastRegexp = NIL(char);

    selCount = 0;
    SelectionItem = -1;

    /* switch buttons */
    swapMode();
    addTimeOut();

    /* update the newsgroup mode windows */
    updateNewsgroupMode();
    
    return;
}

/*
 * install the article mode buttons (and delete the previous mode buttons),
 * build the subject line screen, and call ARTICLE_MODE function 'next unread'
 */
static int
switchToArticleMode(readFlag)
int readFlag;
{
    int	    oldMode;
    /* change the list window */

    if (ArticleNumbers != NIL(art_num)) {
	FREE(ArticleNumbers);
    }

    ArticleNumbers = getArticleNumbers(UNREAD, &SubjectItems);

    ArtPosition = 0;
    oldListed = False;
    if (SubjectItems == 0) {
/*
 *  No articles. report bogus group and
 *  redetermine mode
 */
	bogusNewsgroup();
	SubjectItems = 0;
	determineMode();
	return BAD_GROUP;
    }

    /* get rid of previous groups save file string */
    if (app_resources.resetSave) {
	if (app_resources.saveString == NULL) {
	    app_resources.saveString = "";
	}
	if (SaveString) {
	    XtFree(SaveString);
	}
	SaveString = XtNewString(app_resources.saveString);
    }

    oldMode = PreviousMode;
    PreviousMode = Mode;
    Mode = ARTICLE_MODE;

    /* switch buttons */
    swapMode();

    /*
     * "What's the purpose of this?"  you're saying.  "It looks wrong."
     * Well, try taking out any mention of oldMode in this function,
     * then recompile and do the following:
     * 1. Enter article mode
     * 2. Clock "Next Newsgroup".
     * 3. Click "Quit".
     * You will be returned to all mode instead of article mode.
     * Therefore, we've got to keep track in this function of whether
     * we're switching from article mode to article mode, and if so,
     * set PreviousMode truly in order for swapMode to work, but once
     * that's done, we can put it back to what it was before).
     */
    if (PreviousMode == ARTICLE_MODE) {
	PreviousMode = oldMode;
    }
    setTopInfoLine("Questions apply to current selections or cursor position");

    /* get and display the article */
    if (ArticleNumbers == (art_num *) 0) {
	ArticleNumbers = (art_num *) XtMalloc(512);
    }
    ArtPosition = 0;
    SListLoad(TopList, SubjectItems);
    setBottomInfoLine(openQuestion());
    currentTop = 0;
    updateSubjectWidget(0, 0, True);
    ArtEntry = 1;
    switch (readFlag) {
	case 1:
	artNextUnreadFunction((Widget) 0);
	break;
	case 2:
	markArticleAsRead(ArticleNumbers[0]);
	case 0:
	SListSelectAll(TopList, 0);
	break;
    }
    if (app_resources.twoWindows) {
	XtManageChildren(ArtWinPopupButtons, ArtWinPopupCount);
    }
    return GOOD_GROUP;
}

static void
resetSubjectWidget(artstatus, emptyOK)
/*
 * rebuild the subject list widget
 */
    int artstatus;
    Boolean emptyOK;
{
    int 	i;
    /* change the list window */

    if (ArticleNumbers != NIL(art_num)) {
	FREE(ArticleNumbers);
    }

    ArticleNumbers = getArticleNumbers(artstatus, &SubjectItems);

    if (SubjectItems == 0) {
	if (!emptyOK)
	    bogusNewsgroup();
	SubjectItems = 0;
	return;
    }

    SListLoad(TopList, SubjectItems);
    currentTop = 0;

    if (ArtPosition >= SubjectItems)
	ArtPosition = SubjectItems - 1;
    return;
}

static void
updateAllWidget(left, right)
int left, right;
{
    while (left <= right) {
	SListUpdateLine(TopList, left);
	left++;
    }
    return;
}

/*
 * If the article to be displayed has changed, update the article
 * window and redraw the mode line
 */
static void
redrawArticleWidget(text, question)
char *text, *question;
{
#ifdef MOTIF
    char *wrapped;
    extern char *wrapString();

    if (app_resources.autoWrap) {
	wrapped = wrapString(text);
#ifdef MOTIF_V10
	XmTextDisableRedisplay(ArticleText, FALSE);
	XmTextSetString(ArticleText, wrapped);
	XmTextEnableRedisplay(ArticleText);
#else
	_XmTextDisableRedisplay((XmTextWidget) ArticleText, FALSE);
	XmTextSetString(ArticleText, wrapped);
	_XmTextEnableRedisplay((XmTextWidget) ArticleText);
	XmTextSetCursorPosition(ArticleText, 0);
#endif
	XmTextSetString(ArticleText, wrapped);
	XtFree(wrapped);
    } else {
#ifdef MOTIF_V10
	XmTextDisableRedisplay(ArticleText, FALSE);
	XmTextSetString(ArticleText, text);
	XmTextEnableRedisplay(ArticleText);
#else
	_XmTextDisableRedisplay((XmTextWidget) ArticleText, FALSE);
	XmTextSetString(ArticleText, text);
	_XmTextEnableRedisplay((XmTextWidget) ArticleText);
	XmTextSetCursorPosition(ArticleText, 0);
#endif
	XmTextSetString(ArticleText, text);
    }
    XmTextSetEditable(ArticleText, FALSE);
#else
    DwtSTextSetString(ArticleText, text);
    DwtSTextSetEditable(ArticleText, FALSE);
#endif
	
    setBottomInfoLine(question);
    if (app_resources.twoWindows) {
	XtPopup(ArtTopLevel, XtGrabNone);
	ArtPoppedUp = True;
    }
    return;
}

/*
 * release the storage associated with article mode
 * and go to newsgroup mode
 */
static void
exitArticleMode()
{
    int i;
    PrevArticle = CurrentArticle = 0;
    
    /* clear the article window */

#ifdef MOTIF
    XmTextSetString(ArticleText, NULL);
    XmTextSetEditable(ArticleText, FALSE);
#else
    DwtSTextSetString(ArticleText, "");
    DwtSTextSetEditable(ArticleText, FALSE);
#endif

    setBottomInfoLine("");
    SListLoad(TopList, 0);
    if (ArticleNumbers != NIL(art_num)) {
	FREE(ArticleNumbers);
	ArticleNumbers = NIL(art_num);
    }

    releaseNewsgroupResources();
    if (app_resources.updateNewsrc == TRUE) {
	while (!updatenewsrc())
	    (void) ehErrorRetryXRN("Can not update .newsrc file", True);
    }

    if (app_resources.twoWindows && ArtPoppedUp) {
	xthUnmapWidget(ArtTopLevel);
	XtPopdown(ArtTopLevel);
	ArtPoppedUp = False;
    }
    if (PreviousMode == NEWSGROUP_MODE) {
	switchToNewsgroupMode();
    } else {
	switchToAllMode();
    }
    
    return;
}
    
/*
 * release storage associated with add mode and go to newsgroup mode
 */
static void
exitAddMode()
{
    int i;

    SListLoad(TopList, 0);
    if (AddGroups) {
	FREE(AddGroups);
	AddGroups = (struct newsgroup **) 0;
    }

    AddGroupItems = 0;
    switchToNewsgroupMode();
    
    return;
}

/*
 * update the add mode text window to correspond to the new set of groups
 */
static void
redrawAddListWidget(insertPoint)
int insertPoint;
{
    int i;    

    if (AddGroups != (struct newsgroup **) 0) {
	FREE(AddGroups);
	AddGroups = (struct newsgroup **) 0;
    }
    AddGroupItems = 0;
    AddGroups = newGroups();
    if (AddGroups == NULL)
	return;

    for (AddGroupItems=0;
	AddGroups[AddGroupItems] != NULL;
	AddGroupItems++);

    SListLoad(TopList, AddGroupItems);
    currentTop = 0;
    resetSelection();
    subjSetPos(TopList,insertPoint, True);
    AddGroupPosition = insertPoint;
    if (XtIsRealized(TopLevel)) {
#ifdef WATCH
	if(watchingGroup())
#endif
	    xmSetIconAndName(UnreadIcon);
#ifdef WATCH
	else
	    xmSetIconAndName(ReadIcon);
#endif
    }
    return;
}

/*
 * Display new article, mark as read.
 */
void
foundArticle(file, ques, artNum)
char *file, *ques;
art_num artNum;
{
    int curArt;

    PrevArticle = CurrentArticle;
    curArt = ArticleNumbers[ArtPosition];
    if (curArt != artNum) {	/* not current pos; have to find it */
	for (ArtPosition = 0; ArtPosition < SubjectItems; ArtPosition++) {
	    curArt = ArticleNumbers[ArtPosition];
	    if (curArt == artNum) break;
	}
	if (curArt != artNum) {	/* not in current screen, go get 'em all */
	    resetSubjectWidget(ALL, False);
	    for (ArtPosition = 0; ArtPosition < SubjectItems; ArtPosition++) {
		curArt = ArticleNumbers[ArtPosition];
		if (curArt == artNum) break;
	    }
	}
    }
    if (curArt != artNum) ArtPosition = 0;
    if (articleMarkedUnread(ArticleNumbers[ArtPosition])) {
	markArticleAsRead(ArticleNumbers[ArtPosition]);
    } 
    CurrentArticle = ArticleNumbers[ArtPosition];
    markArticleAsRead(CurrentArticle);

    resetSelection();
    updateSubjectWidget(ArtPosition, ArtPosition, True);
    subjSetPos(TopList, ArtPosition, True);
    redrawArticleWidget(file, ques);
    
    return;
}

/*
 * used when the user has elected to catch
 * up newsgroups in newsgroup mode
 */
static void
catchUpNG()
{
    int first;
    int savePosition = GroupPosition;
    int i;
    
    if (SelectionItem < 0) {
	first = SListGetHighlight(TopList);
	resetSelection();
	if (first < 0) return;
	if (first >= GroupItems) {
	    first = GroupItems - 1;
	}
	GroupPosition = first;
	if (gotoNewsgroup(groupName(NewsGroups[GroupPosition])) == GOOD_GROUP) {
	    catchUp();
	}
	updateNewsgroupMode();
	return;
    } else {
	for (i = 0; i < GroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		if (gotoNewsgroup(groupName(NewsGroups[i])) == GOOD_GROUP) {
		    catchUp();
		}
	    }
	}
	resetSelection();
    }
    GroupPosition = savePosition;
    if (GroupPosition >= GroupItems) {
	GroupPosition = GroupItems - 1;
    }
    updateNewsgroupMode();
    return;
}


#define XRNunsub_ABORT 0
#define XRNunsub_OK 1
static char UnsubGroup[GROUP_NAME_SIZE];
/*ARGSUSED*/
static void
unsubscribeHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    PopDownDialog(UnSubConfirmBox);
    busyCursor();
    if ((int) client_data == XRNunsub_OK) {
	if (gotoNewsgroup(UnsubGroup) == GOOD_GROUP) {
	    unsubscribe();
	    updateNewsgroupMode();
	}
    }
    subjSetPos(TopList,GroupPosition, True);
    unbusyCursor();
    inCommand = 0;
    return;
}

/*
 * Unsubscribe user from selected group(s)
 */
static void
unsubscribeNG()
{
    static struct DialogArg args[] = {
	{"Unsubscribe",	unsubscribeHandler, (caddr_t) XRNunsub_OK},
	{"Cancel",	unsubscribeHandler, (caddr_t) XRNunsub_ABORT},
    };

    if (app_resources.confirmMode & NG_UNSUBSCRIBE) {
	if (UnSubConfirmBox == (Widget) 0) {
	    UnSubConfirmBox = CreateDialog(TopLevel, "Confirm Unsubscribe",
					   "OK to unsubscribe?",
					   DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(UnSubConfirmBox);
	return;
    }
    if (gotoNewsgroup(UnsubGroup) == GOOD_GROUP) {
	unsubscribe();
	updateNewsgroupMode();
    }
    return;
}

/*
 * Catch up group, and exit article mode
 */
static void
catchUpART()
{
    if (SelectionItem > 0) {
	catchUpPartART();
	return;
    }
    catchUp();
    exitArticleMode();
    return;   
}

/*
 * Mark all articles between 0 and ArtPosition as read.
 * Get the next unread article and display it, quit
 * if there are no more unread articles.
 */
static void
catchUpPartART()
{
    char *text, *question;
    art_num artNum;
    int i, last;
    int save, top;
    if (SelectionItem > 0) {
	ArtPosition = SelectionItem;
    } else {
	ArtPosition = SListGetHighlight(TopList);
    }
    if (ArtPosition >= SubjectItems) {
	ArtPosition = SubjectItems - 1;
    }
    last = ArtPosition;
    if ((top = SListGetTop(TopList)) == -1) top = 0;
    save = ArtPosition;
    for (i = 0; i < last; i++) {
	if (!articleRead(ArticleNumbers[i])) {
	    markArticleAsRead(ArticleNumbers[i]);
	    if (app_resources.watchProgress) {
		updateSubjectWidget(i, i, False);
	    }
	}
    }
    updateSubjectWidget(0, last, True);
    ArtPosition = save;
    SListSetTop(TopList, top);
    if (app_resources.autoRead) {
	if (getNearbyArticle(art_UNREAD, &text, &question, &artNum) == 
		art_DONE) {
	    exitArticleMode();
	    return;
	}
	foundArticle(text, question, artNum);
    } else {
	if (getNearbyArticle(art_FINDUNREAD, &text, &question, &artNum) == 
		art_DONE) {
	    return;
	}
	setBottomInfoLine(currentQuestion());
    }	
    return;
}

static void
fedUpART()
{
    void artQuitFunction(), artNextGroupFunction();
    PrevArticle = CurrentArticle = 0;

    resetSelection();
    catchUp();
    if (NewsgroupDisplayMode) {
	artQuitFunction((Widget) 0);
    } else {
	artNextGroupFunction((Widget) 0);
    }
    return;
}

/*
 * Unsubscribe user from the current group;
 * exit article mode
 */
static void
unsubscribeART()
{
    unsubscribe();
    exitArticleMode();
    
    return;
}


static Widget ExitConfirmBox = (Widget) 0;
static Widget CatchUpConfirmBox = (Widget) 0;
static Widget PartCatchUpConfirmBox = (Widget) 0;
static Widget FedUpConfirmBox = (Widget) 0;
static Widget EndActionBox = (Widget) 0;

/*ARGSUSED*/
static void
generalHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    busyCursor();
    switch(Action) {
	case NG_EXIT:
	PopDownDialog(ExitConfirmBox);
	if ((int) client_data == XRN_YES) {
	    ehNoUpdateExitXRN();
	}
	break;

	case NG_QUIT:
	PopDownDialog(ExitConfirmBox);
	if ((int) client_data == XRN_YES) {
	    ehCleanExitXRN();
	}
    	break;

	case NG_CATCHUP:
	PopDownDialog(CatchUpConfirmBox);
	if ((int) client_data == XRN_YES) {
	    catchUpNG();
	}
    	break;

	case ART_CATCHUP:
	PopDownDialog(CatchUpConfirmBox);
	if ((int) client_data == XRN_YES) {
	    catchUpART();
	}	    
	break;

	case ART_PART_CATCHUP:
	PopDownDialog(PartCatchUpConfirmBox);
	if ((int) client_data == XRN_YES) {
	    catchUpPartART();
	}
	break;

	case ART_UNSUBSCRIBE:
	PopDownDialog(UnSubConfirmBox);
	if ((int) client_data == XRN_YES) {
	    unsubscribeART();
	}
	break;

	case ART_FEDUP:
	PopDownDialog(FedUpConfirmBox);
	if ((int) client_data == XRN_YES) {
	    fedUpART();
	}
	break;
    }
    unbusyCursor();
    inCommand = 0;
    return;
}

/*
 * called when the user wants to quit xrn
 *
 *  full update the newsrc file
 *  exit
 */
/*ARGSUSED*/
void
ngQuitFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Yes",	generalHandler, (caddr_t) XRN_YES},
	{"No",	generalHandler, (caddr_t) XRN_NO},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    if (app_resources.confirmMode & NG_QUIT) {
	Action = NG_QUIT;
	if (ExitConfirmBox == (Widget) 0) {
	    ExitConfirmBox = CreateDialog(TopLevel, "Confirm Quit",
					   "Are you sure?",
					   DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(ExitConfirmBox);
	return;
    }
    ehCleanExitXRN();
}

/*
 * called when the user wants to read a new newsgroup
 *
 * get the selected group, set the internal pointers, and go to article mode
 *
 */
/*ARGSUSED*/
void
ngReadFunction(widget)
Widget widget;
{
    char name[GROUP_NAME_SIZE];
    int 	status;

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    SListSelectAll(TopList, 0);
    if (SelectionItem < 0) {
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem < 0) SelectionItem = 0;
	SelectionCount = 1;
    }
    if (SelectionItem >= GroupItems) {
	resetSelection();
	return;
    }
    GroupPosition = SelectionItem;
    resetSelection();
    if (GroupPosition >= 0) {
	strcpy(name, groupName(NewsGroups[GroupPosition]));
	setTitleBar("Opening newsgroup - please wait");
	if (NewsgroupEntryMode == XRN_GOTO) {
	    status = gotoNewsgroupForRead(name);
	} else {
	    status = jumpToNewsgroup(name);
	    NewsgroupEntryMode = XRN_GOTO;
	}
	if (status == GOOD_GROUP || status == GOOD_GROUP_MARKED) {
	    if (getNewsgroup() == XRN_NOMORE) {
		mesgPane(XRN_SERIOUS, "Can not get the group, it may have been deleted");
		return;
	    }
	    (void) strcpy(LastGroup, name);
	    (void) switchToArticleMode(1);
	}
    } else {
	GroupPosition = 0;
    }
    subjSetPos(TopList, 0, True);
    return;
}

/*
 * called when the user wants to open a new newsgroup
 *
 * get the selected group, set the internal pointers, and go to article mode
 *
 */
/*ARGSUSED*/
void
ngOpenFunction(widget)
Widget widget;
{
    char name[GROUP_NAME_SIZE];
    int 	status;

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    SListSelectAll(TopList, 0);
    if (SelectionItem < 0) {
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem < 0) SelectionItem = 0;
	SelectionCount = 1;
    }
    if (SelectionItem >= GroupItems) {
	resetSelection();
	return;
    }
    GroupPosition = SelectionItem;
    resetSelection();
    if (GroupPosition >= 0) {
	strcpy(name, groupName(NewsGroups[GroupPosition]));
	setTitleBar("Opening newsgroup - please wait");
	if (NewsgroupEntryMode == XRN_GOTO) {
	    status = gotoNewsgroupForRead(name);
	} else {
	    status = jumpToNewsgroup(name);
	    NewsgroupEntryMode = XRN_GOTO;
	}
	if (status == GOOD_GROUP || status == GOOD_GROUP_MARKED) {
	    if (getNewsgroup() == XRN_NOMORE) {
		mesgPane(XRN_SERIOUS, "Can not get the group, it may have been deleted");
		return;
	    }
	    (void) strcpy(LastGroup, name);
	    (void) switchToArticleMode((status == GOOD_GROUP_MARKED) ? 2 : 0);
	}
    } else {
	GroupPosition = 0;
    }
    subjSetPos(TopList, 0, True);
    SListSelectAll(TopList, 0);
    return;
}

/*
 * called when the user does not want to read a newsgroup
 *
 * if selected group, set internal group
 * call updateNewsgroupMode
 */
/*ARGSUSED*/
void
ngNextFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    GroupPosition++;
    if (GroupPosition >= GroupItems) {
	GroupPosition = 0;
    }
    subjSetPos(TopList, GroupPosition, True);
    return;
}

/*
 * called when the user wants to move the cursor up in
 * the newsgroup window
 *
 * if selected group, set internal group
 * call updateNewsgroupMode
 */
/*ARGSUSED*/
void
ngPrevFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    
    GroupPosition--;
    if (GroupPosition < 0) {
	GroupPosition = GroupItems - 1;
    }
    subjSetPos(TopList, GroupPosition, True);
    return;
}

/*
 * called to catch up on all unread articles in this newsgroup
 * use a confirmation box if the user has requested it
 * if selected group, set internal group
 */
/*ARGSUSED*/
void
ngCatchUpFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Yes",	generalHandler, (caddr_t) XRN_YES},
	{"No",	generalHandler, (caddr_t) XRN_NO},
    };

    if (GroupItems <= 0) return;
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    
    if (app_resources.confirmMode & NG_CATCHUP) {
	Action = NG_CATCHUP;
	if (CatchUpConfirmBox == (Widget) 0) {
	    CatchUpConfirmBox = CreateDialog(TopLevel, "Confirm Catch Up",
					     "OK to catch up?",
					     DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(CatchUpConfirmBox);

	return;
    }
    catchUpNG();

    return;
}

/*
 * called to unsubscribe to a newsgroup
 *
 * if selected group, set internal group
 * do internals
 * call updateNewsgroupMode
 */
/*ARGSUSED*/
void
ngUnsubFunction(widget)
Widget widget;
{
    int i;
    int savePosition = GroupPosition;
    char *curGroup;

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    if (GroupItems <= 0) {
	mesgPane(XRN_SERIOUS, "Please select a Group to unsubscribe from");
	return;
    }
    if (SelectionItem < 0) {
	i = SListGetHighlight(TopList);
	strcpy(UnsubGroup, groupName(NewsGroups[i]));
	unsubscribeNG();
    } else {
	for (i=0;i<GroupItems;i++) {
	    if (SListIsSelected(TopList, i)) {
		strcpy(UnsubGroup, groupName(NewsGroups[i]));
		unsubscribeNG();
		return;
	    }
	}
	resetSelection();
    }
    GroupPosition = savePosition;
    if (GroupPosition >= GroupItems) {
	GroupPosition = GroupItems - 1;
    }
    resetSelection();    
    updateNewsgroupMode();

    return;
}


#define XRNsub_ABORT 0
#define XRNsub_LASTGROUP 1
#define XRNsub_FIRST 2
#define XRNsub_LAST 3
#define XRNsub_CURRENT 4

static Widget SubscribeBox = (Widget) 0;

/*ARGSUSED*/
static void
subscribeHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    int status = SUBSCRIBE;
    char name[GROUP_NAME_SIZE];

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    PopDownDialog(SubscribeBox);
    busyCursor();
    switch ((int) client_data) {
	case XRNsub_LASTGROUP:
	if (LastGroup[0] != '\0') {
	    if (gotoNewsgroup(LastGroup) != GOOD_GROUP) {
		mesgPane(XRN_SERIOUS, "Bad Group");
		unbusyCursor();
		inCommand = 0;
		return;
	    }
	    subscribe();
	    updateNewsgroupMode();
	}
	break;

	case XRNsub_FIRST:
	if (addToNewsrcBeginning(GetDialogValue(SubscribeBox),
				 status) == GOOD_GROUP) {
	    GroupPosition = 0;
	    updateNewsgroupMode();
	}
	break;

	case XRNsub_LAST:
	if (addToNewsrcEnd(GetDialogValue(SubscribeBox),
			   status) == GOOD_GROUP) {
	    updateNewsgroupMode();
 	    subjSetPos(TopList, GroupPosition, True);
	}
	break;

	case XRNsub_CURRENT:
	if (GroupItems == 0) {
	    if (addToNewsrcEnd(GetDialogValue(SubscribeBox),
				status) == GOOD_GROUP) {
		updateNewsgroupMode();
		subjSetPos(TopList, GroupPosition, True);
	    }
	    break;
	}
	if (GroupPosition < 0) GroupPosition = 0;
	if (GroupPosition >= GroupItems) GroupPosition = GroupItems - 1;
	if (NewsGroups[GroupPosition] == -1) {
	    if (addToNewsrcEnd(GetDialogValue(SubscribeBox),
			       status) == GOOD_GROUP) {
		updateNewsgroupMode();
 	    	subjSetPos(TopList, GroupPosition, True);
	    }
	} else {

	    /* don't need to check for the null group here, it would have */
	    /* been already handled above */
	    if (GroupPosition == 0) {
		if (addToNewsrcBeginning(GetDialogValue(
		    SubscribeBox),status) == GOOD_GROUP) {
		    updateNewsgroupMode();
		}
	    } else {
		GroupPosition--;
		strcpy(name, groupName(NewsGroups[GroupPosition]));
		if (addToNewsrcAfterGroup(GetDialogValue(
			SubscribeBox), name, status) == GOOD_GROUP) {
		    updateNewsgroupMode();
		}
	    }
    	}
	break;
    }
    subjSetPos(TopList,GroupPosition, True);
    unbusyCursor();
    inCommand = 0;
    return;
}

/*
 * Subscribe to a group currently unsubscribed to
 */
/*ARGSUSED*/
void
ngSubscribeFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Cursor Position",	subscribeHandler, (caddr_t) XRNsub_CURRENT},
	{"Last Group",		subscribeHandler, (caddr_t) XRNsub_LASTGROUP},
	{"Cancel",		subscribeHandler, (caddr_t) XRNsub_ABORT},
	{"  First ",		subscribeHandler, (caddr_t) XRNsub_FIRST},
	{"  Last  ",		subscribeHandler, (caddr_t) XRNsub_LAST},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    
    if (SubscribeBox == (Widget) 0) {
	SubscribeBox = CreateDialog(TopLevel, "Subscribe Select",
				   "Group to subscribe to:",
				    DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(SubscribeBox);

    return;
}

#define XRNgoto_ABORT 0
#define XRNgoto_GOTO 1

static Widget GotoNewsgroupBox = (Widget) 0;

/*ARGSUSED*/
static void
gotoHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *name;

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    PopDownDialog(GotoNewsgroupBox);
    busyCursor();
    resetSelection();
    if ((int) client_data == XRNgoto_GOTO) {
	name = GetDialogValue(GotoNewsgroupBox);
	if (name[0] == '\0') {
	    mesgPane(XRN_INFO, "No newsgroup entered");
	} else {
	    setTitleBar("Opening newsgroup - please wait");
	    if (jumpToNewsgroup(name) == GOOD_GROUP) {
	    /* jumpToNewsgroup may have found a match for regex name */
		name = CurrentGroup->name;
		(void) strcpy(LastGroup, name);
		(void) switchToArticleMode(app_resources.autoRead);
	    }
	}
    }
    unbusyCursor();
    inCommand = 0;
    return;
}

/*
 * Jump to a newsgroup not displayed in newsgroup mode (either because
 * it's not subscribed to, or because all the articles have been read)
 *
 */
/*ARGSUSED*/
void
ngGotoFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Go To Group", gotoHandler, (caddr_t) XRNgoto_GOTO},
	{"Cancel",	gotoHandler, (caddr_t) XRNgoto_ABORT},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    if (GotoNewsgroupBox == (Widget) 0) {
	GotoNewsgroupBox = CreateDialog(TopLevel, "Go To Select",
				    "Group to go to:",
				    DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(GotoNewsgroupBox);
    return;
}

static void
switchToAllMode()
{  
    PreviousMode = Mode;
    Mode = ALL_MODE;

    /* switch buttons */
    swapMode();
    
    setTopInfoLine("View all available groups, with option to subscribe");
    redrawAllWidget(saveTop);

    return;
}

/*ARGSUSED*/
void
ngToggleGroupsFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    NewsgroupDisplayMode = (NewsgroupDisplayMode == 0) ? 1 : 0;
    redrawNewsgroupListWidget();
    return;
}

/*
 * Enter "all" mode.  Display all available groups to allow user to
 * subscribe/unsubscribe to them.
 */
/*ARGSUSED*/
void
ngAllGroupsFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    switchToAllMode();
    return;
}

/*
 * query the server to see if there are any new articles and groups
 */
/*ARGSUSED*/
void
ngRescanFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    rescanServer();
    determineMode();
    
    return;
}

/*ARGSUSED*/
void autoRescan(data, id)
caddr_t data;
XtIntervalId *id;
{
    if (Mode != NEWSGROUP_MODE) {
	TimeOut = 0;
	return;
    }
    TimeOut = 0;
    if (!inCommand && !ComposeActive) {
	inCommand = 1;
	busyCursor();
	infoNow("automatic rescan in progress...");
	ngRescanFunction((Widget) 0);
	infoNow("automatic rescan completed");
	close_server();
	unbusyCursor();
	inCommand = 0;
    }
    addTimeOut();

    return;
}

/*ARGSUSED*/
void autoExitAddMode(data, id)
caddr_t data;
XtIntervalId *id;
{
    if (Mode != ADD_MODE) {
	TimeOut = 0;
	return;
    }
    TimeOut = 0;
    if (!inCommand) {
	inCommand = 1;
	busyCursor();
	exitAddMode();
	infoNow("Add Groups operation timed out");
	unbusyCursor();
	inCommand = 0;
    }
    addTimeOut();

    return;
}

/*
 * put the user in the last newsgroup accessed
 */
/*ARGSUSED*/
void
ngPrevGroupFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    if (LastGroup[0] != '\0') {
	setTitleBar("Opening newsgroup - please wait");
	if (jumpToNewsgroup(LastGroup) == GOOD_GROUP) {
	    (void) switchToArticleMode(app_resources.autoRead);
	}
    }

    return;
}

/* 
 * save the user's selection of groups to be moved with the move
 * command
 */
/*ARGSUSED*/
void
ngSelectFunction(widget)
Widget widget;
{
    int i, count;

    if (Mode != NEWSGROUP_MODE || GroupItems == 0) {
	return;
    }
    if (selSize < GroupItems) {
	if (selList != NULL) FREE(selList);
	selList = XtMalloc(GroupItems+1);
	selSize = GroupItems;
    }
    count = 0;
    for (i=0; i<GroupItems;i++) {
	if (SListIsSelected(TopList, i)) {
	    selList[i] = 1;
	    count++;
	} else {
	    selList[i] = 0;
	}
    }
    if (count == 0) {
	selList[SListGetHighlight(TopList)] = 1;
	count++;
    }
    resetSelection();
    selCount = count;
    SListSelectAll(TopList, 0);
    return;
}

/*
 * Move the previously selected groups to the position before the
 * current selection
 */
/*ARGSUSED*/
void
ngMoveFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    int moveTo;
    int moveFrom;

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    if (selCount <= 0) {
	mesgPane(XRN_INFO, "No groups were selected");
	return;	
    }
    if (SelectionItem < 0) {
	SelectionItem = SListGetHighlight(TopList);
    }
    moveTo = SelectionItem;
    if (moveTo >= GroupItems) moveTo = GroupItems - 1;
    if (moveTo > 0) {
	strcpy(oldGroup, groupName(NewsGroups[moveTo-1]));
    }
    for (moveFrom = 0; moveFrom < GroupItems; moveFrom++) {
	if (selList[moveFrom] != 0) {
	    strcpy(newGroup, groupName(NewsGroups[moveFrom]));
	    if (moveTo <= 0) {
		moveTo = 0;
		(void) addToNewsrcBeginning(newGroup, status);
		(void) strcpy(oldGroup, newGroup);
	    } else {
		(void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
		(void) strcpy(oldGroup, newGroup);
	    }
	}
    }
    resetSelection();
    selCount = 0;
    redrawNewsgroupListWidget();
    return;
}

/*
 * Quit xrn, leaving the newsrc in the state it was in at
 * the last invokation of rescan.
 */
/*ARGSUSED*/
void
ngExitFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Yes",	generalHandler, (caddr_t) XRN_YES},
	{"No",	generalHandler, (caddr_t) XRN_NO},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    if (app_resources.confirmMode & NG_EXIT) {
	Action = NG_EXIT;
	if (ExitConfirmBox == (Widget) 0) {
	    ExitConfirmBox = CreateDialog(TopLevel, "Confirm Exit",
					   "Are you sure?",
					   DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(ExitConfirmBox);
	return;
    }

    ehNoUpdateExitXRN();
}

/*
 * disconnect from the news server
 */
/*ARGSUSED*/
void
ngDisconnectFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }

    close_server();
    return;
}

/*
 * update the .newsrc file
 */
/*ARGSUSED*/
void
ngCheckPointFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }

    while (!updatenewsrc())
	(void) ehErrorRetryXRN("Can not update .newsrc", True);

    return;
}

/*
 * Allow user to gripe
 */
/*ARGSUSED*/
void
ngGripeFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    gripe();
    return;
}

/*
 * allow user to post an article
 */
/*ARGSUSED*/
void
ngPostFunction(widget)
Widget widget;
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    post();
    
    return;
}

/*
 * called when the user wants to quit the current newsgroup and go to
 * the next one
 */
/*ARGSUSED*/
void
artQuitFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    exitArticleMode();
    
    return;
}

/*
 * called when the user wants to read the next article
 */
/*ARGSUSED*/
void
artNextFunction(widget)
Widget widget;
{
    char *text;			/* text of the article */
    char *question;		/* question to put in the question box */
    art_num artNum;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (getNearbyArticle(ArtStatus, &text, &question,
			 &artNum) == art_DONE) {
	exitArticleMode();
	return;
    }
    /* update the text window */
    foundArticle(text, question, artNum);

    ArtStatus = art_NEXT;
    ArtEntry = 0;
    return;
}

/*
 * called when the user wants to scroll the current article
 */
/*ARGSUSED*/
void
artScrollFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (!app_resources.pageArticles) {
	if (ScrollDownOnePage(ArticleText)) {
	    if (artCheckReadCurrent()) return;
	    if (app_resources.subjectRead == False) {
		artNextUnreadFunction((Widget) 0);
	    } else {
		if (app_resources.sortedSubjects == THREAD_SORTED) {
		    artNextUnreadFunction((Widget) 0);
		} else {
		    artSubNextFunction((Widget) 0);
		}
	    }
	}
    } else {
	ScrollDownOnePage(ArticleText);
	return;
    }
    return;
}

/*
 * called when the user wants to scroll the current article
 */
/*ARGSUSED*/
void
artScrollBackFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    (void) ScrollUpOnePage(ArticleText);
    return;
}

/*
 * called when the user wants to scroll the current article
 */
/*ARGSUSED*/
void
artScrollLineFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
#ifdef MOTIF
#ifdef MOTIF_V10
    XmTextDisableRedisplay(ArticleText, FALSE);
#else
    _XmTextDisableRedisplay((XmTextWidget) ArticleText, FALSE);
#endif
    XmTextScroll(ArticleText, 1);
#ifdef MOTIF_V10
    XmTextEnableRedisplay(ArticleText);
#else
    _XmTextEnableRedisplay((XmTextWidget) ArticleText);
#endif
#else
    DwtTextDisableRedisplay(ArticleText, FALSE);
    (void) DwtTextScroll(ArticleText, 1);
    DwtTextEnableRedisplay(ArticleText);
#endif
    return;
}

/*
 * called when the user wants to scroll the current article
 */
/*ARGSUSED*/
void
artScrollBackLineFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
#ifdef MOTIF
#ifdef MOTIF_V10
    XmTextDisableRedisplay(ArticleText, FALSE);
#else
    _XmTextDisableRedisplay((XmTextWidget) ArticleText, FALSE);
#endif
    XmTextScroll(ArticleText, -1);
#ifdef MOTIF_V10
    XmTextEnableRedisplay(ArticleText);
#else
    _XmTextEnableRedisplay((XmTextWidget) ArticleText);
#endif
#else
    DwtTextDisableRedisplay(ArticleText, FALSE);
    (void) DwtTextScroll(ArticleText, -1);
    DwtTextEnableRedisplay(ArticleText);
#endif
    return;
}

void
artScrollIndexFunction(widget)
Widget widget;
/*
 * called when the user wants to scroll the index window
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    return;
}

/*ARGSUSED*/
void
artScrollIndexBackFunction(widget)
Widget widget;
/*
 * called when the user wants to scroll the index window
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    return;
}

/*
 * called when the user wants to scroll the current article
 */
/*ARGSUSED*/
void
artScrollEndFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
#ifdef MOTIF
    XmTextShowPosition(ArticleText, XmTextGetLastPosition(ArticleText));
    XmTextScroll(ArticleText, -1);
#else
    DwtTextShowPosition(ArticleText, DwtSTextGetLastPosition(ArticleText));
    DwtTextScroll(ArticleText, -1);
#endif
    return;
}


/*ARGSUSED*/
void
artScrollBeginningFunction(widget)
Widget widget;
/*
 * called when the user wants to scroll the current article
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
#ifdef MOTIF
    XmTextShowPosition(ArticleText, 0);
#else
    DwtTextShowPosition(ArticleText, 0);
#endif
    return;
}

/*
 * called when the user wants to go to the next unread news
 * article in the current newsgroup
 * 
 */
void
artNextUnreadFunction(widget)
Widget widget;
{
    void artNextFunction();
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (app_resources.nextReadsFirst) {
	ArtPosition = 0;
    }
    ArtStatus = art_UNREAD;
    artNextFunction(widget);
    return;
}

/*
 * called when the user wants to read the previous article
 */
/*ARGSUSED*/
void
artPrevFunction(widget)
Widget widget;
{
    art_num artNum;
    char *text, *question;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    ArtStatus = art_PREV;
    if (ArtPosition <= 0) {
	ArtPosition = 0;
	if (getPrevious(&artNum)) {
	    gotoArticle(artNum);
	    (void) getArticleText(&text, &question);
	    foundArticle(text, question, artNum);
	    ArtStatus = art_NEXT;
	    return;
	}
	setTopInfoLine("No previous article available");
	return;
    }
    ArtStatus = art_PREV;
    if (getNearbyArticle(ArtStatus, &text, &question,
			 &artNum) == art_DONE) {
	exitArticleMode();
	return;
    }
    foundArticle(text, question, artNum);
    ArtStatus = art_NEXT;
    
    return;
}

/*ARGSUSED*/
void
artReadFunction(widget)
Widget widget;
/*
 * called when the user wants to read the current article
 */
{
    char *text;			/* text of the article */
    char *question;		/* question to put in the question box */
    art_num artNum;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    ArtStatus = art_CURRENT;
    if (getNearbyArticle(ArtStatus, &text, &question,
			 &artNum) == art_DONE) {
	exitArticleMode();
	return;
    }
    /* update the text window */
    foundArticle(text, question, artNum);

    ArtStatus = art_NEXT;
    return;
}


/*ARGSUSED*/
void
artNextGroupFunction(widget)
Widget widget;
{
    void artQuitFunction();
    char name[GROUP_NAME_SIZE];
    int append = 0;
    int i;
    struct newsgroup *LastGroupStruct = CurrentGroup;

    PrevArticle = CurrentArticle = 0;
    SubjectItems = 0;

#ifdef MOTIF
    XmTextSetString(ArticleText, NULL);
    XmTextSetEditable(ArticleText, FALSE);
#else
    DwtSTextSetString(ArticleText, "");
    DwtSTextSetEditable(ArticleText, FALSE);
#endif
    setBottomInfoLine("");
    releaseNewsgroupResources();

    while (1) {
	/*
	 * XXX if the newsgroup is fully read, then when the new newsgroup
	 * string is regenerated (upon entry to newsgroup mode), the string
	 * for this group will not be there and thus the Group Position will
	 * be too far forward (by one group)
	 */

	if (NewsGroups != NIL(ng_num)) {
	    FREE(NewsGroups);
	    NewsGroups = (ng_num *)0;
	}

	NewsGroups = unreadGroups(NewsgroupDisplayMode, &unreadNews);
	GroupItems = 0;
	for (i=0;NewsGroups[i] != -1;i++) {
	    GroupItems++;
	}
	if (GroupPosition >= GroupItems) {
	    GroupPosition = 0;
	    artQuitFunction(widget);
	    return;
	}
	strcpy(name, groupName(NewsGroups[GroupPosition]));
	if (STREQ(name, LastGroup)) {
	    /* last group not fully read */
	    GroupPosition++;
	    if (GroupPosition >= GroupItems) {
		artQuitFunction(widget);
		return;
	    }
	    strcpy(name, groupName(NewsGroups[GroupPosition]));
	}

	if ((name == NIL(char)) || (*name == '\0')) {
	    artQuitFunction(widget);
	    return;
	}

	setTitleBar("Opening newsgroup - please wait");
	if (gotoNewsgroup(name) != GOOD_GROUP) {
	    mesgPane(XRN_SERIOUS | append,
		"Bad group `%s', skipping to next group", name);
	    append = XRN_APPEND;
	    (void) strcpy(LastGroup, name);
	    GroupPosition++;
	    continue;
	}

	if (getNewsgroup() == XRN_NOMORE) {
	    mesgPane(XRN_INFO | append,
		       "No unread articles in `%s', skipping to next group",
		       name);
	    if (NewsgroupDisplayMode) {
		if (GroupPosition >= GroupItems) {
		    artQuitFunction(widget);
		    return;
		}
	    }
	    append = XRN_APPEND;
	    GroupPosition++;
	    continue;
	}

	if (switchToArticleMode(app_resources.autoRead) == GOOD_GROUP) {
	    (void) strcpy(LastGroup, name);
	    if (app_resources.updateNewsrc == TRUE) {
		while (!updatenewsrc())
		    ehErrorRetryXRN("Cannot update .newsrc", True);
	    }
	    return;
	}
	 /*
	  * Normally, I'd put a call to mesgPane in here to tell the
	  * user that the switchToArticleMode failed, but it isn't
	  * necessary because switchToArticleMode calls bogusNewsgroup
	  * if it fails, and bogusNewsgroup calls mesgPane with an
	  * appropriate message.
	  */
    }
}


/*ARGSUSED*/
void
artFedUpFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"No",	generalHandler, (caddr_t) XRN_NO},
	{"Yes",	generalHandler, (caddr_t) XRN_YES},
    };

    if (app_resources.confirmMode & ART_FEDUP) {
	Action = ART_FEDUP;
	if (FedUpConfirmBox == (Widget) 0) {
	    FedUpConfirmBox = CreateDialog(TopLevel, "Confirm Fed Up",
					   "Are you sure?",
					   DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(FedUpConfirmBox);
	return;
    }

    fedUpART();
    return;
}


/*
 * called when the user wants to mark all articles in the current group as read
 */
/*ARGSUSED*/
void
artCatchUpFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Yes",	generalHandler, (caddr_t) XRN_YES},
	{"No",	generalHandler, (caddr_t) XRN_NO},
    };

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (SelectionItem < 0) {
	SelectionItem = SListGetHighlight(TopList);
    }
    ArtPosition++;
    if (ArtPosition >= SubjectItems)
	ArtPosition = SubjectItems - 1;
    if (app_resources.confirmMode & ART_CATCHUP) {
	Action = ART_PART_CATCHUP;
	if (PartCatchUpConfirmBox == (Widget) 0) {
		PartCatchUpConfirmBox = CreateDialog(TopLevel,
				"Position Catch Up Confirm",
				"OK to catch up to current position?",
				DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(PartCatchUpConfirmBox);
    } else {
	catchUpPartART();
    }
    return;
}

/*
 * called when the user wants to mark all articles in the current group as read
 */
/*ARGSUSED*/
void
artCatchUpAllFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Yes",	generalHandler, (caddr_t) XRN_YES},
	{"No",	generalHandler, (caddr_t) XRN_NO},
    };

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (app_resources.confirmMode & ART_CATCHUP) {
	Action = ART_CATCHUP;
	if (CatchUpConfirmBox == (Widget) 0) {
		CatchUpConfirmBox = CreateDialog(TopLevel,
				"Position Catch Up Confirm",
				"OK to catch up entire group?",
				DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(CatchUpConfirmBox);
    } else {
	catchUpART();
    }
    return;
}


/*

/*
 * called when the user wants to unsubscribe to the current group
 */
/*ARGSUSED*/
void
artUnsubFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Yes",	generalHandler, (caddr_t) XRN_YES},
	{"No",	generalHandler, (caddr_t) XRN_NO},
    };

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (app_resources.confirmMode & ART_UNSUBSCRIBE) {
	Action = ART_UNSUBSCRIBE;
	if (UnSubConfirmBox == (Widget) 0) {
	    UnSubConfirmBox = CreateDialog(TopLevel, "Confirm Unsubscribe",
				"OK to unsubscribe?",
				DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(UnSubConfirmBox);
	return;
    }
    unsubscribeART();
    return;
}

/*
 * Get selection region, mark articles, redisplay subject window.
 */
void
markFunction(marker)
char marker;
{
    int i;
    int count = 0;
    int save, top;

    if ((top = SListGetTop(TopList)) == -1) top = 0;
    save = ArtPosition;

    for (i = 0; i < SubjectItems; i++) {
	if (SListIsSelected(TopList,i)) {
	    markArticle(ArticleNumbers[i], marker);
	    if (app_resources.watchProgress) {
		updateSubjectWidget(i, i, False);
	    }
	    count++;
	}
    }

    if (!app_resources.watchProgress) {
	for (i=0; i < SubjectItems; i++) {
	    SListUpdateLine(TopList, i);
	}
    }

    subjSetPos(TopList, save, True);
    ArtPosition = save;

    resetSelection();

    if (count == 0) {
	count = 1;
	markArticle(ArticleNumbers[ArtPosition], marker);
	SListUpdateLine(TopList, ArtPosition);
    }

    ArtPosition = save;
    SListSetTop(TopList, top);
    setBottomInfoLine(currentQuestion());

    return;
}


/*
 * Mark selected article(s) as read
 */
/*ARGSUSED*/
void
artMarkReadFunction(widget)
Widget widget;
{
    char marker = '+';

    if (Mode != ARTICLE_MODE) {
	return;
    }

    markFunction(marker);
    return;
}


/*
 * Mark selected article(s) as unread
 */
/*ARGSUSED*/
void
artMarkUnreadFunction(widget)
Widget widget;
{
    char marker = 'u';

    if (Mode != ARTICLE_MODE) {
	return;
    }
    markFunction(marker);
    return;
}

/*
 * allow user to post to the newsgroup currently being read
 */
/*ARGSUSED*/
void
artPostFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    post();
    
    return;
}

#define EA_Next 1
#define EA_Followup 2
#define EA_Quit 3
#define EA_Cancel 4
/*ARGSUSED*/
static void
endActionHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    PopDownDialog(EndActionBox);
    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    busyCursor();
    switch((int) client_data) {
	case EA_Next:
	    artNextUnreadFunction((Widget) 0);
	    break;
	case EA_Followup:
	    artFollowupFunction((Widget) 0);
	    break;
	case EA_Quit:
	    artQuitFunction((Widget) 0);
	    break;
	case EA_Cancel:
	    break;
    }
    unbusyCursor();
    inCommand = 0;
    return;
}

/*ARGSUSED*/
void
artSubNextFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Next Unread", endActionHandler, (caddr_t) EA_Next},
	{"Followup",    endActionHandler, (caddr_t) EA_Followup},
	{"Cancel",	endActionHandler, (caddr_t) EA_Cancel},
	{"Return",	endActionHandler, (caddr_t) EA_Quit},
    };

    char *text, *question;
    char *subject;
    art_num artNum;
    int status;
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (SelectionItem < 0) {
	SelectionItem = SListGetHighlight(TopList);
    }
    if (SelectionItem >= 0) {
	ArtPosition = SelectionItem;
	resetSelection();
    }
    if (ArtPosition >= SubjectItems)
	ArtPosition = SubjectItems - 1;
    artNum = ArticleNumbers[ArtPosition];
    subject = XtNewString(getSubject(artNum));
    (void) sprintf(error_buffer, "Subject search: %s", subject);
    info(error_buffer);
    status = isNextSubject(subject, &text, &question, &artNum);
    switch (status) {
	  case ABORT:
    	    FREE(subject);
	    infoNow("search has been aborted");
	    return;
	  case NOCHANGE:
	    (void) sprintf(error_buffer, "Subject search: %s", subject);
	    info(error_buffer);
    	    FREE(subject);
	    foundArticle(text, question, artNum);
	    return;
	  case DONE:
    	    FREE(subject);
	    if (app_resources.confirmMode & ART_ENDACTION) {
		if (EndActionBox == (Widget) 0) {
			EndActionBox = CreateDialog(TopLevel,
			"End Action Select",
			"End of subject. Next action?",
			DIALOG_NOTEXT, args, XtNumber(args));
		}
		PopUpDialog(EndActionBox);
		return;
	    } else {
		ArtPosition = 0;
		infoNow("Subject has been exhausted, returning to first unread article");
		if (getNearbyArticle(art_UNREAD, &text, &question,
			&artNum) == art_DONE) {
		    exitArticleMode();
		    return;
		}
		foundArticle(text, question, artNum);
		return;
	    }
	  case EXIT:
    	    FREE(subject);
	    exitArticleMode();
	    return;
	}
    if (getNearbyArticle(art_NEXT, &text, &question, &artNum) == art_DONE) {
	exitArticleMode();
	return;
    }
    foundArticle(text, question, artNum);
    
    return;
}
	

/*ARGSUSED*/
void
artSubPrevFunction(widget)
Widget widget;
{
    char *subject;
    art_num artNum;
    char *text, *question;
    int status;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (SelectionItem < 0 ){
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem < 0) SelectionItem = 0;
	if (SelectionItem >= SubjectItems) SelectionItem = SubjectItems - 1;
	ArtPosition = SelectionItem;
	/* nothing selected, we should be on a valid article */

	if (ArtPosition >= SubjectItems) {
	    resetSelection();
	    return;
	}
	artNum = ArticleNumbers[ArtPosition];
	subject = XtNewString(getSubject(artNum));
	(void) sprintf(error_buffer, "Subject search: %s", subject);
	info(error_buffer);
	status = isPrevSubject(subject, &text, &question, &artNum);
	FREE(subject);
	switch(status) {
	  case ABORT:
	    infoNow("search has been aborted");
	    return;
	  case NOCHANGE:
	    foundArticle(text, question, artNum);
	    return;
	  case CHANGE:
	    ArtPosition = 0;
	    resetSubjectWidget(ALL, False);
	    foundArticle(text, question, artNum);
	    return;
	  case DONE:
	    infoNow("Subject has been exhausted");
	    return;
	  case EXIT:
	    exitArticleMode();
	    return;
	}
    }
    ArtPosition = SelectionItem;
    resetSelection();
    if (ArtPosition < 0) ArtPosition = 0;
    if (ArtPosition >= SubjectItems) ArtPosition = SubjectItems - 1;
    artNum = ArticleNumbers[ArtPosition];
    gotoArticle(artNum);
    subjSetPos(TopList, ArtPosition, True);
    if (getArticleText(&text, &question) != XRN_OKAY) {
	if (getNearbyArticle(art_NEXT, &text, &question, &artNum) == art_DONE) {
	    exitArticleMode();
	    return;
	}
	infoNow(error_buffer);
	foundArticle(text, question, artNum);
	return;
    }
    infoNow(error_buffer);
    foundArticle(text, question, artNum);
    
    return;
}


char *SubjectKilled;

/*
 * Allow user to mark all articles with the current subject as read
 *
 * XXX get subject, kill using data structures, rebuild 
 */
/*ARGSUSED*/
static void
_artKillSession(killThreads)
Boolean killThreads;
{
    char *subject;
    char *cursubject;
    char *text, *question;
    art_num artNum;
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (SubjectItems <= 0) return;

    if (SelectionItem > 0 && SelectionItem < SubjectItems) {
	ArtPosition = SelectionItem;
    } else {
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem >= 0) 
	    ArtPosition = SelectionItem;
    }
    resetSelection();
    if (ArtPosition >= SubjectItems) ArtPosition = SubjectItems - 1;
    artNum = ArticleNumbers[ArtPosition];
    subject = XtNewString(getSubject(artNum));
    SubjectKilled = XtNewString(subject);
    ArtPosition = 0;
    while (ArtPosition < SubjectItems) {
	artNum = ArticleNumbers[ArtPosition];
	cursubject = getSubject(artNum);
	/* only kill those that have not been marked as unread */
	if (STREQ(subject, cursubject) && !articleMarkedUnread(artNum)) {
	    markArticleAsKilled(artNum);
	    SListUpdateLine(TopList, ArtPosition);
	    if (killThreads) {
		killThread(artNum);
	    }
	}
	ArtPosition++;
    }
    /* set the cursor back to the beginning of the subject screen */
    clearArtCache();
    ArtPosition = 0;
    infoNow("Subject has been killed, returning to first unread article");
    FREE(subject);

    if (!app_resources.retainKilledArticles)
	resetSubjectWidget(UNKILLED, True);
    if (app_resources.autoRead) {
	if (SubjectItems == 0 ||
	    getNearbyArticle(art_UNREAD, &text, &question, &artNum)
		== art_DONE) {
	    exitArticleMode();
	    return;
	}
	foundArticle(text, question, artNum);
    } else {
	if (getNearbyArticle(art_FINDUNREAD, &text, &question, &artNum) == 
		art_DONE) {
	    return;
	}
	setBottomInfoLine(currentQuestion());
    }	
    return;
}

/*ARGSUSED*/
void
artKillSessionFunction(widget)
Widget widget;
{
    if (app_resources.sortedSubjects == THREAD_SORTED) {
	_artKillSession(True);
    } else {
	_artKillSession(False);
    }
    FREE(SubjectKilled);
    return;
}

/*
 * Allow user to mark all articles with the current author as read
 *
 */
/*ARGSUSED*/
void
artKillAuthorFunction(widget)
Widget widget;
{
    char *author;
    char *curauthor;
    char *text, *question;
    art_num artNum;
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (SelectionItem >= 0 && SelectionItem < SubjectItems) {
	ArtPosition = SelectionItem;
    } else {
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem >= 0)
		ArtPosition = SelectionItem;
    }
    if (ArtPosition >= SubjectItems) ArtPosition = SubjectItems - 1;
    resetSelection();
    artNum = ArticleNumbers[ArtPosition];
    author = XtNewString(getAuthor(artNum));
    killItem(author, KILL_LOCAL);
    ArtPosition = 0;
    while (ArtPosition < SubjectItems) {
	artNum = ArticleNumbers[ArtPosition];
	curauthor = getAuthor(artNum);
	/* only kill those that have not been marked as unread */
	if ((STREQ(author, curauthor)) && !articleMarkedUnread(artNum)) {
	    markArticleAsKilled(artNum);
	    SListUpdateLine(TopList, ArtPosition);
	}
	ArtPosition++;
    }
    /* set the cursor back to the beginning of the subject screen */
    clearArtCache();
    FREE(author);
    ArtPosition = 0;
    infoNow("Author has been killed, returning to first unread article");

    if (!app_resources.retainKilledArticles)
	resetSubjectWidget(UNKILLED, True);
    if (app_resources.autoRead) {
	if (SubjectItems == 0 || 
	    getNearbyArticle(art_UNREAD, &text, &question, &artNum)
		== art_DONE) {
	    exitArticleMode();
	    return;
	}
	foundArticle(text, question, artNum);
    } else {
	if (getNearbyArticle(art_FINDUNREAD, &text, &question, &artNum) == 
		art_DONE) {
	    return;
	}
	setBottomInfoLine(currentQuestion());
    }	
    
    return;
}

/*ARGSUSED*/
void
artKillLocalFunction(widget)
Widget widget;
{
    struct newsgroup *grp = CurrentGroup;
    Boolean restored = False;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    _artKillSession(False);
    if (CurrentGroup == NULL) {
	CurrentGroup = grp;
	restored = True;
    }
    killItem(SubjectKilled, KILL_LOCAL);
    FREE(SubjectKilled);
    if (restored) CurrentGroup = NULL;
    return;
}

/*ARGSUSED*/
void
artKillGlobalFunction(widget)
Widget widget;
{
    struct newsgroup *grp = CurrentGroup;
    Boolean restored = False;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    _artKillSession(False);
    if (CurrentGroup == NULL) {
	CurrentGroup = grp;
	restored = True;
    }
    killItem(SubjectKilled, KILL_GLOBAL);
    FREE(SubjectKilled);
    if (restored) CurrentGroup = NULL;
    return;
}

/*ARGSUSED*/
void
artEditKillFileFunction(widget)
Widget widget;
{
    EditKillFile(CurrentGroup);
    return;
}

/*ARGSUSED*/
void
artEditGlobalKillFileFunction(widget)
Widget widget;
{
    EditKillFile((struct newsgroup *)NULL);
    return;
}

/*ARGSUSED*/
void
artListOldFunction(widget)
Widget widget;
{
    char *text, *question;
    int SavePosition;
    int status;
    art_num artNum;
    art_num saveArtNum;
    struct newsgroup *newsgroup = CurrentGroup;
    struct artStat *artstat = GETSTATUS(newsgroup);

    busyCursor();
    resetSelection();    
    SavePosition = ArtPosition;
    saveArtNum = ArticleNumbers[ArtPosition];

    if (oldListed) {
	oldListed = False;
	artNum = newsgroup->initial = oldInitial;	
    } else {
	oldListed = True;
	oldInitial = newsgroup->initial;
	artNum = newsgroup->first;
    }
    
    for (;artNum<=newsgroup->last;artNum++) {
	if (IS_UNAVAIL(artstat[INDEX(artNum)])) {
	    continue;
	}
	status = moveToArticle(artNum, &text, &question);

	switch (status) {

	case NOMATCH:
	    mesgPane(XRN_SERIOUS,
		"First article (number %d) not available", artNum);
	    break;

	case ERROR:
	    break;

	case MATCH:
	    resetSubjectWidget(ALL, False);
	    ArtPosition = 0;
	    gotoArticle(saveArtNum);
	    foundArticle(text, question, saveArtNum);
	    unbusyCursor();
	    return;
	}
    }
    unbusyCursor();
    return;
}

/*
 * update the .newsrc file
 */
/*ARGSUSED*/
void
artCheckPointFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }

    while (!updatenewsrc())
	ehErrorRetryXRN("Cannot update .newsrc", True);

    return;
}


#define XRNgotoArticle_ABORT	0
#define XRNgotoArticle_DOIT	1

static Widget GotoArticleBox = (Widget) 0;


/*ARGSUSED*/
static void 
gotoArticleHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *numberstr;
    char *text, *question;
    int SavePosition;
    int status;
    art_num artNum;
    int i;

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    PopDownDialog(GotoArticleBox);
    busyCursor();
    SListSelectAll(TopList, 0);
    if ((int) client_data == XRNgotoArticle_ABORT) {
	unbusyCursor();
	inCommand = 0;
	return;
    }
    SavePosition = ArtPosition;
    numberstr = GetDialogValue(GotoArticleBox);
    if (numberstr == NIL(char)) {
	mesgPane(XRN_INFO, "Bad article number");
	unbusyCursor();
	inCommand = 0;
	return;
    }

    artNum = atol(numberstr);
    if (artNum == 0) {
	mesgPane(XRN_SERIOUS, "Bad article number");
	unbusyCursor();
	inCommand = 0;
	return;
    }
    
    status = moveToArticle(artNum, &text, &question);

    switch (status) {

      case NOMATCH:
	mesgPane(XRN_SERIOUS, "Article number %d not available", artNum);
	break;

      case ERROR:
	ArtPosition = SavePosition;
	break;

      case MATCH:
	resetSubjectWidget(ACTIVE, False);
	if (SubjectItems > 0) {
	    for (i = 0; i < SubjectItems; i++) {
		if (ArticleNumbers[i] == artNum) {
		    ArtPosition = i;
		    break;
		}
	    }
	}
	gotoArticle(artNum);
	foundArticle(text, question, artNum);
	break;
    }

    unbusyCursor();
    inCommand = 0;
    return;
}

/*ARGSUSED*/
void
artGotoArticleFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Go To Article", gotoArticleHandler, (caddr_t) XRNgotoArticle_DOIT},
	{"Cancel",	  gotoArticleHandler, (caddr_t) XRNgotoArticle_ABORT},
    };
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (GotoArticleBox == (Widget) 0) {
	GotoArticleBox = CreateDialog(TopLevel, "Go To Article Select",
				"Article Number:",
				DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(GotoArticleBox);
    return;
}

#define XRNSearch_ABORT 0
#define XRNSearch_FORWARD 1
#define XRNSearch_BACK 2

static Widget SearchBox = (Widget) 0;
static Widget AuthorSearchBox = (Widget) 0;

/*ARGSUSED*/
static void 
subSearchHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *regexpr;
    char *text, *question;
    int SavePosition;
    int status;
    int direction;
    art_num artNum;

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;

    busyCursor();
    if ((int) client_data == XRNSearch_ABORT) {
	if (SearchBox) PopDownDialog(SearchBox);
	inCommand = 0;
	unbusyCursor();
	return;
    }
    if (SelectionItem > 0) {
	ArtPosition = SelectionItem; 
    } else {
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem >= 0)
	    ArtPosition = SelectionItem;
    }
    if (ArtPosition >= SubjectItems) {
	ArtPosition = SubjectItems - 1;
    }
    SavePosition = ArtPosition;
    regexpr = GetDialogValue(SearchBox);
    if (*regexpr == 0) {
	if (LastRegexp == NIL(char)) {
	    mesgPane(XRN_INFO, "No previous regular expression");
	    if (SearchBox) PopDownDialog(SearchBox);
	    inCommand = 0;
	    unbusyCursor();
	    return;	   
	}
	regexpr = LastRegexp;
    } else {
	if (LastRegexp != NIL(char)) {
	    FREE(LastRegexp);
	}
	LastRegexp = XtNewString(regexpr);
    }
    
    /* XXX */
    if (SearchBox) PopDownDialog(SearchBox);

    direction = ((int) client_data == XRNSearch_FORWARD) ? FORWARD : BACK;
    LastWasAuthor = False;
    LastSearch = direction;
    status = subjectSearch(direction, ArticleNumbers,
			   &ArtPosition, LastRegexp,
			   &text, &question, &artNum);
    switch (status) {
      case ABORT:
	infoNow("search aborted");
	ArtPosition = SavePosition;
	subjSetPos(TopList, ArtPosition, True);
	break;
      case NOMATCH:
	(void) sprintf(error_buffer, "Search for expression %s: no match was found",
		       LastRegexp);
	infoNow(error_buffer);
      case ERROR:
	ArtPosition = SavePosition;
	subjSetPos(TopList, ArtPosition, True);
	break;
      case MATCH:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	foundArticle(text, question, artNum);
	break;
      case WINDOWCHANGE:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	ArtPosition = 0;
	resetSubjectWidget(ALL, False);
	foundArticle(text, question, artNum);
	break;
      case EXIT:
	exitArticleMode();
	break;
    }
    if (SearchBox) PopDownDialog(SearchBox);
    inCommand = 0;
    unbusyCursor();
    return;
}
/*ARGSUSED*/
static void 
authorSearchHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *regexpr;
    char *text, *question;
    int SavePosition;
    int status;
    int direction;
    art_num artNum;

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;

    busyCursor();
    if ((int) client_data == XRNSearch_ABORT) {
	if (AuthorSearchBox) PopDownDialog(AuthorSearchBox);
	inCommand = 0;
	unbusyCursor();
	return;
    }
    if (SelectionItem > 0) {
	ArtPosition = SelectionItem;
    } else {
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem >= 0)
	    ArtPosition = SelectionItem;
    }
    if (ArtPosition >= SubjectItems) {
	ArtPosition = SubjectItems - 1;
    }
    SavePosition = ArtPosition;
    regexpr = GetDialogValue(AuthorSearchBox);
    if (*regexpr == 0) {
	if (LastRegexp == NIL(char)) {
	    mesgPane(XRN_INFO, "No previous regular expression");
	    if (AuthorSearchBox) PopDownDialog(AuthorSearchBox);
	    inCommand = 0;
	    unbusyCursor();
	    return;	   
	}
	regexpr = LastRegexp;
    } else {
	if (LastRegexp != NIL(char)) {
	    FREE(LastRegexp);
	}
	LastRegexp = XtNewString(regexpr);
    }
    
    /* XXX */
    if (AuthorSearchBox) PopDownDialog(AuthorSearchBox);

    direction = ((int) client_data == XRNSearch_FORWARD) ? FORWARD : BACK;
    LastSearch = direction;
    LastWasAuthor = True;
    status = authorSearch(direction, ArticleNumbers,
			   &ArtPosition, LastRegexp,
			   &text, &question, &artNum);
    switch (status) {
      case ABORT:
	infoNow("search aborted");
	ArtPosition = SavePosition;
	subjSetPos(TopList, ArtPosition, True);
	break;
      case NOMATCH:
	(void) sprintf(error_buffer, "Search for expression %s: no match was found",
		       LastRegexp);
	infoNow(error_buffer);
      case ERROR:
	ArtPosition = SavePosition;
	subjSetPos(TopList, ArtPosition, True);
	break;
      case MATCH:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	foundArticle(text, question, artNum);
	break;
      case WINDOWCHANGE:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	ArtPosition = 0;
	resetSubjectWidget(ALL, False);
	foundArticle(text, question, artNum);
	break;
      case EXIT:
	exitArticleMode();
	break;
    }
    if (AuthorSearchBox) PopDownDialog(AuthorSearchBox);
    inCommand = 0;
    unbusyCursor();
    return;
}


/*ARGSUSED*/
void
artSubSearchFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Forward", subSearchHandler, (caddr_t) XRNSearch_FORWARD},
	{"Back",    subSearchHandler, (caddr_t) XRNSearch_BACK},
	{"Cancel",  subSearchHandler, (caddr_t) XRNSearch_ABORT},
    };
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (SearchBox == (Widget) 0) {
	SearchBox = CreateDialog(TopLevel, "Subject Search Expression",
				"Regular Expression?",
				DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(SearchBox);
    return;
}

/*ARGSUSED*/
void
artAuthorSearchFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Forward", authorSearchHandler, (caddr_t) XRNSearch_FORWARD},
	{"Back",    authorSearchHandler, (caddr_t) XRNSearch_BACK},
	{"Cancel",  authorSearchHandler, (caddr_t) XRNSearch_ABORT},
    };
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (AuthorSearchBox == (Widget) 0) {
	AuthorSearchBox = CreateDialog(TopLevel, "Author Search Expression",
				"Regular Expression?",
				DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(AuthorSearchBox);
    return;
}

/*ARGSUSED*/
void
artCancelSearchFunction(widget)
Widget widget;
{
    abortSet();
}

/*
 * Continue a previously started regular expression
 * search of the subject lines.  Search for same
 * regular expression, in same direction.
 */
/*ARGSUSED*/
void
artContinueFunction(widget)
Widget widget;
{
    char *text, *question;
    int SavePosition;
    int status;
    art_num artNum;

    if (LastRegexp == NULL) {
	mesgPane(XRN_INFO, "No previous regular expression");
	return;
    }
    (void) sprintf(error_buffer, "Searching for expression %s", LastRegexp);
    info(error_buffer);
    resetSelection();
    SavePosition = ArtPosition;
    if (LastWasAuthor) {
	status = authorSearch(LastSearch, ArticleNumbers,
				&ArtPosition, LastRegexp,
				&text, &question, &artNum);
    } else {
	status = subjectSearch(LastSearch, ArticleNumbers,
				&ArtPosition, LastRegexp,
				&text, &question, &artNum);
    }
    switch (status) {
      case ABORT:
	infoNow("search aborted");
	ArtPosition = SavePosition;
	subjSetPos(TopList, ArtPosition, True);
	return;
      case NOMATCH:
	(void) sprintf(error_buffer, "Search for expression %s: no match was found", LastRegexp);
	infoNow(error_buffer);
      case ERROR:
	ArtPosition = SavePosition;
	subjSetPos(TopList, ArtPosition, True);
	return;
      case MATCH:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);    
	foundArticle(text, question, artNum);
	return;
      case WINDOWCHANGE:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	ArtPosition = 0;
	resetSubjectWidget(ALL, False);
	foundArticle(text, question, artNum);
	return;
      case EXIT:
	exitArticleMode();
	return;
    }
}

/*
 * set order of sorting in subject widget and reload
 */
/*ARGSUSED*/
static void
changeSortOrder(widget, neworder)
Widget widget;
int neworder;
{
    art_num artNum, newArtNum;
    int i;

    if (ArtPosition < 0) ArtPosition = 0;
    if (ArtPosition >= SubjectItems) ArtPosition = SubjectItems - 1;
    if (SubjectItems == 0) {
	ArtPosition = 0;
	app_resources.sortedSubjects = neworder;
	resetSubjectWidget(ACTIVE, False);
	return;
    }
    artNum = ArticleNumbers[ArtPosition];
    app_resources.sortedSubjects = neworder;
    resetSubjectWidget(ACTIVE, False);
    if (SubjectItems <= 0) return;
    for (i=0; i < SubjectItems; i++) {
	newArtNum = ArticleNumbers[i];
	if (newArtNum == artNum) {
	    ArtPosition = i;
	    subjSetPos(TopList, ArtPosition, True);
	    return;
	}
    }
    ArtPosition = 0;
    subjSetPos(TopList, ArtPosition, True);
}

void
artNonSortedFunction(widget)
Widget widget;
{
    changeSortOrder(widget, UNSORTED);
}
	
void
artSortedFunction(widget)
Widget widget;
{
    changeSortOrder(widget, ARTICLE_SORTED);
}
	
void
artStrictSortedFunction(widget)
Widget widget;
{
    changeSortOrder(widget, STRICT_SORTED);
}
	
void
artThreadSortedFunction(widget)
Widget widget;
{
    changeSortOrder(widget, THREAD_SORTED);
}
	
/*
 * Display the article accessed before the current one
 */
/*ARGSUSED*/
void
artLastFunction(widget)
Widget widget;
{
    char *text, *question;
    art_num artNum = 0;
    int status;
    int SavePosition;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (PrevArticle <= 0) {
	mesgPane(XRN_INFO, "No previous article");
	return;
    }

    SavePosition = ArtPosition;
    artNum = PrevArticle;

    status = moveToArticle(artNum, &text, &question);
    switch (status) {

      case NOMATCH:
	mesgPane(XRN_SERIOUS, "Article number %d not available", artNum);
	break;

      case ERROR:
	ArtPosition = SavePosition;
	break;

      case MATCH:
	gotoArticle(artNum);
	foundArticle(text, question, artNum);
	break;
    }
    return;
}

/*
 * Exit from the current newsgroup, marking all articles as
 * unread
 */
/*ARGSUSED*/
void
artExitFunction(widget)
Widget widget;
{
    int i;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    for (i = 0; i < SubjectItems; i++) {
	markArticleAsUnread(ArticleNumbers[i]);
    }
    exitArticleMode();
    
    return;
}


/*
 * unsubscribe to the remaining groups and exit add mode
 */
/*ARGSUSED*/
void
addQuitFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int left;
    int status = UNSUBSCRIBE;

    if (Mode != ADD_MODE) {
	return;
    }
    left = 0;

    /*
     * go through the remaining groups, add them
     * to the end of the newsrc and unsubscribe them
     */
    while (left < AddGroupItems) {
	(void) strcpy(newGroup, AddGroups[left]->name);
	(void) addToNewsrcEnd(newGroup, status);
	left++;
    }

    exitAddMode();
    return;
}

/*
 * Find selected group(s) and add them to the .newsrc in the first position.
 * Move the cursor to the next group.
 * Update the AddGroups, going into newsgroup mode if it
 * is NULL.  Update the text window, update the insertion point.
 *
 */
/*ARGSUSED*/
void
addFirstFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    int first = 1;
    int i, top;

    if (Mode != ADD_MODE) {
	return;
    }
    if (SelectionItem == -1) {
	AddGroupPosition = SListGetHighlight(TopList);
	if (AddGroupPosition >= 0) {
	    strcpy(newGroup, AddGroups[AddGroupPosition]->name);
	    (void) addToNewsrcBeginning(newGroup, status);
	}
    } else {
	for (i = 0; i < AddGroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		if (first) {
		    strcpy(newGroup, AddGroups[i]->name);
		    (void) addToNewsrcBeginning(newGroup, status);
		    (void) strcpy(oldGroup, newGroup);
		    first = 0;
		} else {
		    strcpy(newGroup, AddGroups[i]->name);
		    (void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
		    (void) strcpy(oldGroup, newGroup);
		}
	    }
	}
    }
    resetSelection();

    /* update the text window */
    if ((top = SListGetTop(TopList)) == -1) top = 0;
    redrawAddListWidget(top);
    if (AddGroupItems <= 0) {
	exitAddMode();
    }
    return;
}

/*
 * add the currently selected group(s) to the end of the .newsrc file
 * and subscribe to them.
 */
/*ARGSUSED*/
void
addLastFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    int i, top;

    if (Mode != ADD_MODE) {
	return;
    }
    if (SelectionItem == -1) {
	AddGroupPosition = SListGetHighlight(TopList);
	if (AddGroupPosition >= 0) {
	    strcpy(newGroup, AddGroups[AddGroupPosition]->name);
	    (void) addToNewsrcEnd(newGroup, status);
	}
    } else {
	for (i = 0; i < AddGroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		strcpy(newGroup, AddGroups[i]->name);
		(void) addToNewsrcEnd(newGroup, status);
	    }
	}
    }
    resetSelection();
    if ((top = SListGetTop(TopList)) == -1) top = 0;
    redrawAddListWidget(top);
    if (AddGroupItems <= 0) {
	exitAddMode();
    }
    return;
}


/* entering the name of a newsgroup to add after */

#define XRNadd_ADD 1
#define XRNadd_ABORT 0

static Widget AddBox = (Widget) 0;

/*
 * get the newsgroup to add a new newsgroup after in the .newsrc file
 */
/*ARGSUSED*/
static void
addHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    int first = 1;
    int i, top;

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    busyCursor();
    PopDownDialog(AddBox);

    /* Handle "Cancel" press. */
    if (client_data == (caddr_t) XRNadd_ABORT) {
	unbusyCursor();
	inCommand = 0;
	return;
    }
    if (SelectionItem == -1) {
	AddGroupPosition = SListGetHighlight(TopList);
	if (AddGroupPosition >= 0)  {
	    strcpy(newGroup, AddGroups[AddGroupPosition]->name);
	    (void)addToNewsrcAfterGroup(newGroup,GetDialogValue(AddBox),status);
	    resetSelection();
	    if ((top = SListGetTop(TopList)) == -1) top = 0;
	    redrawAddListWidget(top);
	    if (AddGroupItems <= 0) {
		exitAddMode();
	    }
	}
	unbusyCursor();
	inCommand = 0;
	return;
    }
    for (i = 0; i < AddGroupItems; i++) {
	if (SListIsSelected(TopList, i)) {
	    if (first) {
		strcpy(newGroup, AddGroups[i]->name);
		if (!addToNewsrcAfterGroup(newGroup,
				      GetDialogValue(AddBox),
				      status) == GOOD_GROUP) {
		    /* Avoid deadlock */
		    unbusyCursor();
		    inCommand = 0;
		    return;
		} else {
		    first = 0;
		}
	    } else {
		strcpy(newGroup, AddGroups[i]->name);
		(void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
	    }
	    (void) strcpy(oldGroup, newGroup);
	}
    }
    resetSelection();
    if ((top = SListGetTop(TopList)) == -1) top = 0;
    redrawAddListWidget(top);
    if (AddGroupItems <= 0) {
	exitAddMode();
    }
    unbusyCursor();
    inCommand = 0;
    return;
}

/*
 * subscribe to a new newsgroup, adding after a particular group in the
 * .newsrc file
 */
/*ARGSUSED*/
void
addAfterFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Add",   addHandler, (caddr_t) XRNadd_ADD},
	{"Cancel", addHandler, (caddr_t) XRNadd_ABORT},
    };

    if (Mode != ADD_MODE) {
	return;
    }
    if (AddBox == (Widget) 0) {
	AddBox = CreateDialog(TopLevel, "Subscribe After Select",
				"After which newsgroup?",
				DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(AddBox);
    return;
}


/*
 * add a group to the end of the .newsrc file as unsubscribed
 */
/*ARGSUSED*/
void
addUnsubFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int status = UNSUBSCRIBE;
    int i, top;

    if (Mode != ADD_MODE) {
	return;
    }
    if (SelectionItem == -1) {
	AddGroupPosition = SListGetHighlight(TopList);
	if (AddGroupPosition >= 0) {
	    strcpy(newGroup, AddGroups[AddGroupPosition]->name);
	    (void) addToNewsrcEnd(newGroup, status);
	}
    } else {	
	for (i = 0; i < AddGroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		strcpy(newGroup, AddGroups[i]->name);
		(void) addToNewsrcEnd(newGroup, status);
	    }
	}
    }
    resetSelection();
    if ((top = SListGetTop(TopList)) == -1) top = 0;
    redrawAddListWidget(top);
    if (AddGroupItems <= 0){
	exitAddMode();
    }
    return;
}


#define XRNsave_ABORT 0
#define XRNsave_SAVE  1

static Widget SaveBox = (Widget) 0;  /* box for typing in the name of a file */

/*
 * handler for the save box
 */
/*ARGSUSED*/
static void
saveHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    int i, start, end, top, current;

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    busyCursor();
    if ((top = SListGetTop(TopList)) == -1) top = 0;
    current = ArtPosition;
    PopDownDialog(SaveBox);
    if ((int) client_data != XRNsave_ABORT) {
	if (SelectionItem != -1) {
	    for (i = 0; i < SubjectItems; i++) {
		if (SListIsSelected(TopList, i)) {
		    start = ArticleNumbers[i];
		    end = start;
		    if (!saveRangeOfArticles(GetDialogValue(SaveBox),
					     start, end))
			break;
		    markArticle(ArticleNumbers[i], 'S');
		    SListUpdateLine(TopList, i);
		}
	    } 
	    resetSelection();
	    SListSetTop(TopList, top);
	    setBottomInfoLine(currentQuestion());
	} else {
	    if (saveCurrentArticle(GetDialogValue(SaveBox))) {
		markArticle(ArticleNumbers[current], 'S');
		SListUpdateLine(TopList, current);
	    }
	    resetSelection();
	    SListSetTop(TopList, top);
	    setBottomInfoLine(currentQuestion());
	}
	if (SaveString) {
	    XtFree(SaveString);
	}
	SaveString = XtNewString(GetDialogValue(SaveBox));
    }
    unbusyCursor();
    inCommand = 0;
    resetSaveFlag();
    return;
}    

/*
 * query the user about saving an article
 *
 *    brings up a dialog box
 *
 *    returns: void
 *
 */
/*ARGSUSED*/
void
artSaveFunction(widget, ev, params, num_params)
Widget widget;
XEvent *ev;
String *params;
Cardinal *num_params;
{
    static struct DialogArg args[] = {
	{"Save",   saveHandler, (caddr_t) XRNsave_SAVE},
	{"Cancel", saveHandler, (caddr_t) XRNsave_ABORT},
    };

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (num_params && *num_params == 1) {
	int i, start, end;
	char *ssstring;
	ssstring = SaveString;
	SaveString = XtNewString(params[0]);
 	busyCursor();

	if (SelectionItem != -1) {
	    for (i = 0; i < SubjectItems; i++) {
		if (SListIsSelected(TopList, i)) {
		    start = ArticleNumbers[i];
		    end = start;
		    (void) saveRangeOfArticles(SaveString, start, end);
		}
	    } 
	    markFunction('S');
	} else {
	    if (saveCurrentArticle(SaveString)) {
		markArticle(ArticleNumbers[ArtPosition], 'S');
		SListUpdateLine(TopList, ArtPosition);
	    }
	}
	setBottomInfoLine(currentQuestion());
	if (SaveString)
	    XtFree(SaveString);
	SaveString = ssstring;
	return;
    }
    if (SaveBox == (Widget) 0) {
	if (app_resources.saveString) {
	    if (SaveString) {
		XtFree(SaveString);
	    }
	    SaveString = XtNewString(app_resources.saveString);
	}
	SaveBox = CreateDialog(
			(app_resources.twoWindows ? ArtTopLevel : TopLevel),
			"Save File Name Select",
			"File Name?",
			SaveString == NULL ? DIALOG_TEXT : SaveString,
			args, XtNumber(args));
    } else {
	SetDialogValue(SaveBox, SaveString);
    }
    PopUpDialog(SaveBox);
    return;
}

#ifdef XLATE
/*
 * translate an article
 */
/*ARGSUSED*/
void
artXlateFunction(widget)
Widget widget;
{
    char *text, *question;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (toggleXlation(&text, &question) == XRN_OKAY) {
	redrawArticleWidget(text, question);
    }
    return;
}
#endif /* XLATE */

/*ARGSUSED*/
void
artPrintFunction(widget)
Widget widget;
{
    static char buffer[512];
    struct newsgroup *newsgroup = CurrentGroup;
    art_num art = newsgroup->current;
    int i, start, end, top;

#ifdef VMS
    int status;
    short msglen;
    static int printNumber = 0;
    struct dsc$descriptor_s buf_desc = { sizeof(buffer)-1,
	DSC$K_DTYPE_T, DSC$K_CLASS_S, buffer };

    if ((top = SListGetTop(TopList)) == -1) top = 0;

    if (!app_resources.delayedPrint) {
	printNumber++;
	(void) sprintf(buffer, "%sXRN_PRINT_%u.LIS", app_resources.tmpDir,
		printNumber);
    } else {
	(void) sprintf(buffer, "%sXRN_PRINT_%u.LIS",
		app_resources.tmpDir, getpid());
    }
    if (SelectionItem != -1) {
	for (i = 0; i < SubjectItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		start = ArticleNumbers[i];
		end = start;
		if (!saveRangeOfArticles(buffer, start, end))
			break;
		markArticle(ArticleNumbers[i], 'P');
		SET_PRINTED(newsgroup->artStatus[INDEX(art)]);
		SListUpdateLine(TopList, i);
	    }
	} 
	resetSelection();
	SListSetTop(TopList, top);
	if (!app_resources.delayedPrint) {
	    (void) sprintf(buffer, "%s %sXRN_PRINT_%u.LIS",
		    app_resources.printCommand, app_resources.tmpDir,
		    printNumber);
	    status = system(buffer);
	    if (status & 1) {
		info("Article sucessfully queued");
	    } else {
		status = sys$getmsg(status, &msglen, &buf_desc, 0, 0);
		buffer[msglen] = '\0';
		mesgPane(XRN_SERIOUS, buffer);
	    }
	} else {
	    printPending = True;
	    info("Article(s) saved for printing");
	}
    } else {
	if (saveCurrentArticle(buffer)) {
	    if (!app_resources.delayedPrint) {
		(void) sprintf(buffer, "%s %sXRN_PRINT_%u.LIS",
		    app_resources.printCommand, app_resources.tmpDir,
		    printNumber);
		status = system(buffer);
		if (status & 1) {
		    info("Article sucessfully queued");
		    markArticle(ArticleNumbers[ArtPosition], 'P');
		    SET_PRINTED(newsgroup->artStatus[INDEX(art)]);
		} else {
		    status = sys$getmsg(status, &msglen, &buf_desc, 0, 0);
		    buffer[msglen] = '\0';
		    mesgPane(XRN_SERIOUS, buffer);
		}
	    } else {
		printPending = True;
		info("Article saved for printing");
	    }
	}
    }

}
#else /* not VMS */

    if ((top = SListGetTop(TopList)) == -1) top = 0;

    if (!app_resources.delayedPrint) {
	(void) sprintf(buffer, "| %s", app_resources.printCommand);
    } else {
	(void) sprintf(buffer, "%sXrn_print_%u",
		app_resources.tmpDir, getpid());
    }
    if (SelectionItem != -1) {
	for (i = 0; i < SubjectItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		start = ArticleNumbers[i];
		end = start;
		if (!saveRangeOfArticles(buffer, start, end))
			break;
		markArticle(ArticleNumbers[i], 'P');
		SET_PRINTED(newsgroup->artStatus[INDEX(art)]);
		SListUpdateLine(TopList, i);
		if (app_resources.delayedPrint) {
		    printPending = True;
		    info("Article(s) saved for printing");
		}
	    }
	} 
	resetSelection();
	SListSetTop(TopList, top);
    } else {
	if (saveCurrentArticle(buffer)) {
	    markArticle(ArticleNumbers[ArtPosition], 'P');
	    SET_PRINTED(newsgroup->artStatus[INDEX(art)]);
	    if (app_resources.delayedPrint) {
		printPending = True;
		info("Article saved for printing");
	    }
	}
    }
    return;
}
#endif /* VMS */

void
artFlushQueueFunction(widget)
Widget widget;
{
    flushPrintQueue();
}

/*
 * Allow user to post a reply to the currently posted article
 */
/*ARGSUSED*/
void
artReplyFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    reply();
    return;
}

/*
 * Allow user to forward an article to a user(s)
 */
/*ARGSUSED*/
void
artForwardFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    forward();
    return;
}

/*
 * Allow user to gripe
 */
/*ARGSUSED*/
void
artGripeFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    gripe();
    return;
}

/*
 * Allow user to post a followup to the currently displayed article
 */
/*ARGSUSED*/
void
artFollowupFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    followup();
    return;
}

/*
 * Post a followup and reply to the author of current article
 */
/*ARGSUSED*/
void
artFollowupReplyFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    followupAndReply();
    return;
}

/*
 * Allow user to cancel the currently displayed article
 */
/*ARGSUSED*/
void
artCancelFunction(widget)
Widget widget;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    cancelArticle();
    return;
}

/*
 * decrypt a joke
 */
/*ARGSUSED*/
void
artRot13Function(widget)
Widget widget;
{
    char *text, *question;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (toggleRotation(&text, &question) == XRN_OKAY) {
	redrawArticleWidget(text, question);
    }
    return;
}

/*ARGSUSED*/
void
artHeaderFunction(widget)
Widget widget;
{
    char *text, *question;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (toggleHeaders(&text, &question) == XRN_OKAY) {
	redrawArticleWidget(text, question);
    }
    return;
}

#define SUB_STRING   "subscribed  "
#define UNSUB_STRING "unsubscribed"

/*
 * Quit all groups mode.
 */
/*ARGSUSED*/
void
allQuitFunction(widget)
Widget widget;
{
    int i;
    if (Mode != ALL_MODE) {
	return;
    }
    /* destroy strings */
    
    SListLoad(TopList, 0);
    if (AllGroupItems > 0) {
	if (AllGroups != (ng_num *)0) {
	    FREE(AllGroups);
	    AllGroups = NIL(ng_num);
	}
    }

    switchToNewsgroupMode();
    return;
}

/*
 * Make the selected group(s) subscribed to, and leave them in
 * their current position in the newsrc file.
 */
/*ARGSUSED*/
void
allSubFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int i;

    if (Mode != ALL_MODE) {
	return;
    }
    if (SelectionItem < 0) {
	AllPosition = SListGetHighlight(TopList);
	if (AllPosition >= 0) {
	    strcpy(newGroup, groupName(AllGroups[AllPosition]));
	    if (gotoNewsgroup(newGroup) != GOOD_GROUP) {
		mesgPane(XRN_SERIOUS, "Bad group: %s", newGroup);
		return;
	    }
	    subscribe();
	}
    } else {
	AllPosition = 0;
	for (i = 0; i < AllGroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		if (AllPosition == 0) AllPosition = i;
		strcpy(newGroup, groupName(AllGroups[i]));
		if (gotoNewsgroup(newGroup) != GOOD_GROUP) {
		    mesgPane(XRN_SERIOUS, "Bad group: %s", newGroup);
		    return;
		}
		subscribe();
	    }
	}
    }
    updateAllWidget(SListGetTop(TopList), SListGetBottom(TopList));
    resetSelection();
    return;
}

/*
 * Mark the selected group(s) as subscribed to, and move them to the
 * beginning of the newsrc file.
 */
/*ARGSUSED*/
void
allFirstFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int i;

    if (Mode != ALL_MODE) {
	return;
    }
    if (SelectionItem < 0) {
	AllPosition = SListGetHighlight(TopList);
	if (AllPosition >= 0) {
	    strcpy(newGroup, groupName(AllGroups[AllPosition]));
	    (void) addToNewsrcBeginning(newGroup, SUBSCRIBE);
	    if (AllStatus == 0) {
		redrawAllWidget(saveTop);
	    } else {
		updateAllWidget(AllPosition, AllPosition);
	    }
	}
    } else {
	AllPosition = 0;
	for (i = 0; i < AllGroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		if (AllPosition == 0) AllPosition = i;
		strcpy(newGroup, groupName(AllGroups[i]));
		(void) addToNewsrcBeginning(newGroup, SUBSCRIBE);
		if (AllStatus != 0) {
		    updateAllWidget(i,i);
		}
	    }
	}
    }
    resetSelection();
    if (AllStatus == 0) {
	redrawAllWidget(saveTop);
    } 
    subjSetPos(TopList, AllPosition, True);
    return;
}

/*
 * Mark the selected group(s) as subscribed to, and move them
 * to the end of the newsrc file.
 */
/*ARGSUSED*/
void
allLastFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    int i;

    if (Mode != ALL_MODE) {
	return;
    }
    if (SelectionItem < 0) {
	AllPosition = SListGetHighlight(TopList);
	if (AllPosition >= 0) {
	    strcpy(newGroup, groupName(AllGroups[AllPosition]));
	    (void) addToNewsrcEnd(newGroup, status);
	    if (AllStatus == 0) {
		redrawAllWidget(AllPosition);
	    } else {
		updateAllWidget(AllPosition, AllPosition);
	    }
	}
    } else {
	AllPosition = 0;
	for (i = 0; i < AllGroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		if (AllPosition == 0) AllPosition = i;
		strcpy(newGroup, groupName(AllGroups[i]));
		(void) addToNewsrcEnd(newGroup, status);
		if (AllStatus != 0) {
		    updateAllWidget(i, i);
		}
	    }
	}
    }
    resetSelection();
    if (AllStatus == 0) {
	redrawAllWidget(AllPosition);
    } 
    subjSetPos(TopList, AllPosition, True);
    return;
}

static Widget AllBox = (Widget) 0;

/*
 * Mark the selected group(s) as subscribed to, and place them
 * after the group name (entered in the dialog box) in the newsrc file.
 */
/*ARGSUSED*/
static void
allHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int i;
    int first = 1;

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;
    PopDownDialog(AllBox);
    busyCursor();

    /* Handle "Cancel" press. */
    if (client_data == (caddr_t) XRNadd_ABORT) {
	unbusyCursor();
	inCommand = 0;
	return;
    }
    if (SelectionItem < 0) {
	AllPosition = SListGetHighlight(TopList);
	if (AllPosition >= 0) {
	    strcpy(newGroup, groupName(AllGroups[AllPosition]));
	    if (addToNewsrcAfterGroup(newGroup, GetDialogValue(AllBox),
				      SUBSCRIBE) == GOOD_GROUP) {
		if (AllStatus == 0) {
	    	    redrawAllWidget(AllPosition);
		} else {
		    updateAllWidget(AllPosition, AllPosition);
		}
	    }
	}
    } else {
	AllPosition = 0;
	for (i = 0; i < AllGroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		if (AllPosition == 0) AllPosition = i;
		strcpy(newGroup, groupName(AllGroups[i]));
		if (first) {
		    if (addToNewsrcAfterGroup(newGroup, GetDialogValue(AllBox),
				      SUBSCRIBE) == GOOD_GROUP) {
			updateAllWidget(i, i);
		    } else {
			/* Avoid deadlock */
			unbusyCursor();
			inCommand = 0;
			return;
		    }
		} else {
		    strcpy(newGroup, groupName(AllGroups[i]));
		    (void)addToNewsrcAfterGroup(newGroup, oldGroup, SUBSCRIBE);
		    updateAllWidget(i, i);
		}
	    }
	    (void) strcpy(oldGroup, newGroup);
	}
    }
    resetSelection();
    if (AllStatus == 0) {
	redrawAllWidget(AllPosition);
    } 
    subjSetPos(TopList, AllPosition, True);
    unbusyCursor();
    inCommand = 0;
    return;
}

/*
 * Put up a dialog box for the user to enter a group name after which
 * the selected articles should be placed.
 */
/*ARGSUSED*/
void
allAfterFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Subscribe", allHandler, (caddr_t) XRNadd_ADD},
	{"Cancel",    allHandler, (caddr_t) XRNadd_ABORT},
    };

    if (Mode != ALL_MODE) {
	return;
    }
    if (SelectionItem < 0) {
	mesgPane(XRN_INFO, "You must select a group or groups first");
	return;
    }
    if (AllBox == (Widget) 0) {
	AllBox = CreateDialog(TopLevel, "Move After Select",
				  "After which newsgroup?",
				  DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(AllBox);
    return;
}

/*
 * Mark the selected group(s) as unsubscribed, leaving their position
 * in the newsrc file unchanged.
 */
/*ARGSUSED*/
void
allUnsubFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int i;

    if (Mode != ALL_MODE) {
	return;
    }
    if (SelectionItem < 0) {
	AllPosition = SListGetHighlight(TopList);
	if (AllPosition >= 0) {
	    strcpy(newGroup, groupName(AllGroups[AllPosition]));
	    if (gotoNewsgroup(newGroup) != GOOD_GROUP) {
		mesgPane(XRN_SERIOUS, "Bad group: %s\n", newGroup);
		return;
	    }
	    unsubscribe();
	}
    } else {
	AllPosition = 0;
	for (i = 0; i < AllGroupItems; i++) {
	    if (AllPosition == 0) AllPosition = i;
	    if (SListIsSelected(TopList, i)) {
		strcpy(newGroup, groupName(AllGroups[i]));
		if (gotoNewsgroup(newGroup) != GOOD_GROUP) {
		    mesgPane(XRN_SERIOUS, "Bad group: %s\n", newGroup);
		    return;
		}
		unsubscribe();
	    }
	}
    }
    updateAllWidget(SListGetTop(TopList), SListGetBottom(TopList));
    resetSelection();
    return;
}

/*
 * called when the user wants to scroll the all groups window
 */
/*ARGSUSED*/
void
allScrollFunction(widget)
Widget widget;
{
    if (Mode != ALL_MODE) {
	return;
    }
    AllPosition += 9;
    if (AllPosition >= AllGroupItems)
	AllPosition = AllGroupItems - 1;
    subjSetPos(TopList, AllPosition, True);
    return;
}

/*
 * called when the user wants to scroll the all groups window
 */
/*ARGSUSED*/
void
allScrollBackFunction(widget)
Widget widget;
{
    if (Mode != ALL_MODE) {
	return;
    }
    AllPosition -= 9;
    if (AllPosition < 0)
	AllPosition = 0;
    subjSetPos(TopList, AllPosition, True);
    return;
}

/*
 * Go to the current newsgroup.  The current
 * group is either the first group of a selection,
 * or, if there is no selection, the group the cursor
 * is currently on (if any).
 */
/*ARGSUSED*/
void
allGotoFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int gbeg, gend;
    int i;

    if (Mode != ALL_MODE) {
	return;
    }

    if ((saveTop = SListGetTop(TopList)) == -1) saveTop = 0;

    /* get the current group name */

    if (SelectionItem < 0) {
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem < 0) SelectionItem = 0;
	SelectionCount = 1;
    }
    gbeg = SelectionItem;
    gend = SelectionItem + SelectionCount;
    if (gbeg >= AllGroupItems) gbeg = AllGroupItems - 1;
    if (gend > AllGroupItems) gend = AllGroupItems;
    resetSelection();
    if (gbeg < 0 || gend < 0) return;
    saveTop = gbeg;

    strcpy(newGroup, groupName(AllGroups[gbeg]));

    /* jump to the newsgroup */

    setTitleBar("Opening newsgroup - please wait");
    if (jumpToNewsgroup(newGroup) == GOOD_GROUP) {
	(void) strcpy(LastGroup, newGroup);
	
	/* free string */

	if (AllGroupItems > 0) {
	    if (AllGroups != (ng_num *)0) {
		FREE(AllGroups);
		AllGroups = (ng_num *) 0;
	    }
	}
	(void) switchToArticleMode(app_resources.autoRead);
	return;
    }
    return;
}

/*
 * Open the current newsgroup.  The current
 * group is either the first group of a selection,
 * or, if there is no selection, the group the cursor
 * is currently on (if any).
 */
/*ARGSUSED*/
void
allOpenFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int gbeg, gend;
    int i;

    if (Mode != ALL_MODE) {
	return;
    }

    if ((saveTop = SListGetTop(TopList)) == -1) saveTop = 0;

    /* get the current group name */

    if (SelectionItem < 0) {
	SelectionItem = SListGetHighlight(TopList);
	if (SelectionItem < 0) SelectionItem = 0;
	SelectionCount = 1;
    }
    gbeg = SelectionItem;
    gend = SelectionItem + SelectionCount;
    if (gbeg >= AllGroupItems) gbeg = AllGroupItems - 1;
    if (gend > AllGroupItems) gend = AllGroupItems;
    resetSelection();
    if (gbeg < 0 || gend < 0) return;
    strcpy(newGroup, groupName(AllGroups[gbeg]));

    /* jump to the newsgroup */

    setTitleBar("Opening newsgroup - please wait");
    if (jumpToNewsgroup(newGroup) == GOOD_GROUP) {
	(void) strcpy(LastGroup, newGroup);
	
	/* free string */

	if (AllGroupItems > 0) {
	    if (AllGroups != (ng_num *)0) {
		FREE(AllGroups);
		AllGroups = (ng_num *) 0;
	    }
	}
	(void) switchToArticleMode(0);
	return;
    }
    return;
}

/*
 * Post to the current newsgroup.  The current
 * group is either the first group of a selection,
 * or, if there is no selection, the group the cursor
 * is currently on (if any).
 */
/*ARGSUSED*/
void
allPostFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    int first;
    int i;

    if (Mode != ALL_MODE) {
	return;
    }
    
    /* get the current group name */

    if (SelectionItem < 0) {
	first = SListGetHighlight(TopList);
	resetSelection();
	if (first < 0) return;
	if (first >= AllGroupItems) {
	    first = AllGroupItems - 1;
	}
	AllPosition = first;
	strcpy(newGroup, groupName(AllGroups[AllPosition]));
	resetSelection();
    } else {
	for (i = 0; i < AllGroupItems; i++) {
	    if (SListIsSelected(TopList, i)) {
		strcpy(newGroup, groupName(AllGroups[i]));
	    }
	}
	resetSelection();
    }
    setNewsgroup(newGroup);
    post(1);

    return;
}

/*
 * Make note of the groups that were selected
 * to be moved.
 */
/*ARGSUSED*/
void
allSelectFunction(widget)
Widget widget;
{
    int i, count;

    if (Mode != ALL_MODE || AllGroupItems == 0) {
	return;
    }
    if (selSize < AllGroupItems) {
	if (selList != NULL) FREE(selList);
	selList = XtMalloc(AllGroupItems+1);
	selSize = AllGroupItems;
    }
    count = 0;
    for (i=0; i < AllGroupItems;i++) {
	if (SListIsSelected(TopList, i)) {
	    selList[i] = 1;
	    count++;
	} else {
	    selList[i] = 0;
	}
    }
    if (count == 0) {
	count++;
	selList[SListGetHighlight(TopList)] = 1;
    }
    resetSelection();
    selCount = count;
    return;
}

/*
 * Move the previously selected groups to the position before the
 * current selection
 */
/*ARGSUSED*/
void
allMoveFunction(widget)
Widget widget;
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    int moveTo;
    int moveFrom;
    int first = 1;

    if (Mode != ALL_MODE) {
	return;
    }
    if (selCount == 0) {
	mesgPane(XRN_INFO, "No groups were selected");
	return;	
    }
    if (SelectionItem < 0) {
	mesgPane(XRN_INFO, "You must select a group to move before");
	return;
    }
    moveTo = SelectionItem;
    if (moveTo >= AllGroupItems) moveTo = AllGroupItems - 1;
    for (moveFrom = 0; moveFrom < AllGroupItems; moveFrom++) {
	if (selList[moveFrom] != 0) {
	    currentMode(getStatusString(AllGroups[moveFrom]),
			newGroup, &status);
	    if (moveTo <= 0) {
		moveTo = 0;
	    	(void) addToNewsrcBeginning(newGroup, status);
	    	(void) strcpy(oldGroup, newGroup);
	    } else {
		if (first) {
		    strcpy(oldGroup, groupName(AllGroups[moveTo-1]));
		    (void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
		    first = 0;
		} else {
		    currentMode(getStatusString(AllGroups[moveFrom]),
				newGroup, &status);
		    (void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
		}
		(void) strcpy(oldGroup, newGroup);
	    }
	}
    }
    selCount = 0;
    resetSelection();
    redrawAllWidget(moveTo);
    subjSetPos(TopList, moveTo, True);
    return;
}

/* 
 * Change the order the groups appear on the screen.
 */
/*ARGSUSED*/
void
allToggleFunction(widget)
Widget widget;
{
    
    if (Mode != ALL_MODE) {
	return;
    }

    AllStatus = (AllStatus == 0) ? 1 : 0;

    if (AllGroupItems > 0) {
	if (AllGroups != (ng_num *)0) {
	    FREE(AllGroups);
	}
    }
    AllGroups = getStatusList(AllStatus);

    for (AllGroupItems = 0; AllGroups[AllGroupItems] != -1; AllGroupItems++);

    SListLoad(TopList, AllGroupItems);
    currentTop = 0;
    resetSelection();
    subjSetPos(TopList, 0, True);

    return;
}

#define XRNallSearch_ABORT 0
#define XRNallSearch_FORWARD 1
#define XRNallSearch_BACK 2

static Widget AllSearchBox = (Widget) 0;

#undef CLEANUP
#define CLEANUP \
	if (AllSearchBox) PopDownDialog(AllSearchBox); \
	inCommand = 0; \
	unbusyCursor();


static int
groupSearch(dir)
int dir;
{
#ifdef SYSV_REGEX
    extern char *regcmp();
    extern char *regex();
#else
    char *re_comp();		/* regular expression routine */
#endif
    static char *reRet;		/* returned by re_comp/regcmp */
    char newgroup[128];		/* group name of current line */

    abortClear();

    if (LastRegexp != NIL(char)) {
#ifdef SYSV_REGEX
	if ((reRet = regcmp(LastRegexp, NULL)) == NULL) {
#else
	if ((reRet = re_comp(LastRegexp)) != NULL) {
#endif
	    /* bad regular expression */
#ifdef SYSV_REGEX
	    mesgPane(XRN_SERIOUS, "error in regular expression");
#else
	    mesgPane(XRN_SERIOUS, reRet);
#endif
	    failedSearch();
	    return ERROR;
	}
    }

    if (dir == FORWARD) {
	for (;;) {
	    if (abortP()) {
		return ABORT;
	    }
	    if (AllGroups[AllPosition] == -1) {
		if (AllPosition == 0) {

		    /* the string is null, no more articles are left */

		    return EXIT;
		}
		return NOMATCH;
	    }
	    AllPosition = AllPosition + 1;
	    if (AllGroups[AllPosition] == -1) {

		/* reached end of string */

		return NOMATCH;
	    }
	    utLowerCase(newgroup, groupName(AllGroups[AllPosition]),
			sizeof(newgroup) - 1);

#ifdef SYSV_REGEX
	    if (regex(reRet, newgroup) != NULL) {
#else
	    if (re_exec(newgroup)) {
#endif
		/* found a match to the regular expression */

		return MATCH;
	    }
	}
    } else {
	startSearch();
	for (;;) {
	    if (abortP()) {

		/* reset pointers back to where we began, since the */
		/* search was aborted */

		failedSearch();
		return ABORT;
	    }
	    if ((AllPosition == 0) && AllGroups[AllPosition] == -1) {

		/* no more articles remain, return to Newgroup mode */

		return EXIT;
	    }
	    if (AllPosition != 0) {

		/* we are still within the subject list */

		AllPosition = AllPosition - 1;
		utLowerCase(newgroup, groupName(AllGroups[AllPosition]),
			sizeof(newgroup) - 1);

#ifdef SYSV_REGEX
		if (regex(reRet, newgroup) != NULL) {
#else
		if (re_exec(newgroup)) {
#endif
		    /* an article matching the regular expression was found */

		    return MATCH;
		}
	    } else {
		failedSearch();
		return NOMATCH;
	    }
	}
    }
}
/*ARGSUSED*/
static void 
allSearchHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *regexpr;
    int SavePosition;
    int status;
    int direction;

    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return;
    }
    inCommand = 1;

    busyCursor();
    resetSelection();
    if ((int) client_data == XRNallSearch_ABORT) {
	CLEANUP;
	return;
    }
    if (AllPosition >= AllGroupItems) AllPosition = AllGroupItems - 1;
    SavePosition = AllPosition;
    regexpr = GetDialogValue(AllSearchBox);
    if (*regexpr == 0) {
	if (LastRegexp == NIL(char)) {
	    mesgPane(XRN_INFO, "No previous regular expression");
	    CLEANUP;
	    return;	   
	}
	regexpr = LastRegexp;
    } else {
	if (LastRegexp != NIL(char)) {
	    FREE(LastRegexp);
	}
	LastRegexp = XtNewString(regexpr);
    }
    
    /* XXX */
    if (AllSearchBox) PopDownDialog(AllSearchBox);

    direction = ((int) client_data == XRNallSearch_FORWARD) ? FORWARD : BACK;
    LastSearch = direction;
    status = groupSearch(direction);
    switch (status) {
      case ABORT:
	infoNow("search aborted");
	AllPosition = SavePosition;
	subjSetPos(TopList, AllPosition, True);
	break;
      case NOMATCH:
	(void) sprintf(error_buffer, "Search for expression %s: no match was found",
		       LastRegexp);
	AllPosition = SavePosition;
	subjSetPos(TopList, AllPosition, True);
	infoNow(error_buffer);
      case ERROR:
	AllPosition = SavePosition;
	subjSetPos(TopList, AllPosition, True);
	break;
      case MATCH:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	subjSetPos(TopList, AllPosition, True);
	break;
      case EXIT:
	break;
    }
    CLEANUP;
    return;
}


/*ARGSUSED*/
void
allSearchFunction(widget)
Widget widget;
{
    static struct DialogArg args[] = {
	{"Forward", allSearchHandler, (caddr_t) XRNallSearch_FORWARD},
	{"Back",    allSearchHandler, (caddr_t) XRNallSearch_BACK},
	{"Cancel",  allSearchHandler, (caddr_t) XRNallSearch_ABORT},
    };
    
    if (Mode != ALL_MODE) {
	return;
    }
    if (AllSearchBox == (Widget) 0) {
	AllSearchBox = CreateDialog(TopLevel, "Group Search Select",
				  "Regular Expression?",
				  DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(AllSearchBox);
    return;
}

/*ARGSUSED*/
void
allCancelSearchFunction(widget)
Widget widget;
{
    abortSet();
}

/*ARGSUSED*/
void
allContinueFunction(widget)
Widget widget;
/*
 * Continue a previously started regular expression
 * search of the subject lines.  Search for same
 * regular expression, in same direction.
 */
{
    int SavePosition;
    int status;

    if (LastRegexp == NULL) {
	mesgPane(XRN_INFO, "No previous regular expression");
	return;
    }
    (void) sprintf(error_buffer, "Searching for expression %s", LastRegexp);
    info(error_buffer);
    resetSelection();
    SavePosition = AllPosition;
    status = groupSearch(LastSearch);

    switch (status) {
      case ABORT:
	infoNow("search aborted");
	AllPosition = SavePosition;
	subjSetPos(TopList, AllPosition, True);
	return;
      case NOMATCH:
	(void) sprintf(error_buffer, "Search for expression %s: no match was found", LastRegexp);
	AllPosition = SavePosition;
	subjSetPos(TopList, AllPosition, True);
	infoNow(error_buffer);
      case ERROR:
	AllPosition = SavePosition;
	subjSetPos(TopList, AllPosition, True);
	return;
      case MATCH:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);    
	subjSetPos(TopList, AllPosition, True);
	return;
      case EXIT:
	return;
    }
}

void
determineMode()
/*
 * determine the initial mode and set up Text, TopButtonBox, and Question
 */
{
    /* set mode, handle text and question */
    PreviousMode = Mode;
    
    /*
     * article specific buttons, only create them if this is the first
     * call to 'determineMode'
     */
    if (PreviousMode == NO_MODE) {
	createButtons(-1);
	XtManageChildren(ArtSpecButtons, ArtSpecButtonListCount);
    }
    if ((AddGroups = newGroups()) != (struct newsgroup **)0) {
	Mode = ADD_MODE;
	swapMode();
	setTopInfoLine("Select groups to `add', `exit' unsubscribes remaining groups");
	GroupPosition = 0;
	redrawAddListWidget(0);
	if (XtIsRealized(TopLevel))
	    xmSetIconAndName(NewGroupsIcon);
    } else {
	Mode = NEWSGROUP_MODE;
	GroupPosition = 0;
	swapMode();
	redrawNewsgroupListWidget();
	/* update the question */
	if (GroupItems == 0) {
	    setTopInfoLine("No more unread articles in the subscribed to newsgroups");
	    if (XtIsRealized(TopLevel))
		xmSetIconAndName(ReadIcon);
	} else {
	    setTopInfoLine("Questions apply to current selection or cursor position");
	    if (XtIsRealized(TopLevel))
		xmSetIconAndName(UnreadIcon);
	}
    }
    setBottomInfoLine("");
    addTimeOut();
    
    return;
}

static void
listSelect(w, closure, call_data)
Widget		    w;
caddr_t		    closure;
SListCallbackStruct *call_data;
{
    SListCallbackStruct *cb = call_data;
    char buffer[256];

    SelectionItem = cb->item_number;
    SelectionCount = cb->item_count;
#ifdef TITLEBAR
    if (cb->select_count == 0) {
	setTitleBar(NULL);
	return;
    }
    if (Mode == NEWSGROUP_MODE) {
	sprintf(buffer, "Newsgroup %s %sselected",
		groupName(NewsGroups[SelectionItem]),
		(cb->select_count == 1) ? "" : "(and others) ");
	setTitleBar(buffer);
    }
    if (Mode == ALL_MODE) {
	sprintf(buffer, "Newsgroup %s %sselected",
		groupName(AllGroups[SelectionItem]),
		(cb->select_count == 1) ? "" : "(and others) ");
	setTitleBar(buffer);
    }
    if (Mode == ADD_MODE) {
	sprintf(buffer, "Newsgroup %s %sselected",
		AddGroups[SelectionItem]->name,
		(cb->select_count == 1) ? "" : "(and others) ");
	setTitleBar(buffer);
    }
    if (Mode == ARTICLE_MODE) {
	sprintf(buffer, "Article %d %sselected",
		ArticleNumbers[SelectionItem],
		(cb->select_count == 1) ? "" : "(and others) ");
	setTitleBar(buffer);
    }
    return;
#endif
}

static void
DoubleClick(w, closure, call_data)
Widget		    w;
caddr_t		    closure;
SListCallbackStruct *call_data;
{
    SListCallbackStruct *cb = call_data;

    SelectionItem = cb->item_number;

    if (Mode == NEWSGROUP_MODE) {
	if (app_resources.autoRead) {
	   ngReadAction((Widget) 0, NULL, NULL, 0);
	} else {
	   ngOpenAction((Widget) 0, NULL, NULL, 0);
	}
	return;
    }
    if (Mode == ALL_MODE) {
	allGotoAction((Widget) 0, NULL, NULL, 0);
	return;
    }
    if (Mode == ARTICLE_MODE) {
	if (inCommand) {
	    XBell(XtDisplay(TopLevel), 0);
	    return;
	}
	inCommand = 1;
	busyCursor();
	ArtPosition = SelectionItem;
	if (ArtPosition >= SubjectItems) ArtPosition = SubjectItems - 1;
	artReadFunction((Widget) 0);
	unbusyCursor();
	inCommand = 0;
	return;
    }
    return;
}

static void GetData(w, closure, call_data)
    Widget		    w;
    caddr_t		    closure;
    SListCallbackStruct    *call_data;
{
    SListCallbackStruct *cb = call_data;
    int item = cb->item_number;

    *cb->data = " ";

    if (Mode == ALL_MODE) {
	if (item >= AllGroupItems) return;
 	*cb->data = getStatusString(AllGroups[item]);
	return;
    }
    if (Mode == NEWSGROUP_MODE) {
	if (item >= GroupItems) return;
 	*cb->data = groupStatus(NewsGroups[item]);
	return;
    }
    if (Mode == ARTICLE_MODE) {
	if (item >= SubjectItems) return;
 	*cb->data = getSubjectLine(ArticleNumbers[item]);
	return;
    }
    if (Mode == ADD_MODE) {
	if (item >= AddGroupItems) return;
 	*cb->data = AddGroups[item]->name;
	return;
    }
    return;
}

static void subjSetPos(w, position, reset)
    Widget w;
    int position;
    Boolean reset;
{
    int delta;

    if (reset)
	SListSelectAll(w, 0);
    switch (Mode) {
	case NEWSGROUP_MODE:
	    GroupPosition = position;
	    SListHighlight(w, position);
	    break;
	case ARTICLE_MODE:
	    ArtPosition = position;
	    SListHighlight(w, position);
	    break;
	case ADD_MODE:
	    GroupPosition = position;
	    SListHighlight(w, position);
	    break;
	case ALL_MODE:
	    AllPosition = position;
	    SListHighlight(w, position);
	    break;
	default:
	    return;
    }

    if (position < app_resources.maxLines) {
	SListSetTop(w, 0);
	currentTop = 0;
    } else {
	if ((currentTop = SListGetTop(w)) <0) currentTop = 0;
	delta = position - currentTop;	/* lines above position desired */
	if (delta < app_resources.minLines || delta > app_resources.maxLines) {
	    currentTop = position - app_resources.minLines;
	    SListSetTop(w, currentTop);
	}
    } 
}
static XtActionProc
listForwardPage(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    return (slistForwardPage(TopList, event, params, num_params));
}
static XtActionProc
listBackwardPage(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    return (slistBackwardPage(TopList, event, params, num_params));
}
static XtActionProc
listForwardLine(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    int position, delta;

    slistForwardLine(TopList, event, params, num_params);
    position = SListGetHighlight(TopList);
    if (position < 0) position = 0;
    if (position < app_resources.maxLines) {
	SListSetTop(TopList, 0);
	currentTop = 0;
    } else {
	if ((currentTop = SListGetTop(TopList)) <0) currentTop = 0;
	delta = position - currentTop;	/* lines above position desired */
	if (delta < app_resources.minLines || delta > app_resources.maxLines) {
	    currentTop = position - app_resources.minLines;
	    SListSetTop(TopList, currentTop);
	}
    }
    return 0;
}
static XtActionProc
listBackwardLine(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    int position, delta;

    slistBackwardLine(TopList, event, params, num_params);
    position = SListGetHighlight(TopList);
    if (position < 0) position = 0;
    if (position < app_resources.maxLines) {
	SListSetTop(TopList, 0);
	currentTop = 0;
    } else {
	if ((currentTop = SListGetTop(TopList)) <0) currentTop = 0;
	delta = position - currentTop;	/* lines above position desired */
	if (delta < app_resources.minLines || delta > app_resources.maxLines) {
	    currentTop = position - app_resources.minLines;
	    SListSetTop(TopList, currentTop);
	}
    }
    return 0;
}

static XtActionProc
artReadCurrent(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    if (inCommand) {
	XBell(XtDisplay(TopLevel), 0);
	return 0;
    }
    inCommand = 1;
    busyCursor();
    ArtPosition = SListGetHighlight(TopList);
    if (ArtPosition < 0) ArtPosition = SelectionItem;
    if (ArtPosition < 0) return 0;
    if (ArtPosition >= SubjectItems) ArtPosition = SubjectItems - 1;
    artReadFunction((Widget) 0);
    unbusyCursor();
    inCommand = 0;
    return 0;
}
static Boolean
artCheckReadCurrent()
{
    int pos;
    pos = SListGetHighlight(TopList);
    if (pos < 0)
	pos = SelectionItem;
    if (pos < 0)
	return False;
    if (pos >= SubjectItems)
	pos = SubjectItems - 1;
    if (articleRead(ArticleNumbers[pos]))
	return False;
    ArtPosition = pos;
    artReadFunction((Widget) 0);
    return True;
}
