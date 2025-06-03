/* ta=4 */
/****************************************************************************
*						p s f m a i l . c									*
*																			*
*	postscript mail filter													*
*																			*
*	very simple mail filter to print name and subject in bold letters		*
*	for psf usage.  Generates escape sequences that psf can understand.		*
*	Modify the code if you wish to have various headers ignored for print.	*
*																			*
*	Tony Field.       tony@ajfcal.cuc.ab.ca									*
****************************************************************************/
/*
 * $Id: psfmail.c,v 3.4 1992/02/24 06:48:59 ajf Exp ajf $
 *
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "patchlevel.h"
#include "psf.h"

#define MAX_C		62			/*	max char wrap point on mail header line	*/

char	*pgmname;

FILE	*outfp;

main (argc, argv)
int		argc;
char	*argv[];
{
	int		ignore_garbage, i;
	extern char *optarg;
	extern int	optind;
	FILE	*input_fp;

	outfp = stdout;
	pgmname = argv[0];	
	ignore_garbage = 1;
	if (argc > 1  &&  strcmp (argv[1], "-") == 0)
		usage();
	while ((i = getopt(argc, argv, "s-")) != -1)
	{	switch (i)
		{
		case 's':
			ignore_garbage = 0;		/*	print all headers	*/
			break;

		default:
			usage ();
		}
	}
	if (optind >= argc)
	{	input_fp = stdin;
		print_file (input_fp, ignore_garbage);
	}
	else
	{
		for ( i = 0;  optind < argc;  optind++)
		{	if ((input_fp = fopen (argv[optind], "r")) == NULL)
			{	fprintf (stderr, "%s: File %s not found\n", pgmname, argv[optind]);
			}
			else
			{	if (i)
					send ("\f");
				print_file (input_fp, ignore_garbage);
				fclose (input_fp);
				i++;
			}
		}
	}
	exit (0);
}

print_file (fp, ignore_garbage)
FILE *fp;
int	 ignore_garbage;
{
	char	line[900], first[100], tail[800], *strchr();
	int		last_char, header, garbage;

	header = garbage = 0;
	while (fgets (line, 999, fp)  != NULL)
	{
newmail:
		last_char = split (line, first, tail);

		/*	The following headers will be ignored during printing	*/
		
		if (ignore_garbage 
			&&  (strcmp (first, "Distribution:"  ) == 0
			||   strcmp (first, "Keywords:"      ) == 0
			||   strcmp (first, "Lines:"         ) == 0
			||   strcmp (first, "Message-ID:"    ) == 0
			||   strcmp (first, "Message-Id:"    ) == 0
			||   strcmp (first, "News-Path:"     ) == 0
			||   strcmp (first, "Path:"          ) == 0
			||   strcmp (first, "Posted:"        ) == 0
			||   strcmp (first, "Received:"      ) == 0
			||   strcmp (first, "References:"    ) == 0
			||   strcmp (first, "Sender:"        ) == 0
			||   strcmp (first, "Status:"        ) == 0
			||   strncmp(first, "X-",          2 ) == 0))
		{	garbage = 1;
		}
		else if (strcmp (first, "From") == 0)
		{	printclean (first, tail);
			garbage = 0;
		}
		else if (strcmp (first, "From:") == 0
				||  strcmp (first, "Reply-To:") == 0
				||  strcmp (first, "To:") == 0)
		{	header = 1;
			if ((strchr (tail, '(') == NULL)  &&  (strchr (tail, '<') == NULL))
				printbold (first, tail);
			else
				printname (first, tail);
			garbage = 0;
		}
		else if (strcmp (first, "Bcc:") == 0 ||  strcmp (first, "Cc:") == 0)
		{	header = 1;
			printname (first, tail);
			garbage = 0;
		}
		else if (strcmp (first, "Subject:") == 0)
		{	printsubject (first, tail);
			garbage = 0;
		}
		else if (last_char == ':')
		{
			header = 1;
			printclean (first, tail);
			garbage = 0;
		}
		else if (empty (line))
		{	
			send ("\n");
			while (fgets (line, 999, fp) != NULL)
			{	if (strncmp (line, "From ", 5) == 0
					&&  (strchr (line, ':') < strrchr (line, ':')))
				{	send ("\f");
					goto newmail;	/* goto's considered harmful since 1964 */
				}
				printf ("%s", line);
			}
			break;
		}
		else if (garbage == 0)
			printclean (" ", line);
	}
}

