/* ta=4 */
/************************************************************************
*					t a b l e . c										*
*																		*
*	Generate the *.def tables required by psf to describe a printer		*
*																		*
*	Scans the Adobe *.ppd files for specific parametric lines.			*
*	Tony FIeld,   tony@ajfcal.cuc.ab.ca								`	*
************************************************************************/
/*
 * $Id: table.c,v 3.5 1992/06/02 08:16:01 ajf Exp ajf $
 *
*/

/*	Usage:    table files.ppd ...  destination.dir

		where:	files.ppd are the various .ppd files to be converted.
				destination.dir is the destination directory in which
					to place the generated .def file(s)
		
	The layout of the generated .def files is:

*printer								<-- marker
NEC LC-890 v47.0 (modified)             <-- printer name for selectdef
										    (from *Nickname)
Letter                                  <-- user name (e.g.   psf -g Letter)
											(from *PageSize)
statusdict begin lettertray end         <-- actual postscript to select size
											(from *PageSize)
  612   792    18    15   599   776     <-- page width page height LLx LLy URx URy
  											(page width/height from *PaperDimension)
											(LLx LLy URx URy from *ImageableArea)
Legal
statusdict begin legaltray end
  612  1008    18    19   593   990
A4
statusdict begin a4tray end
  595   842    10     8   585   813
B5
statusdict begin b5tray end
  516   729    18     7   482   702
*duplex									<-- marker for duplex
*order									<-- marker for output order
											from DefaultOutputOrder or
											OutputOrder
Normal									<-- default output order
statusdict begin 0 setoutputtray end	<-- set to normal
statusdict begin 1 setoutputtray end	<-- set to reverse
*fonts                                  <-- marker for fonts
AvantGarde-Book                         <-- real font names in font dictionary
AvantGarde-Demi								(from *Font)
Bookman-Demi
Bookman-Light
Courier
Helvetica
Helvetica-Narrow
NewCenturySchlbk-Roman
Palatino-Roman
Symbol
Times-Roman
ZapfChancery-MediumItalic
ZapfDingbats
*slots    								<-- marker for paper tray select
statusdict begin 1 setpapertray end     <-- select first tray
statusdict begin 2 setpapertray end     <-- select second tray  etc..
											(from *InputSlot)
*eof									<-- logical end of printer definition
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include "ctp.h"

char	nickname[200];
struct pdef
{
	char	name[500];
	char	set[500];
	char	tray[500];
	int		lx, ly, ux, uy, sx, sy;
} ;

struct outorder
{	char	bydefault[100];
	char	normal[150];
	char	reverse[150];
} order;

char	duplex[500];

char	*malloc();
char	pname[100];
struct pdef printer[40], pblank;
int		nprinter = 0;
char	*fonts[100];
char	*slots[10];
int		nfonts=0;
int		nslots=0;
void	trim();

struct sizes
{	char	name[50];
	int		wide;
	int		high;
} ;

struct sizes lookfor[] =
{	{"executive",  522,   756},
	{"tabloid",    792,  1224},
	{"statement",  396,   612},
	{"note",       612,   792},
	{"ledger",    1224,   792},
	{"b5",         516,   729},
	{"b4",         729,  1032},
	{"a5",         420,   595},
	{"a4small",    595,   842},
	{"a4",         595,   842},
	{"a3",         842,  1190},
	{"legal",      612,  1008},
	{"letter",     612,   792},
	{"\0",           0,      0}
} ;

main (argc,argv)
int 	argc;
char	*argv[];
{
	char	*fname;
	FILE	*fp_in, *fp_out;
	int		i, j;
	char	*c;
	char	outname[100], fnonly[50];
	char	*destdir;
	
	if (argc < 3)
		usage ();

	argc--;
	destdir = argv[argc];
	if (isdir (destdir) != 1)
		usage ();

	for (i = 1;  i < argc;  i++)
	{	if ((fp_in = fopen (argv[i], "r")) != NULL)
		{	fprintf (stderr, "%s\n", argv[i]);
			for (j = 0;  j < nprinter;  j++)
				printer[j] = pblank;
			
			for (j = 0;  j < nfonts;  j++)
				free (fonts[j]);
			
			for (j = 0;  j < nslots;  j++)
				free (slots[j]);
			
			nfonts = nslots = nprinter = 0;

			fnameonly (argv[i], fnonly);
			if ((c = strchr(fnonly, '.')))
				*c = '\0';
			strcpy (outname,destdir);
			strcat (outname, "/");
			strcat (outname, fnonly);
			strcat (outname, ".def");

			if ((fp_out = fopen (outname, "w")) == NULL)
			{	fprintf (stderr, "%s: cannot create file %s\n", argv[0], outname);
				exit (1);
			}
			build (fp_in);
			display (fp_out);
			fclose (fp_out);

			fclose (fp_in);
		}
	}
		
}

build (fp_in)
FILE	*fp_in;
{
	char	s[1000];
	int		n;
	int		a, b, c, d;
	char	which[500];

	order.bydefault[0] = 0;
	order.normal[0] = 0;
	order.reverse[0] = 0;
	duplex[0] = 0;
	
	while (fgets (s, 999, fp_in) != NULL)
	{	trim (s);
	
		if (strncmp (s, "*PageSize", 9) == 0)
		{
			n = ptype (s, which);
			getmultitext (fp_in, s, printer[n].set);
		}
		else if (strncmp (s, "*NickName:", 10) == 0)
		{
			gettext (s, nickname);
		}
		else if (strncmp (s, "*Font ", 6) == 0)
		{
			ftype (s);
		}
		else if (strncmp (s, "*DefaultOutputOrder:", 20) == 0)
		{	strcpy (order.bydefault, s + 21);
		}
		else if (strncmp (s, "*OutputOrder Normal:", 20) == 0)
		{
			gettext (s, order.normal);
		}
		else if (strncmp (s, "*OutputOrder Reverse:", 21) == 0)
		{
			gettext (s, order.reverse);
		}
		else if (strncmp (s, "*PaperTray", 10) == 0)
		{
			n = ptype (s, which);
			getmultitext (fp_in, s, printer[n].tray);
		}
		else if (strncmp (s, "*InputSlot", 10) == 0)
		{
			getmultitext (fp_in, s, which);
			if (*which)
			{	slots[nslots] = malloc (strlen(which) + 2);
				strcpy (slots[nslots], which);
				nslots++;
			}
		}
		else if (strncmp (s, "*ImageableArea", 14) == 0)
		{
			n = ptype (s, which);
			sscanf (strchr (s, '"') + 1, "%d%d%d%d", &a, &b, &c, &d);
			printer[n].lx = a;
			printer[n].ly = b;
			printer[n].ux = c;
			printer[n].uy = d;
		}
		else if (strncmp (s, "*PaperDimension", 15) == 0)
		{
			n = ptype (s, which);
			sscanf (strchr (s, '"') + 1, "%d %d", &a, &b);
			printer[n].sx = a;
			printer[n].sy = b;
		}
		else if (strncmp (s, "*Duplex True:", 13) == 0)
		{
			getmultitext (fp_in, s, duplex);
		}
	}
}


ptype (s, which)
char	*s, *which;
{
	char	*w, *ww;
	int		i;

	w = s;
	while (*w++ != ' ')
		;
	ww = which;
	while (*w != ':')
	{
		if (*w == '/')
		{

			while (*w++ != ':')
				;
			break;
/*
			ww = which;
			w++;
			continue;
*/
		}
		if (*w == ' ')
			*w = '-';
		*ww++ = *w++;
	}
	*ww = '\0';
	while (*w == ' ')
		w++;
