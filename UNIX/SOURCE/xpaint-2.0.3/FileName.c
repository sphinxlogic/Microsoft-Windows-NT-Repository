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

#include "Paint.h"
#include "Misc.h"
#include "Image.h"

/*
**  "public" interface, for those of you who want to add
**   readers or writers.
*/

typedef	Image	*(*read_proc)(char *);
typedef	Boolean	(*write_proc)(char *, Image *);
typedef	Boolean	(*test_proc)(char *);

typedef struct {
	char		*name;
	Widget		w;
	read_proc	read;
	write_proc	write;
	test_proc	test;
} ImageTypes;

static Image 	*readMag(char*);
extern Image 	*ReadXBM(char*);
extern Image 	*ReadXWD(char*);
extern Image 	*ReadPNM(char*);
extern Boolean	WriteXBM(char *, Image *);
extern Boolean	WriteXWD(char *, Image *);
extern Boolean	WritePNM(char *, Image *);
extern Boolean  TestPNM(char *);
extern Boolean  TestXWD(char *);
extern Boolean  TestXBM(char *);

static ImageTypes imageTypes[] = {
	{ "Best Guess", None, readMag, NULL    , NULL    },
	{ "PPM Format", None, ReadPNM, WritePNM, TestPNM },
	{ "XBM Format", None, ReadXBM, WriteXBM, TestXBM },
	{ "XWD Format", None, ReadXWD, WriteXWD, TestXWD },
};
/*
**  Special reader that uses the above information.
*/
static Image *readMag(char *file)
{
	int	i;

	for (i = 0; i < XtNumber(imageTypes); i++) {
		if (imageTypes[i].read == NULL || imageTypes[i].test == NULL)
			continue;
		if (!imageTypes[i].test(file)) 
			continue;
		return imageTypes[i].read(file);
	}

	return NULL;
}

/*
**  "Std" Save functions
*/

static void	stdSaveCommonCallback(Widget paint, char *file, Boolean flag, write_proc f)
{
	Pixmap		pix;
        int    		width, height;
	Colormap	cmap;
	Image		*image;

	if (*file == '\0') {
		Notice(paint,"No file name supplied");
		return;
	}

	if (flag) {
		PwGetPixmap(paint, &pix, &width, &height);
	} else {
		if (!PwGetRegion(paint, &pix, NULL)) {
			Notice(paint,"Unable to get region");
			return;
		}
	}
	XtVaGetValues(paint, XtNcolormap, &cmap, NULL);

	if ((image = PixmapToImage(paint, pix, cmap)) == NULL) {
		Notice(paint,"Unable to create image for saving");
		return;
	}

	f(file, image);

	if (flag)
		XtVaSetValues(paint, XtNdirty, False, NULL);

	ImageDelete(image);
}
static void saveRegionFileCalback(Widget paint, XtPointer str, XtPointer func)
{
	stdSaveCommonCallback(paint, (char *)str, False, (write_proc)func);
}
static void saveFileCalback(Widget paint, XtPointer str, XtPointer func)
{
	String	name;
	char	*cp;

	stdSaveCommonCallback(paint, (char *)str, True, (write_proc)func);

#if 0
	XtVaGetValues(GetShell(paint), XtNtitle, &name, NULL);
	if (strcmp(name, "Untitled") == 0)
#endif
	if ((cp = strrchr(str, '/')) == NULL)
		cp = str;
	else 
		cp++;

	XtVaSetValues(GetShell(paint), XtNtitle, str, XtNiconName, cp, NULL);
}

void StdSaveRegionFile(Widget w, XtPointer paintArg, XtPointer junk)
{
	Widget	paint = (Widget)paintArg;

        if (PwGetRegion(paint, NULL, NULL))
		GetFileName(paint, True, NULL, saveRegionFileCalback);
	else
		Notice(paint, "No region selected presently");
}

void StdSaveFile(Widget w, XtPointer paintArg, XtPointer junk)
{
	Widget	paint = (Widget)paintArg;
	String	name;
	String	nm = XtName(GetShell(paint));

	XtVaGetValues(GetShell(paint), XtNtitle, &name, NULL);

	if (strcmp(name, "Untitled") == 0 || strcmp(nm, name) == 0)
		name = NULL;

	GetFileName(paint, True, name, saveFileCalback);
}

/*
**
*/
void *ReadMagic(char *file)
{
	return (void*)readMag(file);
}

/*
**  The code begins
*/
typedef struct {
	XtCallbackProc	okFunc;
	int		type;
	int		isRead;
	Widget		w, text;
	XtPointer	closure;
} arg_t;

static void fileTypeCallback(Widget w, XtPointer argArg, XtPointer junk)
{
	int	i;
	arg_t	*arg = (arg_t*)argArg;
	String	nm = XtName(w);

	for (i = 0; i < XtNumber(imageTypes); i++) 
		if (strcmp(nm, imageTypes[i].name) == 0) {
			arg->type = i;
			return;
		}
}

static void callbackOk(Widget w, XtPointer argArg, XtPointer junk)
{
	XtPointer	f;
	arg_t		*arg = (arg_t*)argArg;
	String		string;

	XtVaGetValues(arg->text, XtNstring, &string, NULL);

	if (arg->isRead)
		f = (XtPointer)imageTypes[arg->type].read;
	else
		f = (XtPointer)imageTypes[arg->type].write;

	XtPopdown(GetShell(w));

	if (arg->okFunc != NULL) {
		if (!arg->isRead) {
			arg->okFunc(arg->w, string, f);
		} else {
			Image	*image = ((read_proc)f)(string);
			if (image == NULL)
				Notice(w, "Unable to open input file \"%s\"", string);
			else
				arg->okFunc(arg->w, string, image);
		}
	}
}

