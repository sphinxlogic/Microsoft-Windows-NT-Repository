Article 6 of alt.sources:
Path: mit-eddie!husc6!labrea!decwrl!mejac!hoptoad!ncoast!devon!stb!michael
From: michael@stb.UUCP (Michael)
Newsgroups: alt.sources
Subject: Bulletin board program
Message-ID: <1644@stb.UUCP>
Date: 15 Jul 87 06:42:23 GMT
Reply-To: michael@stb.UUCP (Michael)
Distribution: alt
Organization: STB BBS, La, Ca, USA, 90402, (213) 459-7231
Lines: 1473

This is a bulletin board system, a program designed to let people read and leave
messages and electronic mail. It is not as powerful as news nor /bin/mail;
it was originally written for an 8 bit micro (in basic), and roughly translated
over time. It is relatively bug free (however, the chat command was never
implemented), will work with multiple users at once, and is "completely secure"
-- after losing directories when I ran it as root (unlink ("") did wonders in
V7) I made sure it was secure enough to be a login "shell" that does not need
a password, and still usable by regular users. If you want to see it in action,
give me a call (# is in header, PC-pursuit will work, 3/12/2400)

I have it use the directory /bbs (not /usr/lib/bbs) for personal reasons; feel
free to change it.

(Oh yes-- the code comments make reference to a tree structured message base.
It never got implemented.)
This is not a shar; there are several lines with CUT in them.
PAcking list: /bbs/help userlog.h fixbbs.c logsort.c bbs.c
--- Lights, camera, help file --- (/bbs/help)
Rough draft of help file
B: read bulletin
C: Chat (not working)
F: Leave feedback
G: goodbye, hangup
H: This file
I: System information
L: Leave message
M: Mail user
O: Other systems
Q: Quit (hangup, don't update auto-reads)
R: Read
U: Userlog
X: eXpert mode (this call only)
Y: Return to system
Z: Logon again
ZS: same as Y
?: Same as H

Note: if a second word is given on L, M, and F commands, the system will prompt
before each line (for automated uploads)
--- He hacked him to bits with his CUTlass --- (userlog.h)

/* sys/types.h and sys/dir.h MUST be included already */

struct userlog {
	char	name[DIRSIZ+1];
	char	password[9];
	long	lastread[6];		/* allow enough for 5 boards */
	char	extra[7];		/* size=64 bytes after alignment */
	time_t	lastcall;
	short	numcalls;
	char	lastbyte;
	};

typedef	struct userlog ulog;
--- CUT through the trees over there --- (fixbbs.c)
/* @(#)bbsfix.c	1.2 7/15/87 06:19:41 ( 10/5/86 15:26:56 ) */
# include <stdio.h>
extern int errno;

/* 1.2 reduced the amount of output (no longer copy board to output) */
/* Fix an index file if it gets clobbered. Only 200 messages (unless you
change the constants */

main(argc, argv)
char *argv[];
{
	int fidx, msg;
	FILE *fdat;
	char sidx[200], sdat[200];
	if (argc != 3) {printf("Useage: fixbbs <boardname> <lowmsg>. Only one board fixed per call\n "); exit(1); }
	umask (0002);		/* Only turn off world write */
	sprintf (sidx, "/bbs/idx/%s", argv[1]);
	sprintf (sdat, "/bbs/dat/%s", argv[1]);
	msg=0; sscanf (argv[2], "%d", &msg);
	if (msg==0) die ("Can't read message number");
	fidx = creat (sidx, 0666);
	if (fidx == -1) die ("Index file?!?!?!?");
	fdat = fopen (sdat, "r");
	if (fdat == NULL) die ("Data file ?!?!?!?!!");
	fixit (fdat, fidx, msg);
}

die(s)
char *s;
{
	printf ("Death: %s %d\n", s, errno);
	abort();
}

mywrite (idx, addr)
long addr;
{
	write (idx, &addr, sizeof addr);
}

findmsg (data)
FILE *data;
{
	int x;
	while ((x=getc(data)) != '\001' && x != EOF) ;
	if ((x=ungetc(getc(data), data)) != EOF) return;
	printf("EOF Found\n");
}

fixit (data, idx, lowmsg)
FILE * data;
{
	int cnt=lowmsg;
	mywrite (idx, lowmsg);
	if (empty(idx)) return;
	mywrite (idx, 0L);		/* first message is at 0 */
	while (!feof(data))
	{	printf("Message %d found at %d\n", cnt++, ftell(data) );
		findmsg(data);
		if (!feof(data)) mywrite (idx, ftell(data));
		else printf("Final message, terminating \n");
	};
}

empty(fd)
{
	return (! lseek(fd, 0L, 2)); /* lseek=0 if empty */
}
--- "Enough of the CUTe cut lines" --- (logsort.c)
/* @(#)logsort.c	1.1 7/15/87 06:23:32 ( 10/2/86 18:51:59 ) */
/* Log file sort routine for stb. */

# define NUMLOGS	250		/* Maximun # of userlog entries */
# include <stdio.h>
# include <sys/types.h>
# include <sys/dir.h>
# include <local/userlog.h>
# include <sys/locking.h>

ulog *logs;
int number, fd;
FILE *fp;

main()
{
	readit();
	sortit();
	writeit();
}

comp_log(p1, p2)
ulog *p1, *p2;
{
	return (p2->lastcall - p1->lastcall);	/* if = calls, sort by time */
	/* NOTREACHED */			/* old code to sort by calls */
	if (p1->numcalls < p2->numcalls)	/* return larger calls */
		return 1;
	if (p1->numcalls > p2->numcalls)
		return -1;
}
	
sortit()
{
	qsort(logs, number, sizeof (*logs), comp_log);
}

readit()
{
	fd=open("/bbs/userlog",2);
	fp=fdopen(fd,"r+");
	if (fp==NULL)
	{
		perror("log_sort: /bbs/userlog");
		abort();
	}
	locking (fileno(fp), LK_LOCK, 0);
	logs=(ulog *) calloc (NUMLOGS, sizeof (*logs));
	number=fread(logs, sizeof (*logs), NUMLOGS, fp);
}

writeit()
{
	rewind(fp);
	fwrite(logs, sizeof (*logs), number, fp);
}
--- "CUT!" yelled the director. --- (bbs.c)

/* @(#)stb.c	3.25 7/15/87 06:13:39 ( 2/1/87 20:34:27 ) */

#define VERSION 3
#define FIX 25
#define STB_UID 13
#define STB_GID 13
#define OUTBUF 20		/* output buffering quantity, tunable */
#define crypt(string, salt)	string  /* No encryption */
#define alarm(time)		/* 1.1 No more hangup detection */
/* Serial Tree Board, ver 4.0 (basic algorythm), 0.3 (C conversion) */
/* similarities in algorthm between C and basic are rapidly disapearing, and
	the only similarity left seems to be the basic structure of the
	program. */
/* Changes:
	he_left now updates the userlog at every logon (not after final hangup)
	hangup signal is not trapped; o/s kills the job resulting in effective
		quit command.
*/

/* Version 3. Minor fixes, improvements. Speed up output,
	anything else I think of */

/* Version 2 Get the editor working correctly */

/* 1.6 strchr/strrchr for index/rindex */
/* 1.1 Message read is now by message number. */

/* .8 Final (?) ctrl-c/k fix:
	cbreak/cooked maintain a count of state;
	cbreak also disables echo, cooked re-enables.

	Where to put a <cr> out for ctrl-c/k? FNStop?
*/

/* .7 fix ctrl-c/k that .6 destroyed.
	Fixed by switching between cbreak and cooked. */
/* .6 changes: Will remove ALL calls to signal (except for sighup) */

/* .3 changes: Will use better signal operations (actually turn them
	on/off) and other things. Goal: Make it look done. This includes
	the bbs answering the phone, with getty never seeing the line
	(minor change to /etc/default/newgetty).

	New command: Y, myexits program without hanging up (re-spawn getty)
	EOF in message now means end of message, not hang up.

	Tree might get implemented one of these days.
 */

/* Tree implementation details:
/bin/mkdir is used to create a directory whose name is the title, which has
.text	text of message
dir1	child message 1
dir2	child message 2
etc. The root is in /bbs/tree/root, and should be on a mountable file system
	so it doesn't gobble up root space. /bbs/idx/tree has the following:
msg # [7]
parent # [7]
left # [7]
right # [7]
child # [7]
title [15]
date [11]
total record space: 61. Fields are padded with spaces on the right on disk,
with the line terminated by a newline. Msg titles need not be distinct, but
only the first of duplicates will be found.  Numbers are in ascii. All the data
is on one line. Number first, parent second, title third, date forth.
Read/written with scanf/printf, not read/write.
*/

/* Serial format: Index files are arrays of longs. [0]=lowest number message,
	sequentially up from there. */

# include <stdio.h>
# include <errno.h>
# include <sgtty.h>
# include <fcntl.h>
# include <signal.h>
# include <assert.h>
# include <ctype.h>
# include <sys/types.h>
# include <sys/locking.h>
# include <sys/stat.h>
# include <sys/dir.h>
# include <local/userlog.h>

#if defined(LK_UNLCK) && !defined(LK_UNLK)	/* sys 3 spelling change */
#  define LK_UNLK LK_UNLCK
#else
#  define LK_UNLCK LK_UNLK
#endif

#if defined(SYS3) || defined (M_SYS3) || defined (ATT) || defined (SYS5)
#   define index strchr
#   define rindex strrchr
#endif

# define	FALSE		0

#ifdef lint
# define	TRUE		1
#else
# define	TRUE		!FALSE
#endif

# define	CTRL_K		1
# define	CTRL_C		2
# define	UPDATE		TRUE
# define	NUPDATE		FALSE
# define	LINESIZE	257	/* most that can be typed +1 */
# define	WAIT_TIME	10*60
					/* hangup assumed after 10 minutes */

struct lines {
	struct lines	*next;		/* pointer to next line */
	char	text[LINESIZE];	/* define line for input text */
	};

# define	NUMSIZE		7
# define	DATESIZE	11

struct treejunk {
	char	t_msgnum[NUMSIZE];
	char	t_parent[NUMSIZE];
	char	t_left[NUMSIZE];
	char	t_right[NUMSIZE];
	char	t_child[NUMSIZE];
	char	t_date[DATESIZE];
	char	t_title[DIRSIZ+1];
};

# define	TREESIZE	NUMSIZE*5+DATESIZE+DIRSIZE+1
					/* Sum of all the field sizes in
					treejunk, != sizeof(treejunk). Size of
					TREEIDX records */

struct treestuff {
	int	msgnum;
	int	parent;
	int	left;
	int	right;
	int	child;
	time_t	date;
	char	title[DIRSIZ+1];
};

typedef	struct treestuf treemem;
typedef struct treejunk treedisk;
typedef struct lines line;
typedef int func();
typedef func *fp;

#ifndef __STRING
typedef char *STRING;
#define __STRING
#endif

ulog a_log;
extern errno;
extern char **environ;
extern time_t time ();
extern long lseek ();
extern long tell ();
long filesize();
extern char *malloc();
extern char *realloc();
extern char *calloc();
extern STRING strcat();
extern STRING strncat();
extern STRING strcpy();
extern STRING strncpy();
extern char *index();
extern char *rindex();
extern int read();
extern int write();
treemem go_left();
treemem go_right();
treemem go_up();
treemem go_down();
treemem go_x();
treemem disk_to_mem();
treedisk mem_to_disk();
/* char *crypt(); 	/* no encryption now 0.3 */
char *ctime();
char *getpass();
int line_lost();
long msg_write(), input_message();

# define	prints(string)	{ printf(string); free(string); }
# define	len(s1)		strlen(s1)
# define	yes(string)	(string[0]=='Y' || string[0]=='y')
# define	no(string)	(string[0]=='N' || string[0]=='n')
# define	instr(s, c)	((_temp = index((s),(c))-(s)) >=0 ? _temp :-2)
# define	msg_quit()	((*(get_command("Delete message?")))=='y')
# define	CTRL(c)		('c'-'a'+1)
# define	readline(buf)	{ if (fgets (buf, BUFSIZ, stdin)==0 && \
					errno!=EINTR) fortune(2,NUPDATE); \
				}
	/* .8 */
