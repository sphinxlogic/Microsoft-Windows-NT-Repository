/* FITS file handling utilities.
 * each pixel in the file is 2-bytes, signed, big-endian;
 * first pixel in file is lower-left of scene.
 * in memory, we store them in 2-bytes, unsigned, native byte oder.
 */

#include <stdio.h>

#if defined(__STDC__)
#include <stdlib.h>
#include <string.h>
#else
extern void *malloc(), *realloc();
#endif

#if defined(_POSIX_SOURCE)
#include <unistd.h>
#else
extern int read();
extern int write();
#endif


#include <math.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#include "P_.h"
#include "fits.h"
#include "net.h"

extern char *syserrstr P_((void));
extern void pm_set P_((int percentage));

static int writeFITSHeader P_((FImage *fip, int fd, char *errmsg));
static int findFImageVar P_((FImage *fip, char *name, char **rpp));
static void fmtLogicalFITS P_((FITSRow line, char *name, int value,
    char *comment));
static void fmtIntFITS P_((FITSRow line, char *name, int value, char *comment));
static void fmtRealFITS P_((FITSRow line, char *name, double value, int sigdig,
    char *comment));
static void fmtStringFITS P_((FITSRow line, char *name, char *value,
    char *comment));
static void fmtENDFITS P_((FITSRow line));
static void fmtInlineComment P_((FITSRow line, char *comment));
static void enFITSPixels P_((FImage *fip));
static void unFITSPixels P_((FImage *fip));
static int lendian P_((void));

/* write out the given fip to file descriptor fd.
 * we assume fip->var contains all the fields we will need; all we do is
 *   add END and pad with blanks to a multiple of FITS_HROWS*FITS_HCOLS.
 *   N.B. we don _not_ modify the original var list.
 * we assume fip->image points to an array of fip->sw * fip->sh
 *   unsigned short pixels, with the first pixel in the upper left of the scene.
 *   we force them into the form required by FITS before writing them out. we do
 *   this IN PLACE. if restore is 0, we DO NOT put the pixels back the way we
 *   found them.
 * return 0 if ok, else put a short message into errmsg and return -1.
 */
int
writeFITS (fd, fip, errmsg, restore)
int fd;
FImage *fip;
char *errmsg;
int restore;
{
	int nbytes, n, nw;

	if (!fip->image) {
	    sprintf (errmsg, "No pixels :-(");
	    return (-1);
	}

	if (writeFITSHeader (fip, fd, errmsg) < 0)
	    return (-1);

	/* format the pixels */
	if (fip->bitpix == 16)
	    enFITSPixels(fip);

	/* write the pixels.
	 * might be a pipe so keep writing until eof or error
	 */
	nbytes = fip->sw * fip->sh * (fip->bitpix/8);
	for (nw = 0; nw < nbytes; nw += n) {
	    n = write (fd, fip->image+nw, nbytes-nw);
	    if (n <= 0) {
		if (nw < 0)
		    strcpy (errmsg, syserrstr());
		else
		    sprintf (errmsg, "Short write of FITS pixels");
		if (restore && fip->bitpix == 16)
		    unFITSPixels (fip);
		return (-1);
	    }
	}

	/* ok */

	if (restore && fip->bitpix == 16)
	    unFITSPixels (fip);

	return (0);
}

/* read the given FITS file, filling in fields in fip and mallocing as needed.
 * all header lines are copied to fip->var UP TO BUT NOT INCLUDING "END".
 * we assume the pixels in the file are in standard FITS format and we convert
 *   them to 2-byte unsigned native byte order with the first pixel in the upper
 *   left.
 * return 0 if ok, else put a short message into errmsg and return -1.
 */
