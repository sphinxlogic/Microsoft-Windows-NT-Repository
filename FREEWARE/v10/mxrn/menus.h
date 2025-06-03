/* menus.h - definitions for menu processing */
#ifndef _menus_h
#define _menus_h
/*
 * Redefinitions for Motif widgets to ease XUI port
 */
#ifndef MOTIF
#define XmCreatePushButton DwtPushButtonCreate
#define XmCreatePushButtonGadget DwtPushButtonGadgetCreate
#define XmCreateLabel DwtLabelCreate
#define XmCreateLabelGadget DwtLabelGadgetCreate
#define XmCreateBulletinBoardDialog DwtDialogBoxPopupCreate
#define XmCreateScrolledList DwtListBoxCreate
#define XmCreateFormDialog DwtAttachedDBPopupCreate
#define XmCreateForm DwtAttachedDBCreate
#define XmCreateRadioBox DwtRadioBoxCreate
#define XmCreateText DwtSTextCreate
#define XmCreateToggleButton DwtToggleButtonCreate
#define XmCreateSeparator DwtSeparatorCreate
#define XmToggleButtonGetState DwtToggleButtonGetState
#define XmToggleButtonSetState DwtToggleButtonSetState
#define XmTextSetString DwtSTextSetString
#define XmTextGetString DwtSTextGetString
#define XmStringLtoRCreate(a,b) DwtLatin1String(a)
#define XmStringFree XtFree
#define XmNx DwtNx
#define XmNy DwtNy
#define XmNwidth DwtNwidth
#define XmNheight DwtNheight
#define XmNlabelString DwtNlabel
#define XmString DwtCompString
#define XmNborderWidth DwtNborderWidth
#define XmNresizePolicy DwtNresize
#define XmRESIZE_GROW DwtResizeGrowOnly
#define XmNautoUnmanage DwtNautoUnmanage
#define XmNcolumns DwtNcols
#define XmNorientation DwtNorientation
#define XmHORIZONTAL DwtOrientationHorizontal
#define XmNfontList DwtNfont
#define XmNdialogStyle DwtNstyle
#define XmNdialogTitle DwtNtitle
#define XmDIALOG_APPLICATION_MODAL DwtModal
#define XmNvisibleItemCount DwtNvisibleItemsCount
#define XmNactivateCallback DwtNactivateCallback
#define XmNdefaultButton DwtNdefaultButton
#define XmNcancelButton DwtNcancelButton
#define XmNitems DwtNitems
#define XmNitemCount DwtNitemsCount
#define XmNsingleSelectionCallback DwtNsingleCallback
#define XmListDeselectAllItems DwtListBoxDeselectAllItems
#define XmNfractionBase DwtNfractionBase
#define XmNleftAttachment DwtNadbLeftAttachment
#define XmNrightAttachment DwtNadbRightAttachment
#define XmNtopAttachment DwtNadbTopAttachment
#define XmNbottomAttachment DwtNadbBottomAttachment
#define XmNleftPosition DwtNadbLeftPosition
#define XmNrightPosition DwtNadbRightPosition
#define XmNtopPosition DwtNadbTopPosition
#define XmNbottomPosition DwtNadbBottomPosition
#define XmNleftOffset DwtNadbLeftOffset
#define XmNrightOffset DwtNadbRightOffset
#define XmNtopOffset DwtNadbTopOffset
#define XmNbottomOffset DwtNadbBottomOffset
#define XmNleftWidget DwtNadbLeftWidget
#define XmNtopWidget DwtNadbTopWidget
#define XmNbottomWidget DwtNadbBottomWidget
#define XmATTACH_POSITION DwtAttachPosition
#define XmATTACH_NONE DwtAttachNone
#define XmATTACH_FORM DwtAttachAdb
#define XmATTACH_WIDGET DwtAttachWidget
#define XmATTACH_OPPOSITE_WIDGET DwtAttachOppWidget
#define XmNshowAsDefault DwtNshownAsDefault
#define XmNunitType DwtNunits
#define Xm100TH_FONT_UNITS DwtFontUnits
#define XmNradioAlwaysOne DwtNradioAlwaysOne
#endif

/*
 * Forward declarations
 */

