24-Aug-85 04:21:37-MDT,3163;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 24 Aug 85 04:21:30-MDT
Received: from usenet by TGR.BRL.ARPA id a008341; 24 Aug 85 5:45 EDT
From: Robert Rother <rmr@sdcsvax.uucp>
Newsgroups: net.sources
Subject: A new mkdir program "nmkdir".
Message-ID: <1052@sdcsvax.UUCP>
Date: 20 Aug 85 05:13:47 GMT
To:       unix-sources@BRL-TGR.ARPA

This is a copy of a program I wrote to use in a software installation
sh script.  The only difference (I hope!) between this program and the
original mkdir is that given the command

		nmkdir	a/b/c
	
if any part of the path does not exist, it will make the appropriate
directories.  In other words, just c, b/c or a/b/c.

					Robert Rother
					Mariah Corporation

--------------------------- Cut here ------------------------------
/*
**				NMKDIR.C
**
** Written by Robert Rother, Mariah Corporation, August 1985. 
**
** I wrote this out of shear disgust with myself because I couldn't
** figure out how to do this in /bin/sh.
**
** If you want it, it's yours.  All I ask in return is that if you
** figure out how to do this in a Bourne Shell script you send me
** a copy.
**					sdcsvax!rmr or rmr@uscd
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

/*
** If you are running 4.2BSD you probably want neither of the following,
** on the other hand if your not running of 4.2BSD you probably want
** both.
*/
#define MKDIR		/* Define this if you do not have the mkdir(2) call. */
#define index	strchr  /* Define this if you do not have the index(3c) call. */

int errcnt;
int vflag;

extern int errno;

main(argc, argv)
int argc;
register char *argv[];
{

	while (*++argv) {
		if (**argv == '-' && *(*argv + 1) == 'v')
			vflag++;
		else
			mymkdir(*argv);
	}
	exit(errcnt != 0);
}

mymkdir(path)
register char *path;
{
	register char *pptr;
	char *index();

	if (!path || !*path) return;

	pptr = *path == '/' ?  path + 1 : path;

	do
	{
		if (pptr = index(pptr, '/'))
			*pptr = '\0';

		if (!access(path, 0)) {
			struct stat buf;

			if (stat(path, &buf) < 0) {
				perror(path); 
				errcnt++;
				break;
			}
			if ((buf.st_mode & S_IFMT) != S_IFDIR) {
				fprintf(stderr, "%s already exists\n", path);
				errcnt++;
				break;
			}
		} else {
			if (vflag) printf("making directory %s\n", path);
			if (mkdir(path, 0777) < 0) {
				fprintf(stderr, "mkdir: ");
				perror(path);
				errcnt++;
				break;
			}
		}
		
		if (pptr) *pptr++ = '/';
	} while (pptr);
}

#ifdef MKDIR

mkdir(dpath, dmode)
char *dpath;
int dmode;
{
	int tmp, cpid, status;
	register int (*istat)(), (*qstat)();

	if ((cpid = fork()) == 0)  {
		execl("/bin/mkdir", "mkdir", dpath, (char *)0);
		return(-1);
	}
	if (cpid != -1)  {
		istat = signal(SIGINT, SIG_IGN);
		qstat = signal(SIGQUIT, SIG_IGN);
		while((tmp = wait(&status)) != cpid)
			if (errno != EINTR) break;
		(void)signal(SIGINT, istat);
		(void)signal(SIGQUIT, qstat);
		return (tmp == -1 ? -1 : 0);
	} return(-1);
}
#endif