static void callbackCancel(Widget w, XtPointer argArg, XtPointer junk)
{
	XtPopdown(GetShell(w));
}

void GetFileName(Widget w, int op, char *def, XtCallbackProc okFunc)
{
	Position	x, y;
	static char	buf[256];
	static Widget	shell = None, label, okButton, cancelButton, text;
	Widget		form, form2, toggle = None, firstToggle = None;
	static arg_t	arg;
	int		i;
	static String	textAccelerators = "#override\n\
				<Key>Return: set() notify() unset()\n\
				<Key>Linefeed: set() notify() unset()";
	static String	textTranslations = "#override\n\
						 <Key>Return: no-op()\n\
						 <Key>Linefeed: no-op()\n\
						 Ctrl<Key>M: no-op()\n\
						 Ctrl<Key>J: no-op()\n";

	if (def != NULL && *def != '\0')
		strcpy(buf, def);
	else
		buf[0] = '\0';

	if (shell == None) {
		XtTranslations	trans, toglt;
		XtAccelerators	accel;
		trans = XtParseTranslationTable(textTranslations);
		toglt = XtParseTranslationTable(
				"<BtnDown>,<BtnUp>: set() notify()");
		accel = XtParseAcceleratorTable(textAccelerators);

		shell = XtVaCreatePopupShell("filebrowser",
				transientShellWidgetClass, GetToplevel(w),
				NULL);
		form = XtVaCreateManagedWidget(NULL,
					formWidgetClass, shell,
					XtNborderWidth, 0,
					NULL);

		label = XtVaCreateManagedWidget("label",
					labelWidgetClass, form,
					XtNborderWidth, 0,
					NULL);
		text = XtVaCreateManagedWidget("text",
					asciiTextWidgetClass, form,
					XtNfromVert, label,
					XtNeditType, XawtextEdit,
					XtNwrap, XawtextWrapNever,
					XtNresize, XawtextResizeWidth,
					XtNlength, sizeof(buf) - 1,
					XtNtranslations, trans,
					NULL);

		form2 = XtVaCreateManagedWidget(NULL,
					formWidgetClass, form,
					XtNfromVert, text,
					XtNborderWidth, 0,
					NULL);

		for (i = 0; i < XtNumber(imageTypes); i++) {
			toggle = XtVaCreateManagedWidget(imageTypes[i].name,
						toggleWidgetClass, form2,
						XtNtranslations, toglt,
						XtNradioGroup, firstToggle,
						XtNfromVert, toggle,
						NULL);
			imageTypes[i].w = toggle;
			if (firstToggle == None) {
				XtVaSetValues(toggle, XtNstate, True, NULL);
				firstToggle = toggle;
			}
			XtAddCallback(toggle, XtNcallback, fileTypeCallback, (XtPointer)&arg);
		}

		okButton = XtVaCreateManagedWidget("ok",
					commandWidgetClass, form,
					XtNfromVert, form2,
					XtNaccelerators, accel,
					NULL);
		cancelButton = XtVaCreateManagedWidget("cancel",
					commandWidgetClass, form,
					XtNfromVert, form2,
					XtNfromHoriz, okButton,
					NULL);
		XtSetKeyboardFocus(form, text);
		XtInstallAccelerators(text, okButton);
	}

	/*
	**  Set the title to something interesting
	*/
	if (op) {
		XtVaSetValues(label, XtNlabel, "SAVE -- Filename:", NULL);
		arg.isRead = False;
	} else {
		XtVaSetValues(label, XtNlabel, "READ -- Filename:", NULL);
		arg.isRead = True;
	}

	for (i = 0; i < XtNumber(imageTypes); i++) {
		if ((arg.isRead  && imageTypes[i].read == NULL) ||
		    (!arg.isRead && imageTypes[i].write == NULL)) {
			Boolean	flg;

			XtVaSetValues(imageTypes[i].w, XtNsensitive, False, NULL);
			XtVaGetValues(imageTypes[i].w, XtNstate, &flg, NULL);
			if (flg) {
				int	j;
				/*
				**  If the unhighlighted selection is set,
				**   unset it, and find item to highlite.
				*/
				XtVaSetValues(imageTypes[i].w, XtNstate, False, NULL);
				for (j = 0; j < XtNumber(imageTypes); j++) {
					if ((arg.isRead  && imageTypes[j].read != NULL) ||
					    (!arg.isRead && imageTypes[j].write != NULL)) {
						arg.type = j;
						XtVaSetValues(imageTypes[j].w, XtNstate, True, NULL);
						break;
					}
				}
			}
		} else {
			XtVaSetValues(imageTypes[i].w, XtNsensitive, True, NULL);
		}
	}

	XtVaGetValues(GetToplevel(w), XtNx, &x, XtNy, &y, NULL);
	XtVaSetValues(shell, XtNx, x + 24, XtNy, y + 24, NULL);
	
	/*
	**  Make sure everything is cleared.
	*/
	XtVaSetValues(text, XtNstring, buf, XtNinsertPosition, strlen(buf), NULL);

	XtRemoveAllCallbacks(okButton, XtNcallback);
	XtRemoveAllCallbacks(cancelButton, XtNcallback);

	arg.okFunc  = okFunc;
	arg.w       = w;
	arg.text    = text;

	XtAddCallback(okButton, XtNcallback, callbackOk, (XtPointer)&arg);
	XtAddCallback(cancelButton, XtNcallback, callbackCancel, (XtPointer)&arg);

	XtPopup(shell, XtGrabNone);
}
