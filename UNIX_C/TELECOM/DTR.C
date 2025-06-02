21-May-86 11:40:33-MDT,1821;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-SMOKE.ARPA by SIMTEL20.ARPA with TCP; Wed 21 May 86 11:40:23-MDT
Received: from USENET by SMOKE.BRL.ARPA id a015715; 20 May 86 19:45 EDT
From: "Brandon S. Allbery" <allbery@ncoast.uucp>
Newsgroups: net.sources
Subject: dtr - DTR on/off for sys3/sys5
Message-ID: <1183@ncoast.UUCP>
Date: 15 May 86 22:22:17 GMT
To:       unix-sources@brl-smoke.arpa

The following program (NOT shar'ed!) is a utility to set/reset DTR on a serial
port.  Usage is ``dtr [+-]ttyxx''.  + turns it on, - turns it off.

It's useful for telling a modem not to answer the phone after disabling a
dialin line (I have a shell script which does this automatically).

Cut and enjoy.

--Brandon

------------------------------- cut here ------------------------------------
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termio.h>

main(argc, argv)
char **argv; {
	struct termio tbuf;
	int fd;
	char ttypath[80];
	
	if (argc != 2 || (argv[1][0] != '-' && argv[1][0] != '+')) {
		write(2, "usage: dtr [+-]tty\n", 19);
		exit(1);
	}
	strcpy(ttypath, "/dev/");
	strcat(ttypath, &argv[1][1]);
	if ((fd = open(ttypath, O_RDONLY|O_NDELAY)) < 0) {
		write(2, "can't open ", 11);
		write(2, &argv[1][1], strlen(&argv[1][1]));
		write(2, "\n", 1);
		exit(2);
	}
	ioctl(fd, TCGETA, &tbuf);
	tbuf.c_cflag &= ~CBAUD;
	tbuf.c_cflag |= (argv[1][0] == '-'? B0: B300);
	ioctl(fd, TCSETA, &tbuf);
	close(fd);
	exit(0);
}


-- 
decvax!cwruecmp!ncoast!allbery  ncoast!allbery@Case.CSNET  ncoast!tdi2!brandon
(ncoast!tdi2!root for business) 6615 Center St. #A1-105, Mentor, OH 44060-4101
Phone: +01 216 974 9210      CIS 74106,1032      MCI MAIL BALLBERY (part-time)
PC UNIX/UNIX PC - which do you like best?  See <1129@ncoast.UUCP> in net.unix.
