/*
 *
 * Uuencode -- encode a file so that it's printable ascii, short lines
 *
 * Slightly modified from a version posted to net.sources a while back,
 * and suitable for compilation on the IBM PC
 *
 */

#include "stdio.h"
#include "errno.h"

char *progname = "uuencode";

#define USAGE "Usage: uuencode file [outfile]\n"

/* ENC is the basic 1 character encoding function to make a char printing */
#define ENC(c) (((c) & 077) + ' ')

main(argc, argv)
	int argc; char *argv[];
	{
	FILE *in, *out; int mode;
	if (argc < 2) {
		fprintf(stderr, USAGE);
		exit(2);
		}
	in = fopen(argv[1], "r");
	if  (argc < 3)  {
		if  ((out = stdout) == NULL)
			out = fopen(stderr, "w");
		}
	else	out = fopen(argv[2], "w"); /*szm:09nov1990: argv[3] ->argv[2]*/

	if  (out == NULL)  {
	    perror("uuencode");
	    exit(1);
	}

	fprintf(out, "begin %o %s\n", 0777, argv[1]);
	encode(in, out);
	fprintf(out, "end\n");
	fclose(out);
	}

/*
 * copy from in to out, encoding as you go along.
 */

encode(in, out)
	FILE *in, *out;
	{
	char buf[80];
	int i, n;
	for (;;) {
		n = fr(in, buf, 45);
		fputc(ENC(n), out);
		for (i=0; i<n; i += 3)
			outdec(&buf[i], out);
		fputc('\n', out);
		if (n <= 0)
			break;
		}
	}

/*
 * output one group of 3 bytes, pointed at by p, on file f.
 */

outdec(p, f)
	char *p; FILE *f;
	{
	int c1, c2, c3, c4;
	c1 = *p >> 2;
	c2 = (*p << 4) & 060 | (p[1] >> 4) & 017;
	c3 = (p[1] << 2) & 074 | (p[2] >> 6) & 03;
	c4 = p[2] & 077;
	fputc(ENC(c1), f);
	fputc(ENC(c2), f);
	fputc(ENC(c3), f);
	fputc(ENC(c4), f);
	}

/* fr: like read but stdio */

int fr(fd, buf, cnt)
	FILE *fd; char *buf; int cnt;
	{
	int c, i;
	for (i = 0; i < cnt; i++) {
		c = fgetc(fd);
		if (c == EOF)
			return(i);
		buf[i] = c;
		}
	return (cnt);
	}