# define	FNStop(x)	(\
			(_temp=(x))==CTRL(c) ||  (_temp)==CTRL(k) ?\
			(putchar('\n'), TRUE) : FALSE\
		)
	/* 1.2 */
# define	NOGOOD(name)	(name[0] == '.' || name[0] == '\0'\
		|| name[0] == ' ' || name[0] == '/')

int _temp;

struct sgttyb termp, origp;
struct tchars termc, origc;
fp oldsig[NSIG];

STRING	goof="/bbs/goofmesg";
STRING	bulletin="/bbs/bulletin";
STRING	mhelp="/bbs/help";
STRING	other="/bbs/other";
STRING	sysinfo="/bbs/info";
STRING	ehelp="/bbs/entryhelp";
STRING	notice="/bbs/manycalls";
STRING	complaints="/bbs/mail/sysop";
STRING	errorfile="/bbs/errorlog";
STRING	command="b,c,f,g,h,i,l,m,o,q,r,u,x,y,z,?,a";
STRING	ecommand="c,e,h,l,n,q,s,?";
STRING	tcommand="luqdruq";
STRING	CmdA, CmdB;		/* first word, rest of command */
STRING	currentuser;
char	buffer[BUFSIZ];		/* character buffer for whatever */
char	log_entry[8000];	/* big enough to hold all commands you could
					possibly enter */

