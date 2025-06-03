/* ta=4 */
/*					s e l e c t d e f . c

	allow user to select printer from various *.def files
	and also select defaule page size (letter, a4 etc)
	
	Generate a postscript programme (psizes.ps) that should be
	sent to the printer to print out the actual imageable area.
*/
/*
 * $Id: selectdef.c,v 3.2 1992/01/19 05:50:33 ajf Exp ajf $
 *
*/

#include <stdio.h>
#include "ctp.h"

typedef struct		 			/*	measurement in point					*/
{	char	paper_name[60];		/*	name of paper size (for command line)	*/
	char	paper_tray[200];	/*	postscript operator to select this tray	*/
	int		width;				/*	portrait point width of paper			*/
	int		height;				/*	portrait point height of paper			*/
} measure;

char *psheader = "%!\n\
/getsize {\n\
	/nf nf 1 add def\n\
	clippath\n\
	pathbbox\n\
} def\n\
/sendsize {\n\
	/ury exch def\n\
	/urx exch def\n\
	/lly exch def\n\
	/llx exch def\n\
	/str 64 string def\n\
	show\n\
	(  ) show\n\
	llx    ceiling cvi str cvs show\n\
	(  ) show\n\
	lly    ceiling cvi str cvs show\n\
	(  ) show\n\
	urx    floor cvi str cvs show\n\
	(  ) show\n\
	ury    floor cvi str cvs show\n\
} def\n\
/pg save def\n\
newpath\n\
/nf 0 def\n";

char *psfont = "/Courier findfont 12 scalefont setfont\n";

char *pstail = "{	/ycoord ycoord -15 add def\n\
	50 ycoord moveto\n\
	sendsize\n\
	/nf nf 1 sub def\n\
	nf 0 eq { exit } if\n\
} loop\n\
showpage \n\
pg restore\n\004";

char	printername[100];
int		defaultsize;

int		screen_lines = 20;
void trim();

main (argc, argv)
int		argc;
char	*argv[];
{	int		i,j;
	int		many, n, pnumber;
	char	printers[100];
	FILE	*fp, *fpdef;
	char	cmd[300];
	char	resp;
	int		lines;
	char	*env_lines, *getenv();


	if (env_lines = getenv ("LINES"))
		screen_lines = atoi (env_lines) - 4;
	
	if (argc < 2)
	{	fprintf (stderr, "Cannot find printer definition files (*.def)...\n");
		exit (1);
	}
redo:
	do
	{
		pnumber = 0;
		printf ("\n\n\n                         SELECT PRINTER TYPE\n\n");
		many = 0;
		lines = 0;
		for (i = 1;  i < argc;  i++)
		{	if ((fp = fopen (argv[i], "r")) == NULL)
				exit (1);
			fgets (printers, 99, fp);	/* skip *printer */
			fgets (printers, 99, fp);
			trim (printers);
			n = strlen (printers);
			if (n < 33)
				n = 33;
			many += n;	
			if (many > 66)
			{	many = n;
				lines++;
				if (lines % screen_lines == 0)
				{	if ((pnumber = request (&resp, argc)) > 0)
						goto gotit;
					if (resp == 'R')
						goto redo;
				}
				else printf ("\n");
			}
			printf ("%3d: %-34s", i, printers);
			fclose (fp);
		}
	} while ((pnumber = request (&resp, argc)) == 0);
gotit:
	printf ("\n");
	if (access (argv[pnumber], 4) == 0)
	{
		if (getsize (argv[pnumber]) == 0)
		{	sprintf (cmd, "cp %s psfprint.def", argv[pnumber]);
			system (cmd);
			genpost (argv[pnumber]);
		}
	}
	else
	{	fprintf (stderr, "Cannot find printer definition file: %s\n", argv[pnumber]);
		exit (1);
	}
	exit (0);
}

request (r, maxno)
char	*r;
int		maxno;
{	char printers[100];
	int	 pnumber;
	do
	{	*printers = '\0';
		printf ("\n==> ( q = quit, r = refresh display, ENTER = next screen)\n    SELECT PRINTER NUMBER: ");
		gets (printers);
		if (UCCHAR (*printers) == 'Q')
			incomplete ();
		if (UCCHAR (*printers) == 'R')
		{	*r = 'R';
			return (0);
		}
		pnumber = atoi (printers);
		if (pnumber < 1)
			break;
		else if (pnumber > maxno)
			printf ("... Invalid printer number\n");
		else
			break;
	} while (1);
	*r = ' ';
	return (pnumber);
}

