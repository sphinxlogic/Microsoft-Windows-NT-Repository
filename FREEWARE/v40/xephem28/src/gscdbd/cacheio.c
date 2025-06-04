/* code to handle the details of the GSC cache files for gscdbd.
 * the files have a short header at the front in ASCII.
 * the stars then follow in a packed binary format.
 * the packed format stores locations to within about .2 arcseconds.
 * see the readEntry() function comments for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "gscdbd.h"

static void cacheBuildFilename (char fullpath[], char dir[], GSCRegion *rp);
static void cacheBuildDir (char fullpath[], char dir[], GSCRegion *rp);
static int readHeader (GSCRegion *rp);
static void writeHeader (GSCRegion *rp);
static void readEntry (GSCRegion *rp, GSCEntry *ep);
static void writeEntry (GSCRegion *rp, GSCEntry *ep);

#define	RAERR		(1e-6)		/* allowed ra slop, rads */
#define	DECERR		(1e-6)		/* allowed dec slop, rads */
#define	MAGX		900.0		/* magnitude scale */

#define	LS(v,n)		(((unsigned)(v)) << (n))
#define	RS(v,n)	((unsigned char)(0xff & ((n)==0?(v):(((unsigned)(v)) >> (n)))))

/* open and setup cp->fp, nrows and cosdec0 from an existing cache file off
 *   dir[] for the given region so it is ready to return successive entries.
 * the other header fields should match those in rp.
 * don't print anything (except traces of course) if the only problem is that
 *   the file doesn't exist.
 * return 0 if ok else return -1.
 */
int
cacheOpenRegion (char dir[], GSCRegion *rp, GSCRegion *cp)
{
	char fullpath[1024];

	/* open the cache file */
	cacheBuildFilename (fullpath, dir, rp);
	cp->fp = fopen (fullpath, "rb");
	if (!cp->fp) {
	    if (verbose)
		perror (fullpath);
	    return (-1);
	}

	/* read the header */
	if (readHeader (cp) < 0) {
	    fclose (cp->fp);
	    cp->fp = NULL;
	    return (-1);
	}

	/* header must agree with rp.
	 * N.B. we don't know rp->nrows because that only gets set if we
	 *   can open the cdrom file -- but then we wouldn't be here :-)
	 */
	if (rp->south != cp->south || rp->id != cp->id
					|| fabs(rp->ra0 - cp->ra0) > DECERR
					|| fabs(rp->dec0 - cp->dec0) > RAERR) {
	    fprintf (stderr, "%s: bad header: %d %d %g %g %d\n", fullpath,
			    cp->id, cp->nrows, cp->ra0, cp->dec0, cp->south);
	    fprintf (stderr, "  Expecting %d ?? %g %g %d\n",
					rp->id, rp->ra0, rp->dec0, rp->south);
	    fclose (cp->fp);
	    cp->fp = NULL;
	    return (-1);
	}

	cp->cosdec0 = cos(cp->dec0);

	return (0);
}

/* return the next entry from the cache region.
 * decrement rp->nrows as we go and return -1 when it reaches 0, else 0.
 */
int
cacheGetNextEntry (GSCRegion *rp, GSCEntry *ep)
{
	if (rp->nrows <= 0)
	    return (-1);
	readEntry (rp, ep);
	rp->nrows -= 1;
	return (0);
}

 
/* do whatever cleanup might be required when finished with rp.
 */
void
cacheCloseRegion (GSCRegion *rp)
{
	fclose (rp->fp);
	rp->fp = NULL;
}

/* try to create a new cache file for region *rp off the given directory.
 * make a new descriptor for it at *cp.
 * return 0 if ok return -1 and complain to stderr but no where else.
 */
int
cacheCreateRegion (char dir[], GSCRegion *rp, GSCRegion *cp)
{
	char fullpath[1024];

	/* copy most fields from rp */
	*cp = *rp;

	/* create a new file, and directory too if necessary */
	cacheBuildFilename (fullpath, dir, rp);
	cp->fp = fopen (fullpath, "wb");
	if (!cp->fp) {
	    if (errno == ENOENT) {
		/* failed because of nonexistent dir component */
		char fulldir[1024];

		cacheBuildDir (fulldir, dir, rp);
		if (mkdir (fulldir, 0755) < 0) {
		    perror (fulldir);
		    return (-1);
		}
		cp->fp = fopen (fullpath, "wb");
	    } 

	    if (!cp->fp) {
		perror (fullpath);
		return (-1);
	    }
	}

	/* set up the cosdec0 shortcut */
	cp->cosdec0 = cos(cp->dec0);

	/* write header onto the new cache file */
	writeHeader (cp);

	return (0);
}

/* write ep to rp->fp.
 */
void
cacheAddEntry (GSCRegion *rp, GSCEntry *ep)
{
	writeEntry (rp, ep);
}

/* build a full cache filename for rp off dir[].
 */
static void
cacheBuildFilename (char fullpath[], char dir[], GSCRegion *rp)
{
	(void) sprintf (fullpath, "%s/%s/%04d.ech", dir, rp->dir, rp->id);
}

