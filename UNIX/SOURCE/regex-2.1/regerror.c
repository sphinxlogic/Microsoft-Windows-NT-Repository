/*-
 * Copyright (c) 1992 Henry Spencer.
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Henry Spencer of the University of Toronto.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)regerror.c	5.3 (Berkeley) 9/30/92
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)regerror.c	5.3 (Berkeley) 9/30/92";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <regex.h>

#include "utils.h"

static struct rerr {
	int code;
	char *name;
	char *explain;
} rerrs[] = {
	REG_NOMATCH,	"NOMATCH",	"regexec() failed to match",
	REG_BADPAT,	"BADPAT",	"invalid regular expression",
	REG_ECOLLATE,	"ECOLLATE",	"invalid collating element",
	REG_ECTYPE,	"ECTYPE",	"invalid character class",
	REG_EESCAPE,	"EESCAPE",	"trailing backslash (\\)",
	REG_ESUBREG,	"ESUBREG",	"invalid backreference number",
	REG_EBRACK,	"EBRACK",	"brackets ([ ]) not balanced",
	REG_EPAREN,	"EPAREN",	"parentheses not balanced",
	REG_EBRACE,	"EBRACE",	"braces not balanced",
	REG_BADBR,	"BADBR",	"invalid repetition count(s)",
	REG_ERANGE,	"ERANGE",	"invalid character range",
	REG_ESPACE,	"ESPACE",	"out of memory",
	REG_BADRPT,	"BADRPT",	"repetition-operator operand invalid",
	REG_EMPTY,	"EMPTY",	"empty (sub)expression",
	REG_ASSERT,	"ASSERT",	"\"can't happen\" -- you found a bug",
	0,		"",		"*** unknown regexp error code ***",
};

/*
 - regerror - the interface to error numbers
 */
/* ARGSUSED */
size_t
regerror(errcode, preg, errbuf, errbuf_size)
int errcode;
const regex_t *preg;
char *errbuf;
size_t errbuf_size;
{
	register struct rerr *r;
	register size_t len;

	for (r = rerrs; r->code != 0; r++)
		if (r->code == errcode)
			break;

	len = strlen(r->explain) + 1;
	if (errbuf_size > 0) {
		if (errbuf_size > len)
			(void) strcpy(errbuf, r->explain);
		else {
			(void) strncpy(errbuf, r->explain, errbuf_size-1);
			errbuf[errbuf_size-1] = '\0';
		}
	}

	return(len);
}

#ifdef REDEBUG
/*
 - eprint - express an error number as a string
 */
char *
eprint(eno)
int eno;
{
	register struct rerr *r;
	static char eval[10];

	for (r = rerrs; r->code != 0; r++)
		if (r->code == eno)
			return(r->name);
	sprintf(eval, "#%d", r->code);
	return(eval);
}

/*
 - efind - find an error name
 */
int
efind(ename)
char *ename;
{
	register struct rerr *r;

	for (r = rerrs; r->code != 0; r++)
		if (strcmp(r->name, ename) == 0)
			return(r->code);
	return(0);		/* it'll do */
}
#endif