int
readFITS (fd, fip, errmsg)
int fd;
FImage *fip;
char *errmsg;
{
	int s;

	if (readFITSHeader (fd, fip, errmsg) < 0)
	    return (-1);

	/* get some memory for the pixels */
	fip->totbytes = fip->sw * fip->sh * (fip->bitpix/8);
	fip->image = malloc (fip->totbytes);
	if (!fip->image) {
	    sprintf (errmsg, "Could not malloc %d for pixels", fip->totbytes);
	    resetFImage (fip);
	    return (-1);
	}

	/* now read the pixels.
	 * might be a pipe so keep reading until eof or error
	 */
	for (fip->nbytes = 0; fip->nbytes < fip->totbytes; fip->nbytes += s) {
	    pm_set (fip->nbytes*100/fip->totbytes);

	    s= read (fd, fip->image + fip->nbytes, fip->totbytes - fip->nbytes);
	    if (s <= 0) {
		if (s < 0)
		    strcpy (errmsg, syserrstr ());
		else
		    sprintf (errmsg, "data is short");
		resetFImage (fip);
		return (-1);
	    }
	}

	/* get byte order correct, if 16 bits */
	if (fip->bitpix == 16)
	    unFITSPixels (fip);

	/* ok */
	return (0);
}

/* read another chunk of the given FITS file, filling in fields in fip and
 *   mallocing as needed.
 * all header lines are copied to fip->var UP TO BUT NOT INCLUDING "END".
 * we assume the pixels in the file are in standard FITS format and we convert
 *   them to 2-byte unsigned native byte order with the first pixel in the upper
 *   left.
 * return 0 if ok, else put a short message into errmsg and return -1.
 * N.B. we use recv(), not read() as this is intended strictly for network use.
 */
int
readIncFITS (fd, fip, errmsg)
int fd;
FImage *fip;
char *errmsg;
{
#define	READCHUNK	1024

	int s;
	int n;

	/* if first time, malloc all the pixel memory */
	if (fip->totbytes == 0) {
	    fip->totbytes = fip->sw * fip->sh * (fip->bitpix/8);
	    fip->image = malloc (fip->totbytes);
	    if (!fip->image) {
		sprintf (errmsg,"Could not malloc %d for pixels",fip->totbytes);
		resetFImage (fip);
		return (-1);
	    }
	    fip->nbytes = 0;
	}

	/* now read up to a chunk more of pixels.  */
	n = fip->totbytes - fip->nbytes;
	if (n > READCHUNK)
	    n = READCHUNK;
	s = recvbytes (fd, (unsigned char *) (fip->image + fip->nbytes), n);
	if (s != n) {
	    strcpy (errmsg, syserrstr ());
	    resetFImage (fip);
	    return (-1);
	}
	fip->nbytes += s;

	/* if complete, get byte order correct, if 16 bits */
	if (fip->nbytes == fip->totbytes && fip->bitpix == 16)
	    unFITSPixels (fip);

	/* ok */
	return (0);

#undef READCHUNK
}

/* read the given FITS file header into fip; we don't read the pixels.
 * N.B. we call initFImage(fip) and also resetFImage(fip) if there is an error.
 * return 0 if ok, else put message into errmsg and return -1.
 */