/* These defines aren't actually used (except MAINB) */
# define	MAINB		0
# define	WEIRD		1
# define	ODDBALL		2
# define	DandD		3
# define	newboard	4
# define	AMIGAB		5
# define	BOARDS		6
# define	DAT		0
# define	IDX		1
# define	BTYPES		2

# define	USERLOG		0
# define	LOG		1
# define	MAIL		2
# define	TREEIDX		3
# define	OTHERS		4

/* display is fopened, and does not use a file descriptor. Mail is re-opened
	each time it is used, and may not always have the same descriptor.
	Tree index is fopened/fclosed EACH time it is written to or read from.
*/

int files[OTHERS], board[BOARDS][BTYPES], lastmsg[BOARDS], justread[BOARDS];

/* Next arrays must be in the same order as the #define statements */
STRING	bnames [] [BTYPES] = {
	{"/bbs/dat/board0",	"/bbs/idx/board0"},
	{"/bbs/dat/board1",	"/bbs/idx/board1"},
	{"/bbs/dat/board2",	"/bbs/idx/board2"},
	{"/bbs/dat/board3",	"/bbs/idx/board3"},
	{"/bbs/dat/board4",	"/bbs/idx/board4"},
	{"/bbs/dat/board5",	"/bbs/idx/board5"},
};
STRING	onames [] [2] = {	/* [2] to prevent compiler error */
	{"/bbs/userlog",""},
	{"/bbs/logfile",""},	/* log of who called and what they did */
	{"/bbs/idx/tree",""},
	{"",""},		/* mail is reopened each time */
};

int
	beginner=TRUE,
	cboard=MAINB,
	new_caller=FALSE,
	breakcnt=0,
	__;		/* end of globals */

# define	CBOARD		(board[cboard][DAT])
# define	CIDX		(board[cboard][IDX])

int argc;
char **argv;

main (c,v)
char **v;
{
	if (sizeof (ulog)!=64) {
		printf("Userlog size not 64. Current size=%d\n",sizeof(ulog));
		myexit(1);
	}
	umask (0002);
	setgid (STB_GID); setuid (STB_UID);
		/* Don't run as root on V7 systems (and dump core correctly) */
	argc=c; argv=v;		/* keep track of who we are for z command */
	initialize();		/* set rnd # generator, signals, files */
/*	skip_line();		/* wait for call (DELETED) */
	printf("Can't turn off linefeeds, sorry\n");
	printf("Welcome to the serial tree board (C %d.%d).\n", VERSION, FIX);
	do ; while (!get_password());
	log_user();
	read_bulletin();
	if (read_mail()==-1)
		printf("Sorry, your mailbox is empty\n");
	do_commands();
}

hangup()	/* come here if they hang up */
{
	add_log("\n");
	add_log("HANGUP RECEIVED");
	fortune(0,NUPDATE);		/* Die normally */
}

