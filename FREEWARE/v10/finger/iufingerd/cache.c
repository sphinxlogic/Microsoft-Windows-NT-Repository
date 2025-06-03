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

USER_HASH_TABLE *userHashTable;
HOST_HASH_TABLE *hostHashTable;


/************************************************************************
 * Function:
 *     InitCaches
 *
 * Description:
 *     Initializes the username and hostname caches.
 *
 * Arguments:
 *     userTableSize -	desired username hash table size.
 *     hostTableSize -	desired hostname hash table size.
 *
 * Returns:
 *     True -   if caches were initialized
 *     False -  if it was not
 ************************************************************************/
int InitCaches(int userTableSize, int hostTableSize)
{
  if (debugFlag) SystemLog("creating user hash table");
  userHashTable = CreateUserHashTable(userTableSize);
  if (rfc931Flag || resolveFlag)
  {
    /* Create the host table only if resolving and/or doing rfc931 */
    if (debugFlag) SystemLog("creating host hash table");
    hostHashTable = CreateHostHashTable(hostTableSize);
    return(userHashTable != ((USER_HASH_TABLE *)0) &&
	   hostHashTable != ((HOST_HASH_TABLE *)0));
  }
  else
  {
    return(userHashTable != (USER_HASH_TABLE *)0);
  }
}


/************************************************************************
 * Function:
 *     CacheUser
 *
 * Description:
 *     Cache a username's UAI data in memory.
 *
 * Returns:
 *     True -   if the name is successfully cached.
 *     False -  if it's not successfully cached.
 ************************************************************************/
int CacheUser(struct user_cache_entry *userEntry)
{
  USER_HASH_ENTRY *entry;

  entry = UserHashInsert(userEntry, userHashTable);
  return(entry != (USER_HASH_ENTRY *)0);
}


/************************************************************************
 * Function:
 *     CacheHost
 *
 * Description:
 *     Cache's a remote host's FQDN data in memory.
 *
 * Returns:
 *     True -   if the name is successfully cached.
 *     False -  if it's not successfully cached.
 ************************************************************************/
int CacheHost(struct host_cache_entry *hostEntry)
{
  HOST_HASH_ENTRY *entry;

  entry = HostHashInsert(hostEntry, hostHashTable);
  return(entry != (HOST_HASH_ENTRY *)0);
}


/************************************************************************
 * Function:
 *     UserCached
 *
 * Description:
 *     Checks to see if a username's UAI data is cached in memory.
 *
 * Returns:
 *     True -   if the name is cached.
 *     False -  if it's not cached.
 ************************************************************************/
int UserCached(struct user_cache_entry *userEntry)
{
  int retval = FALSE;
  USER_HASH_ENTRY *entry;

  entry = UserHashFind(userEntry, userHashTable);
  if (entry)
  {
    retval = TRUE;
    memcpy(userEntry, &entry->cacheEntry, sizeof(struct user_cache_entry));
  }
  return(retval);
}


/************************************************************************
 * Function:
 *     HostCached
 *
 * Description:
 *     Checks to see if a remote host's FQDN data is cached in memory.
 *
 * Arguments:
 *     ipaddr   - pointer to applicable IP address in network byte order.
 *     hostName - field to copy remote host FQDN into, if it's cached.
 *
 * Returns:
 *     True -   if the address is cached.
 *     False -  if it's not cached.
 ************************************************************************/
int HostCached(struct host_cache_entry *hostEntry)
{
  int retval = FALSE;
  HOST_HASH_ENTRY *entry;

  entry = HostHashFind(hostEntry, hostHashTable);
  if (entry)
  {
    retval = TRUE;
    memcpy(hostEntry, &entry->cacheEntry, sizeof(struct host_cache_entry));
  }
  return(retval);
}


/************************************************************************
 * Function:
 *     PurgeCaches
 *
 * Description:
 *     Purge old entries from the username and hostname caches.
 *
 * Arguments:
 *     userTimeToLive -	maximum time-to-live for username hash table entries.
 *     hostTimeToLive -	maximum time-to-live for hostname hash table entries.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void PurgeCaches(int userTimeToLive, int hostTimeToLive)
{
  if (debugFlag) SystemLog("purging caches");

  if ((userTimeToLive > 0) && (userHashTable != (USER_HASH_TABLE *)0))
    PurgeUserHashTable(userTimeToLive, userHashTable);
  else if (debugFlag)
    SystemLog("skipping user cache purge");

  if ((hostTimeToLive > 0) && (hostHashTable != (HOST_HASH_TABLE *)0))
    PurgeHostHashTable(hostTimeToLive, hostHashTable);
  else if (debugFlag)
    SystemLog("skipping host cache purge");
}
