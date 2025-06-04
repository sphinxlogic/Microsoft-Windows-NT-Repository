/*	server.c							*/
/*	V1.1			20-Jan-1995	IfN/Mey			*/
/*+
 * Example for TCP-server program.
 *
 * The original file is from DEC's UCX, see copyright below.
 *-
 */
/*====================================================================
*
*			  COPYRIGHT (C) 1989 BY
*	      DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
*
* This software is furnished under a license and may be used and  copied
* only  in  accordance  with  the  terms  of  such  license and with the
* inclusion of the above copyright notice.  This software or  any  other
* copies  thereof may not be provided or otherwise made available to any
* other person.  No title to and ownership of  the  software  is  hereby
* transferred.
*
* The information in this software is subject to change  without  notice
* and  should  not  be  construed  as  a commitment by DIGITAL EQUIPMENT
* CORPORATION.
*
* DIGITAL assumes no responsibility for the use or  reliability  of  its
* software on equipment which is not supplied by DIGITAL.
*
*
*
*  FACILITY:
*	INSTALL 
*      
*
*  ABSTRACT:
*	This is an example of a TCP/IP server using the IPC 
*	socket interface.
*      
*
*  ENVIRONMENT:
*	UCX V1.2 or higher, VMS V5.2 or higher 
*
*	This example is portable to Ultrix. The include
*	files are conditionally defined for both systems, and 
*	"perror" is used for error reporting.
*
*	To link in VAXC/VMS you must have the following
*	entries in your .opt file:
*	   sys$library:ucx$ipc.olb/lib
*	   sys$share:vaxcrtl.exe/share
*
*  AUTHORS:
*	UCX Developer	
*
*  CREATION DATE: May 23, 1989 
*
*  MODIFICATION HISTORY:
*
*/


/*
*
*  INCLUDE FILES
*
*/

#ifdef VMS
#include  <errno.h>
#include  <types.h>
#include  <stdlib.h>
#include  <stdio.h>
#include  <unixio.h>
#include  <string.h>		/* needed for memset in FD_ZERO */
#include  <socket.h>
#include  <in.h>
#include  <netdb.h>		/* change hostent to comply with BSD 4.3 */
#include  <inet.h>
#ifdef UCX	/* IfN/Mey */
#include  <ucx$inetdef.h>	/* INET symbol definitions */
#define si_get_sdc vaxc$get_sdc
#else
/* #define _SOCKETSHR_SKIP_INCLUDES
   #define _SOCKETSHR_SKIP_SOCKET_STRUCTS
*/
#include  "[-]socketshr.h"
#endif
#else
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>		/* needed for memset in FD_ZERO */
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#endif

/* void cleanup(int, int, int); */

void cleanup(how_many, sock1, sock2)
int	how_many;
int	sock1, sock2;
	
{
	int	retval;

	/*
	 * Shutdown and close sock1 completely.
	 */
#ifdef VMS
printf("shutdown 1: chan: %08X\n",si_get_sdc(sock1));
#endif
	retval = shutdown(sock1,2);
	if (retval == -1)
		perror ("shutdown");

        retval = close (sock1);
	if (retval)
		perror ("close");


	/*
	 * If given, shutdown and close sock2. 
	 */
	if (how_many == 2)
	{
#ifdef VMS
printf("shutdown 2: chan: %08X\n",si_get_sdc(sock2));
#endif
		retval = shutdown(sock2,2);
		if (retval == -1)
			perror ("shutdown");

        	retval = close (sock2);
		if (retval)
			perror ("close");
	}

	exit(1);

} /* end cleanup*/

/*-----------------------------------------------------------*/

/*
* Functional Description
*
*	This examples creates a socket of type SOCK_STREAM (TCP),
*	binds and listens on the socket, receives a message 
*	and closes the connection.	
*	Error messages are printed to the screen.
*
*	IPC calls used: 
*	accept
*	bind
*	close
*	gethostbyname
*	listen
*	recv	
*	shutdown
*	socket	
*
*
* Formal Parameters
*	The server program expects one parameter:
*	portnumber ... port number where it will listen
*
*
* Routine Value
*
*	Status	
*/




