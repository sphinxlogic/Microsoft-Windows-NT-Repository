/*
 *  acm : an aerial combat simulator for X
 *  Copyright (C) 1991  Riley Rainey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program;  if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave., Cambridge, MA 02139, USA.
 */
 
#include "manifest.h"
#include "patchlevel.h"
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <setjmp.h>
/* Added by Greg Lehey, LEMIS, 11 May 1993, for SVR4.2 */
#ifdef __svr4__
#include <sys/filio.h>
/* We have POSIX.1 signal handling. This isn't just SVR4, of course, but _POSIX_SOURCE
 * discovers all sorts of problems with header files. (Greg Lehey, LEMIS, 11 May 1993) */
#define POSIX_SIGNALS
sigset_t null_sa_mask = {0, 0, 0, 0};			    /* null sigaction mask */
#endif

#define UPDATE_INTERVAL 1000000				    /* for debugging */

#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Revision: acm by Riley Rainey; Revision 2.4 $";
#endif

extern struct servent *getservent();
int sdebug = 1;
int listen_socket;

void parseinfo (s, a, b, c)
char *s, *a, *b, *c; {

	char	*p;

	for (p=a; *s; ++s, ++p)
		if ((*p = *s) == ' ') {
			*p = '\0';
			break;
		}

	++ s;

	for (p=b; *s; ++s, ++p)
		if ((*p = *s) == ' ') {
			*p = '\0';
			break;
		}

	++ s;

	strcpy (c, s);

	return;
}

