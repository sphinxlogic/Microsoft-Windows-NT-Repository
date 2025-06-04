/* This program was adopted from David A Curry. It has gone through major
   changes in code, functionality and appearance, and may not resemble the
   original code. Folloowing is the comment from the original program.
*/

/*
 * note.c - routines for handling notes.
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
            Robert Zanzerkia (TALLIS::ZANZERKIA) , Advance VAX Development.
        Digital Equipment Corporation
        295 Foster st. Littleton.

   Added a counter in the icon box to show how many notes are created.
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

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   To add functionality of sending and receiveing notes to/from other people
   running postit.
*/

/* Modified by:
	    Jim VanGilder (CLT::VANGILDER), DECwindows Toolkits
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation
    To fix bug in call to XtAddActions on the note text widget -
    Call R4 XtAppAddActions using appcontext created in xpostit
*/

/* Modified by:
	    Dave Porter  (MU::PORTER),  IBM Interconnect
	    Digital Equipment Corporation
	    550 King St, Littleton, MA 01460
    (Re)enable code to restore note position at Xpostit startup.
*/

/* Modified by:
            Simon Graham SMAUG::GRAHAM), T&N IBM Interconnect Group
	    Digital Equipment Corporation
	    550, King St, Littleton, MA 01460

    To add the username and hostname to notes that are sent to other people.
*/

#include <decw$include/DwtWidget>
#include <stdio.h>
#include "xpostit.h"

extern XtAppContext	appcontext;

static PostItNote	*notes = NULL;
static int TotalNotes();

static PostItNote	*FindNote();
static PostItNote	*FindNoteFromWidget();
PostItNote	*AllocNote();

static void		UpdateIcon();
void		SaveNote();
void		EraseNote();
void		DestroyNote();
void		SendNote();
static void		CancelErase();
static void		ConfirmErase();
static void		CancelDestroy();
static void		ConfirmDestroy();
void		MakeNoteWidget();

static	int		current_note_index = 0;

void			SendNoteDialog();

#ifdef VMS

#include		<descrip>
#include		"dir"
#define	VMSDSC		struct dsc$descriptor

/* Macros to initialize VMS descriptors */

#define	INIT_VMSDSC(dsc, len, addr)		\
	((dsc .dsc$b_class) = DSC$K_CLASS_S,	\
	 (dsc .dsc$b_dtype) = DSC$K_DTYPE_T,	\
	 (dsc .dsc$w_length) = (len),		\
	 (dsc .dsc$a_pointer) = (addr),		\
	 (& (dsc) )				\
	)

update_note_text(pn)
PostItNote	* pn;
{
VMSDSC 	dsc;
int	len;

INIT_VMSDSC( dsc, pn->pn_textsize, pn->pn_text);
DWT$S_TEXT_GET_STRING(pn->pn_textwidget, &dsc, &len);
pn->pn_text[len]='\0';
}
#endif

/*
 * CreateNewNote - create a new note of the specified size.
 */
void
CreateNewNote(size)
int size;
{
	static int hpi = 0;
	static int wpi = 0;
	register PostItNote *pn;
	register int hmm, wmm, hpixel, wpixel;

	/*
	 * Find out the number of pixels per inch on the screen.  We
	 * can get the number of pixels, and the size in millimeters.
	 * Then we convert to pixels/inch using the formula
	 *
	 *       2.54 cm     10 mm     pixels     pixels
	 *	--------- x ------- x -------- = --------
	 *	  inch        cm         mm        inch
	 *
	 * The only problem with this is that some servers (like Xsun)
	 * lie about what these numbers really are.
	 */
	if ((hpi == 0) || (wpi == 0)) {
		hpixel = DisplayHeight(display, DefaultScreen(display));
		wpixel = DisplayWidth(display, DefaultScreen(display));

		hmm = DisplayHeightMM(display, DefaultScreen(display));
		wmm = DisplayWidthMM(display, DefaultScreen(display));

		hpi = (int) ((25.4 * hpixel) / (float) hmm + 0.5);
		wpi = (int) ((25.4 * wpixel) / (float) wmm + 0.5);
	}

	/*
	 * Calculate sizes for the note.
	 */
	switch (size) {
	case PostItNote_1p5x2:
		hpixel = 1.5 * hpi;
		wpixel = 2 * wpi;
		break;
	case PostItNote_3x3:
		hpixel = 3 * hpi;
		wpixel = 3 * wpi;
		break;
	case PostItNote_3x5:
		hpixel = 3 * hpi;
		wpixel = 5 * wpi;
		break;
	}

	/*
	 * Allocate a new note structure.
	 */
	pn = AllocNote(NewIndex);

	/*
	 * Set the text window size.
	 */
	pn->pn_textwidth = wpixel;
	pn->pn_textheight = hpixel;

	/*
	 * Make the widget for the note.
	 */
	MakeNoteWidget(pn);

    	UpdateIcon();
}


