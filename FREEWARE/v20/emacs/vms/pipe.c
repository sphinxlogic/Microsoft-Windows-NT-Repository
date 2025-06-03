/*
 * Copyright © 1994 the Free Software Foundation, Inc.
 *
 * Author: Roland B. Roberts (roberts@nsrl.rochester.edu)
 *
 * This file is a part of GNU VMSLIB, the GNU library for porting GNU
 * software to VMS.
 *
 * GNU VMSLIB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNU VMSLIB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU VMSLIB; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iodef.h>
#include <ssdef.h>
#include <syidef.h>
#include <clidef.h>
#include <stsdef.h>
#include <dvidef.h>
#include <nam.h>
#include <descrip.h>
#include <errno.h>
#include <file.h>
#include <lib$routines.h>
#include <starlet.h>
#include "pipe.h"

typedef struct io_status_block
{ unsigned short status, count;
  unsigned long  devdep; } IOSB;

typedef struct exit_handler_control_block
{ struct exhcb *exh$a_link;
  int (*exh$a_routine)();
  long exh$l_argcount;
  long *exh$a_status;
  long exh$l_status; } EXHCB;

struct itemlist3
{ short  itm$w_length;
  short  itm$w_itemcode;
  void  *itm$a_pointer;
  short *itm$a_retlength; };

/*
 * Maximum number of pipes available for popen()
 * This could be extended by using the same event flag but different
 *   I/O status blocks.  However, you may run into an open files
 *   limit before 32 anyway
 */
#define MAXCHILD 32		/* maximum number of pipes for popen */
#define LSTEVFN  31		/* last available event flag number */
#define FSTEVFN   1		/* first available event flag number */
static struct
{ int mode;			/* pipe I/O mode; 1=read, 2=write */
  IOSB iosb;			/* pipe I/O status block */          
  FILE *file;			/* pipe file structure */            
  int pid;			/* pipe process id */                
  short chan;			/* pipe channel */                   
} pipe[MAXCHILD];

/*
 * Exit handler for current process, established by popen().
 * Force the current process to wait for the completion of children
 *   which were started via popen().  
 */
static int pwait (int *status) 
{
  IOSB iosb;
  int evf;

  for (evf = FSTEVFN; evf <= LSTEVFN; evf++)
    if (pipe[evf].iosb.status == 0 && pipe[evf].chan != 0) {
      fflush (pipe[evf].file);
      if (pipe[evf].mode == O_WRONLY)
	sys$qio (0, pipe[evf].chan, IO$_WRITEOF, &iosb, 0, 0, 0, 0, 0, 0, 0, 0);
      fclose (pipe[evf].file);	/* necessary? */
      sys$synch (evf, &pipe[evf].iosb);
      sys$dassgn (pipe[evf].chan);
    }
  return 0;
}

/*
 * Close a "pipe" created by popen()l
 * Return codes
 *  0 success
 * -1 stream not found in list of pipes
 */
int pclose (FILE *stream)
{
  IOSB iosb;
  int evf;

  for (evf = FSTEVFN; evf < LSTEVFN; evf++)
    if (pipe[evf].file == stream) {
      fflush (pipe[evf].file);
      if (pipe[evf].mode == O_WRONLY)
	sys$qio (0, pipe[evf].chan, IO$_WRITEOF, &iosb, 0, 0, 0, 0, 0, 0, 0, 0);
      fclose (pipe[evf].file);	/* necessary? */
      sys$synch (evf, &pipe[evf].iosb);
      sys$dassgn (pipe[evf].chan);
      /* Clear these values so popen() and pwait() know they are unused */
      pipe[evf].chan = 0;
      pipe[evf].pid  = 0;
      pipe[evf].mode = 0;
      pipe[evf].file = (void *) 0;
      return 0;			/* always succeed */
    }
  return -1;			/* not a pipe */
}

/*
 * Subprocess AST completion routine
 * Scan through list of children to find matching channel.
 * When found, indicate successful completion in the iosb
 *   and clear the pid.
 * Note that the channel is *not* deassigned and the file is
 *   *not* closed.  
 */
static pdone (short *chan)
{
  int evf;

  for (evf = FSTEVFN; evf < LSTEVFN; evf++) {
    if (pipe[evf].chan == *chan) {
      pipe[evf].iosb.status = 1;
      pipe[evf].pid  = 0;
      break;
    }
  }
}

/* Exit handler control block for the current process. */
static EXHCB pexhcb = { 0, pwait, 1, &pexhcb.exh$l_status, 0 };

struct vstring { short length; char string[NAM$C_MAXRSS+1]; };

/*
 * Emulate a unix popen() call using lib$spawn
 * 
 * if mode == "w", lib$spawn uses the mailbox for sys$input
 * if mode == "r", lib$spawn uses the mailbox for sys$output
 */
