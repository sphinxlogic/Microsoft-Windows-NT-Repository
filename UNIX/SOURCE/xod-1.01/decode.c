static char *RcsID = "$Id: decode.c,v 1.11 1993/03/02 00:45:01 rfs Exp $";
/*
 * $Log: decode.c,v $
 * Revision 1.11  1993/03/02  00:45:01  rfs
 * Added integrity checks.
 *
 * Revision 1.10  1993/02/26  21:36:37  rfs
 * *** empty log message ***
 *
 * Revision 1.9  1993/02/23  18:22:17  rfs
 * fixed display of ascii decoded data.
 *
 * Revision 1.8  1993/02/17  22:55:01  rfs
 * *** empty log message ***
 *
 * Revision 1.7  1993/02/16  23:13:00  rfs
 * removed "smartness" about handling Home & End
 *
 * Revision 1.6  1993/02/13  23:23:03  rfs
 * Added logic to prevent any display activity on Home or End
 * if we're already there.
 *
 * Revision 1.5  1993/02/13  17:35:50  rfs
 * Fixed pronblem with ASCII decode, and keep mouse position
 * constant whne hcanging view modes.
 *
 * Revision 1.4  1993/02/13  13:41:20  rfs
 * mouse highlight on DisplayFullPagge
 *
 * Revision 1.3  1993/02/13  12:18:24  rfs
 * Added function PackedColumn
 *
 * Revision 1.1  1993/02/13  02:06:39  rfs
 * Initial revision
 *
 * Revision 1.1  1993/02/13  02:06:39  rfs
 * Initial revision
 *
*/

#include <xod.h>
#include <ctype.h>
#include <xod.pt>

static char *escbeg, *escend;

/* what column packed ascii strip begins at */
int
PackedColumn(void) {
	return CtwColumns-BytesHoriz;
}

static char *
AsciiDecode(unsigned char ch) {
	static char chbuf[128];
	char buf[10], *p;
	int  parity = 0;

	if (ch > 127) {
		parity = 1;
		ch -= 127;
	}
	*buf = ch;
	*(buf+1) = (char)0;
	p = buf;

	if (ch < ' ') {
		switch (ch) {
			case 0: p = "nul"; break;
			case 1: p = "soh"; break;
			case 2: p = "stx"; break;
			case 3: p = "etx"; break;
			case 4: p = "eot"; break;
			case 5: p = "enq"; break;
			case 6: p = "ack"; break;
			case 7: p = "bel"; break;
			case 8: p = "bs"; break;
			case 9: p = "ht"; break;
			case 10: p = "nl"; break;
			case 11: p = "vt"; break;
			case 12: p = "np"; break;
			case 13: p = "cr"; break;
			case 14: p = "so"; break;
			case 15: p = "si"; break;
			case 16: p = "dle"; break;
			case 17: p = "dc1"; break;
			case 18: p = "dc2"; break;
			case 19: p = "dc3"; break;
			case 20: p = "dc4"; break;
			case 21: p = "nak"; break;
			case 22: p = "syn"; break;
			case 23: p = "etb"; break;
			case 24: p = "can"; break;
			case 25: p = "em"; break;
			case 26: p = "sub"; break;
			case 27: p = "esc"; break;
			case 28: p = "fs"; break;
			case 29: p = "gs"; break;
			case 30: p = "rs"; break;
			case 31: p = "us"; break;
		}
	}
	if (ch == 127)
		p = "del";
	if (parity)
		sprintf(chbuf, "%s%3s%s", UNDERLINE, p, DATANORMAL);
	else
		sprintf(chbuf, "%3s", p);
	return chbuf;
}

static char *
PackedEncode(unsigned char *data,  int length) {
	static char buffer[MAXLEN];
	unsigned char *src = data;
	unsigned char *dest = buffer;
	unsigned ch;
	int i, ndx;

	for (i=0, ndx =0; i < BytesHoriz; i++) {
		if (i >= length) {
			*dest++ = ' ';
			continue;
		}
		ch = *src;
		*dest = (isprint(ch)) ? ch : ' ';
		src++;
		dest++;
	}
	*dest = (char)0;
	return buffer;
}

char *
GetFormat(int *width) {
	char *fmt;
	int  n = 3;

	switch (DataMode) {
		case HEX:
			n = 2;
			fmt = " %02x";
			break;
		case OCTAL:
			fmt = " %03o";
			break;
		case DECIMAL:
			fmt = " %03d";
			break;
		case ASCII:
			fmt = " %3s";
			break;
		default:
			n = 2;
			fmt = " %02x";
			break;
	}
	*width = n;
	return fmt;
}

