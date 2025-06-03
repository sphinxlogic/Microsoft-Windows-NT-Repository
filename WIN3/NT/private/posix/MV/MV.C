/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ken Smith of The State University of New York at Buffalo.
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

#ifdef DF_POSIX /* DF_MSS */
#include <misc.h>
#include <bsdlib.h>
#endif

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1989 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)mv.c	5.11 (Berkeley) 4/3/91";
#endif /* not lint */
#ifdef DF_POSIX
#include <sys/cdefs.h>
#else /* DF_DSC: not needed by POSIX */
#include <sys/param.h>
#endif
#ifdef _POSIX_SOURCE
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pathnames.h"

#if WIN_NT
int do_move __P((char *, char *));
int fastcopy __P((char *, char *, struct stat *));
int copy __P((char *, char *));
void error __P((char *));
void usage __P((void));
extern int globulate __P((int, int, char **));
extern void deglobulate __P((void));
extern int globulated_argc;
extern char **globulated_argv;
pid_t ppid;
int globulation;
#endif

int fflg, iflg;

int
#if __STDC__
main (int argc, char **argv)
#else
main(argc, argv)
	int argc;
	char **argv;
#endif
{
	extern char *optarg;
	extern int optind;
	register int baselen, exitval, len;
	register char *p, *endp;
	struct stat sb;
	int ch;
#ifdef _POSIX_SOURCE
	char path[PATH_MAX +1];
#else
	char path[MAXPATHLEN + 1];
#endif

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
	while (((ch = getopt(argc, argv, "-if")) != EOF))
		switch((char)ch) {
		case 'i':
			iflg = 1;
			break;
		case 'f':
			fflg = 1;
			break;
		case '-':		/* undocumented; for compatibility */
			goto endarg;
		case '?':
		default:
			usage();
		}
endarg:	argc -= optind;
	argv += optind;

	if (argc < 2)
		usage();

	/*
	 * If the stat on the target fails or the target isn't a directory,
	 * try the move.  More than 2 arguments is an error in this case.
	 */
	if (stat(argv[argc - 1], &sb) || !S_ISDIR(sb.st_mode)) {
		if (argc > 2)
			usage();
#if WIN_NT
		exitval = do_move(argv[0], argv[1]);
		if (ppid == (pid_t) 1 && globulation == 0)
			deglobulate();
		exit(exitval);
#else
		exit(do_move(argv[0], argv[1]));
#endif
	}

	/* It's a directory, move each file into it. */
	(void)strcpy(path, argv[argc - 1]);
	baselen = strlen(path);
	endp = &path[baselen];
	*endp++ = '/';
	++baselen;
	for (exitval = EXIT_SUCCESS; --argc; ++argv) {
		if ((p = rindex(*argv, '/')) == NULL)
			p = *argv;
		else
			++p;
#ifdef _POSIX_SOURCE
		if ((baselen + (len = strlen(p))) >= PATH_MAX)
#else
		if ((baselen + (len = strlen(p))) >= MAXPATHLEN)
#endif
			(void)fprintf(stderr,
			    "mv: %s: destination pathname too long\n", *argv);
		else {
			bcopy(p, endp, len + 1);
			exitval |= do_move(*argv, path);
		}
	}
#if WIN_NT
	if (ppid == (pid_t) 1 && globulation == 0)
		deglobulate();
#endif
	return exitval;
}

int
#if __STDC__
do_move (char *from, char *to)
#else
do_move(from, to)
	char *from, *to;
#endif
{
	struct stat sb;
	int ask, ch;

	/*
	 * Check access.  If interactive and file exists, ask user if it
	 * should be replaced.  Otherwise if file exists but isn't writable
	 * make sure the user wants to clobber it.
	 */
	if (!fflg && !access(to, F_OK)) {
		ask = 0;
		if (iflg) {
			(void)fprintf(stderr, "overwrite %s? ", to);
			ask = 1;
		}
		else if (access(to, W_OK) && !stat(to, &sb)) {
			(void)fprintf(stderr, "override mode %o on %s? ",
			    sb.st_mode & 07777, to);
			ask = 1;
		}
		if (ask) {
			if ((ch = getchar()) != EOF && ch != '\n')
				while (getchar() != '\n');
			if (ch != 'y')
				return(EXIT_SUCCESS);
		}
	}
	if (!rename(from, to))
		return(EXIT_SUCCESS);

	if (errno != EXDEV) {
		(void)fprintf(stderr,
		    "mv: rename %s to %s: %s\n", from, to, strerror(errno));
		return(EXIT_FAILURE);
	}

	/*
	 * If rename fails, and it's a regular file, do the copy internally;
	 * otherwise, use cp and rm.
	 */
	if (stat(from, &sb)) {
		(void)fprintf(stderr, "mv: %s: %s\n", from, strerror(errno));
		return(EXIT_FAILURE);
	}
	return(S_ISREG(sb.st_mode) ?
	    fastcopy(from, to, &sb) : copy(from, to));
}