static void UpdateMenu();
static void makeCustomMenu();
static void makeGeneralMenu();
static void doPopups();
static void CustomizeAddMenus();
static void CustomizeAddPopups();
static void CustomizeAllMenus();
static void CustomizeAllPopups();
static void CustomizeArtMenus();
static void CustomizeArtPopups();
static void CustomizeNgMenus();
static void CustomizeNgPopups();
static void CustomizeGeneral();
static void CustomizeConfirm();
static void CustomizePost();
static void CustomizeMail();
static void CustomizeRead();
void CustomizeSave();
static void menuOK();
static void menuApply();
static void menuCancel();
static void cgOK();
static void cgApply();
static void cgCancel();
static void ccOK();
static void ccApply();
static void ccCancel();
static void cmOK();
static void cmApply();
static void cmCancel();
static void cpOK();
static void cpApply();
static void cpCancel();
static void crOK();
static void crApply();
static void crCancel();
static void addClick();
static void removeClick();
static void cmOK();
static void cmApply();
static void cmCancel();

/*
 * Callbacks
 */
static XtCallbackRec addMenusCallbacks[] = {
    {CustomizeAddMenus, NULL},
    {NULL, NULL},
};
static XtCallbackRec addPopupsCallbacks[] = {
    {CustomizeAddPopups, NULL},
    {NULL, NULL},
};
static XtCallbackRec artMenusCallbacks[] = {
    {CustomizeArtMenus, NULL},
    {NULL, NULL},
};
static XtCallbackRec artPopupsCallbacks[] = {
    {CustomizeArtPopups, NULL},
    {NULL, NULL},
};
static XtCallbackRec ngMenusCallbacks[] = {
    {CustomizeNgMenus, NULL},
    {NULL, NULL},
};
static XtCallbackRec ngPopupsCallbacks[] = {
    {CustomizeNgPopups, NULL},
    {NULL, NULL},
};
static XtCallbackRec allMenusCallbacks[] = {
    {CustomizeAllMenus, NULL},
    {NULL, NULL},
};
static XtCallbackRec allPopupsCallbacks[] = {
    {CustomizeAllPopups, NULL},
    {NULL, NULL},
};
static XtCallbackRec artGeneralCallbacks[] = {
    {CustomizeGeneral, NULL},
    {NULL, NULL},
};
static XtCallbackRec addGeneralCallbacks[] = {
    {CustomizeGeneral, NULL},
    {NULL, NULL},
};
static XtCallbackRec ngGeneralCallbacks[] = {
    {CustomizeGeneral, NULL},
    {NULL, NULL},
};
static XtCallbackRec allGeneralCallbacks[] = {
    {CustomizeGeneral, NULL},
    {NULL, NULL},
};
static XtCallbackRec artConfirmCustomCallbacks[] = {
    {CustomizeConfirm, NULL},
    {NULL, NULL},
};
static XtCallbackRec addConfirmCustomCallbacks[] = {
    {CustomizeConfirm, NULL},
    {NULL, NULL},
};
static XtCallbackRec ngConfirmCustomCallbacks[] = {
    {CustomizeConfirm, NULL},
    {NULL, NULL},
};
static XtCallbackRec allConfirmCustomCallbacks[] = {
    {CustomizeConfirm, NULL},
    {NULL, NULL},
};
static XtCallbackRec artPostCustomCallbacks[] = {
    {CustomizePost, NULL},
    {NULL, NULL},
};
static XtCallbackRec addPostCustomCallbacks[] = {
    {CustomizePost, NULL},
    {NULL, NULL},
};
static XtCallbackRec ngPostCustomCallbacks[] = {
    {CustomizePost, NULL},
    {NULL, NULL},
};
static XtCallbackRec allPostCustomCallbacks[] = {
    {CustomizePost, NULL},
    {NULL, NULL},
};
static XtCallbackRec artMailCustomCallbacks[] = {
    {CustomizeMail, NULL},
    {NULL, NULL},
};
static XtCallbackRec addMailCustomCallbacks[] = {
    {CustomizeMail, NULL},
    {NULL, NULL},
};
static XtCallbackRec ngMailCustomCallbacks[] = {
    {CustomizeMail, NULL},
    {NULL, NULL},
};
static XtCallbackRec allMailCustomCallbacks[] = {
    {CustomizeMail, NULL},
    {NULL, NULL},
};
static XtCallbackRec artReadCustomCallbacks[] = {
    {CustomizeRead, NULL},
    {NULL, NULL},
};
static XtCallbackRec addReadCustomCallbacks[] = {
    {CustomizeRead, NULL},
    {NULL, NULL},
};
static XtCallbackRec ngReadCustomCallbacks[] = {
    {CustomizeRead, NULL},
    {NULL, NULL},
};
static XtCallbackRec allReadCustomCallbacks[] = {
    {CustomizeRead, NULL},
    {NULL, NULL},
};
static XtCallbackRec addSaveCustomCallbacks[] = {
    {CustomizeSave, NULL},
    {NULL, NULL},
};
static XtCallbackRec allSaveCustomCallbacks[] = {
    {CustomizeSave, NULL},
    {NULL, NULL},
};
static XtCallbackRec artSaveCustomCallbacks[] = {
    {CustomizeSave, NULL},
    {NULL, NULL},
};
static XtCallbackRec ngSaveCustomCallbacks[] = {
    {CustomizeSave, NULL},
    {NULL, NULL},
};

