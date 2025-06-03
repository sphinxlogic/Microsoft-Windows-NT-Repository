/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
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
 */

#ifndef lint
/*static char sccsid[] = "from: @(#)uudecode.c	5.10 (Berkeley) 6/1/90";*/
static char rcsid[] = "$Id: uudecode.c,v 1.4 1993/11/09 01:46:38 jtc Exp $";
#endif /* not lint */

/* Ported to VMS and added a feature, enabled with "-m", to extract multiple
 encoded segments from a single file, by matsunaw@jrd.dec.com, Nov 1994. */

/*
 * uudecode [-mvh] [file ...]
 *
 * create the specified file, decoding as you go.
 * used with uuencode.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef VMS
#include <locale.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#define STRERROR(no)	(strerror(no))
#else
#include <errno.h>
#define MAXPATHLEN 256
#define fchmod(f,m)	(0) /* FALSE */
#include <sys/stat.h>
#define STRERROR(no)	(strerror(errno) ? strerror (errno) \
			 : (strerror (vaxc$errno) ? strerror (vaxc$errno) \
			    : "undocumented VMS error"))
#endif /* VMS */

static int decode();
static int decode_1 ();
static void usage();
char *filename;
int	verbose = 0;
int	try_multi_part = 0;
#ifdef	VMS
extern int	getopt ();
char *__progname;	/* for getopt () */
#endif	/* VMS */

/* ARGSUSED */
int
main(argc, argv)
	int argc;
	char **argv;
{
	int rval;
	int	c, nerr = 0;
#if	defined (ultrix) || defined (VMS)
	extern int optind;
#endif

#ifndef VMS
	setlocale(LC_ALL, "");
#else
	__progname = argv [0];
#endif	/* VMS */

	verbose = nerr = 0;
	while ((c = getopt(argc, argv, "mvh")) != -1)
		switch(c) {
		case 'm':
			try_multi_part ++;
			break;
		case 'v':
			verbose ++;
			break;
		case 'h':
		default:
			nerr ++;
			break;
		}
	if(nerr)
		usage();

	argc -= optind;
	argv += optind;

	if (*argv) {
		rval = 0;
		do {
			if (!freopen(filename = *argv, "r", stdin)) {
				(void)fprintf(stderr, "uudecode: %s: %s\n",
				    *argv, STRERROR(errno));
				rval = 1;
				continue;
			}
			rval |= decode();
		} while (*++argv);
	} else {
		filename = "stdin";
		rval = decode();
	}
	exit(rval);
}

static int
decode()
{
	int	nfiles = 0;
	int	nfail = 0;
	char buf[MAXPATHLEN];

	do {
		/* search for header line */
		do {
			if (!fgets(buf, sizeof(buf), stdin)) {
				if(nfiles == 0) {
					(void)fprintf(stderr,
						      "uudecode: %s: no \"begin\" line\n", filename);
					return(1);
				} else
					return 0;
			}
		} while (strncmp(buf, "begin ", 6));
		nfail += decode_1 (buf, sizeof (buf));
		nfiles ++;
	} while(try_multi_part);

	return nfail == 0 ? 0 : 1;
}

static int	unget_req = 0;
static char	kept_buf [MAXPATHLEN];

static char *
get_line (char *buf, int size)
{

	if(unget_req) {
		unget_req = 0;
		strcpy (buf, kept_buf);
		return buf;
	} else {
		char	*c = fgets (buf, size, stdin);
		if(!c)
			strcpy (buf, "\0");
		return c;
	}
}

void
unget_line (char *buf, int size)
{
	if(unget_req ++ == 0)
		strcpy (kept_buf, buf);
	else
		;	/* bug! */
}

