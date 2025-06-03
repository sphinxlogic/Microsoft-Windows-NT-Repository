/***********************************************************************
 *
 *  (C) Copyright 1992-1993 The Trustees of Indiana University
 *
 *   Permission to use, copy, modify, and distribute this program for
 *   non-commercial use and without fee is hereby granted, provided that
 *   this copyright and permission notice appear on all copies and
 *   supporting documentation, the name of Indiana University not be used
 *   in advertising or publicity pertaining to distribution of the program
 *   without specific prior permission, and notice be given in supporting
 *   documentation that copying and distribution is by permission of
 *   Indiana University.
 *
 *   Indiana University makes no representations about the suitability of
 *   this software for any purpose. It is provided "as is" without express
 *   or implied warranty.
 *
 ************************************************************************
 *
 * Program: IUFINGER.C
 *
 * Description:
 *     Finger client for VMS.
 *
 * History:
 *     V1.0 March 1991  cfraizer  Initial implementation.
 *     V1.1 Nov   1992  hughes    Disallow cluster-to-cluster fingers. See
 *                                  "#ifdef NOCLUSTER" code.
 *     V1.2 July  1993  hughes    Support for Multinet and TCPware.
 *
 ************************************************************************/

/********
 * #include's
 ********/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <ctype.h>
#include <string.h>
#ifdef WINS
#include "twg$tcp:[netdist.include]errno.h"
#endif WINS
#include <sys/time.h>

#include <ssdef.h>
#include <descrip.h>
#include <lnmdef.h>
#include <ssdef.h>
#include <psldef.h>

/********
 * #define's
 ********/
/* configuration stuff */
#define                  __STDC__

/* macros */
#ifdef __STDC__
#define _IU_PROTO(x)            x
#else
#define _IU_PROTO(x)            ()
#endif /* __STDC__ */

/* program constants */
#define BUFLEN           255
#define FINGER_PORT      79
#define TERMINATOR  "\015\012"

/* address types */
#define A_UNKNOWN         0
#define A_IPNUMBER        1
#define A_HOSTNAME        2

/* universal constants */
#define TRUE             1
#define FALSE            0

#ifdef WINS
#define nread  netread
#define nwrite netwrite
#define nclose netclose
#endif WINS

#ifdef MULTINET
#define nread  socket_read
#define nwrite socket_write
#define nclose socket_close
#endif MULTINET

#ifdef UCX
#define nread  read
#define nwrite write
#define nclose close
#endif UCX

#ifdef TCPWARE
#define nread  socket_read
#define nwrite socket_write
#define nclose socket_close
#endif TCPWARE

#ifdef NOCLUSTER
#define itemopen(ptr,itm) ptr = itm
#define itemadd(p,a,b,c,d) \
   p->buffer_length = a; p->item_code = b; \
   p->buffer_address = c; p->return_length_address = d; p++;
#define itemclose(ptr) itemadd(ptr,0,0,0,0);

#define vms_error(s) (!(s & 1))

typedef struct
{
  short buffer_length;
  short item_code;
  long  buffer_address;
  long return_length_address;
} ITEMLIST;

ITEMLIST outlist[10];
ITEMLIST *outlist_ptr;
#endif NOCLUSTER

/********
 * typedef's
 ********/

/********
 * global variables
 ********/
int options = 0;
extern int uerrno, verrno;
extern volatile int noshare errno;

/********
 * function prototypes
 ********/
void                  ClusterHelp       _IU_PROTO(());
int                   main              _IU_PROTO((int argc, char **argv));
int                   parse_arg         _IU_PROTO((char *arg));
void                  Quit              _IU_PROTO((void));
int                   set_options       _IU_PROTO((char *arg));

/********
 * Function:
 *     ClusterHelp - help for cluster-to-cluster information.
 *
 * Arguments:
 *     None.
 *
 * Returns:
 *     Nothing.
 ********/
void ClusterHelp()
{
  printf("\nFingering a VMS cluster machine for a list of users is an inefficient\n");
  printf("use of system and network resources.  Instead, please use a command\n");
  printf("like the following:\n\n");
  printf("   $ SHOW USERS/FULL\n");
  printf("   $ SHOW USERS/FULL/NODE=nodename\n\n");
  printf("If you need help with SHOW USERS, use this command:\n\n");
  printf("   $ HELP SHOW USERS\n\n");
  printf("Thank you.\n\n");
}

