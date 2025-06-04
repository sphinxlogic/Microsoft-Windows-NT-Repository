
/* This module added to XPostit by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   To add functionality of sending and receiveing notes to/from other people
   running postit.
*/

/* Modified by:
            Simon Graham SMAUG::GRAHAM), T&N IBM Interconnect Group
	    Digital Equipment Corporation
	    550, King St, Littleton, MA 01460

    To add the username and hostname to notes that are sent to other people.
*/

/* Here is how it works (or is supposed to work!).

   When XPOSTIT comes up, it declares itself as owner of the selection atom
   "XPOSTIT". The selection is used for synchronizing as well as sending
   notes between two XPOSTIT applications.

   Sending XPOSTIT (SendNoteRemote) opens a display on the receiving work
   station and asks for the selection value of atom "XPOSTIT". If the remote
   system is not running XPOSTIT, then the routine SendCheck gets a zero
   length value. If the remote system is running XPOSTIT and it is not
   busy (routine ReceiveStatus), it returns a non-zero selection value with
   format 16 and declares itself busy (receive_in_progress = True). If remote
   system is busy, then it returns selection value with format 16 and length
   zero. Format 16/XA_INTEGER and Format 8/XA_STRING pairs are used to
   distinguish between sending and receiving
   selection owner of atom "XPOSTIT". Getting zero length selection is
   a signal that either remote node is not running XPOSTIT or it is busy.

   Once, SendCheck determines that remote system is ready to receive note, it
   declares itself as the owner of the selection atom "XPOSTIT". This will
   trigger, the selection lost callback (ReceiveStart) on the receiving side.
   This callback will asks the sender for the value of the selection which
   is the note itself. This value is received in ReceiveNote routine which
   creates and saves the new note. Once the receipt is done, the receiver
   declares itself back as owner of the selection to listen for next note!

   Couple of timers are used on each end to reset things if necessary.
   Currently, the timeout value is 10 seconds for each side.
*/

#include <decw$include/DwtWidget>
#include <stdio.h>
#include "xpostit.h"
#include <decw$include/selection>
#include <decw$include/xatom>

Display			* remote_display;
Screen			* remote_screen;
static	Atom		remote_postit_atom;
static	Widget		remote_toplevel;
static	char		* remote_send_text;
static	int		remote_send_text_len;
static	XtIntervalId	send_timeout_id;

PostItNote	*AllocNote();
void		MakeNoteWidget();

char		* index();

extern XtAppContext	appcontext;
XtAppContext		remote_appcontext = 0;
Time			remote_global_time;
char			* remote_display_name = "";
int			send_status;
Boolean			send_in_progress = False;

extern	    Time	global_time;

XtConvertSelectionProc	SendNoteConvert();
XtSelectionCallbackProc	SendCheck();

XtLoseSelectionProc	SendNoteLost();
XtSelectionDoneProc	SendNoteDone();

XtTimerCallbackProc	SendTimeout();

void update_note_text();
void	SendNoteFree();

/* Prepares data for sending and then calls XtGetSelectionValue to see if
   remote system is busy. The callback SendCheck will do rest of the work.
*/

