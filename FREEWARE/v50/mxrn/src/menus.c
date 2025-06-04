/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, 1990, Ellen M. Sentovich and Rick L. Spickelmier.
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
 * menus.c: create the menus
 *
 */

#include "copyright.h"
#include <stdio.h>
#include "config.h"
#ifndef VMS
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#else
#include <decw$include/Intrinsic.h>
#include <decw$include/StringDefs.h>
#endif
#ifdef MOTIF
#include <Xm/BulletinB.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/ScrolledW.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Text.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Form.h>
#if (XmVERSION == 1) && (XmREVISION == 0)
#define MOTIF_V10
#endif
#else
#ifndef VMS
#include <X11/DECwDwtWidgetProg.h>
#else
#include <decw$include:DECwDwtWidgetProg.h>
#endif
#endif /* MOTIF */
#include "xmisc.h"
#include "xrn.h"
#include "utils.h"
#include "resources.h"
#include "mesg.h"
#include "modes.h"
#include "error_hnds.h"
#ifdef __STDC__
#include <stdlib.h>
#endif
#include "menus.h"
#include "buttons.h"
#include "slist.h"

XmString addItems[50];
XmString removeItems[50];
#ifdef MOTIF
XmFontList fontList;
#else
DwtFontList fontList;
#endif

static Boolean updatesDone = False;

static XrmDatabase	xrmDB;
extern char		customName[512];
extern int		Mode;			/* Current mode */
XFontStruct *font;
typedef struct buttonDefs {
    char *name;
    char *label;
    int  addPos;
    int  removePos;
    XmString listString;
} ButtonDefs;

ButtonDefs addButtons[] = {
    {"addQuit","Quit",0,0,NULL},
    {"addFirst","Add First",0,0,NULL},
    {"addLast","Add Last",0,0,NULL},
    {"addAfter","Add After Group",0,0,NULL},
    {"addUnsub","Add Unsubscribed",0,0,NULL},
};
ButtonDefs ngButtons[] = {
    {"ngExit","Exit",0,0,NULL},
    {"ngQuit","Quit",0,0,NULL},
    {"ngRead","Read Group",0,0,NULL},
    {"ngOpen","Open Group",0,0,NULL},
    {"ngNext","Next",0,0,NULL},
    {"ngPrev","Prev",0,0,NULL},
    {"ngCatchUp","Catch Up",0,0,NULL},
    {"ngSubscribe","Subscribe",0,0,NULL},
    {"ngUnsub","Unsubscribe",0,0,NULL},
    {"ngGoto","Go To Group",0,0,NULL},
    {"ngToggleGroups","Toggle Groups",0,0,NULL},
    {"ngAllGroups","All Groups",0,0,NULL},
    {"ngRescan","Rescan",0,0,NULL},
    {"ngPrevGroup","Prev Group",0,0,NULL},
    {"ngSelect","Select Groups",0,0,NULL},
    {"ngMove","Move",0,0,NULL},
    {"ngDisconnect","Disconnect",0,0,NULL},
    {"ngCheckPoint","Checkpoint",0,0,NULL},
    {"ngPost","Post",0,0,NULL},
    {"ngGripe","Gripe",0,0,NULL},
};

ButtonDefs allButtons[] = {
    {"allQuit","Quit",0,0,NULL},
    {"allSub","Subscribe",0,0,NULL},
    {"allFirst","Subscribe First",0,0,NULL},
    {"allLast","Subscribe Last",0,0,NULL},
    {"allAfter","Subscribe After Group",0,0,NULL},
    {"allUnsub","Unsubscribe",0,0,NULL},
    {"allGoto","Go To Group",0,0,NULL},
    {"allOpen","Open Group",0,0,NULL},
    {"allSelect","Select Groups",0,0,NULL},
    {"allMove","Move",0,0,NULL},
    {"allToggle","Toggle Order",0,0,NULL},
    {"allScroll","Scroll Forward",0,0,NULL},
    {"allScrollBack","Scroll Backward",0,0,NULL},
    {"allSearch","Search",0,0,NULL},
    {"allContinue","Continue",0,0,NULL},
    {"allCancelSearch","Cancel Search",0,0,NULL},
};

ButtonDefs artButtons[] = {
    {"artQuit","Quit",0,0,NULL},
    {"artNext","Next",0,0,NULL},
    {"artNextUnread","Next Unread",0,0,NULL},
    {"artPrev","Prev",0,0,NULL},
    {"artLast","Last",0,0,NULL},
    {"artNextGroup","Next Group",0,0,NULL},
    {"artGotoArticle","Go To Article",0,0,NULL},
    {"artCatchUp","Catch Up",0,0,NULL},
    {"artCatchUpAll","Catch Up All",0,0,NULL},
    {"artFedUp","Fed Up",0,0,NULL},
    {"artMarkRead","Mark Read",0,0,NULL},
    {"artMarkUnread","Mark Unread",0,0,NULL},
    {"artUnsub","Unsubscribe",0,0,NULL},
    {"artScroll","Scroll Forward",0,0,NULL},
    {"artScrollBack","Scroll Backward",0,0,NULL},
    {"artScrollEnd","Scroll To End",0,0,NULL},
    {"artScrollBeginning","Scroll To Start",0,0,NULL},
    {"artSubNext","Subject Next",0,0,NULL},
    {"artSubPrev","Subject Prev",0,0,NULL},
    {"artKillSession","Session Kill",0,0,NULL},
    {"artKillLocal","Local Kill",0,0,NULL},
    {"artKillGlobal","Global Kill",0,0,NULL},
    {"artKillAuthor","Author Kill",0,0,NULL},
    {"artEditKillFile","Edit Kill File",0,0,NULL},
    {"artEditGlobalKillFile","Edit Global Kill File",0,0,NULL},
    {"artSubSearch","Subject Search",0,0,NULL},
    {"artAuthorSearch","Author Search",0,0,NULL},
    {"artContinue","Continue Search",0,0,NULL},
    {"artCancelSearch","Cancel Search",0,0,NULL},
    {"artNonSorted", "Article Order",0,0,NULL},
    {"artSorted","Subject Order",0,0,NULL},
    {"artStrictSorted","Strict Order",0,0,NULL},
    {"artThreadSorted","Thread Order",0,0,NULL},
    {"artPost","Post",0,0,NULL},
    {"artExit","Exit",0,0,NULL},
    {"artCheckPoint","CheckPoint newsrc",0,0,NULL},
    {"artGripe","Gripe",0,0,NULL},
    {"artListOld","List Old",0,0,NULL},
    {"artSave","Save",0,0,NULL},
    {"artReply","Reply",0,0,NULL},
    {"artForward","Forward",0,0,NULL},
    {"artFollowup","Followup",0,0,NULL},
    {"artFollowupReply","Followup and Reply",0,0,NULL},
    {"artCancel","Cancel",0,0,NULL},
    {"artRot13","Rot-13",0,0,NULL},
    {"artHeader","Toggle Header",0,0,NULL},
    {"artPrint","Print",0,0,NULL},
    {"artFlushQueue","Flush Queue",0,0,NULL},
};
ButtonDefs 	*currentButtons;
static int	currentCount;
static int	menuMode;
#define addMenu 1
#define addPop 2
#define allMenu 3
#define allPop 4
#define artMenu 5
#define artPop 6
#define ngMenu 7
#define ngPop 8

static Widget ngPulldowns[5];
static Widget artPulldowns[7];
static Widget allPulldowns[5];
static Widget addPulldowns[3];
static Widget tempPulldown;
static Widget tempMenu;

static Widget customDialog = NULL;
static Widget addLabel;
static Widget addScroll;
static Widget addList;
static Widget removeLabel;
static Widget removeScroll;
static Widget removeList;
static Widget okButton;
static Widget applyButton;
static Widget cancelButton;
static int AddCount = 0;
static int NgCount;
static int AllCount;
static int ArtCount;
static int ArtSpecCount;

static Widget readDialog = (Widget) 0;
static Widget crSubjReadButton;
static Widget crRetainKillButton;
static Widget crAuthorFullButton;
static Widget crLineCountButton;
static Widget crAutoWrapButton;
static Widget crKillButton;
static Widget crPageArtButton;
static Widget crAutoReadButton;
static Widget crWatchProgressButton;
static Widget crUpdateButton;
static Widget crOkButton;
static Widget crApplyButton;
static Widget crCancelButton;

static Widget postDialog = (Widget) 0;
static Widget cpLineLength;
static Widget cpWrapLength;
static Widget cpSigFile;
static Widget cpSavePost;
static Widget cpDeadLetters;
static Widget cpEditCmd;
static Widget cpOrganization;
static Widget cpReplyTo;
static Widget cpInclSepButton;
static Widget cpIncludePrefix;
static Widget cpOkButton;
static Widget cpApplyButton;
static Widget cpCancelButton;

static Widget mailDialog = (Widget) 0;
static Widget cmReplyPath;
static Widget cmMailer;
static Widget cmPersonal;
static Widget cmHostName;
static Widget cmCCButton;
static Widget cmOkButton;
static Widget cmApplyButton;
static Widget cmCancelButton;

static Widget confirmDialog = (Widget) 0;
static Widget confQuitButton;
static Widget confExitButton;
static Widget confCatchUpButton;
static Widget confArtCatchUpButton;
static Widget confUnsubButton;
static Widget confArtUnsubButton;
static Widget confArtEndButton;
static Widget confEditButton;
static Widget confOverButton;
static Widget ccSaveForm;
static Widget ccSaveRadio;
static Widget ccSaveMbxButton;
static Widget ccSaveHeadersButton;
static Widget ccSaveSubdirsButton;
static Widget ccSaveOneDirButton;
static Widget ccOkButton;
static Widget ccApplyButton;
static Widget ccCancelButton;

static Widget generalDialog = NULL;
static Widget cgTopLines;
static Widget cgMinLines;
static Widget cgMaxLines;
static Widget cgNNTP;
static Widget cgNewsrc;
static Widget cgSaveNewsrc;
static Widget cgTempDir;
static Widget cgSaveDir;
static Widget cgPrtCmd;
static Widget cgRescan;
static Widget cgMouseBox;
static Widget cgWhichMb2Button;
static Widget cgWhichMb3Button;
static Widget cgInfoButton;
static Widget cgOkButton;
static Widget cgApplyButton;
static Widget cgCancelButton;

static void makeAddMenus()
{
    int i = -1;
    Arg cb[2];
    XmString label;

    menuPulldown(add,File);
	menuBtn(add,Quit,"Exit - Unsubscribe Remaining Entries");
    menuPulldown(add,Add);
 	menuBtn(add,First,"Subscribe, Add At Start");
 	menuBtn(add,Last,"Subscribe, Add At End");
 	menuBtn(add,After,"Subscribe, Add At Custom Position");
 	menuBtn(add,Unsub,"Unsubscribe, Add At End");
    menuPulldown(add,Customize);
	subMenuPulldown(add,addNg,"Newsgroup Mode");
	subMenuBtn(add,ngMenus,addNgMenus,"Menu Buttons...");
	subMenuBtn(add,ngPopups,addNgPopups,"Popup Buttons...");
	subMenuPulldown(add,addArt,"Article Mode");
	subMenuBtn(add,artMenus,addArtMenus,"Menu Buttons...");
	subMenuBtn(add,artPopups,addArtPopups,"Popup Buttons...");
	subMenuPulldown(add,addAll,"All Groups Mode");
	subMenuBtn(add,allMenus,addAllMenus,"Menu Buttons...");
	subMenuBtn(add,allPopups,addAllPopups,"Popup Buttons...");
	subMenuPulldown(add,addArt,"Add Groups Mode");
	subMenuBtn(add,addMenus,addAddMenus,"Menu Buttons...");
	subMenuBtn(add,addPopups,addAddPopups,"Popup Buttons...");
	menuBtn(add,ConfirmCustom, "Controls...");
	menuBtn(add,PostCustom, "Posting...");
	menuBtn(add,ReadCustom, "Reading...");
	menuBtn(add,MailCustom, "Mailing..");
	menuBtn(add,General,"General...");
	menuBtn(add,SaveCustom,"Save Current Settings");
}

static void makeNgMenus()
{
    int i = -1;
    Arg cb[2];
    XmString label;

    menuPulldown(ng,File);
	menuBtn(ng,Quit,"Quit");
 	menuBtn(ng,Exit,"Quit Without Saving Changes");
    menuPulldown(ng,Group);
	menuBtn(ng,Read,"Read Group");
	menuBtn(ng,Open,"Open Group");
	menuBtn(ng,CatchUp,"Catch Up Group");
	menuBtn(ng,Subscribe,"Subscribe");
	menuBtn(ng,Unsub,"Unsubscribe");
	menuBtn(ng,Goto,"Go To Group");
	menuBtn(ng,Rescan,"Rescan Server");
	menuBtn(ng,Disconnect,"Disconnect From Server");
 	menuBtn(ng,CheckPoint,"Save Changes To .newsrc");
	menuBtn(ng,AllGroups,"View All Groups");
 	menuBtn(ng,ToggleGroups,"Toggle Read Groups");
    menuPulldown(ng,Move);
	menuBtn(ng,Select,"Select Groups To Move");
	menuBtn(ng,Move,"Move Selected Groups");
    menuPulldown(ng,Send);
    	menuBtn(ng,Gripe,"Mail Gripe to Maintainer");
	menuBtn(ng,Post,"Post An Article");
    menuPulldown(ng,Customize);
	subMenuPulldown(ng,ngNg,"Newsgroup Mode");
	subMenuBtn(ng,ngMenus,ngNgMenus,"Menu Buttons...");
	subMenuBtn(ng,ngPopups,ngNgPopups,"Popup Buttons...");
	subMenuPulldown(ng,ngArt,"Article Mode");
	subMenuBtn(ng,artMenus,ngArtMenus,"Menu Buttons...");
	subMenuBtn(ng,artPopups,ngArtPopups,"Popup Buttons...");
	subMenuPulldown(ng,ngAll,"All Groups Mode");
	subMenuBtn(ng,allMenus,ngAllMenus,"Menu Buttons...");
	subMenuBtn(ng,allPopups,ngAllPopups,"Popup Buttons...");
	subMenuPulldown(ng,ngArt,"Add Groups Mode");
	subMenuBtn(ng,addMenus,ngAddMenus,"Menu Buttons...");
	subMenuBtn(ng,addPopups,ngAddPopups,"Popup Buttons...");
	menuBtn(ng,ConfirmCustom, "Controls...");
	menuBtn(ng,PostCustom, "Posting...");
	menuBtn(ng,ReadCustom, "Reading...");
	menuBtn(ng,MailCustom, "Mailing..");
	menuBtn(ng,General,"General...");
	menuBtn(ng,SaveCustom,"Save Current Settings");
}

