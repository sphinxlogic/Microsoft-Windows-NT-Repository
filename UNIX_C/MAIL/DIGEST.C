/*
 * digest - create digests of mail messages
 *
 * This program uses the file "digest.info" to figure out what issue
 * of the digest it is making.  The format of this file is:
 *
 *	Name of the List		# leave out the word "digest"
 *	Host				# the host where the digest lives
 *	From				# who sends the digest out
 *	To				# who the list is sent to
 *	Volume				# Volume XX : Issue XXX
 *	Date				# Day, dd Mon yy hh:mm:ss ZZZ
 *
 * As an example:
 *
 *	Foobar
 *	intrepid.ecn.purdue.edu
 *	Dave Curry (The Moderator) <Foobar-Digest@intrepid.ecn.purdue.edu>
 *	Foobar-List@intrepid.ecn.purdue.edu
 *	Volume 1 : Issue 0
 *	Mon,  4 Jan 88 20:15:33 EST
 *
 * Make sure the "From" line includes a legitimate RFC 822 mail address.
 * Make sure the issue number starts at zero; it gets incremented BEFORE
 * generating each digest.  Volume numbers must be incremented by hand.
 * The "digest.info" file gets modified by the program after generation
 * of each digest.
 *
 * The contents of the file "digest.head", if it exists, will be placed
 * between the list of today's topics and the top of the digest.  This
 * can be used to put information about where to FTP archives from, etc.
 *
 * The file "digest.input" should contain a set of mail messages in the
 * format of a UNIX mailbox.  These messages will be read into memory,
 * and a list of "Today's Topics" generated from the subject lines.  The
 * messages will then be sorted so that all the messages on the same topic
 * come out together in the digest.  Any message whost first word in the
 * subject line is "Administrivia" will be guaranteed to come out first
 * in the digest.
 *
 * The digest will be left in the file "digest.output".  You can send it
 * using the command "/usr/lib/sendmail -t < digest.output".
 *
 * I suggest creating the following mail aliases in /usr/lib/aliases:
 *
 *	1. Foobar-Digest:/path/to/the/digest.input/file
 *		This file must be world-writable for sendmail to modify it.
 *		This is the address to publish for people to send digest
 *		submissions to.
 *	2. Foobar-Digest-Request:yourlogin
 *		This is the address for people to use to ask to be added
 *		or deleted from the list.
 *	3. Foobar-List: :include:/path/to/list/of/recipients
 *		This is the list of people who receive the digest.  It should
 *		be a list of addresses of the format:
 *
 *			name, name, name, name,
 *				name, name, name
 *
 *		Continuation lines should start with whitespace.
 *
 * There is one problem with the sorting of messages by subject line to get
 * all the same topic together.  The code handles elimination of "Re:"
 * strings, but if someone changes the subject on you, then things get ugly.
 * This shouldn't happen too often, though.
 *
 * Special thanks to Jon Solomon who sent me his TELECOM digest generating
 * program.  I swiped a lot of ideas from it in writing this one.
 *
 * David A. Curry
 * davy@intrepid.ecn.purdue.edu
 */
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>

#define HEAD1		27		/* Field width of first third	*/
#define HEAD2		20		/* Field width of second third	*/
#define HEAD3		21		/* Field width of last third	*/
#define DATELEN		14		/* Amount of date to put in hdr	*/
#define LINELEN		70		/* Length of an average line	*/
#define MAXMSGS		64		/* Maximum number of msgs/digest*/
#define LINESIZE	256		/* Maximum line size		*/
#define LISTINFO	"digest.info"	/* Information file name	*/
#define LISTHEAD	"digest.head"	/* Header for top of digest	*/
#define LISTINPUT	"digest.input"	/* Input file name		*/
#define LISTOUTPUT	"digest.output"	/* Output file name		*/

/*
 * Message structure.  We read through the input file and fill one of
 * these in for each message.  The To, Cc, From, Date, and Subject
 * point to the fields of the same names from the message.  The
 * "sortstring" is a copy of the subject string with all whitespace
 * deleted and all letters in lower case.  The messageaddr is the
 * seek position in the file where the message body starts, and
 * messagelength is how long the message is.
 */
struct message {
	char *To;
	char *Cc;
	char *From;
	char *Date;
	char *Subject;
	char *sortstring;
	long messageaddr;
	long messagelength;
} messages[MAXMSGS];

/*
 * List structure.  Contains the information from the LISTINFO file.
 */
