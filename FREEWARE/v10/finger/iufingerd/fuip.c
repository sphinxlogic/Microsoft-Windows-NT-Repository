/************************************************************************
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
 *     fuip.c - finger user information protocol.
 *
 * Description:
 *     This module contains routines related to the finger user
 *     information protocol.
 *
 * Routines:
 *     Fao -               format ASCII output.
 *     FListQuery -        write information about each process.
 *     FSingleQuery -      respond to a single query.
 *     FType -             return the query type.
 *     ListHeader -        write list header.
 *     SystatListHeader -  write systat-style list header.
 *
 ************************************************************************/

/************************************************************************
 * #include's
 ************************************************************************/
#include "iufingerd.h"
#include <syidef.h>
#include <lnmdef.h>

/************************************************************************
 * defines
 ************************************************************************/
#define SOR$GK_RECORD   1
#define SOR$M_STABLE    1
#define SOR$M_NODUPS   64
#define SOR$M_SEQ_CHECK 4
#define SOR$M_NOSIGNAL  8
#define SOR$K_WRK_ALQ  11


/************************************************************************
 * Function:
 *     Fao
 *
 * Description:
 *     Format ASCII output.
 *
 * Arguments:
 *     job       - jpi information
 *     outString - output string
 *     outSize   - max size of outString
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void Fao(struct jpiData *job, char *outString, int outSize)
{
  int status;
  static int outLength;
  $DESCRIPTOR(outDesc,     outString);
  $DESCRIPTOR(controlDesc, jpiConfig.fao);
  struct jpiData thisJob;
  static char *p[18];
  static int  firstTime = TRUE;

  controlDesc.dsc$w_length = strlen(jpiConfig.fao);
  outDesc.dsc$w_length = outSize;
  outString[0] = '\0';

  memcpy(&thisJob, job, sizeof(struct jpiData));

  if (firstTime)
  {
    int count;
    firstTime = FALSE;
    bzero(p, sizeof(p));
    for (count=1; count<=17; count++)
    {
      if (jpiConfig.fields.image == count)
        p[count] = thisJob.image;
      else if (jpiConfig.fields.loginTime == count)
        p[count] = thisJob.numericTimeString;
      else if (jpiConfig.fields.uafowner == count)
        p[count] = thisJob.realName;
      else if (jpiConfig.fields.pid == count)
        p[count] = thisJob.pidString;
      else if (jpiConfig.fields.procName == count)
        p[count] = thisJob.procName;
      else if (jpiConfig.fields.remote == count)
        p[count] = thisJob.remote;
      else if (jpiConfig.fields.terminal == count)
        p[count] = thisJob.terminal;
      else if (jpiConfig.fields.username == count)
        p[count] = thisJob.username;
    }
  }

  status = sys$fao(&controlDesc, &outLength, &outDesc,
     p[1],  p[2],  p[3],  p[4],  p[5],  p[6],  p[7],  p[8], p[9],
     p[10], p[11], p[12], p[13], p[14], p[15], p[16], p[17]);

  if (VmsError(status))
    SystemLog("sys$fao error: %s", VmsMessage(status));

  outString[outLength] = '\0';
  strcat(outString, "\r\n");
}

/************************************************************************
 * Function:
 *     FListQuery - write information about each process.
 *
 * Description:
 *     This routine scans the process list, then prints
 *     the list of information about the proceses.
 *
 * Arguments:
 *     sd - the socket on which to write the output.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void FListQuery(int sd)
{
  int bytes;
  int status;
  int error = FALSE;
  long jobCount = 0;
  long batchCount = 0;
  char outBuffer[512];
  u_long  pid = -1;
  u_short recordLen;
  struct jpiData job;
  struct user_data userData;

  struct dsc$descriptor_s bufferDesc =
    { sizeof(outBuffer), DSC$K_DTYPE_T, DSC$K_CLASS_S, outBuffer };

  sortBlock keyBuffer = { 1, DSC$K_DTYPE_T, 0, 0, 0 };
  int sortOptions = SOR$M_NOSIGNAL;
  char sortType   = SOR$GK_RECORD;
  char sortFiles  = 5;

  if (debugFlag) SystemLog("FListQuery()");

  /*** write the header and column titles now if we can ***/
  if (!systatFlag)
  {
    if (headerFlag) ListHeader(sd);
    if (*jpiConfig.title)
    {
      nwrite(sd, jpiConfig.title, strlen(jpiConfig.title));
      nwrite(sd, "\r\n", 2);
    }
  }

  if (sortFlag)
  {
    keyBuffer.offset = sortPosition;
    if (sortSize <= 0)
      keyBuffer.length = 80;
    else
      keyBuffer.length = sortSize;
    recordLen = sizeof(job);
    status = sor$begin_sort(&keyBuffer, &recordLen, &sortOptions,
                            0, 0, 0, &sortType, &sortFiles, 0);
    if (VmsError(status))
    {
      error = TRUE;
      sprintf(outBuffer, "SOR$BEGIN_SORT: %s", VmsMessage(status));
      SystemLog(outBuffer);
      strcat(outBuffer, "\r\n");
      nwrite(sd, outBuffer, strlen(outBuffer));
    }
  }

  while (!error)
  {
    status = GetJpi(&pid, &job);
    if (status == SS$_NOMOREPROC)
      break;
    else if (status == SS$_SUSPENDED)
      continue;
    else if (VmsError(status))
    {
      error = TRUE;
      sprintf(outBuffer, "SYS$GETJPI: %s", VmsMessage(status));
      SystemLog(outBuffer);
      strcat(outBuffer, "\r\n");
      nwrite(sd, outBuffer, strlen(outBuffer));
      break;
    }

    if (job.pid != job.masterPid) continue;
    if (job.mode == JPI$K_BATCH) batchCount++;
    if (job.mode != JPI$K_INTERACTIVE) continue;
    if (strcmp("<login>", job.username) == 0) continue;

    jobCount++;
    strcpy(userData.username, job.username);
    UaiRetrieve(&userData);
    job.realName = userData.realName;

    Fao(&job, outBuffer, sizeof(outBuffer));

    if (sortFlag)
    {
      status = sor$release_rec(&bufferDesc, 0);
      if (VmsError(status))
      {
        error = TRUE;
        sprintf(outBuffer, "SOR$RELEASE_REC: %s", VmsMessage(status));
        SystemLog(outBuffer);
        strcat(outBuffer, "\r\n");
        nwrite(sd, outBuffer, strlen(outBuffer));
        break;
      }
    }
    else if ((bytes = nwrite(sd, outBuffer, strlen(outBuffer))) <= 0)
      break;
  }

  if (sortFlag)
  {
    if (systatFlag)	/* Header is deferred until now for this case */
    {
      if (headerFlag) SystatListHeader(sd,jobCount,batchCount);
      if (*jpiConfig.title)
      {
        nwrite(sd, jpiConfig.title, strlen(jpiConfig.title));
        nwrite(sd, "\r\n", 2);
      }
    }

    status = sor$sort_merge(0);
    if (VmsError(status))
    {
      error = TRUE;
      sprintf(outBuffer, "SOR$SORT_MERGE: %s", VmsMessage(status));
      SystemLog(outBuffer);
      strcat(outBuffer, "\r\n");
      nwrite(sd, outBuffer, strlen(outBuffer));
    }

    while (status == SS$_NORMAL)
    {
      status = sor$return_rec(&bufferDesc, 0, 0);
      if (status == SS$_ENDOFFILE)
        break;
      else if (VmsError(status))
      {
        error = TRUE;
        sprintf(outBuffer, "SOR$RETURN_REC: %s", VmsMessage(status));
        SystemLog(outBuffer);
        strcat(outBuffer, "\r\n");
        nwrite(sd, outBuffer, strlen(outBuffer));
        break;
      }
      else if ((bytes = nwrite(sd, outBuffer, strlen(outBuffer))) <= 0)
        break;
    }

    status = sor$end_sort(0);
    if (VmsError(status))
    {
      error = TRUE;
      sprintf(outBuffer, "SOR$END_SOR: %s", VmsMessage(status));
      SystemLog(outBuffer);
      strcat(outBuffer, "\r\n");
      nwrite(sd, outBuffer, strlen(outBuffer));
    }
  }


  if (!systatFlag && !error)
  {
    sprintf(outBuffer, "\r\nTotal of %d users logged on.\r\n", jobCount);
    nwrite(sd, outBuffer, strlen(outBuffer));
  }
}


