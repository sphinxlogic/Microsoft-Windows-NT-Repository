/* ta=4 */
/****************************************************************************
*				p s f b a n n e r . c										*
*																			*
*	Print a banner page on a postscript printer								*
*																			*
*	The generated print stream is NOT minimally conforming as per Red Book.	*
*																			*
*	Tony Field.       tony@ajfcal.cuc.ab.ca									*
****************************************************************************/

/*
 * $Id: psfbanner.c,v 3.4 1992/02/24 06:48:59 ajf Exp ajf $
 *
*/

#include <stdio.h>
#include <string.h>
#include "ctp.h"
#include "psf.h"
#include "patchlevel.h"

/* modifiy the following font selections if language changes
   are needed in psfprint.def. Also enable the language translation
   code at line 257  (   if (need_xlate)  )
*/

#define FONT1 "/Helvetica-BoldOblique findfont %d scalefont setfont\n"
#define FONT2 "/Helvetica-Bold findfont %d scalefont setfont\n"
#define FONT3 "/Courier findfont %d scalefont setfont\n"
#define FONT4 "/Helvetica-Bold findfont %d scalefont setfont\n"

/*	setup paper dimensions									*/

typedef struct		 			/*	measurement in point					*/
{	char	paper_name[60];		/*	name of paper size (for command line)	*/
	char	paper_tray[200];	/*	postscript operator to select this tray	*/
	int		width;				/*	portrait point width of paper			*/
	int		height;				/*	portrait point height of paper			*/
	int		lx, ly, ux, uy;
} measure;

measure	*p;		/*	working set of parameters  (points to something below) 	*/

				/*	standard paper sizes in portrait layout.				 */

measure	page_types[NPAGE] =
{			{ 	"letter",
				"statusdict begin lettertray end",
				  612,   792,    18,    15,   593,   777
			},
			{ 	"legal",
				"statusdict begin legaltray end",
				  612,  1008,    18,    15,   593,   993
			},
			{	"a4",
				"statusdict begin a4tray end",
				  595,   842,    18,    15,   578,   827
			},
			{	"b5",
				"statusdict begin b5tray end",
				  516,   729,    18,    15,   497,   712
			},
			{	"",			/* end of paper list */
				"",
				 0, 0,  0,  0, 0, 0
			}
};

char	slots[NSLOTS][200] = 
{			"statusdict begin 1 setpapertray end",
			"statusdict begin 2 setpapertray end",
			"", "", "", ""
} ;
int		nslots = 2;

int	X_INDENT	= 30;		/*	points						*/
int Y_INDENT	= 30;
int	BIG_POINT	= 60;		/* changes to point size automatically scale	*/
int MED_POINT	= 35;		/* the printout.								*/
int	SMALL_POINT	= 15;
int	UX, UY, DX, DY, MAX_X, MAX_Y;

/*	The following arguments are received in the command line
	argv[9] and argv[10] may be page size and paper tray specifications.
	these could be:
	
			-b n	= select paper bin n
			-g size = selelct paper tray (letter, b5, etc)
*/

#define	Userid		argv[1]
#define	Name		argv[2]
#define	Requestid	argv[3]
#define	Printer		argv[4]
#define	Options		argv[5]
#define	Date		argv[6]
#define	Machineid	argv[7]
#define	Title		argv[8]

char	*defref = NULL;

FILE	*outfp;
unsigned char	xlate[256];			/*	translation vector 	*/
int		need_xlate = 0;				/*	no default xlate	*/
FILE	*pdef;
void trim();

