/*
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
                %% \___________________________________%% \
                %% |                                   %%  \
                %% |              Measure              %%   \
                %% |          Help.c    c1995          %%    \
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




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	CreateOvHelp		 create Overview help window
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateOvHelp(Widget parent)  /*  parent widget	*/
{
    Widget button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString button_string = NULL;



 /*
  *	Generate message to display.
  */
    sprintf(message, "Xmeasure is a tool which provides X-Y cursor \n\
location information (in pixel units) for the \n\
Root Window OR a user selected window in both \n\
Simple X-Y and Delta X-Y modes. Mode selection \n\
is via the Option Menu located at the bottom \n\
edge of the tool. \n");

    message_string = XmStringCreateLtoR(message, charset);
    button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("XMeasure - Overview", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (button_string) XtFree(button_string);
    XmStringFree(message_string);
    XmStringFree(button_string);
    XmStringFree(genstr_string);
    return (message_box);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	CreateRootHelp		- create Root help window
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateRootHelp(Widget parent)
{
    Widget button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString button_string = NULL;

 /*
  *	Generate message to display.
  */
    sprintf(message, "The Root option in the Option Menu permits users to \n\
obtain pixel unit information referencing the Root window. \n\
At entry the cursor will change to the Xtoolkit Crosshair \n\
pattern. The Xmeasure display will indicate the 'Root Window' \n\
label, its ID, and the display Width/Height information. \n\
   \n\
Pressing MB1 and holding it down will provide continuous \n\
X-Y coordinates relative to the Northwest gravity (0, 0).\n\
MB2 locks the current X-Y cursor pixel coordinates when \n\
released. MB3 exits measure mode.\n");


    message_string = XmStringCreateLtoR(message, charset);
    button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("XMeasure - Root X-Y", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (button_string) XtFree(button_string);
    XmStringFree(message_string);
    XmStringFree(button_string);
    XmStringFree(genstr_string);
    return (message_box);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	CreateSimpleHelp		- create SimpleXY help window
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateSimpleHelp(Widget parent)
{
    Widget button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString button_string = NULL;

 /*
  *	Generate message to display.
  */
    sprintf(message, "The Simple X-Y option is used to select a particular \n\
window to perform pixel location measurements. Once \n\
user selects the Simple X-Y button from the Option Menu, \n\
the Cursor will change to a Fluer pattern, indicating \n\
Xmeasure is ready for the user to select a target \n\
area. Move the pointer into target area and Press MB1.\n\
   \n\
The cursor will then change to the Crosshair pattern.\n\
The Xmeasure window will display the window title, the \n\
window ID, and the Width/Height information. Pressing \n\n\
MB1 and holding it down will provide continuous X-Y \n\
pixel coordinates. MB2 locks the current X-Y coordinates \n\
when released. Pressing MB3 exits the measure mode.\n");


    message_string = XmStringCreateLtoR(message, charset);
    button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("XMeasure - Simple X-Y", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (button_string) XtFree(button_string);
    XmStringFree(message_string);
    XmStringFree(button_string);
    XmStringFree(genstr_string);
    return (message_box);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	CreateDeltaHelp		- create DeltaXY help window
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateDeltaHelp(Widget parent)
{
    Widget button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString button_string = NULL;

 /*
  *	Generate message to display.
  */
    sprintf(message, "The Delta X-Y option is used to select a particular \n\
window to perform delta pixel location measurements. \n\
Once user selects Delta X-Y from the Option Menu, \n\
the cursor will change to a Fluer pattern, indicating \n\
Xmeasure is ready for the user to select a target \n\
window. Move the pointer into the desired window and \n\
press MB1. \n\
    \n\
The cursor then changes to the Crosshair pattern.\n\
The Xmeasure window will display the window title, the \n\
window ID, and the Width/Height information. Pressing \n\n\
MB1 will initialize the Start location. Pressing MB2 \n\
provides the X/Y difference from the Start point (Mark \n\
point). Pressing MB3 will exit the measure mode.\n");


    message_string = XmStringCreateLtoR(message, charset);
    button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("XMeasure - Delta X-Y", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (button_string) XtFree(button_string);
    return (message_box);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	CreateOptionsHelp		- create Options help window
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateOptionsHelp(Widget parent)
{
    Widget button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString button_string = NULL;

 /*
  *	Generate message to display.
  */
    sprintf(message, "The 'Options' MenuBar permits the user to select \n\
the Menu Offset mode applicable to the target window   \n\
and the MB1 set value for the Delta X-Y mode: \n\
    \n\
  Three offset modes are available: \n\
       Motif35 - Sets the Y zero point 35 pixels down \n\
                 from 0,0 to compensate for the MenuBar \n\
       Motif40 - Sets the Y zero point 40 pixels down \n\
                 from 0,0 (the additional 5 pixels are \n\
                 added for  resource borderWidth = 1) \n\
       None    - Zero point is at 0,0 of the selected window \n\
    \n\
  Relative mode provides X-Y values relative to the MB1 \n\
    selection location (Delta Mode) when MB2 is released. \n\
    Absolute mode displays the X-Y value relative to the\n\
    selected window's 0,0 location when MB2 is released. \n");

    message_string = XmStringCreateLtoR(message, charset);
    button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("XMeasure - Options Menu", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (button_string) XtFree(button_string);
    XmStringFree(message_string);
    XmStringFree(button_string);
    XmStringFree(genstr_string);
    return (message_box);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CreateVersion    - create version window
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateVersion(Widget parent)     /*  parent widget  */
{
    Widget button;
    Widget message_box;		/*  Message Dialog   */

    static char message[BUFSIZ];/*  help text  */
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString button_string = NULL;
    char verstext[50];
    char authtext[30] = "    by Lyle W. West   ";


    strcpy(verstext, &verstr);
 /*  Generate message to display. */
    sprintf(message, "%s\n%s", verstext, authtext);

 /* Create the compound strings */
    message_string = XmStringCreateLtoR(message, charset);
    button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("XMeasure - Version", charset);


 /*  Create message box dialog. */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    message_box = XmCreateMessageDialog(parent, "versionbox", args, n);

    button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);
    button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);


 /*  Free strings and return message box. */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (button_string) XtFree(button_string);
    return (message_box);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  HelpOvCB    callback for Overview menu help 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpOvCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateOvHelp(w);
    XtManageChild(message_box);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  HelpRootCB   - callback for Root menu help 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpRootCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateRootHelp(w);
    XtManageChild(message_box);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  HelpSimpleCB   - callback for SimpleXY menu help 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpSimpleCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateSimpleHelp(w);
    XtManageChild(message_box);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  HelpDeltaCB   - callback for DeltaXY menu help 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpDeltaCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateDeltaHelp(w);
    XtManageChild(message_box);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  HelpOffsetCB   - callback for Offset menu help 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpOffsetCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateOptionsHelp(w);
    XtManageChild(message_box);
}


/************************************************************
 * VersionCB      - callback for Version help button
 ************************************************************/
void VersionCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateVersion(w);
    XtManageChild(message_box);
}


