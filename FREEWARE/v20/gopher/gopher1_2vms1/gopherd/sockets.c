/********************************************************************
 * $Author: lindner $
 * $Revision: 3.1 $
 * $Date: 1993/07/07 19:27:25 $
 * $Source: /home/mudhoney/GopherSrc/CVS/gopher+/object/Sockets.c,v $
 * $State: Exp $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: Sockets.c
 * Socket functions
 *********************************************************************
 * Revision History:
 * $Log: Sockets.c,v $
 * Revision 3.1  1993/07/07  19:27:25  lindner
 * Socket functions
 *
 *
 *
 *********************************************************************/

/* Generic stuff */

#include "boolean.h"
#include "Sockets.h"
#include "compatible.h"

/* Socket specific stuff, ugh! */
#ifdef VMS
#include <socket.h>
#include <in.h>
#include <file.h>
#include <inet.h>
#include <netdb.h>

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/file.h>
#ifndef hpux
#include <arpa/inet.h>
#endif
#include <netdb.h>
#endif  /* not VMS */

#include "Malloc.h"

extern int DEBUG;


/*
 * This turns the linger output off
 */

void
SOCKlinger(sockfd, onoff)
  int sockfd;
  boolean onoff;
{
#if defined(SO_LINGER) && !defined(NO_LINGER)
    struct linger linger;
     
    linger.l_onoff = onoff;
    linger.l_linger = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char *)&linger,
		   sizeof (linger)) < 0)
	 perror("server: can't turn off linger sockopt"),exit(-1);
#endif
}



/*
 * This function returns a socket file descriptor bound to the given port
 */

int
SOCKbind_to_port(port) 
  int port;
{
    struct sockaddr_in serv_addr;
    int reuseaddr = 1;
    int sockfd;
    
    
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	 perror("server: can't open stream socket"), exit(-1);
    
    /** Bind our local address so that the client can send to us **/
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family 		= AF_INET;
    serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
    serv_addr.sin_port		= htons(port);
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr,
		   sizeof(reuseaddr)) < 0)
	  perror("server: can't set REUSEADDR!"),exit(-1);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0)
	 perror("server: can't bind local address"),exit(-1);
    
    SOCKlinger(sockfd, FALSE);
    return(sockfd);
}


/* SOCKconnect performs a connection to socket 'service' on host
 * 'host'.  Host can be a hostname or ip-address.  If 'host' is null, the
 * local host is assumed.   The parameter full_hostname will, on return,
 * contain the expanded hostname (if possible).  Note that full_hostname is a
 * pointer to a char *, and is allocated by connect_to_gopher()
 *
 * Errors:
 *
 * -1 get service failed
 * -2 get host failed
 * -3 socket call failed
 * -4 connect call failed
 */

int
SOCKconnect(hostname, port)
  char *hostname;
  int port;
{
     struct sockaddr_in Server;
     struct hostent *HostPtr;
     int sockfd = 0;
     int ERRinet = -1;

#ifdef _CRAY
     ERRinet = 0xFFFFFFFF;  /* -1 doesn't sign extend on 64 bit machines */
#endif

     /*** Find the hostname address ***/
     
     if ((Server.sin_addr.s_addr = inet_addr(hostname)) == ERRinet) {
	  if (HostPtr = gethostbyname(hostname)) {
	       bzero((char *) &Server, sizeof(Server));
	       bcopy(HostPtr->h_addr, (char *) &Server.sin_addr, HostPtr->h_length);
	       Server.sin_family = HostPtr->h_addrtype;
	  } else
	       return (-2);
     } else
	  Server.sin_family = AF_INET;

     Server.sin_port = (unsigned short) htons(port);

     /*** Open the socket ***/

     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	  return (-3);

#ifndef UCX
     setsockopt(sockfd, SOL_SOCKET, ~SO_LINGER, 0, 0);
#endif

     setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 0, 0);
     setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

     /*** Connect ***/

     if (connect(sockfd, (struct sockaddr *) &Server, sizeof(Server)) < 0) {
	  closenet(sockfd);
	  return (-4);
     }

     return(sockfd);
}


/*
 *
 */

int
SOCKlisten(We)
  struct sockaddr_in *	We;
{
     int             sockfd = 0;
     struct hostent *HostPtr;
     int             len = sizeof(struct sockaddr);
     char            name[100];


     sockfd = SOCKbind_to_port(sockfd);
     if (listen(sockfd, 5) || getsockname(sockfd, (struct sockaddr *) We, &len)) {
          closenet(sockfd);
          return(-4);
     }
     
     gethostname(name, 100);
     if (HostPtr = gethostbyname(name)) 
          bcopy(HostPtr->h_addr, (char *) &We->sin_addr, HostPtr->h_length);
     return(sockfd);
}


/* GSaccept accepts a connection from some socket. *
 * Errors:
 *
 * -1 get service failed
 *
 * -2 get host failed
 *
 * -3 socket call failed
 *
 * -4 connect call failed
 */

int
SOCKaccept(s, we)
  int s;
  struct sockaddr_in we;
{
     struct hostent *HostPtr  = NULL;
     int            ERRinet   = -1;
     int            sockfd    = 0;
     int            len       = sizeof(struct sockaddr);
     unsigned short tem;

#ifdef _CRAY
     ERRinet = 0xFFFFFFFF;  /* -1 doesn't sign extend on 64 bit machines */
#endif

     tem = ntohs(we.sin_port);

     if (DEBUG) printf ("Here we go...\n");

     if ((sockfd = accept(s, (struct sockaddr *) &we, &len)) < 0) {
	  return -4;
     }
     close(s); /* Der Mohr hat seine Schuldigkeit getan */

     return(sockfd);
}
