/*
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
                %% \___________________________________%% \
                %% |                                   %%  \
                %% |              MiniSm               %%   \
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
#include <Xm/MwmUtil.h>

/*
 * External Variables
 */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CreateApHelp
 * Description: Create 'Applications' help window
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateApHelp(Widget parent)
{
    Widget Button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString Button_string = NULL;



 /*
  *	Generate message to display.
  */
    sprintf(message, "\
The Applications box displays a list of files which are defined in  \n\
the default file MSMAPPS.INP. MiniSM first checks for the logical name \n\
DECW$USER_DEFAULTS in the users process table. If that file does not \n\
exist, MiniSm look for MSMAPPS.INP in the DECW$USER_DEFAULTS directory. To \n\
execute an application, move the pointer into the applications box, \n\
move the pointer to the desired application, then press MB1. It will \n\
depend on the speed and load of your server, but you will see the \n\
application name appear in the Task box, then hopefully see the application \n\
appear on the screen. \n");

    message_string = XmStringCreateLtoR(message, charset);
    Button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("MiniSm Applications", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, Button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (Button_string) XtFree(Button_string);
    return (message_box);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CreateDirHelp
 * Description: Create 'Directory' help window
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateDirHelp(Widget parent)
{
    Widget Button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString Button_string = NULL;



 /*
  *	Generate message to display.
  */
    sprintf(message, "\
MiniSm displays a list of applications to be executed on a remote \n\
node. At startup, MiniSm by default checks for the file MSMAPPS.INP \n\
in the DECW$USER_DEFAULTS directory. If the file does not exist, or you wish \n\
to use another file, select the 'Open' item in the 'File' menu. \n\
\n\
Applications can be added or removed by using 'Modify' item in the \n\
'File' menu. Changes to the Application Definitions popup display can \n\
be transferred to the main applications list via the 'Apply' or 'Ok' \n\
buttons. \n\
\n\
Changes to the Application list are saved using the 'Save' item in the 'File' \n\
menu. You will be prompted ONCE if you attempt to exit without saving \n\
changed Application Definitions. \n");

    message_string = XmStringCreateLtoR(message, charset);
    Button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("MiniSm Input File", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, Button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (Button_string) XtFree(Button_string);
    return (message_box);
}




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CreateOvHelp
 * Description: Create 'Overview' help window
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateOvHelp(Widget parent)
{
    Widget Button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString Button_string = NULL;



 /*
  *	Generate message to display.
  */
    sprintf(message, "\
MiniSm displays a list of applications read by default from \n\
file MSMAPPS.INP in your DECW$USER_DEFAULTS directory. \n\
Applications are executed by clicking MB1 on the desired item. \n\
\n\
These files are displayed in the Applications box and when selected \n\
to be executed, are displayed in the Task list box.\n\
If an executing task stops, it will be removed from the Task list within \n\
10 seconds of the time it stops, or can be deleted via 'Stop Task' \n\
\n\
If another application list is desired, use the 'Open' button \n\
in the 'File' menu.");

    message_string = XmStringCreateLtoR(message, charset);
    Button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("MiniSm Overview", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, Button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (Button_string) XtFree(Button_string);
    return (message_box);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CreateTskHelp
 * Description: Create 'Task' help window
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateTskHelp(Widget parent)
{
    Widget Button;
    Widget message_box;		/*  Message Dialog 	*/

    static char message[BUFSIZ];/*  help text	*/
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString Button_string = NULL;

 /*
  *	Generate message to display.
  */
    sprintf(message, "\
The MiniSm displays a list of active tasks in the 'Task' box. They \n\
remain active until deleted. Output is displayed on the root window \n\
and normal icon handling is in effect. If the current quantity of active \n\
tasks exceed 5, a vertical scrollbar appears to permit access to all \n\
active tasks. \n\
To remove a task, place the pointer on the desired task and press MB1. \n\
Then move the pointer to the 'Stop Task' button and press MB1. \n\
\n\
The Tasklist will reflect the change and remove the application. In the \n\
event a task is selected, and the user decides not to stop a task, \n\
simply click MN1 on the selected task, and it will be deselected. Note \n\
the 'Remove Task' button is inoperative until a task item is selected \n\
and will again become inoperative if a task is deselected. \n\
If a task is stopped by some means other than by MiniSm control, \n\
MiniSm will remove that task from the task box within 10 seconds.  \n");


    message_string = XmStringCreateLtoR(message, charset);
    Button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("MiniSm Tasks", charset);


 /*
  *	Create message box dialog.
  */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, Button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    message_box = XmCreateMessageDialog(parent, "helpbox", args, n);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);

 /*
  *	Free strings and return message box.
  */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (Button_string) XtFree(Button_string);
    return (message_box);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CreateVersion
 * Description: Create 'Version' window
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Widget CreateVersion(Widget parent)
{
    Widget Button;
    Widget message_box;		/*  Message Dialog   */

    static char message[BUFSIZ];/*  help text  */
    XmString genstr_string = NULL;
    XmString message_string = NULL;
    XmString Button_string = NULL;
    char verstext[50];
    char authtext[30] = "  Lyle W. West";


    strcpy(verstext, &verstr);
 /*  Generate message to display. */
    sprintf(message, "%s\n%s", verstext,authtext);

 /* Create the compound strings */
    message_string = XmStringCreateLtoR(message, charset);
    Button_string = XmStringCreateLtoR("Close", charset);
    genstr_string = XmStringCreateLtoR("MiniSm Version", charset);


 /*  Create message box dialog. */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, genstr_string); n++;
    XtSetArg(args[n], XmNokLabelString, Button_string); n++;
    XtSetArg(args[n], XmNmessageString, message_string); n++;
    message_box = XmCreateMessageDialog(parent, "versionbox", args, n);

    Button = XmMessageBoxGetChild(message_box, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(message_box, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);


 /*  Free strings and return message box. */
    if (genstr_string) XtFree(genstr_string);
    if (message_string) XtFree(message_string);
    if (Button_string) XtFree(Button_string);
    return (message_box);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: HelpApCB
 * Description: Callback for 'Application' menu help
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpApCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateApHelp(w);
    XtManageChild(message_box);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: HelpDirCB
 * Description: Callback for 'Directory' menu help
 * 
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpDirCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateDirHelp(w);
    XtManageChild(message_box);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: HelpOvCB
 * Description: Callback for 'Overview' menu help
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpOvCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateOvHelp(w);
    XtManageChild(message_box);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: HelpTskCB
 * Description: Callback for 'Task' menu help
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HelpTskCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateTskHelp(w);
    XtManageChild(message_box);
}

