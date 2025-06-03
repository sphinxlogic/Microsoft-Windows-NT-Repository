static char *RcsID = "$Id: mkwidgets.c,v 1.7 1993/03/02 01:03:48 rfs Exp $";

/*
 * $Log: mkwidgets.c,v $
 * Revision 1.7  1993/03/02  01:03:48  rfs
 * Make cc stop complaining.
 *
 * Revision 1.6  1993/03/02  00:48:02  rfs
 * Added going to absolute byte routines.
 *
 * Revision 1.5  1993/02/26  21:39:59  rfs
 * *** empty log message ***
 *
 * Revision 1.4  1993/02/14  01:16:04  rfs
 * The byte-offset label now responds to button press.
 *
 * Revision 1.3  1993/02/13  23:26:42  rfs
 * Create ErrorDialog.
 *
 * Revision 1.2  1993/02/13  14:35:39  rfs
 * *** empty log message ***
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
*/

#include <xod.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/SmeBSB.h>
#include <buttons.h>
#include <xod.pt>
#include <frowny.xbm>

static XtActionsRec actions_table[] = {
  {(String)"fix-menu", (XtActionProc)Junk},
  {(String)"set-dialog-button", (XtActionProc)SetDialogButton}
};

static Widget 
    parent_widget,
    formy_widget,
    fileButton_widget, fileMenu_widget,
    editButton_widget, editMenu_widget,
    pane_widget, 
    form_widget;

void 
PopupGotoByte(void) {
	String gotobyte;
	char buffer[MAXLEN];
	long addr;
	int stat, len;

	stat = PopupDialog(GotoByteDialog, "Go to byte:", "", &gotobyte,
		XtGrabExclusive);
	if (stat == Okay) {
		addr = AddressConvert(gotobyte);
		CursorToAbsByte(addr);
	}
}

void 
PopupAdvanceToByte(char *direction) {
	String gotobyte;
	char buffer[MAXLEN], *label;
	long addr, bias;
	int stat, len, line, moff;

	if (*direction == '+')
		label = "Ahead n bytes:";
	else
		label = "Back n bytes:";
	DataLocation(&line, &moff);
	addr = (line*BytesHoriz) + moff + AbsByteOffset();
	stat = PopupDialog(GotoByteDialog, label, "", &gotobyte,
		XtGrabExclusive);
	if (stat == Okay) {
		bias = AddressConvert(gotobyte);
		addr += (*direction == '+') ? bias : -bias;
		if (addr < 0) addr = 0;
		CursorToAbsByte(addr);
	}
}

/* make all the widgets we'll be needing */
void
MakeWidgets(int *argc, char **argv) {
	Widget w, pane, form, top;
	int  i;

	top = XtInitialize(NULL, "Xod", NULL, 0, argc, argv);
	SetTopLevel(top);
	XtAddActions(actions_table, XtNumber(actions_table));

	parent_widget = XtCreateManagedWidget("parent", panedWidgetClass,
		top, NULL, 0);

	formy_widget = XtCreateManagedWidget("formy", formWidgetClass,
		parent_widget, NULL, 0);

	fileMenu_widget = XtCreatePopupShell("menu", 
		simpleMenuWidgetClass, formy_widget, NULL, 0);

	fileButton_widget = XtCreateManagedWidget("File", menuButtonWidgetClass, 
		formy_widget, NULL, 0);

	/* put buttons in the File pulldown menu */
	for (i = 0; i < XtNumber(file_menu); i++) {
		w = XtCreateManagedWidget(file_menu[i].name, 
			(file_menu[i].trap ? smeBSBObjectClass : smeLineObjectClass),
			fileMenu_widget, NULL, 0),
		XtAddCallback(w, XtNcallback, FileMenuCallback, 
			(XtPointer)&file_menu[i].id);
		file_menu[i].widget = w;
	}

	StatusWidget = XtCreateManagedWidget("status", labelWidgetClass, 
		formy_widget, NULL, 0);
	DecodeWidget = XtCreateManagedWidget("decode", labelWidgetClass, 
		formy_widget, NULL, 0);
	ByteOffsetWidget = XtCreateManagedWidget("byteoffset", labelWidgetClass,
		formy_widget, NULL, 0);
	XtVaSetValues(DecodeWidget, XtNwidth, 200, NULL);
	XtAddEventHandler(ByteOffsetWidget, ButtonPressMask, False,
		(XtEventHandler)PopupGotoByte, (XtPointer)NULL);
	pane = XtCreateManagedWidget("pane", panedWidgetClass, parent_widget,
		NULL, 0);
	form = XtCreateManagedWidget("form", formWidgetClass, pane,
		NULL, 0);

	for (i = 0; i < XtNumber(buttonsMode); i++) {
		w = XtCreateManagedWidget(buttonsMode[i].name, 
			(buttonsMode[i].trap ? toggleWidgetClass : commandWidgetClass), 
			form, NULL, 0);
		XtAddCallback(w, XtNcallback, EditModeCB, 
			(XtPointer)&buttonsMode[i]);
		buttonsMode[i].widget = w;
    }
	for (i = 0; i < XtNumber(buttonsDecode); i++) {
		w = XtCreateManagedWidget(buttonsDecode[i].name, 
			(buttonsDecode[i].trap ? toggleWidgetClass : commandWidgetClass), 
			form, NULL, 0);
		XtAddCallback(w, XtNcallback, EditDecodeCB, 
			(XtPointer)&buttonsDecode[i]);
		buttonsDecode[i].widget = w;
    }

	/* holds the base address of each column */
	ByteAddress = XtCreateManagedWidget("byteoff", labelWidgetClass, 
		pane, NULL, 0);
	/* Create CTW widget. */
	ctwWidget = XtCreateManagedWidget("vt100", ctwWidgetClass,
		pane, NULL, 0);
	XtAddCallback(ctwWidget, XtNmouseCallback, (XtCallbackProc)MouseCB,
		 (XtPointer)NULL);

	ErrorDialog = ErrorCreateDialog(top, frowny_bits, frowny_width,
		frowny_height);
}