initialize ()
{
	int t,b,f;
	chdir ("/bbs");	/* make sure we can core dump if there is a problem */
	srand ((int) time(0));
	defopen("/etc/default/stb");
	for (b=0; b<BOARDS; b++)
		for (t=0; t<BTYPES; t++)
			board[b][t]=ropen(bnames[b][t],O_RDWR);
	for (f=0; f<OTHERS; f++)
		files[f]=ropen(onames[f][0],O_RDWR);
	(void) signal (SIGALRM, hangup);
	(void) signal (SIGHUP, hangup);
/* set terminal to disable ctrl-c/k, ctrl-x=kill */
	ioctl (0, TIOCGETP, &origp);   /* 0.3 Save the originals */
	ioctl (0, TIOCGETC, &origc);	/* & restore later. */
	ioctl(0,TIOCGETP,&termp);
	termp.sg_kill=CTRL(x);
	ioctl(0,TIOCSETP,&termp);
	ioctl(0,TIOCGETC,&termc);
	termc.t_quitc= -1;
	termc.t_intrc= -1;
	ioctl(0, TIOCSETC, &termc);
/* need to read index files for the boards */
/* Make sure the index files are not empty (if they are, they get lowmsg=1,
address=0) */
	for (b=0; b<BOARDS; b++)
		if (filesize(board[b][IDX]) == 0)
		{	long x;		/* File position */
			x=1; write (board[b][IDX], &x, sizeof x);
		}
}

nodie()
{
	int x;
	for (x=0; x<NSIG; x++)
		oldsig[x] == signal (x, SIG_IGN);
}

yesdie()
{
	int x;
	for (x=0; x<NSIG; x++)
		signal (x, oldsig[x]);
}

ropen (name, flag)		/* open/creat file, whichever is needed */
STRING name;
{
	int fd;
	if (*name == 0) return -1;
	if ((fd=open(name,flag))!=-1)
		return fd;
	if (errno==ENOENT)	/* if it doesn't exist, try creating it */
		if ((fd=creat(name,0664))!=-1)
			return close(fd), ropen(name, flag);
	fprintf(stderr, "stb: error in file %s ",name);
	perror("");
	errno=0;
	return -1;
}

STRING makestring(n)
{
	char *temp;
	temp=calloc((unsigned)n+1,(unsigned)1);
	if (temp==NULL)
		syscrash ("No Memory");
	return temp;
}

STRING stvcat(s1, s2)	/* Variable string allocator/catinator. Adds string 2
				to the end of string 1, but first free's space
				previously used by string 1. */
STRING *s1, s2;
{
	STRING temp;
	temp=makestring(len(*s1)+len(s2));
	strcpy (temp, *s1);
	strcat (temp, s2);
	free(*s1);
	*s1=temp;
	return (*s1);
}

STRING stvadd(s1, s2)	/* Variable string allocator/catinator. Adds string 2
				to the START of string 1, but first free's space
				previously used by string 1. */
STRING *s1, s2;
{
	STRING temp;
	temp=makestring(len(*s1)+len(s2));
	strcpy (temp, s2);
	strcat (temp, *s1);
	free(*s1);
	*s1=temp;
	return (*s1);
}

STRING mkmail(s1)
STRING s1;
{
	return stvadd(&s1,"/bbs/mail/");
}

STRING strsave(s)		/* save a string in memory. Taken from K&R */
STRING s;
{
	STRING p;
	if ((p=malloc((unsigned)strlen(s)+1)) != NULL)
		strcpy (p,s);
	return p;
}

STRING first(s1)     /* s1 is <returned><space><ignored>. Returns same string
			if there is no space, different string if there is */
STRING s1;
{
	STRING s2;
	char *temp;
	temp=index(s1,' ');
	if (temp==NULL)
		return strsave(s1);
	s2=strncpy (makestring( (int)(temp-s1) ),s1,temp-s1);
	s2[temp-s1]='\0';
	return s2;
}

STRING rest(s1)	/* s1 is <ignored><space><returned>. Null string if no space */
STRING s1;
{
	if (index(s1,' ')==0)
		return makestring(0);
	return strsave(index(s1,' ')+1);
}

STRING strleft(s1,n)
STRING s1;
{
	STRING s2;
	s2=strncpy(makestring(n),s1,n);
	s2[n]='\0';		/* strncpy doesn't guarentee null terminator */
	return s2;
}

STRING strright(s1,n)
STRING s1;
{
	if (n>strlen(s1))
		return strsave(s1);
	return strsave(s1+n-strlen(s1));
}

STRING lower_convert(string)	/* converts a string to lowercase */
STRING string;
{
	STRING s2;
	for (s2=string; *string!=0; string++)
		if (isascii(*string) && isupper(*string))
			*string=tolower(*string);
	return s2;	/* return the converted string. */
}

skip_line()		/* skip rest of line (until newline) */
{
	int c;
	while ((c=getchar())!=EOF && c!='\n')
	;
}

STRING getline(s, lim)	/* input a string up to lim-1 characters, NO \n */
STRING s;
{
	int temp;
	if (fgets (s,lim,stdin)==NULL)
		fortune (2,NUPDATE);
	temp=len(s)-1;
	if (s[temp]!='\n')
		skip_line();
	else	s[temp]='\0';		/* remove newline */
	return s;
}

STRING get_command(string)		/* put the command in CmdA, CmdB */
STRING string;
{
	char buffer[100];
	printf(string);
	getline(buffer, 100);
	if (CmdA) {
		free(CmdA); CmdA=NULL;
	}
	if (CmdB) {
		free(CmdB); CmdB=NULL;
	}
	CmdA=first(buffer);
	CmdB=rest(buffer);
	lower_convert(CmdA);
	lower_convert(CmdB);
	add_log(CmdA);
	add_log(" ");
	return CmdA;
}

STRING strsqueeze (s1)		/* remove blanks in string. Note that there
				  will be an extra 0 for each space removed. */
