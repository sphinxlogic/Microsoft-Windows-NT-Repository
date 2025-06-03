#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include "Misc.h"

static String 	aboutText[] = {
#include "About.txt.h"
};

static void callback(Widget w, XtPointer shell, XtPointer junk)
{
	XtDestroyWidget((Widget)shell);
}

void AboutDialog(Widget parent)
{
	Position	x, y;
	Widget		shell;
	Widget		form, title, okButton;
	int		len = 0, i;
	char		*buf;

	for (i = 0; i < XtNumber(aboutText); i++)
		len += strlen(aboutText[i]) + 2;
	buf = (char *)XtCalloc(len, sizeof(char));
	buf[0] = '\0';
	for (i = 0; i < XtNumber(aboutText); i++) {
		strcat(buf, aboutText[i]);
		strcat(buf, "\n");
	}
	
        XtVaGetValues(GetShell(parent), XtNx, &x, XtNy, &y, NULL);

        shell = XtVaCreatePopupShell("about",
                        transientShellWidgetClass, GetShell(parent),
			XtNtitle, "About",
			XtNx, x,
			XtNy, y,
                        NULL);
        form = XtVaCreateManagedWidget("form",
                                formWidgetClass, shell,
                                XtNborderWidth, 0,
                                NULL);

	title = XtVaCreateManagedWidget("title",
				labelWidgetClass, form,
				XtNlabel, buf, 
				XtNborderWidth, 1,
				NULL);

	okButton = XtVaCreateManagedWidget("ok",
			commandWidgetClass, form,
			XtNfromVert, title,
			XtNlabel, "Ok",
			NULL);
	XtAddCallback(okButton, XtNcallback, callback, shell);
	AddDestroyCallback(shell, (void (*)(Widget, void *, XEvent *))callback, NULL);

	XtPopup(shell, XtGrabNone);
}
