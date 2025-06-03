/* VAX/VMS getmail---Read new messages from user's mailbox.
   Copyright (C) 1993 Roland B. Roberts.

getmail is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

getmail is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with getmail; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * The following is not yet implemented (should it be?):
 *   o Read a someone else's mail
 *
 */

char *version_string = "GNU VMS getmail version 0.99.1\n";

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ssdef.h>
#include <stsdef.h>
#include "mailmsgdef.h"
#include "maildef.h"
#include "mail.h"
#include "getopt.h"

#define BUFLEN 256
#ifndef TRUE
# define TRUE  (1)
# define FALSE (0)
#endif

#ifdef VAXC
# define inline
#endif

/*
 * These are all used to initialize the ITMLST structs for MAIL$ calls
 */
static char CC[BUFLEN], Date[BUFLEN], From[BUFLEN];
static char Reply[BUFLEN], Sender[BUFLEN], Subject[BUFLEN], To[BUFLEN];
static char Buffer[BUFLEN];
static long lCC, lDate, lFrom, lReply, lSender, lSubject, lTo, lBuffer;

static short record_type;
static short newcount;
static long mailfilectx;
static long messagectx;
static long mailuserctx;
static long message_id;
static long selected_count;

static ITMLST messagectx_itmlst[] = {
  { 4, MAIL$_MESSAGE_FILE_CTX, &mailfilectx, 0 },
  { 0, 0, 0, 0 } };

static ITMLST selected_message_count_itmlst[] = {
  { sizeof(selected_count), MAIL$_MESSAGE_SELECTED, &selected_count, 0 },
  { 0, 0, 0, 0 } };

static ITMLST next_message_itmlst[] = {
  { 0, MAIL$_MESSAGE_NEXT, 0, 0 },
  { 0, 0, 0, 0 } };

static ITMLST header_info_itmlst[] = {
  { sizeof(message_id), MAIL$_MESSAGE_CURRENT_ID, &message_id, 0 },
  { BUFLEN-1, MAIL$_MESSAGE_CC, &CC, &lCC },
  { BUFLEN-1, MAIL$_MESSAGE_DATE, &Date, &lDate },
  { BUFLEN-1, MAIL$_MESSAGE_FROM, &From, &lFrom },
  { BUFLEN-1, MAIL$_MESSAGE_REPLY_PATH, &Reply, &lReply },
  { BUFLEN-1, MAIL$_MESSAGE_SENDER, &Sender, &lSender },
  { BUFLEN-1, MAIL$_MESSAGE_SUBJECT, &Subject, &lSubject },
  { BUFLEN-1, MAIL$_MESSAGE_TO, &To, &lTo },
  { 0, 0, 0, 0 } };

static ITMLST delete_message_itmlst[] = {
  { sizeof(message_id), MAIL$_MESSAGE_ID, &message_id, 0 },
  { 0, 0, 0, 0 } };

static ITMLST fullclose_itmlst[] = {
  { 0, MAIL$_MAILFILE_FULL_CLOSE, 0, 0 },
  { 0, 0, 0, 0 } };

static ITMLST continue_message_itmlst[] = {
  { 0, MAIL$_MESSAGE_CONTINUE, 0, 0 },
  { 0, 0, 0, 0 } };

static ITMLST message_buffer_itmlst[] = {
  { BUFLEN-1, MAIL$_MESSAGE_RECORD, &Buffer, &lBuffer },
  { 2, MAIL$_MESSAGE_RECORD_TYPE, &record_type, 0 },
  { 0, 0, 0, 0 } };

static ITMLST get_newcount_itmlst[] = {
  { sizeof(newcount), MAIL$_USER_NEW_MESSAGES, &newcount, 0 },
  { 0, 0, 0, 0 } };

static ITMLST set_newcount_itmlst[] = {
  { sizeof(newcount), MAIL$_USER_SET_NEW_MESSAGES, &newcount, 0 },
  { 0, 0, 0, 0 } };

static ITMLST null_itmlst[] = {
  { 0, 0, 0, 0 } };

/*
 * The program name, as called in argv[0], will go here.
 */
char *program;

