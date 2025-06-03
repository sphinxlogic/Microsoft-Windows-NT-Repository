#include <stdio.h>
#ifdef OSK
#include <modes.h>
#else
#define S_IREAD 0
#endif
#include "flags.h"

#define TRUE  1
#define FALSE 0

extern int	errno;
extern void	exit();

#ifdef OSK
int sighandler() {}
#endif

/*
 * m a i n
 */

main(argc, argv)
int	argc;
char	*argv[];
{
	char		*str, strbuf[200];
	int		i, j, strnum;
	FILE		*infp;
	char		*from;
#ifdef OSK
	char		*plife;
#endif
	banflags	flags;

	flags		= banFlagDefault;
	strnum		= 0;
	from		= NULL;
#ifdef OSK
	plife		= NULL;
#endif

for (i = 1; i < argc; i++)
	if (argv[i][0] == '-')
		for (j = 1; j < strlen(argv[i]); j++)
			switch(tolower(argv[i][j])) {
			case '?':
				usage();
				exit(1);

			case 'i':	/* italic printing */
				flags.italic = TRUE;
				break;

			case 'd':	/* double sized characters */
				flags.height = 2;
				flags.dblwidth = TRUE;
				break;
#ifdef OSK
			case 'l':	/* secret option for OSK */
				plife = argv[i] + j + (argv[i][j+1] == '=' ? 2 : 1);
				j = strlen(argv[i]);
				break;
#endif
			case 'c':	/* character for banner */
				j += argv[i][j+1] == '=' ? 2 : 1;
				flags.bannerch = argv[i][j];
				break;

			case 's':	/* use character */
				flags.bannerch = '\0';
				break;

			case 'z':	/* get text from */
				if (from) {
					usage();
					(void)fputs("multiple 'z' options not allowed\n", stderr);
					exit(1);
				}
				from = argv[i] + j + (argv[i][j+1] == '=' ? 2 : 1);
				j = strlen(argv[i]);
				break;

			default:
				usage();
				(void)fprintf(stderr, "unknown option '%c'\n", argv[i][j]);
				exit(1);
				}
	else
		strnum++;     /* count number of strings */

	if (strnum == 0 && from == NULL) {
		usage();
#ifdef OSK
		exit(_errmsg(1, "no string given\n"));
#else
		(void)fputs("no string given\n", stderr);
		exit(1);
#endif
	}

	if (strnum && from) {
		usage();
#ifdef OSK
		exit(_errmsg(1, "'z' option not allowed if string(s) given\n"));
#else
		(void)fputs("'z' option not allowed if string(s) given\n", stderr);
		exit(1);
#endif
	}

	if (from) {
		if (from[0]) {
			if ((infp = fopen(from, "r")) == NULL) {
#ifdef OSK
				exit(_errmsg(errno, "can't open '%s'\n", from));
#else
				(void)fprintf(stderr, "can't open '%s'\n", from);
				exit(errno);
#endif
			}
		}
		else
			infp = stdin;
	}

#ifdef OSK
	if (plife && !strcmp(plife, "dont_kill_me"))
		intercept(sighandler);
#endif

	str	= strbuf;
	i	= 1;
	while (TRUE) {
		if (from) {	/* read strings from file/stdin */
			if (fgets(strbuf, sizeof(strbuf)-1, infp) == NULL)
				break;
			strbuf[strlen(strbuf)-1] = '\0';
		}
		else {		/* get strings from argument line */
			if ((str = argv[i++]) == NULL)
				break;
			if (str[0] == '-')
				continue;
		}
		/* call the banner subroutine */
		banner(stdout, str, &flags);
	}
	return 0;
}

/*
 * u s a g e
 */

usage()
{
	(void)fputs("Syntax: banner [<opts>] {<string>} [<opts>]\n", stderr);
	(void)fputs("Function: prints a banner to stdout\n", stderr);
	(void)fputs("Options:\n", stderr);
	(void)fputs("     -i         prints italic\n", stderr);
	(void)fputs("     -d         double size\n", stderr);
	(void)fputs("     -c=<char>  character\n", stderr);
	(void)fputs("     -s         use same character\n", stderr);
	(void)fputs("     -z         read strings from standard input\n", stderr);
	(void)fputs("     -z=<file>  read strings from <file>\n", stderr);
}
