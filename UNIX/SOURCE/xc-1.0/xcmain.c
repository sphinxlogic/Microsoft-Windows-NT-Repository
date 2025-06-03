/*	xcmain.c -- main module for XC
	This file uses 4-character tabstops
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>
#include <termio.h>
#include <sys/ioctl.h>
#include <setjmp.h>
#include "xc.h"

#define Resume_Not_Allowed	1

short
	autoflag =	FALSE,	/* Automatic capturing */
	cismode =	FALSE,	/* Automatic response to CIS "ENQ" */
	cr_add =	TRUE,	/* Add cr to nl in B+ uploads */
	hdplxflag =	FALSE,	/* Half-duplex mode */
 	menuflag =	TRUE,	/* Show mini-menu */
	nl2cr =		TRUE,	/* Map nl to cr when transmitting ASCII */
	reterm =	FALSE,	/* Jumping into terminal mode */
	statflag =	FALSE,	/* Flag for status display */
	eofflag =	FALSE;	/* Flag to quit a script */
int s_cis(), s_set(), s_exit(), s_shell();
char Msg[SM_BUFF];
unchar BS, LK;
FILE *tfp;
struct termio newmode, oldmode, sigmode;
static char	*statfmt = "\r\t\t%-8s %25s %s\r\n",
			version[]="@(#)XC 4.1 JPRadley 10 April 1993",
			oldshell[SM_BUFF],
			*babble[] = {
				"\r\nUsage: xc [-l device] [-s file | -t]",
				"\t-l device\tUse 'device' as the modem port",
				"\t-s script\tExecute 'script' immediately",
				"\t-t\t\tEnter terminal mode immediately",
				NIL(char)
			};
static s_script(), s_xmodem(), s_term(), s_help(), s_dial(), puttake(),
	SET_proto(), SET_cr(), SET_cis(), SET_nl(), SET_xon(), SET_xcape(),
	SET_menu(), SET_hdplx(), SET_bps(), SET_autocapt(), SET_cfile(),
	SET_pfile();
extern short scriptflag;
extern void B_Transfer(), dbglog(), mattach(), terminal(), xreceive(), xsend(),
	get_ttype(), unlock_tty();
jmp_buf erret;			/* non-local error return */

struct kw {				/* Used by command parsing routines */
	char *keyword;
	int (*rtn)();
};

static struct kw cmds[] = {
	{"c",		s_cis},
	{"cis",		s_cis},
	{"s",		s_script},
	{"script",	s_script},
	{"h",		hangup},
	{"hangup",	hangup},
	{"bindings",show_bindings},
	{"rb",		s_xmodem},
	{"rt",		s_xmodem},
	{"sb",		s_xmodem},
	{"st",		s_xmodem},
	{"set",		s_set},
	{"t",		s_term},
	{"term",	s_term},
	{"d",		s_dial},
	{"dial",	s_dial},
	{"q",		s_exit},
	{"quit",	s_exit},
	{"exit",	s_exit},
	{"x",		s_exit},
	{"!",		s_shell},
	{"!!",		s_shell},
	{"$",		s_shell},
	{"%p",		puttake},
	{"%t",		puttake},
	{"help",	s_help},
	{"?",		s_help},
	{NIL(char),	0}
};

static struct kw setlist[] = {
	{"auto",	SET_autocapt},
	{"baud",	SET_bps},
	{"bps",		SET_bps},
	{"cfile",	SET_cfile},
	{"cis",		SET_cis},
	{"cr",		SET_cr},
	{"hdplx",	SET_hdplx},
	{"menu",	SET_menu},
	{"nl",		SET_nl},
	{"pfile",	SET_pfile},
	{"proto",	SET_proto},
	{"escape",	SET_xcape},
	{"xcape",	SET_xcape},
	{"xon",		SET_xon},
	{"xoff",	SET_xon},
	{NIL(char),	0}
};

