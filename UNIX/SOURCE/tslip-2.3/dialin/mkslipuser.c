/* TSLIP: @(#)mkslipuser.c	1.1 (11 Oct 1993) */
/*
 * mkslipuser.c
 *
 * Creates a blank user file based on a description of the network
 * configuration.  Initializes the dialup slip system.
 *
 * Copyright 1987 by University of California, Davis
 *
 * Greg Whitehead 10-1-87
 * Computing Services
 * University of California, Davis
 */

/*
 * This program creates the dialup slip USER_FL based on information supplied
 * either on the command line or in a configuration file. The configuration
 * file consists of a number of one-line entries; one for each simultaneous
 * login that is allowed. The number of simultaneous logins must, of course,
 * be less than or equal to the number of interfaces available. Each one-line
 * entry contains an internet address (a.b.c.d or hostname format) for the
 * server side of the slip line. They may all be the same, or they may be
 * different, depending on how you prefer to administer your network. The
 * configuration file may also contain comments (lines starting with a #). In
 * the case where all of the server side addresses are the same, command line
 * arguments may be used instead of the configuration file. The first argument
 * is the number of simultaneous logins to allow, and the second argument is
 * the address of the server side of the point-point link.
 *
 * Modified by Geoff Arnold, Sun Microsystems 10-21-87:
 *
 * Allow a hostname instead of a dotted internet address.
 *
 */
 /* Revised for tslip (started): Jim Avera 9/8/93
  */

#include "../sysdep.h"

#include <ctype.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#if HAVE_RESOLVER
#include <arpa/nameser.h>
#include <resolv.h>
#endif
#include "slip.h"

main(argc,argv)
int argc;
char **argv;
{
    FILE *cfd;
    char cline[80];
    int ufd;
    struct sl_urec urec;
    int x;

 
#if HAVE_RESOLVER
     /* Use a TCP link in case named is not running. ie: dont hang the boot
        scripts for 15 minutes that it takes to time out with UDP 
	(suggested by Peter Wemm) */
     _res.options |= RES_USEVC;
#endif

    /*
     * Open CONFIG_FL if the neccessary arguments aren't available on the
     * command line. If the file isn't there either then give usage.
     *
     */
    if (argc<3 && (cfd=fopen(CONFIG_FL,"r"))==NULL) {
	    fprintf(stderr,"usage: %s [count address]  (reads \"%s\" on default)",argv[0],CONFIG_FL);
	    exit(1);
	}


    /*
     * Open USER_FL.
     *
     */
    if ((ufd=open(USER_FL,O_WRONLY|O_CREAT|O_TRUNC,0644))<0) {
	perror(USER_FL);
	exit(1);
    }


    /*
     * create USER_FL.
     *
     */
    urec.sl_uid = -1;

    if (argc<3) {
	while (fgets(cline,80,cfd)!=NULL)
	    if (*cline!='#')
		if (write_urec(ufd,urec,cline)<0) {
		    fclose(cfd);
		    exit(1);
		}
        fclose(cfd);
    }

    else {
	for (x=0;x<atoi(argv[1]);x++) 
	    if (write_urec(ufd,urec,argv[2])<0)
		exit(1);
    }

    close(ufd);
}


write_urec(ufd,urec,addr)
int ufd;
struct sl_urec urec;
char *addr;
{
    struct hostent *h;
    char *c;

    if (c = strchr(addr, '\n'))			/* zap the newline */
	*c = '\0';

    if ((h = gethostbyname(addr)) == NULL)
	urec.sl_saddr.s_addr=inet_addr(addr);
    else
	urec.sl_saddr.s_addr = *((int *) h->h_addr); /* internet only */

    if (write(ufd,&urec,sizeof(urec))!=sizeof(urec)){
	fprintf(stderr,"%s: write failed\n",USER_FL);
	close(ufd);
	return(-1);
        /* NOTREACHED */
    }

    return(0);
}
