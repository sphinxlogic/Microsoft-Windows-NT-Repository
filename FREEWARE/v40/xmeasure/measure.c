/*
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
                %% \___________________________________%% \
                %% |                                   %%  \
                %% |             XMeasure              %%   \
                %% |          Measure.c  c1995         %%    \
                %% |            Lyle W. West           %%    |
                %% |                                   %%    |
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    |
                \                                        \   |
                 \                                        \  |
                  \                                        \ |
                   \________________________________________\|



     Copyright (C) 1995 Lyle W. West, All Rights Reserved.
     Permission is granted to copy and use this program so long as [1] this
     copyright notice is preserved, and [2] no financial gain is involved
     in copying the program.  This program may not be sold as "shareware"
     or "public domain" software without the express, written permission
     of the author.
 */

#include "includes.h"
#include "xmeasure.icon"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Global Variables
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Boolean Motif_Stat;

Arg args[10];
int n;
int first = TRUE;
int frame;
int indx;
int delta;
int CursorBound = 1;
int InvSelPopAck;
int LastX, LastY;
int OffsetMode = 35;     /* default is Motif35 */
int OffsetVal = 0;
int RelAbsMode = 1;      /* default is Relative */

char runmodes[3][20];
char WinNam[132];
char win_id[16];
char textstr[80];
char verstr[] = "    XMeasure V2.10   \n  28-October-1998  ";
char ContStr[65];    /* Continuous string */
char SetStr[65];     /* Set string */
char MoveStr[65];    /* Move string */

Widget MainForm, Crowcol, Orowcol, Rrowcol, sep1, sep2, cursmode, absrelmode;
Widget offsmode, button, Infostr, Sizestr, X_Ytext, Winstr;
Widget toplevel, main_window, InvSelWarn, CurModeQues, DeltaLbl1, DeltaLbl2;
Widget ConfToglBtnLbl, FreeToglBtnLbl, ConfToglBtn, FreeToglBtn;
Widget RelToglBtnLbl, AbsToglBtnLbl, RelToglBtn, AbsToglBtn;
Widget M35ToglBtnLbl, M40ToglBtnLbl, M35ToglBtn, M40ToglBtn;
Widget M0ToglBtnLbl, M0ToglBtn, cursbtn, absrelbtn, DeltaXY1, DeltaXY2;
Widget OptRowCol, pulldown, option_menu, options[3];
 
Display *display;
Window Measure_Win;
Window Root_win;
XmString CsStr;
XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   external/forward references
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern void CursorModeCB ();
extern void GetCursorMode ();
extern void GetOffsetMode ();
extern void GetRelAbsMode ();
extern void QuitCB ();
extern void OffsetModeCB ();
extern void OptMenuCB ();
extern void RelAbsModeCB ();
extern void SelSimpleCB ();
extern void SelRootCB ();
extern void SelDeltaCB ();
extern void SelModeCB ();
extern void HelpOvCB ();
extern void HelpSimpleCB ();
extern void HelpRootCB ();
extern void HelpDeltaCB ();
extern void HelpOffsetCB ();
extern void VersionCB ();

