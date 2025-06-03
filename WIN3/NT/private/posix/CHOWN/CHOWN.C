/*
 * Copyright (c) 1988 Regents of the University of California.
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
"@(#) Copyright (c) 1988 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)chown.c	5.18 (Berkeley) 3/9/91";
#endif /* not lint */

#ifdef DF_POSIX /* DF_DSC */
#include <misc.h>
#include <bsdlib.h>
#else
#include <sys/param.h>
#endif
#include <sys/types.h>	/* FOR calls to stat */
#include <sys/stat.h>
#include <sys/errno.h>
#include <dirent.h>
#include <fts.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#if WIN_NT

void a_gid __P((register char *));
void a_uid __P((register char *));
void chownerr __P((char *));
void error __P((char *));
void usage __P((void));
void setids __P((char *));

extern int globulate __P((int, int, char **));
extern void deglobulate __P((void));
extern int globulated_argc;
extern char **globulated_argv;
pid_t ppid;
int globulation;
#endif

int ischown, fflag, rflag, retval;
uid_t uid,t_uid;
gid_t gid,t_gid;
int setid_flg;		/* Flg to do STAT */
char *gname, *myname;

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
	register char *cp;
	int ch;

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
	retval = EXIT_SUCCESS;
	myname = ((cp = rindex(*argv, '/')) != NULL) ? cp + 1 : *argv;
	ischown = myname[2] == 'o';

	while ((ch = getopt(argc, argv, "Rf")) != EOF)
		switch((char)ch) {
		case 'R':
			rflag = 1;
			break;
		case 'f':
			fflag = 1;
			break;
		case '?':
		default:
			usage();
		}
	argv += optind;
	argc -= optind;

	if (argc < 2)
		usage();

	uid = (uid_t)-1;
	gid = (gid_t)-1;

	if (ischown) {
#ifdef SUPPORT_DOT
		if (cp = index(*argv, '.')) {
			*cp++ = '\0';
			a_gid(cp);
		} else
#endif
		if ((cp = index(*argv, ':')) != NULL) {
			*cp++ = '\0';
			a_gid(cp);
		} 
		a_uid(*argv);
	}
	else 
		a_gid(*argv);

/* Added to take care of retaining grp/usr association if not specified */
	t_uid = uid;
	t_gid = gid;
	setid_flg = ( int ) uid == -1 || ( int ) gid == -1;

	if (rflag) {
		if (!(fts = fts_open(++argv, FTS_NOSTAT|FTS_PHYSICAL, 0))) {
			(void)fprintf(stderr,
			    "%s: %s.\n", myname, strerror(errno));
#if WIN_NT
			if (ppid == (pid_t) 1 && globulation == 0)
				deglobulate();
#endif
			exit(EXIT_FAILURE);
		}
		while ((p = fts_read(fts)) != NULL) {
			if (p->fts_info == FTS_D)
				continue;
			if (p->fts_info == FTS_ERR) {
				error(p->fts_path);
				continue;
			}
			setids(p->fts_accpath);
			if (chown(p->fts_accpath, uid, gid) && !fflag)
				chownerr(p->fts_path);
		}
#if WIN_NT
		if (ppid == (pid_t) 1 && globulation == 0)
			deglobulate();
#endif
		exit(retval);
	}

	while (*++argv) {
		setids(*argv);
		if (chown(*argv, uid, gid) && !fflag)
			chownerr(*argv);
	}
#if WIN_NT
	if (ppid == (pid_t) 1 && globulation == 0)
		deglobulate();
#endif
	return retval;
}

void
#if __STDC__
a_gid (register char *s)
#else
a_gid(s)
	register char *s;