void makeArtMenus()
{
    int i = -1;
    Arg cb[2];
    XmString label;

    menuPulldown(art,File);
 	menuBtn(art,CheckPoint,"Save Changes To .newsrc");
   	menuBtn(art,Quit,"Return To Last Mode");
 	menuBtn(art,Exit,"Return To Last Mode, Mark All Unread");
    menuPulldown(art,Read);
	menuBtn(art,NextUnread,"Next Unread");
	menuBtn(art,SubNext,"Next With Same Subject");
	menuBtn(art,SubPrev,"Previous With Same Subject");
	menuBtn(art,Next,"Next Article");
	menuBtn(art,Prev,"Previous Article");
 	menuBtn(art,Last,"Go To Last Article Read");
    menuPulldown(art,Group);
	menuBtn(art,NextGroup,"Go To Next Group");
	menuBtn(art,CatchUp,"Catch Up");
	menuBtn(art,CatchUpAll,"Catch Up All Articles In This Group");
 	menuBtn(art,FedUp,"Go To Next Group, Mark All Read");
	menuBtn(art,Unsub,"Unsubscribe");
	subMenuPulldown(art,artSort,"Article Sort Order");
	subMenuBtn(art,artNonSorted,artNonSorted,"Article Number Order");
	subMenuBtn(art,artSorted,artSorted,"Subject Sorted");
	subMenuBtn(art,artStrictSorted,artStrictSorted,"Strict Subject Sorted");
	subMenuBtn(art,artThreadSorted,artThreadSorted,"Thread Sorted");

    menuPulldown(art,Article);
	menuBtn(art,GotoArticle,"Go To Article");
	menuBtn(art,MarkRead,"Mark Selected Articles Read");
	menuBtn(art,MarkUnread,"Mark Selected Articles Unread");
	menuBtn(art,Post,"Post New Article");
	menuBtn(art,Print,"Print Article");
	menuBtn(art,FlushQueue,"Print Pending Pring Jobs Now");
	menuBtn(art,Save,"Save Article");
 	menuBtn(art,Reply,"Mail Reply To Article");
 	menuBtn(art,Forward,"Forward Article");
 	menuBtn(art,Followup,"Post Followup To Article");
 	menuBtn(art,FollowupReply,"Followup And Reply To Article");
 	menuBtn(art,Rot13,"Encode/Decode Article");
	menuBtn(art,Header,"Toggle Full/Stripped Header");
        menuBtn(art,ListOld,"List All Articles");
 	menuBtn(art,Gripe,"Mail Gripe To Maintainer");
    menuPulldown(art,Kill);
	menuBtn(art,KillSession,"This Session Only");
	menuBtn(art,KillLocal,"This Group Only");
	menuBtn(art,KillGlobal,"All Groups");
	menuBtn(art,KillAuthor,"All Articles With This Author");
	menuBtn(art,EditKillFile,"Edit This Group's Kill File");
	menuBtn(art,EditGlobalKillFile,"Edit The Global Kill File");
    menuPulldown(art,Search);
	menuBtn(art,SubSearch,"Subject Search");
	menuBtn(art,AuthorSearch,"Author Search");
	menuBtn(art,Continue,"Continue Search");
	menuBtn(art,CancelSearch,"Cancel Search");
    menuPulldown(art,Customize);
	subMenuPulldown(art,artNg,"Newsgroup Mode");
	subMenuBtn(art,ngMenus,artNgMenus,"Menu Buttons...");
	subMenuBtn(art,ngPopups,artNgPopups,"Popup Buttons...");
	subMenuPulldown(art,artArt,"Article Mode");
	subMenuBtn(art,artMenus,artArtMenus,"Menu Buttons...");
	subMenuBtn(art,artPopups,artArtPopups,"Popup Buttons...");
	subMenuPulldown(art,artAll,"All Groups Mode");
	subMenuBtn(art,allMenus,artAllMenus,"Menu Buttons...");
	subMenuBtn(art,allPopups,artAllPopups,"Popup Buttons...");
	subMenuPulldown(art,artArt,"Add Groups Mode");
	subMenuBtn(art,addMenus,artAddMenus,"Menu Buttons...");
	subMenuBtn(art,addPopups,artAddPopups,"Popup Buttons...");
	menuBtn(art,ConfirmCustom, "Controls...");
	menuBtn(art,PostCustom, "Posting...");
	menuBtn(art,ReadCustom, "Reading...");
	menuBtn(art,MailCustom, "Mailing..");
	menuBtn(art,General,"General...");
	menuBtn(art,SaveCustom,"Save Current Settings");
}
void makeArtArtMenus(ArtMenuBar)
Widget ArtMenuBar;
{
    int i = -1;
    Widget TempMenuBar;
    Widget saveArtPulldowns[7];
    Widget saveArtMenus[7];

    TempMenuBar = MenuBar;
    MenuBar = ArtMenuBar;
    for (i = 0; i < XtNumber(artPulldowns); i++) {
	saveArtPulldowns[i] = artPulldowns[i];
	saveArtMenus[i] = artMenus[i];
    }
    makeArtMenus();
    MenuBar = TempMenuBar;
    for (i = 0; i < XtNumber(artPulldowns); i++) {
	XtManageChild(artMenus[i]);
	artPulldowns[i] = saveArtPulldowns[i];
	artMenus[i] = saveArtMenus[i];
    }
}
static void makeAllMenus()
{
    int i = -1;
    Arg cb[2];
    XmString label;

    menuPulldown(all,File);
	menuBtn(all,Quit,"Return To Newsgroup Mode");
	menuBtn(all,Goto,"Go To Group");
	menuBtn(all,Goto,"Open Group");
    menuPulldown(all,Subscribe);
	menuBtn(all,Sub,"Subscribe");
	menuBtn(all,First,"Subscribe, Move To Start");
	menuBtn(all,Last,"Subscribe, Move To End");
	menuBtn(all,After,"Subscribe, Move To Custom Position");
	menuBtn(all,Unsub,"Unsubscribe");
    menuPulldown(all,Move);
	menuBtn(all,Select,"Select Groups To Move");
	menuBtn(all,Move,"Move Groups");
	menuBtn(all,Toggle,"Toggle .newsrc/Alphabetical Order");
    menuPulldown(all,Search);
	menuBtn(all,Search,"Search For Group");
        menuBtn(all,Continue,"Continue Search");
        menuBtn(all,CancelSearch,"Cancel Search");
    menuPulldown(all,Customize);
	subMenuPulldown(all,allNg,"Newsgroup Mode");
	subMenuBtn(all,ngMenus,allNgMenus,"Menu Buttons...");
	subMenuBtn(all,ngPopups,allNgPopups,"Popup Buttons...");
	subMenuPulldown(all,allArt,"Article Mode");
	subMenuBtn(all,artMenus,allArtMenus,"Menu Buttons...");
	subMenuBtn(all,artPopups,allArtPopups,"Popup Buttons...");
	subMenuPulldown(all,allAll,"All Groups Mode");
	subMenuBtn(all,allMenus,allAllMenus,"Menu Buttons...");
	subMenuBtn(all,allPopups,allAllPopups,"Popup Buttons...");
	subMenuPulldown(all,allArt,"Add Groups Mode");
	subMenuBtn(all,addMenus,allAddMenus,"Menu Buttons...");
	subMenuBtn(all,addPopups,allAddPopups,"Popup Buttons...");
	menuBtn(all,ConfirmCustom, "Controls...");
	menuBtn(all,PostCustom, "Posting...");
	menuBtn(all,ReadCustom, "Reading...");
	menuBtn(all,MailCustom, "Mailing..");
	menuBtn(all,General,"General...");
	menuBtn(all,SaveCustom,"Save Current Settings");
}

void
makeMenus()
{

    AddCount = AddPopupCount;				/* store original counters */
    NgCount = NgPopupCount;
    AllCount = AllPopupCount;
    ArtCount = ArtPopupCount;
    ArtSpecCount = ArtSpecPopupCount;
    font = XLoadQueryFont(XtDisplay(TopLevel),
	"-*-Menu-Medium-R-Normal--*-120-*-*-M-*");
    if (font == NULL) {
        font = XLoadQueryFont(XtDisplay(TopLevel),
	    "-*-Helvetica-Bold-R-Normal--*-120-*");
	if (font == NULL) {
	    font = XLoadQueryFont(XtDisplay(TopLevel),
		"-*-*-Bold-R-Normal--*-120-*");
	    if (font == NULL) {
		font = XLoadQueryFont(XtDisplay(TopLevel),"fixed");
	    }
        }
    }
    if (font == NULL) {
	printf("Can't find any fonts!");
	exit(0);
    }
#ifdef MOTIF
    fontList = XmFontListCreate(font, XmSTRING_DEFAULT_CHARSET);
#else
    fontList = DwtCreateFontList(font, 1); /* ISO_LATIN1 */
#endif

    makeAddMenus();
    makeNgMenus();
    makeArtMenus();
    makeAllMenus();

}
static Widget popup_menu;
static Widget artpopup_menu;

#ifdef MOTIF
static XtEventHandler
pop_up(widget, closure, event, dispatch)
Widget widget;
XtPointer closure;
XButtonEvent *event;
Boolean *dispatch;
{
    if (event->button != app_resources.popupButton)
	return 0;

    if (!XtIsRealized(popup_menu))
	XtRealizeWidget(popup_menu);

    XmMenuPosition(popup_menu, event);
    XtManageChild(popup_menu);
    return 0;
}
static XtEventHandler
artpop_up(widget, closure, event, dispatch)
Widget widget;
XtPointer closure;
XButtonEvent *event;
Boolean *dispatch;
{
    if (event->button != app_resources.popupButton)
	return 0;

    if (!XtIsRealized(artpopup_menu))
	XtRealizeWidget(artpopup_menu);

    XmMenuPosition(artpopup_menu, event);
    XtManageChild(artpopup_menu);
    return 0;
}
#else /* MOTIF */
static void
pop_up(widget, event, params, num_params)
Widget	widget;
XButtonEvent *event;
char	*params;
int	num_params;
{
    if (event->button != app_resources.popupButton)
	return;

    if (!XtIsRealized(popup_menu))
	XtRealizeWidget(popup_menu);

    DwtMenuPosition(popup_menu, event);
    XtManageChild(popup_menu);
}
static void
artpop_up(widget, event, params, num_params)
Widget	widget;
XButtonEvent *event;
char	*params;
int	num_params;
{
    if (event->button != app_resources.popupButton)
	return;

    if (!XtIsRealized(artpopup_menu))
	XtRealizeWidget(artpopup_menu);

    DwtMenuPosition(artpopup_menu, event);
    XtManageChild(artpopup_menu);
}
static void motionhint(m,closure,event)
    MenuWidget m;
    Opaque closure;
    XPointerMovedEvent *event;
{
    if (event->is_hint == NotifyHint) {
	if (event->state & Button1Mask || event->state & Button2Mask ||
		event->state & Button3Mask) {
	    event->state |= Button2Mask;	/* hack in MB2 for XUI */
	}
    }
}	    
#endif /* MOTIF */

static char popup2Trans[] = "<Btn2Down>:pop_up()";
static char popup3Trans[] = "<Btn3Down>:pop_up()";
static char artpopup2Trans[] = "<Btn2Down>:artpop_up()";
static char artpopup3Trans[] = "<Btn3Down>:artpop_up()";

static XtTranslations popupParsed;
static XtTranslations artpopupParsed;

static XtActionsRec popupAction[] = {
    {"pop_up", (XtActionProc)pop_up},
    {"artpop_up", (XtActionProc)artpop_up},
};
externalref XtCallbackRec artSubNextCallbacks[];
externalref XtCallbackRec artQuitCallbacks[];

void makePopup(parent)
Widget	parent;
{
    Arg args[10];
    Widget pop;
    int i;

    if (AddCount == 0) {
	AddCount = AddPopupCount;		/* store original counters */
	NgCount = NgPopupCount;
	AllCount = AllPopupCount;
	ArtCount = ArtPopupCount;
	ArtSpecCount = ArtSpecPopupCount;
    } else {
	AddPopupCount = AddCount;		/* restore original counters */
	NgPopupCount = NgCount;
	AllPopupCount = AllCount;
	ArtPopupCount = ArtCount;
	ArtSpecPopupCount = ArtSpecCount;
    }
#ifndef MOTIF
    XtAddActions(popupAction, XtNumber(popupAction));
    if (app_resources.popupButton == 3) {
	popupParsed = XtParseTranslationTable(popup3Trans);
	artpopupParsed = XtParseTranslationTable(artpopup3Trans);
    } else {
	popupParsed = XtParseTranslationTable(popup2Trans);
	artpopupParsed = XtParseTranslationTable(artpopup2Trans);
    }
    XtOverrideTranslations(parent, popupParsed);

    if (app_resources.popupButton == 2) {
	popup_menu = DwtMenuPopupCreate(parent,"popup", NULL, 0);
    } else {
	i = 0;
	XtSetArg(args[i], DwtNwidth, 5);i++;
	XtSetArg(args[i], DwtNheight, 5);i++;
	XtSetArg(args[i], XtNallowShellResize, True);i++;
	XtSetArg(args[i], XtNoverrideRedirect, True);i++;
	pop = XtCreatePopupShell("popShell", menupopupWidgetClass, 
				 parent, args, i);
	i = 0;
	XtSetArg(args[i], DwtNmenuType, DwtMenuPopup);i++;
	popup_menu = XtCreateWidget("popup", menuwidgetclass, pop, args, i);
	XGrabButton(			/* Do a grab for the button */
	    XtDisplay(popup_menu),	/* event that pops the menu up */
	    app_resources.popupButton,	/* for the parent of the shell */
	    AnyModifier,		/* widget */
	    XtWindow(parent),
	    True,
	    ButtonReleaseMask | PointerMotionHintMask,
	    GrabModeAsync,
	    GrabModeAsync,
	    None,
	    None);
        XtAddEventHandler(popup_menu, PointerMotionHintMask|ButtonMotionMask,
		FALSE, motionhint, NULL);
    }
#else
    if (app_resources.popupButton == 2) {
	XtSetArg(args[0], XmNwhichButton, Button2);
    } else {
	XtSetArg(args[0], XmNwhichButton, Button3);
    }
    popup_menu = XmCreatePopupMenu(parent, "popup", args, 1);
    XtAddEventHandler(parent, ButtonPressMask, False, (XtEventHandler) pop_up,
			popup_menu);
#endif
    AddPopupButtons = ARRAYALLOC(Widget, AddPopupCount);
    doPopups(app_resources.addPopupList, popup_menu, AddPopupButtons,
		AddButtonList, &AddPopupCount);

    NgPopupButtons = ARRAYALLOC(Widget, NgPopupCount);
    doPopups(app_resources.ngPopupList, popup_menu, NgPopupButtons,
		NgButtonList, &NgPopupCount);

    AllPopupButtons = ARRAYALLOC(Widget, AllPopupCount);
    doPopups(app_resources.allPopupList, popup_menu, AllPopupButtons,
		AllButtonList, &AllPopupCount);

    ArtPopupButtons = ARRAYALLOC(Widget, ArtPopupCount);
    doPopups(app_resources.artPopupList, popup_menu, ArtPopupButtons,
		ArtButtonList, &ArtPopupCount);
}

