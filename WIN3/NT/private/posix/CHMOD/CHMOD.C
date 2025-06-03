/*
 * Copyright (c) 1989 The Regents of the University of California.
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
char copyright[] =
"@(#) Copyright (c) 1989 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)chmod.c	5.19 (Berkeley) 3/12/91";
#endif /* not lint */
#ifdef DF_POSIX
#include <misc.h>
#include <bsdlib.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if WIN_NT
# include <unistd.h>
#endif

extern int errno;
int retval;

void error __P((const char *));
void usage __P((void));

#if WIN_NT
extern int globulate __P((int, int, char **));
extern void deglobulate __P((void));
extern int globulated_argc;
extern char **globulated_argv;
pid_t ppid;
int globulation;
#endif

int
#if __STDC__
main (int argc, char **argv)
#else
main(argc, argv)
	int argc;
	char **argv;
#endif
{
	extern int optind;
	register FTS *fts;
	register FTSENT *p;
	register int oct;
	mode_t omode;
	register char *mode;
#if WIN_NT
	mode_t *set;
#else
	mode_t *set, *setmode();
#endif
	struct stat sb;
	int ch, fflag, rflag;

#if WIN_NT
	ppid = getppid();
	if (ppid == (pid_t) 1) /* if parent is CMD.EXE */
	{
		globulation = globulate(1, argc, argv);
		if (globulation == 0)
		{
			argc = globulated_argc;
			argv = globulated_argv;
		}
	}
#endif
	fflag = rflag = 0;
	while ((ch = getopt(argc, argv, "Rfrwx")) != EOF)
		switch((char)ch) {
		case 'R':
			rflag = 1;
			break;
		case 'f':		/* no longer documented */
			fflag = 1;
			break;
		case 'r':		/* "-[rwx]" are valid file modes */
		case 'w':
		case 'x':
			--optind;
			goto done;
		case '?':
		default:
			usage();
		}
done:	argv += optind;
	argc -= optind;

	if (argc < 2)
		usage();

	mode = *argv;
	if (*mode >= '0' && *mode <= '7') {
		omode = (mode_t)strtol(mode, (char **)NULL, 8);
		oct = 1;
	} else {
		if (!(set = setmode(mode))) {
			(void)fprintf(stderr, "chmod: invalid file mode.\n");
#if WIN_NT
			if (ppid == (pid_t) 1 && globulation == 0)
				deglobulate();
#endif
			exit(EXIT_FAILURE);
		}
		oct = 0;
	}

	retval = 0;
	if (rflag) {
		if (!(fts = fts_open(++argv,
		    oct ? FTS_NOSTAT|FTS_PHYSICAL : FTS_PHYSICAL, 0))) {
#if 0
(void) fprintf(stderr, "strerror #1\n");
#endif
			(void)fprintf(stderr, "chmod: %s.\n", strerror(errno));
#if WIN_NT
			if (ppid == (pid_t) 1 && globulation == 0)
				deglobulate();
#endif
			exit(EXIT_FAILURE);
		}
		while ((p = fts_read(fts)) != NULL)
			switch(p->fts_info) {
			case FTS_D:
				break;
			case FTS_DNR:
			case FTS_ERR:
			case FTS_NS:
#if 0
(void) fprintf(stderr, "strerror #2\n");
#endif
				(void)fprintf(stderr, "chmod: %s: %s.\n",
				    p->fts_path, strerror(errno));
#if WIN_NT
				if (ppid == (pid_t) 1 &&
				    globulation == 0)
					deglobulate();
#endif
				exit(EXIT_FAILURE);
			default:
# if 0
(void) fprintf(stderr, "*argv: \"%s\"; omode: %lo\n", p->fts_accpath, (unsigned long)( oct ? omode :getmode(set, p->fts_statb.st_mode)));
# endif
				if (chmod(p->fts_accpath, oct ? omode :
				    getmode(set, p->fts_statb.st_mode)) &&
				    !fflag) {
#if 0
(void) fprintf(stderr, "#1\n");
#endif
					error(p->fts_path);
				}
				break;
			}
#if WIN_NT
		if (ppid == (pid_t) 1 && globulation == 0)
			deglobulate();
#endif
		exit(retval);
	}
#if 0
printf ("argv %s\n", *argv);
printf ("lstat %d\n",  lstat(*argv, &sb));
printf ("mode %d\n",  sb.st_mode);
printf ("stat %d\n",  stat("/demo/cc.exe", &sb));
printf ("mode (stat) %d\n",  sb.st_mode);
printf ("chmod %d\n",  chmod("/demo/cc.exe", 0777));
#endif

#if WIN_NT
	if (oct) {
		while (*++argv) {
# if 0
(void) fprintf(stderr, "*argv: \"%s\"; omode: %lo\n", *argv, (unsigned long) omode);
# endif
			if (chmod(*argv, omode) && !fflag) {
# if 0
(void) fprintf(stderr, "#2\n");
# endif
				error(*argv);
			}
		}
	} else
		while (*++argv)
# if WIN_NT
			if ((stat(*argv, &sb) ||
			    chmod(*argv, getmode(set, sb.st_mode))) && !fflag) {
#  if 0
(void) fprintf(stderr, "#3a\n");
#  endif
				error(*argv);
			}
# else
			if ((lstat(*argv, &sb) ||
			    chmod(*argv, getmode(set, sb.st_mode))) && !fflag) {
#   if 0
(void) fprintf(stderr, "#3b\n");
#   endif
				error(*argv);
			}
# endif /* WIN_NT */
#endif
#if WIN_NT
	if (ppid == (pid_t) 1 && globulation == 0)
		deglobulate();
#endif
	return retval;
}

void
#if __STDC__
error (const char *name)
#else
error(name)
	char *name;
#endif
{
#if 0
(void) fprintf(stderr, "strerror #3\n");
#endif
	(void)fprintf(stderr, "chmod: %s: %s.\n", name, strerror(errno));
	retval = 1;
	return;
}

void
#if __STDC__
usage (void)
#else
usage()
#endif
{
	(void)fprintf(stderr, "chmod: chmod [-R] mode file ...\n");
#if WIN_NT
	if (ppid == (pid_t) 1 && globulation == 0)
		deglobulate();
#endif
	exit(EXIT_FAILURE);
}