getmail_help ()
{
  printf ("Usage: %s [-adnvV] [-f file] [-F folder] [-u user]\n\n\
  Options:\n\
    --append  -a         append to FILE (specified by --file)\n\
    --count   -c         return the mail count only\n\
    --delete  -d         delete messages after reading\n\
    --file    -f FILE    output to FILE\n\
    --folder  -F FOLDER  select mail from FOLDER\n\
    --help    -h         display brief help message and exit\n\
    --newmail -n         select new messages\n\
    --user    -u USER    read mail for USER (not implemented!)\n\
    --version -v         return version information\n\
    --verbose -V         talk to me\n\n\
Folder names should be enclosed in double-quotes to preserve case.\n",
	  program);
}

getmail_usage ()
{
  fprintf (stderr,
	   "Usage: %s [-acdnvV] [-f file] [-F folder] [-u user]\n", program);
  fprintf (stderr,
	   "For help, type: %s -h\n", program);
  exit (SS$_ABORT);
}

/*
 * Minimal error handler---just die.
 */
static inline outerr (void)
{
  perror (program);
  exit (SS$_ABORT);
}

/*
 * This function is called when one of the VMS mail utilities fail and
 * raise an exception. It gets called as a result of specifying it with
 * the VAXC$ESTABLISH routine.
 *
 * If a routine was not established, then a traceback would be printed
 * when the mail utilities failed.
 *
 * Near as I can tell, TFM lies about the calling sequence.
 * The RTL manual claims the calling sequence is:
 *     ExcHandler (void *mecharr, void *sigarr)
 * The difference is significant: mecharr[1] is the frame pointer;
 * sigarr[1] is the condition code being signalled.
 * This order (used below) is identical to that for LIB$ESTABLISH, and,
 * more importantly, it works correctly.
 *
 */
unsigned long expected_error;

int
Exception_Handler (void *sigarr, void *mecharr)
{
  ((unsigned *)sigarr)[1] |= STS$M_INHIB_MSG;
  return SS$_RESIGNAL;
}