/* Print the status of the program */
static void
status()
{
	struct kw *ptr;
	char p[30];
	int (*fct)() = 0;

	statflag = TRUE;

	cls();
	cur_off();
	sprintf(p,"Modem Port: %s",mport(NIL(char)));
	drawline(0, 0, CO);
	ttgoto(1, 9);
	sprintf(Msg,"%-29s%29s",&version[4], p);
	S;
	drawline(2, 0, CO);
	ttgoto(3, 0);
	fprintf(tfp, statfmt, "Keyword", "Description", "Status");
	fprintf(tfp, statfmt, "--------", "-------------------------", "-----------");

	for (ptr = setlist; ptr->keyword; ptr++)
		if (ptr->rtn != fct){
			fct = ptr->rtn;
			(*fct)();
		}

	ttgoto(18, 25);
	S1("Type \"help\" or ? for help");
	statflag = FALSE;
	cur_on();
}

/* Catch a signal and jump to main. Reset signal and do a longjmp */
static void
catch(junk)
int junk;
{
	if (! isatty(2))
		hangup(),
		s_exit();

	S2("XC: Interrupt");

	signal(SIGINT,catch);
	signal(SIGQUIT,catch);
	longjmp(erret,1);
}

static void
usage()
{
	char **ptr;

	for (ptr = babble; *ptr; ptr++)
		fprintf(tfp, "%s\r\n", *ptr);
}

main(argc, argv)
int argc;
char **argv;
{
	char *script = NIL(char);
	extern char *optarg;
	int c;
	extern int optind;

	struct kw *ptr;
	tfp = stderr;
	if (isatty(2))
		get_ttype();

	ioctl(0, TCGETA, &oldmode);	/* get current tty mode	*/

	/* trap for SIGHUP and SIGTERM, make sure LCKfile gets killed */
	signal(SIGHUP,(void *)s_exit);
	signal(SIGTERM,(void *)s_exit);

	newmode = oldmode;

	newmode.c_iflag &= ~(IXON | IXOFF | IXANY);
	newmode.c_lflag &= ~(ICANON | ISIG | ECHO);
	newmode.c_oflag = 0;
	newmode.c_cc[VMIN] = 1;
	newmode.c_cc[VTIME] = 1;
	BS = newmode.c_cc[VERASE];
	LK = newmode.c_cc[VKILL];

	sigmode = newmode;
	sigmode.c_lflag |= ISIG;

	oldshell[0] = '\0';	/* set last command to blank */
	if (setjmp(erret))	/* set error handler to exit */
		exit(0);		/*  while parsing command line */
	signal(SIGINT,catch);	/* catch break & quit signals/keys */
	signal(SIGQUIT,catch);

	default_bindings();

	while ((c = getopt(argc, argv, "s:l:t")) != -1)
		switch (c){
		case 'l':	/* set modem port name */
			mport(optarg);
			break;
		case 's':	/* Execute SCRIPT file */
			script = optarg;
			break;
		case 't':	/* jump into terminal mode */
			reterm = TRUE;
			break;
		default:	/* Bad command .. print help */
			usage();
			exit(1);
		}

	setuid(geteuid());
	setgid(getegid());

	mopen();	/* opens and configures modem port, or exits */

	setuid(getuid());
	setgid(getgid());

	do_script(STARTUP);

#if DEBUG
	dbglog();
#endif

	if (!script)
		status();

	for (;;){
		setjmp(erret);
		signal(SIGQUIT,(void *)s_exit);
		mode(SIGMODE);

		if (script)
			do_script(script),
			script = NIL(char),
			reterm = TRUE;

		if (reterm && isatty(2)){
			s_term();
			continue;
		}

		fputc('\r',tfp),
		fputc('\n',tfp);
 		if (menuflag)
			fputc('\t',tfp),
 			S1("[d]ial directory  [t]erminal mode  [q]uit  [s]cript  [?]help");
		show(-1,"<XC>");
		fputc(' ',tfp);

		lptr = line;
		getline();
		fputc('\r',tfp),
		fputc('\n',tfp);

		getword();
		lc_word(word);
		if (word[0] == '\0')		/* If blank line... reprompt */
			continue;

		for (ptr = cmds; ptr->keyword; ptr++)
			if (!strcmp(word, ptr->keyword))
				break;

		if (ptr->keyword)
			(*ptr->rtn)();
		else
			sprintf(Msg,"Unrecognized command: %s",word),
			S;
	}
}