static XtCallbackRec menuOKCallback[] = {
    {menuOK, NULL},
    {NULL, NULL}
};
static XtCallbackRec menuApplyCallback[] = {
    {menuApply, NULL},
    {NULL, NULL}
};
static XtCallbackRec menuCancelCallback[] = {
    {menuCancel, NULL},
    {NULL, NULL}
};
static XtCallbackRec cgOKCallback[] = {
    {cgOK, NULL},
    {NULL, NULL}
};
static XtCallbackRec cgApplyCallback[] = {
    {cgApply, NULL},
    {NULL, NULL}
};
static XtCallbackRec cgCancelCallback[] = {
    {cgCancel, NULL},
    {NULL, NULL}
};
static XtCallbackRec ccOKCallback[] = {
    {ccOK, NULL},
    {NULL, NULL}
};
static XtCallbackRec ccApplyCallback[] = {
    {ccApply, NULL},
    {NULL, NULL}
};
static XtCallbackRec ccCancelCallback[] = {
    {ccCancel, NULL},
    {NULL, NULL}
};
static XtCallbackRec cmOKCallback[] = {
    {cmOK, NULL},
    {NULL, NULL}
};
static XtCallbackRec cmApplyCallback[] = {
    {cmApply, NULL},
    {NULL, NULL}
};
static XtCallbackRec cmCancelCallback[] = {
    {cmCancel, NULL},
    {NULL, NULL}
};
static XtCallbackRec cpOKCallback[] = {
    {cpOK, NULL},
    {NULL, NULL}
};
static XtCallbackRec cpApplyCallback[] = {
    {cpApply, NULL},
    {NULL, NULL}
};
static XtCallbackRec cpCancelCallback[] = {
    {cpCancel, NULL},
    {NULL, NULL}
};
static XtCallbackRec crOKCallback[] = {
    {crOK, NULL},
    {NULL, NULL}
};
static XtCallbackRec crApplyCallback[] = {
    {crApply, NULL},
    {NULL, NULL}
};
static XtCallbackRec crCancelCallback[] = {
    {crCancel, NULL},
    {NULL, NULL}
};
static XtCallbackRec addCallback[] = {
    {addClick, NULL},
    {NULL, NULL}
};
static XtCallbackRec removeCallback[] = {
    {removeClick, NULL},
    {NULL, NULL}
};

/*
 * External data from buttons.c
 */
externaldef(ngmenus) Widget ngMenus[5];
externaldef(artmenus) Widget artMenus[7];
externaldef(allmenus) Widget allMenus[5];
externaldef(allmenus) Widget addMenus[3];
externaldef(addpopupbuttons) Widget *AddPopupButtons = NIL(Widget);
externaldef(ngpopupbuttons)  Widget *NgPopupButtons = NIL(Widget);
externaldef(allpopupbuttons) Widget *AllPopupButtons = NIL(Widget);
externaldef(artpopupbuttons) Widget *ArtPopupButtons = NIL(Widget);
externaldef(artwinpopupbuttons) Widget *ArtWinPopupButtons = NIL(Widget);