/*
 * LoadSavedNotes - load in the notes the user has saved.
 */
void
LoadSavedNotes()
{
	DIR *dp;
	FILE *fp;
	char *realloc();
	register PostItNote *pn;
	register struct direct *d;
	char buf[BUFSIZ], fname[MAXPATHLEN];
	int n, len, nlen, shellx, shelly, texth, textw;
	char remoteuser[RemoteUserSize];

	/*
	 * Try to open the directory.
	 */
	if ((dp = opendir(app_res.note_dir)) == NULL)
		return;

	nlen = strlen(PostItNoteFname);

	/*
	 * For each entry...
	 */
	while ((d = readdir(dp)) != NULL) {
		/*
		 * Skip over anything which doesn't match our
		 * file naming scheme.
		 */
		if (strncmp(d->d_name, PostItNoteFname, nlen) != 0)
			continue;

		/*
		 * Make the full path name.
		 */
#ifndef VMS
		sprintf(fname, "%s/%s", app_res.note_dir, d->d_name);
#else
		sprintf(fname, "%s%s", app_res.note_dir, d->d_name);
#endif
		/*
		 * Open the file.
		 */
		if ((fp = fopen(fname, "r")) == NULL)
			continue;

		/*
		 * Look for the magic cookie identifying this as
		 * a Post-It note.
		 */
		if ((fscanf(fp, "%s", buf) == EOF) ||
		    (strcmp(buf, PostItNoteMagic) != 0)) {
			fclose(fp);
			continue;
		}

		/*
		 * Get the note position and size information.
		 */
		fgets(buf, sizeof(buf), fp);

		n = sscanf(buf, "%d %d %d %d %d %s", &shellx, &shelly, &texth,
				&textw, &len, remoteuser);

		if (n < 5) {
		    /*
		     * Bad format; skip it.
		     */
		    fclose(fp);
		    continue;
		}
		else if (n==5)
		    /*
		     * This note was NOT sent from a remote user
		     */
		    remoteuser[0] = '\0';

		/*
		 * Get the index number of this note.
		 */
		n = atoi(&(d->d_name[nlen]));

		/*
		 * Get a note structure.
		 */
		pn = AllocNote(n);

		/*
		 * Set the information.
		 */
		pn->pn_shellx = shellx;
		pn->pn_shelly = shelly;
		strcpy(pn->pn_remoteuser, remoteuser);
		pn->pn_textwidth = textw;
		pn->pn_textheight = texth;
		pn->pn_positionit = True;

		/*
		 * Save the file name.
		 */
		pn->pn_file = SafeAlloc(strlen(fname) + 1);
		strcpy(pn->pn_file, fname);

		/*
		 * If we need a bigger buffer than the default,
		 * get one.
		 */
		if (len >= pn->pn_textsize) {
			n = (len + app_res.buf_size - 1) / app_res.buf_size;
			n = n * app_res.buf_size;

			if ((pn->pn_text = realloc(pn->pn_text, n)) == NULL) {
				fprintf(stderr, "xpostit: out of memory.\n");
				ByeBye();
			}

			pn->pn_textsize = n;
		}

		/*
		 * Read in the text.
		 */
		fread(pn->pn_text, sizeof(char), len, fp);
		fclose(fp);

		/*
		 * Make a widget for this note.
		 */
		MakeNoteWidget(pn);
	}

	closedir(dp);

        /* This call counts the # of notes saved/open and updates the icon
         * to reflect the number.
         */
        UpdateIcon();
}

/*
 * RaiseAllNotes - raise all the notes by raising their shell windows.
 */
void
RaiseAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		XRaiseWindow(display, XtWindow(pn->pn_shellwidget));
}

/*
 * LowerAllNotes - lower all the notes by lowering their shell windows.
 */
void
LowerAllNotes()
{
	register PostItNote *pn;


	for (pn = notes; pn != NULL; pn = pn->pn_next)
		XLowerWindow(display, XtWindow(pn->pn_shellwidget));
}

/*
 * SaveAllNotes - save all the notes.
 */
void
SaveAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		{
		SaveNote(pn->pn_shellwidget, (caddr_t) pn->pn_index, 0);
		}
}

