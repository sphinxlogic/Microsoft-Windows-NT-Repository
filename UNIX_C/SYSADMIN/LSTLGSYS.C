From pur-ee!j.cc.purdue.edu!i.cc.purdue.edu!purdue!gatech!udel!rochester!bbn!husc6!necntc!ncoast!allbery Mon Mar  7 08:32:19 EST 1988

Comp.sources.misc: Volume 2, Issue 65
Submitted-By: "Lenny Tropiano" <lenny@icus.UUCP>
Archive-Name: lastlog-sys5

In article <9766@shemp.CS.UCLA.EDU> jimmy@pic.ucla.edu (Jim Gottlieb) writes:
|>I notice that SystemV (at least the SystemV on my AT&T 3B1) doesn't
|>have a /usr/adm/lastlog or any program that performs a similar
|>function.  This prevents the use of a last(1) command and the ability
|>to see the last login in the finger program.  I could keep /etc/wtmp
|>around forever, but that gets too big.  Any suggestions/solutions?
|>
|>Thanks...

Here's my implementation to lastlogin ... 

--- cut here --- --- cut here --- --- cut here --- --- cut here ---

/***************************************************************************
 * Program:  lastlogin 			(c)1987 ICUS Computer Group        *
 * By:       Lenny Tropiano		...{ihnp4,mtune}!icus!lenny        *
 *                                                                         *
 * Program intent:   This will allow programs like 'finger' and 'last' to  *
 *                   lookup in the file /usr/adm/lastlogin.log and see     *
 *                   when a particular user has logged-in.   This saves    *
 *                   the necessity to keep /etc/wtmp around for a long     *
 *                   period of time.                                       *
 *                                                                         *
 *                   This program can be used/modified and redistributed   *
 *                   I declare it PUBLIC DOMAIN.  Please give me credit    *
 *                   when credit is due.                                   *
 *                                                                         *
 *      AT&T 3B1 compiling instructions for shared-libaries:               *
 *                                                                         *
 *      $ cc -c -O lastlogin.c                                             *
 *      $ ld -s -o lastlogin lastlogin.o /lib/shlib.ifile /lib/crt0s.o     *
 *      $ mv lastlogin /etc                                                *
 *      $ su                                                               *
 *      Password:                                                          *
 *      # chown adm /etc/lastlogin /usr/adm                                *
 *      # chgrp adm /etc/lastlogin /usr/adm                                *
 *      # chmod 4755 /etc/lastlogin                                        *
 *                                                                         *
 *      Place a call to /etc/lastlogin in your /etc/localprofile           *
 *      to be run on all user logins.                                      *
 ***************************************************************************/

              /* Print the last login time and record the new time */

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <utmp.h>

#define	LOGFILE	"/usr/adm/lastlogin.log"

main()
{
	struct utmp *utent, *getutent();
	int    fd;
	long   hrs, min, sec;
	struct lastlog {
	   char ll_line[8];
	   time_t ll_time;
	} ll;

	if (access(LOGFILE, 0) == -1) {
	   if ((fd = creat(LOGFILE,0644)) == -1) {
		fprintf(stderr,"Cannot create file %s: ", LOGFILE);
		perror("creat()");
		exit(1);
	   }
	} else {
	   if ((fd = open(LOGFILE,O_RDWR)) == -1) {
		fprintf(stderr,"Cannot open file %s: ", LOGFILE);
		perror("open()");
		exit(1);
	   }
	}

	if (lseek(fd, (long)(getuid()*sizeof(struct lastlog)), 0) == -1) {
		fprintf(stderr,"Cannot position file %s: ", LOGFILE);
		perror("lseek()");
		exit(1);
	}

	if (read(fd, (char *) &ll, sizeof ll) == sizeof ll &&
	    ll.ll_time != 0L) {
		printf("Last login: %.*s on %.*s\n" , 19
			, (char *) ctime(&ll.ll_time) , sizeof(ll.ll_line)
			, ll.ll_line);
	} else  printf("Last login: [No Login information on record]\n");

	sprintf(ll.ll_line, "%.8s", strrchr(ttyname(0), '/')+1);
	setutent();
	while ((utent = getutent()) != NULL) 
	   if (strcmp(utent->ut_line, ll.ll_line) == 0)
		break;

	if (utent == NULL) {
		fprintf(stderr,"Cannot locate utmp entry for tty\n");
		exit(1);
	}
	ll.ll_time = utent->ut_time;
	endutent();

	lseek(fd, (long)(getuid()*sizeof(struct lastlog)), 0);
	write(fd, (char *) &ll, sizeof ll);
	close(fd);

	exit(0);
}


-- 
US MAIL  : Lenny Tropiano, ICUS Computer Group        IIIII  CCC U   U  SSS
           PO Box 1                                     I   C    U   U S
	   Islip Terrace, New York  11752               I   C    U   U  SS 
PHONE    : (516) 968-8576 [H] (516) 582-5525 [W]        I   C    U   U    S
TELEX    : 154232428 [ICUS]                           IIIII  CCC  UUU  SSS 
AT&T MAIL: ...attmail!icus!lenny  
UUCP     : ...{mtune, ihnp4, boulder, talcott, sbcs, bc-cis}!icus!lenny 