int
readFITSHeader (fd, fip, errmsg)
int fd;
FImage *fip;
char errmsg[];
{
	static char simple[] = "SIMPLE  =                    T";
	FITSRow row;
	int nrows;
	int sawend;
	int n1, n2;
	int s;
	int i;

	initFImage (fip);

	/* read header lines until we see END
	 * and have digested a whole number of blocks.
	 */
	nrows = 0;
	sawend = 0;
	do {
	    s = recvbytes (fd, (unsigned char *) row, sizeof(row));
	    if (s != sizeof(row)) {
		if (s < 0)
		    strcpy (errmsg, syserrstr ());
		else
		    sprintf (errmsg, "FITS file is short");
		goto err;
	    }

	    if (nrows == 0 && strncmp (row, simple, sizeof(simple)-1) != 0) {
		sprintf (errmsg, "Not a FITS file");
		goto err;
	    }

	    nrows++;

	    /* add the row to fip unless we've seen or see END */
	    if (!sawend && !(sawend = strncmp (row, "END", 3) == 0))
		addFImageVar (fip, row);

	} while (!sawend || (nrows%FITS_HROWS));

	/* crack the required fields into fip 
	 * and check for required conditions
	 */

	if (getLogicalFITS (fip, "SIMPLE", &i) < 0 || !i) {
	    sprintf (errmsg, "File must claim to be a SIMPLE image.");
	    goto err;
	}

	if (getIntFITS (fip, "BITPIX", &i) < 0 || (i != 16 && i != 8)) {
	    sprintf (errmsg, "BITPIX must be 8 or 16.");
	    goto err;
	}
	fip->bitpix = i;

	if (getNAXIS (fip, &n1, &n2, errmsg) < 0)
	    goto err;

	fip->sw = n1;
	fip->sh = n2;

	return (0);

    err:
	resetFImage (fip);
	return (-1);
}

/* write a nominal FITS-format file of pixels file to fd.
 * we assume pix points to an array of w*h unsigned short pixels, with the
 *   first pixel in the upper left of the scene. thus, we force them into the
 *   form required by FITS before writing them out. we do this IN PLACE. if 
 *   restore is 0, we DO NOT put the pixels back the way we found them.
 * return 0 if ok else -1 if error.
 */
int
writeSimpleFITS (fd, pix, w, h, restore)
int fd;
char *pix;
int w, h;
int restore;
{
	char errmsg[1024];
	FImage fimage;

	initFImage (&fimage);

	fimage.sw = w;
	fimage.sh = h;
	fimage.bitpix = 16;
	fimage.image = pix;

	setSimpleFITSHeader (&fimage);
	
	return (writeFITS (fd, &fimage, errmsg, restore));
}

/* get NAXIS1 and NAXIS2 from fip.
 * return 0 if ok else fill errmsg and return -1.
 * we also require that if NAXISi, with i > 2, exist they be 1.
 */
int
getNAXIS (fip, n1p, n2p, errmsg)
FImage *fip;
int *n1p, *n2p;
char errmsg[];
{
	int n;
	int i;

	if (getIntFITS (fip, "NAXIS", &n) < 0) {
	    sprintf (errmsg, "No NAXIS");
	    return (-1);
	}


	/* check for higher dimensions */
	for (i = 3; i <= n; i++) {
	    char naxisi[16];
	    int ni;

	    sprintf (naxisi, "NAXIS%d", i);
	    if (getIntFITS (fip, naxisi, &ni) < 0) {
		sprintf (errmsg, "NAXIS=%d but no %s", n, naxisi);
		return (-1);
	    }
	    if (ni != 1) {
		sprintf (errmsg, "Require %s to be 1", naxisi);
		return (-1);
	    }
	}

	if (getIntFITS (fip, "NAXIS1", n1p) < 0) {
	    sprintf (errmsg, "No NAXIS1");
	    return (-1);
	}

	if (getIntFITS (fip, "NAXIS2", n2p) < 0) {
	    sprintf (errmsg, "No NAXIS2");
	    return (-1);
	}

	return (0);
}

/* write fip->var then add END and pad to FITS block size.
 * if trouble put message in errmsg and return -1, else return 0.
 */
