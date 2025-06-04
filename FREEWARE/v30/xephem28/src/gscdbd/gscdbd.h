/* include file for gscdbd */

#ifndef PI
#define	PI		3.141592653589793
#endif

/* conversions among hours (of ra), degrees and radians. */
#define	degrad(x)	((x)*PI/180.)
#define	raddeg(x)	((x)*180./PI)
#define	hrdeg(x)	((x)*15.)
#define	deghr(x)	((x)/15.)
#define	hrrad(x)	degrad(hrdeg(x))
#define	radhr(x)	deghr(raddeg(x))

/* common globals */
extern int verbose;
extern FILE *outfp;

/* what we need to know about a small GSC region */
typedef struct {
    /* following are filled by gscPickRegion() */
    char dir[6];	/* dir off gsc/, e.g. "n1234" */
    char file[9];	/* file off gsc/<dir>, e.g. "5678.gsc" */
    int south;		/* 0 if north, 1 if north */
    int id;		/* small region id number */
    double ra0;		/* eastern edge, rads */
    double dec0;	/* fabs(dec edge nearest 0), rads */

    /* following are filled by gscOpenRegion() or cacheOpen/CreateRegion() */
    FILE *fp;		/* file pointer, once open */
    int nrows;		/* rows (stars) remaining */

    /* following is only used by the cache subsystem. */
    double cosdec0;
} GSCRegion;

/* info about one GSC entry.
 * filled in by gscNextEntry()
 */
typedef struct {
    int id;		/* id */
    int class;		/* 0 star */
    double ra;		/* rads */
    double dec;		/* rads */
    double mag;		/* magnitude */
} GSCEntry;

/* functions in gscio.c */
extern void gscPickRegion (double ra, double dec, GSCRegion *rp);
extern int gscOpenRegion (char path[], GSCRegion *rp);
extern int gscGetNextEntry (GSCRegion *rp, GSCEntry *ep);
extern void gscCloseRegion (GSCRegion *rp);

/* functions in cacheio.c */
extern int cacheOpenRegion (char dir[], GSCRegion *rp, GSCRegion *cp);
extern int cacheGetNextEntry (GSCRegion *rp, GSCEntry *ep);
extern void cacheCloseRegion (GSCRegion *rp);
extern int cacheCreateRegion (char dir[], GSCRegion *rp, GSCRegion *cp);
extern void cacheAddEntry (GSCRegion *rp, GSCEntry *ep);

/* functions in support.c */
extern void solve_sphere (double A, double b, double cc, double sc,
    double *cap, double *Bp);
extern void range (double *v, double r);
extern void fs_sexa (char *out, double a, int w, int fracbase);

/* stuff in version.c */
extern double version;
