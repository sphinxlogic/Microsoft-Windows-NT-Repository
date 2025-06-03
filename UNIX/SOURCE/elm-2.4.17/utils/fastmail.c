
static char rcsid[] = "@(#)$Id: fastmail.c,v 5.4 1992/11/22 01:26:12 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.4 $   $State: Exp $
 *
 * 			Copyright (c) 1988-1992 USENET Community Trust
 * 			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: fastmail.c,v $
 * Revision 5.4  1992/11/22  01:26:12  syd
 * The fastmail utility appears to work incorrectly when multiple addresses are
 * supplied. Spaces were inserted between addresses rather than commas.
 * From: little@carina.hks.com (Jim Littlefield)
 *
 * Revision 5.3  1992/10/30  21:12:40  syd
 * Make patchlevel a text string to allow local additions to the variable
 * From: syd via a request from Dave Wolfe
 *
 * Revision 5.2  1992/10/11  00:59:39  syd
 * Fix some compiler warnings that I receive compiling Elm on my SVR4
 * machine.
 * From: Tom Moore <tmoore@fievel.DaytonOH.NCR.COM>
 *
 * Revision 5.1  1992/10/04  00:46:45  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** This program is specifically written for group mailing lists and
    such batch type mail processing.  It does NOT use aliases at all,
    it does NOT read the /etc/password file to find the From: name
    of the user and does NOT expand any addresses.  It is meant 
    purely as a front-end for either /bin/mail or /usr/lib/sendmail
    (according to what is available on the current system).

         **** This program should be used with CAUTION *****

**/

/** The calling sequence for this program is:

	fastmail {args}  [ filename | - ] full-email-address 

   where args could be any (or all) of;

	   -b bcc-list		(Blind carbon copies to)
	   -c cc-list		(carbon copies to)
	   -d			(debug on)
	   -f from 		(from name)
	   -F from-addr		(the actual address to be put in the From: line)
	   -r reply-to-address 	(Reply-To:)
	   -s subject 		(subject of message)
**/

#include "elmutil.h"
#include "patchlevel.h"

#define  binrmail	"/bin/rmail"
#define  temphome	"/tmp/fastmail."

char *get_arpa_date();
static void usage();


