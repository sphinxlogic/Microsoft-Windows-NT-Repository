static char *RcsID = "$Id: xutils.c,v 1.8 1993/03/02 00:49:03 rfs Exp $";
/*
 * $Log: xutils.c,v $
 * Revision 1.8  1993/03/02  00:49:03  rfs
 * Fixed up.
 *
 * Revision 1.7  1993/02/26  21:39:16  rfs
 * Handle decoding of bytes
 *
 * Revision 1.6  1993/02/14  01:17:06  rfs
 * New handling of status line.
 *
 * Revision 1.5  1993/02/13  23:27:17  rfs
 * Added widget association routines.
 *
 * Revision 1.4  1993/02/13  18:00:12  rfs
 * Added routines to set and get mouse coordinates.
 *
 * Revision 1.3  1993/02/13  13:39:54  rfs
 * added function ResetMouseCoord
 *
 * Revision 1.2  1993/02/13  13:21:27  rfs
 * byte offset now reflects where the cursor is.
 *
 * Revision 1.1  1993/02/13  12:02:17  rfs
 * Initial revision
 *
 *
*/


#include <xod.h>
#include <xod.pt>
#include <buttonndx.h>

/* widget association list */
static WidgetList wassoc =  NULL;
static int wassoccnt = 0;

static int fwidth, fheight;
/* mouse coord's in row/column xy's */
static int mousex = 1;
static int mousey = 1;

/* revert mouse coordinates to the root */
ResetMouseCoord(void) {
	mousex = mousey = 1;
}

void
GetMouseCoord(int *x, int *y) {
	*x = mousex;
	*y = mousey;
}

void
SetMouseCoord(int x, int y) {
	mousex = x;
	mousey = y;
}

/*
 * Considering each data byte as displayed consumes a single
 * coordinate, find where cursor is.
*/
void
GetCursorOnData(int *x, int *y) {
	int width, mx, my;

	GetFormat(&width);
	width++;
	GetMouseCoord(&mx, &my);
	*y = my;
	*x = ((mx-1)/width)+1;
}

/* also pretends the x & y are based on displayed bytes */
void
SetCursorOnData(int x, int y) {
	int  mx, my, width;

	GetFormat(&width);
	width++;
	MouseUnhighlight(mousex, mousey);
	mousex = (x-1)*width + 1;
	if (y < 1) y = 1;
	mousey = y;
	MouseHighlight(mousex, mousey);
	UpdateByteOffset();
}

/* compute absolute byte the cursor resides on */
int
AbsByteOffset(void) {
	int  offset, width;

	GetFormat(&width);
	width++;
	offset = (mousey-1)*BytesHoriz;
	offset += (mousex-1)/width;
	return offset;
}

CursorToAbsByte(long addr) {
	long upperleft, filesize;
	int  x, y, line, moff;

	DataLocation(&line, &moff);
	filesize = CurrentFileSize();
	if (addr > filesize) addr = filesize;
	if (addr < 0) addr = 0;
	if (!AddressOnScreen(addr)) {
		line = (addr-moff)/BytesHoriz;
		if (line < 0) line = 0;
		DisplayFullPage(line, moff);
	}
	upperleft = GetDataOffset();
	y = ((addr-upperleft)/BytesHoriz)+1;
	x = ((addr-upperleft) % BytesHoriz)+1;
	SetCursorOnData(x, y);
}

/* update the byte offset label */
void
UpdateByteOffset(void) {
	long addr;
	int  line, moff;

	DataLocation(&line, &moff);
	addr = (line*BytesHoriz) + moff;
	addr += AbsByteOffset();
	XtVaSetValues(ByteOffsetWidget, XtNlabel, FmtOffset(addr), NULL);
}


/* decode the data the mouse is on in a variety of formats */
void
DoDecode(int mode) {
	char buffer[MAXLEN];
	unsigned short us;
	unsigned long ul;
	float f;
	double d;
	int  line, moff;
	long addr;

	DataLocation(&line, &moff);
	addr = (line*BytesHoriz) + moff;
	addr += AbsByteOffset();

	switch (mode) {
		case Short:
		case uShort:
			ReadNbytes(addr, sizeof(unsigned short), (unsigned char *)&us);
			sprintf(buffer, (mode==Short) ? "%d" : "%u", us);
			XtVaSetValues(DecodeWidget, XtNlabel, buffer, NULL);
			break;
		case Long:
		case uLong:
			ReadNbytes(addr, sizeof(unsigned long), (unsigned char *)&ul);
			sprintf(buffer, (mode==Long) ? "%d" : "%u", ul);
			XtVaSetValues(DecodeWidget, XtNlabel, buffer, NULL);
			break;
		case Float:
			ReadNbytes(addr, sizeof(float), (unsigned char *)&f);
			sprintf(buffer, "%f", f);
			XtVaSetValues(DecodeWidget, XtNlabel, buffer, NULL);
			break;
		case Double:
			ReadNbytes(addr, sizeof(double), (unsigned char *)&d);
			sprintf(buffer, "%f", d);
			XtVaSetValues(DecodeWidget, XtNlabel, buffer, NULL);
			break;
	}
}

