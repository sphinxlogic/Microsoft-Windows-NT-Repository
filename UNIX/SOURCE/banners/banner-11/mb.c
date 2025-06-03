/*
 *	mb:	another banner program with external fontfiles
 *
 *	(C) Copyrighted 1992 by Ulrich Dessauer, Germering, Germany
 *
 *	You can copy, forward, redistribute the whole package as long
 *	as you make no profit and this copyright messages stays untouched!
 */
# include	<stdio.h>
# include	<errno.h>
# ifdef		OSK
# include	<modes.h>
# include	<module.h>
# define	R_OK		S_IREAD
# define	MODHEADSIZ	sizeof (struct modhcom)
# else		/* OSK */
# include	<fcntl.h>
# ifndef	R_OK
# define	R_OK		O_RDONLY
# endif		/* R_OK */
# define	MODSYNC		((unsigned long) 0x4afc)
# define	MODHEADSIZ	0x30
# if		defined (USG) || defined (SYSV) || defined (SYS5) || defined (HAS_STRCHR)
# define	index		strchr
# endif		/* defined (USG) || defined (SYSV) || defined (SYS5) || defined (HAS_STRCHR) */
# endif		/* OSK */

	/* binary read for fopen */
# if		defined (MSDOS) || defined (msdos) || defined (TOS)
# define	READ_BINARY	"rb"
# else		/* defined (MSDOS) || defined (msdos) || defined (TOS) */
# define	READ_BINARY	"r"
# endif		/* defined (MSDOS) || defined (msdos) || defined (TOS) */

# define	FALSE		(0 == 1)
# define	TRUE		(1 == 1)

# ifdef		OSK
# define	DEF_FONTFILE	"/dd/SYS/MB_Font"
# else		/* OSK */
# if		defined (MSDOS) || defined (msdos) || defined (TOS)
# define	DEF_FONTFILE	"\\MB_FONT"
# else		/* defined (MSDOS) || defined (msdos) || defined (TOS) */
# define	DEF_FONTFILE	"/etc/MB_Font"
# endif		/* defined (MSDOS) || defined (msdos) || defined (TOS) */
# endif		/* OSK */

# define	NO_MEM		exit (_errmsg (errno, "Out of memory!\n"));

extern char	*getenv ();
extern char	*index ();
extern char	*malloc ();

typedef unsigned char	byte;

static char		*fontfile;
static int		large;
static int		width;
static int		hight;
static int		cursive;
static int		backcursive;
static int		invers;
static char		forground;
static char		background;

# ifndef	OSK
/*
 *	Not very portable....
 */
int
_errmsg (error, fmt, a, b, c)
int error;
char *fmt;
void *a, *b, *c;
{
	fputs ("mb: ", stderr);
	fprintf (stderr, fmt, a, b, c);
	return (error);
}
# endif		/* OSK */

static void
banner (s, cset)
char *s;
byte *cset;
{
	register byte	*ptr;
	register int	n;
	register int	t;
	register byte	b;
	register int	ch;
	int		u, v;
	int		max;
	static char	*space = NULL;
	static int	spcsiz = 0;

	if (! space) {
		spcsiz = (large ? 16 : 8) * hight;
		if (! (space = malloc (spcsiz + 4)))
			return;
	}
	if (cursive || backcursive) {
		memset (space, invers ? forground : background, spcsiz);
		space[spcsiz] = '\0';
	}
	max = (large ? 16 : 8) * hight;
	for (n = 0; n < max; ++n) {
		if (cursive) {
			space[max - n - 1] = '\0';
			fputs (space, stdout);
		} else if (backcursive)
			for (t = 0; t < n; ++t)
				putchar (invers ? forground : background);
		ptr = (byte *) s;
		while (*ptr) {
			b = cset[(*ptr << (large ? 4 : 3)) + (n / hight)];
				for (t = 7; t >= 0; --t) {
					if (((b & (1 << t)) != 0) ^ invers)
						ch = forground ? forground : (int) *ptr;
					else
						ch = background;
					for (u = 0; u < width; ++u)
						putchar (ch);
				}
			++ptr;
		}
		if (cursive)
			for (t = 0; t < n; ++t)
				putchar (invers ? forground : background);
		else if (backcursive) {
			space[max - n - 1] = '\0';
			fputs (space, stdout);
		}
		putchar ('\n');
	}
}

static char	*help[] = {
	"Syntax: mb [<opts>] [<string(s)] [<opts>]\n",
	"Function: prints a string in large letter\n",
	"Options:\n",
	"     -o=<file> Use <file> instead of default font-file\n",
	"     -l        print an even larger banner than default\n",
	"     -w=<num>  prints banner with <num> width\n",
	"     -h=<num>  prints banner with <num> hight\n",
	"     -c        make the banner cursive (eek, not real cursive...)\n",
	"     -C        make the banner cursive in other direction\n",
	"     -i        invert the characters\n",
	"     -f=<char> use <char> as the forground pattern\n",
	"     -b=<char> use <char> as the background pattern\n",
	"     -s        use the printed character as the forground pattern\n",
	NULL
};