/*
 * MakeNoteWidget - make a widget for a Post-It note.
 */
void
MakeNoteWidget(pn)
PostItNote *pn;
{
	Arg args[20];
	int options;
	Widget	    dummy_popup;
	register int nargs;
	static int newx=30, newy=200;
	char	 buf[BUFSIZ];
	char    *widget_class;

	void TextWidgetHandleMb2();

	char text_translation_table [] =
	    "<Btn2Down>: HandleMb2()";

	XtTranslations text_parsed_translations;

	XtActionsRec text_action_table [] =
	    {
            { "HandleMb2",	(XtActionProc) TextWidgetHandleMb2},
            { NULL, NULL}
	    };

	nargs = 0;

	/*
	 * If the shell window coordinates are valid, use them.
	 */
	if (pn->pn_positionit) {
		SetArg(XtNx, pn->pn_shellx);
		SetArg(XtNy, pn->pn_shelly);
	}
        else {
	    if (newx > 400) {
		newx = 30;
		newy = 200;
	    }
	    SetArg(XtNx, newx);
	    SetArg(XtNy, newy);
	    newx = newx + 30;
	    newy = newy + 25;
	}

	if (strlen(pn->pn_remoteuser) > 0) {
	    sprintf(buf, "Note %d from %s",pn->pn_index,pn->pn_remoteuser);
	    widget_class = "ReceiveNote";
	}
	else {
	    sprintf(buf,"Note %d",pn->pn_index);
	    widget_class = "PostItNote";
	}

	SetArg(DwtNtitle, buf);
	SetArg(DwtNnoResize, False);
	pn->pn_shellwidget = XtCreatePopupShell(widget_class,
			transientShellWidgetClass, toplevel, args, nargs);

	/* Translation table for data entry widget. */

	/*
	 * Create the text window.
	 */
	nargs = 0;
	SetArg(DwtNvalue, pn->pn_text);
	SetArg(XtNtextOptions, options);
	/* SetArg(XtNeditType, XttextEdit); */
	SetArg(DwtNmaxLength, pn->pn_textsize);
	SetArg(XtNwidth, pn->pn_textwidth);
	SetArg(XtNheight, pn->pn_textheight);
	SetArg(DwtNhalfBorder, False);
	SetArg(DwtNwordWrap, True);
	SetArg(DwtNeditable, True);


        if (app_res.scroll_ovf)
	    {
	    SetArg(DwtNautoShowInsertPoint, True);
	    }

	if (app_res.scroll_bar)
	    {
	    SetArg(DwtNscrollVertical, True);
	    }

	pn->pn_textwidget = XtCreateWidget("Note", stextwidgetclass,
			pn->pn_shellwidget, args, nargs);

	text_parsed_translations = XtParseTranslationTable(
			text_translation_table );

#ifdef BAD_CODE	    /* jv, 12-Feb-1991 */
	XtAddActions(
	    text_action_table,
	    sizeof( text_action_table) /
	    sizeof( XtActionsRec ) );
#else
	XtAppAddActions(
	    appcontext,
	    text_action_table,
	    sizeof( text_action_table) / sizeof( XtActionsRec ) );
#endif
    
        XtOverrideTranslations (pn->pn_textwidget, text_parsed_translations);;


	/*
	 * Let the top level shell know all these guys are here.
	 */
	XtManageChild(pn->pn_textwidget);

	/*
	 * Realize the note and pop it up.
	 */
	XtRealizeWidget(pn->pn_shellwidget);
	XtPopup(pn->pn_shellwidget, XtGrabNone);

	/* This popup is a dummy one. It is there just to get MB2 to work
	   right. The real popup_menu is declared in MENU.C now and there
	   is only one (as opposed to one per note).
	*/

	pn->pn_popupwidget = DwtMenu (
		pn->pn_textwidget,		/* Parent */
		"", 				/* Name */
		0, 				/* location is moot because */
		0, 				/* controlled by parent */
		DwtMenuPopup,			/* ??? */
		DwtOrientationVertical, 	/* Vertical orientation */
		NULL, NULL, NULL);		/* No callbacks */
}

void TextWidgetHandleMb2(w, event)
Widget w;
XButtonPressedEvent 	* event;
{
PostItNote *pn;
Widget popup;

pn = FindNoteFromWidget(w);

if (pn==NULL)
    return;

current_note_index = pn->pn_index;

if (strlen(pn->pn_remoteuser)==0)
    popup = popup_menu;
else
    popup = rx_popup_menu;

DwtMenuPosition ( popup, event );
XtManageChild(popup);

}