static
s_script()
{
	getword();

	if (word[0] == '\0'){
		S1("Script file not specified");
		return;
	}

	sprintf(ddsname,"%s",word);
	do_script(ddsname);
	reterm = TRUE;
}

static
s_xmodem()
{
	char d = word[0];
	char c = word[1];
	char oldproto[4];

	strcpy(oldproto, protocol);

	xc_setflow(FALSE);
	xc_setproto("8N1");

	getword();
	if (word[0] == '\0')
		S1("Transfer file not specified");
	else if (d == 's')
		xsend(c);
	else
		xreceive(c);

	reterm = TRUE;
	xc_setflow(flowflag);
	xc_setproto(oldproto);
}

static
s_term()
{
	terminal(FALSE);
	if (cismode != 2)
		return;
	cismode = 1;
	s_cis();
}

static
s_dial()
{
	terminal(TRUE);
	if (cismode != 2)
		return;
	cismode = 1;
	s_cis();
}

s_cis()
{
	char oldproto[4];

	strcpy(oldproto, protocol);

	xc_setflow(FALSE);
	xc_setproto("8N1");
	mode(SIGMODE);

	B_Transfer();

	reterm = TRUE;
	xc_setflow(flowflag);
	xc_setproto(oldproto);
}

s_shell()
{
	int stat_loc = 0;
	char c = word[0];
	static char *shell = NIL(char);
	void (*oldvec)();

#if NOSHELL
	return(0);
#endif
	if (word[0] == word[1])
		strcpy(wptr = word, oldshell);
	else {
		getword();
		if (*wptr)
			strcpy(oldshell, wptr);
	}

	if (!shell){
		shell = getenv("SHELL");
		if (!shell)
			shell = "/bin/sh";
	}

	fputc('\r',tfp),
	fputc('\n',tfp);
	mode(OLDMODE);

	if (!forkem()){
		if (c == '$')	/* Attach modem to stdin, stdout */
			mattach();
		signal(SIGCLD,SIG_DFL);
		signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);
		if (word[0] == '\0')
			execl(shell, shell, "-i", NIL(char));
		else
			execl(shell, shell, "-c", wptr, NIL(char));
		S1("Exec failed!");
		exit(2);
	}

	oldvec = signal(SIGINT,SIG_IGN);
	wait(&stat_loc);
	signal(SIGINT,oldvec);

	strcpy(oldshell, wptr);
	return(!!stat_loc);
}

