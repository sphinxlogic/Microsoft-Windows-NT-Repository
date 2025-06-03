static char *RcsID = "$Id: main.c,v 1.10 1993/03/02 02:55:46 rfs Exp $";
/*
 * $Log: main.c,v $
 * Revision 1.10  1993/03/02  02:55:46  rfs
 * *** empty log message ***
 *
 * Revision 1.9  1993/03/02  00:47:41  rfs
 * *** empty log message ***
 *
 * Revision 1.8  1993/02/26  21:38:39  rfs
 * *** empty log message ***
 *
 * Revision 1.7  1993/02/24  23:33:37  rfs
 * *** empty log message ***
 *
 * Revision 1.6  1993/02/23  18:24:38  rfs
 * Added search dialog.
 *
 * Revision 1.5  1993/02/13  23:25:41  rfs
 * pop up error dialog if file to be opened can't be.
 *
 * Revision 1.4  1993/02/13  14:37:09  rfs
 * accept a new file name & load it
 *
 * Revision 1.3  1993/02/13  13:42:17  rfs
 * resources read before DisplayFullPage called.
 *
 * Revision 1.2  1993/02/13  11:35:12  rfs
 * position cursor initially over 1st byte in file
 *
 * Revision 1.1  1993/02/13  02:06:39  rfs
 * Initial revision
 *
 * Revision 1.1  1993/02/13  02:06:39  rfs
 * Initial revision
 *
*/


#define XOD_MAIN 1
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
#include <version.h>
#include <xod.pt>

static Widget top;
static unsigned char *SearchPattern = NULL;
static int  SearchPatternLength = 0;

/* show version information */
char *
XodVersion(void) {
	return VERSION;
}

/* set the toplevel widget */
void
SetTopLevel(Widget w) {
	top = w;
}

/* get the toplevel widget */
Widget
GetTopLevel(void) {
	return top;
}

/* how many bytes to be displayed horizontally */
int
GetBytesHoriz(void) {
	return BytesHoriz;
}

/* data initialization */
gvInit() {
	datafp = (FILE *)NULL;
	FileSize = 0;
	DataOffset = 0;
	BytesHoriz = 16;
	OffsetClickedOn = -1;
	MatchAt = -1;
	SearchMode = SmGrep;
	OffsetFmt = NULL;
	OffsetMode = HEX;
	MaxOffset = 0;
	DataMode = HEX;
}

void
EditModeCB(Widget w, XtPointer client, XtPointer call) {
	ButtonPtr bp = (ButtonPtr)client;
	int  x, y;

	GetCursorOnData(&x, &y);
	DataMode = bp->id;
	RedisplayPage();
	SetCursorOnData(x, y);
}

void
EditDecodeCB(Widget w, XtPointer client, XtPointer call) {
	ButtonPtr bp = (ButtonPtr)client;
	DoDecode(bp->id);
}

XtActionProc
Junk() {
}

void SetDialogButton();

Boolean image_visible = False;
Pixmap check_mark;
Dialog NewFileDialog, SearchDialog, qsave_dialog;
String filename = NULL, basename = NULL, format;
char message[80];


void 
FileMenuCallback(Widget w, XtPointer clientData, XtPointer callData) {
	String file;
	char buffer[MAXLEN];
	int *id = (int *)clientData;
	int stat;

	switch (*id) {
		case New:
			stat = PopupDialog(NewFileDialog, "File to dump:", "", &file,
				XtGrabExclusive);
			if (stat == Okay) {
				if (strlen(file)) {
					if (FileOpen(file)) {
						DisplayFullPage(0, 0);
					}
					else {
						sprintf(buffer, "Cannot open file %s", file);
						PopupErrorDialog(ErrorDialog, buffer);
					}
				}
			}
			break;
		case Quit:
			exit(0);
			break;
	}
}

NextSearch(void) {
	if (!SearchPattern) return;
	DoSearch(SearchPattern, SearchPatternLength);
}

void 
PopupSearch(String message) {
	String search4;
	char buffer[MAXLEN];
	int stat, len;

	stat = PopupDialog(SearchDialog, message, "", &search4,
		XtGrabExclusive);
	if (stat == Okay) {
		if (SearchPattern)
			XtFree(SearchPattern);
		len = strlen(search4);
		SearchPattern = XtMalloc(len+1);
		strcpy(SearchPattern, search4);
		SearchPatternLength = len;
		DoSearch(search4, len);
	}
}

void 
main(int argc, char **argv) {
	Arg wargs[2];
	char buffer[MAXLEN];
	int rows, columns;
	int i, n;

	gvInit();
	MakeWidgets(&argc, argv);
	ParseCommandLine(argc, argv);

	XtVaGetValues(ctwWidget, XtNrows, &rows, XtNcolumns, &columns, NULL);
	NumberRows = rows;
	CtwColumns = columns;
	ShowByteAddress(0);

	/* capture keystrokes in the CTW widget */
	XtAddEventHandler(ctwWidget, KeyPressMask|KeyReleaseMask, False,
		(XtEventHandler)keyhandler, (XtPointer)NULL);

	XtRealizeWidget(top); 
	sprintf(buffer, "xod v%s", VERSION);
	XStoreName(XtDisplay(top), XtWindow(top), buffer);

	/* see getres.c to see what this does */
	XodGetResources(top);
	FigureRevVidSeq();
	DisplayFullPage(0, 0);
	/* do what may have been told us via cmd line args */
	CmdLineInit();

	/* create all the dialogs we'll be using */
	NewFileDialog = CreateDialog(top, "newFile", Okay | Cancel);
	SearchDialog = CreateDialog(top, "search", Okay | Cancel);    
	GotoByteDialog = CreateDialog(top, "gotoByte", Okay | Cancel);    
	HelpDialog = CreateHelpDialog(top);

	XtMainLoop();
}
