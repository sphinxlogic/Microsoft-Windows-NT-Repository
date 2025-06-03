static char *RcsId = "$Id: utils.c,v 1.3 1993/03/02 00:48:38 rfs Exp $";

/*
 * $Log: utils.c,v $
 * Revision 1.3  1993/03/02  00:48:38  rfs
 * Added AddressConvert routine.
 *
 * Revision 1.2  1993/02/13  14:35:15  rfs
 * *** empty log message ***
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
*/

#include "xod.h"

/* Position cursor */
CursorAt(int x, int y) {
	char buffer[MAXLEN];
	sprintf(buffer, "\033[%d;%dH", x, y);
	SendString(buffer);
}

/* clear screen and home cursor */
ClearScreen() {
	char buffer[MAXLEN];
	sprintf(buffer, "\033[2J%s", DATANORMAL);
	CursorAt(1, 1);
	SendString(buffer);
}

/* convert ASCII address in decimal, octal, or hext to numeric equiv */
long
AddressConvert(char *caddr) {
	long addr;
	char *p, *fmt;

	if (!strncmp(caddr, "0x", 2)) {
		p = caddr+2;
		fmt = "%x";
	}
	else if (*caddr == '0') {
		p++;
		fmt = "%o";
	}
	else {
		p = caddr;
		fmt = "%d";
	}
	sscanf(p, fmt, &addr);
	return addr;
}
