/* ta=4 */

/****************************************************************************
*				p s f d o u b  . c											*
*																			*
*	Filter a "single sided" document to become a "double sided document"	*
*																			*
*	Tony Field.       tony@ajfcal.cuc.ab.ca									*
*																			*
*	The basic algorithm for "print_book()"  was based on a routine written	*
*	by Tom Neff (tneff@well.uucp) named "book" which printed 5.5x8.5 		*
*	double sided on an HP LaserJet.											*
****************************************************************************/
/*
 * $Id: psfdoub.c,v 3.4 1992/02/24 06:48:59 ajf Exp ajf $
 *
*/
/*	psfdoub prints simple reverse order or double sided.

	if the printer is "Normal", i.e. stacks face down, then psf
	calls psfdoub to print simple reverse order or double sided.
	
	if the printer is  "Reverse", i.e. stacks face up, then psf
	calls psfdoub to print the file in reverse to make the page order
	as though it were "Normal".  It also calls psfdoub to print
	in book format. 

	Separate page traversal strategies are needed for "Normal"
	and "Reverse" printers because of the different stacking methods.
	
	psfdoub will NOT print reversed order books or double sided!
	All book and double sided output is in normal order as
	far as the user is concerned.

	Double sided print requires two scans over the data. The first scan
	by "psf" generates postscript output in normal (non-double sided)
	form. A second scan examines the postscript and re-orders the output
	to the printer. The second pass is always done by the "psfdoub" filter.

	Psf generates extra information to make it easy for psfdoub to work.
	It simply constructs pointers to each of the %%Page: beginning
	in the print file.  (see function terminate_printer())  psfdoub
	uses the pointers to "skip about" the file while it does the
	actual page layout.
	
	If the target printer has two paper hoppers (i.e. #define HOPPER is
	defined and "nslots > 2") then psf creates a temp file and system("psfdoub
	fname") is called to immediately generate the double sided output.
	The user does not have to manually invoke psfdoub.

	If HOPPER is not defined or nslots <= 1, psf simply creates an
	appropriate output file and the user must manually invoke psfdoub to
	print the file.

	Double sided print is forced into this routine. It would have been
	simple except for the fact that 2-up "book" format is supported.
	The excuse for this strategy is, of course, to minimize the amount
	of file processing overhead when double sided printing is done.
	
	If the double sided printing is for 8.5x5.5 inch paper printed
	two-up, then psf carefully forgets (in function set_frame())
	to put in page scale, rotation, and translation information.  
	The psfdoub filter reads the page layout and carefully re-inserts 
	these when appropriate.  If 2-up "books" are made, each logical
	page becomes a separate page on the output temp file.
	the psf-generated control information is assumed to be at the
	end of the postscript print file.  The postscript file must contain
	a line like:
					%PsfPtr: 21118
					
	If the %PsPtr line is missing, then the file is NOT a book format.
	The format of this information is described in function read_control().
				
	psfdoub does little to normal double sided print other than to change
	the order in which the pages are printed.  It also adjusts the
	%%Pages: value to reflect the actual page count printed.
	
	psfdoub re-inserts translation, rotation and scale information eliminated
	by psf if the double sided print is to make an 8.5x5.5 "book".

	Of course, a bunch of "very ugly" logic is needed for the "book"
	format to re-insert the postscript for page control - but it does work...
*/

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include "psf.h"

/*	If books are made by generating two print files with the -s side count
	then set SIDECOUNT = 0  (i.e. no alternate paper hopper is available.
	
	If books are made by switching to the alternate print hopper for
	the second side, then set SIDECOUNT = 3  (i.e. second hopper is available
	
	Leave this set to 0.
*/

#define	SIDECOUNT	0

static int nsheets, nhalves;
static int row;
static int lpage, rpage;
void catch();
void trim();
char *malloc();

char	slots[NSLOTS][200] = 
{			"statusdict begin 1 setpapertray end",
			"statusdict begin 2 setpapertray end",
			"", "", "", ""
} ;
int		nslots = 2;

