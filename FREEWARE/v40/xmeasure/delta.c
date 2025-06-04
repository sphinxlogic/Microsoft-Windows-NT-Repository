/*
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
            %% \___________________________________%% \
            %% |                                   %%  \
            %% |              XMeasure             %%   \
            %% |           delta.c  c1995          %%    \
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
#include "externs.h"
#include <X11/cursorfont.h>

void GetCursorMode ();
void InvWinSelected ();
void InvRootSelected ();
extern InvSelPopupCB ();
extern CmodePopupCB ();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *    SelDeltaCB - user selected Delta pulldown from options menu
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SelDeltaCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Cursor cursor;
    XEvent event;
    Window This_Win;    /* get window id of this process */
    long                  evt_mask = 0;
    long                  InWindow = FALSE;
    XWindowAttributes     Measure_WinInfo;
    int CursorMode = 0;
    int status, update;
    int screen = 0;
    int x, y;
    int StartX, StartY;
    int focus_state;
    unsigned width, height, border, depth;
    int counter = 0;
    int measuring = FALSE;

    StartX = 0;
    StartY = 0;
    LastX  = 0;
    LastY  = 0;
    XGetInputFocus(display, &This_Win, &focus_state);
    Measure_Win = 0;

    delta = TRUE;
    if(RelAbsMode == -1) RelAbsMode = TRUE;
    n = 0;
    Generic_Win();
    XmUpdateDisplay(MainForm);

    while(!Measure_Win) {
        Measure_Win = Select_Window(display);
        if(!Measure_Win) {
            InvRootSelected(w,1);
            return;
        }
    }
    XGetGeometry (display, Measure_Win, &Root_win, &x, &y,
                  &width, &height, &border, &depth);
    Measure_Win = XmuClientWindow (display, Measure_Win);
    XSync (display, FALSE);
    if(Measure_Win == This_Win) {
        InvWinSelected(w);
        return;
    }
    if(CursorBound != 0) CursorMode = Measure_Win;
    InWindow = TRUE;
    Format_Window_Id(display, Measure_Win);
    if (!XGetWindowAttributes(display, Measure_Win, &Measure_WinInfo)) {
        printf("Can't get window attributes");
        exit(1);
    }

    width = Measure_WinInfo.width;
    height = Measure_WinInfo.height;

    n = 0;
    CsStr = XmStringLtoRCreate(SetStr, charset);
    XtSetArg(args[n], XmNy, 145); n++;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetValues(Infostr, args, n);
    XmStringFree(CsStr);

    n = 0;
    sprintf(textstr, "Window: %s", WinNam);
    CsStr = XmStringLtoRCreate(textstr, charset);

    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetValues(Winstr, args, n);
    XmStringFree(CsStr);

    n = 0;
    sprintf(textstr, "  Width: %3d,  Height: %3d", width, height);
    CsStr = XmStringLtoRCreate(textstr, charset);
    XtSetArg(args[n], XmNx, 65); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetValues(Sizestr, args, n);
    XmStringFree(CsStr);
    XtFree(textstr);

    XtUnmanageChild(X_Ytext);

    n = 0;
    sprintf(textstr,"Start");
    CsStr = XmStringLtoRCreate(textstr, charset);

    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetValues (DeltaLbl1, args, n);
    XtManageChild(DeltaLbl1);
    XmStringFree(CsStr);
    XtFree(textstr);

    n = 0;
    if(RelAbsMode) sprintf(textstr,"Offset");
    else sprintf(textstr,"End");
    CsStr = XmStringLtoRCreate(textstr, charset);

    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetValues (DeltaLbl2, args, n);
    XtManageChild(DeltaLbl2);
    XmStringFree(CsStr);
    XtFree(textstr);

    n = 0;
    sprintf(textstr,"  X: 000  Y: 000");
    XtSetArg(args[n], XmNy, 85); n++;
    XtSetArg(args[n], XmNx, 20); n++;
    XtSetArg(args[n], XmNvalue, textstr); n++;
    XtSetValues (DeltaXY1, args, n);
    XtManageChild(DeltaXY1);

    n = 0;
    sprintf(textstr,"  X: 000 Y: 000");
    XtSetArg(args[n], XmNy, 85); n++;
    XtSetArg(args[n], XmNx, 120); n++;
    XtSetArg(args[n], XmNvalue, textstr); n++;
    XtSetValues (DeltaXY2, args, n);
    XtManageChild(DeltaXY2);

    XmUpdateDisplay(MainForm);

    x = Measure_WinInfo.x;
    y = Measure_WinInfo.y;

    evt_mask = (ButtonPressMask | Button2MotionMask | ButtonReleaseMask |
                EnterWindowMask | LeaveWindowMask);

    cursor = XCreateFontCursor(display, XC_crosshair);
    status = XGrabPointer(display, Measure_Win, True,
                          evt_mask, GrabModeAsync, GrabModeAsync,
                          CursorMode, cursor, CurrentTime);
    if (status != GrabSuccess) {
        printf("Can't grab the mouse.");
        exit(1);
    }

    counter = 0;
    measuring = TRUE;
    while(measuring) {  /* Enter an event loop */
        XAllowEvents(display, SyncPointer, CurrentTime);
        XWindowEvent(display, Measure_Win,
                     evt_mask, &event);
        update = FALSE;
        switch (event.type) {
            case ButtonPress:
                if(!InWindow) break;
                switch (event.xbutton.button) {
                    case 1:
                        StartX = event.xbutton.x;
                        if(OffsetMode) StartY = event.xbutton.y - OffsetVal;
                        else StartY = event.xbutton.y;
                        LastX = 0;
                        LastY = 0;
                        update = TRUE;
                        break;
                    case 2:
                    case 3:
                        break;
                }
                if(update) {
                    n = 0;
                    sprintf(textstr,"  X: %03d,  Y: %03d",
                            StartX, StartY);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (DeltaXY1, args, n);
                    XtFree(textstr);

                    n = 0;
                    sprintf(textstr,"  X: %03d,  Y: %03d",
                            LastX, LastY);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (DeltaXY2, args, n);
                    XtFree(textstr);

                    XmUpdateDisplay(MainForm);
                }
                break;
            case ButtonRelease:
                if(!InWindow) break;
                switch (event.xbutton.button) {
                    case 1:
                        break;
                    case 2:
                        if(RelAbsMode) {
                            LastX = event.xbutton.x - StartX;
                            if(OffsetMode) LastY = ((event.xbutton.y - OffsetVal) - StartY);
                            else LastY = (event.xbutton.y - StartY);
                        }
                        else {
                            LastX = event.xbutton.x;
                            if(OffsetMode) LastY = event.xbutton.y - OffsetVal;
                            else LastY = event.xbutton.y;
                        }
                        update = TRUE;
                        break;
                    case 3:
                        measuring = FALSE;
                        break;
                }
                if(update) {
                    n = 0;
                    sprintf(textstr,"  X: %03d,  Y: %03d",
                            StartX, StartY);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (DeltaXY1, args, n);
                    XtFree(textstr);

                    n = 0;
                    sprintf(textstr,"  X: %03d,  Y: %03d",
                            LastX, LastY);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (DeltaXY2, args, n);
                    XtFree(textstr);

                    XmUpdateDisplay(MainForm);
                }
                break;

            case MotionNotify:
                    if(RelAbsMode) {
                        LastX = (event.xbutton.x - StartX);
                        if(OffsetMode) LastY = ((event.xbutton.y - OffsetVal) - StartY);
                        else LastY = (event.xbutton.y - StartY);
                    }
                    else {
                        LastX = event.xbutton.x;
                        if(OffsetMode) LastY = event.xbutton.y - OffsetVal;
                        else LastY = event.xbutton.y;
                    }
                    n = 0;
                    sprintf(textstr,"  X: %03d,  Y: %03d",
                            StartX, StartY);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (DeltaXY1, args, n);
                    XtFree(textstr);

                    n = 0;
                    sprintf(textstr,"  X: %03d,  Y: %03d",
                            LastX, LastY);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (DeltaXY2, args, n);
                    XtFree(textstr);

                    XmUpdateDisplay(MainForm);
                break;

            case EnterNotify:
                    InWindow = TRUE;
                    XChangeActivePointerGrab(display,
                        evt_mask, cursor, CurrentTime);
                    XSetInputFocus(display, Measure_Win,
                        RevertToNone, CurrentTime);
                    /* Pointer entered our window */
                    printf("enter %s\n", WinNam);
                    break;

            case LeaveNotify:
                    InWindow = FALSE;
                    XChangeActivePointerGrab(display, 
                        evt_mask, None, CurrentTime);
                    XSetInputFocus(display, event.xcrossing.subwindow,
                        RevertToNone, CurrentTime);
                    /* Pointer left our window */
                    printf("left  %s\n", WinNam);
                    break;
        }
    }
    XUngrabPointer(display, CurrentTime);      /* Done with pointer */
    XtUnmanageChild(Infostr);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InvWinSelected()          - user selected Measure window
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void InvWinSelected(Widget w)
{
    char    Wtitle[80];
    char    warnstr[132];
    XmString WdTitle;
    XmString warnmsg;

    w = MainForm;
    sprintf(Wtitle, "Invalid Selection");
    sprintf(warnstr, "Cannot select XMeasure window\n%s",
                     "   Select another window\n");
    InvSelPopAck = TRUE;
    n = 0;
    WdTitle = XmStringCreateLtoR(Wtitle, charset);
    warnmsg = XmStringCreateLtoR(warnstr, charset);
    XtSetArg(args[n], XmNdialogTitle, WdTitle); n++;
    XtSetArg(args[n], XmNmessageString, warnmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    InvSelWarn = XmCreateWarningDialog(w, "InvSelWarn", args, n);
    XtAddCallback (InvSelWarn, XmNokCallback, InvSelPopupCB, NULL); 
    XmStringFree(warnmsg);
    XmStringFree(WdTitle);
    XtFree(warnstr);
        /* remove unneeded buttons */
    button = XmMessageBoxGetChild(InvSelWarn, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(InvSelWarn, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    XtManageChild(InvSelWarn);
    XtUnmanageChild(Infostr);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InvRootSelected()          - user selected root window
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void InvRootSelected(Widget w, int src)
{
    char    Wtitle[80];
    char    warnstr[132];
    char    tmpstr[35];
    XmString WdTitle;
    XmString warnmsg;

    w = MainForm;
    sprintf(Wtitle, "Invalid Selection");
    sprintf(warnstr, "Cannot  select  Root  window\n");
    sprintf(tmpstr, "from '%s' environment. Use\n", src ? "Delta" : "Simple");
    strcat(warnstr, tmpstr);
    strcat(warnstr, "'Root' option in Mode popup");
    InvSelPopAck = TRUE;
    n = 0;
    WdTitle = XmStringCreateLtoR(Wtitle, charset);
    warnmsg = XmStringCreateLtoR(warnstr, charset);
    XtSetArg(args[n], XmNdialogTitle, WdTitle); n++;
    XtSetArg(args[n], XmNmessageString, warnmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    InvSelWarn = XmCreateWarningDialog(w, "InvSelWarn", args, n);
    XtAddCallback (InvSelWarn, XmNokCallback, InvSelPopupCB, NULL); 
    XmStringFree(warnmsg);
    XmStringFree(WdTitle);
    XtFree(warnstr);
        /* remove unneeded buttons */
    button = XmMessageBoxGetChild(InvSelWarn, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(InvSelWarn, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    XtManageChild(InvSelWarn);
    XtUnmanageChild(Infostr);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  GetCursorMode()          - select confine_to or free
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void GetCursorMode(Widget w)
{
    char    Qtitle[20];
    char    quesstr[32];
    XmString QdTitle;
    XmString quesmsg;

    sprintf(Qtitle, "Cursor Mode");
    sprintf(quesstr, "  \n thisisastr      \n"); 

    n = 0;
    QdTitle = XmStringCreateLtoR(Qtitle, charset);
    quesmsg = XmStringCreateLtoR(quesstr, charset); 
    XtSetArg(args[n], XmNdialogTitle, QdTitle); n++;
    XtSetArg(args[n], XmNmessageString, quesmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    CurModeQues = XmCreateQuestionDialog(w, "CurModeQues", args, n);
    XtAddCallback (CurModeQues, XmNokCallback, CmodePopupCB, NULL);
    XmStringFree(quesmsg);
    XmStringFree(QdTitle);
    XtFree(quesstr);
        /* remove unneeded buttons */
    button = XmMessageBoxGetChild(CurModeQues, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(CurModeQues, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    CursorSelect(CurModeQues);
    XtManageChild(CurModeQues);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  GetOffsetMode()          - select Motif35, Motif40 or None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void GetOffsetMode(Widget w)
{
    char    Qtitle[20];
    char    quesstr[80];
    XmString QdTitle;
    XmString quesmsg;

    sprintf(quesstr, " Offset \n   Mode");
    sprintf(Qtitle, " ");

    n = 0;
    QdTitle = XmStringCreateLtoR(Qtitle, charset);
    quesmsg = XmStringCreateLtoR(quesstr, charset); 

    XtSetArg(args[n], XmNdialogTitle, QdTitle); n++;
    XtSetArg(args[n], XmNmessageString, quesmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    CurModeQues = XmCreateQuestionDialog(w, "CurModeQues", args, n);
    XtAddCallback (CurModeQues, XmNokCallback, CmodePopupCB, NULL);
    XmStringFree(quesmsg);
    XmStringFree(QdTitle);
    XtFree(quesstr);

        /* remove unneeded buttons */
    button = XmMessageBoxGetChild(CurModeQues, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(CurModeQues, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    OffsetSelect(CurModeQues);
    XtManageChild(CurModeQues);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  GetRelAbsMode()          - select relative or absolute
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void GetRelAbsMode(Widget w)
{
    char    Qtitle[20];
    char    quesstr[32];
    XmString QdTitle;
    XmString quesmsg;

    sprintf(Qtitle, " ");
    sprintf(quesstr, "Rel/Abs\n   Mode");

    n = 0;
    QdTitle = XmStringCreateLtoR(Qtitle, charset);
    quesmsg = XmStringCreateLtoR(quesstr, charset); 
    XtSetArg(args[n], XmNdialogTitle, QdTitle); n++;
    XtSetArg(args[n], XmNmessageString, quesmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    CurModeQues = XmCreateQuestionDialog(w, "CurModeQues", args, n);
    XtAddCallback (CurModeQues, XmNokCallback, CmodePopupCB, NULL);
    XmStringFree(quesmsg);
    XmStringFree(QdTitle);
    XtFree(quesstr);
        /* remove unneeded buttons */
    button = XmMessageBoxGetChild(CurModeQues, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(CurModeQues, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    DeltaSelect(CurModeQues);
    XtManageChild(CurModeQues);
}