int
SendNoteRemote(pn, name)
PostItNote	* pn;
char		* name;
{

Cardinal    ac = 0;
int	    status;
int	    len, shellx, shelly, texth, textw;
Arg	    args[4];
int	    nargs;
char	    buf[BUFSIZ];
int	    buflen;

if (send_in_progress)
    {
    fprintf(stderr, "%s: Previous send still not completed to : %s\n",
		    PostItNoteClass, remote_display_name);
    return(0);
    }

if (remote_send_text != NULL)
    {
    fprintf(stderr, "%s: Previous send still not completed to : %s\n",
		    PostItNoteClass, remote_display_name);
    free(remote_send_text);
    }

send_in_progress = True;
send_status = 0;

remote_display_name = getenv(name);

if ((remote_display_name == NULL) || (remote_display_name[0] == '\0'))
    remote_display_name = name;

remote_global_time = CurrentTime;
remote_appcontext = XtCreateApplicationContext();

remote_display = XtOpenDisplay(remote_appcontext, remote_display_name, "XPostit", 
			PostItNoteClass, NULL, 0, & ac, NULL);

if (remote_display == NULL)
    {
    send_in_progress = False;
    fprintf(stderr,"%s: Can not open remote display : %s\n",
		    PostItNoteClass, remote_display_name);
    return(0);
    }

remote_toplevel = XtAppCreateShell(NULL, PostItNoteClass,
			applicationShellWidgetClass, remote_display, NULL, 0);

if (remote_toplevel == NULL)
{
    /*
    ** oops; failed to create widget; bomb time!
    */
    send_in_progress = False;
    fprintf(stderr,"Failed to create remote top level application shell\n");
    return(0);
}
    
/* We need to realize the widget so that it can own the selection. However,
   set mappedWhenManaged to False so that the window doesn't show up on the
   receiver's screen. Set width and height to non-zero value.
*/

XtSetArg(args[0], XtNwidth, 10);
XtSetArg(args[1], XtNheight, 10);
XtSetArg(args[2], DwtNmappedWhenManaged, False);
XtSetValues(remote_toplevel, args, 3);
XtRealizeWidget(remote_toplevel);

remote_screen = DefaultScreenOfDisplay(remote_display);

if (remote_screen == NULL)
{
    /*
    ** oops; failed to find remote screen
    */
    send_in_progress = False;
    fprintf(stderr,"Failed to locate remote screen\n");
    return(0);
}

remote_postit_atom = None;
remote_postit_atom = XInternAtom(remote_display, "XPOSTIT", True);

if (remote_postit_atom == None)
    {
    send_in_progress = False;
    fprintf(stderr,"%s: %s not running on display : %s\n",
		    PostItNoteClass, PostItNoteClass, remote_display_name);
    return(0);
    }


update_note_text(pn);
len = strlen(pn->pn_text);

nargs = 0;
shellx = shelly = 0;
SetArg(XtNx, &shellx);
SetArg(XtNy, &shelly);
XtGetValues(pn->pn_shellwidget, args, nargs);

nargs = 0;
textw=texth=0;
SetArg(XtNwidth, &textw);
SetArg(XtNheight, &texth);
XtGetValues(pn->pn_textwidget, args, nargs);


/*
 * Print out the information needed to recreate the note.
 */

sprintf(buf, "%s %d %d %d %d %d %s\n", PostItNoteMagic, shellx, shelly,
		texth, textw, len, OurName);

buflen = strlen(buf);
remote_send_text_len =  len + buflen + 1;
remote_send_text = malloc(remote_send_text_len);
bcopy(buf, remote_send_text, buflen);
bcopy(pn -> pn_text, remote_send_text + buflen, len + 1);

/* A receiver always uses XA_INTEGER and a sender always uses XA_STRING atom.
   This is how SendCheck will distinguish between sender and receiver.
*/

XtGetSelectionValue(remote_toplevel, remote_postit_atom, XA_INTEGER, SendCheck, NULL,
		    remote_global_time);
return(1);
}


/* Selection value received callback: Is it the right one ? If it is, declare
   our self as the owner, so the other side will notice it and ask for
   the value of the selection.
*/   

XtSelectionCallbackProc
SendCheck(widget, closure, selection, type, value, length, format)
Widget	    widget;
Opaque	    closure;
Atom	    * selection, * type;
Opaque	    value;
unsigned long	* length;
int	    * format;
{

int	    status;

if (* type != XA_INTEGER)
    {
    /* This is probably from another sender... */

    fprintf(stderr, "%s: Unexpected value from : %s\n",
		    PostItNoteClass, remote_display_name);
    SendNoteFree();
    return;
    }

if (* length == 0)
    {
    fprintf(stderr, "%s: %s not running or busy at : %s\n",
		    PostItNoteClass, PostItNoteClass, remote_display_name);
    SendNoteFree();
    return;
    }

status = XtOwnSelection(remote_toplevel, remote_postit_atom, remote_global_time, 
		SendNoteConvert, SendNoteLost, SendNoteDone);

if (! status)
    {
    fprintf(stderr, "%s: %s busy at : %s\n",
		    PostItNoteClass, PostItNoteClass, remote_display_name);
    SendNoteFree();
    return;
    };

/* timeout after 10 seconds */

send_timeout_id = XtAppAddTimeOut (appcontext, 10000, SendTimeout, NULL);

return;
}



/* Send timed out. Free every thing. */

XtTimerCallbackProc
SendTimeout(closure)
Opaque	closure;

{

send_timeout_id = 0;

if (remote_send_text != NULL)
    {
    fprintf(stderr, "%s: Send note timed out for : %s, Try again.\n",
		    PostItNoteClass, remote_display_name);
    SendNoteFree();
    }
}




