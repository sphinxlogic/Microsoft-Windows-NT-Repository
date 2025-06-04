/*
*****************************************************************************
*                                                                           *
*  COPYRIGHT (c) 1990  BY                                                   *
*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.                   *
*  ALL RIGHTS RESERVED.                                                     *
*                                                                           *
*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED    *
*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE    *
*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER    *
*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY    *
*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY    *
*  TRANSFERRED.                                                             * 
*                                                                           *
*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE    *
*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT    *
*  CORPORATION.                                                             *
*                                                                           *
*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS    *
*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                  *
*                                                                           *
*****************************************************************************
*/

#include <stdio.h>
#include <ctype.h>

struct _Descriptor
	{
	short length;
	short junk;
	char *pointer;
	};

int
HexToInteger(struct _Descriptor *s)
	{
	int i, v=0;
	char c;

	for(i = 0; i < s->length; i++)
		{
		c = *(s->pointer+i);
		if (isdigit(c))
			v = (v << 4) + (c - '0');
		else if (isupper(c))
			v = (v << 4) + (c - 'A' + 10);
		else
			v = (v << 4) + (c - 'a' + 10);
		}

	return v;
	}

void
IntegerToHex(int value, char *s)
	{
	sprintf(s, "%8.8X", value);
	}
