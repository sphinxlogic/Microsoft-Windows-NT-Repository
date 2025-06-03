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
 *      hash.c -    hashing routines for iufingerd.
 *
 * Description:
 *      This module contains routines used by iufingerd.
 *
 * Routines:
 *	UserHashFind -		Lookup a user in the username cache.
 *	HostHashFind -		Lookup a host in the hostname cache.
 *	UserHashInsert -	Insert a user in the username cache.
 *	HostHashInsert  -	Insert a host in the hostname cache.
 *	HashUser -		Hash a string (username).
 *	HashIPaddr -		Hash an IP (host) address.
 *	CreateUserHashTable -	Create the username hash table.
 *	CreateHostHashTable -	Create the hostname hash table.
 *      PurgeUserHashTable -	Purge old entries from the username hash table.
 *	PurgeHostHashTable -	Purge old entries from the hostname hash table.
 *
 ***********************************************************************
 */

#include "iufingerd.h"


/************************************************************************
 * Function:
 *     UserHashFind
 *
 * Description:
 *     Finds a user hash table entry, if it exists.
 *
 * Returns:
 *     Pointer to entry -   if the entry was found
 *     Null pointer -       if the entry was not found
 ************************************************************************/
USER_HASH_ENTRY *UserHashFind(struct user_cache_entry *userEntry, 
                              USER_HASH_TABLE *hash)
{
  USER_HASH_ENTRY *hashEntry;

  for (hashEntry = (hash->table)[HashUser(userEntry->username, hash)];
       hashEntry != NULL;
       hashEntry = hashEntry->nextEntry)
  {
    if (strcmp(hashEntry->cacheEntry.username, userEntry->username) == 0) 
      return(hashEntry);
  }
  return(NULL);
}


/************************************************************************
 * Function:
 *     HostHashFind
 *
 * Description:
 *     Finds a host hash table entry, if it exists.
 *
 * Returns:
 *     Pointer to entry -   if the entry was found
 *     Null pointer -       if the entry was not found
 ************************************************************************/
HOST_HASH_ENTRY *HostHashFind(struct host_cache_entry *hostEntry,
                             HOST_HASH_TABLE *hash)
{
  HOST_HASH_ENTRY *hashEntry;

  for (hashEntry = (hash->table)[HashIPaddr(hostEntry->IPaddress, hash)];
       hashEntry != (HOST_HASH_ENTRY *)0;
       hashEntry = hashEntry->nextEntry)
  {
    if (hashEntry->cacheEntry.IPaddress == hostEntry->IPaddress)
       return(hashEntry);
  }
  return(NULL);
}


/************************************************************************
 * Function:
 *     UserHashInsert
 *
 * Description:
 *     Inserts an entry into the username hash table
 *
 * Returns:
 *     True -   if the entry was inserted
 *     False -  if the entry was not inserted
 ************************************************************************/
USER_HASH_ENTRY *UserHashInsert(struct user_cache_entry *userEntry,
                                USER_HASH_TABLE *hash)
{
  USER_HASH_ENTRY *hashEntry;
  int value;

  if ((hashEntry = UserHashFind(userEntry, hash)) == NULL)
  {
    if ((hashEntry = (USER_HASH_ENTRY *)malloc(sizeof(USER_HASH_ENTRY))) == NULL)
      return(NULL);
    memcpy(&hashEntry->cacheEntry, userEntry, sizeof(struct user_cache_entry));
    time(&hashEntry->timeStamp);
    value = HashUser(userEntry->username, hash);
    hashEntry->nextEntry = (hash->table)[value];
    (hash->table)[value] = hashEntry;
    if (debugFlag) 
      SystemLog("user cache insert %04d %s", value, userEntry->username);
  }
  return(hashEntry);
}


/************************************************************************
 * Function:
 *     HostHashInsert
 *
 * Description:
 *     Inserts an entry into the hash table
 *
 * Returns:
 *     True -   if the entry was inserted
 *     False -  if the entry was not inserted
 ************************************************************************/
HOST_HASH_ENTRY *HostHashInsert(struct host_cache_entry *hostEntry,
                                HOST_HASH_TABLE *hash)
{
  HOST_HASH_ENTRY *hashEntry;
  int value;
  unsigned char *ipaddr_s;

  if ((hashEntry = HostHashFind(hostEntry, hash)) == NULL)
  {
    if ((hashEntry = (HOST_HASH_ENTRY *)malloc(sizeof(HOST_HASH_ENTRY))) == NULL)
      return(NULL);

    memcpy(&hashEntry->cacheEntry, hostEntry, sizeof(struct host_cache_entry));
    time(&hashEntry->timeStamp);
    value = HashIPaddr(hostEntry->IPaddress, hash);
    hashEntry->nextEntry = (hash->table)[value];
    (hash->table)[value] = hashEntry;
    ipaddr_s = (char *)&hostEntry->IPaddress;
    if (debugFlag) SystemLog("host cache insert %04d %d.%d.%d.%d", value,
                          ipaddr_s[0], ipaddr_s[1], ipaddr_s[2], ipaddr_s[3]);
  }
  return(hashEntry);
}


