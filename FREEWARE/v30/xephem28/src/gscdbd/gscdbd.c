/* main program for gscdbd, the Hubble GSC database daemon for xephem.
 * see usage() for summary of options.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <math.h>
#include "gscdbd.h"

typedef struct {
    double ra, dec;	/* center of interest, rads */
    double cdec, sdec;	/* convenient cos(dec) and sin(dec) */
    double epoch;	/* years */
    double fov;		/* field-of-view, rads */
    double mag;		/* limiting mag */
} Request;

/* configurable defaults */
static char *cdpath  = "/cdrom";	/* where GSC CD rom is mounted */
static char *cache   = "cache";		/* where to build up a cache */
static char *infifo  = "../fifos/xephem_loc_fifo";	/* where to listen */
static char *outfifo = "../fifos/xephem_db_fifo";	/* where to repond */
static double maxfov = degrad(0);	/* max field-of-view to retrieve, rads.
					 * use 0 for exactly 1 gsc patch.
					 */

/* no user serviceable parts beyond this point */

static void usage (char *prog);
static void daemonize(void);
static void openFifos(void);
static void loadcdrom(void);
static void mainloop(void);
static void handleRequest (Request *qp);
static int inFOV (Request *qp, GSCEntry *ep);
static void sendRegion (GSCRegion *rp, Request *qp);
static void sendGSCEntry (GSCRegion *rp, GSCEntry *ep);

int verbose;			/* verbose flag */
static int nodaemon;		/* do not run as a daemon */
static int nocache;		/* force not using the cache */
static int nocdrom;		/* force not using the cdrom */
static int loadall;		/* load entire cdrom */

static FILE *infp;		/* input stream */
FILE *outfp;			/* output stream */

#define	FOVSTEP	degrad(0.1)	/* FOV fill scan step size, rads */

int
main (int ac, char *av[])
{
	char *prog = av[0];

	/* crack our args */
	while ((--ac > 0) && ((*++av)[0] == '-')) {
	    char *s;
	    for (s = av[0]+1; *s != '\0'; s++)
		switch (*s) {
		case 'a':	/* force no cache */
		    nocache++;
		    break;
		case 'c':	/* alternate cache directory */
		    if (--ac < 0)
			usage(prog);
		    cache = *++av;
		    break;
		case 'd':	/* don't daemonize */
		    nodaemon++;
		    break;
		case 'f':	/* max fov */
		    if (--ac < 0)
			usage(prog);
		    maxfov = degrad(atof(*++av));
		    break;
		case 'h':	/* usage summary */
		    usage (prog);
		    break;
		case 'i':	/* alternate input fifo name */
		    if (--ac < 0)
			usage(prog);
		    infifo = *++av;
		    break;
		case 'm':	/* force no cd-rom */
		    nocdrom++;
		    break;
		case 'l':	/* load entire cd-rom */
		    loadall++;
		    break;
		case 'o':	/* alternate output fifo name */
		    if (--ac < 0)
			usage(prog);
		    outfifo = *++av;
		    break;
		case 'r':	/* alternate CDROM mount point */
		    if (--ac < 0)
			usage(prog);
		    cdpath = *++av;
		    break;
		case 'v':	/* verbose */
		    verbose++;
		    break;
		default:
		    usage (prog);
		}
	}

	if (ac > 0)
	    usage(prog);

	if (verbose)
	    fprintf (stderr, "gscdbd version %g\n", version);

	if (nocache && nocdrom) {
	    fprintf (stderr, "Must allow at least one of cdrom or cache.n");
	    exit (0);
	}

	if (loadall) {
	    nocache = 1;
	    nocdrom = 0;
	    loadcdrom();
	    exit(0);
	}

	/* run as a separate process (unless turned off) */
	if (!nodaemon)
	    daemonize();

	/* open the fifos */
	openFifos();

	/* listen and report forever */
	mainloop();

	/* for lint */
	return (0);
}