/*
 * SaveNote - save a note to a file.
 */
void
SaveNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	FILE *fp;
	char *MakeFname();
	XSizeHints sizehints;
	register PostItNote *pn;
	int len, shellx, shelly, texth, textw;
	Arg args[4];
	int nargs;
	char	buf[1024];
	int	search_index;

	if (((int) client_data) != 0)
	    search_index = (int) client_data;
	else
	    search_index = current_note_index;
	/*
	 * Find the note we're saving.
	 */
	if ((pn = FindNote(search_index)) == NULL)
		return;

	/*
	 * If it doesn't have a file name, make one.
	 */
	if (pn->pn_file == NULL)
		pn->pn_file = MakeFname(pn->pn_index);

	/*
	 * Create the file.
	 */
	if ((fp = fopen(pn->pn_file, "w")) == NULL) {
		fprintf(stderr, "xpostit: ");
		perror(pn->pn_file);
		return;
	}

	/*
	 * Get the position of the shell window.
	 */

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

	update_note_text(pn);
	/*
	 * Get the length of the text in the window.
	 */
	len = strlen(pn->pn_text);


	/*
	 * Print out the information needed to recreate the note.
	 */
	fprintf(fp, "%s %d %d %d %d %d %s\n", PostItNoteMagic, shellx, shelly,
			texth, textw, len, pn->pn_remoteuser);

	/*
	 * Write out the text of the note.
	 */
	if (len)
		fwrite(pn->pn_text, sizeof(char), len, fp);

	fclose(fp);
}



/*
 * EraseNote - erase all the text in a note.
 */
void
EraseNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	PostItNote *pn;
	XtCallbackRec cancel[2], confirm[2];
	char	    buf[BUFSIZ];

	bzero(confirm, sizeof(confirm));
	bzero(cancel, sizeof(cancel));

	/*
	 * Find the note we're erasing.
	 */
	if ((pn = FindNote(current_note_index)) == NULL)
		return;

	update_note_text(pn);

	/*
	 * If there's nothing in the window, then there's
	 * no need to erase it.
	 */
	if (strlen(pn->pn_text) == 0)
		return;

	confirm[0].callback = ConfirmErase;
	confirm[0].closure = (caddr_t) pn->pn_index;
	cancel[0].callback = CancelErase;
	cancel[0].closure = (caddr_t) pn->pn_index;

	/*
	 * Get confirmation of what they want to do.
	 */

	sprintf(buf,"Erase note %d.", pn->pn_index);
	ConfirmIt(buf,confirm, cancel);
}

/*
 * DestroyNote - destroy a note.
 */
void
DestroyNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	PostItNote *pn;
	XtCallbackRec cancel[2], confirm[2];
	char	buf[BUFSIZ];

	bzero(confirm, sizeof(confirm));
	bzero(cancel, sizeof(cancel));

	/*
	 * Find the note we're destroying.
	 */
	if ((pn = FindNote(current_note_index)) == NULL)
		return;

	confirm[0].callback = ConfirmDestroy;
	confirm[0].closure = (caddr_t) pn->pn_index;
	cancel[0].callback = CancelDestroy;
	cancel[0].closure = (caddr_t) pn->pn_index;

	/*
	 * Get confirmation of what they want to do.
	 */
	sprintf(buf,"Delete note %d.", pn->pn_index);
	ConfirmIt(buf,confirm, cancel);
}


/*
 * SendNote - send a note to a remote node.
 */
void
SendNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	register PostItNote *pn;

	/*
	 * Find the note we're sending.
	 */

	if ((pn = FindNote(current_note_index)) == NULL)
		return;

	SendNoteDialog(pn);
}


/*
 * ConfirmErase - callback for erase confirmation.
 */
