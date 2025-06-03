
/* This program was adopted from David A Curry. It has gone through major
   changes in code, functionality and appearance, and may not resemble the
   original code. Folloowing is the comment from the original program.
*/


/*
 * menu.c - routines to handle the menu.
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

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   To add functionality of sending and receiveing notes from other people
   running postit.
*/



#include <decw$include/DwtWidget>
#include <stdio.h>
#include "xpostit.h"

Widget	menuwidget;
Widget	listwidget;
Widget	popup_menu, rx_popup_menu;

void		SaveNote();
void		EraseNote();
void		DestroyNote();
void		SendNote();


static String menustrings[] = {
#define MenuCreate_1p5x2	0
	"Create 1.5x2 Note",
#define MenuCreate_3x3		1
	"Create 3x3 Note  ",
#define MenuCreate_3x5		2
	"Create 3x5 Note  ",
#define MenuRaiseAll		3
	"Raise All Notes  ",
#define MenuLowerAll		4
	"Lower All Notes  ",
#define MenuSaveAll		5
	"Save All Notes   ",
#define MenuExit		6
	"Exit             "
};



static void
mk_a_pushbutton (parent, label, callback)	/* build a pushbutton widget */
    Widget parent;				/* the way we want it */
    char *label;
    DwtCallbackPtr callback;
{
    Arg	    arg[1];
    DwtCompString   cs_str = DwtLatin1String(label);

    Widget p = DwtPushButton (
		parent,
		"",
		0, 0,
		cs_str,
		callback,
		NULL);

    XtSetArg(arg[0], DwtNalignment, DwtAlignmentBeginning);
    XtSetValues(p, arg, 1);

    XtFree(cs_str);
    XtManageChild (p);
}





/**********************************************************************
 *
 * from here to the end is all one-time code to build the menus
 * and widgets before we start
 */

void HandleListSelection();

static DwtCallback cb_menu[2] =	/* exit help callback */
{
    { (VoidProc) HandleListSelection, 0},
    NULL
};


static Widget
mk_popup_menu (p)				/* sparse right now */
    Widget p;
{
   Widget w =
	DwtMenu (
	    p, 					/* Parent */
	    "MainMenu", 			/* Name */
	    0, 					/* location is moot because */
	    0, 					/* controlled by parent */
	    DwtMenuPopup,			/* ??? */
	    DwtOrientationVertical, 		/* Vertical orientation */
	    NULL, NULL, NULL);			/* No callbacks */

    menuwidget = w;
    cb_menu[0].tag = MenuCreate_1p5x2;
    mk_a_pushbutton (w, menustrings[cb_menu[0].tag], cb_menu);
    cb_menu[0].tag = MenuCreate_3x3;
    mk_a_pushbutton (w, menustrings[cb_menu[0].tag], cb_menu);
    cb_menu[0].tag = MenuCreate_3x5;
    mk_a_pushbutton (w, menustrings[cb_menu[0].tag], cb_menu);
    cb_menu[0].tag = MenuRaiseAll;
    mk_a_pushbutton (w, menustrings[cb_menu[0].tag], cb_menu);
    cb_menu[0].tag = MenuLowerAll;
    mk_a_pushbutton (w, menustrings[cb_menu[0].tag], cb_menu);
    cb_menu[0].tag = MenuSaveAll;
    mk_a_pushbutton (w, menustrings[cb_menu[0].tag], cb_menu);
    cb_menu[0].tag = MenuExit;
    mk_a_pushbutton (w, menustrings[cb_menu[0].tag], cb_menu);

}

Widget mk_note_popup_menu(p,name)
Widget	p;
char   *name;
{
    Widget popup;

    DwtCallback	save_cb[] = { { SaveNote, NULL}, NULL };
    DwtCallback	erase_cb[] = { { EraseNote, NULL}, NULL };
    DwtCallback	destroy_cb[] = { { DestroyNote, NULL}, NULL };
    DwtCallback	send_cb[] = { { SendNote, NULL}, NULL };

    popup = DwtMenu (
    p,				/* Parent */
    name,			/* Name */
    0, 				/* location is moot because */
    0, 				/* controlled by parent */
    DwtMenuPopup,		/* ??? */
    DwtOrientationVertical, 	/* Vertical orientation */
    NULL, NULL, NULL);		/* No callbacks */

/*
 * Put each button into the shell widget.  The second and subsequent
 * buttons will position themselves relative to the first one;
 * the first one will position itself relative to the edge of
 * the adb.
 */

mk_a_pushbutton(popup, "Save", save_cb);
mk_a_pushbutton(popup, "Erase", erase_cb);
mk_a_pushbutton(popup, "Delete", destroy_cb);
mk_a_pushbutton(popup, "Send...", send_cb);

return popup;

}


void
CreateMenuWidget()
{

mk_popup_menu(plaidwidget);
popup_menu = mk_note_popup_menu(toplevel,"NoteMenu");
rx_popup_menu = mk_note_popup_menu(toplevel,"ReceiveMenu");

}

/*
 * HandleListSelection - callback from list widget to handle a selection.
 */
void
HandleListSelection(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{

	/*
	 * Dispatch the command.
	 */
	switch ((int) client_data) {
	case MenuCreate_1p5x2:
		CreateNewNote(PostItNote_1p5x2);
		break;
	case MenuCreate_3x3:
		CreateNewNote(PostItNote_3x3);
		break;
	case MenuCreate_3x5:
		CreateNewNote(PostItNote_3x5);
		break;
	case MenuRaiseAll:
		RaiseAllNotes();
		break;
	case MenuLowerAll:
		LowerAllNotes();
		break;
	case MenuSaveAll:
		SaveAllNotes();
		break;
	case MenuExit:
		ByeBye();
		break;
	}
}


