/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>
#include "Misc.h"
#include "Text.h"

typedef struct {
	Widget		shell, widget;
	TextPromptInfo	*prmps;
	XtCallbackProc	okFunc, cancelFunc;
	void		*closure;
} arg_t;

static void cancelCallback(Widget junkW, XtPointer argArg, XtPointer junk)
{
	arg_t		*arg = (arg_t *)argArg;
	XtCallbackProc	uf = arg->cancelFunc;
	Widget		w  = arg->widget;
	void		*c = arg->closure;
	TextPromptInfo	*p = arg->prmps;

	XtDestroyWidget(arg->shell);
	XtFree((XtPointer)arg);

	if (uf != NULL)
		uf(w, c, p);
}

static void okCallback(Widget junkW, XtPointer argArg, XtPointer junk)
{
	arg_t		*arg = (arg_t *)argArg;
	XtCallbackProc	uf = arg->okFunc;
	Widget		w  = arg->widget;
	void		*c = arg->closure;
	TextPromptInfo	*p = arg->prmps;
	int		i;

	for (i = 0; i < p->nprompt; i++)
		XtVaGetValues(p->prompts[i].w, XtNstring, &p->prompts[i].rstr, NULL);

	if (uf != NULL)
		uf(w, c, p);

	XtDestroyWidget(arg->shell);
	XtFree((XtPointer)arg);
}

void TextPrompt(Widget w, char *name, TextPromptInfo *prompt, 
		XtCallbackProc okProc, XtCallbackProc nokProc, void *data)
{
	int		i;
	Position	x, y;
	static String	textAccelerators = "#override\n\
				<Key>Return: set() notify() unset()\n\
				<Key>Linefeed: set() notify() unset()";
	static String	textTranslations = "#override\n\
						 <Key>Return: no-op()\n\
						 <Key>Linefeed: no-op()\n\
						 Ctrl<Key>M: no-op()\n\
						 Ctrl<Key>J: no-op()\n";
	XtTranslations	trans, toglt;
	XtAccelerators	accel;
	Widget		shell, form, title, text, last, label;
	Widget		okButton, cancelButton;
	arg_t		*arg = XtNew(arg_t);

	trans = XtParseTranslationTable(textTranslations);
	accel = XtParseAcceleratorTable(textAccelerators);
	toglt = XtParseTranslationTable("<BtnDown>,<BtnUp>: set() notify()");

	XtVaGetValues(GetShell(w), XtNx, &x, XtNy, &y, NULL);

	shell = XtVaCreatePopupShell(name == NULL ? "popup-dialog" : name,
			transientShellWidgetClass, GetShell(w),
			XtNx, x + 24,
			XtNy, y + 24,
			NULL);

	form = XtVaCreateManagedWidget("popup-dialog-form",
				formWidgetClass, shell,
				XtNborderWidth, 0,
				NULL);

	title = XtVaCreateManagedWidget("title",
				labelWidgetClass, form,
				XtNlabel, prompt->title, 
				XtNborderWidth, 0,
				NULL);

	last = title;
	for (i = 0; i < prompt->nprompt; i++) {
		label = XtVaCreateManagedWidget("label",
				labelWidgetClass, form,
				XtNfromVert, last,
				XtNlabel, prompt->prompts[i].prompt, 
				XtNborderWidth, 0,
				NULL);

		text = XtVaCreateManagedWidget("text",
				asciiTextWidgetClass, form,
				XtNfromHoriz, label,
				XtNfromVert, last,
				XtNeditType, XawtextEdit,
				XtNwrap, XawtextWrapNever,
				XtNresize, XawtextResizeWidth,
				XtNtranslations, trans,
				XtNlength, prompt->prompts[i].len,
				XtNstring, prompt->prompts[i].str,
				XtNinsertPosition, strlen(prompt->prompts[i].str),
				XtNleft, XtChainLeft,
				NULL);
		
		last = text;
		prompt->prompts[i].w = text;
	}

	okButton = XtVaCreateManagedWidget("ok",
				commandWidgetClass, form,
				XtNfromVert, last,
				XtNaccelerators, accel,
				NULL);
	cancelButton = XtVaCreateManagedWidget("cancel",
				commandWidgetClass, form,
				XtNfromVert, last,
				XtNfromHoriz, okButton,
				NULL);


	arg->widget     = w;
	arg->shell      = shell;
	arg->prmps      = prompt;
	arg->closure    = data;
	arg->okFunc     = okProc;
	arg->cancelFunc = nokProc;

	if (prompt->nprompt == 1) {
		XtSetKeyboardFocus(form, text);
		XtInstallAccelerators(text, okButton);
	}
	
	XtAddCallback(okButton, XtNcallback, okCallback, arg);
	XtAddCallback(cancelButton, XtNcallback, cancelCallback, arg);

	XtPopup(shell, XtGrabNone);
}
