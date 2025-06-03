/*

Copyright (C) 1991 by John Hughes.

Permission to use, copy, modify, and distribute this software for any purpose
and without fee is hereby granted, provided that the above copyright notice
appear in all copies and that both that copyright notice and this permission
notice appear in supporting documentation.  This software is provided "as is"
without express or implied warranty.

        A little program to make our streamer stream.

        We run two processes, one reading stdin and filling up a nice
        big shared memory segment and another writing the stuff to the
        tape.

        Semaphores are used to synchronise the system in the usual way.
*/

#include <stdio.h>
#include <assert.h>
#include "local.h"
#include <stdarg.h> 
#include <errno.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/signal.h>

/* Added by Greg Lehey, LEMIS, 20 January 1994 */
#ifdef __STDC__
#ifdef SVR4						    /* and SVR4 */
#if (__STDC__ != 0)					    /* the standard header files won't work */
typedef struct
  {
  unsigned long	sa_sigbits[4];
  } sigset_t;

struct sigaction
{
  int sa_flags;
  void (*sa_handler) ();
  sigset_t sa_mask;
  int sa_resv[2];
  };

#define SA_ONSTACK	0x00000001
#define SA_RESETHAND	0x00000002
#define SA_RESTART	0x00000004
#define SA_SIGINFO	0x00000008
#define SA_NODEFER	0x00000010
#define SA_NOCLDWAIT	0x00010000
#define SA_NOCLDSTOP	0x00020000
#endif
#endif
#endif


/* Global variables, copies needed by both the producer and consumer */

PRIVATE int sem_id;
PRIVATE int shm_id;
PRIVATE char *shm_buf;
PRIVATE char *shm_end;

PRIVATE int block_size = 10240;
PRIVATE int num_blocks = 10;
PRIVATE BOOL verbose = FALSE;

typedef struct BLOCK 
{
  int len;
  char data [1];
  }
BLOCK;

#define BLOCK_SIZE (block_size + sizeof (BLOCK))

PRIVATE char name [80];


/* Flag to let us know our child has died */

PRIVATE BOOL child_dead = FALSE;
PRIVATE int child_status;

/* When we are run in a pipeline the process to our left is actualy
   our child, so we can get SIGCLD messages from them.  We need some
   way of ignoring this, and this is it:
   */

PRIVATE int child_pid;
PRIVATE BOOL ignore_signal = FALSE;


/* Semaphores we use */

#define BLOCKS_FREE     0
#define BLOCKS_AVAIL    1

#define NUM_SEMS 2


/* Forward declarations */

PRIVATE void init (int argc, char **argv);
PRIVATE void producer (void);
PRIVATE void consumer (void);
PRIVATE void cleanup (void);
PRIVATE void gone_gone (int sig);
PRIVATE void abort (int sig);
PRIVATE void error (char *err, ...);

PRIVATE void up (int sem, int level);
PRIVATE void down (int sem, int level);
PRIVATE int level (int sem);


/* Ok, off we go */

main (int argc, char **argv) 
{
  struct sigaction abort_action;

  init (argc, argv);
  
  if ((child_pid = fork ()) < 0) 
    {
    error ("fork");
    exit (1);
    }
  else if (child_pid) 
    {
    
    int stat;
    
    /* We are the parent.  start the producer */
    
    strcat (name, ": read");
    
    if (verbose) 
      {
      fprintf (stderr, "%s: Child pid %d\n", name, child_pid);
      }
    
    close (1);
    signal (SIGCLD, &gone_gone);
    sigemptyset (&abort_action.sa_mask);
    abort_action.sa_handler = abort;
    abort_action.sa_flags = SA_RESTART;
    sigaction (SIGHUP, &abort_action, NULL);
    sigaction (SIGINT, &abort_action, NULL);
    sigaction (SIGQUIT, &abort_action, NULL);
    sigaction (SIGTERM, &abort_action, NULL);
    
    
    /* Should catch some other signals here so we can
       clean up ... *FIX* */
    
    producer ();
    
    /* Now wait for the child to finish */
    
    while (!child_dead) 
      {
      if (wait (NULL) < 0 && errno != EINTR) 
	{
	perror ("WAIT");
	exit (99);
	}
      }
    
    cleanup ();
    exit (child_status);
    
    }
  else 
    {
    /* We are the child, start the consumer */
    
    strcat (name, ": write");
    
    close (0);
    
    consumer ();
    
    exit (0);
    }
  }