main (int argc, char **argv)
{
  static struct option getmail_options[] =
    {
      {"append",   no_argument,       0, 'a' },
      {"count",    no_argument,       0, 'c' },
      {"delete",   no_argument,       0, 'd' },
      {"file",     required_argument, 0, 'f' },
      {"folder",   required_argument, 0, 'F' },
      {"help",     no_argument,       0, 'h' },
      {"newmail",  no_argument,       0, 'n' },
      {"user",     required_argument, 0, 'u' },
      {"version",  no_argument,       0, 'v' },
      {"verbose",  no_argument,       0, 'V' },
      { 0, 0, 0, 0 }
   };
  /* Force most options to be explictly requested */
  int append = FALSE;
  int count = FALSE;
  int delete = FALSE;
  int help = FALSE;
  int newmail = FALSE;
  int verbose = FALSE;
  int version = FALSE;
  int badargs = FALSE;
  char *file = NULL;
  FILE *out = stdout;

  int i, c, errno, option_index;

  struct {
    char name[BUFLEN];
    short *namelen;
  } folder, user;

  ITMLST username_itmlst[] = {
    { 0, MAIL$_USER_USERNAME, user.name, 0 },
    { 0, 0, 0, 0 } };

  ITMLST select_folder_and_messages_itmlst[] = {
    { 0, MAIL$_MESSAGE_FOLDER, folder.name , 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 } };

  struct flagsdef mailflags;

/*
 * Initialize here.
 */
  strcpy (folder.name, "NEWMAIL");
  folder.namelen = &select_folder_and_messages_itmlst[0].buflen;
  *folder.namelen = strlen(folder.name);

  strcpy (user.name, getenv("USER"));
  user.namelen = &username_itmlst[0].buflen;
  *user.namelen = strlen(user.name);

  mailflags.mail$r_fill_0.mail$w_flags = 0;
  mailflags.mail$r_fill_0.mail$r_fill_1.mail$v_newmsg = 1;

/*
 * Hack argv[0] to be "pretty", use `program' to point to the result.
 */
  *argv = 1 + strrchr(*argv,']');
  *strchr(*argv,'.') = 0;
  program = *argv;

/*
 * Handle the command line args
 */
  while (1)
    {
      c = getopt_long (argc, argv, "acdf:nu:vF:V",
		       getmail_options, &option_index);
      if (c == EOF)
	break;
      switch (c)
	{
	case 'a':
	  append = TRUE;
	  break;
	case 'c':
	  count = TRUE;
	  break;
	case 'd':
	  delete = TRUE;
	  break;
	case 'f':
	  file = optarg;
	  break;
	case 'h':
	  help = TRUE;
	  break;
	case 'n':
	  newmail = TRUE;
	  break;
	case 'u':
	  fprintf (stderr, "%s: --user not implemented\n", program);
	  strcpy (user.name, optarg);
	  *user.namelen = strlen(user.name);
	  badargs = TRUE;
	  break;
	case 'v':
	  printf ("%s", version_string);
	  break;
	case 'F':
	  strcpy (folder.name, optarg);
	  *folder.namelen = strlen(folder.name);
	  break;
	case 'V':
	  verbose = TRUE;
	  break;
	case 0:
	case '?':
	default:
	  badargs = TRUE;
	}
    }

/*
 * If "NEWMAIL" was selected, we assume "--newmail"
 */
  if (strcmp(folder.name, "NEWMAIL") == 0)
    newmail = TRUE;

/*
 * Only the folder "NEWMAIL" is allowed for --newmail --folder FOLDER
 * Note: the folder specification is *required* in MAIL$MESSAGE_SELECT
 */
  if (badargs || (newmail && !(strcmp(folder.name,"NEWMAIL") == 0)))
    {
      fprintf (stderr, "%s: conflicting options --newmail and --folder %s\n",
	       program, folder.name);
      if (help)
	getmail_help ();
      else
	getmail_usage ();
      exit (SS$_ABORT);
    }

/*
 * Display help information then exit
 */
  if (help)
    {
      getmail_help ();
      exit (SS$_NORMAL);
    }
  
  VAXC$ESTABLISH(Exception_Handler);

  if (count)
    {
      mailuserctx = 0;
      MAIL$USER_BEGIN (&mailuserctx, null_itmlst, null_itmlst);
      MAIL$USER_GET_INFO (&mailuserctx, username_itmlst, get_newcount_itmlst);
      MAIL$USER_END (&mailuserctx, null_itmlst, null_itmlst);
      printf ("You have %d new mail message%s\n", newcount,
	      (newcount > 1 ? "s" : ""));
      exit (SS$_NORMAL);
    }

  mailfilectx = 0;
  MAIL$MAILFILE_BEGIN (&mailfilectx, null_itmlst, null_itmlst);
  MAIL$MAILFILE_OPEN (&mailfilectx, null_itmlst , null_itmlst);
  MAIL$MESSAGE_BEGIN (&messagectx, messagectx_itmlst, null_itmlst);

  if (verbose)
    printf ("Selecting folder %s\n", folder.name);

  if (newmail)
    {
      select_folder_and_messages_itmlst[1].item = MAIL$_MESSAGE_FLAGS;
      select_folder_and_messages_itmlst[1].buflen = sizeof(mailflags);
      select_folder_and_messages_itmlst[1].bufadr = &mailflags;
      if (verbose)
	printf ("Selecting new messages\n");
    }

/*
 * This will complain if the selected folder does not exist.
 * I suppose that's as good a way as any to let the user know....
 */
  MAIL$MESSAGE_SELECT (&messagectx, select_folder_and_messages_itmlst,
		       selected_message_count_itmlst);

  if (verbose)
    printf ("%d messages selected\n", selected_count);

/*
 * If there are no messages selected, or no file was specified, skip this.
 * Note: RMS may fail to open the file.  The most likely error is
 * "RMS-E-CRE, ACP file create failed", probably caused by lack of quota.
 */
  if (selected_count != 0 && file != NULL)
    {
      errno = 0;
      if (append)
	out = fopen (file, "a");
      else
	out = fopen (file, "w");
      if (out == NULL)
	outerr ();
      if (verbose)
	printf ("File %s opened for %s", file,
		(append) ? "append" : "write");
    }

/*
 * Unfortunately, MAIL$MESSAGE_GET signals an error, so we have to count
 * messages instead of just doing the following:
 * while (MAIL$_NOMOREMSG != MAIL$MESSAGE_GET (&messagectx,
 *                              next_message_itmlst, header_info_itmlst))
 */
  for (i = 0; i < selected_count; i++)
    {
      MAIL$MESSAGE_GET (&messagectx, next_message_itmlst, header_info_itmlst);
      if (verbose)
	printf ("\nProcessing message %d...", i+1);
/*
 * Null terminate the returned values.
 */
      CC[lCC] = 0;
      Date[lDate] = 0;
      From[lFrom] = 0;
      Reply[lReply] = 0;
      Sender[lSender] = 0;
      Subject[lSubject] = 0;
      To[lTo] = 0;

/*
 * Print out the message headers just like VMS Mail does.
 *
 * If fprintf fails, it is most like a quota problem, so just abort.
 * This means the output file may be truncated in mid-message.
 * Also, I *think* output can fail once but you can still write more
 * until you exceed your overdraft.  I'm not sure how to try that....
 *
 * We might be able to get away with skipping the sprintf, and just
 * making sure fprintf() > 0.
 */
      sprintf (Buffer, "\f\nFrom:\t%s %s\n", From, Date);
      errno = 0;
      if (fprintf (out, "%s", Buffer) != strlen(Buffer))
	outerr ();

      sprintf (Buffer, "To:\t%s\n", To);
      errno = 0;
      if (fprintf (out, "%s", Buffer) != strlen(Buffer))
	outerr ();

      sprintf (Buffer, "CC:\t%s\n", CC);
      errno = 0;
      if (fprintf (out, "%s", Buffer) != strlen(Buffer))
	outerr ();

      sprintf (Buffer, "Subj:\t%s\n\n", Subject);
      errno = 0;
      if (fprintf (out, "%s", Buffer) != strlen(Buffer))
	outerr ();

      while (MAIL$MESSAGE_GET (&messagectx, continue_message_itmlst,
			       message_buffer_itmlst)
	     != MAIL$_NOMOREREC)
	{
	  if (record_type != MAIL$_MESSAGE_HEADER)
	    {

/*
 * Same as with headers except for the message body now.
 */
	      Buffer[lBuffer++] = '\n';
	      Buffer[lBuffer]   = 0;
	      errno = 0;
	      if (fprintf (out, "%s", Buffer) != lBuffer)
		outerr ();
	    }
	}
/*
 * Delete the message---it has already been written to disk.
 *
 * A (possibly) nicer technique would be to keep a list of message ids
 * and do the deletion as a last thing.  This would also allow outerr()
 * to clean up by deleting any output file and leaving things as they
 * were.
 */
      if (verbose)
	printf (".  Done.");
      if (delete)
	{
	  MAIL$MESSAGE_DELETE (&messagectx, delete_message_itmlst, null_itmlst);
	  if (verbose)
	    printf ("  And deleted.");
	}
    }

/*
 * Close everything and exit.
 * Pretty stupid that we have to set the new mail count "by hand".
 * This approach is (I think) conservative---it will work even if new mail
 * arrived while we were processing messages.
 */
  MAIL$MESSAGE_END (&messagectx, null_itmlst, null_itmlst);
  MAIL$MAILFILE_CLOSE (&mailfilectx, fullclose_itmlst, null_itmlst);
  MAIL$MAILFILE_END (&mailfilectx, fullclose_itmlst, null_itmlst);

  if (delete && newmail)
    {
      if (verbose)
	printf ("\nDecrementing new count by %d ", selected_count);
      mailuserctx = 0;
      MAIL$USER_BEGIN (&mailuserctx, null_itmlst, null_itmlst);
      MAIL$USER_GET_INFO (&mailuserctx, username_itmlst, get_newcount_itmlst);
      newcount -= selected_count;
      MAIL$USER_SET_INFO (&mailuserctx, set_newcount_itmlst, null_itmlst);
      MAIL$USER_END (&mailuserctx, null_itmlst, null_itmlst);
      if (verbose)
	printf ("to %d", newcount);
    }
  if (verbose)
    printf ("\nAll done!", newcount);

  exit (SS$_NORMAL);
}
