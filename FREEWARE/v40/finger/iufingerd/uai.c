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
 *     uai.c - routines related to the user authorization file.
 *
 * Description:
 *     This module contains routines related to the
 *     user authorization file.
 *
 * Routines:
 *     Last Login  - get last login.
 *     UaiRetrieve - get user UAI info.
 *
 ************************************************************************/


/************************************************************************
 * #include's
 ************************************************************************/
#include "iufingerd.h"

/************************************************************************
 * Function:
 *     LastLogin - retrieves last login time.
 *
 * Description:
 *     This routine retrieves the last login time.
 *
 * Arguments:
 *     userData - pointer to user data structure.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void LastLogin(struct user_data *userData)
{
  quadword lastLogin;
  long     uaiStatus;
  $DESCRIPTOR(user_desc, userData->username);
  itemlist outlist[] =
  {
    { sizeof(lastLogin), UAI$_LASTLOGIN_I, lastLogin, 0 },
    { 0, 0, 0, 0 }
  };

  user_desc.dsc$w_length = strlen(userData->username);
  uaiStatus = sys$getuai(0, 0, &user_desc, outlist, 0, 0, 0);
  if (VmsError(uaiStatus))
    SystemLog("sys$getuai: %s", VmsMessage(uaiStatus));
  else
    VmsTime(lastLogin, userData->lastLogin);
}


/************************************************************************
 * Function:
 *     UaiRetrieve - retrieve user UAI info.
 *
 * Description:
 *     This routine looks up information in the sysuaf.
 *
 * Arguments:
 *     userData - pointer to user data structure.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void UaiRetrieve(struct user_data *userData)
{
  char owner[80];
  char defDir[80];
  char defDevice[80];
  long uaiStatus;
  struct user_cache_entry userEntry;
  $DESCRIPTOR(user_desc, userData->username);
  quadword lastLogin;
  itemlist outlist[] =
  {
    { sizeof(owner),     UAI$_OWNER,       owner,     0 },
    { sizeof(defDevice), UAI$_DEFDEV,      defDevice, 0 },
    { sizeof(defDir),    UAI$_DEFDIR,      defDir,    0 },
    { sizeof(lastLogin), UAI$_LASTLOGIN_I, lastLogin, 0 },
    { 0, 0, 0, 0 }
  };

  if (debugFlag) SystemLog("UaiRetrieve()");

  strcpy(userEntry.username, userData->username);
  if (UserCached(&userEntry))
  {
    strcpy(userData->realName, userEntry.realName);
    strcpy(userData->homeDir,  userEntry.homeDir);
    userData->validUser = TRUE;
    userData->wasCached = TRUE;
    return;
  }

  user_desc.dsc$w_length = strlen(userData->username);
  uaiStatus = sys$getuai(0, 0, &user_desc, outlist, 0, 0, 0);
  if (VmsError(uaiStatus))
  {
    if (debugFlag || (uaiStatus != RMS$_RNF))
      SystemLog("%s", VmsMessage(uaiStatus));
    userData->validUser = FALSE;
    strcpy(userData->realName, "???");
  }
  else
  {
    int  size;
    char *s;

    userData->validUser = TRUE;
    userData->wasCached = FALSE;

    /*** null terminate pascal strings ***/
    size = owner[0];     *(owner     + 1 + size) = '\0';
    size = defDevice[0]; *(defDevice + 1 + size) = '\0';
    size = defDir[0];    *(defDir    + 1 + size) = '\0';

    /*** zap garbage in owner name ***/
    for (s = owner+1;  *s;  s++)
      if (iscntrl(*s))
	*s = '.';

    strcpy(userData->realName, owner+1);
    sprintf(userData->homeDir, "%s%s", defDevice+1, defDir+1);

    if (loginFlag)
      VmsTime(lastLogin, userData->lastLogin);

    strcpy(userEntry.realName, userData->realName);
    strcpy(userEntry.homeDir, userData->homeDir);
    CacheUser(&userEntry);
  }
}
