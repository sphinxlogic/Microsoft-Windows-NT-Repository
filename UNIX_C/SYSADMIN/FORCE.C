12-Dec-85 03:47:55-MST,2569;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Thu 12 Dec 85 03:47:49-MST
Received: from usenet by TGR.BRL.ARPA id a029847; 12 Dec 85 3:40 EST
From: Paul Summers <paul@wjvax.uucp>
Newsgroups: net.sources
Subject: Re: force.c
Message-ID: <628@wjvax.wjvax.UUCP>
Date: 10 Dec 85 18:51:53 GMT
To:       unix-sources@BRL-TGR.ARPA

With the rash of un-secure programs that turn the average user into
root without the courtesy of using su,  I felt that I should post a 
program that we have been using fairly successfully here at wjvax.
The main difference between this program and 'asroot' and its spiritual
bretheren is that it keeps a copy of the encrypted root password,
and prompts for it before letting the casual terminal snatcher get
away with murder or worse.

I make no guarantees about portability (we're running bsd 4.2) or
security.  The main point that I am stressing is the password.  A little time
is sacrificed to make sure that only super user privilidged people can use
this program.
(I know of a particular system that has 'chown' set userid root...)

The main idea is to save time. 
Have any of you tried the '-f' option on su?

---------------------cut here-------------------------------------
/*
 *	force.c:  execute $* as user root.
 *
 *	A relatively secure program that executes its arguments
 *	as the super user.  A small speed sacrifice is made to prompt
 *	for a password.  Install the program with mode 4750, owner
 *	root, group root (or operator).
 *
 *	Written by:	Paul M. Summers		(wjvax!paul)
 *			10/85
 *
 *	Compile: cc -o /usr/local/bin/force force.c
 *		 chmod 4750 /usr/local/bin/force
 */
#include	<pwd.h>
/*
 *	Modify the next 2 lines as appropriate.
 */
#define SA	"System Administrator's name"
#define	ROOTPW	"Encrypted root password from /etc/passwd"

main(argc,argv)
int argc;
char *argv[];
{
char *pwd,*cpwd,*crypt(),*getpass(),salt[2];
struct passwd *getpwuid(),*pwdent;

pwd = getpass("Password: ");

strncpy(salt,ROOTPW,2);
cpwd = crypt(pwd,salt);

if (strcmp(ROOTPW,cpwd) == 0) {
	setuid(0);
/*	nice(-5);	/* Overdrive... */
	execvp(argv[1],&argv[1]);
	printf("%s: command not found\n",argv[1]);
	exit(1);
}

/*
 *	Check for changed root password.
 */

setpwent();
pwdent = getpwuid(0);

if (strcmp(pwdent->pw_passwd,ROOTPW) != 0) {
	printf("Root password has changed to %s\n",pwdent->pw_passwd);
	printf("Notify %s that force must be changed\n",SA);
}
else
	printf("Bad password.\n");
}
