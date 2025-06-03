/* @(#)slattach.c	1.3 (13 Oct 1993) */
/*..........................................................................
 . Copyright 1993, Jim Avera.  All Rights Reserved.
 . 
 . This program contains new material owned by the above copyright holder,
 . and may also contain portions derived from existing works used according
 . to permission granted by the owners of those works.
 .
 . You are prohibited from copying, distributing, modifying, or using this
 . file (or the portions owned by the above copyright holder) except as 
 . described in the file "COPYRIGHT" which accompanies this program.
 ...........................................................................*/

/*
 *            Copyright 1991, Intel Corporation
 *                  All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and
 * that both the copyright notice appear in all copies and that both
 * the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior premission.
 * 
 * COMPANY AND/OR INTEL DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO
 * EVENT SHALL COMPANY NOR INTEL BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

/*
 * Name:
 *		slattach
 *
 * Synopsis:
 *		slattach -a "cu-parameters" -t timeout [-p] [-i] interface_name
 *		slattach -d [-p] [-i] devname interface_name [ baudrate ]
 *		slattach - interface_name process_id
 *
 * Description:
 *		Slattach is used to assign a serial line to a network interface
 *		using the Internet Protocol.
 *
 *		-p (persistent) means use persistent links when attaching slip
 *		   to the ip driver, and exit immediately.  Otherwise the
 *		   process sleeps until interrupted (or receives SIGHUP via
 *		   the daemon), at which time the stream is dismantled.
 *
 *		-a (auto-connect) creates the slip interface without any device
 *		   linked below it.  The daemon (slipd) will auto-dial and
 *		   link devices to the slip interface on demand.  Dialout uses
 *		   the given parameter string, which contains parameters 
 *		   exactly like those used by the Taylor "cu" program.
 *		   The string may start with a background command which is run
 *		   each time the dial-up connection is established (see slipd).
 *		   This command is separated from the the cu args by a ':'.
 *
 *		-t (auto-connect idle timeout) specifies a non-default timeout
 *		   value in seconds (used only with -a).  The connection is 
 *		   droppped if no traffic flows for this period, but will be
 *		   transparently restored if needed.
 *
 *		-d (device) causes the indicated tty device to be immediately
 *		   opened and linked under the slip driver.  The resulting
 *		   interface remains available until the process exits (or
 *		   forever, with -p).  This is useful for testing and for
 *		   permanent direct links.  For dial-up connections, use -a.
 *		   [ does not use Taylor code - for backward compatibility ]
 *
 *		-i means ignore SIGHUP.  Ordinarily the process catches SIGHUP
 *		   and then exits (but with -p it exits immediately).
 *
 *		- by itself means to link the current stdin device as the 
 *		   slip device.  This is used by the "slip" program to start
 *		   a slip session from an ordinary dial-in session.
 * 
 * Author:
 *		Sudji Husodo wrote the original (1/31/91)
 *		Jim Avera (jima@netcom.com) rewrote for auto-dial support.
 *
 */

#include "../sysdep.h"
#include "../patchlevel.h"

#include <termio.h>
#include <stropts.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/stream.h>
#include <net/if.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include SLIP_HEADER

#ifdef __STDC__
#define VOLATILE volatile 
#else
#define VOLATILE
#endif

#define DEFAULT_TIMEOUT_SEC 60		/* 1 minute */

char	*program;
char	*ipname = "/dev/ip";
char	*slipname = SLIP_DEV_NAME_STR;	/* e.g., "/dev/tslip" */
char	devname[16], ifname[16];
int	fd_link, fd_ip, fd_slip, fd_dev;
int	ppid = 0;

int	LINK_cmd = I_LINK;

struct	termio	tio;
void	slsignal (int);
void 	slsignal_ok (int);

