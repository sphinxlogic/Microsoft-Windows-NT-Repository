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
 *     qio.c - routines related to QIOs
 *
 * Routines:
 *     CancelReadTimeoutQio
 *     IssueNewConnectQio
 *     IssueReadDataQio
 *     IssueReadTimeoutQio
 *     ReadAst
 *     ReadTimeoutAst
 *
 ************************************************************************/

#include "iufingerd.h"

/************************************************************************
 * Function:
 *     CancelReadTimeoutQio
 *
 * Description:
 *     Cancels the timer on the network read.
 *
 * Arguments:
 *     None.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void CancelReadTimeoutQio()
{
  int status;

  if (debugFlag) SystemLog("CancelReadTimeoutQio()");
  status = sys$cantim(0, 0);
  if (VmsError(status))
    SystemLog("sys$cantim: %s", VmsMessage(status));
};

/************************************************************************
 * Function:
 *     IssueNewConnectQio
 *
 * Description:
 *     Issues the asynchronous QIO for the notification of a new connect.
 *
 * Arguments:
 *     sd - network socket descriptor
 *
 * Returns:
 *     TRUE if successful, FALSE if failed.
 ************************************************************************/
int IssueNewConnectQio(int sd)
{
  int status;

  if (debugFlag) SystemLog("IssueNewConnectQio()");
#ifdef UCX
  status = sys$qio(0, (unsigned)vaxc$get_sdc(sd),
                  IO$_SETMODE | IO$M_READATTN, &connectIosb, 0, 0,
                  &NetworkConnectAst, 0, 0, 0, 0, 0);
#else
  status = sys$qio(0, sd, IO$_ACCEPT_WAIT, &connectIosb,
                   &NetworkConnectAst, 0, 0, 0, 0, 0, 0, 0);
#endif
  if (VmsError(status))
  {
    SystemLog("error: new connect qio: %s", VmsMessage(status));
    return(FALSE);
  }
  else
    return(TRUE);
}

/************************************************************************
 * Function:
 *     IssueReadDataQio
 *
 * Description:
 *     Issues the asynchronous QIO for the network read from client.
 *
 * Arguments:
 *     None.
 *
 * Returns:
 *     VMS status code returned from qio.
 ************************************************************************/
int IssueReadDataQio()
{
  int status;

  if (debugFlag) SystemLog("IssueReadDataQio()");
  memset(clientData.readBuffer, '\0', sizeof(clientData.readBuffer));
  status = sys$qio(0, clientData.channel, IO$_READVBLK, 
              &clientData.iosb, ReadAst, 0, clientData.readBuffer, 
              sizeof(clientData.readBuffer)-1, 0, 0, 0, 0);
  if (VmsError(status))
    SystemLog( "read sys$qio: %s", VmsMessage(status));
  return(status);
}

/************************************************************************
 * Function:
 *     IssueReadTimeoutQio
 *
 * Description:
 *     Issues a timer to trap 10 seconds of client silence on the read.
 *
 * Arguments:
 *     None.
 *
 * Returns:
 *     TRUE if successful, FALSE if failed.
 ************************************************************************/
int IssueReadTimeoutQio()
{
  int status;
  int value = FALSE;
  int interval[2];
  $DESCRIPTOR(time_desc,"0 00:00:10.00");

  if (debugFlag) SystemLog("IssueReadTimeoutQio()");
  status = sys$bintim(&time_desc, interval);
  if (VmsError(status))
    SystemLog("sys$bintim: %s", VmsMessage(status));
  else
  {
    status = sys$setimr(0, interval, ReadTimeoutAst, 0, 0);
    if (VmsError(status))
      SystemLog("sys$setimr: %s", VmsMessage(status));
    else
      value = TRUE;
  }
  return(value);
}

/************************************************************************
 * Function:
 *     ReadAst
 *
 * Description:
 *     Network read AST handler.
 *
 * Arguments:
 *     None.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void ReadAst()
{
  int size;

  if (debugFlag) SystemLog("ReadAst()");

  if (clientData.iosb.status == SS$_CANCEL)
  {
    attnState = ATTN_TIMEOUT;
  }
#ifdef UCX
  else if (clientData.iosb.status == SS$_LINKDISCON)
#else /* WINS */
  else if (clientData.iosb.terminator_offset == 0)
#endif
  {
    SystemLog("abnormal disconnect");
    attnState = ATTN_DISCONNECT;
  }
  else
  { 
    if (clientData.iosb.status != SS$_NORMAL)
      SystemLog("read iosb error: %s", VmsMessage(clientData.iosb.status));

    size = strlen(clientData.request) + strlen(clientData.readBuffer);
    if (size < sizeof(clientData.request)-1)
    {
      strcat(clientData.request, clientData.readBuffer);
      if (strchr(clientData.readBuffer, '\n') != (char *)0) 
        clientData.readComplete = TRUE;
    }
    else
    {
      size = sizeof(clientData.request) - strlen(clientData.request) - 2;
      clientData.readBuffer[size] = '\0';
      strcat(clientData.request, clientData.readBuffer);
      clientData.readComplete = TRUE;
    }

    attnState = ATTN_DATA;
  }
  sys$wake(0, 0);
}

/************************************************************************
 * Function:
 *     ReadTimeoutAst
 *
 * Description:
 *     Routine that traps timeout on network read to client.
 *
 * Arguments:
 *     None.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void ReadTimeoutAst()
{
  /* Just cancel, and let ReadAst() handle the event later */
  if (debugFlag) SystemLog("ReadTimeoutAst()");
  sys$cancel(clientData.channel);
}

