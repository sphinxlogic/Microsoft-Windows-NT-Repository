static char *RcsId = "$Id: helpdialog.c,v 1.8 1993/03/02 23:05:41 rfs Exp $";

/*
 * $Log: helpdialog.c,v $
 * Revision 1.8  1993/03/02  23:05:41  rfs
 * *** empty log message ***
 *
 * Revision 1.7  1993/03/02  00:46:34  rfs
 * Updated messages.
 *
 * Revision 1.6  1993/02/26  21:37:32  rfs
 * *** empty log message ***
 *
 * Revision 1.5  1993/02/23  18:22:53  rfs
 * position the popup.
 *
 * Revision 1.4  1993/02/13  23:25:14  rfs
 * Added code for error dialog popup.
 *
 * Revision 1.3  1993/02/13  18:02:40  rfs
 * generalized slightly for other dialogs.
 *
 * Revision 1.2  1993/02/13  14:34:43  rfs
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
#include <Dialog.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>

static char *viHelp = "\
j    | cursor down\n\
k    | cursor up\n\
h    | cursor left\n\
l    | cursor right\n\
(arrow keys also work)\n\
^F   | page forward\n\
^B   | page backward\n\
G    | end-of-file\n\
$    | end-of-line \n\
q    | quit xod\n";

static char *moveHelp = "\
Home | top-of-file\n\
End  | end-of-file\n\
     |\n\
+    | move cursor ahead n bytes\n\
-    | move cursor back n bytes\n\
/    | search for a string\n\
(popup will appear)\n\
n    | next search\n";

static char *otherHelp = "\
Meta-h | hex\n\
Meta-o | octal\n\
Meta-d | decimal\n\
Meta-a | ASCII\n\
Alt-h  | offset format in hex\n\
Alt-o  | offset format in octal\n\
Alt-d  | offset format in decimal\n";

void SetSelected();

static void 
PopdownDialog(Widget w, XtPointer clientData, XtPointer callData) {
	XtPopdown((Widget)clientData);
}

static Widget 
HelpCreateDialog(Widget parent) {
	Widget pushell, mainpane, helppane, helpform, helplabel, okbutton;

	pushell = XtCreatePopupShell("helpDialog", transientShellWidgetClass, 
		parent, NULL, 0);
	/* pane for window title */
	mainpane = XtCreateManagedWidget("paneMain", panedWidgetClass,
		pushell, NULL, 0);
	helpform = XtCreateManagedWidget("form", formWidgetClass,
		mainpane, NULL, 0);
	helplabel = XtCreateManagedWidget("labelHelp", labelWidgetClass,
		helpform, NULL, 0);
	XtVaSetValues(helplabel, XtNlabel, "Xod Quick Help", NULL);
	/* pane for first help block */
	helppane = XtCreateManagedWidget("pane", panedWidgetClass,
		mainpane, NULL, 0);
	helpform = XtCreateManagedWidget("form", formWidgetClass,
		helppane, NULL, 0);
	helplabel = XtCreateManagedWidget("labelHelp", labelWidgetClass,
		helpform, NULL, 0);
	XtVaSetValues(helplabel, XtNlabel, viHelp, NULL);
	helpform = XtCreateManagedWidget("form", formWidgetClass,
		helppane, NULL, 0);
	helplabel = XtCreateManagedWidget("labelHelp", labelWidgetClass,
		helpform, NULL, 0);
	XtVaSetValues(helplabel, XtNlabel, moveHelp, NULL);
	/* form for second help block */
	helpform = XtCreateManagedWidget("form", formWidgetClass,
		mainpane, NULL, 0);
	helplabel = XtCreateManagedWidget("labelHelp", labelWidgetClass,
		helpform, NULL, 0);
	XtVaSetValues(helplabel, XtNlabel, otherHelp, NULL);
	/* form for the OK button */
	helpform = XtCreateManagedWidget("form", formWidgetClass,
		mainpane, NULL, 0);
	okbutton = XtCreateManagedWidget("helpOK", commandWidgetClass,
		helpform, NULL, 0);
	XtVaSetValues(okbutton, XtNlabel, "Dismiss", NULL);
	XtAddCallback(okbutton, XtNcallback, PopdownDialog, 
		(XtPointer)pushell);
	return pushell;
}

Widget
CreateHelpDialog(Widget parent) {
	Widget w;
	w = HelpCreateDialog(parent);
	return w;
}

PopupHelpDialog(void) {
	XtPopup(HelpDialog, XtGrabNone);
	PositionPopup(GetTopLevel(), HelpDialog);
}

Widget 
ErrorCreateDialog(Widget parent, char *bits, int width, int height) {
	Widget pushell, mainpane, errlabel, okbutton;
	Pixmap bm;

	pushell = XtCreatePopupShell("errorDialog", transientShellWidgetClass, 
		parent, NULL, 0);
	if (bits) {
		bm = XCreateBitmapFromData(XtDisplay(pushell), 
			RootWindowOfScreen(XtScreen(pushell)), bits, width, height);
	}
	/* pane for window title */
	mainpane = XtCreateManagedWidget("paneMain", panedWidgetClass,
		pushell, NULL, 0);
	errlabel = XtCreateManagedWidget("labelError", labelWidgetClass,
		mainpane, NULL, 0);
	XtVaSetValues(errlabel, XtNleftBitmap, bm, NULL);
	okbutton = XtCreateManagedWidget("errorOK", commandWidgetClass,
		mainpane, NULL, 0);
	XtVaSetValues(okbutton, XtNlabel, "OK", NULL);
	XtAddCallback(okbutton, XtNcallback, PopdownDialog, 
		(XtPointer)pushell);
	AddWidgetAssoc(pushell, errlabel);
	return pushell;
}

void
PopupErrorDialog(Widget w, char *msg) {
	Widget label;

	label = (Widget)GetWidgetAssoc(w);
	XtVaSetValues(label, XtNlabel, msg, NULL);
	PositionPopup(GetTopLevel(), w);
	XtPopup(w, XtGrabExclusive);
}
