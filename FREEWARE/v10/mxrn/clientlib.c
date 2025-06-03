
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/clientlib.c,v 1.6 1993/01/11 02:14:30 ricks Exp $";
#endif

/*
 * nntp client interface
 *
 * Copyright (c) 1988-1993, The Regents of the University of California.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef VMS
#define index strchr
int get_dnet_socket();
int get_server();
int start_server();
extern int sys$open();
extern int sys$close();
extern int sys$connect();
extern int sys$put();
extern int sys$flush();
extern int sys$get();
extern int atoi();
extern void ehVMSerror();
/*
 * Define UCX if you want UCX TCP/IP transport for VMS.
 * Define CMU_TCP for CMU TCP/IP transport. (Code supplied by Mike Iglesias,
 *				             iglesias@draco.acs.uci.edu)
 * Define FUSION for Fusion TCP/IP. (Code Supplied by Ken Robinson,
 *				     robinson@cp0201.bellcore.com)
 * Define WOLLONGONG for WIN/TCP.
 * Define MULTINET for MultiNet TCP/IP.
 *				  (Code supplied by John McMahon,
 *				   mcmahon@tgv.com)
 *
 * Define TCPWARE for Process Software's TCPware TCP/IP.
 *					(Code Supplied by David C. Mores,
 *					 mores@rapnet.sanders.lockheed.com)
 *
 * Added authinfo code so this version may be compiled with the AUTHINFO
 * macro. This will inline code to communicate with the news server and
 * pass the authinfo user and password.
 *
 * Mods made by Jim Gerland (gerland@ubvms.cc.buffalo.edu) and Rich
 * Alberth (acscrja@ubvms.cc.buffalo.edu).
 * 
 * Note to source readers: ;-)
 * "SERIALHACK" turns on a barely-functional modem-connection-to-nntpserver
 * mode. It doesn't work terribly well but allows me to debug stuff.
 * Please don't report problems with this stuff.
 */

#if defined(UCX) || defined(CMU_TCP) || defined(FUSION) || defined (WOLLONGONG) || defined(MULTINET) || defined(TCPWARE)
#define VMS_TCPIP
int get_tcp_socket();
#endif 

#ifdef MULTINET
#define BSD_BFUNCS
#include "MultiNet_Common_Root:[MultiNet.Include.Sys]types.h"
#include "MultiNet_Common_Root:[MultiNet.Include.Sys]socket.h"
#include "MultiNet_Common_Root:[MultiNet.Include.NetInet]in.h"
#include "MultiNet_Common_Root:[MultiNet.Include]netdb.h"
#define perror socket_perror
#define close  socket_close
extern int shutdown();
extern void bzero();
extern short htons();
extern int socket();
extern void bcopy();
extern int connect();
extern int send();
extern int recv();
extern int sys$dassgn();
#endif /* MULTINET */

#ifdef TCPWARE
#include "TCPIP_INCLUDE:types.h"
#include "TCPIP_INCLUDE:socket.h"
#include "TCPIP_INCLUDE:in.h"
#include "TCPIP_INCLUDE:netdb.h"
#define send socket_send
#define recv socket_recv
#endif /* TCPWARE */

#define NNTP_PORT_NUMBER 119
#define index strchr

#ifdef CMU_TCP
#include <descrip.h>
#include <ssdef.h>
#include <iodef.h>
#define TCP$OPEN IO$_CREATE
#define TCP$READ IO$_READVBLK
#define TCP$SEND IO$_WRITEVBLK
#define TCP$CLOSE IO$_DELETE
#endif /* CMU_TCP */

#if defined(UCX) || defined(WOLLONGONG)
#ifdef UCX
#include <sys/types.h>
#define CADDR_T
#define __CADDR_T
#define __SOCKET_TYPEDEFS
typedef char * caddr_t;
typedef unsigned short u_short;
typedef unsigned long u_long;
typedef unsigned char u_char;
#endif /* UCX */
#include <socket.h>
#include <in.h>
#include <netdb.h>
#endif /* UCX or WOLLONGONG */

#ifdef FUSION
#include <fns_base:[h]flip.h>
#include <fns_base:[h]socket.h>
#include <fns_base:[h]in.h>
#include <fns_base:[h]netdb.h>
#endif /* FUSION */

#include <unixio.h>
#include <file.h>
#define NNTPobject "::\"0=NNTP\""
#include <fab.h>
#include <rab.h>

static struct FAB server_fab;
static struct RAB server_rab;

#endif /* VMS */
#include <stdio.h>

#ifdef DECNET
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
int get_dnet_socket();
#endif

#include "codes.h"
#include "config.h"
#include "utils.h"
#include "mesg.h"

#ifndef VMS
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#ifdef __STDC__
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
int get_tcp_socket();
int get_server();
int start_server();
#if !defined(__osf__) && !defined(linux)
extern void bcopy _ARGUMENTS((char *, char *, int));
#ifndef hpux
extern int connect _ARGUMENTS((int, struct sockaddr*, int));
#endif
#endif
#ifndef linux
extern void bzero _ARGUMENTS((char *, int));
#endif
extern int socket _ARGUMENTS((int, int, int));
extern unsigned long inet_addr _ARGUMENTS((char *));
#endif
#endif /* VMS */