/*--------------------------------------------------------------------*/
main(argc,argv)
int	argc;
char	**argv;
{

        int     sock_2, sock_3;                 /* sockets */
        static  char  		message[BUFSIZ]; 
static 	struct  sockaddr_in sock2_name;         /* Address struct for socket2.*/
static 	struct  sockaddr_in retsock2_name;      /* Address struct for socket2.*/
        struct  hostent         hostentstruct;  /* Storage for hostent data.  */
        struct  hostent         *hostentptr;    /* Pointer to hostent data.   */
	struct	servent		*serventptr;	/* Pointer to servent data    */
	struct	protoent	*protoentptr;	/* Pointer to protoent data    */
        static  char            hostname[256];  /* Name of local host.        */
	int	flag;
	int	retval;				/* helpful for debugging */
	int	namelength;			
	fd_set	rdfds, wrfds, exfds;		/* fd masks for select() */
	struct timeval timeout;			/* timeout for select() */
	char	**pp;
	char	*p;

	FILE *fptr;

	/*	
	 * Check input parameters.
	 */
	if (argc != 2 )
		{
		printf("Usage: server portnumber.\n");
		exit(1);
		}

#if 0
	setservent(1); /**/
	serventptr = getservbyport(1234, NULL); /**/
/*	endservent(); /**/
/*	setservent(1); /**/
	serventptr = getservent();
	serventptr = getservent();
	endservent();
	if (serventptr != (struct servent *)0 ) {
		printf("Service name: {%s}, Port: %d, Protocol: {%s}\n",
			serventptr->s_name, ntohs(serventptr->s_port),
			serventptr->s_proto);
		for (pp=serventptr->s_aliases; *pp; pp++) {
			printf("  Alias name: {%s}\n",*pp);
		}
	}
	else {
		perror("getservbyname");
		printf("Service not found!\n");
	}
#endif
#if 1
	setprotoent(1); /**/
	protoentptr = getprotobyname("TCP"); /**/
/*	endprotoent(); /**/
/*	setprotoent(1); /**/
/*	protoentptr = getprotoent(); /**/
/*	protoentptr = getprotoent(); /**/
	endprotoent();
	if (protoentptr != (struct protoent *)0 ) {
		printf("Protocol name: {%s}, number: %d\n",
			protoentptr->p_name, protoentptr->p_proto);
		for (pp=protoentptr->p_aliases; *pp; pp++) {
			printf("  Alias name: {%s}\n",*pp);
		}
	}
	else {
		perror("getprotobyname");
		printf("Protocol not found!\n");
	}
#endif
	/*
	 * Open socket 2: AF_INET, SOCK_STREAM. 
	 */
        if ((sock_2 = socket (AF_INET, SOCK_STREAM, 0)) == -1) 
		{
                perror( "socket");
		exit(1);
		}

printf("Socket: %08X\n",sock_2);
#ifdef VMS
printf("chan: %08X\n",si_get_sdc(sock_2));
#endif
fptr = fdopen(sock_2,"r");
printf("fptr: %08X\n",fptr);
printf("fileno(fptr): %d\n",fileno(fptr));
	/*
	 * Get the host local name. 
	 */
        retval = gethostname(hostname,sizeof hostname);
printf("gethostname retval: %d\n",retval);
printf("gethostname: %s\n",hostname);
	if (retval)
		{
		perror ("gethostname");
		cleanup (1, sock_2, 0);
		}

	/*
	 * Get pointer to network data structure for socket 2. 
	 */
        if ((hostentptr = gethostbyname (hostname)) == NULL) 
		{
                perror( "gethostbyname");
		cleanup(1, sock_2, 0);
		}

	/*
	 * Copy hostent data to safe storage. 
	 */
        hostentstruct = *hostentptr;

printf("host name: {%s}\n",hostentstruct.h_name);
for (pp=hostentstruct.h_aliases; *pp; pp++) {
	printf("alias name: {%s}\n",*pp);
}
for (pp=hostentstruct.h_addr_list; *pp; pp++) {
	printf("address: %s\n",inet_ntoa(*(struct in_addr *)(*pp)));
}

	/*
	 * Fill in the name & address structure for socket 2. 
	 */
        sock2_name.sin_family = hostentstruct.h_addrtype;
	sock2_name.sin_port = htons(atoi(argv[1]));
        sock2_name.sin_addr = * ((struct in_addr *) hostentstruct.h_addr);

	/*
	 * Bind name to socket 2.
 	 */
	retval = bind (sock_2, (struct sockaddr *)&sock2_name, sizeof sock2_name);
        if (retval)
		{
		perror("bind");
		cleanup(1, sock_2, 0);
		}

	namelength = sizeof (retsock2_name);
	retsock2_name.sin_addr.s_addr = 0;
	retsock2_name.sin_port = 0;
        retval = getsockname (sock_2, (struct sockaddr *)&retsock2_name, &namelength);
printf("getsockname: addr: %s, port: %d\n",
	inet_ntoa(retsock2_name.sin_addr),ntohs(retsock2_name.sin_port));

	/*
	 * Listen on socket 2 for connections. 
	 */
        retval = listen (sock_2, 5);
        if (retval)
		{
		perror("listen");
		cleanup(1, sock_2, 0);
		}

	namelength = sizeof (retsock2_name);
	retsock2_name.sin_addr.s_addr = 0;
	retsock2_name.sin_port = 0;
        retval = getsockname (sock_2, (struct sockaddr *)&retsock2_name, &namelength);
printf("getsockname: addr: %s, port: %d\n",
	inet_ntoa(retsock2_name.sin_addr),ntohs(retsock2_name.sin_port));

	/*
	 * Accept connection from socket 2: 	    
	 * accepted connection will be on socket 3. 
	 */
	namelength = sizeof (sock2_name);
printf("Waiting for connection (accept) ...\n\n");
#if 0
	while(1) {
	/*
	 * Try select
	 */
		FD_ZERO(&rdfds);
		FD_ZERO(&wrfds);
		FD_ZERO(&exfds);
		FD_SET(sock_2, &rdfds);
printf("socket %d is 0x%02X\n\n",sock_2,rdfds);
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		retval = select(31,&rdfds,&wrfds,&exfds,&timeout);
printf("select returns %d, rdfds=0x%02X\n",retval,rdfds);
		if (FD_ISSET(sock_2,&rdfds) != 0) break;
	}
#endif
        sock_3 = accept (sock_2, (struct sockaddr *)&sock2_name, &namelength);
printf("Accept Socket: %08X\n",sock_3);
#ifdef VMS
printf("chan: %08X\n",si_get_sdc(sock_3));
#endif
	if (sock_3 == -1)
		{
		perror ("accept");
		cleanup( 2, sock_2, sock_3);
		}

	/*
	 * Receive message from socket 3. 
	 */
	flag = 0;	/* maybe 0 or MSG_OOB or MSG_PEEK */

	retval = recv(sock_3, message ,sizeof (message), flag);
printf("recv: %d bytes from socket %d\n",retval,sock_3);
	message[retval] = '\0';
	if (retval == -1)
		{
		perror ("receive");
		cleanup( 2, sock_2, sock_3);
		}
	else
		printf (" %s\n", message);

fprintf(stderr,"Press any key "); fgetc(stdin); /**/
#if 1
	close(sock_3);
printf("Waiting for connection (accept) ...\n\n");
#if 0
	while(1) {
	/*
	 * Try select
	 */
		FD_ZERO(&rdfds);
		FD_ZERO(&wrfds);
		FD_ZERO(&exfds);
		FD_SET(sock_2, &rdfds);
printf("socket %d is 0x%02X\n\n",sock_2,rdfds);
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		retval = select(31,&rdfds,&wrfds,&exfds,&timeout);
printf("select returns %d, rdfds=0x%02X\n",retval,rdfds);
		if FD_ISSET(sock_2,&rdfds) break;
	}
#endif
        sock_3 = accept (sock_2, (struct sockaddr *)&sock2_name, &namelength);
printf("Accept Socket: %08X\n",sock_3);
#ifdef VMS
printf("chan: %08X\n",si_get_sdc(sock_3));
#endif
	if (sock_3 == -1)
		{
		perror ("accept");
		cleanup( 2, sock_2, sock_3);
		}

#endif
	/*
	 * Call cleanup to shutdown and close sockets.
	 */
	cleanup(2, sock_3, sock_2);

 } /* end main */