/* On startup initialisation */

PRIVATE void init (int argc, char **argv) 
{
  
  extern char *optarg;
  extern int optind;
  int c;
  BOOL err = FALSE;
  
  setvbuf (stderr, NULL, _IOLBF, 0);
  
  strcpy (name, argv [0]);
  
  while ((c = getopt (argc, argv, "vb:n:")) != -1) 
    {
    switch (c) 
      {
    case 'v':
      verbose = TRUE;
      break;
    case 'b':
      if ((block_size = atoi (optarg)) <= 0) 
	{
	fprintf (stderr, "%s: Bad block size %s\n",
		 name, optarg);
	err = TRUE;
	}
      break;
    case 'n':
      if ((num_blocks = atoi (optarg)) <= 0) 
	{
	fprintf (stderr, "%s: Bad # blocks %s\n",
		 name, optarg);
	err = TRUE;
	}
      break;
    default:
      err = TRUE;
      }
    }
  
  if (err) 
    {
    fprintf (stderr, "\n%s: Usage:  %s [ -v ] [ -b blocksize ]"
	     " [ -n blocks ]\n"
	     "   -v                  Verbose mode\n"
	     "   -b blocksize        set # bytes in block written to tape (default 10240)\n"
	     "   -n blocks           set # blocks held in shared memory (default 10)\n\n",
	     name,
	     name);
    exit (1);
    }
  
  if (verbose) 
    {
    fprintf (stderr, "%s: blocksize = %d; blocks = %d\n",
	     name, block_size, num_blocks);
    }
  
  sem_id = semget (IPC_PRIVATE, NUM_SEMS, 00600);
  
  if (sem_id < 0) 
    {
    error ("semget");
    exit (1);
    }
  
  shm_id = shmget (IPC_PRIVATE, BLOCK_SIZE * num_blocks, 00600);
  
  if (shm_id < 0) 
    {
    error ("shmget");
    exit (1);
    }
  
  shm_buf = (char *) shmat (shm_id, NULL, 0);
  
  if ((int) shm_buf == -1) 
    {
    error ("shmat");
    exit (1);
    }
  
  shm_end = shm_buf + BLOCK_SIZE * num_blocks;
  
  /* Set the initial free space */
  
  up (BLOCKS_FREE, num_blocks);
  }


/* Producer, read input and stuff it in memory */

PRIVATE void producer (void) 
{
  
  char *index = shm_buf;
  
  int blocks = 0;
  int waits = 0;
  int bytes_read = 0;
  
  struct tms tms;
  long start = times (&tms);
  
  for (;;) 
    {
    
    BLOCK *block = (BLOCK *) index;
    
    if (child_dead) break;
    
    if (verbose && level (BLOCKS_FREE) == 0) waits ++; /* perf */
    ++blocks;                                          /* perf */
    
    down (BLOCKS_FREE, 1);
    
    block->len = 0;
    
    while (block->len < block_size) 
      {
      int bytes = block_size - block->len;
      bytes = read (0, &block->data [block->len], bytes);
      if (bytes < 0) 
	{
	if (errno == EINTR && ignore_signal) 
	  {
	  ignore_signal = FALSE;
	  bytes = 0;
	  }
	else 
	  {
	  error ("read (stdin)");
	  exit (1);
	  }
	}
      else if (bytes == 0) 
	{
	break;
	}
      block->len += bytes;
      }
    
    bytes_read += block->len;                       /* perf */
    
    up (BLOCKS_AVAIL, 1);
    
    if (block->len == 0) break;
    
    if ((index += BLOCK_SIZE) >= shm_end) 
      {
      index = shm_buf;
      }
    }
  if (verbose) 
    {
    fprintf (stderr, "%s: blocks %d; "
	     "waits %d (%.2f%%); %.2f bytes/sec\n",
	     name, blocks,
	     waits, (100.0 * waits / MAX (blocks, 1)),
	     ((double) HZ * bytes_read) / (1 + times(&tms) - start));
      }
  }


/* Consumer, get stuff out of memory and write to the tape */

