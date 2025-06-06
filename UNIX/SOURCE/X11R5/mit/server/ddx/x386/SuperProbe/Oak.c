/*
 * Copyright 1993,1994 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/SuperProbe/Oak.c,v 2.5 1994/02/28 14:09:43 dawes Exp $ */

#include "Probe.h"

static Word Ports[] = {0x3DE, 0x3DF};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_Oak __STDCARGS((int));

Chip_Descriptor Oak_Descriptor = {
	"Oak",
	Probe_Oak,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_Oak,
};

Bool Probe_Oak(Chipset)
int *Chipset;
{
	Byte temp;
	Bool result = FALSE;

	EnableIOPorts(NUMPORTS, Ports);

	if (testinx2(0x3DE, 0x0D, 0x38))
	{
		result = TRUE;
		if (testinx2(0x3DE, 0x23, 0x1F))
		{
			if ((rdinx(0x3DE, 0x00) & 0x02) == 0x00)
			{
				*Chipset = CHIP_OAK087;
			}
			else
			{
				*Chipset = CHIP_OAK083;
			}
		}
		else 
		{
			temp = inp(0x3DE) >> 5;
			switch (temp)
			{
			case 0x00:
				*Chipset = CHIP_OAK037C;
				break;
			case 0x02:
				*Chipset = CHIP_OAK067;
				break;
			case 0x05:
				*Chipset = CHIP_OAK077;
				break;
			case 0x07:
				*Chipset = CHIP_OAK057;
				break;
			default:
				Chip_data = temp;
				*Chipset = CHIP_OAK_UNK;
				break;
			}
		}
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_Oak(Chipset)
int Chipset;
{
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	switch (Chipset)
	{
	case CHIP_OAK083:
	case CHIP_OAK087:
		switch (rdinx(0x3DE, 0x02) & 0x06)
		{
		case 0x00:
			Mem = 256;
			break;
		case 0x02:
			Mem = 512;
			break;
		case 0x04:
			Mem = 1024;
			break;
		case 0x06:
			Mem = 2048;
			break;
		}
		break;
	default:
		switch (rdinx(0x3DE, 0x0D) >> 6)
		{
		case 0x00:
			Mem = 256;
			break;
		case 0x02:
			Mem = 512;
			break;
		case 0x01:
		case 0x03:
			Mem = 1024;
			break;
		}
		break;
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
