/*
 ***********************************************************************
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
 */

#include "iufingerd.h"
#include <climsgdef.h>


/************************************************************************
 * Function:
 *     ProcessArgs
 *
 * Description:
 *     Process command arguments.
 *
 * Arguments:
 *     None.

 * Returns:
 *     Nothing.
 ************************************************************************/
void ProcessArgs()
{
  int status;
  int order = 1;
  int planCount = 0;
  int projectCount = 0;
  unsigned short cliLength;
  char value[256];
  $DESCRIPTOR(valueDesc, value);
  char *tmp;

  /* these must match the parameter labels in the .CLD file */
  $DESCRIPTOR(debugDesc,     "DEBUG");
  $DESCRIPTOR(faoDesc,       "FAO");
  $DESCRIPTOR(headerDesc,    "HEADER");
  $DESCRIPTOR(homeDirDesc,   "HOME_DIRECTORY");
  $DESCRIPTOR(hostCacheDesc, "HOST_CACHE");
  $DESCRIPTOR(jpiDesc,       "JPI");
  $DESCRIPTOR(loginDesc,     "LAST_LOGIN");
  $DESCRIPTOR(mailDesc,      "MAIL_CHECK");
  $DESCRIPTOR(planDesc,      "PLAN");
  $DESCRIPTOR(portDesc,      "PORT");
  $DESCRIPTOR(projectDesc,   "PROJECT");
  $DESCRIPTOR(purgeDesc,     "PURGE_INTERVAL");
  $DESCRIPTOR(resolveDesc,   "RESOLVE_ADDRESSES");
  $DESCRIPTOR(rfcDesc,       "RFC931");
  $DESCRIPTOR(sortDesc,      "SORT");
  $DESCRIPTOR(timefmtDesc,   "TIME_FORMAT");
  $DESCRIPTOR(titleDesc,     "TITLE");
  $DESCRIPTOR(userCacheDesc, "USER_CACHE");

  /* /DEBUG */
  status = cli$present(&debugDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      debugFlag = FALSE;
      break;
    case CLI$_PRESENT:
      debugFlag = TRUE;
      SystemLog("debugFlag = 1");
      break;
    case CLI$_NEGATED:
      debugFlag = FALSE;
      break;
    default:
      exit(status);
  }

  /* /FAO=string */
  status = cli$present(&faoDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      strcpy(jpiConfig.fao, "!12AZ !30AZ !15AZ !AZ");
      break;
    case CLI$_PRESENT:
      status = cli$get_value(&faoDesc, &valueDesc, &cliLength);
      if (VmsError(status)) exit(status);
      value[cliLength] = '\0';
      strcpy(jpiConfig.fao, value);
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("fao = \"%s\"", jpiConfig.fao);

  /* /HEADER */
  status = cli$present(&headerDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      headerFlag = TRUE;
      systatFlag = FALSE;
      break;
    case CLI$_PRESENT:
      headerFlag = TRUE;
      systatFlag = FALSE;
      status = cli$get_value(&headerDesc,  &valueDesc, &cliLength);
      if (VmsError(status))
        break;
      value[cliLength] = '\0';
      if (strncmp(value, "SYSTAT", cliLength) == 0)
        systatFlag = TRUE;
      break;
    case CLI$_NEGATED:
      headerFlag = FALSE;
      systatFlag = FALSE;
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("headerFlag = %d", headerFlag);
  if (debugFlag) SystemLog("systatFlag = %d", systatFlag);

  /* /HOME_DIRECTORY */
  status = cli$present(&homeDirDesc);
  switch (status)
  {
    case CLI$_ABSENT:
    case CLI$_PRESENT:
      homeDirFlag = TRUE;
      break;
    case CLI$_NEGATED:
      homeDirFlag = FALSE;
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("homeDirFlag = %d", homeDirFlag);

  /* /HOST_CACHE=(SIZE=size,TTL=ttl) */
  status = cli$present(&hostCacheDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      hostCacheSize = 100;
      hostCacheTtl  = 86400;
      break;
    case CLI$_PRESENT:
      do
      {
        status = cli$get_value(&hostCacheDesc, &valueDesc, &cliLength);
        switch (status)
        {
          case SS$_NORMAL:
          case CLI$_COMMA:
            value[cliLength] = '\0';
            tmp = strchr(value, '=');
            if (tmp != (char *)0)
            {
              *tmp++ = '\0';
              if (strncmp(value, "SIZE", strlen(value)) == 0)
                hostCacheSize = atoi(tmp);
              else if (strncmp(value, "TTL", strlen(value)) == 0)
                hostCacheTtl = atoi(tmp);
            }
            break;

          default:
            exit(status);
        }
      } while (status == CLI$_COMMA);
      break;

    default:
      exit(status);
  }
  if (debugFlag) SystemLog("hostCacheSize = %d, hostCacheTtl = %d",
                       hostCacheSize, hostCacheTtl);

  /* /JPI=(list) */
  status = cli$present(&jpiDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      jpiConfig.fields.username  = 1;
      jpiConfig.fields.uafowner  = 2;
      jpiConfig.fields.remote    = 3;
      jpiConfig.fields.loginTime = 4;
      if (debugFlag)
      {
        SystemLog("jpiFields[0] = USERNAME");
        SystemLog("jpiFields[1] = UAFOWNER");
        SystemLog("jpiFields[2] = REMOTE");
        SystemLog("jpiFields[3] = LOGINTIM");
      }
      break;
    case CLI$_PRESENT:
      bzero(&jpiConfig.fields, sizeof(jpiConfig.fields));
      do
      {
        status = cli$get_value(&jpiDesc, &valueDesc, &cliLength);
        switch (status)
        {
          case SS$_NORMAL:
          case CLI$_COMMA:
            value[cliLength] = '\0';
            if (strncmp(value, "IMAGNAME", cliLength) == 0)
            {
              jpiConfig.fields.image = order;
              if (debugFlag) SystemLog("jpiFields[%d] = IMAGNAME", order);
            }
            else if (strncmp(value, "LOGINTIM", cliLength) == 0)
            {
              jpiConfig.fields.loginTime = order;
              if (debugFlag) SystemLog("jpiFields[%d] = LOGINTIM", order);
            }
            else if (strncmp(value, "UAFOWNER", cliLength) == 0)
            {
              jpiConfig.fields.uafowner = order;
              if (debugFlag) SystemLog("jpiFields[%d] = UAFOWNER", order);
            }
            else if (strncmp(value, "PID", cliLength) == 0)
            {
              jpiConfig.fields.pid = order;
              if (debugFlag) SystemLog("jpiFields[%d] = PID", order);
            }
            else if (strncmp(value, "PRCNAM", cliLength) == 0)
            {
              jpiConfig.fields.procName = order;
              if (debugFlag) SystemLog("jpiFields[%d] = PRCNAM", order);
            }
            else if (strncmp(value, "REMOTE", cliLength) == 0)
            {
              jpiConfig.fields.remote = order;
              if (debugFlag) SystemLog("jpiFields[%d] = REMOTE", order);
            }
            else if (strncmp(value, "TERMINAL", cliLength) == 0)
            {
              jpiConfig.fields.terminal = order;
              if (debugFlag) SystemLog("jpiFields[%d] = TERMINAL", order);
            }
            else if (strncmp(value, "USERNAME", cliLength) == 0)
            {
              jpiConfig.fields.username = order;
              if (debugFlag) SystemLog("jpiFields[%d] = USERNAME", order);
            }
            order++;
            break;

          default:
            exit(status);
        }
      } while (status == CLI$_COMMA);
      break;

    default:
      exit(status);
  }

  /* /LAST_LOGIN */
  status = cli$present(&loginDesc);
  switch (status)
  {
    case CLI$_ABSENT:
    case CLI$_PRESENT:
      loginFlag = TRUE;
      break;
    case CLI$_NEGATED:
      loginFlag = FALSE;
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("loginFlag = %d", loginFlag);

  /* /MAIL_CHECK */
  status = cli$present(&mailDesc);
  switch (status)
  {
    case CLI$_ABSENT:
    case CLI$_PRESENT:
      mailFlag = TRUE;
      break;
    case CLI$_NEGATED:
      mailFlag = FALSE;
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("mailFlag = %d", mailFlag);

  /* /PLAN=name */
  status = cli$present(&planDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      planFlag = TRUE;
      strcpy(planNames[0], "PLAN.TXT");
      strcpy(planNames[1], "\0");
      if (debugFlag) SystemLog("planNames[0] = PLAN.TXT");
      break;
    case CLI$_NEGATED:
      planFlag = FALSE;
      planNames[0][0] = '\0';
      if (debugFlag) SystemLog("no plan files will be displayed");
      break;
    case CLI$_PRESENT:
      planFlag = TRUE;
      do
      {
        status = cli$get_value(&planDesc, &valueDesc, &cliLength);
        switch (status)
        {
          case SS$_NORMAL:
          case CLI$_COMMA:
            value[cliLength] = '\0';
            strcpy(planNames[planCount], value);
            if (debugFlag)
              SystemLog("planNames[%d] = %s", planCount, planNames[planCount]);
            planCount++;
            break;
          default:
            exit(status);
        }
      } while ((status == CLI$_COMMA) && (planCount < MAX_PLAN_NAMES));
      planNames[planCount][0] = '\0';
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("planFlag = %d", planFlag);

  /* /PORT=port */
  status = cli$present(&portDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      portNumber = FINGER_PORT;
      break;
    case CLI$_PRESENT:
      status = cli$get_value(&portDesc, &valueDesc, &cliLength);
      if (VmsError(status)) exit(status);
      value[cliLength] = '\0';
      portNumber = atoi(value);
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("portNumber = %d", portNumber);

  /* /PROJECT=name */
  status = cli$present(&projectDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      projectFlag = TRUE;
      strcpy(projectNames[0], "PROJECT.TXT");
      strcpy(projectNames[1], "\0");
      if (debugFlag) SystemLog("projectNames[0] = PROJECT.TXT");
      break;
    case CLI$_NEGATED:
      projectFlag = FALSE;
      projectNames[0][0] = '\0';
      if (debugFlag) SystemLog("no project files will be displayed");
      break;
    case CLI$_PRESENT:
      projectFlag = TRUE;
      do
      {
        status = cli$get_value(&projectDesc, &valueDesc, &cliLength);
        switch (status)
        {
          case SS$_NORMAL:
          case CLI$_COMMA:
            value[cliLength] = '\0';
            strcpy(projectNames[projectCount], value);
            if (debugFlag)
              SystemLog("projectNames[%d] = %s", projectCount, projectNames[projectCount]);
            projectCount++;
            break;
          default:
            exit(status);
        }
      } while ((status == CLI$_COMMA) && (projectCount < MAX_PLAN_NAMES));
      projectNames[projectCount][0] = '\0';
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("projectFlag = %d", projectFlag);

  /* /PURGE_INTERAL=interval */
  status = cli$present(&purgeDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      purgeInterval = 14400;
      break;
    case CLI$_PRESENT:
      status = cli$get_value(&purgeDesc, &valueDesc, &cliLength);
      if (VmsError(status)) exit(status);
      value[cliLength] = '\0';
      purgeInterval = atoi(value);
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("purgeInterval = %d", purgeInterval);

  /* /RESOLVE_ADDRESSES */
  status = cli$present(&resolveDesc);
  switch (status)
  {
    case CLI$_ABSENT:
    case CLI$_PRESENT:
      resolveFlag = TRUE;
      break;
    case CLI$_NEGATED:
      resolveFlag = FALSE;
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("resolveFlag = %d", resolveFlag);

  /* /RFC931=TIMEOUT=timeout */
  rfc931Timeout = 10;
  status = cli$present(&rfcDesc);
  switch (status)
  {
    case CLI$_ABSENT:
    case CLI$_NEGATED:
      rfc931Flag = FALSE;
      break;
    case CLI$_PRESENT:
      rfc931Flag = TRUE;
      status = cli$get_value(&rfcDesc, &valueDesc, &cliLength);
      switch (status)
      {
        case CLI$_ABSENT:
          break;
        case SS$_NORMAL:
          value[cliLength] = '\0';
          tmp = strchr(value, '=');
          if (tmp != (char *)0)
          {
            *tmp++ = '\0';
            if (strncmp(value, "TIMEOUT", strlen(value)) == 0)
              rfc931Timeout = atoi(tmp);
          }
          break;
        default:
          exit(status);
      }
      break;
    default:
      exit(status);
  }
  if (debugFlag)
    SystemLog("rfc931Flag = %d, timeout = %d", rfc931Flag, rfc931Timeout);

  /* /SORT=(POSITION=position,SIZE=size) */
  sortPosition = 0;
  sortSize     = 80;
  status = cli$present(&sortDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      sortFlag = TRUE;
      break;
    case CLI$_NEGATED:
      sortFlag = FALSE;
      break;
    case CLI$_PRESENT:
      sortFlag = TRUE;
      do
      {
        status = cli$get_value(&sortDesc, &valueDesc, &cliLength);
        switch (status)
        {
          case CLI$_ABSENT:
            break;
          case SS$_NORMAL:
          case CLI$_COMMA:
            value[cliLength] = '\0';
            tmp = strchr(value, '=');
            if (tmp != (char *)0)
            {
              *tmp++ = '\0';
              if (strncmp(value, "POSITION", strlen(value)) == 0)
                sortPosition = atoi(tmp);
              else if (strncmp(value, "SIZE", strlen(value)) == 0)
                sortSize = atoi(tmp);
            }
            break;
          default:
            exit(status);
        }
      } while (status == CLI$_COMMA);
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("sortFlag = %d, sortPosition = %d, sortSize = %d",
                           sortFlag, sortPosition, sortSize);

  /* /TIME_FORMAT=keyword */
  status = cli$present(&timefmtDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      timeFormat = TIME_NUMERIC;
      break;
    case CLI$_PRESENT:
      status = cli$get_value(&timefmtDesc, &valueDesc, &cliLength);
      if (VmsError(status)) exit(status);
      value[cliLength] = '\0';
      if (strncmp(value, "NUMERIC", cliLength) == 0)
	timeFormat = TIME_NUMERIC;
      else if (strncmp(value, "TEXT", cliLength) == 0)
	timeFormat = TIME_TEXT;
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("timeFormat = %d", timeFormat);

  /* /TITLE=string */
  status = cli$present(&titleDesc);
  switch (status)
  {
    case CLI$_ABSENT:
    case CLI$_NEGATED:
      strcpy(jpiConfig.title, "");
      break;
    case CLI$_PRESENT:
      status = cli$get_value(&titleDesc, &valueDesc, &cliLength);
      if (VmsError(status)) exit(status);
      value[cliLength] = '\0';
      strcpy(jpiConfig.title, value);
      break;
    default:
      exit(status);
  }
  if (debugFlag) SystemLog("title = \"%s\"", jpiConfig.title);

  /* /USER_CACHE=(SIZE=size,TTL=ttl) */
  status = cli$present(&userCacheDesc);
  switch (status)
  {
    case CLI$_ABSENT:
      userCacheSize = 100;
      userCacheTtl  = 86400;
      break;
    case CLI$_PRESENT:
      do
      {
        status = cli$get_value(&userCacheDesc, &valueDesc, &cliLength);
        switch (status)
        {
          case SS$_NORMAL:
          case CLI$_COMMA:
            value[cliLength] = '\0';
            tmp = strchr(value, '=');
            if (tmp != (char *)0)
            {
              *tmp++ = '\0';
              if (strncmp(value, "SIZE", strlen(value)) == 0)
                userCacheSize = atoi(tmp);
              else if (strncmp(value, "TTL", strlen(value)) == 0)
                userCacheTtl = atoi(tmp);
            }
            break;

          default:
            exit(status);
        }
      } while (status == CLI$_COMMA);
      break;
    default:
      exit(status);
  }
  if (debugFlag)
    SystemLog("userCacheSize = %d, userCacheTtl = %d", userCacheSize, userCacheTtl);

}