static void
usage (char *p)
{
	fprintf (stderr, "%s [options]\n", p);
	fprintf (stderr, "version %g\n", version);
	fprintf (stderr, "   -r <dir>:  alternate CDROM mount point (default is %s)\n", cdpath);
	fprintf (stderr, "   -c <dir>:  alternate cache directory (default is %s)\n", cache);
	fprintf (stderr, "   -i <path>: alternate input fifo path (default is %s)\n", infifo);
	fprintf (stderr, "   -o <path>: alternate output fifo path (default is %s)\n", outfifo);
	fprintf (stderr, "   -f <degs>: max FOV to retrieve; 0 for exactly 1 GSC segment (default is %g)\n", raddeg(maxfov));
	fprintf (stderr, "   -d:        do not daemonize (default is to fork and this parent exits)\n");
	fprintf (stderr, "   -h:        help usage summary -- this\n");
	fprintf (stderr, "   -v:        send verbose trace messages to stderr\n");
	fprintf (stderr, "   -a:        don't make or use the disk cache\n");
	fprintf (stderr, "   -m:        don't use the cdrom\n");
	fprintf (stderr, "   -l:        load entire cdrom into cache then exit\n");
	exit (1);
}

static void
daemonize()
{
	switch (fork()) {
	case 0:	/* child */
	    return;
	case -1:/* error */
	    perror ("fork");
	    exit (1);
	default:/* parent */
	    exit (0);
	}
}

/* open infifo[] and set infp; open outfifo[] and set outfp.
 * open "r+" so we don't create them, never block on open, and hang on when
 *   closed by the other side.
 */
static void
openFifos()
{
	infp = fopen (infifo, "r+");
	if (!infp) {
	    perror (infifo);
	    exit (1);
	}

	outfp = fopen (outfifo, "r+");
	if (!outfp) {
	    perror (outfifo);
	    exit (1);
	}
}

/* figure out which cdrom is loaded, then load it all into the cache.
 */
static void
loadcdrom()
{
	char buf[1024];
	Request q;
	int nhem;

	/* first guess northern */
	nhem = 1;
	(void) sprintf (buf, "%s/gsc/n8230/4615.gsc", cdpath);
	if (access (buf, R_OK) < 0) {
	    /* nope, try southern */
	    (void) sprintf (buf, "%s/gsc/s1500/6263.gsc", cdpath);
	    if (access (buf, R_OK) < 0) {
		fprintf (stderr, "CDROM not recognized\n");
		exit (1);
	    }
	    nhem = 0;
	}

	if (nhem) {
	    q.ra = 0.0;
	    q.dec = degrad(90.0);
	    q.cdec = cos(q.dec);
	    q.sdec = sin(q.dec);
	    q.epoch = 2000.0;
	    q.fov = degrad ((90+7)*2);
	    q.mag = 20.0;
	} else {
	    q.ra = 0.0;
	    q.dec = degrad(-90.0);
	    q.cdec = cos(q.dec);
	    q.sdec = sin(q.dec);
	    q.epoch = 2000.0;
	    q.fov = degrad ((90-8)*2);
	    q.mag = 20.0;
	}

	handleRequest (&q);
}

/* listen forever to infp for coordinates.
 * generate reports on outfp when hear anything reasonable.
 */
static void
mainloop()
{
	char buf[1024];

	while (fgets (buf, sizeof(buf), infp)) {
	    Request q;
	    int nscan;

	    /* read a line from xephem */
	    nscan = sscanf (buf, "RA:%lf Dec:%lf Epoch:%lf FOV:%lf Mag:%lf",
				    &q.ra, &q.dec, &q.epoch, &q.fov, &q.mag);
	    q.cdec = cos(q.dec);
	    q.sdec = sin(q.dec);

	    /* clamp the field of view we'll retrieve */
	    if (q.fov > maxfov)
		q.fov = maxfov;

	    if (verbose) {
		fprintf (stderr, "Inbuf: %s", buf);
		fprintf (stderr, "Request: RA=%g Dec=%g E=%g FOV=%g Mag=%g\n",
					    q.ra, q.dec, q.epoch, q.fov, q.mag);
	    }
	    if (nscan != 5) {
		fprintf (stderr, "Bad infifo line: %s", buf);
		continue;
	    }

	    /* make report to outfp, using and/or creating cache as necessary */
	    handleRequest (&q);

	    if (verbose)
		fprintf (stderr, "Report complete\n");
	}

	fprintf (stderr, "%s: closed\n", infifo);
}

/* send the desired response on outfp for this request.
 */
