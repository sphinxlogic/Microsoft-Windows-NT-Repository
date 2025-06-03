#module strings "v1.3"

/*
 * strings - quick-and-dirty hack to locate strings in
 *           a VMS .exe file; patterned after the Unix
 *	     program of the same name.  
 *
 * A 'string' is a sequence of printing ASCII characters
 * terminated by null or newline.  In order to avoid
 * picking up too much trash, strings shorter than a certain
 * minimum length (default 4) are ignored.
 *
 * The definition of a string means that this program is
 * only useful on images that obey C's idea of string storag.
 *
 * Dave Porter  7-Feb-1991
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stat.h>
#include <limits.h>

#define BLKSZ 512
#define BUFSZ (4*BLKSZ)

int min = 4;		/* minimum string length */
int oflg = 0;		/* flag: print offset */
int aflg = 0;		/* flag: scan all file */
char *fspec= "<stdout>";/* output file spec */
FILE *outfp = NULL;	/* output file pointer */

main(int argc, char **argv)
{
    char *c;		/* scratch */
    int i;		/* arg pointer */
    int f = 0;		/* files processed */

    /*
     * -options
     */
    for (i=1; i<argc; i++) 
	if (argv[i][0] == '-') {
	    c = &argv[i][1];
	    while (*c) 
	        if (isdigit(*c)) {
	            min = strtol(c, &c, 10);
		    if (min <= 0) min = 1;
	        }
	        else
	            switch (*c++) {
		        case 'a': aflg = 1;
		 	          break;
		        case 'o': oflg = 1;
			          break;
		        default:  goto quit;
	    	    }
	}

    /*
     * >output              
     */
    for (i=1; i<argc; i++)
	if (argv[i][0] == '>') {
	    if (outfp != NULL) goto quit;
	    fspec = &argv[i][1];
	    outfp = fopen(fspec, "w");
	    if (outfp == NULL) {
	        perror(fspec);
	        return (-1);
	    }
        }
    if (outfp == NULL)
	outfp = stdout;


    /*
     * filespecs
     */
    for (i=1; i<argc; i++)
	if (argv[i][0] != '-' && argv[i][0] != '>') {
	    strings(argv[i]);
	    f++;
        }

quit:
    /*
     * Close output file if necessary 
     */
    if (outfp != NULL && outfp != stdout) {
	fclose(outfp);
	outfp = NULL;
    }

    /*
     * Any syntax error?
     */
    if (f==0) {
	fprintf(stderr, "Usage:  strings [-ao] [-#] [>output] file...\n");
     	fprintf(stderr, "           a  scan all file\n"); 
	fprintf(stderr, "           o  display file offset (octal)\n");
	fprintf(stderr, "           #  minimum string length\n");
	return (-1);
    }

}

/*
 * Process one file
 */