/********
 * Function:
 *     ContactRemoteServer - contact a remote finger server.
 *
 * Description:
 *     This routine handles the connecting to the remote server.
 *
 * Arguments:
 *     remoteUser -   username to finger.
 *     remoteHost -   host to contact.
 *
 * Returns:
 *     Nothing.
 ********/
void ContactRemoteServer(remoteUser, remoteHost)
char       *remoteUser;
char       *remoteHost;
{
    int                   addressType;
    int                   bytesRead;
    struct sockaddr_in    clientAddress;
    char                  localHostName[BUFLEN];
    char                  myBuffer[BUFLEN];
    struct hostent       *myHostEntry;
    struct sockaddr_in    serverAddress;
    int                   socketDescriptor;


    if (!*remoteHost) {
        gethostname(localHostName, sizeof(localHostName));
        localHostName[BUFLEN - 1] = '\0';
        remoteHost = localHostName;
    }

    if (isdigit(*remoteHost))
        addressType = A_IPNUMBER;
    else
        addressType = A_HOSTNAME;

    switch (addressType) {

    case A_IPNUMBER:
        memset((char *)&serverAddress, '\0', sizeof(serverAddress));
        serverAddress.sin_family        = AF_INET;
        serverAddress.sin_addr.s_addr   = inet_addr(remoteHost);
        serverAddress.sin_port          = htons(FINGER_PORT);

    /**
        myHostEntry = gethostbyaddr((char *)serverAddress,
            AF_INET, sizeof(serverAddress));
    **/
        break;

    case A_HOSTNAME:
        myHostEntry = gethostbyname(remoteHost);
        if (!myHostEntry) {
            fprintf(stderr, "iufinger: unknown host '%s'.\n", remoteHost);
            Quit();
        }

        memset((char *)&serverAddress, '\0', sizeof(serverAddress));
        serverAddress.sin_family        = AF_INET;
        memcpy((char *)&serverAddress.sin_addr,
               (char *)myHostEntry->h_addr,
               myHostEntry->h_length);
        serverAddress.sin_port          = htons(FINGER_PORT);
        break;

    case A_UNKNOWN:
    default:
        fprintf(stderr, "iufinger: unknown address type.\n");
        Quit();
        break;
    }


    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor < 0) {
        perror("socket");
        Quit();
    }

    if (connect(socketDescriptor,
        (struct sockaddr *)&serverAddress,
        sizeof(serverAddress)) < 0) {
        perror("connect");
        nclose(socketDescriptor);
        Quit();
    }

    if (nwrite(socketDescriptor, remoteUser, strlen(remoteUser)) < 0) {
        perror("net write");
        nclose(socketDescriptor);
        Quit();
    }

    if (nwrite(socketDescriptor, TERMINATOR, strlen(TERMINATOR)) < 0) {
        perror("net write");
        nclose(socketDescriptor);
        Quit();
    }

    if (addressType == A_HOSTNAME)
        printf("[%s]\n", myHostEntry->h_name);
    else
        printf("[%s]\n", remoteHost);

    while ((bytesRead = 
            nread(socketDescriptor, myBuffer, sizeof(myBuffer))) > 0) 
         write(1, myBuffer, bytesRead);

#if defined(WINS) || defined(MULTINET) || defined(TCPWARE)
    if (bytesRead < 0) perror("net read");
#endif

    nclose(socketDescriptor);
}

#ifdef NOCLUSTER
/* ======================================================================= */
/* Make Argv */
/* ======================================================================= */
void make_argv(string,argc,argv,delimiters)
char *string;
int  *argc;
char *argv[];
char *delimiters;
{
#define isdelimiter(a,b) (strchr(b,a) != NULL)
  register char *cpointer;
  register char **argpointer = argv;

  *argc = 0;
  cpointer = string;
  while (*cpointer)
  {
    while (*cpointer && isdelimiter(*cpointer,delimiters)) cpointer++;
    if (*cpointer == '\0') break;
    if (*cpointer == '\"')
    {
      cpointer++;
      *argpointer++ = cpointer;
      (*argc)++;
      while (*cpointer && (*cpointer != '\"')) cpointer++;
    }
    else
    {
      *argpointer++ = cpointer;
      (*argc)++;
      while (*cpointer && !isdelimiter(*cpointer,delimiters)) cpointer++;
    }
    if (*cpointer == '\0') break;
    *(cpointer++) = '\0';
  }
  *(argpointer++) = 0;
}