main (argc, argv)
int	argc;
char	*argv[]; {

	struct sockaddr_in sin;
#ifdef notdef
	struct	servent *sp;
#endif
	int	on = 1;
	int	i, background = 0;

/*
 *  parse arguments
 */

	for (i=1; i<argc; ++i) {

		if (*argv[i] == '-')
			switch (*(argv[i]+1)) {

			case 'b':
				background = 1;
				break;

			default:
				fprintf (stderr, "Invalid switch \"%s\"\n", argv[i]);
				break;
			}
	}

	if (sdebug) {
#ifdef notdef
		if ((sp = getservbyname ("acm", "tcp");
			fprintf (stderr, "can't find acm service\n");
			exit (1);
		}
#endif

		if ((listen_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
			perror ("socket");
			exit (1);
		}

		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = INADDR_ANY;
		sin.sin_port = htons(ACM_PORT);

		if (bind (listen_socket, &sin, sizeof(sin)) < 0) {
			perror ("bind");
			exit (1);
		}
	}
	else {
		listen_socket = 0;		/* inetd sets this up for us */
/*		freopen ("/people/riley/acm.error", "a", stderr); */
	}

    	(void) setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR,
		(char *) &on, sizeof on);
	(void) setsockopt(listen_socket, SOL_SOCKET, SO_KEEPALIVE,
		(char *) &on, sizeof on);
	ioctl(listen_socket, FIONBIO, (char *) &on);

	if (listen (listen_socket, 5) < 0) {
		perror ("listen");
		close (listen_socket);
		exit (1);
	}

	if (background)
#ifndef linux
#ifdef SYSV
		setpgrp (1);
#else
		setpgrp (0, 1);
#endif
#else
		setpgrp ();
#endif

	printf ("\
ACM version %d.%d,  Copyright (C) 1991  Riley Rainey\n\n\
ACM comes with ABSOLUTELY NO WARRANTY.\n\
This is free software, and you are welcome to distribute it under the\n\
conditions described in the COPYING file.\n\n", REVISION, PATCHLEVEL);

	init ();
	input();

}

int peerdied = 0;

acm_sig_t deadpeer () {
	fprintf (stderr, "SIGPIPE\n");
	peerdied = 1;
#ifndef SIGVOID
	return(0);
#endif
}

#ifndef linux
#ifdef POSIX_SIGNALS
static struct	sigaction alrm, pipe;
#else							    /* not linux, not POSIX */
static struct	sigvec	alrm, pipe;
#endif
#endif
int doUpdate = 0;

#ifndef linux
acm_sig_t myalarm () {
	doUpdate++;
#ifdef POSIX_SIGNALS
	sigaction (SIGALRM, &alrm, (struct sigaction *) 0);
#else
	sigvec (SIGALRM, &alrm, (struct sigvec *) 0);
#endif
#ifndef SIGVOID
	return(0);
#endif
}
#endif

#ifdef linux
void myalarm() {
	doUpdate++;
	signal (SIGALRM, myalarm);
}
#endif

input () {

	fd_set	fdset, ofdset;
	int	news = -1, playerchange = 0, n, pno, addrlen;
	int	on = 1;
	struct	sockaddr addr;
	struct	itimerval update;
	char	*bp, buf[128], name[64], display[64], args[128];

#ifdef POSIX_SIGNALS
	alrm.sa_handler = myalarm;
	alrm.sa_mask = null_sa_mask;
	alrm.sa_flags = 0;
	if (sigaction (SIGALRM, &alrm, (struct sigaction *) 0) < 0)
	  {
	  printf ("Internal signal problem\n");
	  exit (1);
	  }
#else							    /* not POSIX.1 */
#ifndef linux
	alrm.sv_handler = myalarm;
	alrm.sv_mask = 0;
#ifdef hpux
	alrm.sv_flags = SV_BSDSIG;
#else							    /* ndef hpux */
	alrm.sv_flags = SV_INTERRUPT;
#endif							    /* ndef hpux */
	sigvec (SIGALRM, &alrm, (struct sigvec *) 0);
#else							    /* linux */
	signal (SIGALRM, myalarm);
#endif
#endif

/*
 *  Set real time clock to interrupt us every UPDATE_INTERVAL usecs.
 */

	update.it_interval.tv_sec = 0;
	update.it_interval.tv_usec = UPDATE_INTERVAL;
	update.it_value.tv_sec = 0;
	update.it_value.tv_usec = UPDATE_INTERVAL;
	setitimer (ITIMER_REAL, &update, 0);

#ifdef POSIX_SIGNALS
	pipe.sa_handler = SIG_DFL;
	pipe.sa_mask = null_sa_mask;
	pipe.sa_flags = 0;
	sigaction (SIGPIPE, &pipe, (struct sigaction *) 0);
#else							    /* not POSIX.1 */
#ifndef linux
	pipe.sv_handler = SIG_DFL;
	pipe.sv_mask = 0;
#ifdef hpux
	pipe.sv_flags = SV_BSDSIG;
#else
	pipe.sv_flags = SV_INTERRUPT;
#endif
	sigvec (SIGPIPE, &pipe, (struct sigvec *) 0);
#endif
#endif

	FD_ZERO (&ofdset);
	FD_ZERO (&fdset);
	FD_SET (listen_socket, &ofdset);

	for (;;) {

#ifdef POSIX_SIGNALS
		sigset_t sig_mask;			    /* get a signal mask */
		sigemptyset (&sig_mask);		    /* empty it */
		sigprocmask (SIG_BLOCK, &sig_mask, (sigset_t *) NULL); /* and allow all signals */
		sigaddset (&sig_mask, SIGALRM);		    /* add this signal */
#else
		sigsetmask (0);
#endif

		fdset = ofdset;
		pno = select (32, &fdset, (fd_set *) NULL, (fd_set *) NULL,
			(struct itimerval *) NULL);

#ifdef POSIX_SIGNALS
		sigprocmask (SIG_BLOCK, &sig_mask, (sigset_t *) NULL); /* and disallow it */
#else
		sigblock (sigmask(SIGALRM));
#endif

		if (pno < 0) {
			FD_CLR (listen_socket, &fdset);
			if (news > 0)
				FD_CLR (news, &fdset);
		}

		if (FD_ISSET (listen_socket, &fdset) ||
			(news > 0 && FD_ISSET(news, &fdset))) {
			if (news == -1) {
				addrlen = sizeof (addr);
				news = accept(listen_socket, &addr, &addrlen);
				if (news > 0) {
					peerdied = 0;
#ifndef linux
#ifdef POSIX_SIGNALS
					pipe.sa_handler = deadpeer;
					sigaction (SIGPIPE, &pipe, (struct sigaction *) 0);
#else
					pipe.sv_handler = deadpeer;
					sigvec(SIGPIPE, &pipe, (struct sigvec *) 0);
#endif
#endif
					ioctl (news, FIONBIO, &on);
					FD_SET (news, &ofdset);
					bp = buf;
				}
			}
			if (news > 0 && FD_ISSET (news, &fdset)) {
				if ((n = read (news, bp, 1)) > 0) {
					if (*bp == '\n') {
						*bp = '\0';
						parseinfo (buf, display,
							name, args);
						if (newPlayer (news,
							display, name, args) == 0)
							write (news, "Ready.\n", 7);
							printf ("%s\n", display);
						close (news);
						news = -1;
					}
					else
						bp++;
					playerchange = 1;
				}
				if (n == 0)
					peerdied = 1;
			}
		}

		if (playerchange) {
			FD_ZERO (&ofdset);
			FD_SET (listen_socket, &ofdset);
			if (news >= 0)
				FD_SET (news, &ofdset);
#ifndef linux
#ifdef POSIX_SIGNALS
			pipe.sa_handler = SIG_DFL;
			sigaction (SIGPIPE, &pipe, (struct sigaction *) 0);
#else
			pipe.sv_handler = SIG_DFL;
			sigvec (SIGPIPE, &pipe, (struct sigvec *) 0);
#endif
#endif
			playerchange = 0;
		}

		if (doUpdate) {
			doUpdate = 0;
			redraw ();
		}

	}
}