main (int argc, char *argv[])
{
	int		iflag = 0;
	int		pflag = 0;
	int		dflag = 0;
	char		*aparam = NULL;
	long		atoval = 0;
	time_t		a_timeout_sec;
	struct	strioctl siocb;
	extern	char *optarg;
	extern	int  optind;

	int	i, speed;
	pid_t	parpid;
	struct	ifreq	ifr;

	program = argv[0];

 	/* slattach -a "cu args" [-p] interface_name
 	 * slattach [-p] [-i] nodename interface_name
 	 * slattach -d [-p] [-i] devname interface_name [ baudrate ]
 	 * slattach [-p] - interface_name process_id
	 */
	while ((i = getopt (argc, argv, "a:t:pid")) != EOF)
		switch (i) {
			case 'a':	aparam = optarg;	break;
			case 't':	atoval = strtol(optarg,NULL,10); break;
			case 'p':	pflag++; LINK_cmd=I_PLINK;	break;
			case 'i':	iflag++;	break;
			case 'd':	dflag++;	break;
			case '?':	usage ();
		}
	if (optind==argc) usage();

	/*
	 * If first argument is '-' only, we're using stdin as the device
	 */
	if (0==strcmp( argv[optind], "-" )) {
 	 	/* slattach - interface_name process_id */
		if ((argc-optind) != 3) usage();
		if (aparam || atoval || dflag) usage();

 		strcpy( ifname, argv[optind+1] );
 		ppid = atoi (argv[optind+2]);
		fd_dev = fileno (stdin);

		popall(fd_dev);
		if (ioctl (fd_dev, TCGETA, &tio) < 0)
			slerror ("%s: ioctl TCGETA failed", argv[0]);

		tio.c_cflag &= CBAUD;
		tio.c_cflag |= CS8 | CREAD | HUPCL;
		tio.c_iflag = BRKINT|IGNPAR;

		if (ioctl (fd_dev, TCSETA, &tio) < 0)
			slerror ("%s: ioctl TCSANOW failed", argv[0]);
	}

	/*
	 * With -d, open direct connection
	 */
	else if (dflag) {
 	 	/* slattach -d [-p] [-i] devname interface_name [ baudrate ] */
		if ((argc-optind) < 2) usage();
		if (aparam || atoval) usage();

		if (*argv[optind] == '/') {
			strcpy (devname, argv[optind]);
		} else {
			strcpy (devname, "/dev/");
			strcat (devname, argv[optind]);
		}
		strcpy (ifname, argv[optind+1]);
		if ((argc-optind) == 3)
			speed = atoi (argv[optind+2]);

		if ((fd_dev = open (devname, O_RDWR)) < 0)
			slerror ("%s: open %s failed", argv[0], devname);

		popall(fd_dev);	/* remove ldterm and ttcompat */

		if (ioctl (fd_dev, TCGETA, &tio) < 0)
			slerror ("%s: ioctl TCGETA failed", argv[0]);

		tio.c_iflag = BRKINT | IGNPAR;
		tio.c_cflag = CS8 | CREAD | HUPCL;

		switch (speed) {
			/* n.b. low speeds mean super-high with some drivers */
			case 50:	tio.c_cflag |= B50;		break;
			case 75:	tio.c_cflag |= B75;		break;
			case 110:	tio.c_cflag |= B110;		break;
			case 1200:	tio.c_cflag |= B1200;		break;
			case 1800:	tio.c_cflag |= B1800;		break;
			case 2400:	tio.c_cflag |= B2400;		break;
			case 4800:	tio.c_cflag |= B4800;		break;
			case 9600:	tio.c_cflag |= B9600;		break;
			case 19200: 	tio.c_cflag |= B19200;		break;
			case 38400:	tio.c_cflag |= B38400;		break;
			default:	tio.c_cflag |= B9600;		break;
		}

		if (ioctl (fd_dev, TCSETA, &tio) < 0)
			slerror ("%s: ioctl TCSANOW failed", argv[0]);
	}
	/*
	 * If not stdin or -d, must be -a (we don't do dial-out here)
	 */
	else { 
		/* slattach [-p] -a "cu args" [-t timeout] interface_name */
		if (!aparam) usage();
		if (atoval) {
		    if (atoval < 0 || atoval > (24*60*60)) {
			fprintf(stderr,"%s: unreasonable timeout\n",argv[0]);
			exit(1);
		    }
		    a_timeout_sec = atoval;
		} else {
		    a_timeout_sec = DEFAULT_TIMEOUT_SEC;
		}

		if ((argc-optind) != 1) usage();

		strcpy (ifname, argv[optind]);
		fd_dev = -1;	
	}	

#ifndef SLDEBUG
	if (!pflag) {
		/*
		 * Daemonize the process if it is not a DEBUG version.
		 * If the child succeeds, it sends us SIGUSR1 and pauses;
		 * otherwise the child exits or is interrrupted.
		 * The parent's SIGUSR1 handler does exit(0).
		 */
	    	fflush(stdout); fflush(stderr);
		signal(SIGUSR1,slsignal_ok);
	    	setpgrp();
	    	switch (fork()) {
	    	case -1: 
		    perror ("fork failed"); 
		    exit (2);
	    	case 0:  
		    break; /* child */
	    	default: 
		    wait(NULL);
		    exit(1);	/* something went wrong */
	    	}
  	}
#endif

	/*
	 * the default is to catch SIGHUP, but if iflag is set ignore SIGHUP.
	 */

	if (!iflag) {
		signal (SIGHUP, slsignal);
		signal (SIGTERM, slsignal);
	} else
		signal (SIGHUP, SIG_IGN);

	/*
	 * link slip to device, open ip, link ip with fd_dev
	 */

	if ((fd_slip = open (slipname, O_RDWR)) < 0)
		slerror ("%s: open %s failed", argv[0], slipname);

	if ((fd_ip = open (ipname, O_RDWR)) < 0)
		slerror ("%s: open %s failed", argv[0], ipname);

	if (aparam) {
		/* Send the auto-dial parameters to the driver */
		slioctl(fd_slip, SLP_SET_CONNP, "SLP_SET_CONNP",
			aparam, strlen(aparam)+1);
	        slioctl(fd_slip, SLP_SET_IDLETO, "SLP_SET_IDLETO",
		        &a_timeout_sec, sizeof(a_timeout_sec));
	} else {
		/* we opened the serial port immediately */
		if (ioctl (fd_slip, LINK_cmd, fd_dev) < 0)
		    slerror ("%s: ioctl LINK %s failed", argv[0], slipname);
	}

	if ((fd_link = ioctl (fd_ip, LINK_cmd, fd_slip)) < 0)
		slerror ("%s: ioctl LINK %s failed", argv[0], ipname);

	/*
	 * send a SIOCSIFNAME (set interface name) ioctl down the stream
	 * referenced by fd_ip for the link associated with link identier
	 * fd_link specifying the name ifname
	 */

	strcpy (ifr.ifr_name,ifname);
	ifr.ifr_metric = fd_link;

/* 	/* We can't use slioctl because SIOCSIFNAME takes a funny iocb struct */
/* 	siocb.ic_cmd = SIOCSIFNAME;
/* 	siocb.ic_timout = 15;
/* 	siocb.ic_len = sizeof (ifr);
/* 	siocb.ic_dp = (char *) &ifr;
/* 	if (ioctl (fd_ip, I_STR, &siocb) < 0)
/* 	    slerror ("%s: SIOCSIFNAME (set interface name) failed", argv[0]);
 */
	slioctl(fd_ip, SIOCSIFNAME, "SIOCSIFNAME", &ifr, sizeof(ifr));

	if (!pflag) {
	    /* We forked above -- release parent process */
	    if((parpid = getppid()) != 0 && parpid != 1)
		kill (parpid, SIGUSR1);

	    pause ();	/* wait forever (holding non-persistent link) */
  	}
	return(0);
}