STRING s1;
{
	char *cp;
	for (cp=s1; *cp!=0; cp++)
		if (*cp==' ')
			strcpy(cp,cp+1), --cp;
	return s1;
}

cbreak()		/* .8 */
{
	if (breakcnt++ != 0) return;
	termp.sg_flags |= CBREAK;
	termp.sg_flags &= ~ECHO;
	ioctl (0, TIOCSETP, &termp);
}

cooked()		/* .8 */
{
	if (--breakcnt !=0) return;
	termp.sg_flags &= ~CBREAK;
	termp.sg_flags |= ECHO;
	ioctl (0, TIOCSETP, &termp);
}

input (prompt)		/* 2.1 */
STRING prompt;
{
	int x;
	printf (prompt);
	getline (buffer, BUFSIZ);
	if (sscanf (buffer, "%d", &x) == 1)
		return x;
	else return -1;
}

do_commands()			/* .6 */
{
	for (;;) {	/* Exit is in goodbye() */
		alarm (WAIT_TIME);	/* 10 minutes of inactivity = hangup */
		if (beginner) printf("Command list: %s\n",command);
		get_command("Command? ");
		switch (instr(command, *CmdA)/2) {
		case 0: chboard(); break;
		case 1: chat(); break;
		case 2: feedback(); break;
		case 3: goodbye(); myexit (-1); /* exit should be done from
					goodbye; if error, then return -1 */
		case 4: help(); break;
		case 5: info(); break;
		case 6: leave_message(); break;
		case 7: mail(); break;
		case 8: other_systems(); break;
		case 9: log_quit(); myexit(-1);
					/* logoff w/out updating auto-read */
		case 10: msg_read(); break;
		case 11: userlog(); break;
		case 12: xpert(); break;
		case 13: yagain(); break;
					/* it should myexit w/out hang up */
		case 14: relog(); break;
		case 15: help(); break;
		case 16: attach(CmdB); break;
		default: printf("No such command\n"); break;
		}
	}
}

attach (parent)
STRING parent;
{
	treemem tree;
	if (parent==NULL)
	{
		printf("Please specify a parent\n");
		return;
	}
	not_ready_yet();
}

chboard()
{
	int x;
	printf("Enter board number (0-%d): ",BOARDS-1);
	readline (buffer);
	sscanf(buffer, "%d",&x);
	if (x<BOARDS)			/* .6 invalid board means stay here */
		cboard=x;
	else printf("Invalid board--staying on board %d\n", cboard);
}

chat()
{
	printf("Can't chat. Please leave feedback.\n");
	feedback ();
}

feedback()
{
	printf("Enter feedback now\n");
	close(files[MAIL]);
	files[MAIL]=ropen(complaints,O_WRONLY);
	input_message(files[MAIL], "Complaints ");
	close(files[MAIL]);
}

goodbye()
{
	if (new_caller) make_account();
	fortune(0,UPDATE);
}

help ()
{
	displ_file (mhelp);
}

info ()
{
	displ_file (sysinfo);
}

leave_message ()
{
	long where;
	char header[50];
	sprintf (header, "Message %d ", maxmsg(CIDX) +1 ) ;
	printf("Enter your message now.\n");
	where=input_message(CBOARD, header);

	if (where==-1) return;
	locking (CIDX, LK_RLCK, 0L);
	lseek (CIDX, 0L, 2);
	if (write (CIDX, &where, sizeof (where)) != sizeof (where))
		syscrash ("Can't update message pointers");
	locking (CIDX, LK_UNLK, 0L);
}

line *in_message();

mail ()		/* 3.10 */
{
	STRING s1;
	line *mess;
	printf("Enter mail now\n");
	mess = in_message();
	if (mess==NULL) return;
	for (;;) {
		printf("Who shall it go to? ");
		getline(buffer, BUFSIZ);
		if (*buffer == '\0') break;
		if (NOGOOD(buffer)) {printf("Bad name\n"); continue;}
		if (*buffer != '!')
		{	/* Normal--not rmail */
			lower_convert(strsqueeze(buffer));
			s1=strsave(buffer);
			s1=mkmail(s1);		/* Memory loss */
			files[MAIL]=ropen(s1, O_WRONLY);
		} else { /* uucp mail */
			s1=strsave(&(buffer[0])); /* Memory loss--never freed */
			sprintf (buffer, "rmail %s", s1);
			files[MAIL] = pwrite (buffer);
		}
		msg_write (mess, files[MAIL], "Mail ");
		close (files[MAIL]);
	}
}

pwrite (s)
STRING s;
/* This is not accurate, but mearly sufficient. Don't call more than 10 or
so times, ok? */
{
	FILE *f, *popen();
	f=popen(s, "w");
	if (f==NULL) perror("stb:");
	else return fileno(f);
	return -1;
}

other_systems ()
{
	displ_file (other);
}

log_quit ()
{
	fortune(0,NUPDATE);
}

minmsg(fdboard)		/* 1.1 */
{
	long li;		/* pointers into message base may be >32K */
				/* (Unsigned ints are no better (64 K)) */
	lseek (fdboard, 0L, 0);
	return( read (fdboard, &li, sizeof (li) ) == sizeof li ? li
			: syscrash ("Can't read low message number")
		);
}

maxmsg(fdboard)		/* 1.1 */
{
	return (
	(lseek(fdboard, 0L, 2) / sizeof (long)) -2 + minmsg(fdboard)
	) ;
	/*        (Size of file)     # of messages in file    +lowest message
				== highest message */
	/* -2 corrects for first message being #1 */
	/* (for one item file: 8/4 (=2) -2 (=0) +minmsg (1) = 1 */
}