void Generic_Win();


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   main
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int main(int argc, char **argv)
#ifdef ARF
int argc;
char **argv;
#endif
{
    Widget cascade, menu_pane, menu_bar, subbutton;
    XtAppContext app_context;
    String button_string;
    char iconstr[20];
    char titlestr[80];
    int status;

    sprintf(runmodes[0], "Root Window");
    sprintf(runmodes[1], "Simple X-Y");
    sprintf(runmodes[2], "Delta X-Y");
    sprintf(ContStr, "MB1: Continuous,  MB2: Current,  MB3: Done");
    sprintf(SetStr, "   MB1: Set       MB2: Mark       MB3: Done");
    sprintf(MoveStr, "  Move Fluer to Target Window, Press MB1  ");

 /*
  *  Initialize the toolkit.
  */
    toplevel = XtAppInitialize(&app_context, "XMeasure", NULL, 0, &argc,
        argv, NULL, args, 0);
    display = XtDisplay(toplevel);


 /*
  *  Create main window.
  */
    n = 0;
    XtSetArg(args[n], XmNshowSeparator, True); n++;
    main_window = XmCreateMainWindow(toplevel, "XMeasure", args, n);
    XtManageChild(main_window);


 /*
  *  Set main window title, icon pix, and icon name
  */
    n = 0;
    sprintf(titlestr," Xmeasure ");
    XtSetArg(args[n], XmNtitle, titlestr); n++;
    sprintf(iconstr, "XMeasure");
    XtSetArg(args[n], XmNiconName, iconstr); n++;
    XtSetArg(args[n], XmNiconPixmap,
            XCreateBitmapFromData(XtDisplay(toplevel),
                                  RootWindowOfScreen(XtScreen(toplevel)),
                                  measure_bits,
                                  measure_width,
                                  measure_height)); n++;
    XtSetValues (toplevel, args, n);

 /*
  *  Create menu bar in main window.
  */

    n = 0;
    menu_bar = XmCreateMenuBar(main_window, "menu_bar", args, n);
    XtManageChild(menu_bar);

 /*
  *  Create "File" pulldown menu.
  */

    n = 0;
    menu_pane = XmCreatePulldownMenu(menu_bar, "menu_pane", args, n); n++;

    n = 0;
    button = XmCreatePushButton(menu_pane, " Exit ", args, n); n++;
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, QuitCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, menu_pane); n++;
    cascade = XmCreateCascadeButton(menu_bar, " File ", args, n);
    XtManageChild(cascade);


 /*
  *  Create "Mode" pulldown menu.
  */

    n = 0;
    menu_pane = XmCreatePulldownMenu(menu_bar, "menu_pane", args, n); n++;

#ifdef ARF      /* Cursor Mode currently not used */
    n = 0;
    cursbtn = XmCreatePushButton(menu_pane, " Cursor ", args, n); n++;
    XtManageChild(cursbtn);
    XtAddCallback(cursbtn, XmNactivateCallback, GetCursorMode, NULL);
    XtSetSensitive(cursbtn, False);
#endif

    n = 0;
    button = XmCreatePushButton(menu_pane, " MenuBar Offset ", args, n); n++;
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, GetOffsetMode, NULL);

    n = 0;
    absrelbtn = XmCreatePushButton(menu_pane, " MB2 Abs-Rel ", args, n); n++;
    XtManageChild(absrelbtn);
    XtAddCallback(absrelbtn, XmNactivateCallback, GetRelAbsMode, NULL);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, menu_pane); n++;
    cascade = XmCreateCascadeButton(menu_bar, " Options ", args, n);
    XtManageChild(cascade);


 /*
  *  Create "Help" pulldown menu.
  */

    n = 0;
    menu_pane = XmCreatePulldownMenu(menu_bar, "menu_pane", args, n);

    n = 0;
    button = XmCreatePushButton(menu_pane, " Overview ", args, n);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, HelpOvCB, NULL);

    n = 0;
    button = XmCreatePushButton(menu_pane, " Root X-Y ", args, n);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, HelpRootCB, NULL);

    n = 0;
    button = XmCreatePushButton(menu_pane, " Simple X-Y ", args, n);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, HelpSimpleCB, NULL);

    n = 0;
    button = XmCreatePushButton(menu_pane, " Delta X-Y ", args, n);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, HelpDeltaCB, NULL);

    n = 0;
    button = XmCreatePushButton(menu_pane, " Options ", args, n);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, HelpOffsetCB, NULL);

    n = 0;
    button = XmCreatePushButton(menu_pane, " Version ", args, n);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, VersionCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, menu_pane); n++;
    cascade = XmCreateCascadeButton(menu_bar, "Help", args, n);
    XtManageChild(cascade);

    n = 0;
    XtSetArg(args[n], XmNmenuHelpWidget, cascade); n++;
    XtSetValues(menu_bar, args, n);


/*
 *  Create a form widget as the main window
 */

    n = 0;
    XtSetArg(args[n], XmNnoResize, True); n++;
    XtSetArg(args[n], XmNheight, 130); n++;
    XtSetArg(args[n], XmNwidth, 330); n++;
    MainForm = XmCreateForm(main_window, "MainForm", args, n);

    Motif_Stat = XmIsMotifWMRunning(toplevel);
    Generic_Win();

/* Manage the  widget */
    XtManageChild(MainForm);

/*
 * Realize the toplevel widget, which displays all children
 */
    XtRealizeWidget(toplevel);

/*
 * Go into a loop and wait for input
 */
    XtAppMainLoop(app_context);
}

