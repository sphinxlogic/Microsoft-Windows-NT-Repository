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
 * Module:
 *     net.c - routines related to the network.
 *
 * Description:
 *     This module contains information related to the
 *     network.
 *
 * Routines:
 *     NetworkOpen             - open a socket.
 *     NetworkAcceptConnection - accept a network connection.
 *     NetworkCloseConnection  - close a network connection.
 *     NetworkConnectAst       - new connection AST handler.
 *     ResolveRemote           - resolve an IP address to a host name.
 *
 ************************************************************************/

/************************************************************************
 * #include's
 ************************************************************************/
#include "iufingerd.h"


/************************************************************************
 * Function:
 *     NetworkAcceptConnection - accept a network connection.
 *
 * Description:
 *     This routine accepts a connection on a socket.
 *
 * Arguments:
 *     sd -    the socket descriptor on which to accept.
 *
 * Returns:
 *     A new socket.
 ************************************************************************/
int NetworkAcceptConnection(int sd, struct client_data *clientData)
{
  int mySocket;
  int retval = FALSE;
  int status;
  int clientLength = sizeof(struct sockaddr_in);
  static int one = 1;
  static int socketBufferSize = 32768; /* 32K */

  clientData->socket = accept(sd, &clientData->sin, &clientLength);
  if (clientData->socket == -1) 
    SystemLog("error: accept: %d", errno);
  else
  {

    if (rfc931Flag)
    {
      char   *remoteUser;
      u_long remoteAddress;

      memcpy(&remoteAddress, &clientData->sin.sin_addr, sizeof(u_long));
      remoteUser = RemoteUsername(remoteAddress,
                     ntohs(clientData->sin.sin_port), portNumber);
      if (remoteUser == (char *)0) remoteUser = "unknown";
      SystemLog("request from %s %s", 
                inet_ntoa(clientData->sin.sin_addr), 
                remoteUser);
    }
    else
    {
      SystemLog("request from %s", inet_ntoa(clientData->sin.sin_addr)); 
    }

#ifdef UCX
    clientData->channel = vaxc$get_sdc(clientData->socket);
#else /* WINS || MULTINET || TCPWARE */
    clientData->channel = clientData->socket;
#endif UCX

    /* set keepalive */
    status = setsockopt(clientData->socket, SOL_SOCKET, SO_KEEPALIVE, 
                (char *)&one, sizeof(one));
    if (status == -1) SystemLog("error: setsockopt(keepalive): %d", errno);

    /* set socket buffer size */
    status = setsockopt(clientData->socket, SOL_SOCKET, SO_SNDBUF,
               (char *)&socketBufferSize, sizeof(socketBufferSize));
    if (status == -1) SystemLog("error: setsockopt(buffersize): %d", errno);

    retval = TRUE;
   }
  return(retval);
}

/************************************************************************
 * Function:
 *     NetworkCloseConnection
 ************************************************************************/
void NetworkCloseConnection(struct client_data *clientData)
{
  (void) sys$cancel(clientData->channel);
  nclose(clientData->socket);
}

/************************************************************************
 * Function:
 *     NetworkConnectAst - New connection AST handler
 ************************************************************************/
void NetworkConnectAst()
{
  if (VmsError(connectIosb.status))
    SystemLog("error: connect iosb: %s", VmsMessage(connectIosb.status));
  attnState = ATTN_CONNECT;
  sys$wake(0, 0);
}

/************************************************************************
 * Function:
 *     NetworkOpen - open a socket.
 *
 * Description:
 *     This routine opens a socket and starts the ball rolling,
 *     so to speak.
 *
 * Arguments:
 *     portNum -    the port number to use.
 *
 * Returns:
 *      -1 if error, or a socket descriptor.
 ************************************************************************/
int NetworkOpen(unsigned short portNum)
{
  int mySocket;
  int myStatus;
  struct sockaddr_in serverAddress;
 
  mySocket = socket(AF_INET, SOCK_STREAM, 0);
  if (mySocket == -1) 
    SystemLog("error: socket: %d", errno);
  else
  {
    bzero(&serverAddress, sizeof(struct sockaddr_in));
    serverAddress.sin_family =             AF_INET;
    serverAddress.sin_addr.s_addr =        htonl(INADDR_ANY);
    serverAddress.sin_port =               htons(portNum);
    myStatus = bind(mySocket, (struct sockaddr *)&serverAddress,
      sizeof(serverAddress));
    if (myStatus == -1)
    {
      SystemLog("error: bind: %d", errno);
      nclose(mySocket);
      mySocket = -1;
    }
    else
    {
      myStatus = listen(mySocket, SOMAXCONN);
      if (myStatus == -1)
      {
        SystemLog("error: listen: %d", errno);
        nclose(mySocket);
        mySocket = -1;
      }
    }
  }
  return(mySocket);
}

/************************************************************************
 * Function:
 *     ResolveRemote
 *
 * Description:
 *     This routine resolves remote name as IP address to FQDN hostname.
 *
 * Arguments:
 *     IPaddress  - the remote address to be resolved.
 *     remoteName - location to store resolved name (or ascii representation
 *                  of dotted decimal address.
 *
 * Returns:
 *     TRUE  - name was resolved successfully.
 *     FALSE - name was NOT resolved successfully.
 ************************************************************************/
int ResolveRemote(u_long IPaddress, char *remoteName)
{
  struct hostent *hp;

  hp = gethostbyaddr((char *)&IPaddress, sizeof(struct in_addr), AF_INET);
  if (hp != (struct hostent *)0)
  {
    strcpy(remoteName, hp->h_name);
    return(TRUE);
  }
  else
  {
    strcpy(remoteName, inet_ntoa(IPaddress));
    return(FALSE);
  }
}