static char	*cmdlist[] = {
	"\tXC Command Summary",
	"",
	"\tc",
	"\tcis\t\tInitiate CIS B+ File Transfer (Upload and Download)",
	"",
	"\td",
	"\tdial\t\tDialing directory",
	"",
	"\tx",
	"\tq",
	"\texit",
	"\tquit\t\tExit XC",
	"",
	"\th",
	"\thangup\t\tHang up the modem",
	"",
	"\trb file\t\tXMODEM receive file 'file' (binary mode)",
	"\trt file\t\tXMODEM receive file 'file' (Ascii mode)",
	"",
	"\tsb file...\tXMODEM send file 'file' (binary mode)",
	"\tst file...\tXMODEM send file 'file' (Ascii mode)",
	"",
	"\tset\t\tDisplay XC parameters",
	"\tset kw\t\tDisplay XC parameter for 'kw'",
	"\tset kw val\tSet XC keyword 'kw' to 'val'",
	"",
	"\ts file",
	"\tscript file\tExecute XC script 'file'",
	"",
	"\tt",
	"\tterm\t\tEnter terminal mode",
	"",
#if !NOSHELL
	"\t!\t\tExecute a local interactive shell",
	"\t! cmd\t\tExecute shell command string on the local system",
	"\t!!\t\tRe-execute the last shell command string",
	"",
	"\t$ cmd\t\tShell command with stdin and stdout redirected to modem",
	"",
#endif
	"\t%p loc [rem]\tPut local file to a UNIX system",
	"",
	"\t%t rem [loc]\tTake remote file from a UNIX system",
	"",
	"\t?",
	"\thelp\t\tPrint (this) help text",
	"",
	"\tSET Keywords:",
	"",
	"\tset\t\t\tDisplay current XC status",
	"",
	"\tset auto on|off\t\tSet|Unset automatic capturing",
	"",
	"\tset bps value",
	"\tset baud value\t\tSet Bits/Second to 'value'",
	"",
	"\tset cfile name\t\tChange name of capture file",
	"",
	"\tset cis on\t\tSet CIS <ENQ> mode (Auto up/download)",
	"\tset cis off\t\tDo not respond to <ENQ>",
	"",
	"\tset cr on|off\t\tSet|Unset Carriage Return Injection mode",
	"",
	"\tset xcape char",
	"\tset escape char\t\tSet the Terminal mode escape character",
	"",
	"\tset hdplx on\t\tSet half-duplex mode",
	"\tset hdplx off\t\tUnset half-duplex mode (use full-duplex)",
	"",
 	"\tset menu on|off\t\tDo|Don't show mini-menu before XC prompt",
	"",
	"\tset nl on|off\t\tSet|Unset newline translation",
	"",
	"\tset pfile name\t\tChange name of phonelist file",
	"",
	"\tset proto 7E2\t\tSet 7-bit character size, even parity",
	"\tset proto 7O2\t\tSet 7-bit character size, odd parity",
	"\tset proto 8N1\t\tSet 8-bit character size, no parity",
	"",
	"\tset xon on|off",
	"\tset xoff on|off\t\tSet|Unset XON/XOFF flow control",
	"",
	"",
	NIL(char) };

static
s_help()
{
	char **ptr = cmdlist;
	int curline = 0;

	mode(OLDMODE);
	cls();
	cur_off();
	for ( ; *ptr; ptr++) {
		if (**ptr != '') {
			if (curline >= LI-2){
				S0("PRESS ENTER");
				getline();
				cls();
				curline = 0;
			}
			fprintf(tfp, "%s\r\n", *ptr);
			curline++;
		} else {
			S0("PRESS ENTER");
			getline();
			cls();
			curline = 0;
		}
	}
	show_bindings();
	S0("PRESS ENTER");
	getline();
	cls();
	status();
}

s_set()
{
	struct kw *ptr;

	getword();

	if (word[0] == '\0' && !scriptflag){
		status();
		return;
	} else if (word[0] == '\0'){
		S1("SET keyword requires an argument");
		eofflag++;
		return;
	}

	lc_word(word);

	for (ptr = setlist; ptr->keyword; ptr++)
		if (!strcmp(ptr->keyword, word)){
			(*ptr->rtn)();
			return;
		}

	sprintf(Msg,"Invalid SET keyword: %s", word);
	S;
	eofflag++;
}

void
set_onoff(flag)
short *flag;
{
	char *ptr = strdup(word);

	uc_word(ptr);
	getword();
	lc_word(word);

	if (!strcmp(word, "on"))
		*flag = TRUE;
	else if (!strcmp(word, "off"))
		*flag = FALSE;
	else
		sprintf(Msg,"Set '%s' value must be 'on' or 'off'",ptr),
		S,
		eofflag++;

	free(ptr);
}

static
SET_proto()
{
	if (statflag){
		fprintf(tfp, statfmt, "proto", "Port set to", protocol);
		return;
	}

	getword();
	uc_word(word);
	if (word[0] == '\0')
		S1("Set proto must be 7E2, 7O2, or 8N1");
	else if (!xc_setproto(word))
		sprintf(Msg,"Unsupported protocol %s",word),
		S;
	eofflag++;

	if (!scriptflag)
		sprintf(Msg,"Port set to %s", protocol),
		S;
}