externalref XtCallbackRec addQuitCallbacks[];
externalref XtCallbackRec addFirstCallbacks[];
externalref XtCallbackRec addLastCallbacks[];
externalref XtCallbackRec addAfterCallbacks[];
externalref XtCallbackRec addUnsubCallbacks[];
externalref XtCallbackRec ngExitCallbacks[];
externalref XtCallbackRec ngQuitCallbacks[];
externalref XtCallbackRec ngReadCallbacks[];
externalref XtCallbackRec ngOpenCallbacks[];
externalref XtCallbackRec ngNextCallbacks[];
externalref XtCallbackRec ngPrevCallbacks[];
externalref XtCallbackRec ngCatchUpCallbacks[];
externalref XtCallbackRec ngSubscribeCallbacks[];
externalref XtCallbackRec ngUnsubCallbacks[];
externalref XtCallbackRec ngGotoCallbacks[];
externalref XtCallbackRec ngToggleGroupsCallbacks[];
externalref XtCallbackRec ngAllGroupsCallbacks[];
externalref XtCallbackRec ngRescanCallbacks[];
externalref XtCallbackRec ngPrevGroupCallbacks[];
externalref XtCallbackRec ngSelectCallbacks[];
externalref XtCallbackRec ngMoveCallbacks[];
externalref XtCallbackRec ngDisconnectCallbacks[];
externalref XtCallbackRec ngCheckPointCallbacks[];
externalref XtCallbackRec ngPostCallbacks[];
externalref XtCallbackRec ngGripeCallbacks[];
externalref XtCallbackRec allQuitCallbacks[];
externalref XtCallbackRec allSubCallbacks[];
externalref XtCallbackRec allFirstCallbacks[];
externalref XtCallbackRec allLastCallbacks[];
externalref XtCallbackRec allAfterCallbacks[];
externalref XtCallbackRec allUnsubCallbacks[];
externalref XtCallbackRec allGotoCallbacks[];
externalref XtCallbackRec allOpenCallbacks[];
externalref XtCallbackRec allSelectCallbacks[];
externalref XtCallbackRec allMoveCallbacks[];
externalref XtCallbackRec allToggleCallbacks[];
externalref XtCallbackRec allScrollCallbacks[];
externalref XtCallbackRec allScrollBackCallbacks[];
externalref XtCallbackRec allSearchCallbacks[];
externalref XtCallbackRec allContinueCallbacks[];
externalref XtCallbackRec allCancelSearchCallbacks[];
externalref XtCallbackRec artQuitCallbacks[];
externalref XtCallbackRec artNextCallbacks[];
externalref XtCallbackRec artNextUnreadCallbacks[];
externalref XtCallbackRec artPrevCallbacks[];
externalref XtCallbackRec artLastCallbacks[];
externalref XtCallbackRec artNextGroupCallbacks[];
externalref XtCallbackRec artGotoArticleCallbacks[];
externalref XtCallbackRec artCatchUpCallbacks[];
externalref XtCallbackRec artCatchUpAllCallbacks[];
externalref XtCallbackRec artFedUpCallbacks[];
externalref XtCallbackRec artMarkReadCallbacks[];
externalref XtCallbackRec artMarkUnreadCallbacks[];
externalref XtCallbackRec artUnsubCallbacks[];
externalref XtCallbackRec artScrollCallbacks[];
externalref XtCallbackRec artScrollBackCallbacks[];
externalref XtCallbackRec artScrollEndCallbacks[];
externalref XtCallbackRec artScrollBeginningCallbacks[];
externalref XtCallbackRec artSubNextCallbacks[];
externalref XtCallbackRec artSubPrevCallbacks[];
externalref XtCallbackRec artKillSessionCallbacks[];
externalref XtCallbackRec artKillLocalCallbacks[];
externalref XtCallbackRec artKillGlobalCallbacks[];
externalref XtCallbackRec artKillAuthorCallbacks[];
externalref XtCallbackRec artEditKillFileCallbacks[];
externalref XtCallbackRec artEditGlobalKillFileCallbacks[];
externalref XtCallbackRec artSubSearchCallbacks[];
externalref XtCallbackRec artAuthorSearchCallbacks[];
externalref XtCallbackRec artContinueCallbacks[];
externalref XtCallbackRec artCancelSearchCallbacks[];
externalref XtCallbackRec artNonSortedCallbacks[];
externalref XtCallbackRec artSortedCallbacks[];
externalref XtCallbackRec artStrictSortedCallbacks[];
externalref XtCallbackRec artThreadSortedCallbacks[];
externalref XtCallbackRec artPostCallbacks[];
externalref XtCallbackRec artExitCallbacks[];
externalref XtCallbackRec artCheckPointCallbacks[];
externalref XtCallbackRec artGripeCallbacks[];
externalref XtCallbackRec artListOldCallbacks[];
externalref XtCallbackRec artSaveCallbacks[];
externalref XtCallbackRec artReplyCallbacks[];
externalref XtCallbackRec artForwardCallbacks[];
externalref XtCallbackRec artFollowupCallbacks[];
externalref XtCallbackRec artFollowupReplyCallbacks[];
externalref XtCallbackRec artCancelCallbacks[];
externalref XtCallbackRec artRot13Callbacks[];
externalref XtCallbackRec artHeaderCallbacks[];
externalref XtCallbackRec artPrintCallbacks[];
typedef struct buttonList {
    char *label;
    Arg *buttonArgs;
    unsigned int size;
    char *message;
} ButtonList;
externalref ButtonList AddButtonList[];
externalref int AddPopupCount;
externalref ButtonList NgButtonList[];
externalref int NgPopupCount;
externalref ButtonList AllButtonList[];
externalref int AllPopupCount;
externalref ButtonList ArtButtonList[];
externalref int ArtPopupCount;
externalref int ArtWinPopupCount;
externalref ButtonList ArtSpecButtonList[];
externalref int ArtSpecPopupCount;
externalref int ArtSpecButtonListCount;

