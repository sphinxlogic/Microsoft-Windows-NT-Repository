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
 *     jpi.c - routines related to jobs and processes.
 *
 * Description:
 *     This module contains routine related to jobs and
 *     processes on the vms machine.
 *
 * Routines:
 *     FixRemote   - fix remote string
 *     GetJpi      - get JPI information
 *     LoggedIn    - is user logged in?
 *
 ************************************************************************/

/************************************************************************
 * #include's
 ************************************************************************/
#include "iufingerd.h"


/************************************************************************
 * Function:
 *     FixRemote - fix the remote string.
 *
 * Description:
 *     This routine fixes the "remote" (login from) string by:
 *         1) Removes trailing spaces.
 *         2) Removes port number from ip addresses.
 *
 * Arguments:
 *     string - the "remote" string.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void FixRemote(char *remote)
{
  int     dotCount = 0;
  char   *front, *back;
  char    tmpBuffer[256];

  strcpy(tmpBuffer, remote);
  front = tmpBuffer;

#ifdef UCX

  /* "Host: aaa.bbb.ccc.ddd     Port ppp" */
  if (strncmp("Host: ", front, 6) == 0) front +=6;
  if (!isdigit(*front)) return;
  back = front;
  while (*back)
  {
    if (isspace(*back))
    {
      *back = '\0';
      break;
    }
    back++;
  }

#elif WINS

  /* "aaa.bbb.ccc.ddd.ppp" */
  if (!isdigit(*front)) return;
  back = front;
  while (*back)
  {
    if (*back == '.')
    {
      if (++dotCount == 4)
      {
        *back = '\0';
        break;
      }
    }
    back++;
  }

#elif MULTINET

  /* "[aaa.bbb.ccc.ddd]" */
  if (*front != '[') return;
  front++;
  back = front + strlen(front) - 1;
  *back = '\0';

#else /* TCPWARE */

  /* "(aaa.bbb.ccc.ddd)" */
  if (*front != '(') return;
  front++;
  back = front + strlen(front) - 1;
  *back = '\0';

#endif

  strcpy(remote, front);
}


/************************************************************************
 * Function:
 *     GetJpi
 *
 * Description:
 *     Get job process infomration.
 *
 * Arguments:
 *     pid - process ID
 *     job - jpi data structure
 *
 * Returns:
 *     Return code from sys$getjpiw().
 ************************************************************************/
int GetJpi(int *pid, struct jpiData *job)
{
#ifndef JPI$_TT_ACCPORNAM
#define JPI$_TT_ACCPORNAM 813
#endif
  int  status;
  char *tmp;
  struct item_list outlist[] =
  {
    { sizeof(job->username),  JPI$_USERNAME,     job->username,   0 },
    { sizeof(job->terminal),  JPI$_TERMINAL,     job->terminal,   0 },
    { sizeof(job->image),     JPI$_IMAGNAME,     job->image,      0 },
    { sizeof(job->remote),    JPI$_TT_ACCPORNAM, job->remote,     0 },
    { sizeof(job->loginTime), JPI$_LOGINTIM,     job->loginTime,  0 },
    { sizeof(job->pid),       JPI$_PID,          &job->pid,       0 },
    { sizeof(job->masterPid), JPI$_MASTER_PID,   &job->masterPid, 0 },
    { sizeof(job->type),      JPI$_JOBTYPE,      &job->type,      0 },
    { sizeof(job->mode),      JPI$_MODE,         &job->mode,      0 },
    { sizeof(job->procName),  JPI$_PRCNAM,       &job->procName,  0 },
    { 0, 0, 0, 0 },
  };

  status = sys$getjpiw(0, pid, 0, outlist, 0, 0, 0);
  if (!VmsError(status))
  {
    char *s;

    /* trim spaces from username */
    if (tmp = strchr(job->username, ' '))
      *tmp = '\0';

    /* zap garbage in process name */
    for (s = job->procName;  *s;  s++)
      if (iscntrl(*s)) *s = '.';

    /* fix the remote */
    if (!*job->remote)
    {
      if (strncmp(job->terminal, "OP", 2) == 0)
        strcpy(job->remote, "VAX console");
    }
    else
    {
      FixRemote(job->remote);
      if (resolveFlag)
      {
        struct host_cache_entry hostEntry;
        hostEntry.IPaddress = inet_addr(job->remote);
        if ((int)hostEntry.IPaddress != -1)
        {
          if (!HostCached(&hostEntry))
          {
            ResolveRemote(hostEntry.IPaddress, hostEntry.hostName);
            hostEntry.rfc931Status = RFC931_MAYBE;
            CacheHost(&hostEntry);
          }
          strcpy(job->remote, hostEntry.hostName);
        }
      }
    }

    /* fix the image name from "sys$somedisk:[somewhere]some.exe" to "some" */
    TrimImage(job->image);

    /* fix the login time */
    switch (timeFormat)
    {
      case TIME_TEXT:
        WayCoolTime(job->loginTime, job->numericTimeString);
        break;
      default:
        NumericTime(job->loginTime, job->numericTimeString);
        break;
    }

    VmsTime(job->loginTime, job->vmsTimeString);
    sprintf(job->pidString, "%X", job->pid);
  }
  return(status);
}


/************************************************************************
 * Function:
 *     LoggedIn - is user logged in?
 *
 * Description:
 *     This routine returns prints some information and
 *     returns true if a user is logged in; otherwise, it
 *     returns false.
 *
 * Arguments:
 *
 * Returns:
 *     TRUE  -  if user is logged on (and it printed stuff).
 *     FALSE -  otherwise.
 ************************************************************************/
int LoggedIn(struct user_data *userData)
{
  long status;
  long pid = -1;
  int  loggedIn = FALSE;
  int  working = TRUE;
  int  onCount = 0;
  struct jpiData job;

  /*** scan the process list ***/
  while (working  && (onCount < MAX_ONSINCE))
  {
    status = GetJpi(&pid, &job);
    switch(status)
    {
      case SS$_NORMAL:
        if (strcmp(job.username, userData->username) == 0)
        {
          if ((job.type != JPI$K_BATCH) &&
              (job.type != JPI$K_DETACHED) &&
              (job.pid == job.masterPid) &&
              (job.mode == JPI$K_INTERACTIVE))
          {
            loggedIn = TRUE;
            if (loginFlag)
	      if (*job.remote)
                sprintf(userData->onSince[onCount++], "%s on %s from %s",
                  job.vmsTimeString, job.terminal, job.remote);
	      else
                sprintf(userData->onSince[onCount++], "%s on %s",
                  job.vmsTimeString, job.terminal);
          }
        }
        break;

      case SS$_SUSPENDED:
        break;

      case SS$_NOMOREPROC:
        working = FALSE;
        break;

      default:
        working = FALSE;
        SystemLog("sys$getjpiw error: %s", VmsMessage(status));
        break;
    }
  }
  return(loggedIn);
}