long wheremsg (fdboard, msgnum)
{
	long li;
	lseek (fdboard, (long) ((msgnum-minmsg(fdboard)+1) * sizeof (long)), 0);
	return( read (fdboard, &li, sizeof li) == sizeof li ? li
			: syscrash ("Can't find message pointer in read")
		);
}

msg_read ()				/* .6 */
		/* Major changes in 1.1:
		A. Index files are now used as arrays, with known seek offsets
		B. Read is now by message number, not byte number. Message 0
		   is NOT valid, but instead holds the lowest message number.
		*/
{
	long start, current;
	printf("Starting message number (%d-%d, %d suggested)? ",
			minmsg(CIDX),	maxmsg(CIDX), justread[cboard]);
	readline(buffer);
	start=0;
	if (sscanf(buffer, "%D", &start)==0) start=0;
	if (start==0) start=justread[cboard];
	if (start <minmsg(CIDX) || start > maxmsg(CIDX))
	{	printf("No such message\n"); return;
	}
	lseek (CBOARD, wheremsg(CIDX, start), 0);
	cbreak();
	current = start;
	do
	{	justread[cboard]=current++; /* assign old value, then inc */
	} while (!empty(CBOARD) && displ_message(CBOARD));
	cooked();
}

userlog ()				/* .7 */
{
	ulog log;
	lseek(files[USERLOG], 0L, 0);
	printf("Name          Calls    Last called\n");
	cbreak();
	while (read (files[USERLOG], &log, sizeof log)!=NULL
				&& !FNStop(finkey(stdin)))
		printf ("%-14s %4d %s", log.name, log.numcalls,
							ctime(&log.lastcall));
	cooked();
}

xpert ()
{
	beginner= !beginner;
}

yagain()
{
	if (new_caller) make_account();
	fortune(2, UPDATE); myexit (-1);
}

relog ()
{
	int temp=0;
	temp = ( strcmp(CmdA,"zs")==0 ? 1: 0);
	if (new_caller) make_account();
	if (temp==1)
		fortune(2,UPDATE);	/* Return to where we came from */
	else fortune(1,UPDATE);	/* DON'T hang up the phone in either case */
}

not_ready_yet ()
{
	printf("Not finished yet. Please try next week\n");
}

syscrash(s1)
STRING s1;
{
	int x;
	perror("");
	printf("Fatal error (%s) has occured. System crashing\n", s1);
	fflush(stdout);
	freopen(errorfile,"a", stderr); /* use freopen to make sure that
			 		a file descriptor is available */
	fprintf(stderr, "%s %d", s1, errno);
	x=time(0);
	fputs(" ",stderr);
	fputs(ctime(&x),stderr);
	fflush(stderr);
	fclose (stderr);
	fclose(fopen("/bbs/core", "w")); /* Must be empty for core to dump */
	restoretty();
	abort();
}

long filesize(file)		/* Wasn't used til 1.1. Hmm...*/
{
	return lseek (file, 0L, 2);
}

int display(fd)		/* 3.0 */
{
	char c;
	int x, count;
	setbuf (stdout, buffer);
	while (read(fd, &c, 1)==1 && c!='\001')
	{	putchar(c);
		if (count++ >= OUTBUF)
		{	fflush(stdout);
			count = 0;
			if FNStop(x=finkey(stdin))
				break;
		}
	}
	fflush(stdout); setbuf (stdout, NULL);
	return x;
}

displ_file (file)
STRING file;			/* 3.0 */
{
	int fd;
	int c;
	fd = open(file, O_RDONLY);
	if (fd == -1) return -1;	/* no file, then return */
	cbreak();		/* .7 */
	display(fd);		/* 3.0 */
	cooked();		/* .7 */
	close(fd);
	return 0;
}

displ_message (file)		/* display's the file until a chr$(1) or EOF */
				/* .6 Returns false if ctrl_c, true otherwise */
				/* .7 Runs in cbreak */
				/* .8 Now accepts ctrl-c/k at ? prompt */
				/* 1.1 Puts a blank line after return hit */
				/* 1.2 Puts 2 blanks */
				/* 3.0 one blank before, one after */
{
	char c;
	int x;
	cbreak();
	x=display (file);
	cooked();
	if (x==CTRL(c)) return FALSE;
	if (x==CTRL(k)) { skip_message(file); return TRUE; } /* .7 */
	if (empty(file)) return TRUE;
	cbreak();
	putchar('\n');
	putchar('?');
	alarm (WAIT_TIME);		/* hangup detection */
	x=getchar();			/* .8 */
		if (x==EOF)
			fortune(2,NUPDATE);
	cooked();			/* No need to worry about exit throu
					fortune; that clears cbreak */
	puts("\n\n");
	return (x == CTRL(c) ? FALSE:TRUE);
}

skip_message(file)		/* move to next ctrl-a */
{
	char c;
	while (read(file, &c, 1)==1 && c!=CTRL(a))
	;
}

get_password()
{
	char *pass;
loop:
	printf("What is your name (or handle)? ");
	getline(buffer, BUFSIZ);
	strcpy(currentuser=makestring(BUFSIZ), lower_convert(buffer));
	if (NOGOOD(currentuser))
	{	printf ("Bad name, try again. \n");
		goto loop;
		/* NOTREACHED */
	}
	if (!get_log(currentuser))
	{	printf("Are you a new user? ");		/* 1.1 */
		getline (buffer, BUFSIZ);
		if (*buffer != 'y' && *buffer != 'Y') goto loop;
		new_caller=TRUE;
		return TRUE;
	};
	pass=lower_convert(getpass("What is your password?"));
	if (strcmp(a_log.password, pass)==0)
	{
		int x;
		char *temp;
		printf((temp=defread("GREET="))
			? temp
			: "Welcome back. Last call was %s\nTotal calls: %d\n",
		  ctime(&a_log.lastcall),  a_log.numcalls);
							
		for (x=0; x<BOARDS; x++)
			justread[x]=a_log.lastread[x];
		return TRUE;
	}
	else return FALSE;
}