static
SET_cr()
{
	if (statflag){
		fprintf(tfp, statfmt, "cr", "Carriage Return Injection",
			cr_add ? "ON" : "OFF");
		return;
	}

	set_onoff(&cr_add);

	if (!scriptflag)
		sprintf(Msg,"Carriage Returns %s injected in B+ ASCII uploads",
			cr_add ? "ARE" : "are NOT"),
		S;
}

static
SET_xcape()
{
	if (statflag) {
		fprintf(tfp, statfmt, "xcape", "Terminal Escape Character",
				 unctrl(my_escape));
		return;
	}

	getword();
	if (word[0] == '\0') {
		show(1,"Set ESCAPE must specify escape character");
		eofflag++;
		return;
	}

	my_escape = word[0];

	if (!scriptflag)
		sprintf(Msg,"Terminal mode escape character set to '%s'",
				unctrl(my_escape)),
		S;
}

static
SET_nl()
{
	if (statflag){
		fprintf(tfp, statfmt, "nl", "Newline Translation",
			nl2cr ? "ON" : "OFF");
		return;
	}

	set_onoff(&nl2cr);

	if (!scriptflag)
		sprintf(Msg,"Newlines %s changed to Carriage Returns",
			nl2cr ? "ARE" : "are NOT"),
		S;
}

static
SET_cis()
{
	if (statflag){
		fprintf(tfp, statfmt, "cis", "CIS <ENQ> Auto Download",
			cismode ? "ON" : "OFF");
		return;
	}

	set_onoff(&cismode);

	if (!scriptflag)
		sprintf(Msg,"CIS <ENQ> Auto Download is %s", cismode ? "ON" : "OFF"),
		S;
}

static
SET_xon()
{
	if (statflag){
		fprintf(tfp, statfmt, "xoff", "Terminal Mode XON/XOFF",
			flowflag ? "ON" : "OFF");
		return;
	}

	set_onoff(&flowflag);
	xc_setflow(flowflag);

	if (!scriptflag)
		sprintf(Msg,"XON/XOFF Flow control is %s", flowflag ? "ON" : "OFF"),
		S;
}

static
SET_bps()
{
	if (statflag){
		char br[6];
		sprintf(br, "%d", mrate(NIL(char)));
		fprintf(tfp, statfmt, "bps", "Bits per Second", br);
		return;
	}

	getword();
	if (word[0] == '\0')
		S1("Set BPS (or BAUD) must have a rate");
	else if (!mrate(word))
		sprintf(Msg,"Unsupported bps rate %s",word),
		S;
	eofflag++;
	if (!scriptflag)
		sprintf(Msg,"Bits/Second set to %d",mrate(NIL(char))),
		S;
}

static
SET_hdplx()
{
	if (statflag){
		fprintf(tfp, statfmt, "hdplx", "Half-duplex Mode",
			hdplxflag ? "ON" : "OFF");
		return;
	}

	set_onoff(&hdplxflag);

	if (!scriptflag)
		sprintf(Msg,"Half-duplex Mode is %s", hdplxflag ? "ON" : "OFF"),
		S;
}

static
SET_menu()
{
 	if (statflag){
 		fprintf(tfp, statfmt, "menu", "Mini-menu mode",
 			menuflag ? "ON" : "OFF");
 		return;
	}
 
 	set_onoff(&menuflag);
 
 	if (!scriptflag)
 		sprintf(Msg,"Mini-menu is %s shown", menuflag ? "" : "NOT"),
		S;
}

static
SET_autocapt()
{
	if (statflag){
		fprintf(tfp, statfmt, "auto", "Auto Capture",
			autoflag ? "ON" : "OFF");
		return;
	}

	set_onoff(&autoflag);

	if (!scriptflag)
		sprintf(Msg,"Auto Capture is %s", autoflag ? "ON" : "OFF"),
		S;
}