static int
decode_1(char *buf, int size)
{
	extern int errno;
#ifndef	VMS
	struct passwd *pw;
#endif	/* VMS */
	register int n;
	register char ch, *p;
	int mode, n1;
	int maybe_last_line = 0;
	char tmpbuf [MAXPATHLEN];

	if(sscanf(buf, "begin %o %s", &mode, buf) < 2) {
		fprintf (stderr, "uudecode: %s: begin line too short: %s\n", filename, buf);
		return 1;
	}

	/* handle ~user/file format */
#ifndef VMS
	if (buf[0] == '~') {
		if (!(p = (char *) index(buf, '/'))) {
			(void)fprintf(stderr, "uudecode: %s: illegal ~user.\n",
			    filename);
			return(1);
		}
		*p++ = NULL;
		if (!(pw = getpwnam(buf + 1))) {
			(void)fprintf(stderr, "uudecode: %s: no user %s.\n",
			    filename, buf);
			return(1);
		}
		n = strlen(pw->pw_dir);
		n1 = strlen(p);
		if (n + n1 + 2 > MAXPATHLEN) {
			(void)fprintf(stderr, "uudecode: %s: path too long.\n",
			    filename);
			return(1);
		}
		bcopy(p, buf + n + 1, n1 + 1);
		bcopy(pw->pw_dir, buf, n);
		buf[n] = '/';
	}
#endif /* VMS */

	/* create output file, set mode */
	if (!freopen(buf, "w", stdout) ||
	    fchmod(fileno(stdout), mode&0666)) {
		(void)fprintf(stderr, "uudecode: %s: %s: %s\n", buf, filename,
			      STRERROR(errno));
		return(1);
	}

	if(verbose)
		fprintf (stderr, "Writing %s...", buf);

	/* for each input line */
	for (;;) {
		if (!get_line (p = buf, size)) {
			(void)fprintf(stderr, "uudecode: %s: short file.\n",
			    filename);
			return(1);
		}
#define	DEC(c)	(((c) - ' ') & 077)		/* single character decode */

		if(try_multi_part) {
			if(!strncmp(buf, "end", 3))
				break;
			else if(!strncmp(buf, "begin ", 6)) {
				fprintf (stderr, "uudecode: %s: %d: unexpected \"begin\" line.\n", filename);
				return 1;
			}
			/*
			 * Encoded lines should be:
			 *	l%4 = 0
			 *  AND
			 *	l/4 = n/3 + (n%3 == 0 ? 0 : 1)
			 *			[l > 0]
			 *	n = 0		[l = 0, should be "`\n"]
			 * Where:
			 *	n = (Char_at_column (0) - 0x20) & 0x3f
			 *	l = length (line) - 2
			 *		(1 for column 0 + 1 for tailing \n)
			 *
			 * Based on: "$Id: uuencode.c,v 1.5 1993/11/09 01:47:00 jtc Exp $"
			 */
			if(*p <= 040 || *p > 177
			   || (strlen (buf) - 2) % 4 != 0
			   || ((strlen (buf) > 4 + 2
				&& (strlen (buf) - 2) / 4 != DEC (*p) / 3 + (DEC (*p) % 3 == 0 ? 0 : 1))
			       || (strlen (buf) > 2 && strlen (buf) <= 4 + 2
					   && (strlen (buf) - 2) / 4 != DEC (*p) / 3 + (DEC (*p) % 3 == 0 ? 0 : 1))
			       || (strlen (buf) == 2 && DEC (*p) != 0))) {
				/* Look to see if created by
				 more sophisticated encoder which adjusts length of a last line. */
				if((strlen (buf) - 2) % 4 != 0
				   && ((strlen (buf) > 4 + 2
					&& ((strlen (buf) - 2) / 4 + (strlen (buf) - 2) % 4
					    == (DEC (*p) / 3 + DEC (*p) % 3 + (DEC (*p) % 3 == 0 ? 0 : 1))))
				       || (strlen (buf) > 2 && strlen (buf) <= 4 + 2
					   && (strlen (buf) - 2) / 4 == DEC (*p) + (DEC (*p) % 3 == 0 ? 0 : 1)))) {
					char	tmpbuf [MAXPATHLEN];
					if(get_line (tmpbuf, sizeof (tmpbuf))
					   && !strcmp (tmpbuf, "`\n")) { /* Bingo! */
						unget_line (tmpbuf, sizeof (tmpbuf));
						goto decode_line;
					}
				}
				if(verbose)
					fprintf (stderr, "\nSkip line (%d, %d): %.*s",
						 strlen (buf), DEC (*p), strlen (buf)-1, buf);
				continue;		/* Skip multi-part's garbage */
			}
		}
	decode_line:

		/*
		 * `n' is used to avoid writing out all the characters
		 * at the end of the file.
		 */
		if ((n = DEC(*p)) <= 0)
			break;

		for (++p; n > 0; p += 4, n -= 3)
			if (n >= 3) {
				ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
				putchar(ch);
				ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
				putchar(ch);
				ch = DEC(p[2]) << 6 | DEC(p[3]);
				putchar(ch);
			}
			else {
				if (n >= 1) {
					ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
					putchar(ch);
				}
				if (n >= 2) {
					ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
					putchar(ch);
				}
#if	0	/* Nerver used */
				if (n >= 3) {
					ch = DEC(p[2]) << 6 | DEC(p[3]);
					putchar(ch);
				}
#endif	/* 0 */
			}
	}
	if(strcmp(buf, "end\n")
	   && (!fgets(buf, size, stdin) || strcmp(buf, "end\n"))) {
		(void)fprintf(stderr, "uudecode: %s: no \"end\" line.\n",
		    filename);
		return(1);
	}
	if(verbose)
		fprintf (stderr, "Done.\n");
	return(0);
}

static void
usage()
{
	(void)fprintf(stderr, "usage: uudecode [-mvh] [file ...]\n");
	(void)fprintf(stderr, "    -m	Try multi-part extract,\n\
    \tmeans skipping the lines, which don't look like encoded lines\n\
    \tbetween \"begin\" and \"end\" line.\n");
	(void)fprintf(stderr, "    -v	verbose mode. prints skipping lines and file names.\n");
	(void)fprintf(stderr, "    -h	prints this message.\n");
	exit(1);
}
