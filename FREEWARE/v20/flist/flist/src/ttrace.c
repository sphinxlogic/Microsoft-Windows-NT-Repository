/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
#ifndef NO_IDENT
static char *Id = "$Id: ttrace.c,v 1.4 1995/05/28 19:53:39 tom Exp $";
#endif

#include <stdio.h>
#include <stdarg.h>

#include "ttrace.h"
#include "sysutils.h"

static	FILE	*fd;

static
void	TraceVA(char *format, va_list ap)
{
	if (!fd)
		fd = fopen ("sys$scratch:trace", "w");
	if (fd)
		vfprintf (fd, format, ap);
}

void	trace(char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	TraceVA(format, ap);
	va_end(ap);
}

void	ttrace (char *format, ...)
{
	va_list ap;
	char	msg[132];

	sysfom (msg);

	va_start (ap, format);
	TraceVA(format, ap);
	va_end(ap);

	trace (">%s\n", msg);
	sysfom(0);
}
