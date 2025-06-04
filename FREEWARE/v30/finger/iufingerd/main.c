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
 *     main.c - main module.
 *
 * Description:
 *     The main module.
 *
 * Routines:
 *     AppMainLoop - all the real stuff starts here.
 *     main -        entry point into the program.
 *
 ***********************************************************************
 *
 * Program History:
 *
 *     Revision 3.0 93/10/22  hughes & harvey
 *     Nearly a complete rewrite.  Create a QIO-driven asynchronous event
 *     loop, to fix the bug that caused server to hang in V2.x.  Add
 *     support for all CLD command line options and associated features.
 *
 *     Revision 2.1  93/02/16  hughes
 *     Set keepalive on socket to try to avoid hanging on broken
 *     connections, say "No plan." when there is none, plus a few
 *     other minor changes.
 *
 *     Revision 2.0  92/11/25  hughes
 *     Major changes to optimize performance.  Highlights:  better
 *     logging facility, caching of usernames and home directories,
 *     calling hoggy sys$getuai() only when absolutely necessary.
 *
 *     Revision 1.4  92/09/22  15:48:06  cfraizer
 *     Modified main() to work as a -real- server.
 *
 *     Revision 1.3  92/09/22  15:21:45  cfraizer
 *     Added AppMainLoop().
 *
 *     Revision 1.2  92/09/17  14:49:04  cfraizer
 *     Miscellaneous changes.
 *
 *     Revision 1.1  92/09/15  21:39:22  cfraizer
 *     Initial revision
 *
 ************************************************************************/

/************************************************************************
 * #include's
 ************************************************************************/
#include "iufingerd.h"
#include "version.h"

#ifdef UCX
#include <ucx$inetdef.h>
#else
#include <sys/ioctl.h>
#endif

/************************************************************************
 * global variables
 ************************************************************************/

int attnState;

struct IOSB connectIosb;

int portNumber;

struct client_data clientData;
struct user_data   userData;
struct jpiInfo     jpiConfig;

int userCacheSize;
int userCacheTtl;

int hostCacheSize;
int hostCacheTtl;

time_t lastPurgeTime;

int debugFlag;
int sortFlag;
int sortPosition;
int sortSize;

int rfc931Flag;
int rfc931Timeout;

int headerFlag;
int systatFlag;
int planFlag;
int projectFlag;
int mailFlag;
int resolveFlag;
int homeDirFlag;
int loginFlag;

int purgeInterval;

int timeFormat;

char planNames[MAX_PLAN_NAMES+1][MAX_PLAN_FILENAME+1];
char projectNames[MAX_PLAN_NAMES+1][MAX_PLAN_FILENAME+1];

/************************************************************************
 * Function:
 *     AppMainLoop - all the real stuff starts here.
 *
 * Description:
 *     This routine just loops, and dispatches work to the appropriate
 *     routines.
 *
 * Arguments:
 *     sd -        the socket on which to operate.
 *
 * Returns:
 *     Never returns.
 ************************************************************************/
void AppMainLoop(int sd)
{
  int serving;
  int status;
  time_t now;

  sys$setast(0);
  serving = IssueNewConnectQio(sd);
  sys$setast(1);

  while (serving)
  {
    status = sys$hiber();
    if (VmsError(status))
    {
      SystemLog("sys$hiber: %s", VmsMessage(status));
      serving = FALSE;
      break;
    }

    switch (attnState)
    {
      case ATTN_CONNECT:
        if (debugFlag) SystemLog("ATTN_CONNECT");
        memset(&userData,   '\0', sizeof(userData));
        memset(&clientData, '\0', sizeof(clientData));
        if (NetworkAcceptConnection(sd, &clientData))
        {
          IssueReadDataQio();
          IssueReadTimeoutQio();
        }
        else
          serving = FALSE;
        break;

      case ATTN_TIMEOUT:
        if (debugFlag) SystemLog("ATTN_TIMEOUT");
        SystemLog("timeout waiting for input");
        NetworkCloseConnection(&clientData);
        serving = IssueNewConnectQio(sd);
        break;

      case ATTN_DISCONNECT:
        if (debugFlag) SystemLog("ATTN_DISCONNECT");
        CancelReadTimeoutQio();
        NetworkCloseConnection(&clientData);
        serving = IssueNewConnectQio(sd);
        break;

      case ATTN_DATA:
        if (debugFlag) SystemLog("ATTN_DATA");
        if (!clientData.readComplete)
          IssueReadDataQio();
        else
        {
          CancelReadTimeoutQio();
          ProcessRequest();
          NetworkCloseConnection(&clientData);
          time(&now);
          if (((int)now - (int)lastPurgeTime) > purgeInterval)
          {
            PurgeCaches(userCacheTtl, hostCacheTtl);
            lastPurgeTime = now;
          }
          serving = IssueNewConnectQio(sd);
        }
        break;

      default:
        SystemLog("invalid attnState: %d", attnState);
        serving = FALSE;
        break;
    }
  }
  nclose(sd);
}


/************************************************************************
 * Function:
 *     ProcessRequest
 *
 * Description:
 *     Process a client finger request.
 *
 * Arguments:
 *     None.

 * Returns:
 *     Nothing.
 ************************************************************************/
ProcessRequest()
{
  int fuip;
  int status;
  static int one = 1;
  static char unsupported[] =
      "%IUFINGERD-F-UNSUPPORTED, unsupported query type\r\n";

  if (debugFlag) SystemLog("entering ProcessRequest()");

  /* first, set socket to non-blocked I/O */
  status = ioctl(clientData.socket, FIONBIO, (long *)&one);
  if (status == -1) SystemLog("error: ioctl: %d", errno);

  /* second, make the size something sane */
  if (strlen(clientData.request) > SIZE_USERNAME)
    clientData.request[SIZE_USERNAME] = '\0';

  Tokenize(clientData.request, clientData.username);
  fuip = FType(clientData.username);
  switch(fuip)
  {
    case FUIP_Q1:
      UpCase(clientData.username);
      if (debugFlag) SystemLog("query user '%s'", clientData.username);
      strcpy(userData.username, clientData.username);
      FSingleQuery(clientData.socket, &userData);
      break;

    case FUIP_C:
      if (debugFlag) SystemLog("query list");
      FListQuery(clientData.socket);
      break;

    case FUIP_CLOSED:
      if (debugFlag) SystemLog("no query");
      break;

    case FUIP_TIMEOUT:
      if (debugFlag) SystemLog("read timeout");
      break;

    case FUIP_Q2:
    case FUIP_Q1W:
    default:
      SystemLog("unsupported query, fuip == %d", fuip);
      nwrite(clientData.socket, unsupported, strlen(unsupported));
      break;
  }

  if (debugFlag) SystemLog("exiting ProcessRequest()");
}


/************************************************************************
 * Function:
 *     main - entry point into the program.
 *
 * Description:
 *     This routien contains the entry point into the program.
 *
 * Arguments:
 *     argc -   argument counter.
 *     argv -   argument vector.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
int main(int argc, char *argv[])
{
  int sd;

  bzero(&jpiConfig, sizeof(jpiConfig));

  ProcessArgs();

  SystemLog("IUFINGERD version %s starting", VERSION);

  InitCaches(userCacheSize, hostCacheSize);
  time(&lastPurgeTime);

  if ((sd = NetworkOpen(portNumber)) != -1)
    AppMainLoop(sd);
}