static int
writeFITSHeader (fip, fd, errmsg)
FImage *fip;
int fd;
char *errmsg;
{
	int nvar = fip->nvar;	/* handy */
	char *hdr;
	int npad;
	int nbytes, nw;

	/* find number of extra rows we need to add to nvar to make it a
	 * multiple of FITS_NROWS, _NOT_ including the row we add for END.
	 */
	npad = (FITS_HROWS - ((nvar+1)%FITS_HROWS)) % FITS_HROWS;

	/* get room for the var, END and pad rows */
	nbytes = (nvar + 1 + npad) * sizeof(FITSRow);
	hdr = malloc (nbytes);
	if (!hdr) {
	    sprintf(errmsg, "Can not malloc %d for padded FITS header", nbytes);
	    return (-1);
	}

	/* copy the existing fields in var */
	memcpy (hdr, (char *)fip->var, nvar*sizeof(FITSRow));

	/* add the END keyword */
	fmtENDFITS (&hdr[nvar*sizeof(FITSRow)]);

	/* pad out the remaining lines with blanks */
	memset (&hdr[(nvar+1)*sizeof(FITSRow)], ' ', npad*sizeof(FITSRow));

	/* write out the header: var + END + pad */
	nbytes = (nvar + 1 + npad) * sizeof (FITSRow);
	nw = write (fd, hdr, nbytes);
	free (hdr);
	if (nw != nbytes) {
	    if (nw < 0)
		strcpy (errmsg, syserrstr ());
	    else
		sprintf (errmsg, "Short write of FITS header");
	    return (-1);
	}

	return (0);
}

/* initialize each field of fip to a default.
 */
void
initFImage (fip)
FImage *fip;
{
	memset ((char *)fip, 0, sizeof(*fip));
	fip->totbytes = fip->nbytes = 0;
}

/* prepare a fip for reuse.
 * if it already has memory in use, free it also.
 * N.B. don't pass one right off the stack because we free memory when we find
 *    pointer fields that are non-0; use initFImage for that.
 */
void
resetFImage (fip)
FImage *fip;
{
	if (fip->var)
	    free ((char *)fip->var);
	if (fip->image)
	    free (fip->image);

	initFImage (fip);
}

/* add the basic FITS fields to fip.
 * we assume fip->var is empty and we do not add END or pad.
 */
void
setSimpleFITSHeader (fip)
FImage *fip;
{
	setLogicalFITS (fip, "SIMPLE", 1, "Standard FITS");
	setIntFITS (fip, "BITPIX", fip->bitpix, "Bits per pixel");
	setIntFITS (fip, "NAXIS", 2, "Number of dimensions");
	setIntFITS (fip, "NAXIS1", fip->sw, "Number of columns");
	setIntFITS (fip, "NAXIS2", fip->sh, "Number of rows");
	setRealFITS (fip, "BZERO", 32768.0, 6, "Real = Pixel*BSCALE + BZERO");
	setRealFITS (fip, "BSCALE", 1.0, 6, "Pixel scale factor");
}

/* add the Logical field v to fip->var */
void
setLogicalFITS (fip, name, v, comment)
FImage *fip;
char *name;
int v;
char *comment;
{
	char *rp;

	if (findFImageVar (fip, name, &rp) == 0)
	    fmtLogicalFITS (rp, name, v, comment);
	else {
	    FITSRow row;
	    fmtLogicalFITS (row, name, v, comment);
	    addFImageVar (fip, row);
	}
}

/* add the Integer field v to fip->var */
void
setIntFITS (fip, name, v, comment)
FImage *fip;
char *name;
int v;
char *comment;
{
	char *rp;

	if (findFImageVar (fip, name, &rp) == 0)
	    fmtIntFITS (rp, name, v, comment);
	else {
	    FITSRow row;
	    fmtIntFITS (row, name, v, comment);
	    addFImageVar (fip, row);
	}
}

/* add (or replace) the Real field v with so many significant digits */
void
setRealFITS (fip, name, v, sigdig, comment)
FImage *fip;
char *name;
double v;
int sigdig;
char *comment;
{
	char *rp;

	if (findFImageVar (fip, name, &rp) == 0)
	    fmtRealFITS (rp, name, v, sigdig, comment);
	else {
	    FITSRow row;
	    fmtRealFITS (row, name, v, sigdig, comment);
	    addFImageVar (fip, row);
	}
}