/* build a full cache dir for rp off dir[].
 */
static void
cacheBuildDir (char fullpath[], char dir[], GSCRegion *rp)
{
	(void) sprintf (fullpath, "%s/%s", dir, rp->dir);
}

/* read a cache file header and fill in rp.
 * leave rp->fp positioned at first star.
 */
static int
readHeader (GSCRegion *rp)
{
	char hdr[128];
	double v;
	int c, i;

	/* read up to first nl */
	for (i=0; (c=getc(rp->fp)) != EOF && c != '\n' && i < sizeof(hdr); i++)
	    hdr[i] = (char)c;
	if (i == sizeof(hdr)) {
	    if (verbose) {
		char fullpath[1024];
		cacheBuildFilename (fullpath, "?", rp);
		hdr[sizeof(hdr)-1] = '\0';
		fprintf (stderr, "%s: no header: %s\n", fullpath, hdr);
	    }
	    return (-1);
	}

	/* crack it */
	hdr[i] = '\0';
	i = sscanf (hdr, "%lf %d %d %lf %lf %d\n", &v, &rp->id, &rp->nrows,
					&rp->ra0, &rp->dec0, &rp->south);
	if (i != 6) {
	    if (verbose) {
		char fullpath[128];
		cacheBuildFilename (fullpath, "?", rp);
		fprintf (stderr, "%s: bad header: %s\n", fullpath, hdr);
	    }
	    return (-1);
	}

	return (0);
}

/* write a cache file header from rp.
 */
static void
writeHeader (GSCRegion *rp)
{
	char hdr[128];
	int n;

	/* format the header */
	(void) sprintf (hdr, "%4.1f %4d %5d %9.7f %9.7f %1d\n",
		    version, rp->id, rp->nrows, rp->ra0, rp->dec0, rp->south);
	n = strlen (hdr);

	/* write it */
	fwrite (hdr, 1, n, rp->fp);
}

/* read a cache file entry 
 *   0  2  delta ra, rads, 0 .. degrad(4) * cos(dec0), scaled 0 .. 0xffff
 *   2  2  delta dec, rads, 0 .. degrad(4), scaled 0 .. 0xffff
 *   4  2  small region id
 *   6  2  upper 2 bits are class, lower 14 are magnitude*MAGX
 *   8     total bytes per entry
 *  all values stored in big-endian byte order.
 */
static void
readEntry (GSCRegion *rp, GSCEntry *ep)
{
	FILE *fp = rp->fp;
	unsigned char buf[2];
	unsigned i;

	(void) fread (buf, 1, 2, fp);
	i = LS(buf[0],8) | LS(buf[1],0);
	ep->ra = rp->ra0 + (double)i/(double)0xffff*degrad(4)/rp->cosdec0;

	(void) fread (buf, 1, 2, fp);
	i = LS(buf[0],8) | LS(buf[1],0);
	ep->dec = rp->dec0 + (double)i/(double)0xffff*degrad(4);
	if (rp->south)
	    ep->dec = -ep->dec;

	(void) fread (buf, 1, 2, fp);
	ep->id = LS(buf[0],8) | LS(buf[1],0);

	(void) fread (buf, 1, 2, fp);
	i = LS(buf[0],8) | LS(buf[1],0);
	ep->class = (i>>14)&0x3;
	ep->mag = (i&0x3fff)/MAGX;
}

/* write a cache file entry 
 *   0  2  delta ra, rads, 0 .. degrad(4) * cos(dec0), scaled 0 .. 0xffff
 *   2  2  delta dec, rads, 0 .. degrad(4), scaled 0 .. 0xffff
 *   4  2  small region id
 *   6  2  upper 2 bits are class, lower 14 are magnitude*MAGX
 *   8     total bytes per entry
 *  all values stored in big-endian byte order.
 */
static void
writeEntry (GSCRegion *rp, GSCEntry *ep)
{
	FILE *fp = rp->fp;
	unsigned char buf[2];
	unsigned i;
	double d, a;

	d = (ep->ra - rp->ra0)*(double)0xffff/degrad(4)*rp->cosdec0;
	i = (unsigned)floor(d + 0.5);
	buf[0] = RS(i,8);
	buf[1] = RS(i,0);
	fwrite (buf, 1, 2, fp);

	d = rp->south ? -ep->dec : ep->dec;
	a = (d - rp->dec0)*(double)0xffff/degrad(4);
	i = (unsigned)floor(a + 0.5);
	buf[0] = RS(i,8);
	buf[1] = RS(i,0);
	fwrite (buf, 1, 2, fp);

	i = ep->id;
	buf[0] = RS(i,8);
	buf[1] = RS(i,0);
	fwrite (buf, 1, 2, fp);

	i = (unsigned)floor(ep->mag*MAGX + 0.5) | (ep->class << 14);
	buf[0] = RS(i,8);
	buf[1] = RS(i,0);
	fwrite (buf, 1, 2, fp);
}
