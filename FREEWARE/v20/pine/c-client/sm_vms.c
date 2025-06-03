/*
 * Program:	Subscription Manager, VMS version which is a dummy one.
 */
#define	NIL	0


long sm_subscribe (mailbox)
char *mailbox;
{
  return NIL;
}

/* Unsubscribe from mailbox
 * Accepts: mailbox name
 * Returns: T on success, NIL on failure
 */

long sm_unsubscribe (mailbox)
char *mailbox;
{
  return NIL;
}

/* Read subscription database
 * Accepts: pointer to subscription database handle (handle NIL if first time)
 * Returns: character string for subscription database or NIL if done
 * Note - uses strtok() mechanism
 */

char *sm_read (sdb)
void **sdb;
{
  return NIL;
}
