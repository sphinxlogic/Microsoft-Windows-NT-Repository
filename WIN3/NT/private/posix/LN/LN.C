/*
 * Copyright (c) 1987 Regents of the University of California.
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

#ifdef DF_POSIX /* DF_DSC */
#include <misc.h>
#include <bsdlib.h>
#endif

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1987 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)ln.c	4.15 (Berkeley) 2/24/91";
#endif /* not lint */

#ifdef DF_POSIX /* DF_DSC */
#include <sys/cdefs.h>
#else
#include <sys/param.h>
#endif
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int	dirflag,			/* undocumented force flag */
#ifndef _POSIX_SOURCE
		sflag,				/* symbolic, not hard, link */
#endif
		(*linkf) __P((const char *, const char *));	/* system link call */
static int linkit __P((char *, char *, int));
static void usage __P((void));

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
	struct stat buf;
#ifdef _POSIX_SOURCE /* DF_MSS */
	int ch, exitval;
#else
	int ch, exitval, link(), symlink();
#endif
	char *sourcedir;

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

	while ((ch = getopt(argc, argv, "Fs")) != EOF)
		switch((char)ch) {
		case 'F':
			dirflag = 1;
			break;
#ifndef _POSIX_SOURCE /* DF_MSS */
		case 's':
			sflag = 1;
			break;
#endif
		case '?':
		default:
			usage();
		}

	argv += optind;
	argc -= optind;

#ifdef _POSIX_SOURCE /* DF_MSS */
	linkf = link;
#else
	linkf = sflag ? symlink : link;
#endif

	switch(argc) {
	case 0:
		usage();
	case 1:				/* ln target */
		exitval = linkit(argv[0], ".", 1);
		break;
	case 2:				/* ln target source */
		exitval = linkit(argv[0], argv[1], 0);
		break;
	default:			/* ln target1 target2 directory */
		sourcedir = argv[argc - 1];
		if (stat(sourcedir, &buf)) {
			(void)fprintf(stderr,
			    "ln: %s: %s\n", sourcedir, strerror(errno));
#if WIN_NT
			if (ppid == (pid_t) 1 && globulation == 0)
				deglobulate();
#endif
			exit(EXIT_FAILURE);
		}
		if (!S_ISDIR(buf.st_mode))
			usage();
		for (exitval = EXIT_SUCCESS; *argv != sourcedir; ++argv)
			exitval |= linkit(*argv, sourcedir, 1);
		break;
	}
#if WIN_NT
	if (ppid == (pid_t) 1 && globulation == 0)
		deglobulate();
#endif
	return exitval;
}

static int
#if __STDC__
linkit (char *target, char *source, int isdir)
#else
linkit(target, source, isdir)
	char *target, *source;
	int isdir;
#endif
{
	struct stat buf;
	char path[MAXPATHLEN], *cp;

#ifndef _POSIX_SOURCE
	if (!sflag) {
#endif
		/* if target doesn't exist, quit now */
		if (stat(target, &buf)) {
			(void)fprintf(stderr,
			    "ln: %s: %s\n", target, strerror(errno));
			return(EXIT_FAILURE);
		}
		/* only symbolic links to directories, unless -F option used */
		if (!dirflag && (buf.st_mode & S_IFMT) == S_IFDIR) {
			(void)printf("ln: %s is a directory.\n", target);
			return(EXIT_FAILURE);
		}
#ifndef _POSIX_SOURCE
	}
#endif

	/* if the source is a directory, append the target's name */
	if (isdir || !stat(source, &buf) && (buf.st_mode & S_IFMT) == S_IFDIR) {
		if (!(cp = rindex(target, '/')))
			cp = target;
		else
			++cp;
		(void)sprintf(path, "%s/%s", source, cp);
		source = path;
	}
#if 0 && DF_POSIX
printf ("linkit %s %s\n", target, source);
#endif
	if ((*linkf)(target, source)) {
		(void)fprintf(stderr, "ln: %s: %s\n", source, strerror(errno));
		return(EXIT_FAILURE);
	}
	return(EXIT_SUCCESS);
}

static void
#if __STDC__
usage (void)
#else
usage()
#endif
{
	(void)fprintf(stderr,
#ifdef _POSIX_SOURCE /* DF_MSS */
	    "usage:\tln file1 file2\n\tln file ... directory\n");
#else
	    "usage:\tln [-s] file1 file2\n\tln [-s] file ... directory\n");
#endif
#if WIN_NT
	if (ppid == (pid_t) 1 && globulation == 0)
		deglobulate();
#endif
	exit(EXIT_FAILURE);
}