struct listinfo {
	char *Title;
	char *Host;
	char *From;
	char *To;
	char *Volline;
	char *Dateline;
} listinfo;

FILE *input;
FILE *output;

int issue_number;			/* The number of this issue	*/
int nmessages = 0;			/* Number of messages		*/
int digestsize = 0;			/* Size of digest in bytes	*/

char *index(), *malloc(), *safter(), *nospace(), *getline();

main()
{
	/*
	 * Read the list information file and update the
	 * issue number and date strings.
	 */
	get_list_info();
	inc_volume_and_date();

	printf("Assembling %s Digest %s (%.*s)\n", listinfo.Title, listinfo.Volline, DATELEN, listinfo.Dateline);
	printf("Scanning and sorting messages for topic lines.\n");

	/*
	 * Scan the message file for subject strings and
	 * sort the messages to get all the messages for
	 * each topic next to each other.
	 */
	scan_messages();
	sort_messages();

	printf("Writing %s Digest to \"%s\"\n", listinfo.Title, LISTOUTPUT);

	/*
	 * Print the digest header, put the messages
	 * in the digest.
	 */
	do_digest_header();
	read_messages();

	printf("The digest is %d characters long in %d messages.\n", digestsize, nmessages);

	/*
	 * Put out the new list information.
	 */
	put_list_info();
}

/*
 * get_list_info - reads in the LISTINFO file.
 */
get_list_info()
{
	FILE *fp;
	int incomplete;

	if ((fp = fopen(LISTINFO, "r")) == NULL) {
		printf("digest: cannot open \"%s\" for reading.\n", LISTINFO);
		exit(1);
	}

	incomplete = 0;

	if ((listinfo.Title = getline(fp)) == NULL)
		incomplete++;
	if ((listinfo.Host = getline(fp)) == NULL)
		incomplete++;
	if ((listinfo.From = getline(fp)) == NULL)
		incomplete++;
	if ((listinfo.To = getline(fp)) == NULL)
		incomplete++;
	if ((listinfo.Volline = getline(fp)) == NULL)
		incomplete++;
	if ((listinfo.Dateline = getline(fp)) == NULL)
		incomplete++;

	fclose(fp);

	/*
	 * Error-check.  Not too sophisicated, but then you're
	 * supposed to know what you're doing anyway.
	 */
	if (incomplete) {
		printf("digest: incomplete or badly formatted \"%s\" file.\n", LISTINFO);
		printf("Proper format:\n");
		printf("\tTitle\n\tHost\n\tFrom\n\tTo\n\tVolline\n\tDateline\n");
		exit(1);
	}
}

/*
 * inc_volume_and_date - update the volume/issue string and get a new date.
 */
inc_volume_and_date()
{
	char *msgdate();
	register char *volline, *colon;

	if ((volline = malloc(strlen(listinfo.Volline)+1)) == NULL) {
		printf("digest: out of memory.\n");
		exit(1);
	}

	/*
	 * Volume numbers get changed by hand.
	 */
	issue_number = atoi(safter(listinfo.Volline, " Issue ")) + 1;

	if ((colon = index(listinfo.Volline, ':')) != NULL)
		*colon = NULL;

	sprintf(volline, "%s: Issue %3d", listinfo.Volline, issue_number);
	strcpy(listinfo.Volline, volline);

	/*
	 * Get a new date.
	 */
	listinfo.Dateline = msgdate();

	free(volline);
}

/*
 * msgdate - produce a new date string.  Format is
 *
 *		Day, dd Mon yy hh:mm:ss tzn
 */
char *msgdate()
{
	char *timezone();
	struct timeb tbuf;
	register struct tm *t;
	struct tm *localtime();
	static char datebuf[64];
	char *days = "SunMonTueWedThuFriSat";
	char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";

	ftime(&tbuf);
	t = localtime(&(tbuf.time));

	sprintf(datebuf, "%3.3s, %2d %3.3s %02d %02d:%02d:%02d %3.3s",
			&days[3 * t->tm_wday], t->tm_mday,
			&months[3 * t->tm_mon],	t->tm_year, t->tm_hour,
			t->tm_min, t->tm_sec, timezone(tbuf.timezone, t->tm_isdst));

	return(datebuf);
}

/*
 * getline - read a line into a dynamically allocated buffer.
 */