/*
 * Catch signal indicating that child was successful
 */
/*ARGSUSED*/
void slsignal_ok (int x)
{
	exit(0);
}

/*
 * slsignal
 */
void slsignal (int x)
{
	tio.c_cflag = HUPCL;
	ioctl(fd_dev,TCSETA,&tio);

	fprintf (stderr,"%s %d (%s): got ", program, getpid(), ifname);
	if (x==SIGHUP) 
	    fprintf (stderr,"SIGHUP");
	else
	    fprintf (stderr,"SIGNAL %d",x);

	if (ppid) {
	    fprintf (stderr,", sending SIGHUP to pid %d\n", ppid);
	    kill (ppid, SIGHUP);
	} else 
	    fprintf (stderr,"\n");

	exit (0);
}

/*
 * slerror ()
 */

slerror (s1, s2, s3)
char *s1, *s2, *s3;
{
	fprintf (stderr,s1,s2,s3);
	fflush(stderr);
	perror(":");
	exit (1);
	/*NOTREACHED*/
}

/*
 * usage ()
 */

usage ()
{
	fprintf (stderr, "%s version %s:%d\n", program, VERSION, PATCHLEVEL);
	fprintf (stderr, "\nUsage: %s [-p] -a \"cu args\" [-t timeout-seconds] interface-name\n",program);
	fprintf (stderr, "OR:    %s [-p][-i] -d device-name interface-name\n",program);
	fprintf (stderr, "OR:    %s - interface-name [process-id]\n",program);
	exit (1);
	/*NOTREACHED*/
}

popall(fd)
char fd;
{
	char curmod[BUFSIZ];

	while(ioctl(fd,I_LOOK,curmod) >= 0) {
		ioctl(fd,I_POP,curmod);
	}
}

slioctl(fd, cmd, errstring, buf, buflen)
	int fd;
	int cmd;
	char *errstring;
	char *buf;
	unsigned buflen;
{
	struct	strioctl iocb;

	iocb.ic_cmd = cmd;
	iocb.ic_timout = 15;
	iocb.ic_dp = (buflen ? buf : "");
	iocb.ic_len = buflen;
	if (ioctl (fd, I_STR, &iocb) < 0) {
  	    slerror(errstring);
	} else {
#ifdef SLDEBUG
	    fprintf(stderr,"%s: ioctl %s (OK)\n", program, errstring);
#endif
	}
}
