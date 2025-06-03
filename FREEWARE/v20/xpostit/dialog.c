

/* This module added to XPostit by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   To add functionality of sending and receiveing notes to/from other people
   running postit.
*/

/* Modified by:
	    Dave Porter  (MU::PORTER),  IBM Interconnect
	    Digital Equipment Corporation
	    550 King St, Littleton, MA 01460
   "Fix" the code which obtains the send destination; it apparently
   broke under DECwindows V3.
*/

#include <decw$include/DwtWidget>
#include <stdio.h>
#include "xpostit.h"

extern	XtAppContext	remote_appcontext;
extern	Display		* remote_display;
extern	Time		remote_global_time;

extern	XtAppContext	appcontext;
extern	Time		global_time;
Time			TimeOfEvent();

void		DisplayMessage();
void		HideMessage();
static Widget	message_box = NULL;

char		* GetAscizString();
void		GetSendList();

extern	Boolean	send_in_progress;
extern	int	send_status;

static	Widget		send_dialog_widget;
static	Widget		send_button_widget;
PostItNote		* note_to_send;

void	SendUserCancel();

char	* index();

void	SendDialogActivate();



/* Put up the Send dialog. */

void
SendNoteDialog(pn)
PostItNote	* pn;

{

Arg		arg[25];
int		ac;
Position	shellx, shelly;
DwtCallback	activate_cb[] = { { SendDialogActivate, NULL}, NULL };
int		status;
char		buf[BUFSIZ];
DwtCompString	cs_str;

note_to_send = pn;

ac = 0;
XtSetArg(arg[ac], XtNx, & shellx); ac++;
XtSetArg(arg[ac], XtNy, & shelly); ac++;
XtGetValues(pn->pn_shellwidget, arg, ac);

shellx += 30;
shelly += 50;

if (send_dialog_widget == NULL)
    {
    DwtCompString	cs_label = DwtLatin1String("Select From:");
    DwtCompString	cs_sel_label = DwtLatin1String("Send To:");

    ac = 0;
    XtSetArg(arg[ac], DwtNstyle, DwtModeless); ac++;
    XtSetArg(arg[ac], DwtNactivateCallback, activate_cb); ac++;
    XtSetArg(arg[ac], DwtNcancelCallback, activate_cb); ac++;
    XtSetArg(arg[ac], DwtNdefaultPosition, False); ac++;
    XtSetArg(arg[ac], DwtNx, shellx); ac++;
    XtSetArg(arg[ac], DwtNy, shelly); ac++;
    XtSetArg(arg[ac], DwtNlabel, cs_label); ac++;
    XtSetArg(arg[ac], DwtNselectionLabel, cs_sel_label); ac++;

    send_dialog_widget = DwtSelectionCreate(toplevel, "SendDialog", arg, ac);
    
    XtFree(cs_label);
    XtFree(cs_sel_label);

    GetSendList(app_res.send_list, strlen(app_res.send_list));

    cs_label = DwtLatin1String("Send");

    ac = 0;
    XtSetArg(arg[ac], DwtNactivateCallback, activate_cb); ac++;
    XtSetArg(arg[ac], DwtNlabel, cs_label); ac++;
    send_button_widget = 
	DwtPushButtonCreate(send_dialog_widget, "SendButton", arg, ac);

    XtManageChild(send_button_widget);
    }
else
    {
    /* If it is not on the screen, then move it. If it is, then just
       unmanage-manage to so it pops up to front. */

    if (! XtIsManaged(send_dialog_widget))
	{
        XtUnmanageChild(send_dialog_widget);
        XtMoveWidget(XtParent(send_dialog_widget), shellx, shelly);
        }
    else
	{
	/* Just pop it up.... */

	XtUnmanageChild(send_dialog_widget);
	}
    }

if (strlen(pn->pn_remoteuser)==0)
    sprintf(buf,"Send note %d",pn->pn_index);
else
    sprintf(buf,"Send note %d (from %s)",pn->pn_index,pn->pn_remoteuser);
cs_str = DwtLatin1String(buf);

ac = 0;
XtSetArg(arg[ac], DwtNtitle, cs_str); ac++;
XtSetValues(send_dialog_widget, arg, ac);

XtFree(cs_str);

XtManageChild(send_dialog_widget);

}


/* User cancelled the send. */

void
SendUserCancel(w, client_data, cb_data)
Widget		w;
Opaque		client_data;
DwtAnyCallbackStruct	* cb_data;
{
send_status = -1;
send_in_progress = False;
SendNoteFree();
}




/* One of the button in send dialog activated. */

void
SendDialogActivate(w, client_data, cb_data)
Widget		w;
Opaque		client_data;
DwtSelectionCallbackStruct	* cb_data;