void makeArtPopup(parent)
Widget	parent;
{
    Arg args[10];
    Widget pop;
    int i;

    if (!app_resources.twoWindows) return;
    ArtWinPopupCount = ArtCount;
#ifndef MOTIF
    XtOverrideTranslations(parent, artpopupParsed);

    if (app_resources.popupButton == 2) {
	artpopup_menu = DwtMenuPopupCreate(parent,"popup", NULL, 0);
    } else {
	i = 0;
	XtSetArg(args[i], DwtNwidth, 5);i++;
	XtSetArg(args[i], DwtNheight, 5);i++;
	XtSetArg(args[i], XtNallowShellResize, True);i++;
	XtSetArg(args[i], XtNoverrideRedirect, True);i++;
	pop = XtCreatePopupShell("popShell", menupopupWidgetClass, 
				 parent, args, i);
	i = 0;
	XtSetArg(args[i], DwtNmenuType, DwtMenuPopup);i++;
	artpopup_menu = XtCreateWidget("popup", menuwidgetclass, pop, args, i);
	XGrabButton(			/* Do a grab for the button */
	    XtDisplay(artpopup_menu),	/* event that pops the menu up */
	    app_resources.popupButton,	/* for the parent of the shell */
	    AnyModifier,		/* widget */
	    XtWindow(parent),
	    True,
	    ButtonReleaseMask | PointerMotionHintMask,
	    GrabModeAsync,
	    GrabModeAsync,
	    None,
	    None);
        XtAddEventHandler(artpopup_menu, PointerMotionHintMask|ButtonMotionMask,
		FALSE, motionhint, NULL);
    }
#else
    if (app_resources.popupButton == 2) {
	XtSetArg(args[0], XmNwhichButton, Button2);
    } else {
	XtSetArg(args[0], XmNwhichButton, Button3);
    }
    artpopup_menu = XmCreatePopupMenu(parent, "popup", args, 1);
    XtAddEventHandler(parent, ButtonPressMask, False, (XtEventHandler) artpop_up,
			artpopup_menu);
#endif
    ArtWinPopupButtons = ARRAYALLOC(Widget, ArtWinPopupCount);
    doPopups(app_resources.artPopupList, artpopup_menu, ArtWinPopupButtons,
	ArtButtonList, &ArtWinPopupCount);
}

static void
doPopups(resource, box, buttons, buttonList, size)
char *resource;
Widget box;
Widget *buttons;
ButtonList *buttonList;
int *size;
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
			    (XtArgVal)XmStringLtoRCreate(
				buttonList[j].label, XmSTRING_DEFAULT_CHARSET);
		    }
		    buttons[i] = XmCreatePushButtonGadget(box,
				    (char *)buttonList[j].buttonArgs[0].value,
				    buttonList[j].buttonArgs,
				    buttonList[j].size);
#else
		    if (buttonList[j].buttonArgs[1].value == NULL) {
			buttonList[j].buttonArgs[1].value =
			    (XtArgVal)DwtLatin1String(buttonList[j].label);
		    }
		    buttons[i] = DwtPushButtonGadgetCreate(box,
				    (char *) buttonList[j].buttonArgs[0].value,
				    buttonList[j].buttonArgs,
				    buttonList[j].size);
#endif
		    i++;
		    break;
		}
	    }
	    if (j == *size && buttonList == ArtButtonList) {
		for (j = 0; j < ArtSpecButtonListCount; j++) {
		    if (STREQ(token,
			(char *) ArtSpecButtonList[j].buttonArgs[0].value)) {
#ifdef MOTIF
			if (ArtSpecButtonList[j].buttonArgs[1].value == (XtArgVal) NULL) {
		    	    ArtSpecButtonList[j].buttonArgs[1].value =
			        (XtArgVal)XmStringLtoRCreate(
			    ArtSpecButtonList[j].label, XmSTRING_DEFAULT_CHARSET);
			}
		    	buttons[i] = XmCreatePushButtonGadget(box,
			    (char *)ArtSpecButtonList[j].buttonArgs[0].value,
			    ArtSpecButtonList[j].buttonArgs,
			    ArtSpecButtonList[j].size);
#else
			if (ArtSpecButtonList[j].buttonArgs[1].value == NULL) {
		    	   ArtSpecButtonList[j].buttonArgs[1].value =
			      (XtArgVal)DwtLatin1String(ArtSpecButtonList[j].label);
			}
		    	buttons[i] = DwtPushButtonGadgetCreate(box,
			    (char *)ArtSpecButtonList[j].buttonArgs[0].value,
			    ArtSpecButtonList[j].buttonArgs,
			    ArtSpecButtonList[j].size);
#endif
		    	i++;
		    	break;
		    }
	        }
		if (j == ArtSpecButtonListCount) {
		    j = *size;
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
	    	    (XtArgVal) XmStringLtoRCreate(buttonList[i].label,
				XmSTRING_DEFAULT_CHARSET);
	    }
	    buttons[i] = XmCreatePushButtonGadget(box,
				(char *) buttonList[i].buttonArgs[0].value,
				buttonList[i].buttonArgs,
				buttonList[i].size);
#else
	    if (buttonList[i].buttonArgs[1].value == NULL) {
		buttonList[i].buttonArgs[1].value =
	    	    (XtArgVal)DwtLatin1String(buttonList[i].label);
	    }
	    buttons[i] = DwtPushButtonGadgetCreate(box,
				(char *)buttonList[i].buttonArgs[0].value,
				buttonList[i].buttonArgs,
				buttonList[i].size);
#endif
	}
    }
    return;
}
void
resetPopupSize()
{
#ifndef MOTIF
    static Arg popupResizeArg[] = {
	{DwtNheight, 0},
    };
    XtSetValues(popup_menu, popupResizeArg, XtNumber(popupResizeArg));
#endif
}

static void makeGeneralMenu()
{
    Arg arg[20];
    int i;
    XmString str;

    i = 0;
#ifdef MOTIF
    str = XmStringLtoRCreate("mxrn: Customize/General",XmSTRING_DEFAULT_CHARSET);
#else
    str = DwtLatin1String("dxrn: Customize/General");
#endif
    XtSetArg(arg[i], XmNborderWidth, 	1);		i++;
    XtSetArg(arg[i], XmNautoUnmanage,	False);		i++;
    XtSetArg(arg[i], XmNfractionBase,	60);		i++;
    XtSetArg(arg[i], XmNresizePolicy,	XmRESIZE_GROW);	i++;
    XtSetArg(arg[i], XmNfontList,	fontList);	i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNunitType,	Xm100TH_FONT_UNITS);i++;
    XtSetArg(arg[i], XmNdialogStyle,	XmDIALOG_APPLICATION_MODAL);i++;
#ifdef MOTIF_V10
    XtSetArg(arg[i], XmNwidth,		620*16);	i++;
    XtSetArg(arg[i], XmNheight,		420*16);	i++;
#endif
#else
    XtSetArg(arg[i], DwtNunits,		DwtPixelUnits);	i++;
    XtSetArg(arg[i], DwtNwidth,		620);		i++;
    XtSetArg(arg[i], DwtNheight,	420);		i++;
#endif
    XtSetArg(arg[i], XmNdialogTitle,	str);		i++;
    generalDialog = XmCreateFormDialog(TopLevel, "generalCustom",arg,i);
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Top Lines", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	4);		i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "topLinesLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	5);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	3);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgTopLines = XmCreateText(generalDialog, "topLines", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Min Lines", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	27);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	4);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "minLinesLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	5);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	36);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	3);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgMinLines = XmCreateText(generalDialog, "minLines", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Max Lines", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	43);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	4);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "maxLinesLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	5);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	52);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	3);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgMaxLines = XmCreateText(generalDialog, "maxLines", arg, i));

    i = 0;
    str = XmStringLtoRCreate("NNTP Server", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	9);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "nntpServerLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	8);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgNNTP = XmCreateText(generalDialog, "nntpServer", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Newsrc File", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	14);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "newsrcFileLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	13);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgNewsrc = XmCreateText(generalDialog, "newsrcFile", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Saved Newsrc File", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	19);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "saveNewsrcLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	18);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgSaveNewsrc = XmCreateText(generalDialog, "saveNewsrc", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Temp Directory", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	24);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "tempDirLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	23);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgTempDir = XmCreateText(generalDialog, "tempDir", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Save Directory", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	29);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "saveDirLabel", arg, i));	    /* ggh */
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	28);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgSaveDir = XmCreateText(generalDialog, "saveDir", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Print Command", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	34);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "printCommandLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	33);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgPrtCmd= XmCreateText(generalDialog, "printCommand", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Rescan Time", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	39);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "rescanTimeLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	5);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	38);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	cgRescan = XmCreateText(generalDialog, "rescanTime", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Popup Menu Button", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	27);		i++;
    XtSetArg(arg[i], XmNtopAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	39);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(generalDialog, "popupMenuLabel", arg, i));
    XmStringFree(str);

    i = 0;
#ifdef MOTIF
    XtSetArg(arg[i], XmNborderWidth,	25);		i++;
#endif
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	44);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	38);		i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNorientation, 	XmHORIZONTAL);	i++;
#ifdef MOTIF_V10
    XtSetArg(arg[i], XmNentryClass, 	xmToggleButtonWidgetClass);i++;