static
SET_cfile()
{
	if (statflag){
		fprintf(tfp, statfmt, "cfile", "Capture File", captfile);
		return;
	}

	getword();
	if (word[0] == '\0'){
		S1("Set CFILE must have file name");
		eofflag++;
		return;
	}

	strcpy(captfile, word);

	if (!scriptflag)
		sprintf(Msg,"Capture file set to '%s'",captfile),
		S;
}

static
SET_pfile()
{
	if (statflag){
		fprintf(tfp, statfmt, "pfile", "Phone Number File", phonefile);
		return;
	}

	getword();
	if (word[0] == '\0'){
		S1("Set PFILE must have file name");
		eofflag++;
		return;
	}

	strcpy(phonefile, word);

	if (!scriptflag)
		sprintf(Msg,"Phone number file set to '%s'",phonefile),
		S;
}

/*	Put and Take a file to/from a UNIX-type "cu" system. Unfortunately,
	the stty command is one of those commands that always gets changed
	with different UNIX systems, so you will get (at least) a file full of
	^M on the take command for systems later than V7 or work-alikes.

	Additionally, the Take command takes a bit too much!

	Fixed a lot of this: JPRadley 89/07/27
*/

static
puttake()
{
	FILE *fp;
	int i, Ch;
	char c = word[1], fname[SM_BUFF], tname[SM_BUFF], wrkbuf[SM_BUFF];

	getword();

	signal(SIGINT,catch);
	signal(SIGQUIT,catch);
	xc_setflow(TRUE);
	if (word[0] == '\0'){
		sprintf(Msg,"Must give a filename with the '%%%c' option",c);
		S;
		return;
	}

	strcpy(fname, word);
	getword();
	if (word[0] == '\0')
		strcpy(tname, fname);
	else
		strcpy(tname, word);
	switch (c){
	case 'p':
		if (!(fp = fopen(fname, "r")))
			sprintf(Msg,"Can't open '%s'",fname),
			S;
		else {
			fprintf(tfp, "\r\nPutting file '%s' to '%s' on remote UNIX\r\n",
				fname, tname);
			sprintf(wrkbuf,
				"sh -c \"stty -echo;(cat >%s)||cat >/dev/null;stty echo\"\n",
					tname);
			send_string(wrkbuf);	/* send command string to remote shell */
			i = 64;
			while ((Ch = getc(fp)) != EOF){
				if (++i > 64){		/* this prevents an overload on the */
					i = 0;			/* receiver's input buffer (64=kludge) */
					msecs((1+CBAUD-cbaud) * 100);
				}
				sendbyte(Ch);		/* send characters to cat command */
			}
			fclose(fp);
			sendbyte(EOT);			/* send a ^D to cat */
			purge();				/* get rid of whatever was sent back */
			sendbyte('\n');
		}
		break;

	case 't':
		strcpy(Name, tname);
		if ((fp=QueryCreate(Resume_Not_Allowed))){
			fprintf(tfp, "\r\nTaking file '%s' from remote UNIX to '%s'\r\n",
				fname, tname);	
			purge();
			sprintf(wrkbuf,
				"sh -c \"stty nl;test -r %s&&cat %s;echo %c;stty -nl\"\n",
					fname, fname, DLE);	/* if 'fname' has a DLE, we'll die */
			send_string(wrkbuf);		/* send command to remote shell */
			while (readbyte(3) != '\n')	/* discard up to the \n in wrkbuf */
				;
			while ((Ch=readbyte(0)) != -1	/* while chars are being sent */
					 && Ch != DLE)			/* and we haven't seen our DLE */
				fputc(Ch,fp);
			fclose(fp);
		}
		break;
	}
	xc_setflow(flowflag);
	reterm = TRUE;
}

s_exit()
{
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGTERM,SIG_IGN);

	mode(OLDMODE);
	unlock_tty();

	exit(0);
}