/* add a character string field to fip->var.
 * strings are represented by a ' in column 11 and closed by a ' not before
 *   col 20, i.e. 8 characters minimum including blanks.
 */
void
setStringFITS (fip, name, string, comment)
FImage *fip;
char *name;
char *string;
char *comment;
{
	char *rp;

	if (findFImageVar (fip, name, &rp) == 0)
	    fmtStringFITS (rp, name, string, comment);
	else {
	    FITSRow row;
	    fmtStringFITS (row, name, string, comment);
	    addFImageVar (fip, row);
	}
}

/* add the comment field to fip->var left justified in columns 9-80.
 * if too wide, break into several rows, all but first starting with "... ".
 * this is generally only used for names of HISTORY and COMMENT
 */
void
setCommentFITS (fip, name, comment)
FImage *fip;
char *name;
char *comment;
{
	char lline[FITS_HCOLS+1];	/* room for sprintf's trailing '\0' */
	int l = strlen(comment);
	int n;

	for (n = 0; n < l; ) {
	    if (n == 0) {
		sprintf (lline, "%-8.8s%-72.72s", name, comment);
		n += 72;
	    } else {
		sprintf (lline, "%-8.8s... %-68.68s", name, comment+n);
		n += 68;
	    }
	    addFImageVar (fip, lline);
	}
}

/* search fip->var for the given Logical field.
 * return 0 and set *vp if we find it, else return -1.
 */
int
getLogicalFITS (fip, name, vp)
FImage *fip;
char *name;
int *vp;
{
	char *rp;

	if (findFImageVar (fip, name, &rp) < 0)
	    return (-1);

	switch (rp[29]) {
	case 'T': case 't': *vp = 1; return (0);
	case 'F': case 'f': *vp = 0; return (0);
	default: return (-1);
	}
}

/* search fip->var for the given Integer field.
 * return 0 and set *vp if we find it, else return -1.
 */
int
getIntFITS (fip, name, vp)
FImage *fip;
char *name;
int *vp;
{
	char *rp;

	if (findFImageVar (fip, name, &rp) < 0)
	    return (-1);
	*vp = atoi (rp+10);
	return (0);
}

/* search fip->var for the given Real field.
 * return 0 and set *vp if we find it, else return -1.
 */
int
getRealFITS (fip, name, vp)
FImage *fip;
char *name;
double *vp;
{
	char buf[32];
	char *dp, *rp;

	if (findFImageVar (fip, name, &rp) < 0)
	    return (-1);
	memcpy (buf, rp+10, 30);
	buf[30] = '\0';
	if ((dp = strchr (buf,'D')) || (dp = strchr (buf,'d')))
	    *dp = 'e';
	*vp = atof (buf);
	return (0);
}

/* search fip->var for the given Comment field.
 * return 0 and fill in buf (including a trailing 0) if we find it, else
 *   return -1.
 * buf should be at least 73 chars long.
 */
int
getCommentFITS (fip, name, buf)
FImage *fip;
char *name;
char *buf;
{
	char *rp;

	if (findFImageVar (fip, name, &rp) < 0)
	    return (-1);
	memcpy (buf, rp, 72);
	rp[72] = '\0';
	return (0);
}

/* search fip->var for the given Character string field.
 * fill in buf and return 0 if we find name, else return -1.
 * buf will include a trailing 0, no ' and no trailing blanks.
 * string should be at least 69 chars long.
 */
int
getStringFITS (fip, name, string)
FImage *fip;
char *name;
char *string;
{
	char *string0 = string;
	char *rp;
	char c;
	int i;

	if (findFImageVar (fip, name, &rp) < 0)
	    return (-1);
	if (rp[10] != '\'')		/* should have a ' in column 11 */
	    return (-1);
	for (i = 11; i < FITS_HCOLS; i++) {
	    c = rp[i];
	    if (c == '\'') {		/* and another someplace later */
		while (string > string0 && string[-1] == ' ')
		    string--;
		*string = '\0';
		return (0);
	    } else
		*string++ = c;
	}
	return (-1);
}