/*
	sscanf (s, "%*s%s", which);
*/
/*
	w = which;
	while (*w)
	{	if (*w == ':')
		{	*w = '\0';
			break;
		}
		w++;
	}
*/
	for (i = 0;  i < nprinter;  i++)
	{	if (strcmp (which, printer[i].name) == 0)
			return (i);
	}
	strcpy (printer[nprinter].name, which);
	nprinter++;
	return (nprinter - 1);
}

ftype (s)
char	*s;
{
	char	*w, *ww;
	char	which[100];
	int		i;

	w = s;
	while (*w++ != ' ')
		;
	ww = which;
	while (*w != ':')
	{
		if (*w == ' ')
			*w = '-';
		*ww++ = *w++;
	}
	*ww = '\0';
	while (*w == ' ')
		w++;

	if (strncmp (which, "Za", 2))
	{	if (tscan (which, "Bold") >= 0   ||  tscan (which,"Italic") >= 0 
			|| tscan (which,"Obliq") >= 0)
			return;
	}
	
	fonts[nfonts] = malloc (strlen (which) + 1);
	strcpy (fonts[nfonts++], which);
	
}


display (fp_out)
FILE	*fp_out;
{	char	pn[100], *p, newpg[200];
	int		i, j;
	
	fprintf (fp_out, "*printer\n");
	fprintf (fp_out, "%s\n", nickname);
	fprintf (fp_out, "*paper %d\n", nprinter);
	for (i = 0;  i < nprinter;  i++)
	{	if (printer[i].sx + printer[i].sy == 0)
		{	/* must be a specification for physical page size.
				we better guess.
			*/
			for (j = 0;  lookfor[j].name[0];  j++)
			{	strcpy (newpg, lookfor[j].name);
				if (compare (printer[i].name, newpg) == 0)
				{	printer[i].sx = lookfor[j].wide;
					printer[i].sy = lookfor[j].high;
					fprintf (stderr, "\tusing default size: %s %d %d \n", 
						printer[i].name,  printer[i].sx, printer[i].sy);
					break;
				}
				else
				{	strcat (newpg, ".transverse");
					if (compare (printer[i].name, newpg) == 0)
					{	printer[i].sx = lookfor[j].high;
						printer[i].sy = lookfor[j].wide;
						fprintf (stderr, "\tusing default size: %s %d %d \n", 
							printer[i].name,  printer[i].sx, printer[i].sy);
						break;
					}
				}
			}
		}
		if (printer[i].sx + printer[i].sy == 0)
			fprintf (stderr, "\tunknown paper size: %s %d %d \n", 
				printer[i].name,  printer[i].sx, printer[i].sy);
		fprintf (fp_out, "%s\n%s\n", printer[i].name, printer[i].set);
		fprintf (fp_out, "%5d %5d %5d %5d %5d %5d\n",
			printer[i].sx, printer[i].sy,
			printer[i].lx, printer[i].ly, printer[i].ux, printer[i].uy);

/*		print the actual size of the image area 

		strcpy (pn, nickname);
		p = pn;
		while (*p)
		{	if (*p == ' ')
				*p = '.';
			p++;
		}
		printf ("%-55s %-20s %4d %4d\n", pn, printer[i].name,
			printer[i].ux - printer[i].lx + 1,
			printer[i].uy - printer[i].ly + 1);
*/

	}

	if (duplex[0])
	{	fprintf (fp_out, "*duplex 1\n");
		fprintf (fp_out, "%s\n", duplex);
	}

	if (order.normal[0]  ||  order.reverse[0])
		fprintf (fp_out, "*order 3\n");
	else
		fprintf (fp_out, "*order 1\n");
	fprintf (fp_out, "%s\n", order.bydefault);
	if (order.normal[0]  ||  order.reverse[0])
	{	fprintf (fp_out, "%s\n", order.normal);
		fprintf (fp_out, "%s\n", order.reverse);
	}
	fprintf (fp_out, "*fonts %d\n", nfonts);
	for (i = 0;  i < nfonts;  i++)
		fprintf (fp_out, "%s\n", fonts[i]);

	fprintf (fp_out, "*slots %d\n", nslots);
	for (i = 0;  i < nslots;  i++)
		fprintf (fp_out, "%s\n", slots[i]);
	fprintf (fp_out, "*eof\n");
}