/* ====================================================================== */
/* Translate Logical Name */
/* ====================================================================== */
int trnlnm(char *logical_name, char *translated_name, char *name_table,
           int access_mode)
{
  int  xcase = LNM$M_CASE_BLIND;
  int  length = 0;
  int  status;
  int  retval = 0;
  unsigned char mode;
  $DESCRIPTOR(name_table_desc, name_table);
  $DESCRIPTOR(name_desc, logical_name);

  name_table_desc.dsc$w_length  = strlen(name_table);
  name_desc.dsc$w_length        = strlen(logical_name);
  mode = (unsigned char)access_mode;

  itemopen(outlist_ptr, outlist);
  itemadd(outlist_ptr, 255, LNM$_STRING, translated_name, &length);
  itemclose(outlist_ptr);
  status = sys$trnlnm(&xcase, &name_table_desc, &name_desc, &mode, &outlist);
  if (vms_error(status))
    exit(status);
  else if (length > 0)
  {
    retval = 1;
    *(translated_name+length) = '\0';
  }

  return(retval);
}
#endif NOCLUSTER

/********
 * main()
 ********/
int main(int argc, char **argv)
{
  int counter;

  if (argc == 1) {
#ifdef NOCLUSTER
    ClusterHelp();
#else
    ContactRemoteServer("", "");
#endif NOCLUSTER
    exit(0);
  }

#ifdef NOCLUSTER
  counter = 0;
  while (++counter < argc)
  {
    /* Is this an '@node' request? */
    if (argv[counter][0] == '@')
    {
      int i;
      int clusterCount = 0;
      char *clusterAddresses[64];
      char *address;
      char string[1024];

      /* Make the list of cluster IP addresses, from the logical name */
      if (!trnlnm("CLUSTER$ADDRESSES", string, "LNM$PROCESS", PSL$C_USER))
        exit(SS$_NOLOGNAM);
      make_argv(string, &clusterCount, clusterAddresses, " ");
      if (clusterCount <= 0)
        exit(SS$_IVLOGNAM);

      /* If the node to finger is given as a name, resolve it */
      address = &argv[counter][1];
      if (!isdigit(*address))
      {
        struct hostent *hp;
        struct in_addr ipaddr;
        if (hp = gethostbyname(address))
        {
          memcpy(&ipaddr, hp->h_addr_list[0], sizeof(ipaddr));
          address = inet_ntoa(ipaddr);
        }
      }

      /* If the node to finger is in the list of cluster addresses, quit */
      for (i=0; i<clusterCount; i++)
      {
        if (strcmp(address, clusterAddresses[i]) == 0)
        {
          ClusterHelp();
          exit(0);
        }
      }

    }
  }
#endif NOCLUSTER

  counter = 0;
  while (++counter < argc)
  {
    if (parse_arg(argv[counter]) < 0)
    {
      fprintf(stderr, "usage:  finger [options] [user[@node] [ user[@node]...]\n");
      exit(0);
    }
  }
}

/********
 * parse_arg()
 ********/
int parse_arg(char *arg)
{
  char *argp;
  int len;
  char username[20], node[65];
  char *up, *np;
  char remote = FALSE;

  argp = arg;
  if (*argp == '-') {
    if (set_options(argp++) < 0)
      return -1;
    else
      return 0;
  }

  up = username;
  while (*argp && *argp != '@' && *argp != ' ')
    *up++ = *argp++;
  *up = '\0';

  np = node;
  if (*argp && *argp != ' ') {
    remote = TRUE;
    argp++;
    while (*argp && *argp != ' ')
      *np++ = *argp++;
  }
  *np = '\0';
  if (remote)
    ContactRemoteServer(username, node);
  else
    ContactRemoteServer(username, node);
}

/********
 * Function:
 *     Quit -    exit routine for the program.
 *
 * Description:
 *     This routine handles "unnatural" exits from the "finger"
 *     client program.
 *
 * Arguments:
 *     None.
 *
 * Returns:
 *     Nothing.
 ********/
void Quit()
{
    exit(-1);
}

/********
 * set_options()
 ********/
int set_options(char *arg)
{
  while (*arg) {
    switch (*arg) {
    case ' ':
      return 0;
    default:
      options &= 0;
    }
    ++arg;
  }
}