/************************************************************************
 * Function:
 *     HashUser
 *
 * Description:
 *     Convert a username string to a hash value.
 *
 * Arguments:
 *     string - character string
 *     hash   - address of hash table
 *
 * Returns:
 *     Hash value.
 ************************************************************************/
int HashUser(char *string, USER_HASH_TABLE *hash)
{
  int value;
  int coefficient;

  /* coefficient = sum of relative position in alphabet of first 2 chars */
  coefficient = *string + *(string+1) - 130;
  if (coefficient < 0) coefficient *= -1;

  for (value=0; *string != '\0'; string++)
    value += *string;

  value *= coefficient;

  return(value % (hash->tableSize-1));
}


/************************************************************************
 * Function:
 *     HashIPaddr
 *
 * Description:
 *     Convert a string to a hash value.
 *
 * Arguments:
 *     string - character string
 *     hash   - address of hash table
 *
 * Returns:
 *     Hash value.
 ************************************************************************/
int HashIPaddr(u_long IPaddress, HOST_HASH_TABLE *hash)
{
  unsigned char *string = (unsigned char *) &IPaddress;
  int i;
  int value;
  int net;
  unsigned char *net_s = (unsigned char *) &net;
  int coefficient;
  struct in_addr address;

  /* coefficient = sum of network number bytes */
  memcpy(&address, &IPaddress, sizeof(struct in_addr));
  net = inet_netof(address);
  coefficient = net_s[0] + net_s[1] + net_s[2] + net_s[3];
 
  if (coefficient < 0) coefficient *= -1;
 
  for (i = sizeof(struct in_addr), value=0; i > 0; i--)
    value += *string++;

  value *= coefficient;
  value %= hash->tableSize-1;

  return(value);
}


/************************************************************************
 * Function:
 *     CreateUserHashTable
 *
 * Description:
 *     Creates the username hash table
 *
 * Arguments:
 *     tableSize - size of table
 *
 * Returns:
 *     NULL if table could not be created.
 *     Pointer if table was created.
 ************************************************************************/
USER_HASH_TABLE *CreateUserHashTable(unsigned int tableSize)
{
  USER_HASH_TABLE  *tptr;
  USER_HASH_ENTRY **eptr;

  if ((tptr = (USER_HASH_TABLE *) malloc(sizeof(USER_HASH_TABLE))) == NULL ||
      (eptr = (USER_HASH_ENTRY **)calloc(tableSize, sizeof(USER_HASH_ENTRY *))) == NULL)
  {
    SystemLog("error: alloc for user hash table failed");
    return(NULL);
  }
  tptr->table     = eptr;
  tptr->tableSize = tableSize;
  if (debugFlag) SystemLog("user hash table pointers = %p, %p", tptr, eptr);
  return(tptr);
}


/************************************************************************
 * Function:
 *     CreateHostHashTable
 *
 * Description:
 *     Creates the hostname hash table
 *
 * Arguments:
 *     tableSize - size of table
 *
 * Returns:
 *     NULL if table could not be created.
 *     Pointer if table was created.
 ************************************************************************/
HOST_HASH_TABLE *CreateHostHashTable(unsigned int tableSize)
{
  HOST_HASH_TABLE  *tptr;
  HOST_HASH_ENTRY **eptr;

  if ((tptr = (HOST_HASH_TABLE *) malloc(sizeof(HOST_HASH_TABLE))) == NULL ||
      (eptr = (HOST_HASH_ENTRY **)calloc(tableSize, sizeof(HOST_HASH_ENTRY *))) == NULL)
  {
    SystemLog("error: alloc for host hash table failed");
    return(NULL);
  }
  tptr->table     = eptr;
  tptr->tableSize = tableSize;
  if (debugFlag) SystemLog("host hash table pointers = %p, %p", tptr, eptr);
  return(tptr);
}