void close_server();
#ifdef BSD_BFUNCS
#ifndef memset
#define memset(_Str_, _Chr_, _Len_) bzero(_Str_, _Len_)
#endif
#ifndef memcpy
#define memcpy(_To_, _From_, _Len_) bcopy(_From_, _To_, _Len_)
#endif
#endif

int put_server();

#ifdef SERIALHACK
static char realNNTPhost[128];
#endif

static FILE	*ser_rd_fp = NULL;
static FILE	*ser_wr_fp = NULL;
static Boolean	ServerOpen = False;
static int	sockt_rd = -1, sockt_wr = -1;
static Boolean	VMSTcpIp = False;
static int  	level = 0;
static int  	inBuf = 0;
static Boolean  ignoreErrors = False;
char	*server_init_msg = NULL;
static int	server_length = 0;

#ifdef CMU_TCP
static unsigned short cmu_chan;
$DESCRIPTOR (cmu_ip, "IP0");
int status;
struct iosb_str {
    short stat;     /* status */
    short count;    /* Byte count */
    int	  info;	    /* additional info */
};
struct iosb_str iosb;
#endif /* CMU_TCP */
/*
 * getserverbyfile	Get the name of a server from a named file.
 *			Handle white space and comments.
 *			Use NNTPSERVER environment variable if set.
 *
 *	Parameters:	"file" is the name of the file to read.
 *
 *	Returns:	Pointer to static data area containing the
 *			first non-ws/comment line in the file.
 *			NULL on error (or lack of entry in file).
 *
 *	Side effects:	None.
 */

char *
getserverbyfile(file)
char	*file;
{
    char *cp;
    extern char *getenv(), *getinfofromfile();
    static char	buf[256];

    if (cp = getenv("NNTPSERVER")) {
	(void) strcpy(buf, cp);
    } else {
	cp = getinfofromfile(file);
	if (cp == NULL) {
	    return(NULL);
	} else {
	    (void) strcpy(buf, cp);
	}
    }
    return (buf);
}

/*
 * server_init  Get a connection to the remote news server.
 *
 *	Parameters:	"machine" is the machine to connect to.
 *
 *	Returns:	-1 on error
 *			server's initial response code on success.
 *
 *	Side effects:	Connects to server.
 *			"ser_rd_fp" and "ser_wr_fp" are fp's
 *			for reading and writing to server.
 */

#ifndef VMS
server_init(machine)
char	*machine;
{
    int	status;
    char line[256];
    char	*cp;

#ifdef NNTPVIATIP
    ser_rd_fp = fdopen(3, "r"); /* 3 is dictated by tip(1) ~C command */
    ser_wr_fp = fdopen(4, "r"); /* 4 is dictated by tip(1) ~C command */
    (void) sprintf(line, "telnet %s %d", machine, 119);
    put_server(line, True);
    get_server(line, sizeof(line)); /* Trying ... */
    get_server(line, sizeof(line)); /* Connected to ... */
    get_server(line, sizeof(line)); /* Escape character is ... */
#else /* NNTPVIATIP */
    close_server();				/* make sure it's closed */

#ifdef DECNET
    cp = index(machine, ':');

    if (cp && cp[1] == ':') {
	*cp = '\0';
	sockt_rd = get_dnet_socket(machine);
	*cp = ':';
    } else
	sockt_rd = get_tcp_socket(machine);
#else  /* DECNET */
    sockt_rd = get_tcp_socket(machine);
#endif /* DECNET */

    if (sockt_rd < 0)
	return (-1);

    /*
     * Now we'll make file pointers (i.e., buffered I/O) out of
     * the socket file descriptor.  Note that we can't just
     * open a fp for reading and writing -- we have to open
     * up two separate fp's, one for reading, one for writing.
     */

    if ((ser_rd_fp = (FILE *) fdopen(sockt_rd, "r")) == NULL) {
	perror("server_init: fdopen #1");
	return (-1);
    }

    sockt_wr = dup(sockt_rd);
    if ((ser_wr_fp = (FILE *) fdopen(sockt_wr, "w")) == NULL) {
	perror("server_init: fdopen #2");
	ser_rd_fp = NULL;		/* from above */
	return (-1);
    }

#endif /* NNTPVIATIP */

    ServerOpen = True;

    /* Now get the server's signon message */

    if (server_init_msg)
	XtFree(server_init_msg);
    if (! (server_init_msg = XtMalloc(1024))) {
	perror("malloc failed.");
	if (ser_rd_fp) (void) fclose(ser_rd_fp);
	if (ser_wr_fp) (void) fclose(ser_wr_fp);
	ser_rd_fp = ser_wr_fp = NULL;
	ServerOpen = False;
	return (-1);
    }
    *server_init_msg = '\0';
    server_length = 1024;

    do {
	if (get_server(line, sizeof(line)) == -1) {
	    if (ser_rd_fp && feof(ser_rd_fp)) {
		fprintf(stderr, "Unexpected EOF on NNTP server socket -- server is probably malfunctioning.\n");
	    } else {
		perror("reading from server");
	    }
	    if (ser_rd_fp) (void) fclose(ser_rd_fp);
	    if (ser_wr_fp) (void) fclose(ser_wr_fp);
	    ser_rd_fp = ser_wr_fp = NULL;
	    ServerOpen = False;
	    return (-1);
	}
	while (strlen(line)+strlen(server_init_msg)+1 >= (unsigned) server_length) {
	    server_length += 1024;
	    if (! (server_init_msg = XtRealloc(server_init_msg, server_length))) {
		if (ser_rd_fp) (void) fclose(ser_rd_fp);
		if (ser_wr_fp) (void) fclose(ser_wr_fp);
		ser_rd_fp = ser_wr_fp = NULL;
		ServerOpen = False;
		perror("realloc failed.");
		return (-1);
	    }
	}
	strcat(server_init_msg, line);
	strcat(server_init_msg, "\n");
    } while (line[3] == '-');

    status = atoi(line);
    ServerOpen = True;

    return status;
}
#else /* VMS */
server_init(machine)
char	*machine;
{
#ifdef AUTHINFO
/*
 * Authentication code added by Jim Gerland, gerland@ubvms.cc.buffalo.edu
 * and Rich Alberth, acscrja@ubvms.cc.buffalo.edu
 * Feb 1992
 */
/* authinfo.h: This file should have the following character strings defined
 * as the authinfo user and authinfo pass:
 *    char *authinfo_user = "whatever";
 *    char *authinfo_pass = "whatever";
 */
    int auth_status;	/* status values returned by authinfo calls to
			 * get_server() and put_server().
			 */
    char tmp_str[128];	/* this used as a temporary string for use by
			 * sprintf(). */
#endif /* AUTHINFO */
    int	 status;
    char line[256];
    char *cp;

    close_server();			/* make sure it's closed */

    level = 0;				/* Reset buffering */
    inBuf = 0;

#ifdef SERIALHACK
    cp = strrchr(machine, '/');
    strcpy(realNNTPhost, cp+1);
    *cp = '\0';
    sockt_rd = get_dnet_socket(machine);
#else
    cp = strrchr(machine, ':');
    if (cp) {
	cp--;
	if (cp[1] == ':') {		/* :: means DECnet */
	    sockt_rd = get_dnet_socket(machine);
	}
#ifdef VMS_TCPIP
    } else {
	cp = index(machine, '.');	/* . means IP */
	if (cp) {
	    sockt_rd = get_tcp_socket(machine);
	} else {			/* No indication; try DECnet then IP */
	    sockt_rd = get_dnet_socket(machine);
	    if (sockt_rd < 0)
		sockt_rd = get_tcp_socket(machine);
	}
    }
#else
    } else {
	sockt_rd = get_dnet_socket(machine);
    }