/* The remote node asked for the selection. Send the note data. If the
   target is not XA_STRING, it means it is another sender asking
   for the selection -  return False in that case. */

XtConvertSelectionProc 
SendNoteConvert	(widget, selection, target, type, value, length, format)
Widget	    widget;
Atom	    * selection, * target, * type;
Opaque	    * value;
unsigned long	* length;
int	    * format;
{

if (* selection != remote_postit_atom)
    return(False);

if (* target != XA_STRING)
    return(False);

* type = * target;
* value = remote_send_text;
* length = remote_send_text_len;
* format = 8;
return(True);

}


/* We lost the selection - why ? This should not happen. But just in case,
   free the data.
*/

XtLoseSelectionProc
SendNoteLost(widget, selection)
Widget	    widget;
Atom	    * selection;

{
if (remote_send_text != NULL)
    {
    fprintf(stderr, "%s: Remote node %s did note get the note.\n",
		    PostItNoteClass, remote_display_name);
    SendNoteFree();
    }
}


/* The other side received the selection. Yeh!!! Free data and set the
   success flag.
*/

XtSelectionDoneProc
SendNoteDone(widget, selection, target)
Widget	    widget;
Atom	    * selection, * target;

{

send_status = 1;

fprintf(stdout, "%s: Send note to %s was successfull.\n",
		    PostItNoteClass, remote_display_name);
SendNoteFree();
}


/* Free the context, destroy the widget etc... */

void
SendNoteFree ()
{
int	status;

if (remote_send_text != NULL)
    {
    free(remote_send_text);
    remote_send_text = NULL;
    }

if (remote_toplevel != NULL)
    {
    XtDestroyWidget(remote_toplevel);
    remote_toplevel = NULL;
    }

if (send_timeout_id != 0)
    {
    XtRemoveTimeOut(send_timeout_id);
    send_timeout_id = 0;
    }

if (remote_display != NULL)
    {
    /* XtCloseDisplay(remote_display); */
    remote_display = NULL;
    }

if (remote_appcontext != 0)
    {
    XtDestroyApplicationContext(remote_appcontext);
    remote_appcontext = 0;
    }

send_in_progress = False;

}





static Atom		postit_atom;
static XtIntervalId	receive_timeout_id;
static Boolean		receive_in_progress;
short int		dummy_short;

XtConvertSelectionProc	ReceiveStatus();
XtLoseSelectionProc	ReceiveStart();
XtSelectionDoneProc	ReceiveInProgress();
XtSelectionCallbackProc	ReceiveNote();
XtTimerCallbackProc	ReceiveTimeout();
void			ReceiveReady();



/* Initialize the atom value and declare our self as owner of the selection
   atom XPOSTIT.
*/

ReceiveInit()
{

int	    status;

postit_atom = XInternAtom(XtDisplay(toplevel), "XPOSTIT", False);

if (postit_atom == None)
    {
    fprintf(stderr, "%s: Will not be able to receive remote notes.\n",
		    PostItNoteClass);
    return;
    }

ReceiveReady();

receive_timeout_id = (XtIntervalId) 0;
}


/* A prospective sender asked for the selection value. If we are busy,
   return a zero length value, otherwise return non-zero length and declare
   ourself busy. Also set a timer.
*/

XtConvertSelectionProc
ReceiveStatus(widget, selection, target, type, value, length, format)
Widget	    widget;
Atom	    * selection, * target, * type;
Opaque	    * value;
unsigned long	* length;
int	    * format;
{

* type = XA_INTEGER;
* format = 16;

if (receive_in_progress)
    {
    /* This is a special convention between XPostit clients. This means - I am
       busy. */

    * length = 0;
    }
else
    {
    /* Declare yourself busy and setup a timer to guard against failures. */

    * length = 1;
    * value = & dummy_short;

    receive_in_progress = True;

    receive_timeout_id = XtAppAddTimeOut (appcontext, 1000, ReceiveTimeout, NULL);
    }

return(True);
}




/* We lost the selection. A sender must be the owner now. Ask for the selection
   value. When we get it, make a postit note out of it - in the routine
   ReceiveNote.
*/

XtLoseSelectionProc
ReceiveStart(widget, selection)
Widget	    widget;
Atom	    * selection;