#endif
    XtManageChild(
	cgMouseBox = XmCreateRadioBox(generalDialog, "mouseBox", arg, i));

    i = 0;
    str = XmStringLtoRCreate("MB2", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtManageChild(
	cgWhichMb2Button = XmCreateToggleButton(cgMouseBox, "whichMb2", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("MB3", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtManageChild(
	cgWhichMb3Button = XmCreateToggleButton(cgMouseBox, "whichMb2", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Display Informational Messages in Message Window",
				XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	47);		i++;
    XtSetArg(arg[i], XmNleftPosition,	4);		i++;
    XtManageChild(
	cgInfoButton = XmCreateToggleButton(generalDialog, "infoMsgs", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("OK", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	6);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	100);		i++;
    XtSetArg(arg[i], XmNshowAsDefault,	True);		i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	20);		i++;
#endif
    XtSetArg(arg[i], XmNactivateCallback,cgOKCallback); i++;
    XtManageChild(
	cgOkButton = XmCreatePushButton(generalDialog, "cgokButton", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Apply", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNactivateCallback,cgApplyCallback);i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	23);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	144);		i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	22);		i++;
#endif
    XtSetArg(arg[i], XmNfontList,	fontList);	i++;
    XtManageChild(
	cgApplyButton = XmCreatePushButton(generalDialog, "cgapplyButton",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Cancel", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNactivateCallback,cgCancelCallback);i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	42);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	144);		i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	22);		i++;
#endif
    XtSetArg(arg[i], XmNfontList,	fontList);	i++;
    XtManageChild(
	cgCancelButton = XmCreatePushButton(generalDialog, "cgcancelButton",
		arg,i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNdefaultButton, cgOkButton);i++;
    XtSetArg(arg[i], XmNcancelButton,  cgCancelButton);i++;
    XtSetValues(generalDialog, arg, i);

#ifndef MOTIF_V10
    i = 0;
    str = XmStringLtoRCreate(" ", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString, 	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNleftWidget,	cgPrtCmd);	i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNtopWidget,	cgMouseBox);	i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNrightOffset,	66);		i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNx,		6000);		i++; 
    XtSetArg(arg[i], XmNy,		6000);		i++; 
    XtSetArg(arg[i], XmNtopOffset,	1411);		i++;
    XtSetArg(arg[i], XmNleftOffset,	28);		i++;
    XtSetArg(arg[i], XmNbottomOffset,	99);		i++;
#else
    XtSetArg(arg[i], XmNx,		60);		i++; 
    XtSetArg(arg[i], XmNy,		60);		i++; 
    XtSetArg(arg[i], XmNtopOffset,	14);		i++;
    XtSetArg(arg[i], XmNleftOffset,	1);		i++;
    XtSetArg(arg[i], XmNbottomOffset,	1);		i++;
#endif

    XtManageChild(
	XmCreateLabel(generalDialog, "dummyLabel", arg, i));
    XmStringFree(str);
#endif
}
static void makeConfirmMenu()
{
    Arg arg[20];
    int i;
    XmString str;

    i = 0;
#ifdef MOTIF
    str = XmStringLtoRCreate("mxrn: Customize/Confirm",XmSTRING_DEFAULT_CHARSET);
#else
    str = DwtLatin1String("dxrn: Customize/Confirm");
#endif
    XtSetArg(arg[i], XmNborderWidth, 	1);		i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNunitType,	Xm100TH_FONT_UNITS);i++;
    XtSetArg(arg[i], XmNdialogStyle,	XmDIALOG_APPLICATION_MODAL);i++;
#ifdef MOTIF_V10
    XtSetArg(arg[i], XmNwidth,		650*16);	i++;
    XtSetArg(arg[i], XmNheight,		410*16);		i++;
#endif
#else
    XtSetArg(arg[i], DwtNunits,		DwtFontUnits);	i++;
    XtSetArg(arg[i], DwtNwidth,		650);		i++;
    XtSetArg(arg[i], DwtNheight,	410);		i++;
#endif
    XtSetArg(arg[i], XmNautoUnmanage,	False);		i++;
    XtSetArg(arg[i], XmNresizePolicy,	XmRESIZE_GROW);	i++;
    XtSetArg(arg[i], XmNfractionBase,	60);		i++;
    XtSetArg(arg[i], XmNfontList,	fontList);	i++;
    XtSetArg(arg[i], XmNdialogTitle,	str);		i++;
    confirmDialog = XmCreateFormDialog(TopLevel, "generalConfirm",arg,i);
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Confirm Quit", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION); i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION); i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	5);		i++;
    XtSetArg(arg[i], XmNtopPosition,	3);		i++;
    XtManageChild(
	confQuitButton = XmCreateToggleButton(confirmDialog, "confQuit",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Confirm Exit", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	36);	i++;
    XtSetArg(arg[i], XmNtopPosition,	3);	i++;
    XtManageChild(
	confExitButton = XmCreateToggleButton(confirmDialog, "confQuit",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Confirm NG Catch Up", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,  XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	confQuitButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	10);	i++;
    XtManageChild(
	confCatchUpButton = XmCreateToggleButton(confirmDialog, "confCatchUp",
						arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Confirm Article Catch Up", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	confExitButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	10);	i++;
    XtManageChild(
	confArtCatchUpButton = XmCreateToggleButton(confirmDialog,
		"confArtCatchUp", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Confirm NG Unsubscribe", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	confCatchUpButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	17);	i++;
    XtManageChild(
	confUnsubButton = XmCreateToggleButton(confirmDialog, "confUnsub",
						arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Confirm Article Unsubscribe", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	confArtCatchUpButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	17);	i++;
    XtManageChild(
	confArtUnsubButton = XmCreateToggleButton(confirmDialog,
		"confArtUnsub", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Action Prompt At Subject Change",
	XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	confUnsubButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	24);	i++;
    XtManageChild(
	confArtEndButton = XmCreateToggleButton(confirmDialog, "confArtEnd",
						arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Confirm Save file Overwrite",
		XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	confArtUnsubButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	24);	i++;
    XtManageChild(
	confOverButton = XmCreateToggleButton(confirmDialog, "confOver",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Prompt Before Posting After Editor Exits",
	XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	confArtEndButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	31);	i++;
    XtManageChild(
	confEditButton = XmCreateToggleButton(confirmDialog, "confEdit",
						arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNtopPosition,	37);i++;
    XtManageChild(
	XmCreateSeparator(confirmDialog, "saveSep", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Save Mode", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	39);	i++;
    XtSetArg(arg[i], XmNleftPosition,	24);	i++;
    XtManageChild(
	XmCreateLabel(confirmDialog, "saveModeLabel", arg, i));
    XmStringFree(str);

#ifdef MOTIF
    XtSetArg(arg[i], XmNborderWidth,	25);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNresizePolicy,	XmRESIZE_GROW);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	44);	i++;
    XtSetArg(arg[i], XmNleftPosition,	5);	i++;
    XtSetArg(arg[i], XmNrightPosition,  55);	i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNunitType,	Xm100TH_FONT_UNITS);i++;

    XtManageChild(
	ccSaveForm = XmCreateForm(confirmDialog, "saveForm", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Mailbox", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	5);	i++;
    XtSetArg(arg[i], XmNtopPosition,	25);	i++;
    XtManageChild(
	ccSaveMbxButton = XmCreateToggleButton(ccSaveForm, "ccSaveMbx",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Headers", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	24);	i++;
    XtSetArg(arg[i], XmNtopPosition,	25);	i++;
    XtManageChild(
	ccSaveHeadersButton = XmCreateToggleButton(ccSaveForm, "ccSaveHeaders",
						arg, i));
    XmStringFree(str);

    i = 0;

    XtSetArg(arg[i], XmNborderWidth,	25);	i++;
    XtSetArg(arg[i], XmNorientation,	XmHORIZONTAL);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNleftPosition,	50);	i++;
    XtSetArg(arg[i], XmNradioAlwaysOne, False); i++;
#ifdef MOTIF_V10
    XtSetArg(arg[i], XmNentryClass, 	xmToggleButtonWidgetClass);i++;
    XtSetArg(arg[i], XmNbottomOffset,	20);	i++;
    XtSetArg(arg[i], XmNtopOffset,	20);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	200);	i++;
    XtSetArg(arg[i], XmNtopOffset,	200);	i++;
#endif
    XtManageChild(
	ccSaveRadio = XmCreateRadioBox(ccSaveForm, "saveRadio", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Subdirectory", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtManageChild(
	ccSaveSubdirsButton = XmCreateToggleButton(ccSaveRadio, "saveSubdirs",
						arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("One Directory", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtManageChild(
	ccSaveOneDirButton = XmCreateToggleButton(ccSaveRadio, "saveOneDir",
						arg, i));
    XmStringFree(str);

#else /* MOTIF */

    i = 0;
    XtSetArg(arg[i], DwtNborderWidth,	1);		i++;
    XtSetArg(arg[i], DwtNx,		165);		i++;
    XtSetArg(arg[i], DwtNy,		425);		i++;
    XtSetArg(arg[i], DwtNorientation,	DwtOrientationHorizontal);	i++;
    XtSetArg(arg[i], DwtNmenuIsHomogeneous,False);	i++;
    XtSetArg(arg[i], DwtNfont,		fontList);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	37);	i++;
    XtSetArg(arg[i], XmNleftPosition,	5);	i++;
    XtManageChild(ccSaveForm =
	DwtMenuCreate(confirmDialog, "saveBox", arg,i)
	);

    i = 0;
    str = DwtLatin1String("Mailbox");
    XtSetArg(arg[i], DwtNlabel,		str);	i++;
    XtManageChild(
	ccSaveMbxButton = DwtToggleButtonCreate(ccSaveForm, "ccSaveMbx",
		arg, i));
    XtFree(str);

    i = 0;
    str = DwtLatin1String("Headers");
    XtSetArg(arg[i], DwtNlabel,		str);	i++;
    XtManageChild(
	ccSaveHeadersButton = DwtToggleButtonCreate(ccSaveForm, "ccSaveHeaders",
						arg, i));
    XtFree(str);

    i = 0;
    XtSetArg(arg[i], DwtNborderWidth,	2);		i++;
    XtSetArg(arg[i], DwtNorientation,	DwtOrientationHorizontal);	i++;
    XtSetArg(arg[i], DwtNfont,		fontList);	i++;
    XtManageChild(ccSaveRadio =
	DwtRadioBoxCreate(ccSaveForm, "saveRadio", arg,i)
	);

    i = 0;
    str = DwtLatin1String("Subdirectory");
    XtSetArg(arg[i], DwtNlabel,		str);	i++;
    XtManageChild(
	ccSaveSubdirsButton = DwtToggleButtonCreate(ccSaveRadio, "saveSubdirs",
						arg, i));
    XtFree(str);

    i = 0;
    str = DwtLatin1String("One Directory");
    XtSetArg(arg[i], DwtNlabel,		str);	i++;
    XtManageChild(
	ccSaveOneDirButton = DwtToggleButtonCreate(ccSaveRadio, "saveOneDir",
						arg, i));
    XtFree(str);

#endif /* MOTIF */

    i = 0;
    str = XmStringLtoRCreate("OK", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNshowAsDefault,	True);		i++;
    XtSetArg(arg[i], XmNbottomOffset,	200);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);		i++;
#endif
    XtSetArg(arg[i], XmNleftPosition,	10);	i++;
    XtSetArg(arg[i], XmNactivateCallback,ccOKCallback); i++;

    XtManageChild(
	ccOkButton = XmCreatePushButton(confirmDialog, "ccokButton", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Apply", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNactivateCallback,ccApplyCallback);i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	266);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);		i++;
#endif
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	25);	i++;
    XtSetArg(arg[i], XmNbottomWidget,	ccCancelButton);i++;
    XtManageChild(
	ccApplyButton = XmCreatePushButton(confirmDialog, "ccapplyButton",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Cancel", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);		i++;
    XtSetArg(arg[i], XmNactivateCallback,ccCancelCallback);i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	266);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);		i++;
#endif
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	40);	i++;
    XtManageChild(
	ccCancelButton = XmCreatePushButton(confirmDialog, "cgcancelButton",
		arg,i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNdefaultButton, ccOkButton);i++;
    XtSetArg(arg[i], XmNcancelButton,  ccCancelButton);i++;
    XtSetValues(confirmDialog, arg, i);

#ifndef MOTIF_V10
    i = 0;
    str = XmStringLtoRCreate(" ", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNtopWidget,	ccSaveForm);i++;
    XtSetArg(arg[i], XmNleftWidget,	confOverButton);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	60);	i++;
    XtSetArg(arg[i], XmNleftOffset,	14);	i++;
    XtSetArg(arg[i], XmNrightOffset,	60);	i++;
    XtSetArg(arg[i], XmNx,		6814);	i++;
    XtSetArg(arg[i], XmNy,		3400);	i++;
    XtSetArg(arg[i], XmNtopOffset,	800);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	1);	i++;
    XtSetArg(arg[i], XmNleftOffset,	1);	i++;
    XtSetArg(arg[i], XmNrightOffset,	1);	i++;
    XtSetArg(arg[i], XmNx,		68);	i++;
    XtSetArg(arg[i], XmNy,		34);	i++;
    XtSetArg(arg[i], XmNtopOffset,	8);	i++;
#endif
    XtManageChild(
	XmCreateLabel(confirmDialog, "dummyLabel", arg, i));
    XmStringFree(str);
#endif
}
static void makeMailMenu()
{
    Arg arg[20];
    int i;
    XmString str;

    i = 0;
#ifdef MOTIF
    str = XmStringLtoRCreate("mxrn: Customize/Mail",XmSTRING_DEFAULT_CHARSET);
#else
    str = DwtLatin1String("dxrn: Customize/Mail");
#endif
    XtSetArg(arg[i], XmNborderWidth, 	1);		i++;
    XtSetArg(arg[i], XmNautoUnmanage,	False);		i++;
    XtSetArg(arg[i], XmNfractionBase,	60);		i++;
    XtSetArg(arg[i], XmNresizePolicy,	XmRESIZE_GROW);	i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNunitType,	Xm100TH_FONT_UNITS);i++;
    XtSetArg(arg[i], XmNdialogStyle,	XmDIALOG_APPLICATION_MODAL);i++;
#ifdef MOTIF_V10
    XtSetArg(arg[i], XmNwidth,		620*16);	i++;
    XtSetArg(arg[i], XmNheight,		290*16);	i++;
#endif
#else
    XtSetArg(arg[i], DwtNunits,		DwtFontUnits);	i++;
    XtSetArg(arg[i], DwtNwidth,		620);		i++;
    XtSetArg(arg[i], DwtNheight,	290);		i++;
#endif
    XtSetArg(arg[i], XmNfontList,	fontList);	i++;
    XtSetArg(arg[i], XmNdialogTitle,	str);		i++;
    mailDialog = XmCreateFormDialog(TopLevel, "generalConfirm",arg,i);
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Reply Path", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	4);	i++;
    XtSetArg(arg[i], XmNleftPosition,	5);	i++;
    XtManageChild(
	XmCreateLabel(mailDialog, "replyLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	3);	i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;

    XtManageChild(
	cmReplyPath = XmCreateText(mailDialog, "replyPath", arg, i));

#ifndef VMS
    i = 0;
    str = XmStringLtoRCreate("Mailer", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	14);	i++;
    XtSetArg(arg[i], XmNleftPosition,	5);	i++;
    XtManageChild(
	XmCreateLabel(mailDialog, "mailerLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNtopPosition,	13);	i++;

    XtManageChild(
	cmMailer = XmCreateText(mailDialog, "Mailer", arg, i));

#else
    i = 0;
    str = XmStringLtoRCreate("Personal Name", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	14);	i++;
    XtSetArg(arg[i], XmNleftPosition,	5);	i++;
    XtManageChild(
	XmCreateLabel(mailDialog, "persLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNtopPosition,	13);	i++;

    XtManageChild(
	cmPersonal = XmCreateText(mailDialog, "Personal", arg, i));

#endif
    i = 0;
    str = XmStringLtoRCreate("Host Name", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	24);	i++;
    XtSetArg(arg[i], XmNleftPosition,	5);	i++;
    XtManageChild(
	XmCreateLabel(mailDialog, "hostLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNtopPosition,	23);	i++;

    XtManageChild(
	cmHostName = XmCreateText(mailDialog, "hostName", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Include CC in Mail headers",
		XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	37);	i++;
    XtSetArg(arg[i], XmNleftPosition,	1);	i++;

    XtManageChild(
	cmCCButton = XmCreateToggleButton(mailDialog, "cmCC", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("OK", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	6);	i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	100);	i++;
    XtSetArg(arg[i], XmNshowAsDefault,	True);		i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNy,		4900);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNactivateCallback,cmOKCallback); i++;

    XtManageChild(
	cmOkButton = XmCreatePushButton(mailDialog, "cmokButton", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Apply", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNy,		4900);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	144);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNleftPosition,	23);	i++;
    XtSetArg(arg[i], XmNactivateCallback,cmApplyCallback); i++;

    XtManageChild(
	cmApplyButton = XmCreatePushButton(mailDialog, "cmApplyButton",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Cancel", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNy,		5011);	i++;
    XtSetArg(arg[i], XmNx,		5255);	i++;
    XtSetArg(arg[i], XmNleftPosition,	42);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	144);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNactivateCallback,cmCancelCallback); i++;

    XtManageChild(
	cmCancelButton = XmCreatePushButton(mailDialog, "cmCancelButton",
		arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNdefaultButton, cmOkButton);i++;
    XtSetArg(arg[i], XmNcancelButton,  cmCancelButton);i++;
    XtSetValues(mailDialog, arg, i);

#ifndef MOTIF_V10
    i = 0;
    str = XmStringLtoRCreate("   ", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNleftWidget,	cmHostName);i++;
    XtSetArg(arg[i], XmNtopWidget,	cmHostName);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	99);	i++;
    XtSetArg(arg[i], XmNrightOffset,	66);	i++;
    XtSetArg(arg[i], XmNy,		6000);	i++;
    XtSetArg(arg[i], XmNx,		6000);	i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNtopOffset,	1300);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	1);	i++;
    XtSetArg(arg[i], XmNrightOffset,	1);	i++;
    XtSetArg(arg[i], XmNy,		60);	i++;
    XtSetArg(arg[i], XmNx,		60);	i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNtopOffset,	13);	i++;
#endif
    XtManageChild(
	XmCreateLabel(mailDialog, "dummyLabel", arg, i));

    XmStringFree(str);
#endif
}

static void makePostMenu()
{
    Arg arg[20];
    int i;
    XmString str;

    i = 0;
#ifdef MOTIF
    str = XmStringLtoRCreate("mxrn: Customize/Post",XmSTRING_DEFAULT_CHARSET);
#else
    str = DwtLatin1String("dxrn: Customize/Post");
#endif
    XtSetArg(arg[i], XmNborderWidth,	1);	i++;
    XtSetArg(arg[i], XmNautoUnmanage,	False);	i++;
    XtSetArg(arg[i], XmNfractionBase,	60);	i++;
    XtSetArg(arg[i], XmNresizePolicy,	XmRESIZE_GROW);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNunitType,	Xm100TH_FONT_UNITS);i++;
    XtSetArg(arg[i], XmNdialogStyle,	XmDIALOG_APPLICATION_MODAL);i++;
#ifdef MOTIF_V10
    XtSetArg(arg[i], XmNwidth,		650*16);	i++;
    XtSetArg(arg[i], XmNheight,		420*16);	i++;
#endif
#else
    XtSetArg(arg[i], DwtNunits,		DwtFontUnits);	i++;
    XtSetArg(arg[i], DwtNwidth,		650);		i++;
    XtSetArg(arg[i], DwtNheight,	420);		i++;
#endif
    XtSetArg(arg[i], XmNfontList,	fontList);	i++;
    XtSetArg(arg[i], XmNdialogTitle,	str);		i++;
    postDialog = XmCreateFormDialog(TopLevel, "customizePost",arg,i);
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Line Length", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	4);	i++;
    XtSetArg(arg[i], XmNleftPosition,	4);	i++;
    XtManageChild(
	XmCreateLabel(postDialog, "lineLengthLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	5);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	3);	i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtManageChild(
	cpLineLength = XmCreateText(postDialog, "cpLineLength", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Wrap Length", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftPosition,	27);	i++;
    XtSetArg(arg[i], XmNtopPosition,	4);	i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtManageChild(
	XmCreateLabel(postDialog, "wrapLengthLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	5);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	3);	i++;
    XtSetArg(arg[i], XmNleftPosition,	37);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtManageChild(
	cpWrapLength = XmCreateText(postDialog, "cpWrapLength", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Signature File", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	11);	i++;
    XtSetArg(arg[i], XmNleftPosition,	4);	i++;
    XtManageChild(
	XmCreateLabel(postDialog, "sigFileLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNtopPosition,	10);	i++;
    XtManageChild(
	cpSigFile = XmCreateText(postDialog, "cpSigFile", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Save Postings", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	17);	i++;
    XtSetArg(arg[i], XmNleftPosition,	4);	i++;
    XtManageChild(
	XmCreateLabel(postDialog, "savePostLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNtopPosition,	16);	i++;
    XtManageChild(
	cpSavePost = XmCreateText(postDialog, "cpSavePost", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Dead Letters", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	23);	i++;
    XtSetArg(arg[i], XmNleftPosition,	4);	i++;
    XtManageChild(
	XmCreateLabel(postDialog, "deadLettersLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNtopPosition,	22);	i++;
    XtManageChild(
	cpDeadLetters = XmCreateText(postDialog, "cpDeadLetters", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Editor Command", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	29);	i++;
    XtSetArg(arg[i], XmNleftPosition,	4);	i++;
    XtManageChild(
	XmCreateLabel(postDialog, "editorCommandLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNtopPosition,	28);	i++;
    XtManageChild(
	cpEditCmd = XmCreateText(postDialog, "cpEditCmd", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Organization", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	35);	i++;
    XtSetArg(arg[i], XmNleftPosition,	4);	i++;
    XtManageChild(
	XmCreateLabel(postDialog, "orgLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtSetArg(arg[i], XmNtopPosition,	34);	i++;
    XtManageChild(
	cpOrganization = XmCreateText(postDialog, "cpOrganization",
		arg, i));

    i = 0;
    str = XmStringLtoRCreate("Reply To", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	41);	i++;
    XtSetArg(arg[i], XmNleftPosition,	4);	i++;
    XtManageChild(
	XmCreateLabel(postDialog, "cpReplyLabel", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	40);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	41);	i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtManageChild(
	cpReplyTo = XmCreateText(postDialog, "cpReplyTo", arg, i));

    i = 0;
    str = XmStringLtoRCreate("Include Separator", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	47);	i++;
    XtSetArg(arg[i], XmNleftPosition,	1);	i++;
    XtManageChild(
	cpInclSepButton = XmCreateToggleButton(postDialog, "cpInclSep",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Include Prefix", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	48);	i++;
    XtSetArg(arg[i], XmNleftPosition,	19);	i++;
    XtManageChild(
	XmCreateLabel(postDialog, "cpIncludePrefix", arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNcolumns,	5);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopPosition,	47);	i++;
    XtSetArg(arg[i], XmNleftPosition,	31);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtManageChild(
	cpIncludePrefix = XmCreateText(postDialog, "cpIncPrefix", arg, i));

    i = 0;
    str = XmStringLtoRCreate("OK", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	6);	i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	100);	i++;
    XtSetArg(arg[i], XmNshowAsDefault,	True);		i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNy,		4900);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNactivateCallback,cpOKCallback); i++;

    XtManageChild(
	cpOkButton = XmCreatePushButton(postDialog, "cpokButton", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Apply", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNy,		4900);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	144);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNleftPosition,	23);	i++;
    XtSetArg(arg[i], XmNactivateCallback,cpApplyCallback); i++;

    XtManageChild(
	cpApplyButton = XmCreatePushButton(postDialog, "cpApplyButton",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Cancel", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNy,		5011);	i++;
    XtSetArg(arg[i], XmNx,		5255);	i++;
    XtSetArg(arg[i], XmNleftPosition,	42);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	144);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNactivateCallback,cpCancelCallback); i++;
    XtManageChild(
	cpCancelButton = XmCreatePushButton(postDialog, "cpCancelButton",
		arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNdefaultButton, cpOkButton);i++;
    XtSetArg(arg[i], XmNcancelButton,  cpCancelButton);i++;
    XtSetValues(postDialog, arg, i);

#ifndef MOTIF_V10
    i = 0;
    str = XmStringLtoRCreate(" ", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNleftWidget,	cpReplyTo);i++;
    XtSetArg(arg[i], XmNtopWidget,	cpReplyTo);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNx,		6000);	i++;
    XtSetArg(arg[i], XmNy,		6000);	i++;
    XtSetArg(arg[i], XmNbottomOffset,	99);	i++;
    XtSetArg(arg[i], XmNrightOffset,	66);	i++;
    XtSetArg(arg[i], XmNtopOffset,	1000);	i++;
#else
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNx,		60);	i++;
    XtSetArg(arg[i], XmNy,		60);	i++;
    XtSetArg(arg[i], XmNbottomOffset,	1);	i++;
    XtSetArg(arg[i], XmNrightOffset,	1);	i++;
    XtSetArg(arg[i], XmNtopOffset,	10);	i++;
#endif
    XtManageChild(
	XmCreateLabel(postDialog, "dummyLabel", arg, i));
    XmStringFree(str);
#endif
}
static void makeReadMenu()
{
    Arg arg[20];
    int i;
    XmString str;

    i = 0;
#ifdef MOTIF
    str = XmStringLtoRCreate("mxrn: Customize/Read", XmSTRING_DEFAULT_CHARSET);
#else
    str = DwtLatin1String("dxrn: Customize/Read");
#endif
    XtSetArg(arg[i], XmNdialogTitle,	str);	i++;
    XtSetArg(arg[i], XmNborderWidth,	1);	i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNunitType,	Xm100TH_FONT_UNITS);i++;
#ifdef MOTIF_V10
    XtSetArg(arg[i], XmNwidth,		500*16);	i++;
    XtSetArg(arg[i], XmNheight,		310*16);	i++;
#endif
#else
    XtSetArg(arg[i], DwtNunits,		DwtFontUnits);	i++;
    XtSetArg(arg[i], DwtNwidth,		500);		i++;
    XtSetArg(arg[i], DwtNheight,	310);		i++;
#endif
    XtSetArg(arg[i], XmNautoUnmanage,	False);	i++;
    XtSetArg(arg[i], XmNresizePolicy,	XmRESIZE_GROW);i++;
    XtSetArg(arg[i], XmNfractionBase,	60);	i++;
    readDialog = XmCreateFormDialog(TopLevel, "customizeRead",arg,i);
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Subject Read", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	5);	i++;
    XtSetArg(arg[i], XmNtopPosition,	3);	i++;
    XtManageChild(
	crSubjReadButton = XmCreateToggleButton(readDialog, "crSubjRead",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Retain Killed", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	36);	i++;
    XtSetArg(arg[i], XmNtopPosition,	3);	i++;
    XtManageChild(
	crRetainKillButton = XmCreateToggleButton(readDialog, "crRetainKill",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Full Name", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	crSubjReadButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	11);	i++;
    XtManageChild(
	crAuthorFullButton = XmCreateToggleButton(readDialog, "crAuthorFull",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Show Line Count", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	crRetainKillButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNrightOffset,	200);	i++;
#else
    XtSetArg(arg[i], XmNrightOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNtopPosition,	11);	i++;
    XtManageChild(
	crLineCountButton = XmCreateToggleButton(readDialog, "crLineCount",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Auto Wrap", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	crAuthorFullButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	19);	i++;
    XtManageChild(
	crAutoWrapButton = XmCreateToggleButton(readDialog, "crAutoWrap",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Use Kill Files", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	crLineCountButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	19);	i++;
    XtManageChild(
	crKillButton = XmCreateToggleButton(readDialog, "crKill",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Page Articles", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	crAutoWrapButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	27);	i++;
    XtManageChild(
	crPageArtButton = XmCreateToggleButton(readDialog, "crPageArt",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Read First Article", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	crKillButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	27);	i++;
    XtManageChild(
	crAutoReadButton = XmCreateToggleButton(readDialog, "crAutoRead",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Watch Kill Progress", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
    XtSetArg(arg[i], XmNleftWidget,	crAutoReadButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	35);	i++;
    XtManageChild(
	crWatchProgressButton = XmCreateToggleButton(readDialog,
		"crWatchProgress", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Update Newsrc when\nExiting Group",
		XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_POSITION);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNleftOffset,	-10);	i++;
#else
    XtSetArg(arg[i], XmNleftOffset,	-1);	i++;
#endif
    XtSetArg(arg[i], XmNleftWidget,	crPageArtButton);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNtopPosition,	35);	i++;
    XtManageChild(
	crUpdateButton = XmCreateToggleButton(readDialog, "crUpdate",
		arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("OK", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNactivateCallback,crOKCallback); i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	200);	i++;
    XtSetArg(arg[i], XmNshowAsDefault,	True);		i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	10);	i++;
    XtManageChild(
	crOkButton = XmCreatePushButton(readDialog, "crokButton", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Apply", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNactivateCallback,crApplyCallback); i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	266);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	24);	i++;
    XtManageChild(
	crApplyButton = XmCreatePushButton(readDialog, "crApplyButton", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Cancel", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNactivateCallback,crCancelCallback); i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	266);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	2);	i++;
#endif
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);i++;
    XtSetArg(arg[i], XmNleftPosition,	40);	i++;
    XtManageChild(
	crCancelButton = XmCreatePushButton(readDialog, "crCancelButton",
		arg, i));
    XmStringFree(str);

    i = 0;
    XtSetArg(arg[i], XmNdefaultButton, crOkButton);i++;
    XtSetArg(arg[i], XmNcancelButton,  crCancelButton);i++;
    XtSetValues(readDialog, arg, i);

#ifndef MOTIF_V10
    i = 0;
    str = XmStringLtoRCreate(" ", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);	i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);i++;
    XtSetArg(arg[i], XmNtopWidget,	crWatchProgressButton);i++;
    XtSetArg(arg[i], XmNleftWidget,	crWatchProgressButton);i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	60);	i++;
    XtSetArg(arg[i], XmNtopOffset,	800);	i++;
    XtSetArg(arg[i], XmNrightOffset,	60);	i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	1);	i++;
    XtSetArg(arg[i], XmNtopOffset,	8);	i++;
    XtSetArg(arg[i], XmNrightOffset,	1);	i++;
    XtSetArg(arg[i], XmNleftOffset,	0);	i++;
#endif
    XtManageChild(
	XmCreateLabel(readDialog, "dummyLabel", arg, i));
    XmStringFree(str);
#endif
}
static void updateText(widget, field)
Widget widget;
char **field;
{
    char *tempStr;
    char *p;
    char *old;
    char *new;

#ifdef MOTIF
    tempStr = XmTextGetString(widget);
#else
    tempStr = DwtSTextGetString(widget);
#endif

    old = *field;
    if (tempStr == NULL || strlen(tempStr) == 0) {
	*field = NULL;
	new = NULL;
    } else {
	p = tempStr;
	while (*p == ' ') p++;
	if (strlen(p) == 0) {
	    *field = NULL;
	    new = NULL;
	} else {
	    *field = XtNewString(p);
	    new = p;
	}
    }
    if (old == NULL && new == NULL) {
	XtFree(tempStr);
	return;
    }
    if ( (old == NULL && new != NULL) ||
	 (old != NULL && new == NULL) ||
	 (strcmp(old, new) != 0)) {
	updatesDone = True;
    }
    XtFree(tempStr);
}
static int getInt(widget,def)
Widget widget;
int def;
{
    int ret;
    char *tempStr;

    ret = def;
#ifdef MOTIF
    tempStr = XmTextGetString(widget);
#else
    tempStr = DwtSTextGetString(widget);
#endif
    if (tempStr != NULL && strlen(tempStr) != 0) {
	ret = atoi(tempStr);
    }
    XtFree(tempStr);
    if (ret != def)
	updatesDone = True;
    return ret;
}
static void updateBoolean(widget, field)
Widget widget;
Boolean *field;
{
    Boolean old, new;

    old = *field;
    new = XmToggleButtonGetState(widget);

    if (old != new)
	updatesDone = True;

    *field = new;
}
static void UpdateGeneral()
{
    int topLines, minLines, maxLines;
    int rescanTime;

    topLines = getInt(cgTopLines, app_resources.topLines);
    minLines = getInt(cgMinLines, app_resources.minLines);
    maxLines = getInt(cgMaxLines, app_resources.maxLines);
    if (topLines < 3) {
	warning("Top Lines must be greater than 3. Top/Min/Max Lines not changed");
    } else {
	if (topLines > 40) {
	    warning("Top Lines must be less than 40. Top/Min/Max Lines not changed.");
	} else {
	    if (minLines > topLines) {
		warning("Min Lines must be > Top Lines. Top/Min/Max Lines not changed");
	    } else {
		if (maxLines >= topLines) {
		    warning("Max lines must be < Top Lines. Top/Min/Max Lines not changed.");
		} else {
		    if (maxLines < minLines) {
			warning("Max Lines must be >= Min Lines. Top/Min/Max not changed");
		    } else {
			app_resources.topLines = topLines;
			app_resources.minLines = minLines;
			app_resources.maxLines = maxLines;
		    }
		}
	    }
	}
    }
    updateText(cgNNTP,&app_resources.nntpServer);
    updateText(cgNewsrc,&app_resources.newsrcFile);
    updateText(cgSaveNewsrc,&app_resources.saveNewsrcFile);
    updateText(cgTempDir,&app_resources.tmpDir);
    updateText(cgSaveDir,&app_resources.saveDir);
    updateText(cgPrtCmd,&app_resources.printCommand);
    rescanTime = getInt(cgRescan,app_resources.rescanTime);
    app_resources.rescanTime = rescanTime;
    if (XmToggleButtonGetState(cgWhichMb2Button)) {
	if (app_resources.popupButton != 2)
	    updatesDone = True;
	app_resources.popupButton = 2;
    } else {
	if (app_resources.popupButton != 3)
	    updatesDone = True;
	app_resources.popupButton = 3;
    }
    updateBoolean(cgInfoButton, &app_resources.info);
}
static void UpdateConfirm()
{
    int oldSaveMode = app_resources.saveMode;
    int oldConfirmMode = app_resources.confirmMode;

    app_resources.saveMode = 0;
    if (XmToggleButtonGetState(ccSaveMbxButton)) {
	app_resources.saveMode |= MAILBOX_SAVE;
    }
    if (XmToggleButtonGetState(ccSaveHeadersButton)) {
	app_resources.saveMode &= ~NOHEADERS_SAVE;
	app_resources.saveMode |= HEADERS_SAVE;
    } else {
	app_resources.saveMode &= ~HEADERS_SAVE;
	app_resources.saveMode |= NOHEADERS_SAVE;
    }
    if (XmToggleButtonGetState(ccSaveSubdirsButton)) {
	app_resources.saveMode &= ~SAVE_DIR_DEFAULT;
	app_resources.saveMode &= ~ONEDIR_SAVE;
	app_resources.saveMode |= SUBDIRS_SAVE;
    }
    if (XmToggleButtonGetState(ccSaveOneDirButton)) {
	app_resources.saveMode &= ~SAVE_DIR_DEFAULT;
	app_resources.saveMode &= ~SUBDIRS_SAVE;
	app_resources.saveMode |= ONEDIR_SAVE;
    }
    if (app_resources.saveMode != oldSaveMode)
	updatesDone = True;

    if (XmToggleButtonGetState(confQuitButton))
	app_resources.confirmMode |= NG_QUIT;
    if (XmToggleButtonGetState(confExitButton))
	app_resources.confirmMode |= NG_EXIT;
    if (XmToggleButtonGetState(confCatchUpButton))
	app_resources.confirmMode |= NG_CATCHUP;
    if (XmToggleButtonGetState(confArtCatchUpButton))
	app_resources.confirmMode |= ART_CATCHUP;
    if (XmToggleButtonGetState(confUnsubButton))
	app_resources.confirmMode |= NG_UNSUBSCRIBE;
    if (XmToggleButtonGetState(confArtUnsubButton))
	app_resources.confirmMode |= ART_UNSUBSCRIBE;
    if (XmToggleButtonGetState(confArtEndButton))
	app_resources.confirmMode |= ART_ENDACTION;
    if (XmToggleButtonGetState(confOverButton))
	app_resources.confirmMode |= ART_SAVE;
    if (!XmToggleButtonGetState(confEditButton))
	app_resources.confirmMode |= COMP_POST;
    if (app_resources.confirmMode != oldConfirmMode)
	updatesDone = True;
}
static void UpdateMail()
{

    updateText(cmReplyPath,&app_resources.replyPath);
#ifndef VMS
    updateText(cmMailer,&app_resources.mailer);
#else
    updateText(cmPersonal,&app_resources.personalName);
#endif

    updateText(cmHostName,&app_resources.hostName);

    updateBoolean(cmCCButton, &app_resources.cc);
}

static void UpdatePost()
{
    int lineLength, breakLength;

    lineLength = getInt(cpLineLength,app_resources.lineLength);
    if (lineLength && (lineLength < 40 || lineLength > 250)) {
	warning("Line length out of range. Use 40 thru 250");
    } else {
	if (lineLength != app_resources.lineLength)
	    updatesDone = True;
        app_resources.lineLength = lineLength;
    }
    breakLength = getInt(cpWrapLength,app_resources.breakLength);
    if (breakLength && (breakLength < lineLength)) {
	warning("Wrap length must be greater than Line Length");
    } else {
	if (breakLength > 250) {
	    warning("Break length out of range. Must be less than 250");
   	} else {
	if (breakLength != app_resources.breakLength)
	    updatesDone = True;
	    app_resources.breakLength = breakLength;
	}
    }
    updateText(cpSigFile,&app_resources.signatureFile);
    updateText(cpSavePost,&app_resources.savePostings);
    updateText(cpDeadLetters,&app_resources.deadLetters);
    updateText(cpEditCmd,&app_resources.editorCommand);
    updateText(cpOrganization,&app_resources.organization);
    updateText(cpReplyTo,&app_resources.replyTo);
    updateBoolean(cpInclSepButton, &app_resources.includeSep);
    updateText(cpIncludePrefix,&app_resources.includePrefix);
}
static void UpdateRead()
{
    updateBoolean(crSubjReadButton, &app_resources.subjectRead);
    updateBoolean(crRetainKillButton, &app_resources.retainKilledArticles);
    updateBoolean(crAuthorFullButton, &app_resources.authorFullName);
    updateBoolean(crLineCountButton, &app_resources.displayLineCount);
    updateBoolean(crAutoWrapButton, &app_resources.autoWrap);
    updateBoolean(crKillButton, &app_resources.killFiles);
    updateBoolean(crPageArtButton, &app_resources.pageArticles);
    updateBoolean(crAutoReadButton, &app_resources.autoRead);
    updateBoolean(crWatchProgressButton, &app_resources.watchProgress);
    updateBoolean(crUpdateButton, &app_resources.updateNewsrc);
}

static void CustomizeGeneral()
{
    char tempStr[20];

    if (generalDialog == 0) {
	makeGeneralMenu();
    }
    sprintf(tempStr,"%d",app_resources.topLines);
    XmTextSetString(cgTopLines, tempStr);
    sprintf(tempStr,"%d",app_resources.minLines);
    XmTextSetString(cgMinLines,tempStr);
    sprintf(tempStr,"%d",app_resources.maxLines);
    XmTextSetString(cgMaxLines,tempStr);
    XmTextSetString(cgNNTP, app_resources.nntpServer);
    XmTextSetString(cgNewsrc, app_resources.newsrcFile);
    XmTextSetString(cgSaveNewsrc, app_resources.saveNewsrcFile);
    XmTextSetString(cgTempDir, app_resources.tmpDir);
    XmTextSetString(cgSaveDir, app_resources.saveDir);
    XmTextSetString(cgPrtCmd, app_resources.printCommand);
    sprintf(tempStr,"%d",app_resources.rescanTime);
    XmTextSetString(cgRescan,tempStr);
    if (app_resources.popupButton == 2) {
	XmToggleButtonSetState(cgWhichMb2Button, True, False);
	XmToggleButtonSetState(cgWhichMb3Button, False, False);
    } else {
	XmToggleButtonSetState(cgWhichMb2Button, False, False);
	XmToggleButtonSetState(cgWhichMb3Button, True, False);
    }
    XmToggleButtonSetState(cgInfoButton, app_resources.info, False);/* ggh */
    XtManageChild(generalDialog);
    XtRealizeWidget(XtParent(generalDialog));
}
static void CustomizeConfirm()
{
    if (confirmDialog == 0) {
	makeConfirmMenu();
    }
    XmToggleButtonSetState(confQuitButton,
	(app_resources.confirmMode & NG_QUIT) ? True : False, False);
    XmToggleButtonSetState(confExitButton,
	(app_resources.confirmMode & NG_EXIT) ? True : False, False);
    XmToggleButtonSetState(confCatchUpButton,
	(app_resources.confirmMode & NG_CATCHUP) ? True : False, False);
    XmToggleButtonSetState(confArtCatchUpButton,
	(app_resources.confirmMode & ART_CATCHUP) ? True : False, False);
    XmToggleButtonSetState(confUnsubButton,
	(app_resources.confirmMode & NG_UNSUBSCRIBE) ? True : False, False);
    XmToggleButtonSetState(confArtUnsubButton,
	(app_resources.confirmMode & ART_UNSUBSCRIBE) ? True : False, False);
    XmToggleButtonSetState(confArtEndButton,
	(app_resources.confirmMode & ART_ENDACTION) ? True : False, False);
    XmToggleButtonSetState(confOverButton,
	(app_resources.confirmMode & ART_SAVE) ? True : False, False);
    XmToggleButtonSetState(confEditButton,
	(app_resources.confirmMode & COMP_POST) ? False : True, False);

    XmToggleButtonSetState(ccSaveMbxButton,
	(app_resources.saveMode & MAILBOX_SAVE) ? True : False, False);
    XmToggleButtonSetState(ccSaveHeadersButton,
	(app_resources.saveMode & HEADERS_SAVE) ? True : False, False);
    XmToggleButtonSetState(ccSaveSubdirsButton,
	(app_resources.saveMode & SUBDIRS_SAVE) ? True : False, False);
    XmToggleButtonSetState(ccSaveOneDirButton,
	(app_resources.saveMode & ONEDIR_SAVE) ? True : False, False);

    XtManageChild(confirmDialog);
    XtRealizeWidget(XtParent(confirmDialog));
}
static void CustomizeMail()
{
    if (mailDialog == 0) {
	makeMailMenu();
    }

    XmTextSetString(cmReplyPath, app_resources.replyPath);
#ifndef VMS
    XmTextSetString(cmMailer, app_resources.mailer);
#else
    XmTextSetString(cmPersonal, app_resources.personalName);
#endif
    XmTextSetString(cmHostName, app_resources.hostName);
    XmToggleButtonSetState(cmCCButton,
		app_resources.cc, False);

    XtManageChild(mailDialog);
    XtRealizeWidget(XtParent(mailDialog));
}
static void CustomizePost()
{
    char tempStr[20];

    if (postDialog == 0) {
	makePostMenu();
    }

    sprintf(tempStr,"%d",app_resources.lineLength);
    XmTextSetString(cpLineLength,tempStr);
    sprintf(tempStr,"%d",app_resources.breakLength);
    XmTextSetString(cpWrapLength,tempStr);
    XmTextSetString(cpSigFile, app_resources.signatureFile);
    XmTextSetString(cpSavePost, app_resources.savePostings);
    XmTextSetString(cpDeadLetters, app_resources.deadLetters);
    XmTextSetString(cpEditCmd, app_resources.editorCommand);
    XmTextSetString(cpOrganization, app_resources.organization);
    XmTextSetString(cpReplyTo, app_resources.replyTo);
    XmToggleButtonSetState(cpInclSepButton,
		app_resources.includeSep, False);		    /* ggh */
    XmTextSetString(cpIncludePrefix, app_resources.includePrefix);


    XtManageChild(postDialog);
    XtRealizeWidget(XtParent(postDialog));
}
static void CustomizeRead()
{
    if (readDialog == 0) {
	makeReadMenu();
    }

    XmToggleButtonSetState(crSubjReadButton,
		app_resources.subjectRead, False);
    XmToggleButtonSetState(crRetainKillButton,
		app_resources.retainKilledArticles, False);
    XmToggleButtonSetState(crAuthorFullButton,
		app_resources.authorFullName, False);
    XmToggleButtonSetState(crLineCountButton,
		app_resources.displayLineCount, False);
    XmToggleButtonSetState(crAutoWrapButton,
		app_resources.autoWrap, False);
    XmToggleButtonSetState(crKillButton,
		app_resources.killFiles, False);
    XmToggleButtonSetState(crPageArtButton,
		app_resources.pageArticles, False);
    XmToggleButtonSetState(crAutoReadButton,
		app_resources.autoRead, False);
    XmToggleButtonSetState(crWatchProgressButton,
		app_resources.watchProgress, False);
    XmToggleButtonSetState(crUpdateButton,
		app_resources.updateNewsrc, False);

    XtManageChild(readDialog);
    XtRealizeWidget(XtParent(readDialog));
}

static void putNullStr(name, src)
char *name;
char *src;
{
    char resName[512];
    char *trim;
    char *value;

    strcpy(resName, app_resources.progName);
    strcat(resName, ".");
    strcat(resName, name);

    if (src != NULL) {
	value = XtNewString(src);
	trim = utTrimSpaces(value);
	XrmPutStringResource(&xrmDB, resName, trim);
	XtFree(value);
    } else {
	XrmPutStringResource(&xrmDB, resName, "");
    }
}
static void putStr(name, src)
char *name;
char *src;
{
    char resName[512];
    char *trim;
    char *value;

    strcpy(resName, app_resources.progName);
    strcat(resName, ".");
    strcat(resName, name);

    if (src != NULL && *src != '\0') {
	value = XtNewString(src);
	trim = utTrimSpaces(value);
	XrmPutStringResource(&xrmDB, resName, trim);
	XtFree(value);
    } 
}
static void putBool(name, src)
char *name;
Boolean src;
{
    char resName[512];

    strcpy(resName, app_resources.progName);
    strcat(resName, ".");
    strcat(resName, name);

    if (src) {
	XrmPutStringResource(&xrmDB, resName, "on");
    } else {
	XrmPutStringResource(&xrmDB, resName, "off");
    }
}
static void putInt(name, src)
char *name;
int src;
{
    char resName[512];
    char numStr[20];

    strcpy(resName, app_resources.progName);
    strcat(resName, ".");
    strcat(resName, name);

    sprintf(numStr, "%d", src);
    XrmPutStringResource(&xrmDB, resName, numStr);
}
void CustomizeSave()
{
    char tmpStr[512];
#if defined(MOTIF_V10) && (XtSpecificationRelease == 3)
    static int width = 0;	/* Gross hack dept: real X11R3 measures */
    static int height = 0;	/* using Dimension (shorts) but Motif V1.0 */
    static int x = 0;		/* uses ints. So we use int and zero 'em */
    static int y = 0;		/* first so the high order word is OK.   */
    static int rows = 0;
#else /* Not X11R3 */
    static Dimension width = 0;
    static Dimension height = 0;
    static Position x = 0;
    static Position y = 0;
    static Dimension rows = 0;
#endif
    static Arg sizeArgs[] = {
	{XtNwidth, (XtArgVal) &width},
	{XtNheight,(XtArgVal) &height},
    };
    static Arg rowArgs[] = {
#ifdef MOTIF
	{XmNrows, (XtArgVal) &rows},
#else
	{DwtNrows, (XtArgVal) &rows},
#endif
    };

    xrmDB = XrmGetFileDatabase(customName);
#ifndef VMS
    if (xrmDB != NULL) {
	sprintf(tmpStr, "%s.old", customName);
	utCopyFile(customName, tmpStr);
    }
#endif
    XtGetValues(MainWindow, sizeArgs, XtNumber(sizeArgs));
    XtTranslateCoords(TopLevel, 0, 0, &x, &y);
    sprintf(tmpStr, "%dx%d+%d+%d", width, height, x, y);
    if (!app_resources.twoWindows) {
	putStr("layout", tmpStr);
    } else {
	putStr("indexLayout", tmpStr);
	XtGetValues(ArtMainWindow, sizeArgs, XtNumber(sizeArgs));
	if (XtIsRealized(ArtTopLevel)) {
	    XtTranslateCoords(ArtTopLevel, 0, 0, &x, &y);
	    sprintf(tmpStr, "%dx%d+%d+%d", width, height, x, y);
	    putStr("artLayout", tmpStr);
	}
    }
    if (ComposeTopLevel && XtIsRealized(ComposeTopLevel)) {
	XtGetValues(ComposeTopLevel, sizeArgs, XtNumber(sizeArgs));
	XtTranslateCoords(ComposeTopLevel, 0, 0, &x, &y);
	sprintf(tmpStr, "%dx%d+%d+%d", width, height, x, y);
	putStr("compLayout", tmpStr);
    }
    app_resources.topLines =  SListGetRows(TopList);
    putInt("topLines",app_resources.topLines);
    XtGetValues(ArticleText, rowArgs, XtNumber(rowArgs));
    putInt("textLines", rows + 1);
    putNullStr("replyPath",app_resources.replyPath);
    putNullStr("saveDir",app_resources.saveDir);
    putStr("newsrcFile",app_resources.newsrcFile);
    putStr("saveNewsrcFile",app_resources.saveNewsrcFile);
    putStr("signatureFile",app_resources.signatureFile);
    putStr("nntpServer",app_resources.nntpServer);
    tmpStr[0] = '\0';
    if (app_resources.saveMode & MAILBOX_SAVE) {
	strcpy(tmpStr, "mailbox,");
    }
    if (app_resources.saveMode & NOHEADERS_SAVE) {
	strcat(tmpStr,"noheaders,");
    }
    if (app_resources.saveMode & HEADERS_SAVE) {
	strcat(tmpStr,"headers,");
    }
    if (app_resources.saveMode & SUBDIRS_SAVE) {
	strcat(tmpStr,"subdirs");
    } else {
	if (app_resources.saveMode & ONEDIR_SAVE) {
	    strcat(tmpStr,"onedir");
	} else {
	    strcat(tmpStr, "default");
	}
    }
    putStr("saveMode", tmpStr);

    putStr("savePostings",app_resources.savePostings);
    putStr("deadLetters",app_resources.deadLetters);
    putInt("minLines",app_resources.minLines);
    putInt("maxLines",app_resources.maxLines);
#ifndef VMS
    putNullStr("mailer",app_resources.mailer);
#else
    putNullStr("personalName",app_resources.personalName);
#endif
    putBool("subjectRead",app_resources.subjectRead);
    putBool("info",app_resources.info);
    putStr("tmpDir",app_resources.tmpDir);
    tmpStr[0] = '\0';
    if (app_resources.confirmMode & NG_QUIT)
	strcat(tmpStr,"ngQuit,");
    if (app_resources.confirmMode & NG_EXIT)
	strcat(tmpStr,"ngExit,");
    if (app_resources.confirmMode & NG_CATCHUP)
	strcat(tmpStr,"ngCatchUp,");
    if (app_resources.confirmMode & ART_CATCHUP)
	strcat(tmpStr,"artCatchUp,");
    if (app_resources.confirmMode & NG_UNSUBSCRIBE)
	strcat(tmpStr,"ngUnsub,");
    if (app_resources.confirmMode & ART_UNSUBSCRIBE)
	strcat(tmpStr,"artUnsub,");
    if (app_resources.confirmMode & ART_ENDACTION)
	strcat(tmpStr,"artEndAction,");
    if (app_resources.confirmMode & ART_SAVE)
	strcat(tmpStr,"artSave,");
    tmpStr[strlen(tmpStr)-1] = '\0';
    putNullStr("confirm", tmpStr);

    putBool("killFiles",app_resources.killFiles);
    putStr("editorCommand",app_resources.editorCommand);
    putStr("organization",app_resources.organization);
    putNullStr("replyTo",app_resources.replyTo);
    putBool("includeHeader",app_resources.includeHeader);
    putBool("includeSep",app_resources.includeSep);
    putBool("updateNewsrc",app_resources.updateNewsrc);
    putInt("compLineLength",app_resources.lineLength);
    putInt("compBreakLength",app_resources.breakLength);
    putInt("rescanTime",app_resources.rescanTime);
    putBool("pageArticles",app_resources.pageArticles);
    putBool("authorFullName",app_resources.authorFullName);
    putBool("displayLineCount",app_resources.displayLineCount);
    putBool("autoRead",app_resources.autoRead);
    putBool("autoWrap",app_resources.autoWrap);
    putBool("cc",app_resources.cc);
    putBool("retainKilled",app_resources.retainKilledArticles);
    putStr("addButtonList",app_resources.addButtonList);
    putStr("ngButtonList",app_resources.ngButtonList);
    putStr("allButtonList",app_resources.allButtonList);
    putStr("artButtonList",app_resources.artButtonList);
    putStr("artSpecButtonList",app_resources.artSpecButtonList);
    putStr("printCommand",app_resources.printCommand);
    putNullStr("includePrefix",app_resources.includePrefix);
    putNullStr("hostName",app_resources.hostName);
    putNullStr("replyPath",app_resources.replyPath);
    putStr("addPopupList",app_resources.addPopupList);
    putStr("ngPopupList",app_resources.ngPopupList);
    putStr("allPopupList",app_resources.allPopupList);
    putStr("artPopupList",app_resources.artPopupList);
    putInt("popupButton",app_resources.popupButton);

    XrmPutFileDatabase(xrmDB, customName);
    xrmDB = NULL;
    updatesDone = False;
}
Boolean getUpdateStatus()
{
    return updatesDone;
}
static void menuOK()
{
    UpdateMenu();
    XtUnmanageChild(customDialog);
}
static void menuApply()
{
    UpdateMenu();
}
static void menuCancel()
{
    XtUnmanageChild(customDialog);
}
static void cgOK()
{
    UpdateGeneral();
    XtUnmanageChild(generalDialog);
}
static void cgApply()
{
    UpdateGeneral();
}
static void cgCancel()
{
    XtUnmanageChild(generalDialog);
}
static void ccOK()
{
    UpdateConfirm();
    XtUnmanageChild(confirmDialog);
}
static void ccApply()
{
    UpdateConfirm();
}
static void ccCancel()
{
    XtUnmanageChild(confirmDialog);
}
static void cmOK()
{
    UpdateMail();
    XtUnmanageChild(mailDialog);
}
static void cmApply()
{
    UpdateMail();
}
static void cmCancel()
{
    XtUnmanageChild(mailDialog);
}
static void cpOK()
{
    UpdatePost();
    XtUnmanageChild(postDialog);
}
static void cpApply()
{
    UpdatePost();
}
static void cpCancel()
{
    XtUnmanageChild(postDialog);
}
static void crOK()
{
    UpdateRead();
    XtUnmanageChild(readDialog);
}
static void crApply()
{
    UpdateRead();
}
static void crCancel()
{
    XtUnmanageChild(readDialog);
}

static void makeCustomMenu()
{
    Arg arg[20];
    int i;
    XmString str;

    i = 0;
#ifdef MOTIF
    str = XmStringLtoRCreate("mxrn: Customize/Menus", XmSTRING_DEFAULT_CHARSET);
#else
    str = DwtLatin1String("dxrn: Customize/Menus");
#endif
    XtSetArg(arg[i], XmNborderWidth,	1);			i++;
    XtSetArg(arg[i], XmNautoUnmanage,	False);			i++;
    XtSetArg(arg[i], XmNdialogTitle,	str);			i++;
    XtSetArg(arg[i], XmNresizePolicy,	XmRESIZE_GROW);		i++;
    XtSetArg(arg[i], XmNfractionBase,	100);			i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNdialogStyle,	XmDIALOG_APPLICATION_MODAL);i++;
    XtSetArg(arg[i], XmNunitType,	Xm100TH_FONT_UNITS);	i++;
    XtSetArg(arg[i], XmNwidth, 		550*16); 		i++;
    XtSetArg(arg[i], XmNheight,		430*16);			i++;
#else
    XtSetArg(arg[i], DwtNwidth, 	550);			i++;
    XtSetArg(arg[i], DwtNheight,	430);			i++;
    XtSetArg(arg[i], DwtNunits,		DwtFontUnits);		i++;
#endif
    customDialog = XmCreateFormDialog(TopLevel, "customDialog",arg,i);
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Click on these items to\nadd them to the menu",
	XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);			i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_FORM);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_FORM);		i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNleftOffset,	0);			i++;
    XtSetArg(arg[i], XmNtopOffset,	0);			i++;
#else
    XtSetArg(arg[i], XmNleftOffset,	1);			i++;
    XtSetArg(arg[i], XmNtopOffset,	1);			i++;
#endif
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_POSITION);	i++;
    XtSetArg(arg[i], XmNrightPosition,	50);			i++;
    XtManageChild(addLabel =
	XmCreateLabel(customDialog,"addLabel",arg,i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate(
	"Click on these items to\nremove them from the menu",
	XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);			i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_FORM);		i++;
    XtSetArg(arg[i], XmNleftOffset,	0);			i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNtopOffset,	0);			i++;
#else
    XtSetArg(arg[i], XmNtopOffset,	1);			i++;
    XtSetArg(arg[i], XmNrightOffset,	1);			i++;
#endif
    XtSetArg(arg[i], XmNleftWidget,	addLabel);		i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_FORM);		i++;
    XtManageChild(removeLabel =
	XmCreateLabel(customDialog,"removeLabel",arg,i));
    XmStringFree(str);

#ifdef MOTIF
    i = 0;
    XtSetArg(arg[i], XmNborderWidth,	25);			i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_FORM);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNleftOffset,	800);			i++;
    XtSetArg(arg[i], XmNtopOffset,	50);			i++;
    XtSetArg(arg[i], XmNtopWidget,	addLabel);		i++;
    XtSetArg(arg[i], XmNscrollBarDisplayPolicy,	XmSTATIC);	i++;
    XtSetArg(arg[i], XmNvisualPolicy,	XmVARIABLE);		i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNscrollingPolicy, XmAPPLICATION_DEFINED);i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_POSITION);	i++;
    XtSetArg(arg[i], XmNrightPosition,	50);			i++;
    XtSetArg(arg[i], XmNrightOffset,	400);			i++;
    XtManageChild(addScroll =
	XmCreateScrolledWindow(customDialog,"addScroll", arg, i));

    i = 0;
    XtSetArg(arg[i], XmNautomaticSelection, False);		i++;
    XtSetArg(arg[i], XmNlistSizePolicy, XmCONSTANT);		i++;
    XtSetArg(arg[i], XmNselectionPolicy,XmSINGLE_SELECT);	i++;
    XtSetArg(arg[i], XmNvisibleItemCount,10);			i++;
    XtSetArg(arg[i], XmNscrollBarDisplayPolicy, XmSTATIC);	i++;
    XtManageChild(
	addList = XmCreateList(addScroll,"addList", arg, i));
#else
    i = 0;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_FORM);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNleftOffset,	8);			i++;
    XtSetArg(arg[i], XmNtopOffset,	5);			i++;
    XtSetArg(arg[i], XmNtopWidget,	addLabel);		i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_NONE);	i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_POSITION);	i++;
    XtSetArg(arg[i], XmNrightPosition,	50);			i++;
    XtSetArg(arg[i], XmNrightOffset,	1);			i++;
    XtSetArg(arg[i], DwtNwidth,		200);			i++;
    XtSetArg(arg[i], DwtNsingleSelection,True);			i++;
    XtSetArg(arg[i], DwtNhorizontal,	True);			i++;
    XtSetArg(arg[i], XmNvisibleItemCount,10);			i++;
    XtManageChild(
	addList = DwtListBoxCreate(customDialog,"addList", arg, i));
#endif

#ifdef MOTIF
    i = 0;
    XtSetArg(arg[i], XmNborderWidth,	25);			i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNleftWidget,	addScroll);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNtopWidget,	removeLabel);		i++;
    XtSetArg(arg[i], XmNscrollBarDisplayPolicy,	XmSTATIC);	i++;
    XtSetArg(arg[i], XmNvisualPolicy,	XmVARIABLE);		i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_FORM);		i++;
    XtSetArg(arg[i], XmNrightOffset,	800);			i++;
    XtSetArg(arg[i], XmNscrollingPolicy,XmAPPLICATION_DEFINED);	i++;
    XtSetArg(arg[i], XmNleftOffset,	800);			i++;
    XtSetArg(arg[i], XmNtopOffset,	50);			i++;

    XtManageChild(removeScroll =
	XmCreateScrolledWindow(customDialog,"removeScroll", arg, i));

    i = 0;
    XtSetArg(arg[i], XmNautomaticSelection, False);		i++;
    XtSetArg(arg[i], XmNlistSizePolicy, XmCONSTANT);		i++;
    XtSetArg(arg[i], XmNselectionPolicy,XmSINGLE_SELECT);	i++;
    XtSetArg(arg[i], XmNvisibleItemCount,10);			i++;
    XtSetArg(arg[i], XmNscrollBarDisplayPolicy, XmSTATIC);	i++;

    XtManageChild(
	removeList = XmCreateList(removeScroll, "removeList", arg, i));
#else
    i = 0;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNleftWidget,	addList);		i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNtopWidget,	removeLabel);		i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_FORM);		i++;
    XtSetArg(arg[i], XmNrightOffset,	4);			i++;
    XtSetArg(arg[i], XmNleftOffset,	4);			i++;
    XtSetArg(arg[i], XmNtopOffset,	5);			i++;
    XtSetArg(arg[i], DwtNwidth,		200);			i++;
    XtSetArg(arg[i], DwtNsingleSelection,True);			i++;
    XtSetArg(arg[i], DwtNhorizontal,	True);			i++;
    XtSetArg(arg[i], XmNvisibleItemCount,10);			i++;

    XtManageChild(
	removeList = DwtListBoxCreate(customDialog, "removeList", arg, i));
#endif

    i = 0;
    str = XmStringLtoRCreate("OK", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);			i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);	i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);		i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);	i++;
    XtSetArg(arg[i], XmNleftPosition,	7);			i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	52);			i++;
    XtSetArg(arg[i], XmNshowAsDefault,	True);			i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	5);			i++;
#endif
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);		i++;
    XtSetArg(arg[i], XmNactivateCallback,menuOKCallback); 	i++;

    XtManageChild(
	okButton = XmCreatePushButton(customDialog, "okButton", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Apply", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);	i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);		i++;
    XtSetArg(arg[i], XmNleftPosition,	44);			i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);		i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);	i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	142);			i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	5);			i++;
#endif
    XtSetArg(arg[i], XmNlabelString,	str);			i++;
    XtSetArg(arg[i], XmNactivateCallback,menuApplyCallback); 	i++;
    XtManageChild(
	applyButton = XmCreatePushButton(customDialog, "applyButton", arg, i));
    XmStringFree(str);

    i = 0;
    str = XmStringLtoRCreate("Cancel", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_POSITION);	i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_NONE);		i++;
    XtSetArg(arg[i], XmNleftPosition,	78);			i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);	i++;
    XtSetArg(arg[i], XmNrightAttachment,XmATTACH_NONE);		i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNbottomOffset,	142);			i++;
#else
    XtSetArg(arg[i], XmNbottomOffset,	5);			i++;
#endif
    XtSetArg(arg[i], XmNlabelString,	str);			i++;
    XtSetArg(arg[i], XmNactivateCallback,menuCancelCallback); 	i++;
    XtManageChild(
	cancelButton = XmCreatePushButton(customDialog, "cancelButton", arg,i));
    XmStringFree(str);

#ifndef MOTIF_V10
    i = 0;
    str = XmStringLtoRCreate(" ", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNlabelString,	str);			i++;
    XtSetArg(arg[i], XmNleftAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNtopAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(arg[i], XmNbottomAttachment,XmATTACH_FORM);	i++;
    XtSetArg(arg[i], XmNrightAttachment, XmATTACH_FORM);	i++;
#ifdef MOTIF
    XtSetArg(arg[i], XmNleftWidget,	removeScroll); 		i++;
    XtSetArg(arg[i], XmNtopWidget,	removeScroll);		i++;
    XtSetArg(arg[i], XmNbottomOffset,	128);			i++;
    XtSetArg(arg[i], XmNleftOffset,	20);			i++;
    XtSetArg(arg[i], XmNrightOffset,	50);			i++;
    XtSetArg(arg[i], XmNtopOffset,	471);			i++;
#else
    XtSetArg(arg[i], XmNleftWidget,	removeList); 		i++;
    XtSetArg(arg[i], XmNtopWidget,	removeList);		i++;
    XtSetArg(arg[i], XmNbottomOffset,	1);			i++;
    XtSetArg(arg[i], XmNleftOffset,	1);			i++;
    XtSetArg(arg[i], XmNrightOffset,	1);			i++;
    XtSetArg(arg[i], XmNtopOffset,	5);			i++;
#endif

    XtManageChild(
	XmCreateLabel(customDialog, "dummyLabel", arg, i));
    XmStringFree(str);
#endif

    i = 0;

    XtSetArg(arg[i], XmNdefaultButton, okButton);		i++;
    XtSetArg(arg[i], XmNcancelButton, cancelButton);		i++;
    XtSetValues(customDialog, arg, i);
}
static void LoadCustomList(string)
char	*string;
{
    int i, j, add, rem;
    char *ptr, *token, *savePtr;
    Arg arg[10];

    if (customDialog == 0) {
	makeCustomMenu();
    }
    i = 0;
#ifdef MOTIF
    XtSetArg(arg[i], XmNsingleSelectionCallback, addCallback);	i++;
#else
    XtSetArg(arg[i], DwtNsingleCallback, addCallback);		i++;
    XtSetArg(arg[i], DwtNsingleConfirmCallback, addCallback);	i++;
#endif
    XtSetValues(addList, arg, i);
    i = 0;
#ifdef MOTIF
    XtSetArg(arg[i], XmNsingleSelectionCallback, removeCallback);i++;
#else
    XtSetArg(arg[i], DwtNsingleCallback, removeCallback);	i++;
    XtSetArg(arg[i], DwtNsingleConfirmCallback, removeCallback);i++;
#endif
    XtSetValues(removeList, arg, i);
    for (i = 0; i < currentCount;i++) {
	if (currentButtons[i].listString == NULL)
	    currentButtons[i].listString =
	      XmStringLtoRCreate(currentButtons[i].label,
			XmSTRING_DEFAULT_CHARSET);
	if (string != NULL) {
	    currentButtons[i].addPos = -1;
	    currentButtons[i].removePos = -1;
	} else {
	    currentButtons[i].addPos = -1;
	    currentButtons[i].removePos = i;
	}
    }
    add = 0;
    rem = 0;

    if (string != NULL) {
	savePtr = ptr = XtNewString(string);

	while ((token = strtok(ptr, ", \t\n")) != NIL(char)) {
	    for (j = 0; j < currentCount; j++) {	
		if (STREQ(token, (char *) currentButtons[j].name)) {
		    currentButtons[j].removePos = rem;
		    removeItems[rem] = currentButtons[j].listString;
		    rem++;
		    break;
		}
	    }
	    ptr = NULL;
	}
	XtFree (savePtr);
	for (i = 0; i < currentCount; i++) {
	    if (currentButtons[i].removePos == -1) {
		currentButtons[i].addPos = add;
		addItems[add] = currentButtons[i].listString;
		add++;
	    }
	}
    } else {
	for (rem = 0; rem < currentCount; rem++)
	    removeItems[rem] = currentButtons[rem].listString;
    }
    i = 0;
    XtSetArg(arg[i], XmNitems, addItems);i++;
    XtSetArg(arg[i], XmNitemCount, add); i++;
    XtSetValues(addList, arg, i);
    i = 0;
    XtSetArg(arg[i], XmNitems, removeItems);i++;
    XtSetArg(arg[i], XmNitemCount, rem); i++;
    XtSetValues(removeList, arg, i);
}
static void addClick(w, call_data, click)
Widget 			w;
caddr_t			call_data;
#ifdef MOTIF
XmListCallbackStruct 	*click;
#else
DwtListBoxCallbackStruct *click;
#endif
{
    int i, add, rem, addCount, removeCount;
#ifdef MOTIF
    int selItem = click->item_position - 1;
    Arg arg[10];
#else
    int selItem = click->item_number - 1;
#endif
    int item;

    XmListDeselectAllItems(w);
    addCount = add = 0;
    removeCount = rem = 0;
    for (i = 0; i < currentCount; i++) {
	if (currentButtons[i].addPos != -1) {
	    addCount++;
	}
	if (currentButtons[i].removePos != -1) {
	    removeCount++;
	}
	if (currentButtons[i].addPos == selItem) {
	    item = i;
	}
    }

    for (i = 0; i < currentCount; i++) {
	if (currentButtons[i].addPos > selItem) {
	    currentButtons[i].addPos = currentButtons[i].addPos - 1;
	}
    }
    currentButtons[item].removePos = removeCount;
    currentButtons[item].addPos = -1;

    addCount = addCount - 1;
    removeCount = removeCount + 1;
    for (i = 0; i < currentCount; i++) {
	if ((add = currentButtons[i].addPos) != -1) {
	    addItems[add] = currentButtons[i].listString;
	}
	if ((rem = currentButtons[i].removePos) != -1) {
	    removeItems[rem] = currentButtons[i].listString;
	}
    }
#ifdef MOTIF
    XmListDeletePos(addList, selItem+1);
    if (removeCount > 1) {
        XmListAddItem(removeList, removeItems[removeCount-1], 0);
    } else {
	i = 0;
	XtSetArg(arg[i], XmNitems, removeItems);i++;
	XtSetArg(arg[i], XmNitemCount, removeCount); i++;
	XtSetValues(removeList, arg, i);
    }
#else
    DwtListBoxDeletePos(addList, selItem+1);
    DwtListBoxAddItem(removeList, removeItems[removeCount-1], removeCount);
#endif
}
static void removeClick(w, call_data, click)
Widget 			w;
caddr_t			call_data;
#ifdef MOTIF
XmListCallbackStruct 	*click;
#else
DwtListBoxCallbackStruct 	*click;
#endif
{
    int i, add, rem, addCount, removeCount;
#ifdef MOTIF
    int selItem = click->item_position - 1;
    Arg arg[10];
#else
    int selItem = click->item_number - 1;
#endif
    int item;

    XmListDeselectAllItems(w);
    addCount = add = 0;
    removeCount = rem = 0;
    for (i = 0; i < currentCount; i++) {
	if (currentButtons[i].addPos != -1) {
	    addCount++;
	}
	if (currentButtons[i].removePos != -1) {
	    removeCount++;
	}
	if (currentButtons[i].removePos == selItem) {
	    item = i;
	}
    }
    for (i = 0; i < currentCount; i++) {
	if (currentButtons[i].removePos > selItem) {
	    currentButtons[i].removePos = currentButtons[i].removePos - 1;
	}
    }

    currentButtons[item].addPos = addCount;
    currentButtons[item].removePos = -1;

    addCount = addCount + 1;
    removeCount = removeCount - 1;
    for (i = 0; i < currentCount; i++) {
	if ((add = currentButtons[i].addPos) != -1) {
	    addItems[add] = currentButtons[i].listString;
	}
	if ((rem = currentButtons[i].removePos) != -1) {
	    removeItems[rem] = currentButtons[i].listString;
	}
    }
#ifdef MOTIF
    XmListDeletePos(removeList, selItem+1);
    if (addCount > 1) {
        XmListAddItem(addList, addItems[addCount-1], 0);
    } else {
	i = 0;
	XtSetArg(arg[i], XmNitems, addItems);i++;
	XtSetArg(arg[i], XmNitemCount, addCount); i++;
	XtSetValues(addList, arg, i);
    }
#else
    DwtListBoxDeletePos(removeList, selItem+1);
    DwtListBoxAddItem(addList, addItems[addCount-1], addCount);
#endif
}
static void CustomizeAddMenus()
{
    currentButtons = addButtons;
    currentCount = XtNumber(addButtons);
    menuMode = addMenu;
    LoadCustomList(app_resources.addButtonList);
    XtManageChild(customDialog);
    XtRealizeWidget(XtParent(customDialog));
}
static void CustomizeAddPopups()
{
    currentButtons = addButtons;
    currentCount = XtNumber(addButtons);
    menuMode = addPop;
    LoadCustomList(app_resources.addPopupList);
    XtManageChild(customDialog);
    XtRealizeWidget(XtParent(customDialog));
}
static void CustomizeAllMenus()
{
    currentButtons = allButtons;
    currentCount = XtNumber(allButtons);
    menuMode = allMenu;
    LoadCustomList(app_resources.allButtonList);
    XtManageChild(customDialog);
    XtRealizeWidget(XtParent(customDialog));
}
static void CustomizeAllPopups()
{
    currentButtons = allButtons;
    currentCount = XtNumber(allButtons);
    menuMode = allPop;
    LoadCustomList(app_resources.allPopupList);
    XtManageChild(customDialog);
    XtRealizeWidget(XtParent(customDialog));
}
static void CustomizeArtMenus()
{
    currentButtons = artButtons;
    currentCount = XtNumber(artButtons);
    menuMode = artMenu;
    LoadCustomList(app_resources.artButtonList);
    XtManageChild(customDialog);
    XtRealizeWidget(XtParent(customDialog));
}
static void CustomizeArtPopups()
{
    currentButtons = artButtons;
    currentCount = XtNumber(artButtons);
    menuMode = artPop;
    LoadCustomList(app_resources.artPopupList);
    XtManageChild(customDialog);
    XtRealizeWidget(XtParent(customDialog));
}
static void CustomizeNgMenus()
{
    currentButtons = ngButtons;
    currentCount = XtNumber(ngButtons);
    menuMode = ngMenu;
    LoadCustomList(app_resources.ngButtonList);
    XtManageChild(customDialog);
    XtRealizeWidget(XtParent(customDialog));
}
static void CustomizeNgPopups()
{
    currentButtons = ngButtons;
    currentCount = XtNumber(ngButtons);
    menuMode = ngPop;
    LoadCustomList(app_resources.ngPopupList);
    XtManageChild(customDialog);
    XtRealizeWidget(XtParent(customDialog));
}
static void UpdateMenu()
{
    char menuString[2048];
    int i,j;

    menuString[0] = '\0';

    updatesDone = True;
    for (i = 0; i < currentCount; i++) {
        for (j = 0; j < currentCount; j++) {
	    if (currentButtons[j].removePos == i) {
		strcat(menuString,currentButtons[j].name);
		strcat(menuString,",");
		break;
	    }
	}
    }
    menuString[strlen(menuString)-1] = '\0';
    switch (menuMode) {
	case addMenu:
	    app_resources.addButtonList = XtNewString(menuString);
	    createButtons(ADD_MODE);
	    break;
	case addPop:
	    app_resources.addPopupList = XtNewString(menuString);
	    XtUnmanageChildren(AddPopupButtons, AddPopupCount);
	    for (i = 0; i < AddPopupCount; i++) {
		XtDestroyWidget(AddPopupButtons[i]);
	    }
	    AddPopupCount = AddCount;
	    doPopups(app_resources.addPopupList, popup_menu, AddPopupButtons,
		AddButtonList, &AddPopupCount);
	    if (Mode == ADD_MODE) {
		XtManageChildren(AddPopupButtons, AddPopupCount);
		resetPopupSize();
	    }
	    break;
	case allMenu:
	    app_resources.allButtonList = XtNewString(menuString);
	    createButtons(ALL_MODE);
	    break;
	case allPop:
	    app_resources.allPopupList = XtNewString(menuString);
	    XtUnmanageChildren(AllPopupButtons, AllPopupCount);
	    for (i = 0; i < AllPopupCount; i++) {
		XtDestroyWidget(AllPopupButtons[i]);
	    }
	    AllPopupCount = AllCount;
	    doPopups(app_resources.allPopupList, popup_menu, AllPopupButtons,
		AllButtonList, &AllPopupCount);
	    if (Mode == ALL_MODE) {
		XtManageChildren(AllPopupButtons, AllPopupCount);
		resetPopupSize();
	    }
	    break;
	case artMenu:
	    app_resources.artButtonList = XtNewString(menuString);
	    createButtons(ARTICLE_MODE);
	    break;
	case artPop:
	    app_resources.artPopupList = XtNewString(menuString);
	    XtUnmanageChildren(ArtPopupButtons, ArtPopupCount);
	    for (i = 0; i < ArtPopupCount; i++) {
		XtDestroyWidget(ArtPopupButtons[i]);
	    }
	    if (app_resources.twoWindows) { 
		XtUnmanageChildren(ArtWinPopupButtons, ArtWinPopupCount);
		for (i = 0; i < ArtWinPopupCount; i++) {
		    XtDestroyWidget(ArtWinPopupButtons[i]);
		}
	    }
	    ArtPopupCount = ArtCount;
	    ArtSpecPopupCount = ArtSpecCount;
	    ArtWinPopupCount = ArtCount;
	    doPopups(app_resources.artPopupList, popup_menu, ArtPopupButtons,
		ArtButtonList, &ArtPopupCount);
	    if (Mode == ARTICLE_MODE) {
		XtManageChildren(ArtPopupButtons, ArtPopupCount);
		resetPopupSize();
	    }
	    if (app_resources.twoWindows) {
		doPopups(app_resources.artPopupList, artpopup_menu,
			 ArtWinPopupButtons, ArtButtonList, &ArtWinPopupCount);
		if (Mode == ARTICLE_MODE) {
		    XtManageChildren(ArtWinPopupButtons, ArtWinPopupCount);
		    resetPopupSize();
		}
	    }
	    break;
	case ngMenu:
	    app_resources.ngButtonList = XtNewString(menuString);
	    createButtons(NEWSGROUP_MODE);
	    break;
	case ngPop:
	    app_resources.ngPopupList = XtNewString(menuString);
	    XtUnmanageChildren(NgPopupButtons, NgPopupCount);
	    for (i = 0; i < NgPopupCount; i++) {
		XtDestroyWidget(NgPopupButtons[i]);
	    }
	    NgPopupCount = NgCount;
	    doPopups(app_resources.ngPopupList, popup_menu, NgPopupButtons,
		NgButtonList, &NgPopupCount);
	    if (Mode == NEWSGROUP_MODE) {
		XtManageChildren(NgPopupButtons, NgPopupCount);
		resetPopupSize();
	    }
	    break;
    }
}
