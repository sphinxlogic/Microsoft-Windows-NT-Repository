/*
 * This file is part of the Seyon, Copyright (c) 1992-1993 by Muhammad M.
 * Saggaf. All rights reserved.
 *
 * See the file COPYING (1-COPYING) or the manual page seyon(1) for a full
 * statement of rights and permissions for this program.
 */

/*
 * This file contains routines for Seyon's terminal. The main routine is
 * terminal(), which reads characters from the terminal and sends them to the
 * port. That routine also forks a child process that reads characters from
 * the port and writes them to the temrinal. Once the parent receives SIGTERM
 * (which should be sent by the grand parent), it kills the child and exits.
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <X11/Intrinsic.h>

#include "seyon.h"
#include "SeDecl.h"

extern char     TtyReadChar();
extern int      MdmReadStr();

extern FILE    *tfp,			/* terminal pointer */
               *cfp;			/* capture file pointer */
extern Boolean  capture;		/* are we capturing or not ? */
extern int      tfd;			/* terminal descriptor */

void            send_tbyte(),
                toggle(),
                cleanup();

pid_t           termProcPid = 0,	/* pid of the terminal subprocess */
                readProcPid = 0; /* pid of child process */

void
TerminalKillHandler(dummy)
     int             dummy;
{
  signal(SIGTERM, SIG_IGN);

  /* Kill the child process and wait for it to die, sounds vicious, 
	 doesn't it? The child process is the read process from the port */

  if (readProcPid)
	if (kill(readProcPid, SIGTERM) == 0) wait((int*)0);
  
  exit(0);
}

/*
 * Terminal: main routine. Has two processes one to read from terminal 
 * and send to the port and the other to do the oppsite.
 */

void
Terminal()
{
  char            c;

  /* Tell the program where to go when SIGTERM is received */
  signal(SIGTERM, TerminalKillHandler);

  termProcPid = getpid();

  /* Split into read and write processes */

  /* Child, read proc: read from port and write to tty */
  if ((readProcPid = SeFork()) == 0)
    PortToTty();

  /* Parent, write proc: read from tty and write to port */
  while (1) {
	if (TtyReadChar(tfd, &c) < 0) {
	  SeError("TTY Read error. Terminal process exiting");
	  if (kill(readProcPid, SIGTERM) == 0) wait((int*)0);
	  ProcRequest(POPUP_ERROR, "TTY Read Error", "errReadError");
	  exit(1);
	}
    send_tbyte(c);
  }
}

/*
 * send a translated character to the modem
 */

void
send_tbyte(c)
     int             c;
{
  switch (c) {

    /*Translate new line to carriage return if newline translation mode is
	  in effect*/
  case '\n':
    switch (newlineTrMode) {
    case 2:
      c = '\r';
      break;
    case 3:
      sendbyte('\r');
    default:
      break;
    }
    break;

    /*Translate backspace to delete if del translation mode is in effect*/
  case 0x08:
    if (qres.backspaceTranslation)
      c = 0x7f;
    break;

  default:
    break;
  }

  /*Send ESC before the character if meta key is pressed with the  character
	and the meta key translation mode is on*/
  if (qres.metaKeyTranslation && (c & 0x80)) {
    sendbyte('\033');
    sendbyte(c);
  }

  /*Send the character to the port*/
  else
    sendbyte(c);
}

/*
 * Cleanup, flush and exit
 */

void
cleanup()
{
  fflush(tfp);
  exit(0);
}

/*
 * Read from the port and write to the tty
 */

void
PortToTty()
{
  static char           zmSig[] = "**\030B00";
  static char          *zmSigPtr = zmSig;
  char                  buf[BUFSIZ], c;
  int                   n, i;

  signal(SIGTERM, cleanup);

  while (1) {

	/* Read incoming charaters and exit the process if a read error
	   is encountered */
    if ((n = MdmReadStr(buf)) < 0) {
	  SeError("Modem Read error. Terminal process exiting");
	  ProcRequest(KILL_TERM, "", "");
	  ProcRequest(POPUP_ERROR, "Modem Read Error", "errReadError");
	  exit(1);
	}

	/* Write incoming characters to the tty */
	fwrite(buf, sizeof(char), n, tfp); 
	fflush(tfp);

	for(i = 0; i < n; i++) {
	  c = buf[i];
	  
	  /* Write to capture file if capture is enabled */
	  if (capture /*&& c != '\r'*/)
		fputc(c, cfp);

	  /* Look for Zmodem signature */
	  if (c != *zmSigPtr)
		zmSigPtr = zmSig;
	  else if (*++zmSigPtr == '\0' /*&& qres.zmodemAutoDownload*/) 
		{ProcRequest(START_AUTO_ZM, "Zmodem Auto-Download", ""); exit(0);}
	} /* for... */
  }	/* while(1)... */
  /*NOT REACHED*/
}

/* ------------------------------------------------------------
 * Routines to manipulate the terminal process. 
 */

/*
 * StartTerminal: start the terminal process.
 */

void
StartTerminal()
{
  if (termProcPid) return;

  if ((termProcPid = SeFork()) == 0)
	{Terminal(); exit(1);}
}

/*
 * KillTerminal: kill the terminal process.
 */

void
KillTerminal()
{
  static void     (*oldSigHandler)();

  if (termProcPid == 0) return;

  oldSigHandler = signal(SIGCHLD, SIG_IGN);

  /* Kill the child and wait for it to die */
  if (kill(termProcPid, SIGTERM) == 0) wait((int*)0);

  termProcPid = 0;
  signal(SIGCHLD, oldSigHandler);
}

/*
 * Restart the terminal process (refresh) by killing it and starting a new
 * one
 */

void
restart_terminal()
{
  KillTerminal();
  StartTerminal();
}
