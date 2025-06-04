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
 *     mail.c -    routines related to vms mail.
 *
 * Description:
 *     This module contains routines related to vms mail.
 *
 * Routines:
 *
 ************************************************************************/

/************************************************************************
 * #include's
 ************************************************************************/
#include "iufingerd.h"


/************************************************************************
 * Function:
 *     CheckMail - print newmail message.
 *
 * Description:
 *     Determines if the user has new mail.
 *
 * Arguments:
 *     userData - user data structure.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void CheckMail(struct user_data *userData)
{
    short newMessageCount = 0;
    u_long context = 0;
    u_long status;

    itemlist nullist[] = { {0,0,0,0} };

    itemlist inlist[] =
    {
      { strlen(userData->username), MAIL$_USER_USERNAME, userData->username, 0},
      { 0, MAIL$_NOSIGNAL, 0, 0 },
      { 0, 0, 0, 0 },
    };

    itemlist outlist[] =
    {
      { sizeof(newMessageCount), MAIL$_USER_NEW_MESSAGES, &newMessageCount, 0 },
      { sizeof(userData->forward)-1, MAIL$_USER_FORWARDING, 
          &userData->forward, 0 },
      { 0, MAIL$_NOSIGNAL, 0, 0 },
      { 0, 0, 0, 0 },
    };

    /*** open a user context ***/
    status = mail$user_begin(&context, nullist, nullist);
    if (VmsError(status))
        SystemLog("mail$user_begin: %s", VmsMessage(status));

    /** get new message count ***/
    status = mail$user_get_info(&context, inlist, outlist);
    if (VmsError(status))
        SystemLog("mail$user_get_info: %s", VmsMessage(status));
    if (debugFlag) SystemLog("newMessageCount = %d", newMessageCount);

    /*** set the flag value ***/
    userData->hasNewMail = (newMessageCount > 0);

    /*** close the user context ***/
    status = mail$user_end(&context, nullist, nullist);
    if (VmsError(status))
        SystemLog("mail$user_end: %s", VmsMessage(status));
}
