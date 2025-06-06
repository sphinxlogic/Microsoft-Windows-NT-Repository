/*
**  GETOPT PROGRAM AND LIBRARY ROUTINE
**
**  I wrote main() and AT&T wrote getopt() and we both put our efforts into
**  the public domain via mod.sources.
**	Rich $alz
**	Mirror Systems
**	(mirror!rs, rs@mirror.TMC.COM)
**	August 10, 1986
** 
**  This is the public-domain AT&T getopt(3) code.  Hacked by Rich and by Jim.
*/

#include <stdio.h>
#ifdef VMS
#include <decw$include/Xos.h>
#include "vmsutil.h"
#else
#include <X11/Xos.h>
#endif /* vax11c */

#ifndef vax11c
#define ERR(_s, _c) { if (opterr) fprintf (stderr, "%s%s%c\n", argv[0], _s, _c);}
#else
#define ERR(_s, _c) { if (opterr) VMSERR(argv[0], _s, _c);}
#endif /* !vax11c */

int	opterr = 1;
int	optind = 1;
int	optopt;
char	*optarg;

#ifdef vax11c
static void VMSERR(prog, s, c)
char *prog;
char *s;
char c;
{
   char *ep;
   char *p;

   p = strrchr(prog, ']');
   p++;
   ep = strchr(p, '.');
   if (ep != NULL) *ep = '\0';

   fprintf (stderr, "%s%s%c\n", p, s, c);
}
#endif /* vax11c */

int
getopt(argc, argv, opts)
int	argc;
char	**argv, *opts;
{
	static int sp = 1;
	register int c;
	register char *cp;

	if(sp == 1)
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
#ifdef __DECC
		else if(strcmp(argv[optind], "--") == 0) {
#else
		else if(strcmp(argv[optind], "--") == NULL) {
#endif /* __DECC */
			optind++;
			return(EOF);
		}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=(char *)index(opts, (char *)c)) == NULL) {
		ERR(": illegal option -- ", c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			ERR(": option requires an argument -- ", c);
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}
