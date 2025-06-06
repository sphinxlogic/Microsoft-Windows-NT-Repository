10-Dec-85 13:09:35-MST,4267;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Tue 10 Dec 85 13:09:24-MST
Received: from usenet by TGR.BRL.ARPA id a003897; 10 Dec 85 14:04 EST
From: Brandon Allbery <allbery@ncoast.uucp>
Newsgroups: net.unix-wizards,net.sources
Subject: csh vs. # comments in bourne shell scripts
Message-ID: <918@ncoast.UUCP>
Date: 10 Dec 85 03:08:18 GMT
Followup-To: net.unix-wizards
Xref: seismo net.unix-wizards:16109 net.sources:4009
To:       unix-sources@BRL-TGR.ARPA

Expires:

Quoted from <392@brl-tgr.ARPA> ["Re: Magic Numbers"], by gwyn@brl-tgr.ARPA (Doug Gwyn <gwyn>)...
+---------------
| > Csh on non-4.2 systems checks for a # as
| > the first character of the file, and forks itself if it sees it; if not, it
| > forks a /bin/sh.
| 
| Yes, some of them do, but if they do that's a bug.
| Virtually all of my Bourne shell scripts (including
| SVR2 system utility scripts) start with "#".
+---------------

All of them do.  When csh was written bsh still only spoke :.

There *is* a simple (albeit slow) solution.  Write a program to intuit the
program to invoke on the file, with the file's full pathname passed as argv[1]
and its arguments as argv[2] .. argv[argc - 1].  Then say:

	alias shell $HOME/bin/bexec

or whatever you decide to call it.  IT MUST BE A BINARY FILE AND IT MUST BE A
FULL PATHNAME.  If csh can't exec() a program it will prepend the value of the
``shell'' alias to it, if any, and try again; if there isn't a shell alias it
uses its own heuristics.

The program below uses this (mis-?)feature of bastard (orphaned?) csh'es to
handle #! magic numbers, sans set[ug]id and other fun stuff.  Other than
that it uses the same test as csh itself.  But you can do whatever you like,
say to use * to cause your local Kermit-running MS-DOS machine to receive and
execute the file as an MS-DOS batch file. [ :-) ]  Let me know if you come
up with any interesting new twists.

--Brandon
----------------------------- cut here ---------------------------------------
#include <stdio.h>
#include <fcntl.h>

/*
 * bexec: execute a non-binary file for csh, with #! recognition
 * (usage: alias shell /usr/local/bin/bexec)
 */

extern int errno;
extern char *sys_errlist[];

#define SH		"/bin/sh"
#define CSH		"/usr/plx/csh"

#define NARG		512	/* this is the number csh uses */

char *sysargv[NARG + 1];
char **newargv();
char *basename();
extern char *strrchr();

main(argc, argv, envp)
char **argv, **envp; {
	FILE *cmd;
	char kcmd[512];
	char *cp, *ap;
	
	if ((cmd = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "bexec: can't open %s: %s\n", argv[1], sys_errlist[errno]);
		exit(1);
	}
	fgets(kcmd, sizeof kcmd, cmd);
	kcmd[strlen(kcmd) - 1] = '\0';	/* kill trailing \n */
	if (strncmp(kcmd, "#!", 2) != 0) {
		if (kcmd[0] == '#') {
			argv[0] = "csh";
			cp = CSH;
		}
		else {
			argv[0] = "sh";
			cp = SH;
		}
		execve(cp, argv, envp);
		fprintf(stderr, "bexec: can't exec %s: %s\n", cp, sys_errlist[errno]);
		exit(100);
	}
	for (cp = kcmd + 2; *cp == ' '; cp++)
		;
	for (ap = cp; *ap != ' ' && *ap != '\0'; ap++)
		;
	if (*ap == ' ')
		*ap++ = '\0';
	argv = newargv(basename(argv[1]), ap, argv);
	fclose(stdin);
	fcntl(fileno(cmd), F_DUPFD, 0);
	lseek(0, ftell(cmd), 0);
	fclose(cmd);
	execve(cp, argv, envp);
	fprintf(stderr, "bexec: can't exec %s: %s\n", cp, sys_errlist[errno]);
	exit(100);
}

char **newargv(newar0, newar1, oldargv)
char *newar0, *newar1, **oldargv; {
	int ap, op;
	
	sysargv[0] = newar0;
	ap = 1;
	if (newar1[0] != '\0') {
		sysargv[1] = newar1;
		ap++;
	}
	for (op = 2; ap < NARG && oldargv[op] != NULL; ap++, op++)
		sysargv[ap] = oldargv[op];
	sysargv[ap] = NULL;
	return sysargv;
}

char *basename(path)
char *path; {
	char *cp;

	if ((cp = strrchr(path, '/')) == NULL)
		return path;
	return cp + 1;
}

-- 

			Lord Charteris (thurb)

ncoast!allbery@Case.CSNet (ncoast!allbery%Case.CSNet@CSNet-Relay.ARPA)
..decvax!cwruecmp!ncoast!allbery (..ncoast!tdi2!root for business)
6615 Center St., Mentor, OH 44060 (I moved) --Phone: +01 216 974 9210
CIS 74106,1032 -- MCI MAIL BALLBERY (WARNING: I am only a part-time denizen...)