int
#if __STDC__
fastcopy (char *from, char *to, struct stat *sbp)
#else
fastcopy(from, to, sbp)
	char *from, *to;
	struct stat *sbp;
#endif
{
	struct timeval tval[2];
	static u_int blen;
	static char *bp;
	register int nread, from_fd, to_fd;

	if ((from_fd = open(from, O_RDONLY, 0)) < 0) {
		error(from);
		return(EXIT_FAILURE);
	}
	if ((to_fd = open(to, O_CREAT|O_TRUNC|O_WRONLY, sbp->st_mode)) < 0) {
		error(to);
		(void)close(from_fd);
		return(EXIT_FAILURE);
	}
/* DF_MSS: st_blksize is not part of the STAT struct; defining preferred blksize to 1024 */
#ifdef DF_POSIX /* DF_MSS */
        if (!blen && !(bp = malloc(blen = /*sbp->st_blksize*/ 1024))) {
#else
        if (!blen && !(bp = malloc(blen = sbp->st_blksize))) {
#endif
		error(NULL);
		return(EXIT_FAILURE);
	}
	while ((nread = read(from_fd, bp, blen)) > 0)
		if (write(to_fd, bp, nread) != nread) {
			error(to);
			goto err;
		}
	if (nread < 0) {
		error(from);
err:		(void)unlink(to);
		(void)close(from_fd);
		(void)close(to_fd);
		return(EXIT_FAILURE);
	}
#ifdef _POSIX_SOURCE /* DF_MSS */
	(void)chown(to, sbp->st_uid, sbp->st_gid);
	(void)chmod(to, sbp->st_mode);
#else
	(void)fchown(to_fd, sbp->st_uid, sbp->st_gid);
	(void)fchmod(to_fd, sbp->st_mode);
#endif

	(void)close(from_fd);
	(void)close(to_fd);

	tval[0].tv_sec = sbp->st_atime;
	tval[1].tv_sec = sbp->st_mtime;
	tval[0].tv_usec = tval[1].tv_usec = 0;
	(void)utimes(to, tval);
	(void)unlink(from);
	return(EXIT_SUCCESS);
}

int
#if __STDC__
copy (char *from, char *to)
#else
copy(from, to)
	char *from, *to;
#endif
{
#if _POSIX_SOURCE
	pid_t pid;
	int status;
#else
	int pid, status;
#endif

#ifdef _POSIX_SOURCE /* DF_MSS */
	if (!(pid = fork())) {
#else
	if (!(pid = vfork())) {
#endif
		execl(_PATH_CP, "mv", "-pr", from, to, NULL);
		error(_PATH_CP);
#if WIN_NT
		if (ppid == (pid_t) 1 && globulation == 0)
			deglobulate();
#endif
		_exit(EXIT_FAILURE);
	}
	(void)waitpid(pid, &status, 0);
	if (!WIFEXITED(status) || WEXITSTATUS(status))
		return(EXIT_FAILURE);
#ifdef _POSIX_SOURCE /* DF_MSS */
	if (!(pid = fork())) {
#else
	if (!(pid = vfork())) {
#endif
		execl(_PATH_RM, "mv", "-rf", from, NULL);
		error(_PATH_RM);
#if WIN_NT
		if (ppid == (pid_t) 1 && globulation == 0)
			deglobulate();
#endif
		_exit(EXIT_FAILURE);
	}
	(void)waitpid(pid, &status, 0);
	return(!WIFEXITED(status) || WEXITSTATUS(status));
}

void
#if __STDC__
error (char *s)
#else
error(s)
	char *s;
#endif
{
	if (s)
		(void)fprintf(stderr, "mv: %s: %s\n", s, strerror(errno));
	else
		(void)fprintf(stderr, "mv: %s\n", strerror(errno));
}

void
#if __STDC__
usage (void)
#else
usage()
#endif
{
	(void)fprintf(stderr,
"usage: mv [-if] src target;\n   or: mv [-if] src1 ... srcN directory\n");
#if WIN_NT
	if (ppid == (pid_t) 1 && globulation == 0)
		deglobulate();
#endif
	exit(EXIT_FAILURE);
}