#ifdef __STDC__
#ifdef MOTIF
#define menuBtn(type, lbl, text)					\
    label = XmStringLtoRCreate(text, XmSTRING_DEFAULT_CHARSET);		\
    XtSetArg(cb[0], XmNlabelString, label);				\
    XtSetArg(cb[1], XmNactivateCallback, type##lbl##Callbacks);		\
    if (app_resources.useGadgets)					\
	XtManageChild(XmCreatePushButtonGadget(				\
		type##Pulldowns[i], #lbl, cb, 2));			\
    else								\
	XtManageChild(XmCreatePushButton(type##Pulldowns[i], #lbl, cb, 2));\
    XmStringFree(label)

#define menuPulldown(type, lbl)						\
    i++;								\
    type##Pulldowns[i] = XmCreatePulldownMenu(MenuBar,#lbl, NULL, 0);	\
    XtManageChild(XmCreateSeparator(type##Pulldowns[i], "sep", NULL, 0));\
    label = XmStringLtoRCreate(#lbl, XmSTRING_DEFAULT_CHARSET);	\
    XtSetArg(cb[0], XmNlabelString, label);				\
    XtSetArg(cb[1], XmNsubMenuId, type##Pulldowns[i]);			\
    if (app_resources.useGadgets)					\
	type##Menus[i] = XmCreateCascadeButtonGadget(MenuBar, #lbl, cb, 2);\
    else								\
	type##Menus[i] = XmCreateCascadeButton(MenuBar, #lbl, cb, 2);	\
    XmStringFree(label)

#define subMenuPulldown(type,name,lbl)					\
    tempPulldown = XmCreatePulldownMenu(type##Pulldowns[i], #name,NULL,0);\
    label = XmStringLtoRCreate(lbl,XmSTRING_DEFAULT_CHARSET);		\
    XtSetArg(cb[0], XmNlabelString, label);				\
    XtSetArg(cb[1], XmNsubMenuId, tempPulldown);			\
    if (app_resources.useGadgets)					\
	tempMenu = XmCreateCascadeButtonGadget(type##Pulldowns[i], #name, cb, 2);\
    else								\
	tempMenu = XmCreateCascadeButton(type##Pulldowns[i], #name, cb, 2);\
    XtManageChild(tempMenu);						\
    XmStringFree(label);

#define subMenuBtn(type,sub,name,lbl)					\
    label = XmStringLtoRCreate(lbl,XmSTRING_DEFAULT_CHARSET);		\
    XtSetArg(cb[0], XmNlabelString, label);				\
    XtSetArg(cb[1], XmNactivateCallback, sub##Callbacks);		\
    if (app_resources.useGadgets)					\
	XtManageChild(XmCreatePushButtonGadget(tempPulldown, #name, cb, 2));\
    else								\
	XtManageChild(XmCreatePushButton(tempPulldown, #name, cb, 2));	\
    XmStringFree(label);

#else
#define menuBtn(type, lbl, text)					\
    label = DwtLatin1String(text);					\
    XtSetArg(cb[0], DwtNlabel, label);					\
    XtSetArg(cb[1], DwtNactivateCallback, type##lbl##Callbacks);	\
    if (app_resources.useGadgets)					\
	XtManageChild(DwtPushButtonGadgetCreate(type##Pulldowns[i], #lbl, cb, 2));\
    else								\
	XtManageChild(DwtPushButtonCreate(type##Pulldowns[i], #lbl, cb, 2));\
    XtFree(label)

#define menuPulldown(type, lbl)						\
    i++;								\
    type##Pulldowns[i] = DwtMenu(MenuBar,#lbl, 0, 0, 			\
	DwtMenuPulldown, DwtOrientationVertical, NULL, NULL, NULL);	\
    label = DwtLatin1String(#lbl);					\
    XtSetArg(cb[0], DwtNlabel, label);					\
    XtSetArg(cb[1], DwtNsubMenuId, type##Pulldowns[i]);			\
    type##Menus[i] = DwtPullDownMenuEntryCreate(MenuBar,#lbl, cb, 2);	\
    XtFree(label)

#define subMenuPulldown(type,name,lbl)					\
    tempPulldown = DwtMenu(type##Pulldowns[i], #name,0,0,		\
	DwtMenuPulldown, DwtOrientationVertical, NULL, NULL, NULL);	\
    label = DwtLatin1String(lbl);					\
    XtSetArg(cb[0], DwtNlabel, label);					\
    XtSetArg(cb[1], DwtNsubMenuId, tempPulldown);			\
    tempMenu = DwtPullDownMenuEntryCreate(type##Pulldowns[i], #name, cb, 2);\
    XtManageChild(tempMenu);						\
    XtFree(label);

#define subMenuBtn(type,sub,name,lbl)					\
    label = DwtLatin1String(lbl);					\
    XtSetArg(cb[0], DwtNlabel, label);					\
    XtSetArg(cb[1], DwtNactivateCallback, sub##Callbacks);		\
    if (app_resources.useGadgets)					\
	XtManageChild(DwtPushButtonGadgetCreate(tempPulldown, #name, cb, 2));\
    else								\
	XtManageChild(DwtPushButtonCreate(tempPulldown, #name, cb, 2));	\
    XtFree(label);

#endif
#else /* STDC */
#ifdef MOTIF
#define menuBtn(type, lbl, text)					\
    label = XmStringLtoRCreate(text, XmSTRING_DEFAULT_CHARSET);		\
    XtSetArg(cb[0], XmNlabelString, label);				\
    XtSetArg(cb[1], XmNactivateCallback, type/**/lbl/**/Callbacks);	\
    if (app_resources.useGadgets)					\
	XtManageChild(XmCreatePushButtonGadget(type/**/Pulldowns[i], "lbl", cb, 2));\
    else								\
	XtManageChild(XmCreatePushButton(type/**/Pulldowns[i], "lbl", cb, 2));\
    XmStringFree(label)

#define menuPulldown(type, lbl)						\
    i++;								\
    type/**/Pulldowns[i] = XmCreatePulldownMenu(MenuBar,"lbl", NULL, 0);\
    XtManageChild(XmCreateSeparator(type/**/Pulldowns[i], "sep", NULL, 0));\
    label = XmStringLtoRCreate("lbl", XmSTRING_DEFAULT_CHARSET);	\
    XtSetArg(cb[0], XmNlabelString, label);				\
    XtSetArg(cb[1], XmNsubMenuId, type/**/Pulldowns[i]);		\
    if (app_resources.useGadgets)					\
	type/**/Menus[i] = XmCreateCascadeButtonGadget(MenuBar, "lbl", cb, 2);\
    else								\
	type/**/Menus[i] = XmCreateCascadeButton(MenuBar, "lbl", cb, 2);\
    XmStringFree(label)

#define subMenuPulldown(type,name,lbl)					\
    tempPulldown = XmCreatePulldownMenu(type/**/Pulldowns[i], "name",NULL,0);\
    label = XmStringLtoRCreate(lbl,XmSTRING_DEFAULT_CHARSET);		\
    XtSetArg(cb[0], XmNlabelString, label);				\
    XtSetArg(cb[1], XmNsubMenuId, tempPulldown);			\
    if (app_resources.useGadgets)					\
	tempMenu = XmCreateCascadeButtonGadget(type/**/Pulldowns[i], "name", cb, 2);\
    else								\
	tempMenu = XmCreateCascadeButton(type/**/Pulldowns[i], "name", cb, 2);\
    XtManageChild(tempMenu);						\
    XmStringFree(label);

#define subMenuBtn(type,sub,name,lbl)					\
    label = XmStringLtoRCreate(lbl,XmSTRING_DEFAULT_CHARSET);		\
    XtSetArg(cb[0], XmNlabelString, label);				\
    XtSetArg(cb[1], XmNactivateCallback, sub/**/Callbacks);		\
    if (app_resources.useGadgets)					\
	XtManageChild(XmCreatePushButtonGadget(tempPulldown, "name", cb, 2));\
    else								\
	XtManageChild(XmCreatePushButton(tempPulldown, "name", cb, 2));	\
    XmStringFree(label);

#else
#define menuBtn(type, lbl, text)					\
    label = DwtLatin1String(text);					\
    XtSetArg(cb[0], DwtNlabel, label);					\
    XtSetArg(cb[1], DwtNactivateCallback, type/**/lbl/**/Callbacks);	\
    if (app_resources.useGadgets)					\
	XtManageChild(DwtPushButtonGadgetCreate(type/**/Pulldowns[i], "lbl", cb, 2));\
    else								\
	XtManageChild(DwtPushButtonCreate(type/**/Pulldowns[i], "lbl", cb, 2));\
    XtFree(label)

#define menuPulldown(type, lbl)						\
    i++;								\
    type/**/Pulldowns[i] = DwtMenu(MenuBar,"lbl", 0, 0, 		\
	DwtMenuPulldown, DwtOrientationVertical, NULL, NULL, NULL);	\
    label = DwtLatin1String("lbl");					\
    XtSetArg(cb[0], DwtNlabel, label);					\
    XtSetArg(cb[1], DwtNsubMenuId, type/**/Pulldowns[i]);		\
    type/**/Menus[i] = DwtPullDownMenuEntryCreate(MenuBar,"lbl", cb, 2);\
    XtFree(label)

#define subMenuPulldown(type,name,lbl)					\
    tempPulldown = DwtMenu(type/**/Pulldowns[i], "name",0,0,		\
	DwtMenuPulldown, DwtOrientationVertical, NULL, NULL, NULL);	\
    label = DwtLatin1String(lbl);					\
    XtSetArg(cb[0], DwtNlabel, label);					\
    XtSetArg(cb[1], DwtNsubMenuId, tempPulldown);			\
    tempMenu = DwtPullDownMenuEntryCreate(type/**/Pulldowns[i], "name", cb, 2);\
    XtManageChild(tempMenu);						\
    XtFree(label);

#define subMenuBtn(type,sub,name,lbl)					\
    label = DwtLatin1String(lbl);					\
    XtSetArg(cb[0], DwtNlabel, label);					\
    XtSetArg(cb[1], DwtNactivateCallback, sub/**/Callbacks);		\
    if (app_resources.useGadgets)					\
	XtManageChild(DwtPushButtonGadgetCreate(tempPulldown, "name", cb, 2));\
    else								\
	XtManageChild(DwtPushButtonCreate(tempPulldown, "name", cb, 2));\
    XtFree(label);

#endif
#endif /* STDC */

#endif /* _menus_h */