void CursorSelect(Widget w)
{
/*
 *  Create a row column widget for the 'Cursor Mode' radio buttons 
 */

    n = 0;
    XtSetArg(args[n], XmNx, 44); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNwidth, 180); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    cursmode = XmCreateRowColumn(w, "cmode", args, n);
    XtManageChild(cursmode);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    Crowcol = XmCreateRadioBox(cursmode, "Crowcol", args, n);
    XtManageChild(Crowcol);

    ConfToglBtnLbl = XmStringCreateLtoR("Confine", XmSTRING_DEFAULT_CHARSET);
    FreeToglBtnLbl = XmStringCreateLtoR("Free", XmSTRING_DEFAULT_CHARSET);

/*
 * Create toggle buttons for CursorMode 
 */
    n = 0;      /* first the 'Confine' button */
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, ConfToglBtnLbl); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    ConfToglBtn = XmCreateToggleButton(Crowcol, "ConfToglBtn", args, n);
    XtManageChild(ConfToglBtn);
    XmStringFree(ConfToglBtnLbl);

    n = 0;      /* then the 'Free' button */
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, FreeToglBtnLbl); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    FreeToglBtn = XmCreateToggleButton(Crowcol, "FreeToglBtn", args, n);
    XtManageChild(FreeToglBtn);
    XmStringFree(FreeToglBtnLbl);

    if(CursorBound) {   /* confine button was selected */
        XmToggleButtonSetState(FreeToglBtn, FALSE, FALSE);
        XmToggleButtonSetState(ConfToglBtn, TRUE, FALSE);
    }
    else {      /* free button was selected */
        XmToggleButtonSetState(FreeToglBtn, TRUE, FALSE);
        XmToggleButtonSetState(ConfToglBtn, FALSE, FALSE);
    }

/*
 * Add Callbacks for radio buttons
 */
    XtAddCallback(ConfToglBtn, XmNarmCallback, CursorModeCB, 1);
    XtAddCallback(FreeToglBtn, XmNarmCallback, CursorModeCB, 2);
    XtManageChild(Crowcol);
}

void DeltaSelect(Widget w)
{
/*
 *  Create a row column widget for the 'Delta Mode' radio buttons 
 */

    n = 0;
    XtSetArg(args[n], XmNx, 44); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNwidth, 180); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    absrelmode = XmCreateRowColumn(w, "rmode", args, n);
    XtManageChild(absrelmode);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    Rrowcol = XmCreateRadioBox(absrelmode, "Rrowcol", args, n);
    XtManageChild(Rrowcol);

    RelToglBtnLbl = XmStringCreateLtoR("Relative", XmSTRING_DEFAULT_CHARSET);
    AbsToglBtnLbl = XmStringCreateLtoR("Absolute", XmSTRING_DEFAULT_CHARSET);

/*
 * Create toggle buttons for Delta Mode
 */
    n = 0;      /* first the 'Relative' button */
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, RelToglBtnLbl); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    RelToglBtn = XmCreateToggleButton(Rrowcol, "RelToglBtn", args, n);
    XtManageChild(RelToglBtn);
    XmStringFree(RelToglBtnLbl);

    n = 0;      /* then the 'Absolute' button */
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, AbsToglBtnLbl); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    AbsToglBtn = XmCreateToggleButton(Rrowcol, "AbsToglBtn", args, n);
    XtManageChild(AbsToglBtn);
    XmStringFree(AbsToglBtnLbl);

    if(RelAbsMode) {   /* relative button was selected */
        XmToggleButtonSetState(AbsToglBtn, FALSE, FALSE);
        XmToggleButtonSetState(RelToglBtn, TRUE, FALSE);
    }
    else {             /* absolute button was selected */
        XmToggleButtonSetState(AbsToglBtn, TRUE, FALSE);
        XmToggleButtonSetState(RelToglBtn, FALSE, FALSE);
    }

/*
 * Add Callbacks for radio buttons
 */
    XtAddCallback(RelToglBtn, XmNarmCallback, RelAbsModeCB, 1);
    XtAddCallback(AbsToglBtn, XmNarmCallback, RelAbsModeCB, 2);
    XtManageChild(Rrowcol);
}

void OffsetSelect(Widget w)
{
/*
 *  Create a row column widget for the 'Offset Mode' radio buttons 
 */

    n = 0;
    XtSetArg(args[n], XmNx, 44); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNwidth, 180); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    offsmode = XmCreateRowColumn(w, "omode", args, n);
    XtManageChild(offsmode);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    Orowcol = XmCreateRadioBox(offsmode, "Orowcol", args, n);
    XtManageChild(Orowcol);

    M35ToglBtnLbl = XmStringCreateLtoR("Motif35  ", XmSTRING_DEFAULT_CHARSET);
    M40ToglBtnLbl = XmStringCreateLtoR("Motif40  ", XmSTRING_DEFAULT_CHARSET);
    M0ToglBtnLbl = XmStringCreateLtoR("None     ", XmSTRING_DEFAULT_CHARSET);