getsize (fn)
char	*fn;
{	char	ptypes[40][100];
	char	line[100];
	int		ntypes, i, ncol, pnumber;
	FILE	*fpdef;

	if ((fpdef = fopen (fn, "r")) != NULL)
	{	fgets (printername, 99, fpdef);	/* skip *printer */
		fgets (printername, 99, fpdef);
		trim (printername);
		ntypes = 1;
		for ( ; ; )
		{	fgets (line, 99, fpdef);
			if (*line == '*')
				break;
			trim (line);
			strcpy (ptypes[ntypes], line);
			fgets (line, 99, fpdef);
			fgets (line, 99, fpdef);
			ntypes++;
		}
		fclose (fpdef);
		printf ("\nPrinter is: %s\n\n", printername);
	}
	else
	{	fprintf (stderr, "Cannot find printer definition file: %s\n", fn);
		return (1);
	}
	return (0);
}

genpost (f)
char	*f;
{
	FILE *pdef, *psfile;
	char	line[500];
	measure	page_types[40];
	int		i, j;
	int		x, y;

	if ((pdef = fopen (f, "r")) != NULL)
	{	fgets (line, 100, pdef);		/* skip *printer 		*/
		fgets (line, 100, pdef);		/*	skip printer name 	*/
		fgets (line, 100, pdef);		/*	skip *paper			*/
		for (i = 0;  i < 28;  i++)
		{	if (fgets (page_types[i].paper_name, 59, pdef) == NULL)
				break;
			trim (page_types[i].paper_name);
			if (page_types[i].paper_name[0] ==  '*')
				break;
			if (fgets (page_types[i].paper_tray, 199, pdef) == NULL)
				bad_file(f);
			trim (page_types[i].paper_tray);
			if (fgets (line, 100, pdef) == NULL)
				bad_file(f);
			trim (line);
			sscanf (line, "%d%d",
				&page_types[i].width,
				&page_types[i].height);

		}
		page_types[i].paper_name[0] = '\0';
		fclose (pdef);
		if ((psfile = fopen ("psizes.ps", "w")) != NULL)
		{	fprintf (psfile, "%s", psheader);
			for (j = i-1;  j >= 0;  j--)
			{	lcase (line, page_types[j].paper_name);
				fprintf (psfile, "%s\n(%-10s:%5d%5d) getsize\n",
							line,
							page_types[j].paper_name,
							page_types[j].width, 
							page_types[j].height);
			}
/*
			fprintf (psfile, "%s\n", page_types[defaultsize].paper_tray);
*/
			fprintf (psfile, "%s", psfont);
			x = 50;
			y = 500;
			fprintf (psfile, "%d %d moveto\n", x, y);
			fprintf (psfile, "(Imageable area) show\n");
			y -= 15;
			fprintf (psfile, "%d %d moveto\n", x, y);
			fprintf (psfile, "(");
			send (printername, psfile);
			fprintf (psfile, ") show\n");
			fprintf (psfile, "/ycoord %d def\n", y);
			fprintf (psfile, "%s", pstail);
		}
		else
			fprintf (stderr, "Cannot create psizes.ps\n");
	}
	else
		fprintf (stderr, "Cannot open %s for reading\n", f);
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



bad_file(f)
char	*f;
{
	fprintf (stderr, "Bad %s file\n", f);
	exit (1);
}

lcase (a, b)
char	*a, *b;
{
	while (*b)
	{	*a++ = LCCHAR (*b);
		b++;
	}
	*a = '\0';
}

send (s, fp)
char	*s;
FILE	*fp;
{
	char	*strchr();
	
	while (*s)
	{
		if (strchr ("\r\b\\()", *s) != NULL)
			fputc ('\\', fp);
		fputc (*s, fp);
		s++;
	}
}

incomplete ()
{
	printf ("Incomplete printer installation\n");
	exit (44);
}