printclean (first, tail)
char	*first, *tail;
{
	printf ("%-14s", first);
	printlong (tail);
}

printlong (tail)
char	*tail;
{	char	*c;
	int		marks[100], nmarks, i, j, nchar;

	/*	locate all marks that can be use for a line break 	*/

	marks[0] = 0;
	marks[1] = 0;
	c = tail;
	for ( i = nchar = 0, nmarks = 1;  nmarks < 100;  i++, c++, nchar++)
	{
		if (*c == '!'  ||  *c == ' ' ||  *c == '<'  
			||  *c == '('  ||  *c == '\0')
		{	if (nchar < MAX_C)
				marks[nmarks] = i;
			else
			{	nchar = i - marks[nmarks];
				marks[++nmarks] = i;
			}
			if (*c == '\0')
				break;
		}
	}
	marks[nmarks] = i;
	for (i = 0;  i < nmarks;  i++)
	{
		if (i)
			printf ("%-14s", " ");
		for (j = marks[i];  j < marks[i+1];  j++)
			fputc (tail[j], outfp);
		fputc ('\n', outfp);
	}
}

printsubject (first, tail)
char	*first, *tail;
{
	printf ("%-14s", first);
	send (BEGIN_SUBJECT);
	send (tail);
	send (END_SUBJECT);
	send ("\n");
}

printbold (first, tail)
char	*first, *tail;
{
	printf ("%-14s", first);
	send (BEGIN_BOLD);
	printlong (tail);
	send (END_BOLD);	
}

printname (first, tail)
char	*first, *tail;
{
	printf ("%-14s", first);

	if (strchr (tail, '<') != NULL)
	{	/*	address syntax "name <address> stuff" */
		send (BEGIN_NAME);
		while (*tail != '<')
			fputc (*tail++, outfp);
		send (END_NAME);
		fputc (*tail++, outfp);
		while (*tail)
		{	fputc (*tail, outfp);
			if (*tail++ == '>')
			{	if (*tail)
				{	send (BEGIN_NAME);
					while (*tail)
						fputc (*tail++, outfp);
					send (END_NAME);
				}
			}
		}
	}
	else
	{	/*	address syntax "address (name)" */
		while (*tail  &&  *tail != '(')
			fputc (*tail++, outfp);
		if (*tail)
		{	send (BEGIN_NAME);
			fputc (*tail++, outfp);
			while (*tail)
			{	fputc (*tail, outfp);
				if (*tail++ == ')')
				{	send (END_NAME);
					while (*tail)
						fputc (*tail++, outfp);
					fputc ('\n', outfp);
					return;
				}
			}
		}
	}
	fputc ('\n', outfp);
}

send (s)
char	*s;
{
	while (*s)
		fputc (*s++, outfp);
}

empty (s)
char	*s;
{	while (*s  &&  *s <= ' ')
		s++;
	if (*s)
		return (0);
	else
		return (1);
}

split (line, first, tail)
char	*line;			/*	input:	full input line			*/
char	*first;			/*	return:	first word of line		*/
char	*tail;			/*	return:	all others words of line*/
{	int	last;

	last = 0;
	while (*line  &&  isspace (*line) == 0)
	{	last = *line;
		*first++ = *line++;
	}
	*first = '\0';
	
	while (*line  &&  isspace (*line))
		line++;
	
	while (*line  &&  *line != '\n')
		*tail++ = *line++;
	*tail = '\0';
	*line = '\0';
	return (last);
}

usage ()
{
	fprintf (stderr, "Usage:   psfmail [-s] files... >out.file\n");
	fprintf (stderr, " where            -s  = show all header lines\n");
	fprintf (stderr, "                files = input files (or stdin)\n");
	exit (0);	
}