PRIVATE void consumer (void) 
{
  
  char *exdex = shm_buf;
  
  int blocks = 0;
  int waits = 0;
  int bytes_written = 0;
  
  struct tms tms;
  long start = times (&tms);
  
  for (;;) 
    {
    
    BLOCK *block = (BLOCK *) exdex;
    int bytes;
    
    if (verbose && level (BLOCKS_AVAIL) == 0) waits++; /* perf */
    blocks ++;                                         /* perf */
    
    down (BLOCKS_AVAIL, 1);
    
    if (block->len == 0) break;
    
    bytes = write (1, block->data, block->len);
    if (bytes < block->len) 
      {
      error ("write (stdout)");
      exit (1);
      }
    
    bytes_written += bytes;
    
    up (BLOCKS_FREE, 1);
    
    if ((exdex += BLOCK_SIZE) >= shm_end) 
      {
      exdex = shm_buf;
      }
    }
  
  if (verbose) 
    {
    fprintf (stderr, "%s: blocks %d; "
	     "waits %d (%.2f%%); %.2f bytes/sec\n",
	     name, blocks,
	     waits, (100.0 * waits / MAX (blocks, 1)),
	     ((double) HZ * bytes_written) / (1 + times (&tms) - start) );
    }
  }


/* Cleanup - get rid of semaphores, shared memory */

PRIVATE void cleanup (void) 
{
  
  if (shmdt (shm_buf) < 0) 
    {
    error ("shmdt");
    exit (1);
    }
  
  if (shmctl (shm_id, 0, IPC_RMID, NULL) < 0) 
    {
    error ("shmctl (IPC_RMID)");
    exit (1);
    }
  
  if (semctl (sem_id, 0, IPC_RMID, NULL) < 0) 
    {
    error ("semctl (IPC_RMID)");
    exit (1);
    }
  }


/* Gone, Gone and never called me mother! */

PRIVATE void gone_gone (int sig) 
{
  
  int stat;
  
  if (wait (&stat) != child_pid) 
    {
    if (verbose) 
      {
      fprintf (stderr, "%s: left hand '|' died\n", name);
      }
    ignore_signal = TRUE;
    signal (SIGCLD, &gone_gone);
    }
  else 
    {
    if ((stat & 0xff) == 0) 
      {
      child_status == stat >> 8;
      if (verbose) 
	{
	fprintf (stderr, "%s: Child exit (%d)\n",
		 name, child_status);
	}
      }
    else if (stat >> 8 == 0) 
      {
      child_status = 1;
      fprintf (stderr, "%s: Child terminated by signal (%d)%s\n",
	       name,
	       stat & 0x7f,
	       stat & 0x80 ? " with core dump" : "");
      }
    else 
      {
      child_status = 1;
      fprintf (stderr, "%s: Child stopped by signal (%d)\n",
	       name, stat >> 8);
      }
    ignore_signal = FALSE;
    child_dead = TRUE;
    }
  }


/* Tidy up on interrupt */

PRIVATE void abort (int sig) 
{
  
  signal (SIGCLD, SIG_IGN);
  
  if (verbose && sig != SIGHUP) 
    {
    fprintf (stderr, "%s: Aborted by signal %d\n", name, sig);
    }
  cleanup ();
  exit (1);
  }


/* Produce a prettier error message */

PRIVATE void error (char *err, ...) 
{
  
  va_list vp;
  char buf [256];
  char message [256];
  
  va_start (vp, err);
  
  vsprintf (buf, err, vp);
  
  va_end (vp);
  
  strcpy (message, name);
  strcat (message, ": ");
  strcat (message, buf);
  
  perror (message);
  }


/* Increment a semaphore */

PRIVATE void up (int sem, int level) 
{
  
  struct sembuf sop;
  
  sop.sem_num = sem;
  sop.sem_op = level;
  sop.sem_flg = 0;
  
  if (semop (sem_id, &sop, 1) < 0) 
    {
    error ("semop (up %d, %d)", sem, level);
    exit (1);
    }
  }


/* Decrement a semaphore */

PRIVATE void down (int sem, int level) 
{
  
  struct sembuf sop;
  
  sop.sem_num = sem;
  sop.sem_op = -level;
  sop.sem_flg = 0;
  
  while (semop (sem_id, &sop, 1) < 0) 
    {
    if (errno == EINTR && ignore_signal) 
      {
      ignore_signal = FALSE;
      }
    else 
      {
      error ("semop (down %d, %d)", sem, level);
      exit (1);
      }
    }
  }


/* Get current level of a semaphore */

PRIVATE int level (int sem) 
{
  
  int val;
  
  if ((val = semctl (sem_id, sem, GETVAL, NULL)) < 0) 
    {
    error ("semctl (level)");
    exit (1);
    }
  
  return val;
  }