#endif /* VMS_TCPIP */
#endif

    if (sockt_rd < 0)
	return (-1);

    /* Now get the server's signon message */

    ServerOpen = True;

#ifdef SERIALHACK
    sprintf(line, "telnet %s 119", realNNTPhost);
    put_server(line, True);
    get_server(line, sizeof(line)); /* Trying ... */
    get_server(line, sizeof(line)); /* Connected to ... */
    get_server(line, sizeof(line)); /* Escape character is ... */
#endif

    if (server_init_msg)
	XtFree(server_init_msg);
    if (! (server_init_msg = XtMalloc(1024))) {
	perror("malloc failed.");
	if (ser_rd_fp) (void) fclose(ser_rd_fp);
	if (ser_wr_fp) (void) fclose(ser_wr_fp);
	ser_rd_fp = ser_wr_fp = NULL;
	ServerOpen = False;
	return (-1);
    }
    *server_init_msg = '\0';
    server_length = 1024;

    do {
	if (get_server(line, sizeof(line)) == -1) {
	    if (ser_rd_fp && feof(ser_rd_fp)) {
		fprintf(stderr, "Unexpected EOF on NNTP server socket -- server is probably malfunctioning.\n");
	    } else {
		perror("reading from server");
	    }
	    if (ser_rd_fp) (void) fclose(ser_rd_fp);
	    if (ser_wr_fp) (void) fclose(ser_wr_fp);
	    ser_rd_fp = ser_wr_fp = NULL;
	    ServerOpen = False;
	    return (-1);
	}
	while (strlen(line)+strlen(server_init_msg)+1 >= (unsigned) server_length) {
	    server_length += 1024;
	    if (! (server_init_msg = XtRealloc(server_init_msg, server_length))) {
		if (ser_rd_fp) (void) fclose(ser_rd_fp);
		if (ser_wr_fp) (void) fclose(ser_wr_fp);
		ser_rd_fp = ser_wr_fp = NULL;
		perror("realloc failed.");
		ServerOpen = False;
		return (-1);
	    }
	}
	strcat(server_init_msg, line);
	strcat(server_init_msg, "\n");
    } while (line[3] == '-');

    status = atoi(line);
    ServerOpen = True;