strings(char *file)
{
    FILE *fp;
    char buff[BUFSZ+1];                	
    int offs = 0;
    int endoff = INT_MAX;
    int leftover = 0;

    /*
     * Open image file...
     */
    fp = fopen(file, "rb");
    if (fp == NULL) {
	perror(file);
	return (-1);
    }

    /*
     * Unless we're supposed to process the whole file, check that
     * file has fixed-512-byte records.  If that's the case then
     * scan the image header to find out where the image starts.
     * If the file format is wrong or we don't understand the header, 
     * carry on anyway (as if -a was specified for this file).
     */
    if (!aflg) {
        stat_t stat;
        fstat(fileno(fp), &stat);
    	if (stat.st_fab_rfm == 1 && stat.st_fab_rat == 0 &&
            stat.st_fab_fsz == 0 && stat.st_fab_mrs == BLKSZ) {
	    if (header(fp, &offs, &endoff) < 0) 
	        fprintf(stderr, "%s: bad image header\n", file);
	    fseek(fp, offs, SEEK_SET);
	}
	else
	    fprintf(stderr, "%s: not fixed-length 512-byte records\n", file);
    }

    /*
     * Process image contents.  Note, usually 'leftover' is zero.
     * If we find a tentative string which falls of the end of the 
     * buffer, however, we loop back here with 'leftover' set to the 
     * number of bytes we retained (moved to the start of the buffer).
     */
    fprintf(outfp, "%s\n", file);
    while (offs+leftover < endoff) {
        char *p, *q;
	int len;

	/*
	 * Read next chunk of input file (always read a multiple
	 * of the block size).
	 */
	len = leftover ? ((BUFSZ-leftover)/BLKSZ)*BLKSZ : BUFSZ;
	if (len > endoff-offs-leftover) len = endoff-offs-leftover;
        len = fread(buff+leftover, 1, len, fp);
	if (len <= 0) break;
	len += leftover;
	buff[len] = '\0';
	leftover = 0;

        /*
	 * Scan buffer
	 */
	p = buff;
	while (p < buff+len) {
	
	    /*
	     * Determine longest run of printable characters.
	     * Note there's a null after the buffer to prevent
	     * runaway searches.
	     */
	    q = p;
	    while (isascii(*q) && isprint(*q))
		q++;

	    /*
 	     * If we fell off the end of the buffer, shuffle the last
	     * chars to the top of the buffer, saving the leftover
	     * length so the file-read code can figure it out.
	     */
	    if (q >= buff+len && q-p <= BUFSZ-BLKSZ) {
	        leftover = q - p;
		memcpy(buff, p, leftover);
		break;
	    }

	    /* 
	     * If the run of printable characters ends with
	     * a null or a newline, then we deem it to be a string.
	     */
	    if (q-p >= min && (*q == '\0' || *q == '\n')) {
		*q = '\0';
		if (oflg)
		    fprintf(outfp, "%o: %s\n", offs+(p-buff), p);
		else
		    fprintf(outfp, "%s\n", p);
	    }

	    /*
	     * Skip string
	     */
	    p = q+1;
	}

	/*
	 * Account for what we consumed
	 */
	offs += len-leftover;
    }

    /*
     * Success
     */
    fclose(fp);
    return (0);

}

/*
 * Process image header.  We scan all ISDs to determine the lowest
 * and highest blocks which contain the image itself.   This
 * routine contains a lot of kludgery; contrariwise, all the
 * kludgery is contained here.
 */                                     
header(FILE *fp, int *start, int *end)
{
#define xtract(type, ptr)  (*(type *)(ptr))
    char smallbuff[BLKSZ];
    char *bigbuff = NULL;
    char *p, *q;
    int hdrlen;
    int len;
    int lo = INT_MAX, hi = 0;

    /*
     * Read first block of header, extract header block count
     */
    len = fread(smallbuff, 1, BLKSZ, fp);
    if (len != BLKSZ) return (-1);
    p = smallbuff;
    hdrlen = xtract(unsigned char, p+16) * BLKSZ;

    /*
     * If header is more than one block, read the whole thing
     */
    if (hdrlen > BLKSZ) {
	bigbuff = malloc(hdrlen);
	if (bigbuff == NULL) return (-1);
	fseek(fp, 0, SEEK_SET);
	len = fread(bigbuff, 1, hdrlen, fp);
	if (len != hdrlen) {
	    free(bigbuff);
	    return (-1);
	}
        p = bigbuff;
    }

    /*
     * Scan through all image section descriptors.   Hack - ignore 
     * fixup vectors (flags&1024) since we aren't interested in them.
     */
    q = p + hdrlen;			/* end pointer */
    p += xtract(short, p);		/* point past 1st rec to isd array */
    while (p < q && (len = xtract(short,p)) > 0) {
        if (len >= 16) {
            int pagcnt, vbn, flags;
	    pagcnt = xtract(short,p+2);	/* page count for this section */
	    flags  = xtract(int,p+8);	/* ISD flags */
	    vbn    = xtract(int,p+12);	/* base VBN in file */
	    if (vbn != 0 && pagcnt != 0 && (flags&1024) == 0) {
		if (vbn < lo) lo = vbn;
		if (vbn+pagcnt > hi) hi = vbn+pagcnt;
	    }
	}
	p += len;
    }

    /*
     * Clean up, return results 
     */
    if (bigbuff != NULL)
	free(bigbuff);

    if (lo > hi)
	return (-1);

    *start = (lo-1) * BLKSZ;
    *end   = (hi-1) * BLKSZ;
    return (0);

}