static void
ConfirmErase(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	register PostItNote *pn;

	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();

	/*
	 * Find the note we're erasing.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	bzero(pn->pn_text, pn->pn_textsize);
	DwtSTextSetString(pn->pn_textwidget, pn->pn_text);
}

/*
 * CancelErase - callback for erase cancellation.
 */
static void
CancelErase(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();
}

/*
 * ConfirmDestroy - callback for destroy confirmation.
 */
static void
ConfirmDestroy(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	register PostItNote *pn, *prev;

	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();

	/*
	 * Find the note we're destroying.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * Get rid of the widgets for this note.
	 */
	XtPopdown(pn->pn_shellwidget);
	XtDestroyWidget(pn->pn_shellwidget);

	/*
	 * Get rid of the note structure.
	 */
	if (pn != notes) {
		for (prev = notes; prev->pn_next; prev = prev->pn_next) {
			if (prev->pn_next == pn)
				break;
		}

		prev->pn_next = pn->pn_next;
	}
	else {
		notes = pn->pn_next;
	}

	/*
	 * Get rid of the file.
	 */
	if (pn->pn_file) {
		unlink(pn->pn_file);
		free(pn->pn_file);
	}

	/*
	 * Free the memory we used.
	 */
	free(pn->pn_text);
	free(pn);

        /* This call counts the # of notes saved/open and updates the icon
         * to reflect the number.
         */
        UpdateIcon();
}

/*
 * CancelDestroy - callback for destroy cancellation.
 */
static void
CancelDestroy(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();
}

/*
 * AllocNote - allocate a new note structure and insert in into the
 *	       list of notes.
 */
PostItNote *
AllocNote(index)
int index;
{
	register PostItNote *pn;

	/*
	 * Allocate a structure.
	 */
	if (notes == NULL) {
		notes = (PostItNote *) SafeAlloc(sizeof(PostItNote));
		pn = notes;
	}
	else {
		for (pn = notes; pn->pn_next != NULL; pn = pn->pn_next)
			;

		pn->pn_next = (PostItNote *) SafeAlloc(sizeof(PostItNote));
		pn = pn->pn_next;
	}

	/*
	 * Initialize the note.
	 */
	pn->pn_positionit = False;
	pn->pn_textsize = app_res.buf_size;
	pn->pn_text = SafeAlloc(pn->pn_textsize);

	/*
	 * If the index number was given, use it.  Otherwise,
	 * get a new index number.
	 */
	pn->pn_index = (index == NewIndex ? NoteIndex() : index);

	return(pn);
}

/*
 * FindNote - find the note structure with the given index number.
 */
static PostItNote *
FindNote(index)
register int index;
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next) {
		if (pn->pn_index == index)
			return(pn);
	}

	return(NULL);
}

/*
 * FindNoteFromWidget - find the note structure with the given widget id
 */
static PostItNote *
FindNoteFromWidget(w)
register Widget w;
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next) {
		if (pn->pn_textwidget == w)
			return(pn);
	}

	return(NULL);
}


/*
 * NoteIndex - find the lowest free index number.
 */
static int
NoteIndex()
{
	register int index;
	register PostItNote *pn;

	/*
	 * This is O(n**2), but the list should be small.
	 */
	for (index = 1; ; index++) {
		if ((pn = FindNote(index)) == NULL)
			return(index);
	}
}

/*
 *  This routine treverses the PostItNote list and counts the number of notes.
 */
static int TotalNotes()
{
int i;
PostItNote *pn;

    if(!notes) i = 0;   /* No notes */
    else
        for (i=1,pn = notes; pn->pn_next; pn = pn->pn_next, i++)
            ;

    return i;
}

/*
 * Update the Icon label to reflect number of notes..
 */
void
UpdateIcon()
{
char buf[BUFSIZ];

    /* TotalNotes returns the number of notes */
    sprintf(buf, "XPostit: %d",TotalNotes());
    XSetIconName(display, XtWindow(toplevel), buf);
}


Window WmRootWindow (dpy, root)
    Display *dpy;
    Window root;
{
    Window parent;
    Window *child;
    unsigned int nchildren;
    XWindowAttributes rootatt, childatt;
    if (!XGetWindowAttributes (dpy, root, &rootatt)) {
	fprintf (stderr, "XGetWindowAttributes on root failed.\n");
	exit(1);
    }

    if (XQueryTree (dpy, root, &root, &parent, &child, &nchildren)) {
	int i;
	for (i = 0; i < nchildren; i++) {
	    if (!XGetWindowAttributes (dpy, child[i], &childatt)) {
		fprintf (stderr, "XGetWindowAttributes on child failed.\n");
		exit(1);
	    }
	    if ((rootatt.width == childatt.width) &&
		(rootatt.height == childatt.height))
		return child[i];
	}
	return root;
    } else {
	fprintf (stderr, "XQueryTree failed (window doesn't exist).\n");
	exit(1);
    }
}


Window get_parent(win)

Window win;
{

Window parent;
Window * child;
int    nchildren;
Window root;

if (XQueryTree (display, win, &root, &parent, &child, &nchildren))
    return(parent);
else
    {
    fprintf(stderr,"XQueryTree failed.");
    exit(1);
    }
}
