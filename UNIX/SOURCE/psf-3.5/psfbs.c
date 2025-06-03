/* ta=4 */
/****************************************************************************
*						p s f b s . c      									*
*																			*
*	postscript filter for nroff'ed text										*
*																			*
*	Translate backspaces in nroff documents to either BOLD or UNDERLINE		*
*	for psf usage															*
*																			*
*	Tony Field.       tony@ajfcal.cuc.ab.ca									*
****************************************************************************/
/*
 * $Id: psfbs.c,v 3.4 1992/02/24 06:48:59 ajf Exp ajf $
 *
 */

/*	For each line in of input text, scan for backspaces.  Determine if
	the operation is an underline (i.e. the preceeding character is
	the "_") or if it is a bolding (the character after the underscore
	is the same as the preceeding.
	
	Generate the sequence \005B..text..\005b   for bold
						  \005U..text..\005u   for underline
						  \005I..text..\005i   for italics

	Since the text from nroff does not have a decent way of identifying
	italics, you may make the decision to generate the "italic" or 
	"underline" escape sequence  whenever an underline is detected in
	the output.  Italic fonts look nicer than the underlines 2-up.

	The logic cannot handle bold-underlined (nor italic-underlined).
	This could be done with a bit of extra logic to manipulate bits
	in the "how" vector.

	For nroff, remove three blank lines between pages (an nroff'ed
		document, letter size, has 66 lines.  the postscript
		printer must see 63 lines.)   Assume that nroff is consistent.
		If "point size" adjustments are made (e.g. some documents
		originally intended for troff), then lines per page may not
		be consistent.
		
*/

#include <stdio.h>
#include <string.h>
#include "psf.h"
#include "patchlevel.h"

char	*pgmname;
FILE	*outfp;

main (argc, argv)
int		argc;
char	*argv[];
{	int		i, c;
	int		nroff_lines = 66;
	int		remove = 3;
	int		count_lines = 1;
	char	*underline_on, *underline_off;
	extern char *optarg;
	extern int	optind, getopt();
	FILE	*input_fp;

	outfp = stdout;
	pgmname = argv[0];
	underline_on  = BEGIN_UNDERLINE;
	underline_off = END_UNDERLINE;

	if (argc > 1  &&  strcmp (argv[1], "-") == 0)
		usage();
	while ((c = getopt(argc, argv, "ifr:l:-")) != -1)
	{	switch (c)
		{
		case 'i':
			underline_on  = BEGIN_ITALICS;
			underline_off = END_ITALICS;
			break;
		
		case 'l':
			nroff_lines = atoi (optarg);
			break;
			
		case 'r':
			remove = atoi (optarg);
			break;
			
		case 'f':
			count_lines = 0;
			break;
			
		default:
			usage ();
		}
	}
	
	if (optind >= argc)
	{	input_fp = stdin;
		print_file (input_fp, nroff_lines, remove, underline_on, underline_off, count_lines);
	}
	else
	{
		for (i = 0 ;  optind < argc;  optind++)
		{	if ((input_fp = fopen (argv[optind], "r")) == NULL)
			{	fprintf (stderr, "%s: File %s not found\n", pgmname, argv[optind]);
			}
			else
			{	if (i)
					send ("\f");
				print_file (input_fp, nroff_lines, remove, underline_on, underline_off, count_lines);
				fclose (input_fp);
				i++;
			}
		}
	}
	exit (0);
		
}

print_file (fp, nroff_lines, remove, underline_on, underline_off, count_lines)
FILE	*fp;
int		nroff_lines;
int		remove;
int		count_lines;
char	*underline_on, *underline_off;
{
	int		c, n, i;
	int		how[401];
	char	line[401];
	int		nroff_count;

	nroff_count = n = 0;
	
	clear (line, how, 400);
	while (( c = fgetc (fp)) != EOF)
	{
		if (c == '\033')			/* if ^[ (real ESCAPE character), then handle (sort of) */
		{	if ((c = fgetc (fp)) == EOF)
				break;
			if (c == '9')		/*	1/2 line space fwd				*/
			{	fputc (ESCAPE, outfp);
				fputc ('+', outfp);
				nroff_count += 5;
				continue;
			}
			else if (c == '8')		/*	1/2 line space back			*/
			{	fputc (ESCAPE, outfp);
				fputc ('-', outfp);
				nroff_count -= 5;
				continue;
			}
			else
			{	/*	don't know what to do with it */
				continue;
			}
		}
			
		else if (c == '\b')
			n--;
		else if (c == '\n')
		{	if (count_lines)
			{	/*	remove 3 blank lines between pages
					1 from beginning of page,
					2 from end of page.
				*/
				nroff_count = (nroff_count + 10) % (nroff_lines * 10);
				if (nroff_count < 20  ||  nroff_count > (nroff_lines - (remove-1)) * 10)
					continue;
			}
			how[n] = 0;
			line[n] = 0;
			for (i = 0;  i <= n;  i++)
			{
				if (how[i])
				{	/*	either bold or underlined see if previous char
						is not escaped - indicates the beginning of 
						an escape sequence.  may need to terminate
						previous sequence first (e.g. bold followed
						by underlined).
					*/
					if (i > 0  &&  how[i-1]  &&  (how[i] != how[i-1]))
					{	if (how[i-1] == 1)
							send (END_BOLD);
						else
							send (underline_off);
					}
					if (how[i] == 1  &&  (i == 0  ||  how[i-1] != 1))
						send (BEGIN_BOLD);
					else if (how[i] == 2  &&  (i == 0  ||  how[i-1] != 2))
						send (underline_on);
				}
				else	/* zero means just a character, unmodified */
				{	/* are we at the end of an escape sequence? */
					if (i > 0)
					{	if (how[i-1] == 1)
							send (END_BOLD);
						else if (how[i-1] == 2)
							send (underline_off);
					}
				}
				if (line[i])
					fputc (line[i], outfp);
			}
			fputc ('\n', outfp);	
			clear (line, how, n);
			n = 0;
		}
		else
		{	if (line[n])
			{	if (line[n] == c)		/* same character?		*/
					how[n] = 1;			/*	yes:	bold		*/
				else
					how[n] = 2;			/*	no:		underline	*/
			}
			if (line[n] == 0 || c != '_')
				line[n] = c;
			n++;
		}
	}
}

clear (line, how, n)
char	*line;
int		*how;
int		n;
{
	int		i;
	
	for (i  = 0;  i <= n;  i++)
	{	line[i] = '\0';
		how[i] = 0;
	}
}

send (s)
char	*s;
{
	while (*s)
		fputc (*s++, outfp);
}

usage ()
{
	fprintf (stderr, "Usage:   psfbs [-i] [ -f ] [ -l n ] [ -r n ] file ... >out.file\n");
	fprintf (stderr, " where          -i   = use italics in lieu of underline\n");
	fprintf (stderr, "                -f   = document has form feeds, ignore line count\n");
	fprintf (stderr, "                -l n = make n line nroff'ed documents fit\n");
	fprintf (stderr, "                -r n = number of blank lines to remove\n");
	fprintf (stderr, "                file = input files (or stdin)\n");
	exit (0);	
}