FILE *popen (const char *cmd, const char *mode) 
{
  int evf, status, flags, mbxsize;
  IOSB iosb;
  char *cmdstr;
  struct dsc$descriptor_s cmddsc, mbxdsc;
  struct vstring mbxname = { sizeof(mbxname.string) };
  struct itemlist3 mbxlist[2] = {
    { sizeof(mbxname.string)-1, DVI$_DEVNAM, &mbxname.string, &mbxname.length },
    { 0, 0, 0, 0} };
  struct itemlist3 syilist[2] = {
    { sizeof(mbxsize), SYI$_MAXBUF, &mbxsize, (void *) 0 },
    { 0, 0, 0, 0} };
  static int FIRST = 0;

  /* First see if we have any more event flags available */
  for (evf = FSTEVFN; evf <= LSTEVFN; evf++)
    if (pipe[evf].chan == 0)
      break;
  if (pipe[evf].chan != 0) {
    fprintf (stderr, "popen, too many pipes %d\n", MAXCHILD);
    exit (SS$_ABORT | STS$M_INHIB_MSG);
  }

  /* Use the smaller of SYI$_MAXBUF and 2048 for the mailbox size */
  status = sys$getsyiw(0, 0, 0, syilist, &iosb, 0, 0, 0);
  if (status != SS$_NORMAL && !(iosb.status & STS$M_SUCCESS)) {
    vaxc$errno = iosb.status;
    errno = EVMSERR;
    perror ("popen, $GETSYIW failure for SYI$_MAXBUF");
    exit (vaxc$errno | STS$M_INHIB_MSG);
  }
  if (mbxsize > 2048)
    mbxsize = 2048;

  status = sys$crembx (0, &pipe[evf].chan, mbxsize, mbxsize, 0, 0, 0);
  if (status != SS$_NORMAL) {
    vaxc$errno = status;
    errno = EVMSERR;
    perror ("popen, can't create mailbox");
    exit (vaxc$errno | STS$M_INHIB_MSG);
  }

  /* Retrieve mailbox name, use for fopen */
  status = sys$getdviw (0, pipe[evf].chan, 0, &mbxlist, &iosb, 0, 0, 0);
  if (status != SS$_NORMAL && !(iosb.status & STS$M_SUCCESS)) {
    vaxc$errno = iosb.status;
    errno = EVMSERR;
    perror  ("popen can't get mailbox name");
    exit (vaxc$errno | STS$M_INHIB_MSG);
  }

  /* Spawn the command using the mailbox as the name for sys$input */
  mbxname.string[mbxname.length] = 0;
  mbxdsc.dsc$w_length  = mbxname.length;
  mbxdsc.dsc$b_dtype   = DSC$K_DTYPE_T;
  mbxdsc.dsc$b_class   = DSC$K_CLASS_S;
  mbxdsc.dsc$a_pointer = mbxname.string;
  
  
  cmddsc.dsc$w_length  = strlen(cmd);
  cmddsc.dsc$b_dtype   = DSC$K_DTYPE_T;
  cmddsc.dsc$b_class   = DSC$K_CLASS_S;
  cmddsc.dsc$a_pointer = (char *) cmd; /* This is ugly, but otherwise, the
					  compiler yells at me.  */
  flags = CLI$M_NOWAIT;
  if (strcmp(mode,"w") == 0) {
    status = lib$spawn (&cmddsc, &mbxdsc, 0, &flags, 0, &pipe[evf].pid,
			&pipe[evf].iosb, &evf, pdone, &pipe[evf].chan);
    pipe[evf].mode = O_WRONLY;
  }
  else {
    status = lib$spawn (&cmddsc, 0, &mbxdsc, &flags, 0, &pipe[evf].pid,
			&pipe[evf].iosb, &evf, pdone, &pipe[evf].chan);
    pipe[evf].mode = O_RDONLY;
  }
  if (status != SS$_NORMAL) {
    vaxc$errno = status;
    errno = EVMSERR;
    perror("Can't spawn subprocess");
    exit (vaxc$errno | STS$M_INHIB_MSG);
  }

  /* Set up an exit handler so the subprocess isn't prematurely killed */
  if (!FIRST) {
    sys$dclexh (&pexhcb);
    FIRST = 1;
  }

  /* Pipes are always binary mode devices */
  if (pipe[evf].mode == O_WRONLY)
    pipe[evf].file = fopen (mbxname.string, "wb");
  else
    pipe[evf].file = fopen (mbxname.string, "rb");

  return (pipe[evf].file);
}


#ifdef TEST
main (int argc, char **argv)
{
  FILE *stdpipe;
  char line[512];

  while (1) {
    printf ("Enter a command to run >> ");
    fgets (line, 511, stdin);
    if (!strlen(line))
      exit (1);
    line[strlen(line)-1] = 0;
    stdpipe = popen (line, "r");
    if (!stdpipe) {
      fprintf (stderr, "popen failed.\n");
      exit(44);
    }
    fgets (line, 511, stdpipe);
    while (!feof(stdpipe)) {
      fputs (line, stdout);
      fgets (line, 511, stdpipe);
    }
    pclose (stdpipe);
  }
}
#endif