char *getline(fp)
FILE *fp;
{
	register int c;
	register char *str, *str_begin;

	if ((str = malloc(LINESIZE)) == NULL) {
		printf("digest: out of memory.\n");
		exit(1);
	}

	str_begin = str;

	while (((str - str_begin) < (LINESIZE - 1)) &&
	       ((c = getc(fp)) != '\n') && (c != EOF))
		*str++ = c;
	*str++ = NULL;

	if (c == EOF)
		return(NULL);

	return(str_begin);
}

/*
 * scan_messages - scans through LISTINPUT reading in header fields
 *		   and marking the beginning and ending of messages.
 *
 * NOTE: some of the code here depends on the UNIX mail header format.
 *       This format simply guarantees that the first line of a message's
 *	 header will be "From blah-blah-blah".  Note there is no colon
 *	 (`:') on the "From", the real "From:" line is farther down in
 *	 the headers.
 */
scan_messages()
{
	register long n;
	register char *s;

	if ((input = fopen(LISTINPUT, "r")) == NULL) {
		printf("digest: cannot open \"%s\" for reading.\n", LISTINPUT);
		exit(1);
	}

	/*
	 * We break out of this from inside.
	 */
	for (;;) {
		if (nmessages >= MAXMSGS) {
			printf("digest: too many messages.\n");
			exit(1);
		}

		/*
		 * Find the start of the next message.
		 */
		do {
			/*
			 * If we hit EOF, mark the length of the
			 * previous message and go back.
			 */
			if ((s = getline(input)) == NULL) {
				n = ftell(input);
				n = n - messages[nmessages - 1].messageaddr;
				messages[nmessages - 1].messagelength = n;
				return;
			}
		} while (strncmp(s, "From ", 5) != 0);

		/*
		 * If we have found another message, mark the
		 * length of the previous message.
		 */
		if (nmessages) {
			n = ftell(input);
			n = n - (strlen(s) + 1);
			n = n - messages[nmessages - 1].messageaddr;
			messages[nmessages - 1].messagelength = n;
		}

		/*
		 * Read in the headers.
		 */
		for (;;) {
			/*
			 * We shouldn't hit EOF here, we should
			 * at least finish the headers first.
			 */
			if ((s = getline(input)) == NULL) {
				printf("digest: \"%s\": unexpected EOF.\n", LISTINPUT);
				exit(1);
			}

			/*
			 * Blank line terminates headers.
			 */
			if (*s == NULL)
				break;

			/*
			 * Save certain headers.  We strip the
			 * header name and leading whitespace.
			 */
			if (strncmp(s, "To:", 3) == 0) {
				messages[nmessages].To = nospace(safter(s, "To:"));
			}
			else if (strncmp(s, "Cc:", 3) == 0) {
				messages[nmessages].Cc = nospace(safter(s, "Cc:"));
			}
			else if (strncmp(s, "From:", 5) == 0) {
				messages[nmessages].From = nospace(safter(s, "From:"));
			}
			else if (strncmp(s, "Date:", 5) == 0) {
				messages[nmessages].Date = nospace(safter(s, "Date:"));
			}
			else if (strncmp(s, "Subject:", 8) == 0) {
				s = nospace(safter(s, "Subject:"));

				/*
				 * We don't need the "Re:" stuff.
				 */
				if ((strncmp(s, "re:", 3) == 0) || (strncmp(s, "Re:", 3) == 0) ||
				    (strncmp(s, "RE:", 3) == 0) || (strncmp(s, "rE:", 3) == 0))
				    	s += 3;

				messages[nmessages].Subject = nospace(s);
			}
			else {
				/*
				 * If we aren't saving this line,
				 * give the memory back.
				 */
				free(s);
			}
		}

		/*
		 * The message starts after the header.
		 */
		messages[nmessages].messageaddr = ftell(input);
		nmessages++;
	}
}

/*
 * sort_messages - convert each message's subject line to a string
 *		   all in lower case with no whitespace.  Then sort
 *		   the messages on this string.  This will group
 *		   all the messages on the same subject together.
 */
sort_messages()
{
	register int i;
	extern int comp();
	register char *s, *t;

	for (i=0; i < nmessages; i++) {
		/*
		 * Skip messages with no subject.
		 */
		if (messages[i].Subject == NULL)
			continue;

		s = messages[i].Subject;

		if ((t = malloc(strlen(s)+1)) == NULL) {
			printf("digest: out of memory.\n");
			exit(1);
		}

		messages[i].sortstring = t;

		/*
		 * Zap leading whitespace.
		 */
		s = nospace(s);

		/*
		 * Copy the subject string into sortstring
		 * converting upper case to lower case and
		 * ignoring whitespace.
		 */
		while (*s) {
			if ((*s == ' ') || (*s == '\t')) {
				s++;
				continue;
			}

			if (isupper(*s))
				*t++ = tolower(*s);
			else
				*t++ = *s;

			s++;
		}

		*t = NULL;
	}

	/*
	 * Sort 'em.
	 */
	qsort(messages, nmessages, sizeof(struct message), comp);
}