#ifdef AUTHINFO
    sprintf(tmp_str, "AUTHINFO USER %s", authinfo_user);
    auth_status = put_server(tmp_str, True);
    if (auth_status == -1) {
	printf("Error trying to send AUTHINFO USER message to server.\n");
	sys$close(&server_fab, 0, 0);
	ServerOpen = False;
	return -1;
    }
    /* wait for a 381 "PASS required" message back from the server. */

    (void) get_server(tmp_str, sizeof(tmp_str));
    if (strncmp(tmp_str, "381", 3) != 0) {
	printf("Didn't get back 381 when waiting for PASS.\n");
	sys$close(&server_fab, 0, 0);
	ServerOpen = False;
	return -1;
    }
    /* Send the password to the nntp server. */

    sprintf(tmp_str, "AUTHINFO PASS %s", authinfo_pass);
    auth_status = put_server(tmp_str, True);
    if (auth_status == -1) {
	printf("Error trying to send AUTHINFO PASS message to server.\n");
	sys$close(&server_fab, 0, 0);
	ServerOpen = False;
	return -1;
    }
    /* wait for 281 'password correct' */

    (void) get_server(tmp_str, sizeof(tmp_str));
    if (strncmp(tmp_str, "281", 3) != 0) {
	printf("Didn't receive OK on password from nntp server.\n");
	sys$close(&server_fab, 0, 0);
	ServerOpen = False;
	return -1;
    }
#endif /* AUTHINFO */

    return status;
}
#endif /* VMS */

#ifndef VMS
/*
 * get_tcp_socket -- get us a socket connected to the news server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *
 *	Returns:	Socket connected to the news server if
 *			all is ok, else -1 on error.
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Printed via perror.
 *
 *  This is a rewrite of the original get_tcp_socket.  This rewrite was
 *  supplied by Casey Leedom <casey@gauss.llnl.gov>
 */
static int get_tcp_socket1 _ARGUMENTS((struct sockaddr_in *));

get_tcp_socket(machine)
char	*machine;
{
    int s;
#ifdef h_addr
    register char **cp;
#endif /* h_addr */

    struct sockaddr_in sin;
#ifdef SOLARIS
    struct servent *getservbyname _ARGUMENTS((char *, char *));
#else
    struct servent *getservbyname _ARGUMENTS((const char *, const char *));
#endif
    struct servent *sp;
    struct hostent *gethostbyname _ARGUMENTS((const char *)), *hp;

    (void) memset((char *) &sin, 0, sizeof(sin));
    if ((sp = getservbyname("nntp", "tcp")) ==  NULL) {
	(void) fprintf(stderr, "nntp/tcp: Unknown service.\n");
	return (-1);
    }

    sin.sin_port = sp->s_port;
    if ((sin.sin_addr.s_addr = inet_addr(machine)) != -1) {
	sin.sin_family = AF_INET;
	return(get_tcp_socket1(&sin));
    } else {
	if ((hp = gethostbyname(machine)) == NULL) {
	    (void) fprintf(stderr, "%s: Unknown host.\n", machine);
	    return (-1);
	}
	sin.sin_family = hp->h_addrtype;
    }

    /*
     * The following is kinda gross.  The name server under 4.3
     * returns a list of addresses, each of which should be tried
     * in turn if the previous one fails.  However, 4.2 hostent
     * structure doesn't have this list of addresses.
     * Under 4.3, h_addr is a #define to h_addr_list[0].
     * We use this to figure out whether to include the NS specific
     * code...
     */

#if defined(h_addr)

    /* attempt multiple addresses */

    for (s = 0, cp = hp->h_addr_list; cp && *cp; cp++) {
	if (s < 0)
	    (void) fprintf(stderr,"trying alternate address for %s\n", machine);
	(void) memcpy((char *) &sin.sin_addr, *cp, hp->h_length);
	s = get_tcp_socket1(&sin);
	if (s >= 0)
	    return(s);
    }
    (void) fprintf(stderr, "giving up...\n");
    return (-1);

#else	/* 4.2BSD hostent structure */

    (void) memcpy((char *) &sin.sin_addr, hp->h_addr, hp->h_length);
    return(get_tcp_socket1(&sin));

#endif
}

static int
get_tcp_socket1(sp)
struct sockaddr_in *sp;
{
    int s;

    s = socket(sp->sin_family, SOCK_STREAM, 0);
    if (s < 0) {
	(void) perror("socket");
	return (-1);
    }
    if (connect(s, (struct sockaddr *)sp, sizeof (*sp)) < 0) {
	(void) perror("connect");
	(void) close(s);
	return(-1);
    }
    return(s);
}
#else /* VMS */
#ifdef VMS_TCPIP
#ifndef CMU_TCP
/*
 * get_tcp_socket -- get us a socket connected to the news server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *
 *	Returns:	Socket connected to the news server if
 *			all is ok, else -1 on error.
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Printed via perror.
 */