/* search through var for an entry with the given name.
 * N.B. name should _not_ include trailing blanks.
 * if find it set *rpp to its address and return 0, else -1.
 */
static int
findFImageVar (fip, name, rpp)
FImage *fip;
char *name;
char **rpp;
{
	char field[9];	/* FITS field name */
	int i;

	sprintf (field, "%-8.8s", name);

	for (i = 0; i < fip->nvar; i++)
	    if (strncmp (field, fip->var[i], 8) == 0) {
		*rpp = fip->var[i];
		return (0);
	    }
	return (-1);
}

/* add the row to the end of the fip->var array.
 */
void
addFImageVar (fip, row)
FImage *fip;
FITSRow row;
{
	char *mem;
	int newn;

	newn = fip->nvar + 1;

	/* get room for one more FITSrow */
	if (fip->var)
	    mem = realloc ((char *)fip->var, newn*sizeof(FITSRow));
	else
	    mem = malloc (newn*sizeof(FITSRow));

	if (!mem) {
	    fprintf (stderr, "No memory for more FITS header lines\n");
	    return;
	}

	/* copy to the new (last) position */
	fip->var = (FITSRow *) mem;
	memcpy (fip->var[fip->nvar], row, FITS_HCOLS);
	fip->nvar = newn;
}

/* delete the given field from the FImage.
 * return 0 if ok, else -1 if field didn't exist.
 */
int
delFImageVar (fip, name)
FImage *fip;
char *name;
{
	char *rp;
	char *mem;
	FITSRow *dst;
	int newn;
	int i;

	if (!fip->var || findFImageVar (fip, name, &rp) < 0)
	    return (-1);

	newn = fip->nvar - 1;

	/* get room for one fewer FITSrow */
	mem = malloc (newn*sizeof(FITSRow));
	if (!mem) {
	    fprintf (stderr, "No memory for fewer FITS header lines (?!)\n");
	    return (-1);
	}

	/* copy all entries except the one at rp */
	dst = (FITSRow *) mem;
	for (i = 0; i < fip->nvar; i++) {
	    char *src = fip->var[i];
	    if (src != rp)
		memcpy ((void *)(dst++), (void *)(src), FITS_HCOLS);
	}

	free ((char *)fip->var);
	fip->var = (FITSRow *) mem;
	fip->nvar = newn;

	return (0);
}

/* copy the given field from fip2 to fip1.
 * return 0 if ok, else -1 if trouble.
 */
int
cpyFImageVar (fip1, fip2, name)
FImage *fip1, *fip2;
char *name;
{
	char *srcrow, *dstrow;

	if (!fip2->var || findFImageVar (fip2, name, &srcrow) < 0)
	    return (-1);
	if (fip1->var && findFImageVar (fip1, name, &dstrow) == 0)
	    memcpy (dstrow, srcrow, FITS_HCOLS);
	else
	    addFImageVar (fip1, srcrow);
	return (0);
}

/* given a name and a 0 or !0 write the logical FITS variable to line
 * as a F or T in column 30.
 */
static void
fmtLogicalFITS (line, name, value, comment)
FITSRow line;
char *name;
int value;
char *comment;
{
	sprintf (line, "%-8.8s=%20s%c", name, "", value ? 'T' : 'F');
	fmtInlineComment (line, comment);
}

/* given a name and an int value write the int FITS variable to line
 * right justified in columns 11-30.
 */
static void
fmtIntFITS (line, name, value, comment)
FITSRow line;
char *name;
int value;
char *comment;
{
	char str[30];
	int strl;

	sprintf (str, "%d", value);
	strl = strlen (str);

	sprintf (line, "%-8.8s= %*s%s", name, 20-strl, "", str);
	fmtInlineComment (line, comment);
}

