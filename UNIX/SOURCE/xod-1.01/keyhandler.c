static char *RcsID = "$Id: keyhandler.c,v 1.5 1993/03/02 00:47:01 rfs Exp $";

/*
 * $Log: keyhandler.c,v $
 * Revision 1.5  1993/03/02  00:47:01  rfs
 * Added new features.
 *
 * Revision 1.4  1993/02/26  21:38:22  rfs
 * *** empty log message ***
 *
 * Revision 1.3  1993/02/23  18:23:38  rfs
 * Many mods, all for the better.
 *
 * Revision 1.2  1993/02/14  01:15:09  rfs
 * made certain keys work properly
 *
 * Revision 1.1  1993/02/13  02:19:24  rfs
 * Initial revision
 *
*/

#include <xod.h>
#include <keys.h>

/*
 * This is the function that handles all keys the user hits
 * while focus is in the data display window.
*/

XtEventHandler
keyhandler(Widget w, void *cd, XKeyEvent *event) {
	static int ctrl = 0;
	static int meta = 0;
	static int alt = 0;
	static int lastch = 0;
	char buffer[MAXLEN];
	KeySym keysym;
	XComposeStatus compose;
	int line, moff, x, y;
	int isctrl = False;
	int ismeta = False;
	int isalt = False;
	int isshift = False;

	
	XLookupString(event, buffer, MAXLEN, &keysym, &compose);

	/* see if key is a meta or ctrl key */
	if (keysym == XK_Meta_L || keysym == XK_Meta_R)
		ismeta = True;
	if (keysym == XK_Control_L || keysym == XK_Control_R)
		isctrl = True;
	if (keysym == XK_Alt_L || keysym == XK_Alt_R)
		isalt = True;
	if (keysym == XK_Shift_L || keysym == XK_Shift_R)
		isshift = True;

	/* ignore KeyRelease for non-meta or ctrl keys */
	if (isshift)
		return;
	if (!ismeta && !isctrl && !isalt && (event->type != KeyPress))
		return;

	/* set internal state of meta, ctrl, and alt */
	if (isctrl) {
		ctrl = (event->type == KeyPress) ? 1 : 0;
		return;
	}
	else if (ismeta) {
		meta = (event->type == KeyPress) ? 1 : 0;
		return;
	}
	else if (isalt) {
		alt = (event->type == KeyPress) ? 1 : 0;
		return;
	}

	/* Meta keys */
	if (meta) {
		switch (keysym) {
			case 'h': DataMode = HEX; ClearScreen(); break;
			case 'o': DataMode = OCTAL; ClearScreen(); break;
			case 'd': DataMode = DECIMAL; ClearScreen(); break;
			case 'a': DataMode = ASCII; ClearScreen(); break;
		}
	}
	/* Alt keys */
	else if (alt) {
		long addr = GetDataOffset();
		/* select offset display mode */
		switch (keysym) {
			case 'h':
				UpdateAll(HEX, addr);
				break;
			case 'o':
				UpdateAll(OCTAL, addr);
				break;
			case 'd':
				UpdateAll(DECIMAL, addr);
				break;
		}
		UpdateByteOffset();
		return;
	}
	/* Ctrl keys */
	else if (ctrl) {
		switch (keysym) {
			case 'f': keysym = PgDn; break;
			case 'b': keysym = PgUp; break;
			case 'c': exit(0); break;
			case 'v': keysym = PgDn; break;			/* emacs... ulk */
			case 'n': keysym = DownArrow; break;	/* emacs... ulk */
			case 'p': keysym = UpArrow; break;		/* emacs... ulk */
		}
	}
	/* regular keys */
	else {
		switch (keysym) {
			case 'h': keysym = LeftArrow; break;
			case 'l': keysym = RightArrow; break;
			case 'j': keysym = DownArrow; break;
			case 'k': keysym = UpArrow; break;
			case 'n': NextSearch(); return; break;
			case 'v': 
				if (lastch == XK_Escape) keysym = PgUp;	/* emacs... ulk */
				break;
			case XK_slash: 
				PopupSearch("Search for:");
				return;
				break;
		}
	}
	switch (keysym) {
		case 'G':
			keysym = End;
			break;
		case '+':
			break;
		case '-':
			break;
	}
	DataLocation(&line, &moff);
	switch (keysym) {
		case '+':
			PopupAdvanceToByte("+");
			return;
			break;
		case '-':
			PopupAdvanceToByte("-");
			return;
			break;
		case '<':
		case '>':
			return;
			break;
		case '?':
			PopupHelpDialog();
			return;
			break;
		case '$':
			GetCursorOnData(&x, &y);
			if (x < BytesHoriz)
				SetCursorOnData(BytesHoriz, y);
			return;
			break;
		case 'q':
			exit(0);
			break;
		case XK_Return:
			if (!IsAtEOF())
				line++;
			break;
		case PgUp:
			if (line == 0)
				moff = 0;
			if (line > 0)
				line -= NumberRows;
			if (line < 0) {
				line = 0;
				moff = 0;
			}
			DataOffset -= NumberRows*BytesHoriz;
			break;
		case PgDn:
			if (!IsAtEOF())
				line += NumberRows;
			break;
		case Home:
			line = moff = 0;
			break;
		case End:
			CursorToAbsByte(CurrentFileSize());
			return;
			break;
		case RightArrow:
			GetCursorOnData(&x, &y);
			x++;
			if (x <= BytesHoriz) 
				SetCursorOnData(x, y);
			else {
				x = 1;
				y++;
				if (y > NumberRows) {
					if (IsAtEOF()) return;
					line++;
					DisplayFullPage(line, moff);
					SetCursorOnData(x, 0);
				}
				else
					SetCursorOnData(x, y);
			}
			return;
			break;
		case LeftArrow:
			GetCursorOnData(&x, &y);
			if (x == 1) {
				y--;
				if (y == 0) {
					line--;
					if (line >= 1)
						DisplayFullPage(line, moff);
				}
				x = BytesHoriz;
				SetCursorOnData(x, y);
			}
			else {
				if (x >= 1)
					SetCursorOnData(--x, y);
			}
			return;
			break;
		case DownArrow:
			GetCursorOnData(&x, &y);
			if (y >= NumberRows) {
				if (IsAtEOF()) return;
				line++;
				DisplayFullPage(line, moff);
				SetCursorOnData(x, NumberRows);
			}
			else {
				SetCursorOnData(x, ++y);
			}
			return;
			break;
		case UpArrow:
			GetCursorOnData(&x, &y);
			y--;
			if (y == 0) {
				line--;
				if (line < 0) return;
				DisplayFullPage(line, moff);
				SetCursorOnData(x, y);
			}
			else
				SetCursorOnData(x, y);
			return;
			break;
	}
	lastch = keysym;
	DisplayFullPage(line, moff);
}
