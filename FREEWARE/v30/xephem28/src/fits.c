/* code to read a basic FITS file.
 * we only handle files with SIMPLE=T, BITPIX=8, NAXIS=2 files.
 *
 * (Contact the author, Elwood Downey, ecdowney@noao.edu, for a complete X/Motif
 * 16-bit FITS viewer.)
 */

#include <stdio.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#include <string.h>
#else
extern void *malloc();
#endif

#include "P_.h"

extern FILE *fopenh P_((char *name, char *how));
extern void xe_msg P_((char *msg, int app_modal));
extern char *syserrstr P_((void));

#define	FITSROWL	80	/* chars per FITS header row */
#define	FITSNROWS	36	/* number of rows in FITS header */

/* read the given FITS file, flipping rows as per the FITS convention so [0,0]
 *   is the upper left corner as per the X convention..
 * if ok, fill in pixels and size pointers and return 0, else report why not
 *   using xe_msg(,1) and return -1.
 * N.B. caller must free *pixp if we return 0.
 */
int
fits_read (fn, pixp, nrp, ncp)
char *fn;		/* file name */
unsigned char **pixp;	/* where to save what we malloc for pixels */
int *nrp, *ncp;		/* where we save number of rows and cols */
{
	char msg[1024];
	char fitsrow[FITSROWL];
	unsigned char *pix;
	FILE *fp;
	int sawsimple;
	int nrows, ncols;
	int npix;
	int sawend;
	int i;

	/* open the file */
	fp = fopenh (fn, "r");
	if (!fp) {
	    (void) sprintf (msg, "%s: %s", fn, syserrstr());
	    xe_msg (msg, 1);
	    return (-1);
	}

	/* read the header */
	sawend = sawsimple = nrows = ncols = 0;
	for (i = 0; i < FITSNROWS; i++) {
	    /* read a row */
	    if (fread (fitsrow, 1, FITSROWL, fp) != FITSROWL) {
		(void) sprintf (msg, "%s: header is short", fn);
		xe_msg (msg, 1);
		(void) fclose (fp);
		return (-1);
	    }

	    /* crack it */
	    if (sawend) {
		continue;
	    } else
	    if (!strncmp (fitsrow, "SIMPLE  =                    T", 30)) {
		sawsimple = 1;
	    } else
	    if (!strncmp (fitsrow, "BITPIX ", 7)) {
		int bitpix = atoi (fitsrow+9);
		if (bitpix != 8) {
		    (void) sprintf (msg, "%s: BITPIX must be 8 but is %d", fn,
									bitpix);
		    xe_msg (msg, 1);
		    (void) fclose (fp);
		    return (-1);
		}
	    } else
	    if (!strncmp (fitsrow, "NAXIS  ", 7)) {
		int naxis = atoi (fitsrow+9);
		if (naxis != 2) {
		    (void) sprintf (msg, "%s: NAXIS must be 2 but is %d", fn,
								    naxis);
		    xe_msg (msg, 1);
		    (void) fclose (fp);
		    return (-1);
		}
	    } else
	    if (!strncmp (fitsrow, "NAXIS1 ", 7)) {
		nrows = atoi (fitsrow+9);
	    } else
	    if (!strncmp (fitsrow, "NAXIS2 ", 7)) {
		ncols = atoi (fitsrow+9);
	    } else
	    if (!strncmp (fitsrow, "END", 3)) {
		sawend = 1;
	    }
	}

	/* sanity check */
	if (!sawsimple) {
	    (void) sprintf (msg, "%s: image is not SIMPLE", fn);
	    xe_msg (msg, 1);
	    (void) fclose (fp);
	    return (-1);
	}
	if (!sawend) {
	    (void) sprintf (msg, "%s: no END", fn);
	    xe_msg (msg, 1);
	    (void) fclose (fp);
	    return (-1);
	}
	if (nrows <= 0 || ncols <= 0) {
	    (void) sprintf (msg, "%s: bogus NAXIS1=%d and/or NAXIS2=%d", fn,
								nrows, ncols);
	    xe_msg (msg, 1);
	    (void) fclose (fp);
	    return (-1);
	}

	/* allocate a byte array in which to read the image */
	npix = nrows * ncols;
	pix = (unsigned char *) malloc (npix);
	if (!pix) {
	    (void) fclose (fp);
	    xe_msg ("No memory for moon pixels.", 1);
	    return (-1);
	}

	/* read the image pixels, reversing rows as we go.
	 * the header has already been skipped.
	 */
	for (i = nrows; --i >= 0; ) {
	    if (fread (pix + i*ncols, 1, ncols, fp) != ncols) {
		(void) sprintf (msg, "%s: too few pixels", fn);
		xe_msg (msg, 1);
		(void) fclose(fp);
		free ((void *)pix);
		return (-1);
	    }
	}

	/* finished with fp */
	(void) fclose (fp);

	/* ok! */
	*pixp = pix;
	*nrp = nrows;
	*ncp = ncols;
	return (0);
}
