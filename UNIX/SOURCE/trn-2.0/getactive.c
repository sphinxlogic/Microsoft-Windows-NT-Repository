/* $Id: getactive.c,v 1.2 92/01/11 16:04:23 usenet Exp $
 *
 * $Log:	getactive.c,v $
 * Revision 1.2  92/01/11  16:04:23  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4  1991/09/09  20:18:23  sob
 * release 4.4
 *
 *
 * 
 *
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */
#include <stdio.h>
#include "config.h"
#include <signal.h>
/* what to do with ansi prototypes -- '()' == ignore, 'x' == use */
#ifndef ANSI
#   define ANSI(x) ()
#endif
#include "INTERN.h"
#ifdef SERVER
#include "server.h"
#endif

main(argc, argv)
	int		argc;
	char	 	*argv[];
{
	char		ser_line[NNTP_STRLEN];
	char		command[32];
	int		response;
	char 		*action;
	register char	*server;
	register FILE	*actfp;

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: getactive [active|distributions|newsgroups] filename\n");
		exit(1);
	}
	if (argc == 2) action = "ACTIVE";
	else{
		 action = argv[1];
		 argc--;
		 argv++;
	}
	if ((server = get_server_name(1)) == NULL)
		exit(1);
	response = server_init(server);
	if (response < 0) {
		fprintf(stderr,
			"getactive: Can't get %s file from server %s.\n",
				action, server);
		exit(1);
	}

	if (handle_server_response(response, server) < 0)
		exit(1);

	sprintf(command,"LIST %s",action);
	put_server(command); 
#ifdef HAVESIGHOLD
 	sighold(SIGINT);
#endif
	(void) get_server(ser_line, sizeof(ser_line));
	if (*ser_line != CHAR_OK) {		/* and then see if that's ok */
		fprintf(stderr,
			"getactive: Can't get %s file from server.\n",action);
		fprintf(stderr, "Server said: %s\n", ser_line);
		exit(1);
	}

	actfp = fopen(argv[1], "w");		/* and get ready */
	if (actfp == NULL) {
		close_server();
		perror(argv[1]);
		exit(1);
	}

	while (get_server(ser_line, sizeof(ser_line)) >= 0) {  /* while */
		if (ser_line[0] == '.')		/* there's another line */
			break;			/* get it and write it to */
		if (actfp != NULL) {		/* the temporary active file */
			fputs(ser_line, actfp);
			putc('\n', actfp);
		}
	}

	if (ferror(actfp)) {
	     perror(argv[1]);
	     exit(1);
	}

	if (fclose(actfp) == EOF) {
	     perror(argv[1]);
	     exit(1);
	}

#ifdef HAVESIGHOLD

	exit(0);
	sigrelse(SIGINT);
#endif
	close_server();
	exit(0);
}