static void
handleRequest (Request *qp)
{
	int *ids;		/* malloced list of region numbers sent */
	int nids;		/* number of slots in ids[] */
	double radius, angle;	/* fov scanning parameters */
	GSCRegion region;

	/* keep a record of each unique region we send to avoid dups */
	ids = (int *) malloc (sizeof(int));

	/* get and send the region we are directly in for sure */
	gscPickRegion (qp->ra, qp->dec, &region);
	sendRegion (&region, qp);
	ids[0] = region.id;
	nids = 1;

	/* scan an area around qp of size fov and report all new regions*/
	for (radius = FOVSTEP; radius <= qp->fov/2; radius += FOVSTEP)
	    for (angle = 0.0; angle < 2*PI; angle += FOVSTEP/radius) {
		double ra, sindec, delra;
		int i;

		/* find this region */
		solve_sphere(angle,radius, qp->sdec, qp->cdec, &sindec, &delra);
		ra = qp->ra + delra;
		range (&ra, 2*PI);
		gscPickRegion (ra, asin(sindec), &region);

		/* if not already in list, send its coords and add to list */
		for (i = 0; i < nids; i++)
		    if (region.id == ids[i])
			break;
		if (i == nids) {
		    sendRegion (&region, qp);
		    ids = (int *) realloc ((void *)ids, (nids+1)*sizeof(int));
		    ids[nids++] = region.id;
		}
	    }

	free ((void *)ids);

	if (outfp) {
	    fprintf (outfp, "!End\n");
	    fflush (outfp);
	}
}

/* build and send the response for this region and center location.
 * use cache if possible (and desired) else cdrom (if desired).
 * if no cache copy exits, create it along the way (if desired).
 * any error messages go to stderr and maybe outfp.
 */
static void
sendRegion (GSCRegion *rp, Request *qp)
{
	char rastr[64], decstr[64];
	GSCRegion cr;	/* used for the cache entry */
	GSCEntry e;

	/* format ra0 and dec0 in case of tracing */
        fs_sexa (rastr, radhr(rp->ra0), 2, 36000);
        fs_sexa (decstr, raddeg(rp->dec0) * (rp->south?-1:1), 3, 3600);

	/* first try to use the cache, if desired */
	if (!nocache) {
	    if (cacheOpenRegion (cache, rp, &cr) == 0) {
		while (cacheGetNextEntry (&cr, &e) == 0)
		    if (inFOV (qp, &e) == 0)
			sendGSCEntry (&cr, &e);
		cacheCloseRegion (&cr);
		if (verbose)
		    fprintf (stderr, "Sent cache region %d (%s %s)\n", cr.id,
								rastr, decstr);
		return;
	    }
	}

	/* if not, then the cdrom.
	 * also make the cache entry along the way, if desired.
	 */
	if (!nocdrom) {
	    if (gscOpenRegion (cdpath, rp) == 0) {
		int cacheok = nocache ? -1 : cacheCreateRegion (cache, rp, &cr);
		while (gscGetNextEntry (rp, &e) == 0) {
		    if (outfp && inFOV (qp, &e) == 0)
			sendGSCEntry (rp, &e);
		    if (cacheok == 0)
			cacheAddEntry (&cr, &e);
		}
		if (cacheok == 0) {
		    cacheCloseRegion (&cr);
		    if (verbose)
			fprintf (stderr, "Built cache region %d (%s %s)\n",
							cr.id, rastr, decstr);
		}
		gscCloseRegion (rp);
		if (verbose)
		    fprintf (stderr, "Sent cdrom region %d (%s %s)\n", rp->id,
								rastr, decstr);
		return;
	    }
	}

	if (outfp)
	    fflush (outfp);
}

/* return 0 if the given entry is within the given request, else return -1.
 * if fov == 0, we let anything work.
 * N.B. we actually prune to a square with side fov, not a circle.
 */
static int
inFOV (Request *qp, GSCEntry *ep)
{
	double rov = qp->fov/2;	/* radius of view */
	double tmp;

	/* if rov==0 anything goes */
	if (rov == 0.0)
	    return (0);

	/* prune by dec */
	if (fabs (qp->dec - ep->dec) > rov)
	    return (-1);

	/* prune by RA -- beware of wrap at 24 */
	tmp = fabs (qp->ra - ep->ra);
	if (tmp > PI)
	    tmp = 2*PI - tmp;
	if (tmp*qp->cdec > rov)
	    return (-1);

	/* ok */
	return (0);
}

/* format and send one GSC entry to outfp */
static void
sendGSCEntry (GSCRegion *rp, GSCEntry *ep)
{
	char rastr[64], decstr[64];

	fs_sexa (rastr, radhr(ep->ra), 2, 36000);
	fs_sexa (decstr, raddeg(ep->dec), 3, 3600);

	fprintf (outfp, "GSC-%04d-%04d,f%s,%s,%s,%g,2000\n", rp->id,
		    ep->id, ep->class == 0 ? "|S" : "", rastr, decstr, ep->mag);
}