/*
 * comp - comparison routine for qsort.  Meassges with no subject go
 *	  at the end of the digest, messages with "administrivia" as
 *	  the subject go to the top of the digest.
 */
comp(m1, m2)
register struct message *m1, *m2;
{
	int admin1, admin2;

	if (m1->sortstring == NULL) {
		if (m2->sortstring == NULL)
			return(0);
		return(1);		/* no subject messages to end */
	}

	if (m2->sortstring == NULL)
		return(-1);		/* no subject messages to end */

	admin1 = strncmp(m1->sortstring, "administrivia", 13);
	admin2 = strncmp(m2->sortstring, "administrivia", 13);

	if (admin1 == 0) {
		if (admin2 == 0)
			return(0);
		return(-1);		/* administrivia to beginning */
	}

	if (admin2 == 0)
		return(1);		/* administrivia to beginning */

	return(strcmp(m1->sortstring, m2->sortstring));
}

/*
 * do_digest_header - prints the digest header and mailer headers.
 */
do_digest_header()
{
	FILE *fp;
	char *laststr;
	char buf[BUFSIZ];
	char tmp[LINESIZE];
	extern int comp2();
	register int i, j, length;

	if ((output = fopen(LISTOUTPUT, "w")) == NULL) {
		printf("digest: cannot create \"%s\"\n", LISTOUTPUT);
		exit(1);
	}

	digestsize = 0;

	/*
	 * Mailer headers.
	 */
	sprintf(buf, "Date: %s\n", listinfo.Dateline);
	digestsize += strlen(buf);
	fputs(buf, output);

	sprintf(buf, "From: %s\n", listinfo.From);
	digestsize += strlen(buf);
	fputs(buf, output);

	sprintf(buf, "Reply-To: %s@%s\n", listinfo.Title, listinfo.Host);
	digestsize += strlen(buf);
	fputs(buf, output);

	sprintf(buf, "Subject: %s Digest V1 #%d\n", listinfo.Title, issue_number);
	digestsize += strlen(buf);
	fputs(buf, output);

	sprintf(buf, "To: %s\n", listinfo.To);
	digestsize += strlen(buf);
	fputs(buf, output);

	/*
	 * The digest header.
	 */
	sprintf(tmp, "%s Digest", listinfo.Title);
	sprintf(buf, "\n\n%-*.*s %-*.*s %-*.*s\n\n",
				HEAD1, HEAD1, tmp,
				HEAD2, DATELEN, listinfo.Dateline,
				HEAD3, HEAD3, listinfo.Volline);
	digestsize += strlen(buf);
	fputs(buf, output);

	sprintf(buf, "Today's Topics:\n");
	digestsize += strlen(buf);
	fputs(buf, output);

	/*
	 * Do today's topics lines.
	 */
	laststr = "";
	for (i=0; i < nmessages; i++) {
		/*
		 * No topic.
		 */
		if (messages[i].Subject == NULL)
			continue;

		laststr = messages[i].sortstring;

		/*
		 * Count the number of messages with this topic.
		 */
		j = 1;
		while (((i + j) < nmessages) && (strcmp(laststr, messages[i+j].sortstring) == 0))
			j++;

		/*
		 * Print the topic centered on the line.
		 */
		if (j > 1) {
			sprintf(tmp, "%s (%d msgs)", messages[i].Subject, j);
			length = (LINELEN / 2) + (strlen(tmp) / 2);
			sprintf(buf, "%*s\n", length, tmp);

			/*
			 * Sort messages with same topic into their
			 * original arrival order.
			 */
			qsort(&messages[i], j, sizeof(struct message), comp2);
			i += (j - 1);
		}
		else {
			length = (LINELEN / 2) + (strlen(messages[i].Subject) / 2);
			sprintf(buf, "%*s\n", length, messages[i].Subject);
		}

		digestsize += strlen(buf);
		fputs(buf, output);
	}

	/*
	 * Read the LISTHEAD file, if there is one.
	 */
	if ((fp = fopen(LISTHEAD, "r")) != NULL) {
		fputc('\n', output);
		digestsize++;

		while (fgets(buf, BUFSIZ, fp) != NULL) {
			digestsize += strlen(buf);
			fputs(buf, output);
		}

		fclose(fp);
	}

	/*
	 * Print a line of dashes.
	 */
	for (i=0; i < LINELEN; i++) {
		putc('-', output);
		digestsize++;
	}

	fputs("\n\n", output);
	digestsize += 2;
}

