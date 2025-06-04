/*	client.c							*/
/*	V1.1			20-Jan-1995	IfN/Mey			*/
/*+
 * Example for TCP client program.
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
*	This is an example of a TCP/IP client using the IPC 
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
#include  <socket.h>
#include  <in.h>
#include  <netdb.h>		/* change hostent to comply with BSD 4.3*/
#include  <inet.h>
#ifdef UCX	/* IfN/Mey */
#include  <ucx$inetdef.h>	/* INET symbol definitions */
#define si_get_sdc vaxc$get_sdc
#else
#include "[-]socketshr.h"
#endif
#else
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#endif


/*
*
*   MACRO DEFINITIONS
*
*/

#ifndef vms
#define TRUE 1
#define FALSE 0
#endif

void cleanup(int shut, int socket);

/*
* Functional Description
*
*	This example creates a socket of type SOCK_STREAM (TCP),
*	initiates a connection to the remote host, sends
*	a message to the remote host, and closes the connection.	
*	Error messages are printed to the screen.
*
*	IPC calls used: 
*	close
*	connect
*	gethostbyname
*	send
*	shutdown
*	socket	
*	getsockname
*	getpeername
*
*
* Formal Parameters
*	The client program expects two parameters:
*	hostname ... name of remote host 
*	portnumber ... port where remote host(server) is listening
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

        int     sock_1;               		/* socket */
static  char  	message[] = "Hi there.";
static 	struct  sockaddr_in sock2_name;    	/* Address struct for socket2.*/
        struct  hostent         hostentstruct;  /* Storage for hostent data.  */
        struct  hostent         *hostentptr;    /* Pointer to hostent data.   */
        static  char            hostname[256];  /* Name of local host.        */
	int	flag;
	int	retval;				/* helpful for debugging */
	int	shut = FALSE;			/* flag to cleanup */
	FILE	*fp;
	char	buff[256];			/* read buffer */
	char	*p;
	char	**pp;


	/*
	 * Check input parameters.
	 */
	if (argc != 3 )
		{
		printf("Usage: client hostname portnumber.\n");
		exit(1);
		}


	/*
	 * Open socket 1: AF_INET, SOCK_STREAM. 
	 */
        if ((sock_1 = socket (AF_INET, SOCK_STREAM, 0)) == -1) 
		{
                perror( "socket");
		exit(1);
		}
printf("sock_1 = %d (0x%08X)\n",sock_1,sock_1);
#ifdef VMS
printf("channel = %04X\n",si_get_sdc(sock_1));
#endif

	/*
	 *Get pointer to network data structure for socket 2 (remote host). 
	 */
        if ((hostentptr = gethostbyname (argv[1])) == NULL) 
		{
                perror( "gethostbyname");
		cleanup(shut, sock_1);
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
	sock2_name.sin_port = htons(atoi(argv[2]));
        sock2_name.sin_addr = * ((struct in_addr *) hostentstruct.h_addr);


	/*
	 * Connect socket 1 to sock2_name.
	 */
	retval = connect(sock_1, (struct sockaddr *)&sock2_name, sizeof (sock2_name));
	if (retval)
		{
		perror("connect");
		cleanup(shut, sock_1);
		}

printf("Connected.\n");
	/*
	 * get ports
	 */
	flag = sizeof(struct sockaddr_in);
	sock2_name.sin_port = 0;
	sock2_name.sin_addr.s_addr = 0;
	retval = getsockname(sock_1, (struct sockaddr *)&sock2_name, &flag);
	if (retval)
		{
		perror("getsockname");
		cleanup(shut, sock_1);
		}
printf("getsockname: port: %d, addr: %s, AF family: %d\n",
ntohs(sock2_name.sin_port),
/*inet_ntoa(sock2_name.sin_addr.s_addr),*/
inet_ntoa(sock2_name.sin_addr),
sock2_name.sin_family);

	flag = sizeof(struct sockaddr_in);
	sock2_name.sin_port = 0;
	sock2_name.sin_addr.s_addr = 0;
	retval = getpeername(sock_1, (struct sockaddr *)&sock2_name, &flag);
	if (retval)
		{
		perror("getpeername");
		cleanup(shut, sock_1);
		}
printf("getpeername: port: %d, addr: %s, AF family: %d\n",
ntohs(sock2_name.sin_port),
/*inet_ntoa(sock2_name.sin_addr.s_addr),*/
inet_ntoa(sock2_name.sin_addr),
sock2_name.sin_family);

fprintf(stderr,"Press any key "); fgetc(stdin); /**/

	/*
	 * Send message to socket 2.
	 */
	flag = 0;	/* maybe 0 or MSG_OOB */
	retval = send(sock_1, message ,sizeof (message), flag);
	if (retval < 0)
		{
		perror ("send");
		cleanup(shut, sock_1);
		}
#if 1
	/*
	 * Get a file pointer and read data via standard i/o
	 */
	fp = fdopen(sock_1, "r+");
	if (fp == NULL)
		{
		perror ("fdopen");
		cleanup(shut, sock_1);
		}
	printf ("feof=%d, ferror=%d\n",feof(fp), ferror(fp));
	p = fgets(buff, sizeof(buff), fp);
	printf ("feof=%d, ferror=%d, errno=%d\n\n",feof(fp), ferror(fp), errno);
	if (p == NULL)
		{
		perror ("fgets");
		cleanup(shut, sock_1);
		}
	printf ("{%s}", buff);
	fclose(fp);
#endif

	/*
	 * Call cleanup to shutdown and close socket.
	 */
	cleanup(shut, sock_1);

 } /* end main */

/*-----------------------------------------------------------*/

void cleanup(shut, socket)
int	shut;
int	socket;
	
{
	int	retval;

	/*
	 * Shutdown socket completely -- only if it was connected 
	 */
	if (shut) {
		retval = shutdown(socket,2);
		if (retval == -1)
			perror ("shutdown");
	}


	/*
	 * Close socket.
	 */
        retval = close (socket);
	if (retval)
		perror ("close");

	exit(1);

 } /* end main */