get_tcp_socket(machine)
char	*machine;
{
    int	s;
#ifdef h_addr
    int x = 0;
    register char **cp;
#endif /* h_addr */

#ifdef FUSION
#define gethostbyname ghbyname
#endif
    struct	sockaddr_in sin;
    struct	servent *getservbyname(), *sp;
    struct	hostent *gethostbyname(), *hp;

    if ((hp = gethostbyname(machine)) == NULL) {
	(void) fprintf(stderr, "%s: Unknown host.\n", machine);
	return (-1);
    }

    (void) memset((char *) &sin, 0, sizeof(sin));
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = htons(NNTP_PORT_NUMBER);

    /*
     * The following is kinda gross.  The name server under 4.3
     * returns a list of addresses, each of which should be tried
     * in turn if the previous one fails.  However, 4.2 hostent
     * structure doesn't have this list of addresses.
     * Under 4.3, h_addr is a #define to h_addr_list[0].
     * We use this to figure out whether to include the NS specific
     * code...
     */

#if defined(h_addr)
    /* get a socket and initiate connection -- use multiple addresses */

    for (cp = hp->h_addr_list; cp && *cp; cp++) {
	s = socket(hp->h_addrtype, SOCK_STREAM, 0);
	if (s < 0) {
	    (void) perror("socket");
	    return (-1);
	}
	(void) memcpy((char *) &sin.sin_addr, *cp, hp->h_length);
		
	if (x < 0) {
	    (void) fprintf(stderr, "trying %s\n", machine);
	}
	x = connect(s, (struct sockaddr *)&sin, sizeof (sin));
	if (x == 0)
	    break;
	(void) fprintf(stderr, "connection to %s: ", machine);
	(void) perror("");
	(void) close(s);
    }
    if (x < 0) {
	(void) fprintf(stderr, "giving up...\n");
	return (-1);
    }
#else	/* no name server */

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* Get the socket */
	(void) perror("socket");
	return (-1);
    }

    /* And then connect */

    (void) memcpy((char *) &sin.sin_addr, hp->h_addr, hp->h_length);
    if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
	(void) perror("connect");
	(void) close(s);
	return (-1);
    }
#endif
    VMSTcpIp = True;
    return (s);
}
#else /* CMU_TCP */
/*
 * get_tcp_socket -- get us a CMU TCP/IP socket connected to the news server
 *
 *	Paremeters:	"machine" is the machine the server is running on.
 *
 *	Returns:	Socket connected to the news server if
 *			all is ok, else -1 on error.
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Printed vua perror.
 */
get_tcp_socket (machine)
char *machine;
{
    int stat;

    /* first, open a channel to the IP0 device */
    if (((stat = SYS$ASSIGN(&cmu_ip, &cmu_chan, 0, 0)) & 1) != 1)
	return (-1);

    /* now connect to the server */

    stat = SYS$QIOW (0, cmu_chan, TCP$OPEN, &iosb, 0, 0, machine,
		     NNTP_PORT_NUMBER, 0, 1, 0, 0);
    if ((stat & 1) != 1) return (-1);
    if (iosb.stat == SS$_ABORT) return (-1);
    VMSTcpIp = True;
    ServerOpen = True;
    return (1);
}
#endif /* CMU_TCP */
#endif /* VMS_TCPIP */
#endif /* VMS */

#if defined(DECNET) || defined(VMS)
/*
 * get_dnet_socket -- get us a socket connected to the news server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *
 *	Returns:	Socket connected to the news server if
 *			all is ok, else -1 on error.
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Printed via nerror.
 */