#endif
{
	struct group *gr;

	if (!*s) {
		gid = (gid_t)-1;	/* argument was "uid." */
		return;
	}
	gname = s;
	if ((gr = getgrnam(s)) != NULL)
		gid = gr->gr_gid;
	else {
		for (; *s && isdigit(*s); ++s);
		if (!*s)
			gid = (gid_t)atoi(gname);
		else {
			(void)fprintf(stderr, "%s: unknown group id: %s\n",
			    myname, gname);
#if WIN_NT
			if (ppid == (pid_t) 1 && globulation == 0)
				deglobulate();
#endif
			exit(EXIT_FAILURE);
		}
	}
}

void
#if __STDC__
a_uid (register char *s)
#else
a_uid(s)
	register char *s;
#endif
{
	struct passwd *pw;
	char *uname;

	if (!*s) {
		uid = (uid_t)-1;		/* argument was ".gid" */
		return;
	}
	if ((pw = getpwnam(s)) != NULL)
		uid = pw->pw_uid;
	else {
		for (uname = s; *s && isdigit(*s); ++s);
		if (!*s)
			uid = (uid_t)atoi(uname);
		else {
			(void)fprintf(stderr,
			    "chown: unknown user id: %s\n", uname);
#if WIN_NT
			if (ppid == (pid_t) 1 && globulation == 0)
				deglobulate();
#endif
			exit(EXIT_FAILURE);
		}
	}
}

void
#if __STDC__
chownerr (char *file)
#else
chownerr(file)
	char *file;
#endif
{
	static uid_t euid = (uid_t)-1;
	static int ngroups = -1;

	/* check for chown without being root */
	if (errno != EPERM || uid != (uid_t)-1 && euid == (uid_t)-1 && (euid = geteuid()) != 0) {
		if (fflag) {
#if WIN_NT
			if (ppid == (pid_t) 1 && globulation == 0)
				deglobulate();
#endif
			exit(EXIT_SUCCESS);
		}
		error(file);
#if WIN_NT
		if (ppid == (pid_t) 1 && globulation == 0)
			deglobulate();
#endif
		exit(EXIT_FAILURE);
	}
	/* check group membership; kernel just returns EPERM */
	if (gid != -1 && ngroups == -1) {
#if DF_POSIX /* DF_MSS: NGROUPS_MAX is defined as 0 in Posix headers */
# if NGROUPS_MAX < 1
		gid_t groups[1];
# else
		gid_t groups[NGROUPS_MAX];
# endif
#else
		gid_t groups[NGROUPS_MAX];
#endif

		ngroups = getgroups(NGROUPS_MAX, groups);
		while (--ngroups >= 0 && gid != groups[ngroups]);
		if (ngroups < 0) {
			if (fflag) {
#if WIN_NT
				if (ppid == (pid_t) 1 && globulation == 0)
					deglobulate();
#endif
				exit(EXIT_SUCCESS);
			}
			(void)fprintf(stderr,
			    "%s: you are not a member of group %s.\n",
			    myname, gname);
#if WIN_NT
			if (ppid == (pid_t) 1 && globulation == 0)
				deglobulate();
#endif
			exit(EXIT_FAILURE);
		}
	}
	if (!fflag)
		error(file);
}

void
#if __STDC__
error (char *name)
#else
error(name)
	char *name;
#endif
{
	(void)fprintf(stderr, "%s: %s: %s\n", myname, name, strerror(errno));
	retval = EXIT_FAILURE;
}

void
#if __STDC__
usage (void)
#else
usage()
#endif
{
	(void)fprintf(stderr, "usage: %s [-Rf] %s file ...\n", myname,
	    ischown ? "[owner][:group]" : "group");
#if WIN_NT
	if (ppid == (pid_t) 1 && globulation == 0)
		deglobulate();
#endif
	exit(EXIT_FAILURE);
}

void
#if __STDC__
setids (char *fname)
#else
setids ( fname )
	char *fname;
#endif
{
	struct stat buf;

	if ( setid_flg )
		if ( stat( fname , &buf ) == 0 )
		{
			if ( t_uid == ( int ) -1 )
				uid = buf.st_uid;
			if ( t_gid == ( int ) -1 )
				gid = buf.st_gid;
		}
}
