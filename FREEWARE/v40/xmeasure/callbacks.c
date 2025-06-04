/*
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
            %% \___________________________________%% \
            %% |                                   %%  \
            %% |              XMeasure             %%   \
            %% |        callbacks.c  c1995,1998    %%    \
            %% |            Lyle W. West           %%    |
            %% |                                   %%    |
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    |
            \                                        \   |
             \                                        \  |
              \                                        \ |
               \________________________________________\|



     Copyright (C) 1995,1998 Lyle W. West, All Rights Reserved.
     Permission is granted to copy and use this program so long as [1] this
     copyright notice is preserved, and [2] no financial gain is involved
     in copying the program.  This program may not be sold as "shareware"
     or "public domain" software without the express, written permission
     of the author.

*/

#include "includes.h"
#include "externs.h"
#include <X11/cursorfont.h>

extern void SelDeltaCB ();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *    Select_Window - select a window using the mouse
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Window Select_Window(Display *dpy)
{
    int status;
    int screen = 0;
    Cursor cursor;
    XEvent event;
    Window target_win = None;
    int buttons = 0;

    if(InvSelPopAck) return(FALSE);

    /* Invoke the selection cursor */
    cursor = XCreateFontCursor(dpy, XC_fleur);
    Root_win = RootWindow(dpy,screen);

    /* Grab the pointer */
    status = XGrabPointer(dpy, Root_win, False,
                ButtonPressMask|ButtonReleaseMask, GrabModeSync,
                GrabModeAsync, Root_win, cursor, CurrentTime);
    if (status != GrabSuccess) {
        printf("Can't grab the mouse.");
        exit(1);
    }

    /* Let the user select a window... */
    while ((target_win == None) || (buttons != 0)) { /* allow one more event */
        XAllowEvents(dpy, SyncPointer, CurrentTime);
        XWindowEvent(dpy, Root_win, ButtonPressMask|ButtonReleaseMask, &event);
        switch (event.type) {
            case ButtonPress:
                switch (event.xbutton.button) {
                    case 1:
                        if (target_win == None) {
                            target_win = event.xbutton.subwindow; /* window selected */
                        if (target_win == None) target_win = Root_win;
                        }
                        buttons++;
                        break;
                    case 2:
                    case 3:
                        break;
                }
                break;
            case ButtonRelease:
                if (buttons > 0) /* there may have been some down before we started */
                    buttons--;
                break;
        }
    } 

    XUngrabPointer(dpy, CurrentTime);      /* Done with pointer */
    if(target_win == Root_win) return(FALSE);
    else return(target_win);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *    Format_Window_Id - Display window name and hex id
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Format_Window_Id(Window dpy, Window window_id)
{
    char *win_name;
    int screen = 0;

    sprintf(win_id," (%08X) ", window_id);         /* print id # in hex */
    if (!dpy) {
        sprintf(WinNam," none ");
        return;
    }
    if (!XFetchName(dpy, window_id, &win_name)) { /* Get window name if any */
        sprintf(WinNam," has no name ");
        return;
    }
    if (win_name) {
        sprintf(WinNam," %s ", win_name);
        XFree(win_name);
        if(strlen(WinNam) >= 35) WinNam[35] = '\0';
    } 
    else if (dpy == RootWindow(dpy, screen))
            sprintf(WinNam," the root window ");
        else
            sprintf(WinNam," No Name ");
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *    SelSimpleCB - user selected simple pulldown from option menu
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef ARF
void SelSimpleCB(Widget w, XtPointer client_data, XtPointer call_data)
#else
void SelSimpleCB(Widget w)
#endif
{
    Cursor cursor;
    XEvent event;
    Window This_Win;    /* get window id of this process */
    int CursorMode = 0;
    int status;
    int screen = 0;
    int x, y;
    int focus_state;
    unsigned width, height, border, depth;
    int counter = 0;
    int measuring = FALSE;
    XWindowAttributes win_attributes;

    XGetInputFocus(display, &This_Win, &focus_state);
    if(delta) {
        XtUnmanageChild(DeltaLbl1);
        XtUnmanageChild(DeltaLbl2);
        XtUnmanageChild(DeltaXY1);
        XtUnmanageChild(DeltaXY2);
        XtManageChild(X_Ytext);
        delta = FALSE;
    }
    Measure_Win = 0;
    Generic_Win();
    XmUpdateDisplay(MainForm);

    while(!Measure_Win) {
        Measure_Win = Select_Window(display);
        if(!Measure_Win) {
            InvRootSelected(w,0);
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
    Format_Window_Id(display, Measure_Win);
    if (!XGetWindowAttributes(display, Measure_Win, &win_attributes)) {
        printf("Can't get window attributes");
        exit(1);
    }
    width = win_attributes.width;
    height = win_attributes.height;

    n = 0;
    CsStr = XmStringLtoRCreate(ContStr, charset);        
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

    n = 0;
    sprintf(textstr,"    X: 000,   Y: 000");
    XtSetArg(args[n], XmNvalue, textstr); n++;
    XtSetValues (X_Ytext, args, n);
    XtFree(textstr);
    XmUpdateDisplay(MainForm);

    cursor = XCreateFontCursor(display, XC_crosshair);
    status = XGrabPointer(display, Measure_Win, True,
                Button1MotionMask|ButtonReleaseMask, GrabModeAsync,
                GrabModeAsync, CursorMode, cursor, CurrentTime);
    if (status != GrabSuccess) {
        printf("Can't grab the mouse.");
        exit(1);
    }

    measuring = TRUE;
    while(measuring) {  /* Enter an event loop */
        XAllowEvents(display, SyncPointer, CurrentTime);
            XWindowEvent(display, Measure_Win,
                     Button1MotionMask|ButtonReleaseMask, &event);
            switch (event.type) {
                case ButtonRelease:
                    switch (event.xbutton.button) {
                        case 1:
                        case 2:
                            LastX = event.xbutton.x;
                            if (OffsetMode) LastY = event.xbutton.y - OffsetVal;
                            else LastY = event.xbutton.y;
                            break;
                        case 3:
                            measuring = FALSE;
                            break;
                    }
                    n = 0;
                    sprintf(textstr,"   X: %2d,  Y: %2d\n",
                            LastX, LastY);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (X_Ytext, args, n);
                    XtFree(textstr);
                    XmUpdateDisplay(MainForm);
                    break;

                case MotionNotify:
                    n = 0;
                    sprintf(textstr,"   X: %2d,  Y: %2d\n",
                        event.xbutton.x,
                        OffsetMode ? event.xbutton.y - OffsetVal : event.xbutton.y);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (X_Ytext, args, n);
                    XtFree(textstr);
                    XmUpdateDisplay(MainForm);
                    counter = 5; 
                break;
            }
    }
    XUngrabPointer(display, CurrentTime);      /* Done with pointer */
    XtUnmanageChild(Infostr);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *    SelRootCB - user selected Root pulldown from options menu
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef ARF
void SelRootCB(Widget w, XtPointer client_data, XtPointer call_data)
#else
void SelRootCB(Widget w)
#endif
{
    Cursor cursor;
    XEvent event;
    int status;
    int screen = 0;
    int x, y;
    unsigned width, height, border, depth;
    int counter = 0;
    int measuring = FALSE;
    XWindowAttributes win_attributes;


    if(delta) {
        XtUnmanageChild(DeltaLbl1);
        XtUnmanageChild(DeltaLbl2);
        XtUnmanageChild(DeltaXY1);
        XtUnmanageChild(DeltaXY2);
        XtManageChild(X_Ytext);
        delta = FALSE;
    }
    Generic_Win();
    Root_win = RootWindow(display,screen);
    XGetGeometry (display, Root_win, &Root_win, &x, &y,
                  &width, &height, &border, &depth);
    XSync (display, FALSE);
    if (!XGetWindowAttributes(display, Root_win, &win_attributes)) {
        printf("Can't get window attributes");
        exit(1);
    }

    n = 0;
    CsStr = XmStringLtoRCreate(ContStr, charset);
    XtSetArg(args[n], XmNy, 145); n++;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetValues(Infostr, args, n);
    XmStringFree(CsStr);

    n = 0;
    sprintf(win_id," (%08X) ", Root_win);         /* print id # in hex */
    sprintf(textstr, "Window: Root Window %s", win_id);
    CsStr = XmStringLtoRCreate(textstr, charset);
    XtSetArg(args[n], XmNy, 5); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetValues(Winstr, args, n);
    XmStringFree(CsStr);

    n = 0;
    sprintf(textstr, "  Width: %3d,  Height: %3d", width, height);
    CsStr = XmStringLtoRCreate(textstr, charset);
    XtSetArg(args[n], XmNy, 28); n++;
    XtSetArg(args[n], XmNx, 65); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetValues(Sizestr, args, n);
    XmStringFree(CsStr);

    n = 0;
    sprintf(textstr,"    X: 000,   Y: 000");
    XtSetArg(args[n], XmNvalue, textstr); n++;
    XtSetValues (X_Ytext, args, n);
    XtFree(textstr);
    XmUpdateDisplay(MainForm);

    cursor = XCreateFontCursor(display, XC_crosshair);
    status = XGrabPointer(display, Root_win, True,
                Button1MotionMask|ButtonReleaseMask, GrabModeAsync,
                GrabModeAsync, Root_win, cursor, CurrentTime);
    if (status != GrabSuccess) {
        printf("Can't grab the mouse.");
        exit(1);
    }

    measuring = TRUE;
    while(measuring) {  /* Enter an event loop */
        XAllowEvents(display, SyncPointer, CurrentTime);
            XWindowEvent(display, Root_win,
                     Button1MotionMask|ButtonReleaseMask, &event);
            switch (event.type) {
                case ButtonRelease:
                    switch (event.xbutton.button) {
                        case 1:
                        case 2:
                            LastX = event.xbutton.x_root;
                            LastY = event.xbutton.y_root;
                            break;
                        case 3:
                            measuring = FALSE;
                            break;
                    }
                    n = 0;
                    sprintf(textstr,"   X: %2d,  Y: %2d \n",
                            LastX, LastY);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (X_Ytext, args, n);
                    XtFree(textstr);
                    XmUpdateDisplay(MainForm);
                    break;

                case MotionNotify:
                    n = 0;
                    sprintf(textstr,"   X: %2d,  Y: %2d \n",
                        event.xbutton.x_root, event.xbutton.y_root);
                    XtSetArg(args[n], XmNvalue, textstr); n++;
                    XtSetValues (X_Ytext, args, n);
                    XtFree(textstr);
                    XmUpdateDisplay(MainForm);
                    counter = 5;
                    break;
            }
    }
    XUngrabPointer(display, CurrentTime);      /* Done with pointer */
    XtUnmanageChild(Infostr);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  QuitCB          - callback for quit button
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void QuitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
/*
 *  Terminate 
 */
    exit(0);
}

/************************************************************************
 *              CursorModeCB - Manage Radio Buttons (not used curently)
 ************************************************************************/
void CursorModeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    switch ((int)client_data) {
        case 1:
            CursorBound = TRUE;     /* ConfineTo */
            break;
        case 2:
            CursorBound = FALSE;    /* Free */
            break;
    }
    if(CursorBound) {   /* confine button was selected */
        XmToggleButtonSetState(FreeToglBtn, FALSE, FALSE);
        XmToggleButtonSetState(ConfToglBtn, TRUE, FALSE);
    }
    else {      /* free button was selected */
        XmToggleButtonSetState(FreeToglBtn, TRUE, FALSE);
        XmToggleButtonSetState(ConfToglBtn, FALSE, FALSE);
    }
    XmUpdateDisplay(MainForm);
}


/************************************************************************
 *              RelAbsModeCB - Manage Radio Buttons
 ************************************************************************/
void RelAbsModeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    switch ((int)client_data) {
        case 1:
            RelAbsMode = TRUE;       /* Relative */
            XmToggleButtonSetState(AbsToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(RelToglBtn, TRUE, FALSE);
            break;
        case 2:
            RelAbsMode = FALSE;      /* Absolute */
            XmToggleButtonSetState(AbsToglBtn, TRUE, FALSE);
            XmToggleButtonSetState(RelToglBtn, FALSE, FALSE);
            break;
    }
    XmUpdateDisplay(MainForm);
}


/************************************************************************
 *              OptMenuCB - Manage run OptionMenu
 ************************************************************************/
void OptMenuCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtUnmanageChild(OptRowCol);
    XtManageChild(Infostr);
    switch ((int)client_data) {
        case 0:         /* root window mode */
            SelRootCB(w);
            break;
        case 1:         /* simple x-y mode */
            SelSimpleCB(w);
            break;
        case 2:         /* delta x-y mode */
            SelDeltaCB(w);
            break;
    }
    XtManageChild(OptRowCol);
}

/************************************************************************
 *              OffsetModeCB - Manage Radio Buttons
 ************************************************************************/
void OffsetModeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    switch ((int)client_data) {
        case 1:
            OffsetMode = TRUE;       /* Motif35 */
            OffsetVal = 35;
            XmToggleButtonSetState(M40ToglBtn, TRUE, FALSE);
            XmToggleButtonSetState(M40ToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(M0ToglBtn, FALSE, FALSE);
            break;
        case 2:
            OffsetMode = TRUE;       /* Motif40 */
            OffsetVal = 40;
            XmToggleButtonSetState(M40ToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(M40ToglBtn, TRUE, FALSE);
            XmToggleButtonSetState(M0ToglBtn, FALSE, FALSE);
            break;
        case 3:
            OffsetMode = FALSE;      /* None */
            OffsetVal = 0;
            XmToggleButtonSetState(M40ToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(M40ToglBtn, FALSE, FALSE);
            XmToggleButtonSetState(M0ToglBtn, TRUE, FALSE);
            break;
    }
    XmUpdateDisplay(w);
}



/************************************************************************
 *              CmodePopupCB - unmanage cursor mode popup (not used)
 ************************************************************************/
void CmodePopupCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtUnmanageChild(CurModeQues);
}



/************************************************************************
 *              InvSelPopupCB - user specified illegal win from select
 ************************************************************************/
void InvSelPopupCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    InvSelPopAck = FALSE;
    XtUnmanageChild(InvSelWarn);
}