gettext (s, t)
char	*s, *t;
{
	strcpy (t, strchr (s, '"') + 1);
	while (*t)
	{	if (*t == '"')
		{	*t = '\0';
			break;
		}
		t++;
	}
}

getmultitext (fp, s, t)
FILE	*fp;
char	*s, *t;
{	int	eos;
	char	xlin[500], *c;

	eos = 0;
	c = strchr (s, '"');
	if (c == NULL)
	{	*t = 0;
		return;
	}
	strcpy (t, c + 1);
	while (eos == 0)
	{	while (*t)
		{	if (*t == '"')
			{	*t = '\0';
				eos = 1;
				break;
			}
			t++;
		}
		if (eos == 0)
		{	fgets (xlin, 499, fp);
			trim (xlin);
			c = xlin;
			while (isspace (*c))
				c++;
			if (strlen (t))
				strcat (t, " ");
			strcat (t, c);
		}
	}
}

tscan (s, t)
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



fnameonly (path, fn)
char	*path, *fn;
{
	char	*strrchr(), *where;
	
	if ((where = strrchr (path, '/')) == NULL)
		strcpy (fn, path);
	else
		strcpy (fn, where + 1);
	return;
}


/****************************************************************************
*	isdir (name)															*
*	See if the 'name' is a directory entry.  If so, return 1.  If not,		*
*	return a 0.																*
****************************************************************************/

isdir (name)
char	*name;
{
	struct stat buf;

	if (stat (name, &buf) != 0)				/* ensure stat() is happy		*/
		return (-1);
	if ((buf.st_mode & S_IFMT) == S_IFDIR) 	/* good call, see if 			*/
		return (1);							/*		is directory			*/
	if ((buf.st_mode & S_IFMT) == S_IFREG) 	/* good call, see if			*/
		return (0);							/*		is file					*/
	return (-1);							/* something else				*/
}

usage ()
{	fprintf (stderr, "Usage: table f.ppd f.ppd ... /dest.dir\n");
	fprintf (stderr, "   where   f.ppd    = source .ppd files\n");
	fprintf (stderr, "           dest.dir = desination directory for .def files\n");
	exit (1);
}			
	
int compare (a,b)				/*	case insensitive compare */
char	*a, *b;
{	int	aa, bb;

	while (*a)
	{	aa = *a++;
		bb = *b++;
		if (UCCHAR (aa)  !=  UCCHAR (bb))
			return (UCCHAR(aa) - UCCHAR(bb));
	}
	return (UCCHAR (*a) - UCCHAR (*b));
}