/************************************************************************
 * Function:
 *     FSingleQuery - respond to a single query.
 *
 * Description:
 *     This routine responds to single-user queries.
 *
 * Arguments:
 *     sd       - the socket descriptor.
 *     userData - pointer to the user data structure.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void FSingleQuery(int sd, struct user_data *userData)
{
  char outBuffer[132];

  if (debugFlag) SystemLog("FSingleQuery()");

if (debugFlag) SystemLog("calling UaiRetrieve()");
  UaiRetrieve(userData);
if (debugFlag) SystemLog("back from UaiRetrieve()");

  sprintf(outBuffer, "Login name: %s\t\t\t", userData->username);
  nwrite(sd, outBuffer, strlen(outBuffer));
  sprintf(outBuffer, "In real life: %s\r\n", userData->realName);
  nwrite(sd, outBuffer, strlen(outBuffer));

  if (userData->validUser)
  {
    if (homeDirFlag)
    {
      sprintf(outBuffer, "Directory:  %s\r\n", userData->homeDir);
      nwrite(sd, outBuffer, strlen(outBuffer));
    }

if (debugFlag) SystemLog("calling LoggedIn()");
    if (!LoggedIn(userData))
    {
      if (userData->wasCached && loginFlag) 
{
if (debugFlag) SystemLog("calling LastLogin()");
        LastLogin(userData);
if (debugFlag) SystemLog("back from LastLogin()");
}
    }
if (debugFlag) SystemLog("back from LoggedIn()");

    if (*userData->onSince[0])
    {
      int onCount;
      for (onCount=0;
           *userData->onSince[onCount] && (onCount<MAX_ONSINCE);
           onCount++)
      {
        sprintf(outBuffer, "On since: %s\r\n", userData->onSince[onCount]);
        nwrite(sd, outBuffer, strlen(outBuffer));
      }
    }
    else if (*userData->lastLogin)
    {
      sprintf(outBuffer, "Last login: %s\r\n", userData->lastLogin);
      nwrite(sd, outBuffer, strlen(outBuffer));
    }

    if (mailFlag)
    {
if (debugFlag) SystemLog("calling CheckMail()");
      CheckMail(userData);
if (debugFlag) SystemLog("back from CheckMail()");
      sprintf(outBuffer, "Has %snew mail.\r\n",
              (userData->hasNewMail ? "" : "no "));
      nwrite(sd, outBuffer, strlen(outBuffer));

      if (*userData->forward)
      {
        sprintf(outBuffer, "Forward: %s\r\n", userData->forward);
        nwrite(sd, outBuffer, strlen(outBuffer));
      }
    }

if (debugFlag) SystemLog("calling Project()");
    if (projectFlag) Project(sd, userData);

if (debugFlag) SystemLog("calling Plan()");
    if (planFlag) Plan(sd, userData);

if (debugFlag) SystemLog("leaving FSingleQuery()");
  }
}


/************************************************************************
 * Function:
 *     FType - return the query type.
 *
 * Description:
 *     This routine looks at a query and returns the type
 *     of query.
 *
 * Arguments:
 *     theQuery -   the character string query.
 *
 * Returns:
 *     One of:
 *         FUIP_Q1 -       single query.
 *         FUIP_Q2 -       relay query.
 *         FUIP_C -        list query.
 ************************************************************************/
