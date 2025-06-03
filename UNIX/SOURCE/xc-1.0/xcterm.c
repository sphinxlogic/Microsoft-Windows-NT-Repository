/*	xcterm.c -- terminal mode module for XC
	This file uses 4-character tabstops
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <termio.h>
#include "xc.h"

char captfile[SM_BUFF] = CAPTFILE,	/* capture file's name */
	 phonefile[SM_BUFF] = PHFILE,	/* phone number file's name */
	 ddsname[SM_BUFF];
FILE	*cfp;				/* capture file pointer */
static FILE	*fp;			/* file to transmit */
static child_pid;			/* ID of child process */
static jmp_buf rtm, stop;
static void (*oldvec)();
short	s_flag, capture = FALSE;
extern short autoflag, hdplxflag, nl2cr;
extern get_bound_char();

/*  start capturing */
static void 
capt_on(junk)
int junk;
{
	if (capture)
		sprintf(Msg, "Already capturing to \"%s\"", captfile);
	else {
		if (!(cfp = fopen(captfile, "a")))
			sprintf(Msg,"Can't open \"%s\" for capturing",captfile);
		else
			capture = TRUE,
			sprintf(Msg,"Capturing to \"%s\"",captfile),
			setbuf(cfp, NIL(char));
	}
	S2(Msg);

	signal(SIGUSR1, capt_on);	/* set signal for next capt_on */
}

/*  stop capturing */
static void 
capt_off(junk)
int junk;
{
	if (!capture)
		sprintf(Msg,"Sorry, we haven't been capturing lately");
	else
		fclose(cfp),
		capture = FALSE,
		sprintf(Msg,"\"%s\" closed for capturing",captfile);

	S2(Msg);

	signal(SIGUSR2, capt_off);	/* set signal for next capt_off */
}

/*	cleanup, flush and exit */
static void 
cleanup(junk)
int junk;
{
	if (capture)
		fclose(cfp),
		sprintf(Msg,"\"%s\" closed for capturing",captfile),
		S2(Msg);

	exit(0);
}

static void 
cisbmode(junk)
int junk;
{
	cismode = 2;
	signal(SIGCLD, SIG_IGN);
	longjmp(rtm,1);
}

static void 
end_divert(junk)
int junk;
{
	show_abort();
	fclose(fp);
	signal(SIGINT, oldvec);
	longjmp(stop,1);
}

/*	Divert file into input stream, with delay after each newline. */
void 
divert(script)
short script;
{
	int c;
	long i = 1;

	if (!script)
		fputc('\r',tfp),
		fputc('\n',tfp),
		show(-1,"File?"),
		getline(),
		getword();

	if (word[0] == '\0')
		return;

	if (!(fp = fopen(word, "r"))){
		sprintf(Msg,"Can't access '%s'",word);
		S2(Msg);
		return;
	}

	oldvec = signal(SIGINT,end_divert);
	if (setjmp(stop))
		return;

	while ((c = getc(fp)) != EOF){
		if (c != '\n')
			sendbyte(c),
			i++;
		else {
			sendbyte(nl2cr ? '\r' : '\n');
			/*i = (CBAUD-cbaud)*80 + 4*i + 50; /* season to taste... */
			i *= 3;
			if (script)
				k_waitfor(-i, "");
			else
				msecs(i);
			i = 1;
		}
	}
	fclose(fp);
	signal(SIGINT,oldvec);
}

/*	Select a script file. */
static 
get_script()
{
	fputc('\r',tfp),
	fputc('\n',tfp);
	show(-1,"Enter script file:");
	fputc(' ',tfp);
	getline();
	if (line[0] == '\0'){
		fputc('\r',tfp),
		fputc('\n',tfp);
		S1("Script file not specified");
		return FAILURE;
	}
	linkflag = FALSE;
	getword();
	sprintf(ddsname,"%s",word);
	return SUCCESS;
}

void 
terminal(todir)
short todir;
{
	register c;
	short doneyet_dd = FALSE;


Reterm:
	if (setjmp(rtm) || doneyet_dd)
		return;

	mode(NEWMODE);
	s_flag = FALSE;		/* reset scripting flag */

	if (!todir)
		sprintf(Msg, "Entering TERMINAL mode - Escape character is '%s'",
					   unctrl(my_escape)),
		S2(Msg);

	/* split into read and write processes */
	if ((child_pid = forkem()) == 0){
		/* child, read proc: read from port and write to tty */
		cfp = NIL(FILE);
		if (autoflag && !todir)
			capt_on(0);
		signal(SIGUSR1, capt_on);
		signal(SIGUSR2, capt_off);
		signal(SIGTERM, cleanup);

		while (1){
			while ((c = readbyte(0)) == -1)
				;
			if (cismode && c == ENQ)
				cleanup(0);

			fputc(c,tfp);

			if (capture && c != '\r')
				fputc(c,cfp);
		}
		/*NOTREACHED*/
	}
	/* parent, write proc: read from tty and write to port */
	if (cismode)
		signal(SIGCLD, cisbmode);

	if (todir)
		goto dialdir;
	do {
		switch (c = get_bound_char()){
		case CAPTYES:		/* signal child to open capture file */
			kill(child_pid, SIGUSR1);
			break;

		case CAPTEND:		/* signal child to close capture file */
			kill(child_pid, SIGUSR2);
			break;

		case DIVCHAR:		/* divert a file through modem port */
			mode(SIGMODE);
			divert(FALSE);
			mode(NEWMODE);
			break;
	
		case BRKCHAR:
			xmitbrk();
			break;

		case HLPCHAR:
			show_bindings();
			break;

		case SCRPCHR:		/* execute a script file */
			if (get_script()==FAILURE)
				break;

			/* fall through...  */

		case DOSCRPT:		/* named script file */
			s_flag = TRUE;
			goto filicide;

		case DIALCHR:		/* select and dial a phone number */
dialdir:
			doneyet_dd = TRUE;
			if ((dial_dir()==FAILURE && todir) || s_flag)
				goto filicide;
			break;

		case ENDCHAR:		/* signal child to cleanup and exit */
filicide:
			c = ENDCHAR;
			signal(SIGCLD, SIG_IGN);
			kill(child_pid, SIGTERM);
			break;
		
		case QUITCHR:
			signal(SIGCLD, SIG_IGN);
			kill(child_pid, SIGTERM);
			s_exit();
			break;

		case HUPCHAR:		/* Hangup */
			hangup();
			break;

		case '\n':		/* See if NL translation in effect */
			if (nl2cr)
				c = '\r';

		default:	/* just send the character to the port */
			sendbyte(c);
			if (hdplxflag)
				fputc(c,tfp);
			break;
		}
		todir = FALSE;
	} while (c != ENDCHAR);

	while (wait(NIL(int)) >= 0)	/* wait for the read process to die */
		;

	if (s_flag){
		mode(SIGMODE);
		do_script(ddsname);
		goto Reterm;
	}

	reterm = FALSE;
}
