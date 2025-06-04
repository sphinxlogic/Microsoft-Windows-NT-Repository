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

#include <stdlib.h>

typedef struct {
	int address;
	void *info;
} AddrInfo;

static int addr_info_size = 0;
static int valid_info = 0;
static AddrInfo (*addr_info)[] = (void *)0;

void
ResetAddressSpace(int size)
	{
	if (size > addr_info_size)
		{
		if (addr_info)
			free(addr_info);

		addr_info = (AddrInfo *)malloc(sizeof(AddrInfo) * size);
		addr_info_size = size;
		}

	valid_info = 0;
	}

void
DefineAddress(int address, void *info)
	{
	(*addr_info)[valid_info].address = address;
	(*addr_info)[valid_info].info    = info;

	valid_info++;
	}


void *
LookupAddress(int address)
{
	int first = 0;
	int last = valid_info - 1;
	int mid;

	while (first <= last)
		{
		mid = (last + first) / 2;

		if (address < (*addr_info)[mid].address)
			last = mid - 1;
		else if (address > (*addr_info)[mid].address)
			first = mid + 1;
		else
			return (*addr_info)[mid].info;
		}

	return (*addr_info)[last].info;
}