/* given a name and a double write the floating point FITS variable to line
 * in columns 11-30 with at most sigdig significant digits.
 */
static void
fmtRealFITS (line, name, value, sigdig, comment)
FITSRow line;
char *name;
double value;
int sigdig;
char *comment;
{
	sprintf (line, "%-8.8s= %20.*G", name, sigdig, value);
	fmtInlineComment (line, comment);
}

/* given a name and a character string write the string FITS variable to line
 * represented by a ' in column 11 and closed by a ' not before col 20, i.e. 8
 * characters minimum including blanks.
 */
static void
fmtStringFITS (line, name, value, comment)
FITSRow line;
char *name;
char *value;
char *comment;
{
	char lline[FITS_HCOLS+1];	/* room for sprintf's trailing '\0' */
	int l = strlen(value);

	if (l < 8)
	    l = 8;
	else if (l > 68)
	    l = 68;

	sprintf (lline, "%-8.8s= '%-*.*s'%*s", name, l, l, value,  80-12-l, "");
	if (comment && l < 80-3-12) {
	    int start = 12+l;
	    if (start < 30)
		start = 30;
	    sprintf (&lline[start], " / %-*.*s", 80-3-start, 80-3-start,
								    comment);
	}

	memcpy (line, lline, FITS_HCOLS);
}

/* write the END marker to the FITS line */
static void
fmtENDFITS (line)
FITSRow line;
{
	sprintf (line, "%-79s", "END");
	line[79] = ' ';
}

/* attend to the final 50 chars of line */
static void
fmtInlineComment (line, comment)
FITSRow line;
char *comment;
{
	char buf[100]; /* buffer to allow for sprintf's trailing '\0' */

	if (comment)
	    sprintf (buf, " / %-47.47s", comment);
	else
	    sprintf (buf, "%50s", "");
	memcpy (&line[30], buf, FITS_HCOLS-30);
}

/* turn native unsigned shorts into FITS' big-endian signed.
 */
static void
enFITSPixels (fip)
FImage *fip;
{
	unsigned short *pixp = (unsigned short *)fip->image;
	int n = fip->sh * fip->sw;
	unsigned short *r0;
	int p0;

	if (fip->bitpix == 8)
	    return;	/* nothing to do if just bytes */

	if (lendian()) {
	    while (--n >= 0) {
		r0 = pixp++;
		p0 = (int)(*r0) - 32768;
		*r0 = ((p0 << 8) & 0xff00) | ((p0 >> 8) & 0xff);
	    }
	} else {
	    while (--n >= 0) {
		r0 = pixp++;
		p0 = (int)(*r0) - 32768;
		*r0 = p0;
	    }
	}
}

/* turn FITS' big-endian signed shorts into native unsigned shorts.
 */
static void
unFITSPixels (fip)
FImage *fip;
{
	unsigned short *pixp = (unsigned short *)fip->image;
	int n = fip->sh * fip->sw;
	unsigned short *r0;
	int p0;

	if (fip->bitpix == 8)
	    return;	/* nothing to do if just bytes */

	if (lendian()) {
	    while (--n >= 0) {
		r0 = pixp++;
		p0 = ((*r0 << 8) & 0xff00) | ((*r0 >> 8) & 0xff);
		*r0 = (unsigned short)(p0 + 32768);
	    }
	} else {
	    while (--n >= 0) {
		r0 = pixp++;
		p0 = (int)(*r0);
		*r0 = (unsigned short)(p0 + 32768);
	    }
	}
}

/* return 1 if this machine is little-endian (low-byte first) else 0. */
static int
lendian()
{
	union {
	    short s;
	    char c[2];
	} U;

	U.c[0] = 1;
	U.c[1] = 0;

	return (U.s == 1);
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: fits.c,v $ $Date: 1998/12/27 21:07:42 $ $Revision: 1.2 $ $Name:  $"};
