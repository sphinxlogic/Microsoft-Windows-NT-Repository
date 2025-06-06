From riacs!eos!ames!mailrus!cwjcc!hal!ncoast!allbery Fri Oct 28 08:03:24 PDT 1988

Posting-number: Volume 5, Issue 7
Submitted-by: "Larry Campbell" <campbell@redsox.UUCP>
Archive-name: s5look

Here's a quickie public domain implementation of look.  It's not blazingly
fast, but it sure beats grep, it lints cleanly, and the price is right.

/*
 * look.c
 *
 *	<-xtx-*> cc -o look -O -s look.c
 *
 *	An implementation of the V7/Berkeley look(1) utility, which
 *	is annoyingly lacking from System V.  This code is in the public
 *	public domain.  You may use this code for any purpose, commercial
 *	or noncommercial, as long as you don't try to claim you wrote it.
 *
 *	This code hasn't been tuned;  it runs about half the speed of the
 *	V7 version.  That's good enough for me.
 *
 *	If you find bugs, please send me the fixes.
 *
 * Written October, 1988 by:
 *
 *	Larry Campbell
 *	The Boston Software Works, Inc.
 *	120 Fulton Street, Boston, MA 02109
 *	campbell@bsw.com
 ******************************************************************
 * usage:
 *
 *	look [-f] words file
 *
 *	Conducts a binary search in <file>, which must be sorted,
 *	for a line beginning with <words>.  The -f switch means
 *	ignore (fold) case.
 *
 *	If the input file is not sorted, the results are unpredictable.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

extern void
    perror(),
    exit();

char
    *words,
    *filename,
    buf[BUFSIZ];

int
    fold_flag,
    length;

FILE
    *f;

long
    left_offset,
    offset,
    right_offset,
    size,
    step;

/* Forward declarations */

void
    search(),
    start_of_line(),
    strlwr(),
    usage();

main(argc, argv)
char *argv[];
{
int
    i;

struct stat
    statb;

if (argc < 3 || argc > 4)
    usage();
fold_flag = 0;
if (argc == 4) 
    if (argv[1][0] == '-' && argv[1][1] == 'f')
	fold_flag = 1;
    else
	usage();
i = argc - 2;
words = argv[i];
length = strlen(words);
if (fold_flag)
    strlwr(words);

filename = argv[i + 1];

f = fopen(filename, "r");
if (f == NULL)
    perror("can't open input file");

if (fstat(fileno(f), &statb) < 0)
    perror("stat failed on input file");

size = statb.st_size;
if (size == 0)
    exit(0);

offset = size / 2;
step = offset / 2;
search();
return 0;
}

void
search()
{
int
    compare;

char
    buf2[BUFSIZ];

for (;;)
    {
    if (fseek(f, offset, 0) == -1)
	perror("fseek");
    start_of_line();
    compare = strncmp(words, buf, length);
    if (compare == 0)
	{
	while (offset > 0L)	/* back up to first nonmatching line */
	    {
	    offset -= left_offset;
	    start_of_line();
	    compare = strncmp(words, buf, length);
	    if (compare != 0)
		{
		if (fseek(f, ++offset, 0) == -1) /* skip nonmatching line */
		    perror("fseek");
		if (fgets(buf, BUFSIZ, f) == 0)	/* reload matching line */
		    perror("fgets");
		break;
		}
	    }
	for (;;)
	    {
	    (void) fputs(buf, stdout);
	    if (fgets(buf, BUFSIZ, f) == 0)
		break;
	    (void) strcpy(buf2, buf);
	    if (fold_flag)
		strlwr(buf2);
	    if (strncmp(words, buf2, length) != 0)
		break;
	    }
	exit(0);
	}

    if (step == 0)
	exit(1);
    if (compare < 0)
	{
	if (step < left_offset)
	    offset -= left_offset;
	else
	    offset -= step;
	if (offset <= 0L)
	    exit(1);
	}
    else
	{
	if (step < right_offset)
	    offset += right_offset;
	else
	    offset += step;
	if (offset > size)
	    exit(1);
	}
    step = step / 2;
    }
}

void
start_of_line()
{
int
    c;

long
    toffset;

if (offset <= 0L)
    return;
toffset = offset;
while (toffset > 0L)
    {
    c = fgetc(f);
    if (c == '\n')
	break;
    if (fseek(f, --toffset, 0) == -1)
	perror("fseek");
    }
if (fgets(buf, BUFSIZ, f) == 0)
    perror("prev_line: fgets");
if (fold_flag)
    strlwr(buf);
left_offset = offset - toffset;
right_offset = (strlen(buf) - left_offset) + 1;
}


void
usage()
{
(void) fprintf(stderr, "usage: look [-f] string file\n");
exit(2);
}


/*
 * This function is in ANSI C but AT&T is still behind the times.
 */

void
strlwr(s)
char *s;
{
register char
    c;

while (c = *s)
    {
    if (isupper(c))
	*s = tolower(c);
    s++;
    }
}
-- 
Larry Campbell                          The Boston Software Works, Inc.
campbell@bsw.com                        120 Fulton Street
wjh12!redsox!campbell                   Boston, MA 02146


