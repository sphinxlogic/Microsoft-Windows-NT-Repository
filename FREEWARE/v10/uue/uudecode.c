/*
 * Uudecode -- decode a uuencoded file back to binary form.
 *
 * Slightly modified from a version posted to net.sources once;
 * suitable for compilation on an IBM PC.
 *
 */

#include <stdio.h>

char *progname = "UUDECODE";

#define USAGE "Usage: UUDECODE [file]\n"

/* single character decode */
#define DEC(c)	(((c) - ' ') & 077)

main(argc, argv)
	int argc; char *argv[];
	{
	static  FILE *in, *out;
	int mode;
	char dest[128];
	char buf[80];
	/* optional input arg */
	if (argc > 1) {
		in = fopen(argv[1], "r");
		argv++; argc--;
		}
	else
		in = stdin;
	if (argc != 1) {
		fprintf(stderr, USAGE);
		exit(2);
		}
	/* search for header line */
	for (;;) {
		if (fgets(buf, sizeof buf, in) == NULL) {
			fprintf(stderr, "No begin line\n");
			exit(3);
			}
		if (strncmp(buf, "begin ", 6) == 0)
			break;
		}
	sscanf(buf, "begin %o %s", &mode, dest);
	out = fopen(dest, "w", "alq=300", "deq=100");  /* create output file */
	decode(in, out);
	fclose(out);
	if (fgets(buf, sizeof buf, in) == NULL || strcmp(buf, "end\n")) {
		fprintf(stderr, "No end line\n");
		exit(5);
		}
	}

/*
 * copy from in to out, decoding as you go along.
 */

decode(in, out)
	FILE *in, *out;
	{
	char buf[80];
	char *bp;
	int n;
	for (;;) {
		if (fgets(buf, sizeof buf, in) == NULL) {
			fprintf(stderr, "Short file\n");
			break;
			}
		n = DEC(buf[0]);
		if (n <= 0)
			break;
		bp = &buf[1];
		while (n > 0) {
			outdec(bp, out, n);
			bp += 4;
			n -= 3;
			}
		}
	}

/*
 * output a group of 3 bytes (4 input characters).
 * the input chars are pointed to by p, they are to
 * be output to file f.  n is used to tell us not to
 * output all of them at the end of the file.
 */

outdec(p, f, n)
	char *p; FILE *f; int n;
	{
	int c1, c2, c3;
	c1 = DEC(*p) << 2 | DEC(p[1]) >> 4;
	c2 = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
	c3 = DEC(p[2]) << 6 | DEC(p[3]);
	if (n >= 1)
		putc(c1, f);
	if (n >= 2)
		putc(c2, f);
	if (n >= 3)
		putc(c3, f);
	}


/* fr: like read but stdio */

int fr(fd, buf, cnt)
	FILE *fd; char *buf; int cnt;
	{
	int c, i;
	for (i = 0; i < cnt; i++) {
		c = getc(fd);
		if (c == EOF)
			return(i);
		buf[i] = c;
		}
	return (cnt);
	}

/* If your library already has this function, use it and nuke the code below */

#ifdef noindex
/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

char *index(sp, c)
	register char *sp, c;
	{
	do {
		if (*sp == c)
			return(sp);
		} while (*sp++);
	return(NULL);
	}
#endif
