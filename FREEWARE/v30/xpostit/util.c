/* This program was adopted from David A Curry. It has gone through major
   changes in code, functionality and appearance, and may not resemble the 
   original code. Folloowing is the comment from the original program.
*/


/*
 * util.c - utility routines.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log$
 */

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   to work with VMS DECwindows environment. Look for ifndef VMS and
   ifdef VMS to see changes.
*/

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   To remove Athena widget set code. This version now only works with
   DECwindows toolkit widgets. It should work on both VMS and Ultrix
   although I haven't tried to even compile on Ultrix. You can no
   longer search for ifdef VMS to look for changes from original
   version.
*/

#include <decw$include/DwtWidget>
#include <stdio.h>
#include "xpostit.h"

/*
 * ByeBye - clean up and exit.
 */
void
ByeBye()
{
	/*
	 * If saving notes is on, save all notes.
	 */
	if (app_res.save_notes)
		SaveAllNotes();

	XtUnmapWidget(toplevel);
	XCloseDisplay(display);

	exit(0);
}

/*
 * SetNoteDir - determine the path to the note directory.
 */
void
SetNoteDir()
{
	char *getenv();
	char *home, *s;
	struct passwd *pwd;
	char fname[MAXPATHLEN];

#ifndef VMS
	/*
	 * If it's an absolute path name,
	 * we're done.
	 */
	if (app_res.note_dir[0] == '/')
		return;

	/*
	 * Find the user's home directory.
	 */
	if ((home = getenv("HOME")) == NULL) {
		if ((pwd = getpwuid(getuid())) == NULL) {
			fprintf(stderr, "xpostit: who are you?\n");
			exit(1);
		}

		home = pwd->pw_dir;
	}

	/*
	 * Save the path.
	 */
	sprintf(fname, "%s/%s", home, app_res.note_dir);
#else
	if(app_res.note_dir[0] != '\0')
	    return;

	if ((home = getenv("POSTIT$HOME")) == NULL) {
		home = "SYS$DISK:[]";
	}

	strcpy(fname, home);
	/*
	 * Save the path.
	 */
#endif

	s = SafeAlloc(strlen(fname) + 1);
	app_res.note_dir = s;
	strcpy(s, fname);
}

/*
 * MakeFname - make a file name from a note index number.
 */
char *
MakeFname(index)
register int index;
{
	char *s;
	char fname[MAXPATHLEN];
#ifndef VMS
	sprintf(fname, "%s/%s%d", app_res.note_dir, PostItNoteFname, index);
#else
	sprintf(fname, "%s%s%d", app_res.note_dir, PostItNoteFname, index);
#endif
	s = SafeAlloc(strlen(fname) + 1);
	strcpy(s, fname);
	return(s);
}

/*
 * SafeAlloc - allocate n bytes of memory, exit if we run out.
 */
char *
SafeAlloc(nbytes)
register int nbytes;
{
	char *malloc();
	register char *s;

	if ((s = malloc(nbytes)) == NULL) {
		fprintf(stderr, "xpostit: out of memory.\n");
		exit(1);
	}

	bzero(s, nbytes);
	return(s);
}



/* GetAscizString - Returns ASCIZ string from the first segment of a
   compound string. Caller must XtFree the returned string.
*/

char *
GetAscizString(cstr, len)

DwtCompString	cstr;
int	    	* len;

{
DwtCompStringContext 	context;
char			* str;
unsigned long 		charset_return;
int 			dirrtol_return;
unsigned long 		lang_return;
DwtRendMask 		rend_return;
int			status;

/* ......................................................................... */

/* For initializing the context. */

status = DwtInitGetSegment (& context, cstr);   

/* If the context was found then copy the comp. string to the ascii string. */

if (status == DwtSuccess)
    {
    status = DwtGetNextSegment (& context,          /* (IN)  context */
                                & str,		    /* (OUT) ascii string */
                                & charset_return,   /* (OUT) not used */
                                & dirrtol_return,   /* (OUT) not used */
                                & lang_return,      /* (OUT) not used */
                                & rend_return);     /* (OUT) not used */
    }

/* If the string was not successfully copied, make it a null string */

if (status != DwtSuccess)
    {
    str = XtMalloc (1);
    str[0] = '\0';
    }

/* Return the length if requested. */

if ( len != 0 )
    * len = strlen(str);

return(str);
}


/* TimeOfEvent - Given an event, returns time. */

Time 
TimeOfEvent ( event )

XEvent		* event;

{
Time		local_time;

/*............................................................................*/

/* Treat null pointer specially. Return CurrentTime. */

if (event == NULL)
    return(CurrentTime);

/* See if this type of event has a time field associated with it. */

switch (event -> type)
    {
    case KeyPress:
	local_time = ((XKeyPressedEvent *) event) -> time;
	break;

    case KeyRelease:
	local_time = ((XKeyReleasedEvent *) event) -> time;
	break;

    case ButtonPress:
	local_time = ((XButtonPressedEvent *) event) -> time;
	break;

    case ButtonRelease:
	local_time = ((XButtonReleasedEvent *) event) -> time;
	break;

    case MotionNotify:
	local_time = ((XPointerMovedEvent *) event) -> time;
	break;

    case EnterNotify:
	local_time = ((XEnterWindowEvent *) event) -> time;
	break;

    case LeaveNotify:
	local_time = ((XLeaveWindowEvent *) event) -> time;
	break;

    case PropertyNotify:
	local_time = ((XPropertyEvent *) event) -> time;
	break;

    case SelectionClear:
	local_time = ((XSelectionClearEvent *) event) -> time;
	break;

    case SelectionRequest:
	local_time = ((XSelectionRequestEvent *) event) -> time;
	break;

    case SelectionNotify:
	local_time = ((XSelectionEvent *) event) -> time;
	break;

    case FocusIn:
    case FocusOut:
    case KeymapNotify:
    case Expose:
    case GraphicsExpose:
    case NoExpose:
    case VisibilityNotify:
    case CreateNotify:
    case DestroyNotify:
    case UnmapNotify:
    case MapNotify:
    case MapRequest:
    case ReparentNotify:
    case ConfigureNotify:
    case ConfigureRequest:
    case GravityNotify:
    case ResizeRequest:
    case CirculateNotify:
    case CirculateRequest:
    case ColormapNotify:
    case ClientMessage:
    case MappingNotify:
	local_time = CurrentTime;
	break;
    }

/* Return the time */

return(local_time);
}

