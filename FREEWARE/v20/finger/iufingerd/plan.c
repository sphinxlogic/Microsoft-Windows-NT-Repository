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
 *     plan.c - routines related to the plan and project files.
 *
 * Description:
 *     This module contains routines related to the plan and project files.
 *
 * Routines:
 *     Plan -      print the plan file.
 *     Project -   print the project file.
 *
 ************************************************************************/


/************************************************************************
 * #include's
 ************************************************************************/
#include "iufingerd.h"


/************************************************************************
 * Function:
 *     Plan - print the plan file.
 *
 * Description:
 *     This routine prints the plan file.
 *
 * Arguments:
 *     sd       - socket to which to write.
 *     userData - user data structure
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void Plan(int sd, struct user_data *userData)
{
  char buffer[256];
  char fileName[256];
  int  charCount = 0;
  int  status;
  int  count;
  int  bytes;
  int  plan;

  if (debugFlag) SystemLog("Plan()");

  /* Be paranoid about the pointers */
  if (!userData)          return;
  if (!userData->homeDir) return;

  /* Open the plan file if it exists */
  for (count=0; planNames[count][0]; count++)
  {
    sprintf(fileName, "%s%s", userData->homeDir, planNames[count]);
    if ((plan = open(fileName, O_RDONLY)) != -1) break;
  }

  if (plan == -1)
  {
    sprintf(buffer, "No Plan.\r\n");
    nwrite(sd, buffer, strlen(buffer));
    return;
  }

  if (debugFlag) SystemLog("plan: %s", fileName);
  sprintf(buffer, "Plan:\r\n");
  nwrite(sd, buffer, strlen(buffer));

  /* Print the contents of the file */
  while(bytes = read(plan, buffer, sizeof(buffer)))
  {
    charCount += bytes;
    if ((bytes = nwrite(sd, buffer, bytes)) <= 0) break;
    if (charCount >= MAX_PLAN_CHARS) break;
  }
  close(plan);
}


/************************************************************************
 * Function:
 *     Project - print the project file.
 *
 * Description:
 *     This routine prints the project file.
 *
 * Arguments:
 *     sd       - socket to which to write.
 *     userData - user data structure
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void Project(int sd, struct user_data *userData)
{
  char buffer[256];
  char fileName[256];
  int  charCount = 0;
  int  status;
  int  count;
  int  bytes;
  int  project;

  if (debugFlag) SystemLog("Project()");

  /* Be paranoid about the pointers */
  if (!userData)          return;
  if (!userData->homeDir) return;

  /* Open the project file if it exists */
  for (count=0; projectNames[count][0]; count++)
  {
    sprintf(fileName, "%s%s", userData->homeDir, projectNames[count]);
    if ((project = open(fileName, O_RDONLY)) != -1) break;
  }

  if (project == -1)
  {
    return;
  }

  if (debugFlag) SystemLog("project: %s", fileName);
  sprintf(buffer, "Project: ");
  nwrite(sd, buffer, strlen(buffer));

  /* Print at most 256 characters of the first line of the file */
  if ((bytes = read(project, buffer, sizeof(buffer))) > 0)
    nwrite(sd, buffer, ((buffer[bytes-1] == '\n') ? bytes-1 : bytes));

  /* Follow with a CRLF */
  nwrite(sd, "\r\n", 2);
  close(project);
}