main(argc, argv)
int argc;
char *argv[];
{

	extern char *optarg;
	extern int optind;
	FILE *tempfile;
	char hostname[NLEN], username[NLEN], from_string[SLEN], subject[SLEN];
	char filename[SLEN], tempfilename[SLEN], command_buffer[256];
	char replyto[SLEN], cc_list[SLEN], bcc_list[SLEN], to_list[SLEN];
	char from_addr[SLEN];
	char *tmplogname;
	int  c, sendmail_available, debug = 0;

	from_string[0] = '\0';
	subject[0] = '\0';
	replyto[0] = '\0';
	cc_list[0] = '\0';
	bcc_list[0] = '\0';
	to_list[0] = '\0';
	from_addr[0] = '\0';

	while ((c = getopt(argc, argv, "b:c:df:F:r:s:")) != EOF) {
	  switch (c) {
	    case 'b' : strcpy(bcc_list, optarg);		break;
	    case 'c' : strcpy(cc_list, optarg);		break;
	    case 'd' : debug++;					break;	
	    case 'f' : strcpy(from_string, optarg);	break;
	    case 'F' : strcpy(from_addr, optarg);		break;
	    case 'r' : strcpy(replyto, optarg);		break;
	    case 's' : strcpy(subject, optarg);		break;
	    case '?' : usage();
 	  }
	}	

	if (optind >= argc) {
	  usage();
	}

	strcpy(filename, argv[optind++]);

	if (optind >= argc) {
	  usage();
	}

#ifdef HOSTCOMPILED
	strncpy(hostname, HOSTNAME, sizeof(hostname));
#else
	gethostname(hostname, sizeof(hostname));
#endif

	tmplogname = getlogin();
	if (tmplogname != NULL)
	  strcpy(username, tmplogname);
	else
	  username[0] = '\0';

	if (strlen(username) == 0)
	  cuserid(username);

	if (strcmp(filename, "-")) {
	  if (access(filename, READ_ACCESS) == -1) {
	    fprintf(stderr, "Error: can't find file %s!\n", filename);
	    exit(1);
	  }
	}

	sprintf(tempfilename, "%s%d", temphome, getpid());

	if ((tempfile = fopen(tempfilename, "w")) == NULL) {
	  fprintf(stderr, "Couldn't open temp file %s\n", tempfilename);
	  exit(1);
	}

	/** Subject must appear even if "null" and must be first
	    at top of headers for mail because the
	    pure System V.3 mailer, in its infinite wisdom, now
	    assumes that anything the user sends is part of the 
	    message body unless either:
		1. the "-s" flag is used (although it doesn't seem
		   to be supported on all implementations?)
		2. the first line is "Subject:".  If so, then it'll
		   read until a blank line and assume all are meant
		   to be headers.
	    So the gory solution here is to move the Subject: line
	    up to the top.  I assume it won't break anyone elses program
	    or anything anyway (besides, RFC-822 specifies that the *order*
	    of headers is irrelevant).  Gahhhhh....

	    If we have been configured for a smart mailer then we don't want
	    to add a from line.  If the user has specified one then we have
	    to honor their wishes.  If they've just given a 'from name' then
	    we'll just put in the username and hope the mailer can add the
	    correct domain in.
	**/
	fprintf(tempfile, "Subject: %s\n", subject);

	if (*from_string)
	  if (*from_addr)
	      fprintf(tempfile, "From: %s (%s)\n", from_addr, from_string);
	  else
#ifdef DONT_ADD_FROM
	      fprintf(tempfile, "From: %s (%s)\n", username, from_string);
#else
	      fprintf(tempfile, "From: %s!%s (%s)\n", hostname, username, 
		      from_string);
#endif
	else
	  if (*from_addr)
	    fprintf(tempfile, "From: %s\n", from_addr);
#ifndef DONT_ADD_FROM
	  else
	    fprintf(tempfile, "From: %s!%s\n", hostname, username);
#endif

	fprintf(tempfile, "Date: %s\n", get_arpa_date());

	if (strlen(replyto) > 0)
	  fprintf(tempfile, "Reply-To: %s\n", replyto);

	while (optind < argc) 
          sprintf(to_list, "%s%s%s", to_list, (strlen(to_list) > 0? ",":""), 
		  argv[optind++]);
	
	fprintf(tempfile, "To: %s\n", to_list);

	if (strlen(cc_list) > 0)
	  fprintf(tempfile, "Cc: %s\n", cc_list);

#ifndef NO_XHEADER
	fprintf(tempfile, "X-Mailer: fastmail [version %s PL%s]\n",
	  VERSION, PATCHLEVEL);
#endif /* !NO_XHEADER */
	fprintf(tempfile, "\n");

	fclose(tempfile);

	/** now we'll cat both files to /bin/rmail or sendmail... **/

	sendmail_available = (access(sendmail, EXECUTE_ACCESS) != -1);

	if (debug)
		printf("Mailing to %s%s%s%s%s [via %s]\n", to_list,
			(strlen(cc_list) > 0 ? " ":""), cc_list,
			(strlen(bcc_list) > 0 ? " ":""), bcc_list,
			sendmail_available? "sendmail" : "rmail");

	sprintf(command_buffer, "cat %s %s | %s %s %s %s", 
		tempfilename, filename, 
	        sendmail_available? sendmail : mailer,
		to_list, cc_list, bcc_list);

	if (debug)
	  printf("%s\n", command_buffer);

	c = system(command_buffer);

	unlink(tempfilename);

	exit(c != 0);
}

static void usage()
{
	fprintf(stderr,"Usage: fastmail {args} [ filename | - ] address(es)\n");
	fprintf(stderr, "   where {args} can be;\n");
	fprintf(stderr,"\t-b bcc-list\n\t-c cc-list\n\t-d\n");
	fprintf(stderr,"\t-f from-name\n\t-F from-addr\n");
	fprintf(stderr, "\t-r reply-to\n\t-s subject\n\n");
	exit(1);
}
