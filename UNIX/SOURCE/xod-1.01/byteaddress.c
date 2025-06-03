static char *RcsId = "$Id: byteaddress.c,v 1.2 1993/02/13 18:43:58 rfs Exp rfs $";

/*
 * $Log: byteaddress.c,v $
 * Revision 1.2  1993/02/13  18:43:58  rfs
 * *** empty log message ***
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
*/

#include "defines.h"
#include "gvars.h"
#include "gwidgets.h"
#include <stdio.h>
#include <malloc.h>

static char *fmts[] = {
	"%06x\n",
	"%06o\n",
	"%06d\n",
};
static char *offsetbuf = NULL;

void
SetAddressMode(int mode) {
	OffsetMode = mode;
	OffsetFmt = fmts[mode];
}

char *
FmtOffset(long addr) {
	static char buffer[MAXLEN];

	sprintf(buffer, OffsetFmt, addr);
	return buffer;
}

void
ShowByteAddress(long startaddr) {
	char buffer[128];
	int addr = startaddr;
	int i;

	if (!offsetbuf) {
		offsetbuf = (char *)malloc(NumberRows*(6+2));
	}
	if (!OffsetFmt) {
		OffsetMode = 0;
		OffsetFmt = fmts[0];
	}
	*offsetbuf = (int)NULL;
	for (i=0; i < NumberRows; i++) {
		sprintf(buffer, OffsetFmt, addr);
		strcat(offsetbuf, buffer);
		addr += BytesHoriz;
	}
	SetWidgetLabel(ByteAddress, offsetbuf);
}