main (argc, argv)
int		argc;
char	*argv[];
{	int		x, y, c, i, set_paper_tray, set_paper_bin, number, j;
	char	line[200];

	if (argc < 8)
		usage ();

	for (i = 0;  i < 256;  i++)
		xlate[i] = i;
	
	outfp = stdout;
	pdef = NULL;

	/* get user specified def file with -u */

	for (i = 0;  i < argc;  i++)
	{	if (strncmp (argv[i], "-u", 2) == 0)
		{	if (strlen (argv[i]) > 2)
				defref = argv[i] + 2;
			else
				defref = argv[i+1];
			break;
		}
	}
	if (i >= argc)
		defref = PDEF;

	if ((pdef = fopen (defref, "r")) != NULL)
	{	char	line_type[200];

		while (fgets (line, 200, pdef))
		{	if (strncmp (line, "*printer", 8) == 0)
				break;
		}
		nslots = 0;
		fgets (line, 200, pdef);		/*	skip printer name 	*/

		while (fgets (line, 200, pdef))
		{	trim (line);
			sscanf (line, "%s%d", line_type, &number);

			if (strcmp (line_type, "*paper") == 0)
			{	for (i = j = 0;  i < number;  i++)
				{	if (i >= NPAGE - 1)
					{	fgets (line, 200, pdef);
						fgets (line, 200, pdef);
						fgets (line, 200, pdef);
						continue;
					}
					if (fgets (line, 200, pdef) == NULL)
						bad_file();
					line[59] = 0;
					trim (line);
					strcpy (page_types[i].paper_name, line);
					if (fgets (line, 99, pdef) == NULL)
						bad_file();
					line[99] = 0;
					trim (line);
					strcpy (page_types[i].paper_tray, line);
					if (fgets (line, 100, pdef) == NULL)
						bad_file();
					trim (line);
					sscanf (line, "%d%d%d%d%d%d",
						&page_types[i].width, &page_types[i].height,
						&page_types[i].lx,    &page_types[i].ly,
						&page_types[i].ux,    &page_types[i].uy);
					j = i;
				}
				page_types[j+1].paper_name[0] = '\0';
			}

			else if (strcmp (line_type, "*slots") == 0)
			{
				for (j = 0;  j < NSLOTS;  j++)
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
	}

	MAX_X = page_types[0].width;
	MAX_Y = page_types[0].height;
	set_paper_bin = 0;
	set_paper_tray = -1;
	
	for (i = 9;  i < argc;  i++)
	{	if (argv[i][0] != '-')
			continue;
		c = argv[i][1];
		switch (c)
		{
		case 'b':
			if ((set_paper_bin = atoi (argv[i+1])) > nslots)
			{	fprintf (stderr, "paper bin greater than %d\n", nslots);
				exit (DISCARD_JOB);
			}
			i++;
			break;

		case 'g':
			for (set_paper_tray = 0;  page_types[set_paper_tray].paper_name[0];  set_paper_tray++)
			{	if (compare (argv[i+1], page_types[set_paper_tray].paper_name) == 0)
				{	MAX_X = page_types[set_paper_tray].width;
					MAX_Y = page_types[set_paper_tray].height;
					break;
				}
			}
			if (page_types[set_paper_tray].paper_name[0] == '\0')
			{	fprintf (stderr, "Invalid paper size\n");
				exit (DISCARD_JOB);
			}
			i++;
			break;

		case 'u':			/* already processed */
			break;

		default: ;
		}
	}

	UX	=	X_INDENT;
	UY	=	(MAX_Y - Y_INDENT);
	DX	=	(MAX_X - 2 * X_INDENT);
	DY	=	(BIG_POINT * 3);

	/*	center text used for userid and job title print  */
	
	send ("%!\n");
	send ("/ctext {     % center text:   string x y dx\n");
	send ("   2 div\n");
	send ("   /Dx exch def\n");
	send ("   /Yv exch def\n");
	send ("   /Xv exch def\n");
	send ("   dup stringwidth pop\n");
	send ("   2 div\n");
	send ("   Dx exch sub\n");
	send ("   Xv add\n");
	send ("   Yv moveto\n");
	send ("   show\n");
	send ("} def\n");
	replacement_fonts();				/*	get font translations */
	send ("%%EndProlog\n");

	/*	Uncomment the following code if languange translation is
		needed for the following parameters
	*/
/*
	if (need_xlate)
	{	xlate_string (Userid);
		xlate_string (Name);
		xlate_string (Requestid);
		xlate_string (Printer);
		xlate_string (Options);
		xlate_string (Date);
		xlate_string (Machineid);
		xlate_string (Title);
	}
*/
	send ("%%Page: ? 1\n");
	if (set_paper_bin)
		printf ("%s\n", slots[set_paper_bin-1]);

	if (set_paper_tray >= 0)
		printf ("%s\n", page_types[set_paper_tray].paper_tray);
		
	send ("/pg save def\n");

	/*	draw a box for the userid */
	
	printf ("newpath\n");
	printf ("%d %d moveto\n", UX,UY);
	printf ("%d %d rlineto\n", DX, 0);
	printf ("%d %d rlineto\n", 0, -DY);
	printf ("%d %d rlineto\n", -DX, 0);
	printf ("closepath\n");
	printf ("4 setlinewidth\n");
	printf ("stroke\n");

	/*	center the userid and the job title */
	
	y = UY - DY / 2 - BIG_POINT / 2;
	printf (FONT1, BIG_POINT);
	printf ("(%s) %d %d %d ctext\n", Userid, X_INDENT, y, DX);

	y = UY - (DY + MED_POINT * 3);
	printf (FONT2, MED_POINT);
	printf ("(%s) %d %d %d ctext\n", Title, X_INDENT, y, DX);

	/*	print other banner page parameters */
	
	x = X_INDENT;
	y -= (MED_POINT * 2);
	printf ("%d %d moveto\n", x,y);	
	sendnormal ("User:       ");
	sendbold (Name);

	y -= SMALL_POINT + (SMALL_POINT / 2);
	printf ("%d %d moveto\n", x,y);	
	sendnormal ("Request ID: ");
	sendbold (Requestid);
	
	y -= SMALL_POINT + (SMALL_POINT / 2);
	printf ("%d %d moveto\n", x,y);	
	sendnormal ("Printer ID: ");
	sendbold (Printer);
	
	y -= SMALL_POINT + (SMALL_POINT / 2);
	printf ("%d %d moveto\n", x,y);	
	sendnormal ("Options:    ");
	sendbold (Options);
	
	y -= SMALL_POINT + (SMALL_POINT / 2);
	printf ("%d %d moveto\n", x,y);	
	sendnormal ("Date:       ");
	sendbold (Date);
	
	y -= SMALL_POINT + (SMALL_POINT / 2);
	printf ("%d %d moveto\n", x,y);	
	sendnormal ("Machine:    ");
	sendbold (Machineid);

	send ("showpage pg restore\n");
	send ("%%Trailer\n");	
	exit (JOB_IS_DONE);
}


/****************************************************************************
*	send font selection information and text.								*
****************************************************************************/

send (s)
char	*s;
{
	while (*s)
		fputc (*s++, outfp);
}

sendnormal (s)
char 	*s;
{
	printf (FONT3, SMALL_POINT);
	send ("(");
	send (s);
	send (")show\n");
}

sendbold (s)
char 	*s;
{
	printf (FONT4, SMALL_POINT);
	send ("(");
	send (s);
	send (")show\n");
}

/****************************************************************************
*	trim ()																	*
*	trucate garbage															*
****************************************************************************/

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

/****************************************************************************
*	bad_file()																*
****************************************************************************/

bad_file()
{
	fprintf (stderr, "Bad %s file\n", defref);
	exit (DISCARD_JOB);
}

/****************************************************************************
*	compare ()																*
*	partial compare of two strings.											*
****************************************************************************/

compare (a,b)
char	*a, *b;
{	int	aa, bb;

	while (*a  &&  *b)
	{	aa = *a++;
		bb = *b++;
		if (UCCHAR (aa)  !=  UCCHAR (bb))
			return (aa - bb);
	}
	return (0);
}


/****************************************************************************
*	replacement_fonts()														*
*	read the text translation strings only.									*
****************************************************************************/

replacement_fonts ()
{	int		found, xold, xnew;
	char	line[201];
	char	*x;

	if (pdef == NULL)
		return;
	found = 0;
	while (fgets (line, 200, pdef))		/*	skip printer name 	*/
	{	if (strncmp (line, "%%PsfStart", 10) == 0)
		{	found = 1;
			break;
		}
	}
	if (found)
	{	while (fgets (line, 200, pdef))			/*	read fonts, translate */
		{	if (strncmp (line, "%%PsfEnd", 8) == 0)
				break;
			if (strncmp (line, "%%translate",  11) == 0)
			{	x = strtok (line + 11, " ");
				do
				{	xold = otoi (x) % 256;
					xnew = otoi (x+4) % 256;
					xlate[xold] = xnew;
				} while (x = strtok (NULL, " "));
				need_xlate = 1;
			}
		}
		fclose (pdef);
		pdef = NULL;
	}
	else
	{	fclose (pdef);
		pdef = NULL;
	}
	return;
}

/****************************************************************************
*	octal to integer														*
****************************************************************************/

int	otoi (s)
char	*s;
{
	int		v, p, quit;

	v = 0;
	while (*s == ' ')
		s++;
	quit = 0;
	while (*s)
	{	switch (*s)
		{
		case '0':	p = 0;  break;
		case '1':	p = 1;  break;
		case '2':	p = 2;  break;
		case '3':	p = 3;  break;
		case '4':	p = 4;  break;
		case '5':	p = 5;  break;
		case '6':	p = 6;  break;
		case '7':	p = 7;  break;
		default:	quit = 1;
		}
		if (quit)
			break;
		v = (v << 3) + p;
	}
	return (v);
}


/****************************************************************************
*	xlate_string ()															*
*	translate the string according to the language translation vector		*
****************************************************************************/
xlate_string (s)
unsigned char	*s;
{
	int	sold;
	
	while (*s)
	{	sold = *s;
		*s = xlate[sold];
		s++;
	}
}


/****************************************************************************
*	usage()																	*
****************************************************************************/
usage ()
{
	fprintf (stderr, "Usage:  psfbanner Userid Name Requestid Printer Options Date\n");
	fprintf (stderr, "                   Machineid Title [-b n] [-g size] [-u f.def]\n");
	fprintf (stderr, "        where:   Userid, Name, etc are passed by lpsched\n");
	fprintf (stderr, "                 -b n    = select paper bin n\n");
	fprintf (stderr, "                 -g size = select paper size (letter, b5, etc)\n");
	fprintf (stderr, "                -u f.def = use this printer definition file\n");	
	exit (DISCARD_JOB);
}		
