/* scp_tty.c: Operating system-dependent terminal I/O routines.

   Copyright 1994, Robert M Supnik, Digital Equipment Corporation
   Commercial use prohibited

   ttinit	-	called once to get initial terminal state
   ttrunstate	-	called to put terminal into run state
   ttcmdstate	-	called to return terminal to command state
   ttclose	-	called once before the simulator exits
   sim_poll_kbd	-	poll for keyboard input, -1 if no input
   sim_type_tty -	output character to terminal

   Versions are included for generic (BSD) UNIX and for VMS.
*/

#include "sim_defs.h"
int sim_int_char = 005;				/* interrupt character */

/* VMS routines */

#ifdef VMS
#define __TTYROUTINES 0
#include <descrip.h>
#include <ttdef.h>
#include <tt2def.h>
#include <iodef.h>
#include <ssdef.h>
#define EFN 0
unsigned int tty_chan = 0;
typedef struct {
	unsigned short sense_count;
	unsigned char sense_first_char;
	unsigned char sense_reserved;
	unsigned int stat;
	unsigned int stat2; } SENSE_BUF;
typedef struct {
	unsigned short status;
	unsigned short count;
	unsigned int dev_status; } IOSB;
SENSE_BUF cmd_mode = { 0 };
SENSE_BUF run_mode = { 0 };

int ttinit (void)
{
unsigned int status;
IOSB iosb;
$DESCRIPTOR (terminal_device, "tt");

status = sys$assign (&terminal_device, &tty_chan, 0, 0);
if (status != SS$_NORMAL) return SCPE_TTIERR;
status = sys$qiow (EFN, tty_chan, IO$_SENSEMODE, &iosb, 0, 0,
	&cmd_mode, sizeof (cmd_mode), 0, 0, 0, 0);
if ((status != SS$_NORMAL) || (iosb.status != SS$_NORMAL)) return SCPE_TTIERR;
run_mode = cmd_mode;
run_mode.stat = cmd_mode.stat | TT$M_NOECHO & ~(TT$M_HOSTSYNC | TT$M_TTSYNC);
run_mode.stat2 = cmd_mode.stat2 | TT2$M_PASTHRU;
return SCPE_OK;
}

int ttrunstate (void)
{
unsigned int status;
IOSB iosb;

status = sys$qiow (EFN, tty_chan, IO$_SETMODE, &iosb, 0, 0,
	&run_mode, sizeof (run_mode), 0, 0, 0, 0);
if ((status != SS$_NORMAL) || (iosb.status != SS$_NORMAL)) return SCPE_TTIERR;
return SCPE_OK;
}

int ttcmdstate (void)
{
unsigned int status;
IOSB iosb;

status = sys$qiow (EFN, tty_chan, IO$_SETMODE, &iosb, 0, 0,
	&cmd_mode, sizeof (cmd_mode), 0, 0, 0, 0);
if ((status != SS$_NORMAL) || (iosb.status != SS$_NORMAL)) return SCPE_TTIERR;
return SCPE_OK;
}

int ttclose (void)
{
return ttcmdstate ();
}

int sim_poll_kbd (void)
{
unsigned int status, term[2];
unsigned char buf[4];
IOSB iosb;
SENSE_BUF sense;

term[0] = 0; term[1] = 0;
status = sys$qiow (EFN, tty_chan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, &iosb,
	0, 0, &sense, 8, 0, term, 0, 0);
if ((status != SS$_NORMAL) || (iosb.status != SS$_NORMAL)) return SCPE_TTIERR;
if (sense.sense_count == 0) return SCPE_OK;
term[0] = 0; term[1] = 0;
status = sys$qiow (EFN, tty_chan,
	IO$_READLBLK | IO$M_NOECHO | IO$M_NOFILTR | IO$M_TIMED | IO$M_TRMNOECHO,
	&iosb, 0, 0, buf, 1, 0, term, 0, 0);
if ((status != SS$_NORMAL) || (iosb.status != SS$_NORMAL)) return SCPE_OK;
if (buf[0] == sim_int_char) return SCPE_STOP;
return (buf[0] | SCPE_KFLAG);
}

int sim_type_tty (char out)
{
unsigned int status;
IOSB iosb;

status = sys$qiow (EFN, tty_chan, IO$_WRITELBLK | IO$M_NOFORMAT,
	&iosb, 0, 0, &out, 1, 0, 0, 0, 0);
if ((status != SS$_NORMAL) || (iosb.status != SS$_NORMAL)) return SCPE_TTOERR;
return SCPE_OK;
}
#endif

/* UNIX routines */

#ifndef __TTYROUTINES
#include <sgtty.h>
#include <fcntl.h>

struct sgttyb cmdtty,runtty;			/* V6/V7 stty data */
struct tchars cmdtchars,runtchars;		/* V7 editing */
struct ltchars cmdltchars,runltchars;		/* 4.2 BSD editing */
int cmdfl,runfl;				/* TTY flags */

int ttinit (void)
{
cmdfl = fcntl (0, F_GETFL, 0);			/* get old flags  and status */
runfl = cmdfl | FNDELAY;
if (ioctl (0, TIOCGETP, &cmdtty) < 0) return SCPE_TTIERR;
if (ioctl (0, TIOCGETC, &cmdtchars) < 0) return SCPE_TTIERR;
if (ioctl (0, TIOCGLTC, &cmdltchars) < 0) return SCPE_TTIERR;
runtty = cmdtty;				/* initial run state */
runtty.sg_flags = cmdtty.sg_flags & ~(ECHO|CRMOD) | CBREAK;
runtchars.t_intrc = sim_int_char;		/* interrupt */
runtchars.t_quitc = 0xFF;			/* no quit */
runtchars.t_startc = 0xFF;			/* no host sync */
runtchars.t_stopc = 0xFF;
runtchars.t_eofc = 0xFF;
runtchars.t_brkc = 0xFF;
runltchars.t_suspc = 0xFF;			/* no specials of any kind */
runltchars.t_dsuspc = 0xFF;
runltchars.t_rprntc = 0xFF;
runltchars.t_flushc = 0xFF;
runltchars.t_werasc = 0xFF;
runltchars.t_lnextc = 0xFF;
return SCPE_OK;					/* return success */
}

int ttrunstate (void)
{
runtchars.t_intrc = sim_int_char;		/* in case changed */
fcntl (0, F_SETFL, runfl);			/* non-block mode */
if (ioctl (0, TIOCSETP, &runtty) < 0) return SCPE_TTIERR;
if (ioctl (0, TIOCSETC, &runtchars) < 0) return SCPE_TTIERR;
if (ioctl (0, TIOCSLTC, &runltchars) < 0) return SCPE_TTIERR;
return SCPE_OK;
}

int ttcmdstate (void)
{
fcntl (0, F_SETFL, cmdfl);			/* block mode */
if (ioctl (0, TIOCSETP, &cmdtty) < 0) return SCPE_TTIERR;
if (ioctl (0, TIOCSETC, &cmdtchars) < 0) return SCPE_TTIERR;
if (ioctl (0, TIOCSLTC, &cmdltchars) < 0) return SCPE_TTIERR;
return SCPE_OK;
}

int ttclose (void)
{
return ttcmdstate ();
}

int sim_poll_kbd (void)
{
int status;
char buf[1];

status = read (0, buf, 1);
if (status != 1) return SCPE_OK;
else return (buf[0] | SCPE_KFLAG);
}

int sim_type_tty (char out)
{
write (1, &out, 1);
return SCPE_OK;
}
#endif
