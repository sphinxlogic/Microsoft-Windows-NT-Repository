 9-Dec-85 13:43:37-MST,2349;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Mon 9 Dec 85 13:43:29-MST
Received: from usenet by TGR.BRL.ARPA id a005932; 9 Dec 85 12:56 EST
From: Kevin Szabo <ksbszabo@watvlsi.uucp>
Newsgroups: net.sources
Subject: asroot: a program that gives you root permissions for a single command
Message-ID: <2821@watvlsi.UUCP>
Date: 6 Dec 85 17:25:55 GMT
To:       unix-sources@BRL-TGR.ARPA

Some time ago there was discussion about the single user
systems and the problems of running as ROOT most of the
time.  In a single user system you can easily become root
(via SU) but it is dangerous to always run as ROOT since you can
clobber yourself.  Unfortunately SU tends to be quite slow for
those times when you only want to execute a single command.

Thus I have written a small command called 'asroot' which will
momentarily give a process root permissions.  The use of the
command is:

	$ asroot cmd arg1 arg2 ...

or you can type

	$ asroot

and 'asroot' will prompt you for command line.

I use 'asroot' frequently on my systemIII machine; it
should run correctly on other version of UNIX as well.

PLEASE NOTE THAT THIS PROGRAM IS A MASSIVE SECURITY HOLE.
YOU SHOULD NOT ALLOW IT TO EXIST ON A MULTI-USER SYSTEM,
OR ONE WHICH MAY BE ACCESSED REMOTELY.

So here's the program.  It's too small for a shar file...


#include	<stdio.h>

/*
 *	Asroot	- execute a command with root permissions.
 *		  compile with 'cc -o asroot asroot.c'
 *		  then 'chown root asroot; chmod u+s asroot'.
 *
 *	This program is a convenience for single-user systems,
 *	BUT it is a MASSIVE security hole.  Please use caution.
 */

main( argc, argv )
	int	argc;
	char	**argv;
{
	extern	char	*gets();
	int		retcode;

	char	string[260];

	setuid( geteuid() );

	if ( argc > 1 ) {
		execvp( argv[1], &argv[1] );
		fprintf( stderr,"%s: execution of '%s' failed: ", argv[0], argv[1] );
		perror( "" );
		exit( 1 );
	} else {
		printf("Enter string to be executed: ");
		fflush( stdout );
		retcode=system(  gets( string ) );
		if ( retcode != 0 )
			fprintf(stderr,"%s: the command '%s' exited with status %d\n", argv[0], string, retcode );
		exit( retcode );
	}
}
-- 
Kevin Szabo' watmath!watvlsi!ksbszabo (U of W VLSI Group, Waterloo, Ont, Canada)