read_bulletin()
{
	displ_file (bulletin);
}
;

read_mail()			/* .6 version */
{
	int file;
	STRING s1;
	file=open(s1=strsave(mkmail(strsqueeze(strsave(currentuser))))
								,O_RDONLY);
	if (file==-1) return -1;
	while (!empty(file) && displ_message (file))
	;			/* displ_message returns false if ctrl_c */
	close (file);
	get_command("Delete this? ");
	if (*CmdA=='y')
		unlink (s1);
	return 0;
}

log_user()
{
	long x;
	if (add_log(currentuser)==-1)
		syscrash("Can't log you in, sorry");
	add_log(" ");
	x=time(0);
	if (add_log(ctime(&x))==-1)
		syscrash("Can't log you in, sorry");
}

add_log(s1)
STRING s1;
{
	sprintf(log_entry,"%s%s", log_entry, s1);
	return 0;
}

get_log(name)		/* get the log entry for name into a_log */
char *name;		/* returns 0 for not found, non-zero otherwise */
{
	int x;
	char *temp;
	lseek (files[USERLOG], 0L, 0);
	temp=strleft(strsqueeze(strsave(name)),DIRSIZ);
	while ((x=read (files[USERLOG], &a_log, sizeof a_log)) == sizeof a_log)
		if (strcmp(a_log.name, temp)==0)
			break;
	if (!(x==sizeof a_log || x==0))
		syscrash("Error in userlog");
	return x;
}

he_left(arg)		/* update calls, last call date, last message read */
{
	int x;
	lseek (files[LOG], 0L, 2);
	nodie();
	write(files[LOG], log_entry, strlen(log_entry));
	write(files[LOG],"\n",1);
	yesdie();
	if (new_caller) return;
/* search for record (assume it exists) */
	get_log(currentuser);
/* update it (always do time, the rest only if he didn't quit or hang up)*/
	if (arg==UPDATE)
	{
		for (x=0; x<BOARDS; x++)
			a_log.lastread[x]=justread[x];
		a_log.numcalls++;
	}
	a_log.lastcall=time(0);
	lseek(files[USERLOG],(long) -sizeof a_log, 1);
	nodie();
	write (files[USERLOG], &a_log, sizeof a_log);
	yesdie();
}

zeromem (addr, bytes)
char *addr;
{
	while (bytes--) *addr++ = ' ';
}

make_account()		/* Called when a new user logs off */
{
	get_command("Will you be coming back? ");
	if (*lower_convert(CmdA)=='y')
	{
		STRING pass1, pass2;
		zeromem (&a_log, sizeof a_log);
		strcpy(a_log.name,strleft(strsqueeze(currentuser),DIRSIZ));
		do
		{
			pass1=strsave(lower_convert(getpass(
					"Please enter your password: ")));
			pass2=strsave(lower_convert(getpass("Retype it: ")));
		} while (strcmp(pass1,pass2)!=0);
		if (*pass1 == '\0') return; /* if no password then return */
		strcpy(a_log.password, pass1 );
		a_log.numcalls=0;
		a_log.lastbyte='\n';
		lseek (files[USERLOG], 0L, 2);
		if (write(files[USERLOG], &a_log, sizeof a_log)!=sizeof a_log)
			syscrash ("Write error on userlog--userlog clobbered");
		new_caller=FALSE;
	}
}

fortune (arg, update)
		/* print a fortune, hang up phone, and reset system for next
		caller. If arg=1, phone is NOT hung up, but the program is re-
		executed (assumed to be in either /bin or /usr/bin) If arg=2,
		eof is assumed (myexit code 0) */
{	int temp=0;
	alarm (0);
	signal (SIGHUP, SIG_IGN);
	temp=fork();
	if (temp==0)
	/* in child */
	{	execl("/usr/games/fortune","fortune",NULL);
		printf("No cookies available--sorry\n");
		kill(getpid(), SIGEMT);	/* any unused signal that core dumps */
	}
	/* in parent */
	he_left(update);
	if (arg==1) {
		int x;
		for (x=3; x<20; x++)		/* keep terminal channels */
			close(x);		/* open, close the rest */
		wait (0);		/* let the fortune print out */
		execlp ("bbs","bbs",0);
		syscrash ("YOW! Can't find myself!");
	}
	if (arg==2)
	{
/* SYS 3 SYS3 SYS3 SYS3 SYS3 SYS 3 */
/* The following is to prevent hanging up after the last close, which is
impossible under V7 (where once set, always set) 
/* */
#ifdef SYS3
#include <termio.h>
#include <sys/ioctl.h>
		struct termio t;
		ioctl (0, TCGETA, &t);
		t.c_cflag &= ~HUPCL;	/* Don't hang up */
		ioctl (0, TCSETA, &t);
#endif
/* End sys 3 */
		wait (0);		/* let the fortune print out */
		myexit (0);
	}
/*	termp.sg_flags ^= ECHO;		/* no echo */
/*	stty(1,&termp);
/*	wait (0);			/* let the fortune print out */
/*	printf("\r");
/*	sleep (3);			/* wait a bit */
/*	printf("+++");			/* hang up phone */
/*	sleep (3);
/*	printf("ath\r");
/*	termp.sg_flags ^= ECHO;		/* turn echo on for local use */
/*	sleep (1);			/* just to be safe */
/*	stty(1,&termp);
/* Lines deleted since the system now hangs up properly. */
	wait(0);
	myexit (0);			/* exit program */
}