/*
 * Create toggle buttons for OffsetMode Mode
 */
    n = 0;      /* first the 'Motif35' button */
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, M35ToglBtnLbl); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    M35ToglBtn = XmCreateToggleButton(Orowcol, "M35ToglBtn", args, n);
    XtManageChild(M35ToglBtn);
    XmStringFree(M35ToglBtnLbl);

    n = 0;      /* then the 'Motif40' button */
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, M40ToglBtnLbl); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    M40ToglBtn = XmCreateToggleButton(Orowcol, "M40ToglBtn", args, n);
    XtManageChild(M40ToglBtn);
    XmStringFree(M40ToglBtnLbl);

    n = 0;      /* last the 'None' button */
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, M0ToglBtnLbl); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    M0ToglBtn = XmCreateToggleButton(Orowcol, "M0ToglBtn", args, n);
    XtManageChild(M0ToglBtn);
    XmStringFree(M0ToglBtnLbl);

    switch(OffsetVal) {
        case 35:            /* Motif35 */
            XmToggleButtonSetState(M35ToglBtn, TRUE, FALSE);
            XmToggleButtonSetState(M40ToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(M0ToglBtn, FALSE, FALSE);
            break;
        case 40:             /* Motif40 */
            XmToggleButtonSetState(M35ToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(M40ToglBtn, TRUE, FALSE);
            XmToggleButtonSetState(M0ToglBtn, FALSE, FALSE);
            break;
        case 0:              /* None */
            XmToggleButtonSetState(M35ToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(M40ToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(M0ToglBtn, TRUE, FALSE);
            break;
    }


/*
 * Add Callbacks for radio buttons
 */
    XtAddCallback(M35ToglBtn, XmNarmCallback, OffsetModeCB, 1);
    XtAddCallback(M40ToglBtn, XmNarmCallback, OffsetModeCB, 2);
    XtAddCallback(M0ToglBtn, XmNarmCallback, OffsetModeCB, 3);
    XtManageChild(Orowcol);
}


void Generic_Win()
{
    n = 0;
    if(!Infostr) sprintf(textstr," Window: ");
    else sprintf(textstr," Window:                                                 ");
    CsStr = XmStringLtoRCreate(textstr, charset);
    XtSetArg(args[n], XmNy, 5 ); n++;
    XtSetArg(args[n], XmNx, 20); n++; 
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    if(!Infostr) {
        Winstr = XmCreateLabel(MainForm, "Winstr", args, n);
        XtManageChild(Winstr);
    }
    else XtSetValues(Winstr, args, n);
    XmStringFree(CsStr);

    n = 0;
    if(!Infostr) sprintf(textstr,"  Size: ");
    else sprintf(textstr,"   Size:                                              ");
    CsStr = XmStringLtoRCreate(textstr, charset);
    XtSetArg(args[n], XmNy, 28); n++;
    XtSetArg(args[n], XmNx, 30); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    if(!Infostr) {
        Sizestr = XmCreateLabel(MainForm, "Sizestr", args, n);
        XtManageChild(Sizestr);
    }
    else XtSetValues(Sizestr, args, n);
    XmStringFree(CsStr);

/*
 * Create a separator widget to separate labels and X-Y box 
 * ( for the Delta option, create a label widget and text widget,
 *   but don't manage them until delta window is selected )
 */

    if(!Infostr) {
        n = 0;
        XtSetArg(args[n], XmNy, 55); n++;
        XtSetArg(args[n], XmNorientation,  XmHORIZONTAL); n++;
        XtSetArg(args[n], XmNshadowThickness, 5); n++;
        XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
        XtSetArg(args[n], XmNwidth, 330); n++;
        sep1 = XmCreateSeparatorGadget(MainForm, "sep1", args, n);
        XtManageChild(sep1);

        n = 0;
        XtSetArg(args[n], XmNy, 75); n++;
        XtSetArg(args[n], XmNx, 80); n++;
        XtSetArg(args[n], XmNcolumns, 25); n++;
        XtSetArg(args[n], XmNshadowThickness, 3); n++;
        XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
        XtSetArg(args[n], XmNeditable, False); n++;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
        X_Ytext = XmCreateText(MainForm, "X_Ytext", args, n);
        if(!delta) XtManageChild(X_Ytext);

        n = 0;
        XtSetArg(args[n], XmNy, 65); n++;
        XtSetArg(args[n], XmNx, 55); n++;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
        DeltaLbl1 = XmCreateLabel(MainForm, "DeltaLbl1", args, n);

        n = 0;
        XtSetArg(args[n], XmNy, 65); n++;
        XtSetArg(args[n], XmNx, 200); n++;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
        DeltaLbl2 = XmCreateLabel(MainForm, "DeltaLbl2", args, n);

        n = 0;
        XtSetArg(args[n], XmNy, 90); n++;
        XtSetArg(args[n], XmNx, 20); n++;
        XtSetArg(args[n], XmNcolumns, 15); n++;
        XtSetArg(args[n], XmNshadowThickness, 3); n++;
        XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
        XtSetArg(args[n], XmNeditable, False); n++;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
        DeltaXY1 = XmCreateText(MainForm, "DeltaXY1", args, n);
        if(delta) XtManageChild(DeltaXY1);

        n = 0;
        XtSetArg(args[n], XmNy, 90); n++;
        XtSetArg(args[n], XmNx, 160); n++;
        XtSetArg(args[n], XmNcolumns, 15); n++;
        XtSetArg(args[n], XmNshadowThickness, 3); n++;
        XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
        XtSetArg(args[n], XmNeditable, False); n++;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
        DeltaXY2 = XmCreateText(MainForm, "DeltaXY2", args, n);
        if(delta) XtManageChild(DeltaXY2);


    /* Create a separator widget to separate action instructions */

        n = 0;
        XtSetArg(args[n], XmNy, 130); n++;
        XtSetArg(args[n], XmNorientation,  XmHORIZONTAL); n++;
        XtSetArg(args[n], XmNshadowThickness, 5); n++;
        XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
        XtSetArg(args[n], XmNwidth, 330); n++;
        sep2 = XmCreateSeparatorGadget(MainForm, "sep2", args, n);
        XtManageChild(sep2);
    }

    /* Create an option menu to start measuring */

    if(!Infostr) {
        n = 0;
        XtSetArg(args[n], XmNwidth, 190); n++;
        XtSetArg(args[n], XmNheight, 40); n++;
        XtSetArg(args[n], XmNx, 40); n++;
        XtSetArg(args[n], XmNy, 140); n++;
        XtSetArg(args[n], XmNresizeWidth, False); n++;
        XtSetArg(args[n], XmNresizeHeight, False); n++;
        XtSetArg(args[n], XmNnumColumns, 2); n++;
        XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
        OptRowCol = XmCreateRowColumn(MainForm, "OptRowCol", args, n);
        XtManageChild(OptRowCol);
        pulldown = (Widget)XmCreatePulldownMenu(OptRowCol, "pulldown", NULL, 0);
        for(indx = 0; indx < 3; indx++) {
            n = 0;
            XtSetArg(args[n], XmNlabelString,
                XmStringCreate(runmodes[indx], charset)); n++;
            options[indx] = XmCreatePushButtonGadget(pulldown,
                runmodes[indx], args, n);
            XtAddCallback(options[indx], XmNactivateCallback, OptMenuCB, indx);
        }
        XtManageChildren(options, indx);
        n = 0;
        XtSetArg(args[n], XmNlabelString,
            XmStringCreate(" Mode:", charset)); n++;
        XtSetArg(args[n], XmNsubMenuId, pulldown); n++;
        XtSetArg(args[n], XmNmenuHistory, options[1]); n++;
        option_menu = XmCreateOptionMenu(OptRowCol, "option_menu", args, n);
        XtManageChild(option_menu);
    }

/* Create a text label saying how to start */

    n = 0;
    CsStr = XmStringLtoRCreate(MoveStr, charset);
    XtSetArg(args[n], XmNy, 145); n++;
    XtSetArg(args[n], XmNx, 10); n++; 
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    if(!Infostr) {
        first = FALSE;
        Infostr = XmCreateLabel(MainForm, "Infostr", args, n);
    }
    else XtSetValues(Infostr, args, n);
#ifdef ARF
    XtManageChild(Infostr);
#endif
    XmStringFree(CsStr);
}