static char *
WhatMode(int mode) {
	char *amode = "?";
	switch (mode) {
		case HEX:
			amode = "hex"; break;
		case OCTAL:
			amode = "octal"; break;
		case DECIMAL:
			amode = "decimal"; break;
		case ASCII:
			amode = "ascii"; break;
	}
	return amode;
}

void
UpdateAll(int mode, long addr) {
	SetAddressMode(mode);
	ShowByteAddress(addr);
	UpdateStatus();
}

/* update the status line */
void
UpdateStatus(void) {
	char buffer[MAXLEN];
	long addr;
	int  line, moff;

	UpdateByteOffset();
	DataLocation(&line, &moff);
	addr = (line*BytesHoriz) + moff;
	ShowByteAddress(addr);
	sprintf(buffer, "< %s, offset %s, data %s >", GetFileName(),
		WhatMode(OffsetMode), WhatMode(DataMode));
	XtVaSetValues(StatusWidget, XtNlabel, buffer, NULL);
}

/* set a widget's label field */
void
SetWidgetLabel(Widget w, char *value) {
	XtVaSetValues(w, XtNlabel, value, NULL);
}

/* Send a string to the CTW widget */
SendString(char *string) {
	ctw_add_string((CtwWidget)ctwWidget, string, strlen(string));
	if (*(string+strlen(string)-1) == '\n')
		ctw_add_string((CtwWidget)ctwWidget, "\r", 1);
}

void
get_font_size(CtwWidget w, int *fwidth, int *fheight) {
	char **attr_names;
	int	len;
	int	*flags;

	len = ctw_get_attributes((CtwWidget)ctwWidget, &flags, &attr_names);
	*fwidth = flags[CTW_FONT_SIZE] >> 16;
	*fheight = flags[CTW_FONT_SIZE] & 0xffff;
}

static
FontSizes(void) {
	if (fwidth) return;
	get_font_size((CtwWidget)ctwWidget, &fwidth, &fheight);
}

static
FindCharUnderMouse(int x, int y) {
	int  row, column;
	int  width;

	FontSizes();
	GetFormat(&width);
	width++;
	/* xlate an absolute mouse xy to a char xy (root 1,1) */
	column = x/fwidth+1;
	row = y/fheight+1;
	if (column < 1) column = 1;
	if (row < 1) row = 1;
	/* make clicking in packed area look like clicking over data */
	if (column >= PackedColumn()) {
		column = ((column-PackedColumn()) * width)+1;
	}
	if (column > (BytesHoriz*width))
		return;
	MouseUnhighlight(mousex, mousey);
	CursorAt(row, column);
	mousex = column;
	mousey = row;
	MouseHighlight(column, row);
	UpdateByteOffset();
}

XtCallbackProc
MouseCB(Widget widget, char *name, ctw_callback_t *reason) {
	Window root, child;
	int  rx, ry, wx, wy;
	unsigned int mask;

	XQueryPointer(XtDisplay(widget), XtWindow(widget), &root, &child, &rx, &ry,
		&wx, &wy, &mask);
	FindCharUnderMouse(wx, wy);
	return;
}

/* build & manage table associating one widget with another */
void
AddWidgetAssoc(Widget a, Widget b) {
	int  ndx, realndx;

	wassoccnt++;
	wassoc = (WidgetList)XtRealloc((char *)wassoc, 
		wassoccnt*(2*sizeof(Widget)));
	ndx = wassoccnt-1;
	realndx = 2*ndx;

	wassoc[realndx] = a;
	wassoc[realndx+1] = b;
}

Widget
GetWidgetAssoc(Widget w) {
	Widget assoc = NULL;
	int  i, ndx;

	for (i=0; i < wassoccnt; i++) {
		ndx = i*2;
		if (wassoc[ndx] == w) {
			assoc = wassoc[ndx+1];
			break;
		}
	}
	return assoc;
}