{

if (* selection != postit_atom)
    return;

receive_in_progress = True;

/* Just to be on the safe side, turn timer on if necessary. */

#if (0)
if (receive_timeout_id == 0)
    receive_timeout_id = XtAppAddTimeOut (appcontext, 1000, ReceiveTimeout, NULL);
#endif

XtGetSelectionValue(toplevel, postit_atom, XA_STRING, ReceiveNote, NULL,
		    CurrentTime);  /* global_time); why not ? */

}


/* We timed out while receiving a note. Oh well, make ourself ready for the
   next one!
*/

XtTimerCallbackProc
ReceiveTimeout(closure)
Opaque	closure;

{
receive_timeout_id = (XtIntervalId) 0;
ReceiveReady();
}


/* We got the selection value, make a note out of it and save it. */

XtSelectionCallbackProc
ReceiveNote(widget, closure, selection, type, value,
				    length, format)
Widget	    widget;
Opaque	    closure;
Atom	    * selection, * type;
Opaque	    value;
unsigned long	* length;
int	    * format;
{

PostItNote  * pn;
int	    status;
int	    n, len, nlen, shellx, shelly, texth, textw;
char	    * text_ptr;
int	    text_len;
char	    buf[BUFSIZ];

if ((* type != XA_STRING) || (* format != 8))
    {
    fprintf(stderr, "%s: Selection not from a postit client.\n",
		    PostItNoteClass);
    ReceiveReady();
    return;
    }

pn = AllocNote(NewIndex);

if ((sscanf(value, "%s ", buf) == EOF) || (strcmp(buf, PostItNoteMagic) != 0))
    {
    fprintf(stderr, "%s: Selection is not a note.\n",
		    PostItNoteClass);
    ReceiveReady();
    return;
    }

text_ptr = index(value, ' ');

if (text_ptr == NULL)
    {
    fprintf(stderr, "%s: Selection is not a note.\n",
		    PostItNoteClass);
    ReceiveReady();
    return;
    }

text_ptr++;

n = sscanf(text_ptr, "%d %d %d %d %d %s", &shellx, &shelly, &texth,
				&textw, &len, pn->pn_remoteuser);

if (n < 5)
    {
    fprintf(stderr, "%s: Selection is not a note.\n",
		    PostItNoteClass);
    ReceiveReady();
    return;
    }
else if (n==5)
    pn->pn_remoteuser[0] = '\0';

text_ptr = index(value,'\n');

if (text_ptr == NULL)
    text_ptr = "";

pn->pn_shellx = shellx;
pn->pn_shelly = shelly;
pn->pn_textwidth = textw;
pn->pn_textheight = texth;
pn->pn_positionit = True;

text_len = strlen(text_ptr);

if (text_len >= pn->pn_textsize) 
    {
    n = ((* length) + app_res.buf_size - 1) / app_res.buf_size;

    n = n * app_res.buf_size;

    if ((pn->pn_text = realloc(pn->pn_text, n)) == NULL) 
	{
	fprintf(stderr, "%s: out of memory.\n",
		    PostItNoteClass);
	ByeBye();
	}

    pn->pn_textsize = n;
    }

/*
 * Read in the text.
 */

bcopy(text_ptr, pn->pn_text, text_len + 1);

MakeNoteWidget(pn);
SaveNote(pn->pn_shellwidget, (caddr_t) pn->pn_index, 0);

XtFree(value);

/**** We really need some kind of notification here (reverse icon ?) to tell
      user that there is a new note. If we do that we will also have to track
      the icon state - to change the icon back to normal when the state changes.
*****/

UpdateIcon();

ReceiveReady();

return;
}


/* 
    This is necessary because otherwise, toolkit tries to free the data
    from ReceiveStatus routine (dummy_short) which is not virtual memory.
*/

XtSelectionDoneProc
ReceiveInProgress(widget, selection, target)
Widget	    widget;
Atom	    * selection, * target;
{
}



/* Make ourself owner of the selection atom XPOSTIT. */

void
ReceiveReady()
{
int	status;

status = XtOwnSelection(toplevel, postit_atom, CurrentTime, 
		ReceiveStatus, ReceiveStart, ReceiveInProgress);

if (receive_timeout_id != 0)
    {
    XtRemoveTimeOut(receive_timeout_id);
    receive_timeout_id = 0;
    }

if (!status)
    {
    fprintf(stderr, "%s: Will not be able to get remote notes.\n",
		    PostItNoteClass);
    }

receive_in_progress = False;

return;
}