int FType(char *theQuery)
{
    int     rc;

    if (!strlen(theQuery))
        rc = FUIP_C;
    else if (strchr(theQuery, '@'))
        rc = FUIP_Q2;
    else
        rc = FUIP_Q1;

    return(rc);
}


/************************************************************************
 * Function:
 *     ListHeader
 *
 * Description:
 *     Write list header to finger client.
 *
 * Arguments:
 *     sd - socket descriptor
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void ListHeader(int sd)
{
  int status;
  char node[17];
  char hostname[65];
  struct times
  {
    int  value[2];
    char buffer[23];
  } now, boot, diff;
  $DESCRIPTOR(now_desc,  now.buffer);
  $DESCRIPTOR(boot_desc, boot.buffer);
  $DESCRIPTOR(diff_desc, diff.buffer);
  struct item_list outlist[] =
  {
    { sizeof(node),       SYI$_NODENAME, node,       0 },
    { sizeof(boot.value), SYI$_BOOTTIME, boot.value, 0 },
    { 0, 0, 0, 0 },
  };
  char days[6];
  char hours[12];
  char outBuffer[512];

  /* Get the current time */
  lib$date_time(&now_desc);
  lib$convert_date_string(&now_desc, now.value, 0, 0, 0, 0);

  /* Get the boot time and node name */
  bzero(node, sizeof(node));
  status = sys$getsyiw(0, 0, 0, outlist, 0, 0, 0);
  lib$sys_asctim(0, &boot_desc, boot.value, 0);
  hostname[0] = '\0';
  gethostname(hostname, sizeof(hostname));

  /* Subtract current and boot time to get difference */
  lib$sub_times(now.value, boot.value, diff.value);
  lib$sys_asctim(0, &diff_desc, diff.value, 0);
  sscanf(diff.buffer, "%s %s", days, hours);

  /* Print initial information */
  hours[5] = now.buffer[17] = boot.buffer[17] = '\0';
  sprintf(outBuffer, "Node:\t%s (%s)\r\n", node, hostname);
  nwrite(sd, outBuffer, strlen(outBuffer));
  sprintf(outBuffer, "Date:\t%s\r\n", now.buffer);
  nwrite(sd, outBuffer, strlen(outBuffer));
  sprintf(outBuffer, "Uptime:\t%s day%c, %s hours (since %s)\r\n",
         days, (days==1 ? '\0' : 's'), hours, boot.buffer);
  nwrite(sd, outBuffer, strlen(outBuffer));