{
char		* send_to;
int		len;
DwtCompString	value;
Arg		arg[1];
char		buf[BUFSIZ];

if (cb_data -> reason == DwtCRCancel)
    {
    XtUnmanageChild(w);
    return;
    }

#if (1)
/* This doesn't always work. Sometimes, we get the value from list box
   instead from the text widget. So work around by using an unsupported
   way.
*/
/* On the other hand, under DECwindows V3, the 'unsupported' code 
   doesn't work at all.  This stuff here seems to work in all the 
   cases I've tried.  So, it's back to the supported way.   =dp 12-Feb-1991
*/
XtSetArg(arg[0], DwtNvalue, & value);
XtGetValues(send_dialog_widget, arg, 1);
send_to = GetAscizString(value, & len);

#else
{
Widget	txtw = ((SelectionWidget) send_dialog_widget) -> selection.selectiontextwid;
send_to = DwtSTextGetString(txtw);
len = strlen(send_to);
}
#endif

if(len <= 0)
    {
    XBell(XtDisplay(w));
    return;
    }

if (SendNoteRemote(note_to_send, send_to))
    {

    /* It is a shame that we have to go into comput bound loop when opening
       multiple displays.
    */

    sprintf(buf, "Sending Note to : %s", send_to);
    DisplayMessage(send_dialog_widget, buf, "Cancel", SendUserCancel);

    /* printf("Into compute loop...\n"); */

    while (send_in_progress)
	{
	XEvent	event;
	Time	time;
	XtInputMask	mask;

	if (appcontext != NULL)
	    {
	    mask = XtAppPending( appcontext);
	    if (mask != 0)
		{
		if (mask & XtIMXEvent)
		    {
		    XtAppNextEvent(appcontext, & event);

		    time = TimeOfEvent(& event);
		    if (time != CurrentTime)
			global_time = time;

		    XtDispatchEvent( & event);
		    }
		else
		    XtAppProcessEvent(appcontext, mask);
		}
	    }

	if (remote_appcontext == NULL) break;

	if (remote_appcontext != NULL)
	    {
	    mask = XtAppPending( remote_appcontext);
	    if (mask != 0)
		{
		if (mask & XtIMXEvent)
		    {
		    XtAppNextEvent(remote_appcontext, & event);

		    time = TimeOfEvent(& event);
		    if (time != CurrentTime)
			remote_global_time = time;

		    XtDispatchEvent( & event);
		    }
		else
		    XtAppProcessEvent(remote_appcontext, mask);
		}
	    }
	}

    /* printf("Out of compute loop...\n"); */

    HideMessage();

    if ((send_status != 1) && (send_status != -1))
	{
	sprintf(buf, "Could not send note to %s. Try again later.\nMake sure %s is running %s.",
		send_to, send_to, PostItNoteClass);
	DisplayMessage(send_dialog_widget, buf, "Acknowledge", NULL);
	}
    }
else
    {
    sprintf(buf, "Could not send note to %s. Try again later.\nMake sure %s is running %s.",
		send_to, send_to, PostItNoteClass);
    DisplayMessage(send_dialog_widget, buf, "Acknowledge", NULL);
    }

if ((w != send_button_widget) && (send_status == 1))
    XtUnmanageChild(w);

if (send_to != NULL)
    XtFree(send_to);

return;
}




/* Display a message */

void
DisplayMessage(p, msg, ok_label, callback)
Widget		p;
char		* msg;
char		* ok_label;
void		* callback();
{
DwtCompString	cs_msg;
DwtCompString	cs_ok_label;
int		argc = 0;
Arg		arg[4];
DwtCallback	cb_struct[] = { { callback, NULL}, NULL };
DwtCallbackPtr	cb_ptr;

cs_msg = DwtLatin1String(msg);
cs_ok_label = (ok_label) ? DwtLatin1String(ok_label) : DwtLatin1String("");
cb_ptr = callback ? cb_struct : NULL;

if (message_box != NULL)
   XtDestroyWidget(message_box);

message_box = DwtMessageBox( (p) ? p : toplevel, "XPostit: Message", 
			True, 0, 0, DwtModal, 
			cs_msg, cs_ok_label, cb_ptr, NULL);

XtManageChild(message_box);

if (cs_msg != NULL)
    XtFree(cs_msg);

if (cs_ok_label != NULL)
    XtFree(cs_ok_label);

return;
}



/* Hide and destroy a message box. */

void 
HideMessage()
{

if (message_box != NULL) 
    XtDestroyWidget(message_box);

message_box = NULL;
}




/* Add items to list box from a comma separated item list. SetValues on
   DwtNitems on the selection widget does'nt work (ACCVIO). So use the
   structure SelectionWidget to get at the list box widget directly.
*/

void
GetSendList(str, len)
char	* str;
int	len;
{

char	* local_str = malloc(len+1);
char	* cptr, * old_cptr, * temp_cptr;
DwtCompString	cs_str;
Widget	lbw = ((SelectionWidget) send_dialog_widget) -> selection.listboxwid;

strncpy(local_str, str, len);
local_str[len]= '\0';

old_cptr = local_str;

cptr = index(old_cptr, ',');

while (cptr != NULL)
    {
    * cptr = '\0';

    cs_str = DwtLatin1String(old_cptr);
    DwtListBoxAddItem(lbw, cs_str, 0);
    XtFree(cs_str);

    temp_cptr = cptr + 1;
    old_cptr = temp_cptr;
    cptr = index(old_cptr, ',');
    }

if (*old_cptr != '\0')
    {
    cs_str = DwtLatin1String(old_cptr);
    DwtListBoxAddItem(lbw, cs_str, 0);
    XtFree(cs_str);
    }

return;
}