char	order_default[20] = "Normal";
char	order_normal[100] = "\0";
char	order_reverse[100] = "\0";
char	*order_command = NULL;


char	line[LONG_STR + 1];
char	scale[100];
int		max_frame;
int		landscape;
int		real_width;
int		height;
int		width;
int 	dx_home[4];
int 	dy_home[4];
int		npage;
long	*pg_loc;
long	trailer_loc;
int		phys_page;
int		side;
int		bookwork;
int		alternate_tray = 0;
int		second_side = 0;
int		reverse_requested;		/*	user requested reverse?			*/
int		reverse_order;			/*	actually reverse the sequence?	*/
int		need_showpage = 0;		/*	is pending show page needed? 	*/

FILE	*input_fp;
FILE	*output_fp;

char	*pgmname;
char	*defref;

main(argc, argv)
int		argc;
char	*argv[];
{	int		c, i, j;
	extern char *optarg;
	extern int	optind;
	FILE	*pdef;
	char	*env_fname;			/*	environment specified output file */
	char	*getenv();
	char	in_fname[100];
	
	/*	user may specify def file with -u */

	reverse_requested = 0;
	for (i = 0;  i < argc;  i++)
	{
		if (strncmp (argv[i], "-u", 2) == 0)
		{	if (strlen (argv[i]) > 2)
				defref = argv[i] + 2;
			else
				defref = argv[i+1];
			break;
		}
	}
	if (i >= argc)
	{	if ((defref = getenv ("PSFDEF")) == NULL)
			defref = PDEF;
	}

	if ((pdef = fopen (defref, "r")) != NULL)
	{	char	line_type[200];
		int		number;
	
		while (fgets (line, 200, pdef))
		{	if (strncmp (line, "*printer", 8) == 0)
				break;
		}
		nslots = 0;
		fgets (line, 200, pdef);		/*	skip printer name 	*/

		while (fgets (line, 200, pdef))
		{	trim (line);
			sscanf (line, "%s%d", line_type, &number);

			if (strcmp (line_type, "*order") == 0)
			{	fgets (line, 200, pdef);
				line[19] = 0;
				trim (line);
				strcpy (order_default, line);
				if (number > 1)
				{	if (fgets (line, 200, pdef) == NULL)
						bad_file();
					trim (line);
					line[99] = 0;
					strcpy (order_normal, line);
					if (fgets (line, 200, pdef) == NULL)
						bad_file();
					trim (line);
					line[99] = 0;
					strcpy (order_reverse, line);
				}
			}

			else if (strcmp (line_type, "*slots") == 0)
			{	for (j = 0;  j < NSLOTS;  j++)
					slots[j][0] = 0;
				for (i = j = 0;  i < number;  i++)
				{	if (fgets (line, 200, pdef) == NULL)
						bad_file();
					if (i >= NSLOTS)
						continue;
					line[99] = 0;
					trim (line);
					strcpy (slots[i], line);
					j = i;
				}
				nslots = j + 1;
			}

			else if (strcmp (line_type, "*eof") == 0)
				break;
		}
		fclose (pdef);
	}

	pgmname = argv[0];
	if (argc == 1  ||  strcmp (argv[1], "-") == 0)
		usage ();

	side = SIDECOUNT;			/* print size 1, 2, or both = 3 */

	while ((c = getopt(argc, argv, "123vu:-")) != -1)
	{	switch (c)
		{
		case '1':
		case '2':
		case '3':
			side = c - '0';
			break;
		case 'v':
			reverse_requested = 1;
			break;
		case 'u':		/* alread processed */
			break;
		default:
			usage ();
		}
	}
	if (strcmp (order_default, "Reverse") == 0)
	{	reverse_order = 1;
	}
	else
		reverse_order = 0;
	if (reverse_requested)
		reverse_order = !reverse_order;
	if (reverse_order  &&  order_reverse[0])
		order_command = order_reverse;
	else if (reverse_order == 0  &&  order_normal[0])
		order_command = order_normal;
	else
		order_command = NULL;

	if (reverse_requested  &&  side != 0)
	{	fprintf (stderr, "Cannot print double sided or book in reverse order.\n");
		exit (DISCARD_JOB);
	}

	if (side == 3  &&  nslots < 2)
	{	fprintf (stderr, "Cannot use -3: only one output tray\n");
		usage();
	}

	if ( side >= 4)
	{	fprintf (stderr, "Print side must be 1, 2, or 3\n");
		usage ();
	}

	if (optind >= argc)
		strcpy (in_fname, BOOKFILE);
	else
		strcpy (in_fname, argv[optind]);

	if (env_fname = getenv ("PSFLP"))
#if defined(MSDOS)  ||  defined (__MSDOS__)
	{	if ((output_fp = fopen (env_fname, "wt")) == NULL)
#else
	{	if ((output_fp = fopen (env_fname, "w")) == NULL)
#endif
		{	fprintf (stderr, "Cannot open output file %s\n", env_fname);
			exit (DISCARD_JOB);
		}
	}
	else
		output_fp = stdout;

	if ((pg_loc = (long *) malloc (sizeof (long) * MAX_PAGES)) == NULL)
	{	fprintf (stderr, "%s: cannot allocate enough memory\n", pgmname);
		exit (DISCARD_JOB);
	}
	
	if ((input_fp = fopen(in_fname, "r")) == NULL)
	{
		fprintf(stderr, "%s: Error opening %s!\n", pgmname, in_fname);
		exit (DISCARD_JOB);
	}
	(void)	signal (SIGINT, catch);		/*	for lpd quit */
	read_control (input_fp);
	get_prologue ();
	if (reverse_order  &&  side == 0)
		reverse_pages ();
	else if (bookwork)
		print_book ();
	else
		print_double ();
	get_trailer ();
	exit (JOB_IS_DONE);
}

/****************************************************************************
*	read_control()															*
*	Read the control information at the end of the file for the page 		*
*	dimensions and byte address of the start of each page.					*
*	Each of the parameter lines is a comment with the form %Psfxxxx: yyyy	*
****************************************************************************/

read_control (fp)
FILE	*fp;
{	char	tail[100];
	int		i, j;
	long	atol(), psfloc;
	
	/*	%PsfPtr: xxx  --> xxx is the pointer to the first %Psf... line
		in the file.  Seek to the indicated byte position to begin
		reading the Psf generated information which looks like:

				%%Trailer							<--- (a)
				%%DocumentFonts: Courier
				%%Pages: 8
				%PsfScale: 0.63194 0.79558 scale	<--- (b)
				%PsfMargin: 2 1 612 769 626
				%PsfHome: 0 0 0
				%PsfHome: 1 626 0
				%PsfHome: 2 0 0
				%PsfHome: 3 0 0
				%PsfPg: 0 405			<--- byte offsets to page
				%PsfPg: 1 3885
				%PsfPg: 2 7023
					...
				%PsfPg: 9999 21072		<--- points to (a) above
				%PsfPtr: 21118			<--- points to (b) above
				<ctrl/d>
	*/
	fseek (fp, -50L, 2);
	fread (tail, 50, 1, fp);
	if ((i = tscan (tail, "%PsfPtr:")) == -1)
	{	fprintf (stderr, "%s: File is not in psf book format\n", pgmname);
		exit (DISCARD_JOB);
	}
	psfloc = atol (tail + i + 9);		/*	beginning  of psf information */
	fseek (fp, psfloc, 0);
	
	fgets (scale, 99, fp);
	strcpy (tail, scale + 11);			/* get rid of the %psf comment */
	strcpy (scale, tail);

	/*	fetch psf data.	*/
														/*	%PsfMargin:	*/
	fscanf (fp, "%s %d %d %d %d %d", tail,
				&max_frame, &landscape, &real_width, &height, &width);

	bookwork = (max_frame == 2  &&  landscape && !reverse_requested);
	for (i = 0;  i < 4;  i++)							/*	%PsfHome:	*/
		fscanf (fp, "%s %d %d %d",  tail, &j, &dx_home[i], &dy_home[i]);

	npage = 0;											/*	%PsfPg:		*/
	while (fscanf (fp, "%s %d %ld", tail, &i, &pg_loc[npage]) == 3)
	{
		if (i == 9999)
		{	trailer_loc = pg_loc[npage];
			break;
		}
		npage++;
	}
	fseek (fp, 0L, 0);
}

/****************************************************************************
*	get_prologue()															*
*	Read the prologue and pass it directly to the output					*
****************************************************************************/

get_prologue ()
{
	fgets (line, LONG_STR, input_fp);		/*	skip the psf header 	*/
	fprintf (output_fp, "%%!PS-Adobe-\n");	/*	write a valid header	*/
	while (fgets (line, LONG_STR, input_fp) != NULL)
	{	if (strncmp (line, "%%Page:", 7) == 0)
			break;
		fputs (line, output_fp);
	}
}

/****************************************************************************
*	get_trailer ()															*
*	Read the trailer and pass it to the output.  Modify the page count to	*
*	reflect the number of physical pages printed.							*
*	Remove any reference to the %Psf... lines.								*
****************************************************************************/

get_trailer ()
{
	if (phys_page == 0  ||  (bookwork  &&  second_side))
		fprintf (output_fp, "showpage pg restore\n");
	if (alternate_tray == 2)
		fprintf (output_fp, "%s\n", slots[0]);
	fseek (input_fp, trailer_loc, 0);
	while (fgets (line, LONG_STR, input_fp) != NULL)
	{	if (strncmp (line, "%%Pages:", 8) == 0)
			fprintf (output_fp, "%%%%Pages: %d\n", phys_page);
		else if (strncmp (line, "%Psf", 4) != 0)
			fputs (line, output_fp);
	}
}

/****************************************************************************
*	display_page()															*
*	Send all ouput that belongs to a specific page.  For 8.5x5.5 books,		*
*	generate the scale, etc that was omitted by psf.  Also enable the		*
*	alternate tray if it to be used.										*
****************************************************************************/

display_page  (pgno, n)
int pgno;
int	n;
{
	fseek (input_fp, pg_loc[pgno], 0);
	fgets (line, LONG_STR, input_fp);
	if (n == 0)
	{
		if ((phys_page  &&  bookwork)  ||  need_showpage)
			fprintf (output_fp, "showpage pg restore\n");

		if (alternate_tray == 1)
		{	alternate_tray = 2;
			fprintf (output_fp, "%s\n", slots[1]);
		}
		fprintf (output_fp, "%%%%Page: ? %d\n", ++phys_page);

		if ((bookwork  &&  max_frame == 2)  ||  pgno >= npage)
		{	fprintf (output_fp, "/pg save def\n");
		}
		need_showpage = pgno >= npage;
	}

	if (bookwork)
	{	if (n == 0)
		{
			fprintf (output_fp, "90 rotate 0 %d translate\n", -real_width);
			fprintf (output_fp, "%s", scale);
		}
		fprintf (output_fp, "%d %d translate\n", dx_home[n], dy_home[n]);
	}

	if (pgno < npage)
	{	while (fgets (line, LONG_STR, input_fp) != NULL)
		{	if(strncmp (line, "%%", 2) == 0)
				break;
			fputs (line, output_fp);
		}
	}
}


/****************************************************************************
*	print_book()															*
*	print_book() is based on Tom Neff's (tneff@well.uucp) "book" 			*
*	for the HP LaserJet 													*
*	Scan the text to ensure that the 8.5x.5.5 pages are constructed in		*
*	such a way that a "book" is generated - simply staple in the middle	.	*
****************************************************************************/

print_book ()		
{	int	done;

	nsheets = (npage+3)/4;
	nhalves = nsheets*4;
	phys_page = 0;

	if (side == 1 ||  side == 3)
	{	for (rpage=0, lpage=nhalves-1;  rpage < lpage;  rpage+=2, lpage-=2)
		{
			display_page (lpage, 0);
			display_page (rpage, 1);
		}
	}

	if (side == 3)
		alternate_tray = 1;

	if ((side == 2  ||  side == 3)  &&  nhalves > 1)
	{	second_side = 1;
		if (reverse_order)
		{	/* printer stacks face up */
			for (rpage=nhalves-2, lpage=1;  lpage < nhalves/2;  
						rpage-=2, lpage+=2)
			{
				display_page (lpage, 0);
				display_page (rpage, 1);
			}
		}
		else
		{	/*	printer stacks face down */
			for (rpage=nhalves/2, lpage=rpage-1; lpage >= 0; 
						rpage+=2, lpage-=2)
			{
				display_page (lpage, 0);
				display_page (rpage, 1);
			}
		}
	}
	if (need_showpage)
		fprintf (output_fp, "showpage pg restore\n");
}

/****************************************************************************
*	print double()															*
*	print double sided pages.  first odd numbered, then even numbered.		*
****************************************************************************/

print_double ()
{	int	apage;

	nsheets = (npage+1)/2;
	phys_page = 0;

	if (side == 1  ||  side == 3)
	{	for (apage = 0;  apage < nsheets;  apage++)
		{	display_page (apage * 2, 0);
		}
	}

	if (side == 3)
		alternate_tray = 1;

	if ((side == 2  ||  side == 3)  &&  npage > 1)
	{	second_side = 1;
		if (reverse_order)
		{	/*	printer stacks face up */
			for (apage = 0;  apage < nsheets;  apage++)
			{	display_page (apage * 2 + 1, 0);
			}
		}
		else
		{	/* printer stacks face down */
			for (apage = nsheets - 1;  apage >= 0;  apage--)
			{	display_page (apage * 2 + 1, 0);
			}
		}
		
	}
	if (need_showpage)
		fprintf (output_fp, "showpage pg restore\n");
}


/****************************************************************************
*	reverse_pages ()														*
*	normal print, reverse order of pages.									*
****************************************************************************/

reverse_pages ()
{
	int pgno;

	phys_page = 0;
	for (pgno = npage-1;  pgno >= 0;  pgno--)
		display_page (pgno, 0);
}


tscan (s, t)		/* search for string t in s */
char 	s[], t[];
{
	int	i, j, k;
	for (i = 0;  s[i] != '\0';  i++)
	{	for (j = i, k=0;  t[k] != '\0'  &&  s[j] == t[k];  j++, k++)
			;
		if (t[k] == '\0')
			return (i);
	}
	return (-1);
}
usage ()
{
	printf ("Usage:  psfdoub -n [-v] [-u f.def] file\n");
	printf ("  where:         n = side number to print\n");
	printf ("                -0 = not double sided (reverse print only)\n");
	printf ("                -1 = print side 1 only\n");
	printf ("                -2 = print side 2 only\n");
	printf ("                -3 = print both sides in one pass\n");
	printf ("                -v = reverse page order\n");
	printf ("          -u f.def = use this .def file\n");
	printf ("              file = print this file\n");
	exit (DISCARD_JOB);
}

void trim (s)					/*	trim trailing blanks  and \n */
char	*s;
{	int many;

	for (many = strlen (s) - 1;  many >= 0;  many--)
	{	if (isgraph (s[many]))
			break;
		else
			s[many] = '\0';
	}
}


/********************************************************
*	catch ()											*
*	Catch SIGINT from lpd								*
********************************************************/

void catch (signo)
int	signo;
{
		fprintf (output_fp, "%c", 0x04);
		exit (DISCARD_JOB);
}
bad_file()
{
	fprintf (stderr, "Bad %s file\n", defref);
	exit (DISCARD_JOB);
}