#if defined(WINS) || defined(MULTINET)
  /*** Read and write the load averages ***/
  {
    int averages, bytes;
    struct
    {
      float loadAvg[3];
      float prioAvg[3];
      float queueAvg[3];
    } avgData;

    /* $$VMS_LOAD_AVERAGE points to LAV0: */
    if (averages = open("$$VMS_LOAD_AVERAGE", O_RDONLY))
    {
      bytes = read(averages, (char *)&avgData, sizeof(avgData));
      if (bytes)
      {
        sprintf(outBuffer, "Load:\t%.2f, %.2f, %.2f\r\n",
          avgData.loadAvg[0], avgData.loadAvg[1], avgData.loadAvg[2]);
        nwrite(sd, outBuffer, strlen(outBuffer));
      }
      close(averages);
    }
  }
#endif

  nwrite(sd, "\r\n", 2);
}


/************************************************************************
 * Function:
 *     SystatListHeader
 *
 * Description:
 *     Write systat-style list header to finger client.
 *
 * Arguments:
 *     sd - socket descriptor
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void SystatListHeader(int sd, int interactiveJobs, int batchJobs)
{
  int status;
  static unsigned long now_context = 0;	/* Must be static */
  int now[2], boot[2], uptime[2], len;
  char now_s[80];
  char zone_s[4];
  char uptime_s[17], *uptime_p;
  char node[16];
  char hostname[65];
  $DESCRIPTOR(now_desc, now_s);
  $DESCRIPTOR(uptime_desc, uptime_s);
  static const $DESCRIPTOR(now_format,"|!WC, !MAC !DD, !Y4|!HH2:!M0!MIU|");
  static const $DESCRIPTOR(system_table,"LNM$SYSTEM");
  static const $DESCRIPTOR(zone_logical,TIME_ZONE_LOGICAL);
  struct item_list lnmlist[] =
  {
    { sizeof(zone_s)-1, LNM$_STRING, zone_s, 0 },
    { 0, 0, 0, 0 },
  };
  struct item_list syilist[] =
  {
    { sizeof(node)-1, SYI$_NODENAME, node, 0 },
    { sizeof(boot), SYI$_BOOTTIME, boot, 0 },
    { 0, 0, 0, 0 },
  };
  char outBuffer[512];


  /* We only need to initialize the context once. */
  if (!now_context)
  {
    status = lib$init_date_time_context(
    		&now_context,
    		&LIB$K_OUTPUT_FORMAT,
    		&now_format);
    if (VmsError(status))
      exit(status);
  }

  /* Get current time, convert to string */
  status = sys$gettim(now);
  if (VmsError(status))
    exit(status);
  status = lib$format_date_time(&now_desc, now, &now_context, &len, 0);
  if (VmsError(status))
    exit(status);
  now_s[len] = '\0';

  /* Try to get the timezone name, else set to null */
  bzero(zone_s, sizeof(zone_s));
  status = sys$trnlnm(0, &system_table, &zone_logical, 0, lnmlist);
  if (VmsError(status))
    zone_s[0] = '\0';

  /* Get the boot time and node name */
  bzero(node, sizeof(node));
  status = sys$getsyiw(0, 0, 0, syilist, 0, 0, 0);
  hostname[0] = '\0';
  gethostname(hostname, sizeof(hostname));

  /* Subtract current and boot time to get system uptime */
  uptime[0] = uptime[1] = -1;
  lib$sub_times(now, boot, uptime);
  bzero(uptime_s, sizeof(uptime_s));
  sys$asctim(0, &uptime_desc, uptime, 0);
  for (uptime_p = uptime_s;  isspace(*uptime_p);  uptime_p++)
    ;					/* Zap leading blanks */
  uptime_s[13] = '\0';			/* Zap hundredths of seconds too */

  /* Print initial information */
  if (*zone_s)
    sprintf(outBuffer, "%s-%s   Up %s\r\n", now_s, zone_s, uptime_p);
  else
    sprintf(outBuffer, "%s   Up %s\r\n", now_s, uptime_p);
  nwrite(sd, outBuffer, strlen(outBuffer));

  sprintf(outBuffer,"%d+%d Jobs on %s (%s)",
		interactiveJobs, batchJobs, node, hostname);

#if defined(WINS) || defined(MULTINET)
  /*** Read and write the load averages ***/
  {
    int averages, bytes;
    struct
    {
      float loadAvg[3];
      float prioAvg[3];
      float queueAvg[3];
    } avgData;

    /* $$VMS_LOAD_AVERAGE points to LAV0: */
    if (averages = open("$$VMS_LOAD_AVERAGE", O_RDONLY))
    {
      bytes = read(averages, (char *)&avgData, sizeof(avgData));
      if (bytes)
      {
        sprintf(outBuffer+strlen(outBuffer), "  Load ave  %.2f %.2f %.2f",
          avgData.loadAvg[0], avgData.loadAvg[1], avgData.loadAvg[2]);
      }
      close(averages);
    }
  }
#endif

  strcat(outBuffer+strlen(outBuffer),"\r\n\r\n");
  nwrite(sd, outBuffer, strlen(outBuffer));
}
