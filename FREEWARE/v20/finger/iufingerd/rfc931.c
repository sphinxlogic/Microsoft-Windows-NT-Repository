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
 ***********************************************************************
 *
 * Module:
 *     rfc931.c - routines related to rfc931
 *
 * Routines:
 *     RemoteUsername
 *
 ************************************************************************/

#include "iufingerd.h"

#ifdef UCX
#include <ucx$inetdef.h>
#else
#include <sys/ioctl.h>
#endif

#ifdef TCPWARE

#undef accept
#undef bind
#undef socket
#undef listen
#undef connect
#undef select
#undef ioctl
#undef setsockopt
#undef nclose
#undef nread
#undef nwrite

#define nread  socket_read
#define nwrite socket_write
#define nclose socket_close

#endif TCPWARE

/************************************************************************
 * Function:
 *     RemoteUsername
 *
 * Description:
 *     Obtains the remote username if the system is running an RFC931 daemon.
 *
 * Arguments:
 *     remoteAddress
 *     remotePort
 *     localPort
 *
 * Returns:
 *     NULL if failed, pointer to username if succeeded.
 ************************************************************************/
char *RemoteUsername(u_long remoteAddress, int remotePort, int localPort)
{
  int bytes;
  int checkRemotePort, checkLocalPort;
  int mySocket;
  int numScanned;
  int numSelected;
  int wasCached;
  struct sockaddr_in server;
  char buffer[256];
  char *remoteUser = NULL;
  static char username[256];
  struct host_cache_entry hostEntry;
  fd_set ibits;
  struct timeval timeout;

  timeout.tv_sec  = rfc931Timeout;
  timeout.tv_usec = 0;

  bzero(&hostEntry, sizeof(hostEntry));
  hostEntry.IPaddress = remoteAddress;
  wasCached = HostCached(&hostEntry);
  if (wasCached)
  {
    /* If it was cached, check the rfc931 daemon status */
    if (debugFlag)
      SystemLog("host is cached, rfc931Status = %d", hostEntry.rfc931Status);
    if (hostEntry.rfc931Status == RFC931_NO) return(NULL);
  }
  else if (debugFlag)
    SystemLog("host is not yet cached");

  hostEntry.rfc931Status = RFC931_NO;

  server.sin_family = AF_INET;
  server.sin_port = htons(RFC931_PORT);
  memcpy(&server.sin_addr, &remoteAddress, 4);

  if ((mySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    SystemLog("error: socket: %d", errno);
  else if (connect(mySocket, (struct sockaddr *)&server,
                   sizeof(struct sockaddr_in)) == -1)
  {
    /* No daemon */
    if (debugFlag) SystemLog("no RFC931 daemon present");
  }
  else
  {
    /* Daemon is there, send it the query */
    sprintf(buffer, "%u,%u\r\n", remotePort, localPort);
    if (debugFlag) SystemLog(">rfc931: \"%-.*s\"", strlen(buffer)-2, buffer);
    bytes = nwrite(mySocket, buffer, strlen(buffer));
    if (bytes > 0)
    {
      /* Now read the query, but don't wait forever */
      FD_ZERO(&ibits);
      FD_SET(mySocket, &ibits);
      numSelected = select(FD_SETSIZE, &ibits, 0, 0, &timeout);
      if ((numSelected == 1) && (FD_ISSET(mySocket, &ibits)))
      {
        bzero(buffer, sizeof(buffer));
        bytes = nread(mySocket, buffer, sizeof(buffer));
        if (bytes > 0)
        {
	  /* Trim trailing CRLF off response */
	  while (bytes > 0 && iscntrl(buffer[bytes-1]))
            buffer[--bytes] = '\0';

          /* We got a reply, so make sure it's in the valid format */
          if (debugFlag) SystemLog("<rfc931: \"%s\"", buffer);
          numScanned = sscanf(buffer, "%d , %d : USERID :%*[^:]:%255s",
                              &checkRemotePort, &checkLocalPort, username);
          if ((numScanned == 3) &&
              (remotePort == checkRemotePort) &&
              (localPort == checkLocalPort))
          {
            hostEntry.rfc931Status = RFC931_YES;
            remoteUser = username;
          }
          else
            SystemLog("invalid reply from rfc931 daemon: %s", buffer);
        }
      }
      else if (debugFlag)
        SystemLog("select() timed out");
    }
  }

  nclose(mySocket);

  /* Now cache the host */
  if (!wasCached)
  {
    if (resolveFlag)
      ResolveRemote(remoteAddress, hostEntry.hostName);
    else
      strcpy( hostEntry.hostName, inet_ntoa(remoteAddress));
    CacheHost(&hostEntry);
  }

  return(remoteUser);
}