/* code to highlight the byte under the mouse click */
static void
MouseLight(int mousex, int mousey, int hilite) {
	char buffer[MAXLEN], chbuf[MAXLEN];
	char *fmt, *pa;
	unsigned char ch;
	int  width, chpos, row, col;

	switch (hilite) {
		case 0:
			escbeg = DATANORMAL;
			escend = "";
			break;
		case 1:
			escbeg = RevVideo();
			escend = DATANORMAL;
			break;
	}
	fmt = GetFormat(&width);
	/* account for space in front of each entry */
	width++;
	chpos = (mousex-1)/width;
	chpos++;
	if (chpos > BytesHoriz)
		return;
	ch = ByteAtPosition(mousey, chpos);
	row = mousey;
	col = (mousex-1)-((mousex-1)%width)+1;
	if (DataMode == ASCII) {
		*chbuf = ' '; *(chbuf+1) = NIL;
		strcat(chbuf, (hilite) ? RevVideo() : DATANORMAL);
		strcat(chbuf, AsciiDecode(ch));
	}
	else
		sprintf(chbuf, fmt, ch);
	sprintf(buffer, "%s%s%s", escbeg, chbuf, escend);
	CursorAt(row, col);
	SendString(buffer);
	/* and, highlight the packed ascii portion too */
	CursorAt(mousey, PackedColumn()+chpos-1);
	pa = PackedEncode(&ch, 1);
	*(pa+1) = NIL;
	sprintf(buffer, "%s%s%s", escbeg, pa, escend);
	SendString(buffer);
	/* send cursor to lower right of screen */
	CursorAt(NumberRows, CtwColumns);
}

void
MouseHighlight(int mousex, int mousey) {
	MouseLight(mousex, mousey, 1);
}

void
MouseUnhighlight(int mousex, int mousey) {
	MouseLight(mousex, mousey, 0);
}

char *
BuildLine(unsigned char *data, int length) {
	static char buffer[3*MAXLEN];
	char minibuf[MAXLEN];
	char *fmt, *here;
	char *p;
	int i, n;

	*buffer = (char)0;
	here = buffer;
	fmt = GetFormat(&n);

	for (i=0; i < length; i++) {
		if (DataMode == ASCII) {
			p = AsciiDecode(*data);
			sprintf(here, " %s", p);
			here += strlen(p)+1;
		}
		else {
			sprintf(here, fmt, *data);
			here += n+1;
		}
		data++;
	}

	if (DataMode == ASCII) {
		p = AsciiDecode(0);
		n = strlen(p);
	}
	else {
		sprintf(minibuf, fmt, 0);
		p = minibuf;
	}

	for (i=length; i < BytesHoriz; i++) {
		sprintf(here, "%s", p);
		here += n+1;
	}
	return buffer;
}

/* display a single line of binary data in the CTW widget */
void
DisplayLine(unsigned char *data, int length, int line) {
	char buffer[MAXLEN];
	char *cp;

	line++;
	CursorAt(line, 1);
	cp = BuildLine(data, length);
	SendString(cp);
	CursorAt(line, PackedColumn());
	SendString(PackedEncode(data, length));
}

void
DisplayFullPage(int line, int moff) {
	unsigned char *data;
	static int dispcnt = 0;
	int i, cline, cmoff, maxline;

	/* don't do End if we're already there */
	if (line < 0 && IsAtEOF()) return;
	/* don't do Home if we're already there */
	if (dispcnt)
		DataLocation(&cline, &cmoff);

	maxline = LargestLine();
	if (line > maxline) line = maxline;
	data = ReadBlock(line, BytesHoriz, NumberRows, moff);
	if (!dispcnt)
		ClearScreen();
	dispcnt++;
	for (i=0; i < NumberRows; i++)  {
		DisplayLine(data+(i*BytesHoriz), BytesHoriz, i);
	}
	ResetMouseCoord();
	MouseHighlight(1, 1);
	UpdateStatus();
}

void
RedisplayPage(void) {
	int line, moff;
	int x, y;

	GetCursorOnData(&x, &y);
	DataLocation(&line, &moff);
	ClearScreen();
	DisplayFullPage(line, moff);
	MouseUnhighlight(1, 1);
	/*
	SetMouseCoord(mx, my);
	MouseHighlight(mx, my);
	*/
	SetCursorOnData(x, y);
}