static void
usage ()
{
	register int	t;

	for (t = 0; help[t]; ++t)
		fputs (help[t], stderr);
}

int
main (ac, av)
int ac;
char *av[];
{
	FILE	*fp;
	byte	msync[2];
	byte	*csetl, *csets;
	int	t, err;
	char	*ptr, *tmp;
	int	use_char;
	int	found_string;

	fontfile = NULL;
	large = FALSE;
	width = 1;
	hight = 1;
	cursive = FALSE;
	backcursive = FALSE;
	invers = FALSE;
	forground = '*';
	background = ' ';
	use_char = FALSE;
	found_string = FALSE;
	for (t = 1; t < ac; ++t)
		if ((av[t][0] == '-') && (av[t][1] != '-')) {
			ptr = av[t] + 1;
			while (*ptr) {
				switch (*ptr) {
					case 'o':
						ptr += *(ptr + 1) == '=' ? 2 : 1;
						fontfile = ptr;
						while (*ptr)
							++ptr;
						break;
					case 'l':
						large = TRUE;
						break;
					case 'w':
						ptr += *(ptr + 1) == '=' ? 2 : 1;
						if ((width = atoi (ptr)) < 1)
							width = 1;
						while (*ptr)
							++ptr;
						break;
					case 'h':
						ptr += *(ptr + 1) == '=' ? 2 : 1;
						if ((hight = atoi (ptr)) < 1)
							hight = 1;
						while (*ptr)
							++ptr;
						break;
					case 'c':
						cursive = TRUE;
						break;
					case 'C':
						backcursive = TRUE;
						break;
					case 'i':
						invers = TRUE;
						break;
					case 'f':
						ptr += *(ptr + 1) == '=' ? 2 : 1;
						forground = *ptr;
						break;
					case 'b':
						ptr += *(ptr + 1) == '=' ? 2 : 1;
						background = *ptr;
						break;
					case 's':
						use_char = TRUE;
						break;
					case '?':
					default:
						usage ();
						exit (((*ptr == '?') || (*ptr == 'h')) ? 0 :
							_errmsg (1, "Unknown option '%c'.\n", *ptr));
				}
				if (*ptr)
					++ptr;
			}
		} else
			found_string = TRUE;
	if (use_char)
		forground = '\0';
	if (large) {
		if (! (csetl = (byte *) malloc (256 * 16)))
			NO_MEM
		csets = NULL;
	} else {
		if (! (csets = (byte *) malloc (256 * 8)))
			NO_MEM
		csetl = NULL;
	}
	if (! fontfile) {
		if (ptr = getenv ("HOME"))
			if (tmp = malloc (strlen (ptr) + 16)) {
				sprintf (tmp, "%s/.font", ptr);
				if (access (tmp, R_OK) != -1)
					fontfile = tmp;
				else
					free (tmp);
			}
		if (! fontfile)
			fontfile = DEF_FONTFILE;
	}
	if (! (fp = fopen (fontfile, READ_BINARY)))
		exit (_errmsg (errno, "Can't open %s.\n", fontfile));
	if (fread (msync, sizeof (byte), 2, fp) != 2) {
		fclose (fp);
		exit (_errmsg (errno, "Can't read first %d byte%s.\n",
			sizeof (short), sizeof (short) > 1 ? "s" : ""));
	}
	if ((msync[0] == (byte) ((MODSYNC >> 8) & 0xff)) && (msync[1] == (byte) (MODSYNC & 0xff)))
		err = fseek (fp, MODHEADSIZ, 0);
	else
		err = fseek (fp, 0, 0);
	if (err == -1) {
		fclose (fp);
		exit (_errmsg (errno, "Can't fseek to start of font data!\n"));
	}
	if (large) {
		if (fread (csetl, sizeof (byte), 16 * 256, fp) != 16 * 256) {
			fclose (fp);
			exit (_errmsg (errno, "Can't read font data (large).\n"));
		}
	} else {
		if (fseek (fp, 256 * 16, 1) == -1) {
			fclose (fp);
			exit (_errmsg (errno, "Can't fseek to font data.\n"));
		}
		if (fread (csets, sizeof (byte), 8 * 256, fp) != 8 * 256) {
			fclose (fp);
			exit (_errmsg (errno, "Can't read font data (small).\n"));
		}
	}
	fclose (fp);
	if (found_string) {
		for (t = 1; t < ac; ++t)
			if ((av[t][0] != '-') || (av[t][1] == '-'))
				banner (av[t] + (av[t][0] == '-' ? 1 : 0), large ? csetl : csets);
	} else if (ptr = malloc (130)) {
		while (fgets (ptr, 128, stdin)) {
			if (tmp = index (ptr, '\n'))
				*tmp = '\0';
			banner (ptr, large ? csetl : csets);
		}
		free (ptr);
	}
}