get_dnet_socket(machine)
char	*machine;
{
#ifdef VMS
    static char connect[128];
    char *ptr;
    int	s,colon;
    int status;

#ifdef SERIALHACK
    strcpy(connect, machine);
#else
    ptr = strrchr(machine, ':');
    if (ptr) {
	colon = ptr - machine - 1;
    } else {
	colon = strlen(machine);
    }
    (void) strncpy(connect, machine, colon);
    (void) strcpy(&connect[colon], NNTPobject);
#endif
	
    memset((char *) &server_fab, 0, sizeof(server_fab));
    memset((char *) &server_rab, 0, sizeof(server_rab));

    server_fab = cc$rms_fab;
    server_rab = cc$rms_rab;

    server_fab.fab$v_put = 1;
    server_fab.fab$v_get = 1;
    server_fab.fab$l_fna = connect;
    server_fab.fab$b_fns = strlen(connect);
    status = sys$open(&server_fab, 0, 0);
    if ((status & 1) != 1) {
	ehVMSerror("NNTP Server Open error:", server_fab.fab$l_sts,
		   server_fab.fab$l_stv);
	sys$close(&server_fab, 0, 0);
	ServerOpen = False;
	return (-1);
    }
    server_rab = cc$rms_rab;
    server_rab.rab$l_fab = &server_fab;
    status = sys$connect(&server_rab, 0, 0);
    if ((status & 1) != 1) {
	ehVMSerror("NNTP Server Open error:", server_rab.rab$l_sts,
		   server_rab.rab$l_stv);
	sys$close(&server_fab, 0, 0);
	ServerOpen = False;
	return (-1);
    }

    s = 0;
    VMSTcpIp = False;

#else /* Not VMS */

    int	s, area, node;
    struct sockaddr_dn sdn;
    struct nodeent *getnodebyname(), *np;

    (void) memset((char *) &sdn, 0, sizeof(sdn));

    switch (s = sscanf( machine, "%d%*[.]%d", &area, &node )) {
    	case 1: 
	    node = area;
	    area = 0;
	case 2: 
	    node += area*1024;
	    sdn.sdn_add.a_len = 2;
	    sdn.sdn_family = AF_DECnet;
	    sdn.sdn_add.a_addr[0] = node % 256;
	    sdn.sdn_add.a_addr[1] = node / 256;
	    break;
	default:
	    if ((np = getnodebyname(machine)) == NULL) {
		(void) fprintf(stderr, 
				    "%s: Unknown host.\n", machine);
		return (-1);
	    } else {
		(void) memcpy((char *) sdn.sdn_add.a_addr, 
					  np->n_addr, 
					  np->n_length);
		sdn.sdn_add.a_len = np->n_length;
		sdn.sdn_family = np->n_addrtype;
	    }
	    break;
    }
    sdn.sdn_objnum = 0;
    sdn.sdn_flags = 0;
    sdn.sdn_objnamel = strlen("NNTP");
    (void) memcpy(&sdn.sdn_objname[0], "NNTP", sdn.sdn_objnamel);

#ifdef ANU_NEWS_SERVER
    if ((s = socket(AF_DECnet, SOCK_SEQPACKET, 0)) < 0) {
#else
    if ((s = socket(AF_DECnet, SOCK_STREAM, 0)) < 0) {
#endif
    	nerror("socket");
    	return (-1);
    }

    /* And then connect */

    if (connect(s, (struct sockaddr *) &sdn, sizeof(sdn)) < 0) {
    	nerror("connect");
    	close(s);
    	return (-1);
    }
#endif /* VMS */

    return (s);
}
#endif /* DECNET or VMS */

/*
 * handle_server_response
 *
 *	Print some informative messages based on the server's initial
 *	response code.  This is here so inews, rn, etc. can share
 *	the code.
 *
 *	Parameters:	"response" is the response code which the
 *			server sent us, presumably from "server_init",
 *			above.
 *			"server" is the news server we got the
 *			response code from.
 *
 *	Returns:	-1 if the error is fatal (and we should exit).
 *			0 otherwise.
 *
 *	Side effects:	None.
 */
int
handle_server_response(response, server)
int	response;
char	*server;
{
    char line[256];

    switch (response) {
	case OK_NOPOST:		/* fall through */
	    mesgPane(XRN_INFO, 
	    "NOTE: This machine does not have permission to post articles.");
	    mesgPane(XRN_INFO|XRN_APPEND,
	    "      Please don't waste your time trying.\n\n");

	case OK_CANPOST:
#ifdef NOISY_OK
	    mesgPane(XRN_INFO,
	    "Connected to %s news server. Posting is permitted.\n",
		server);
#endif
	    if (put_server("mode reader", True) == -1) {
		mesgPane(XRN_SERIOUS,
		"Error trying to send MODE READER message to server.");
		return -1;
	    }
    
	    if (get_server(line, sizeof(line)) == -1) {
		mesgPane(XRN_SERIOUS,
		"Error trying to read MODE READER response from server.");
		return -1;
	    }
    
	    return (0);

	case ERR_GOODBYE:
	    mesgPane(XRN_SERIOUS,
	    "The news service is currently not available from %s:\n%s\n\n",
		server, server_init_msg);
	    return(-1);

	case ERR_ACCESS:
	    mesgPane(XRN_SERIOUS,
	    "This machine does not have permission to use the %s news server:\n%s\n",
		server, server_init_msg);
	    return (-1);

	default:
	    mesgPane(XRN_SERIOUS,
		"Unexpected response code from %s news server:%s\n\n",
			server, server_init_msg);
	    return (-1);
    }
	/*NOTREACHED*/
}

/*
 * put_server -- send a line of text to the server, terminating it
 * with CR and LF, as per ARPA standard.
 *
 *	Parameters:	"string" is the string to be sent to the
 *			server.
 *			"flush" is true if should flush the output
 *			after send.
 *
 *	Returns:	Status (-1 for failure).
 *
 *	Side effects:	Talks to the server.
 *
 */

#ifndef VMS
int
put_server(string, flushit)
char *string;
Boolean flushit;
{
    if (!ServerOpen)
	start_server(NIL(char));
#ifdef DEBUG
    (void) fprintf(stdout, ">>> %s\n", string);
#endif

    if (fprintf(ser_wr_fp, "%s\r\n", string) == EOF) {
	return -1;
    }
#ifndef ANU_NEWS_SERVER
    if (flushit) {
#endif
	if (fflush(ser_wr_fp) == EOF) {
	    return -1;
	}
#ifndef ANU_NEWS_SERVER
    }
#endif
#ifdef NNTPVIATIP
	{
	    char line[256];
	    get_server(line, sizeof(line));
	}
#endif
    return 0;
}
#else /* VMS */
int
put_server(string, flushit)
char *string;
Boolean flushit;
{
    int status;
    static char *sendString;		/* Including cr/lf at end */
    static int sendSize = 0;
    int s;
#ifdef SERIALHACK
    char echobuf[128];
#endif

    if (!ServerOpen)
	start_server(NIL(char));
#ifdef DEBUG
    (void) fprintf(stdout, ">>> %s\n", string);
#endif

    if (sendSize == 0) {
	sendSize = 128;			/* Prob. adequate - 80 should be max */
	sendString = XtMalloc(128);
    }
    s = strlen(string);
    if (s + 5 > sendSize) {
	sendSize = s + 5;
	sendString = XtRealloc(sendString, sendSize);
    }
#ifdef SERIALHACK
    sprintf(sendString, "%s\r", string);
#else
    sprintf(sendString, "%s\r\n", string);
#endif
#ifdef VMS_TCPIP
    if (VMSTcpIp) {
#ifndef CMU_TCP
	if (send(sockt_rd, sendString, s + 2, 0) == -1) {
	    perror("Server write failed");
	    return -1;
	}
#else /* CMU_TCP */
	status = SYS$QIOW (0, cmu_chan, TCP$SEND, &iosb, 0, 0,
			   sendString, s+2, 0, 0, 0, 0);
#endif /* CMU_TCP */
    } else {		/* DECnet */
	server_rab.rab$l_rbf = sendString;
#ifdef SERIALHACK
	server_rab.rab$w_rsz = s + 1;
#else
	server_rab.rab$w_rsz = s + 2;
#endif
	status = sys$put(&server_rab, 0, 0);
	if ((status & 1) != 1) {
	    if (!ignoreErrors) {
		ehVMSerror("NNTP Server Write error:", server_rab.rab$l_sts,
			   server_rab.rab$l_stv);
	    }
	    sys$close(&server_fab, 0, 0);
	    ServerOpen = False;
	    return (-1);
	}
#ifndef SERIALHACK
	if (flushit) {
#endif
	    status = sys$flush(&server_rab, 0, 0);
	    if ((status & 1) != 1) {
		if (!ignoreErrors) {
		    ehVMSerror("NNTP Server Write error:", server_rab.rab$l_sts,
			   server_rab.rab$l_stv);
		}
		sys$close(&server_fab, 0, 0);
		ServerOpen = False;
		return (-1);
	    }
#ifndef SERIALHACK
	}
#endif
    }
#else /* VMS_TCPIP */
    server_rab.rab$l_rbf = sendString;
#ifdef SERIALHACK
	server_rab.rab$w_rsz = s + 1;
#else
	server_rab.rab$w_rsz = s + 2;
#endif
    status = sys$put(&server_rab, 0, 0);
    if ((status & 1) != 1) {
	if (!ignoreErrors) {
	    ehVMSerror("NNTP Server Write error:", server_rab.rab$l_sts,
			   server_rab.rab$l_stv);
	}
	sys$close(&server_fab, 0, 0);
	ServerOpen = False;
	return (-1);
    }
#ifndef SERIALHACK
    if (flushit) {
#endif
	status = sys$flush(&server_rab, 0, 0);
	if ((status & 1) != 1) {
	    if (!ignoreErrors) {
		ehVMSerror("NNTP Server Write error:", server_rab.rab$l_sts,
			   server_rab.rab$l_stv);
	    }
	    sys$close(&server_fab, 0, 0);
	    ServerOpen = False;
	    return (-1);
	}
#ifndef SERIALHACK
    }
#endif
#endif /* VMS_TCPIP */
#ifdef SERIALHACK
    get_server(echobuf, sizeof(echobuf));
#endif
    return 0;
}
#endif /* VMS */
/*
 * get_server -- get a line of text from the server.  Strips
 * CR's and LF's.
 *
 *	Parameters:	"string" has the buffer space for the
 *			line received.
 *			"size" is the size of the buffer.
 *
 *	Returns:	-1 on error, 0 otherwise.
 *
 *	Side effects:	Talks to server, changes contents of "string".
 */

#ifndef VMS
get_server(string, size)
char	*string;
int	size;
{
    register char *cp;

    if (!ServerOpen)
	start_server(NIL(char));

    if (fgets(string, size, ser_rd_fp) == NULL)
	return (-1);
#ifdef NNTPVIATIP
    for (cp = string; cp < &string[size]; cp++) {
	*cp = *cp & 0x7F;
	if (*cp == '\0') {
	    break;
	}
    }
#endif
    if ((cp = index(string, '\r')) != NULL)
	*cp = '\0';
    else if ((cp = index(string, '\n')) != NULL)
	*cp = '\0';

#ifdef DEBUG
    (void) fprintf(stdout, "<<< %s\n", string);
#endif

    return (0);
}
#else /* VMS */
get_server(string, size)
char	*string;
int	size;
{
    register char *cp;
    static char netBuf[8193];
    static char *bufPtr;
    int ret;
    int status;
#ifdef SERIALHACK
#include <rmsdef.h>
#endif

    if (!ServerOpen) {
	start_server(NIL(char));
	level = 0;
	inBuf = 0;
    }

    if (level == 0)
	*string = '\0';
    if (inBuf <= 0) {
#ifdef VMS_TCPIP
	if (!VMSTcpIp) {
	    server_rab.rab$l_ubf = netBuf;
	    server_rab.rab$w_usz = sizeof(netBuf) - 1;
	    status = sys$get(&server_rab, 0, 0);
	    if ((status & 1) != 1) {
		if (!ignoreErrors) {
		    ehVMSerror("NNTP Server Read error:", server_rab.rab$l_sts,
			   server_rab.rab$l_stv);
		}
		sys$close(&server_fab, 0, 0);
		ServerOpen = False;
		return (-1);
	    }
	    inBuf = server_rab.rab$w_rsz;
	} else {
#ifndef CMU_TCP
	    if ((inBuf = recv(sockt_rd, netBuf, 1024, 0)) <= 0) 
		return (-1);
#else
	    status = SYS$QIOW (0, cmu_chan, TCP$READ, &iosb, 0, 0,
			       &netBuf, 1024, 0, 0, 0, 0);
	    if ((status & 1) != 1) return (-1);
	    if (iosb.stat == SS$_ABORT) return (-1);
	    inBuf = iosb.count;
#endif	/* CMU_TCP */
	}
#else /* VMS_TCPIP */
	
	server_rab.rab$l_ubf = netBuf;
	server_rab.rab$w_usz = sizeof(netBuf) - 1;
	status = sys$get(&server_rab, 0, 0);
#ifdef SERIALHACK
	if (status == RMS$_TNS) {
	    status = status | 1;
	}
#endif
	if ((status & 1) != 1) {
	    if (!ignoreErrors) {
		ehVMSerror("NNTP Server Read error:", server_rab.rab$l_sts,
		   server_rab.rab$l_stv);
	    }
	    sys$close(&server_fab, 0, 0);
	    ServerOpen = False;
	    return (-1);
	}
	inBuf = server_rab.rab$w_rsz;

#endif /* VMS_TCPIP */

#ifdef SERIALHACK
	if (status != (RMS$_TNS | 1))
	    netBuf[inBuf++] = '\r';
#endif
	netBuf[inBuf] = '\0';
	bufPtr = netBuf;
	if (*bufPtr == '\n')
	    bufPtr++;
    }
    if ((cp = index(bufPtr, '\r')) != NULL) {
	*cp = '\0';
	strcat(string, bufPtr);
	inBuf = inBuf - (cp - bufPtr) - 1;
	bufPtr = ++cp;
	if (*bufPtr == '\n') {
	    bufPtr++;
	    inBuf--;
	}
    } else {
	if ((cp = index(bufPtr, '\n')) != NULL) {
	    *cp = '\0';
	    strcat(string, bufPtr);
	    inBuf = inBuf - (cp - bufPtr) - 1;
	    bufPtr = ++cp;
	    if (*bufPtr == '\n') {
		bufPtr++;
		inBuf--;
	    }
	} else {
	    level++;
	    inBuf = 0;
	    strcat(string, bufPtr);
	    ret = get_server(string, size);
	    level--;
	    return ret;
	}
    }

#ifdef DEBUG
    (void) fprintf(stdout, "<<< %s\n", string);
#endif
    return (0);
}
#endif /* VMS */

/*
 * close_server -- close the connection to the server, after sending
 *		the "quit" command.
 *
 *	Parameters:	None.
 *
 *	Returns:	Nothing.
 *
 *	Side effects:	Closes the connection with the server.
 *			You can't use "put_server" or "get_server"
 *			after this routine is called.
 */

void
close_server()
{
    char	ser_line[256];

    if (!VMSTcpIp) {
	if (!ServerOpen)
	    return;
    } else {
	if (!ServerOpen || sockt_rd < 0)
	    return;
    }

    ignoreErrors = True;
    if (put_server("QUIT", True) != -1) {
#ifndef SERIALHACK
	while (ServerOpen) {
	    if (get_server(ser_line, sizeof(ser_line)) < 0)
		ServerOpen = False;
	}
#else
	(void) get_server(ser_line, sizeof(ser_line));
	(void) get_server(ser_line, sizeof(ser_line));
#endif
    }
    ignoreErrors = False;

#ifndef VMS
    if (ser_wr_fp) (void) fclose(ser_wr_fp);
    ser_wr_fp = NULL;
    if (ser_rd_fp) (void) fclose(ser_rd_fp);
    ser_rd_fp = NULL;
    if (sockt_rd > 0) {
	(void) shutdown(sockt_rd, 2);
	(void) close(sockt_rd);
	sockt_rd = -1;
    }
    if (sockt_wr > 0) {
	(void) shutdown(sockt_wr, 2);
	(void) close(sockt_wr);
	sockt_wr = -1;
    }
#else /* VMS */

    if (VMSTcpIp) {
	if (ser_wr_fp) (void) fclose(ser_wr_fp);
	ser_wr_fp = NULL;
	if (ser_rd_fp) (void) fclose(ser_rd_fp);
	ser_rd_fp = NULL;
    } else {
	sys$close(&server_fab, 0, 0);
    }

#ifdef VMS_TCPIP
#ifndef CMU_TCP
    if (VMSTcpIp) {
	(void) shutdown(sockt_rd, 2);
	(void) close(sockt_rd);
	(void) close(sockt_wr);
    }
#if defined(MULTINET) || defined(TCPWARE) || defined(WOLLONGONG)
    sys$dassgn(sockt_rd);
    sys$dassgn(sockt_wr);
    sockt_rd = -1;
#endif /* MULTINET etc */
#else  /* CMU_TCP */
    status = SYS$QIOW (0, cmu_chan, TCP$CLOSE, 0, 0, 0, 2, 0, 0, 0, 0, 0);
#endif /* CMU_TCP */
#endif /* VMS_TCPIP */
#endif /* VMS */
    ServerOpen = False;
}

#ifdef VMS
char *
localHostName()
{
    static char *ptr = NULL;
    static char host_name[128];
#ifdef VMS_TCPIP
    extern int gethostname _ARGUMENTS((char *, int));
#endif

    if (ptr == NULL) {
	ptr = getenv("SYS$CLUSTER_NODE");
    }
    if (ptr == NULL) {
	ptr = getenv("SYS$NODE");
    }
    if (ptr == NULL) {
#ifdef VMS_TCPIP
	if (gethostname(host_name, sizeof(host_name))) {
	    strcpy(host_name, "unavailable");
	}
#else
	strcpy(host_name, "unavailable");
#endif
	ptr = host_name;
    }
    return ptr;
}
#endif