myexit(arg)		/* Restore terminal and exit */
{
	restoretty();
	exit (arg);
}

restoretty()
{
	ioctl (0, TIOCSETP, &origp);
	ioctl (0, TIOCSETC, &origc);
}

empty(file)	/* Checks to see if a file has at least 2 characters (ctrl-a
						and newline usually) */
{
	char c;
	if (read (file,&c,2)==2) {		/* if a character was read, */
		lseek (file, -2L, 1);		/* go back to it */
		return FALSE;
	}
	return TRUE;			/* nothing left in file */
}

line *findline (message, l)		/* 2.1 */
line *message;
{
	while (message != NULL && (--l) > 0)
		message = message->next;
	return message;
}

line *cont (message)
line *message;
{
	line *buffer, *last;
/* find end of message */
	for (last=message; last!=NULL && last->next!=NULL; last=last->next)
	;
	printf("Input text now\n");
	do {
		alarm (WAIT_TIME);		/* hangup detection */
		if ((buffer=(line *)calloc((unsigned)1,
						(unsigned)sizeof(line)))==NULL)
			syscrash("No memory");
		printf("%s",CmdB);
		/* Do not use getline (EOF) */
		fgets (buffer->text, LINESIZE, stdin);
			/* if EOF then break loop */
		if (*buffer->text=='\n' || *buffer->text == '\0') break;
		else
		/* adjust pointers */
		if (last==NULL) message=buffer;
		else last->next=buffer;
		last=buffer;
		buffer->next=NULL;
	} while (*buffer->text!='\n');	/* continue until blank line */
	return message;
}

line *edit (message)	/* 2.1 */
line *message;
{
	int x;
	line *edline;
	x = input ("Line number to change? ");
	edline=findline(message, x); if (edline == NULL) return message;
	printf ("Old line:\n");
	printf (edline->text);
	printf ("Enter new line\n");
	getline (buffer, BUFSIZ);	/* 2.4 */
	if (*buffer == '\0' || *buffer == '\n')
		return message;
	else sprintf (edline->text, "%s\n", buffer);	/* 2.7 */
	return message;
}

list_message (message)		/* 2.1 now stops on ctrl-c/k */
line *message;
{
	line *cline;
	int line_no=1;
	for (cline=message; cline!=NULL && !FNStop (finkey(stdin));
							cline=cline->next)
		printf ("%d: %s", line_no++, cline->text);
}

line *delete (message)
line *message;
{
	return NULL;	/* temp. version; doesn't actually delete anything */
}

line *new (message)
line *message;
{
	if (msg_quit()) {
	message=delete (message);
	return cont(message);
	}
	return message;
}

long msg_write (message,file,header)
			/* Actual header is header+"from xx date yyy" */
line *message;
STRING header;
{
	time_t tim;
	long retval;
	nodie();
	locking (file, LK_RLCK, 0L);	/* don't let anyone else write; wait if
						someone is writting */
	retval=lseek (file, 0L, 2);		/* EOF */
	tim=time(0);
	sprintf (buffer, "%sfrom %s date %s", header, currentuser,
			ctime(&tim));
	write (file, buffer, len(buffer));
	for (; message != NULL; message=message->next)
		if (write (file, message->text, len(message->text))
				!=len(message->text))
			syscrash("write error in message entry");
	delete(message);	/* recover memory used by message */
	write (file, "\001", 1);	/* ctrl-a terminates each message */
	locking (file, LK_UNLK, 0L);	/* let others write to it */
	yesdie();
	return retval;
}

line *in_message ()		/* Input a message at the end of file. */
				/* Message terminated by ctrl-a */
{
	line	*message=NULL;
			/* message points to the first line in the message */
	message=cont(message);
	for (;;) {
		if (beginner) printf("Command list: %s\n",ecommand);
		get_command("Message entry command? ");
		switch (instr(ecommand, *CmdA)/2) {
		case 0: message=cont(message); break;
		case 1: message=edit(message); break;
		case 2: displ_file(ehelp); break;
		case 3: list_message(message); break;
		case 4: message=new(message); break;
		case 5: if (msg_quit()) return NULL; else break;
		case 6: return message;
		case 7: displ_file(ehelp); break;
		default: printf("No such command\n"); break;
		}
	}
}

long input_message (file, header)	/* 3.10 */
STRING header;
{
	line *message = in_message ();
	if (message == 0) return -1;
	return msg_write (message, file, header);
}

/* Tree routines follow. Routines that return a treestuff structure (treemem)
go_left(treemem)
go_right(treemem)
go_up(treemem)
go_down(treemem)
go_x(msgnum)
disk_to_mem(treedisk)
make_down(treemem)	(makes a child and returns its treemem)

All the above change the current directory to whatever the message is.
Other routines: returns STRING
message_name(treemem)

List of children is obtained by system(ls), or childfile=popen("ls","r")

mem_to_disk(treedisk) returns a treejunk structure.

Remember, treejunk is used for TREEIDX file, treestuff is used for internal
computation. TREEIDX is NOT memory resident; it is read a record at a time
when needed. Note: The message number in treejunk/treestuff MUST be the record
number in TREEIDX; if messages are deleted, they must be renumbered.
Future improvment number one: Get around this restriction.
*/
-- 
: Michael Gersten		seismo!scgvaxd!stb!michael
: Copy protection? Just say Pirate! (if its worth pirating)