/*
 * comp2 - comparison routine for second qsort.  This one simply compares
 *	   messages addresses in the input file, so that we can sort the
 *	   messages with the same topic back into the order they arrived.
 */
comp2(m1, m2)
register struct message *m1, *m2;
{
	return(m1->messageaddr - m2->messageaddr);
}

/*
 * read_messages - reads in the message texts and puts them in the
 *		   digest with their headers.
 */
read_messages()
{
	char buf[BUFSIZ];
	register char *s, *t;
	register int i, length;

	for (i=0; i < nmessages; i++) {
		/*
		 * Just in case.
		 */
		clearerr(input);

		/*
		 * Put the message's headers back in.
		 */
		sprintf(buf, "Date: %s\n", messages[i].Date);
		digestsize += strlen(buf);
		fputs(buf, output);

		sprintf(buf, "From: %s\n", messages[i].From);
		digestsize += strlen(buf);
		fputs(buf, output);

		if (messages[i].Subject != NULL) {
			sprintf(buf, "Subject: %s\n", messages[i].Subject);
			digestsize += strlen(buf);
			fputs(buf, output);
		}

		if (messages[i].To != NULL) {
			sprintf(buf, "To: %s\n\n", messages[i].To);
			digestsize += strlen(buf);
			fputs(buf, output);
		}

		/*
		 * Read the message into memory.  This is
		 * so we can zap extra blank lines.
		 */
		fseek(input, messages[i].messageaddr, 0);
		length = messages[i].messagelength;

		if ((s = malloc(length+1)) == NULL) {
			printf("digest: out of memory.\n");
			exit(1);
		}

		fread(s, 1, length, input);

		/*
		 * Zap trailing newlines.
		 */
		t = s + length;
		while (*--t == '\n')
			length--;
		*++t = NULL;
		
		/*
		 * Zap leading newlines.
		 */
		t = s;
		while (*t++ == '\n')
			length--;
		t--;

		/*
		 * Write the message.
		 */
		digestsize += length;
		fwrite(t, 1, length, output);

		sprintf(buf, "\n\n------------------------------\n\n");
		digestsize += strlen(buf);
		fputs(buf, output);
		free(s);
	}

	/*
	 * All done.
	 */
	sprintf(buf, "End of %s Digest\n******************************\n", listinfo.Title);
	digestsize += strlen(buf);
	fputs(buf, output);
	fclose(output);
	fclose(input);
}

/*
 * put_list_info - rewrite the LISTINFO file with the new data.
 */
put_list_info()
{
	FILE *fp;
	char tmp[LINESIZE];

	sprintf(tmp, "%s.old", LISTINFO);

	if (rename(LISTINFO, tmp) < 0) {
		printf("digest: cannot move old \"%s\" file, today's data lost.\n", LISTINFO);
		return;
	}

	if ((fp = fopen(LISTINFO, "w")) == NULL) {
		printf("digest: cannot create \"%s\", today's data lost.\n", LISTINFO);
		return;
	}

	fprintf(fp, "%s\n", listinfo.Title);
	fprintf(fp, "%s\n", listinfo.Host);
	fprintf(fp, "%s\n", listinfo.From);
	fprintf(fp, "%s\n", listinfo.To);
	fprintf(fp, "%s\n", listinfo.Volline);
	fprintf(fp, "%s\n", listinfo.Dateline);

	fclose(fp);
	unlink(tmp);
}

/*
 * safter - return a pointer to the position in str which follows pat.
 */
char *safter(str, pat)
register char *str, *pat;
{
	register int len;

	len = strlen(pat);

	while (*str) {
		if (strncmp(str, pat, len) == 0) {
			str += len;
			return(str);
		}

		str++;
	}

	return(NULL);
}

/*
 * nospace - advance s over leading whitespace, return new value.
 */
char *nospace(s)
register char *s;
{
	while ((*s != NULL) && ((*s == ' ') || (*s == '\t')))
		s++;

	return(s);
}