/************************************************************************
 * Function:
 *     PurgeUserHashTable
 *
 * Description:
 *     Purge old entries from the username hash table
 *
 * Arguments:
 *     timeToLive -	number of seconds entries are allowed to live.
 *     hash -		pointer to username hash table to be purged.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void PurgeUserHashTable(int timeToLive, USER_HASH_TABLE *hash)
{
    auto int i;
    auto time_t now;
    auto int savedEntries, freedEntries;
    auto int maxLength, chainLength, lengthDist[4];
    auto USER_HASH_ENTRY *thisEntry, *lastEntry, *tempEntry;


    if (timeToLive <= 0)
	return;

    savedEntries = 0;
    freedEntries = 0;
    maxLength = 0;
    for (i = 0;  i < 4;  i++)
	lengthDist[i] = 0;

    time(&now);

    /* Search each chain in the hash table for expired entries */
    for (i = 0;  i < hash->tableSize;  i++) {

	lastEntry = NULL;	       /* lastEntry NULL indicates list head */
	thisEntry = (hash->table)[i];
	chainLength = 0;

	/* Search a single chain */
	while (thisEntry != NULL)
	    if (now - thisEntry->timeStamp > timeToLive) {

		/* Expired.  Head of list is a special case. */
		if (lastEntry == NULL)
		    (hash->table)[i] = thisEntry->nextEntry;
		else
		    lastEntry->nextEntry = thisEntry->nextEntry;

		tempEntry = thisEntry;		/* Point to expired entry    */
		thisEntry = thisEntry->nextEntry;  /* Advance scan pointer  */
		free(tempEntry);		/* Reclaim the space it used */
		freedEntries++;			/* Count one more reclaimed  */

	    } else {

		lastEntry = thisEntry;		/* Not expired, skip it      */
		thisEntry = thisEntry->nextEntry;  /* Advance scan pointer  */
		savedEntries++;			/* Count one more saved      */
		chainLength++;			/* and one more on chain     */

	    }

	/* End of while (end of chain) */

	if (chainLength > maxLength)
	    maxLength = chainLength;

	if (chainLength < 4)
	    lengthDist[chainLength]++;
	else
	    lengthDist[3]++;

    } /* End of for (end of hash table) */

    if (debugFlag) {
	SystemLog("user cache purge: Freed %d  Saved %d  Max %d  Avg %.2f",
		freedEntries, savedEntries, maxLength,
		(1.0 * savedEntries) / hash->tableSize);
	SystemLog("user cache purge: Dist 0: %d  1: %d  2: %d  3+: %d",
		lengthDist[0], lengthDist[1], lengthDist[2], lengthDist[3]);
    }

}


/************************************************************************
 * Function:
 *     PurgeHostHashTable
 *
 * Description:
 *     Purge old entries from the hostname hash table
 *
 * Arguments:
 *     timeToLive -	number of seconds entries are allowed to live.
 *     hash -		pointer to hostname hash table to be purged.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void PurgeHostHashTable(int timeToLive, HOST_HASH_TABLE *hash)
{
    auto int i;
    auto time_t now;
    auto int savedEntries, freedEntries;
    auto int maxLength, chainLength, lengthDist[4];
    auto HOST_HASH_ENTRY *thisEntry, *lastEntry, *tempEntry;


    if (timeToLive <= 0)
	return;

    savedEntries = 0;
    freedEntries = 0;
    maxLength = 0;
    for (i = 0;  i < 4;  i++)
	lengthDist[i] = 0;

    time(&now);

    /* Search each chain in the hash table for expired entries */
    for (i = 0;  i < hash->tableSize;  i++) {

	lastEntry = NULL;	       /* lastEntry NULL indicates list head */
	thisEntry = (hash->table)[i];
	chainLength = 0;

	/* Search a single chain */
	while (thisEntry != NULL)
	    if (now - thisEntry->timeStamp > timeToLive) {

		/* Expired.  Head of list is a special case. */
		if (lastEntry == NULL)
		    (hash->table)[i] = thisEntry->nextEntry;
		else
		    lastEntry->nextEntry = thisEntry->nextEntry;

		tempEntry = thisEntry;		/* Point to expired entry    */
		thisEntry = thisEntry->nextEntry;  /* Advance scan pointer  */
		free(tempEntry);		/* Reclaim the space it used */
		freedEntries++;			/* Count one more reclaimed  */

	    } else {

		lastEntry = thisEntry;		/* Not expired, skip it      */
		thisEntry = thisEntry->nextEntry;  /* Advance scan pointer  */
		savedEntries++;			/* Count one more saved      */
		chainLength++;			/* and one more on chain     */

	    }

	/* End of while (end of chain) */

	if (chainLength > maxLength)
	    maxLength = chainLength;

	if (chainLength < 4)
	    lengthDist[chainLength]++;
	else
	    lengthDist[3]++;

    } /* End of for (end of hash table) */

    if (debugFlag) {
	SystemLog("host cache purge: Freed %d  Saved %d  Max %d  Avg %.2f",
		freedEntries, savedEntries, maxLength,
		(1.0 * savedEntries) / hash->tableSize);
	SystemLog("host cache purge: Dist 0: %d  1: %d  2: %d  3+: %d",
		lengthDist[0], lengthDist[1], lengthDist[2], lengthDist[3]);
    }

}
