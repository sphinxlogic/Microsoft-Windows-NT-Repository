/* code to manage what the outside world sees as the db_ interface.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>


#if defined(__STDC__)
#include <stdlib.h>
#include <string.h>
typedef const void * qsort_arg;
#else
typedef void * qsort_arg;
extern void *malloc(), *realloc();
#endif

#if defined(_POSIX_SOURCE)
#include <unistd.h>
#else
extern int read();
extern int close();
#endif

#include <X11/Intrinsic.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"

extern Widget toplevel_w;
extern char *myclass;
extern XtAppContext xe_app;
#define XtD XtDisplay(toplevel_w)


extern FILE *fopenh P_((char *name, char *how));
extern Now *mm_get_now P_((void));
extern char *syserrstr P_((void));
extern double atod P_((char *buf));
extern int db_n P_((void));
extern int db_n_cp P_((void));
extern int db_read P_((FILE *fp));
extern int db_set_field P_((char bp[], int id, PrefDateFormat pref, Obj *op));
extern double delra P_((double dra));
extern int obj_cir P_((Now *np, Obj *op));
extern int openh P_((char *name, int flags));
extern void all_newdb P_((int appended));
extern void db_connect_fifo P_((void));
extern void db_del_all P_((void));
extern void db_del_cp P_((void));
extern void db_scaninit P_((DBScan *sp));
extern void db_set_cp P_((void));
extern void db_clr_cp P_((void));
extern void db_update P_((Obj *op));
extern void f_scansex P_((double o, char bp[], double *np));
extern void f_sscandate P_((char *bp, int pref, int *m, double *d, int *y));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void pm_set P_((int p));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));
extern void zero_mem P_((void *loc, unsigned len));


static int db_crack_line P_((char s[]));
static Obj *db_new P_((int t));
static void db_init P_((void));
static void db_nodups P_((void));
static int nxt_db P_((char buf[], int blen, FILE *fp));
static int line_candidate P_((char *buf));
static void crack_year P_((char *bp, PrefDateFormat pref, double *p));
static int get_fields P_((char *s, int delim, char *fields[]));
static void dbfifo_cb P_((XtPointer client, int *fdp, XtInputId *idp));

#define	MAXDBLINE	256	/* longest allowed db line */
#define	FLDSEP		','	/* major field separator */
#define	SUBFLD		'|'	/* subfield separator */

#define	DBFIFO_MSG	'!'	/* introduces a message line from the DBFIFO */
#define	MAXDUPSEP	degrad(1.0/3600.) /* max dup separation, rads */

#define	ASIZ(a)	(sizeof(a)/sizeof(a[0]))

/* This counter is incremented when we want to mark all the Obj derived entries
 * as being out-of-date. This works because then each of the age's will be !=
 * db_age. This is to eliminate ever calling obj_cir() under the same
 * circumstances for a given db object.
 * N.B. For this to work, call db_update() before using a Obj *.
 */
static Objage_t db_age;

/* the "database".
 * one such struct per object type. the space for objects is malloced in
 *   seperate chunks of DBCHUNK to avoid needing big contiguous memory blocks.
 * N.B. because the number is fixed and known, we use static storage for the
 *   NOBJ Objects for the PLANET type; see db_init().
 */
#define	DBCHUNK	256	/* number we malloc more of at once; a power of two
			 * might help the compiler optimize the divides and
			 * modulo arithmetic.
			 */
typedef struct {
    char **dblist;	/* malloced list of malloced DBCHUNKS arrays */
    int nobj;		/* number of objects actually in use */
    int nmem;		/* total number of objects for which we have room */
    int ncp;		/* nobj at time of last checkpoint */
    int size;		/* bytes per object */
} DBMem;
static DBMem db[NOBJTYPES];	/* this is the head for each object */
#define	DBINITED	(db[PLANET].dblist)	/* anything setup by db_init()*/

/* macro that returns the address of an object given its type and index */
#define	OBJP(t,n)	\
		((Obj *)(db[t].dblist[(n)/DBCHUNK] + ((n)%DBCHUNK)*db[t].size))

/* db fifo fd and XtAddInput id.
 */
static int db_fifofd = -1;
static XtInputId db_fifoid;


/* set whenever we want to take care to eliminate any items in the db with
 *   duplicate names.
 */
static int db_wantnodups;

/* return number of objects in the database.
 * this includes the NOBJ basic objects.
 * N.B. this is expected to be inexpensive to call.
 */
int
db_n()
{
	DBMem *dmp;
	int n;

	if (!DBINITED)
	    db_init();

	for (n = 0, dmp = db; dmp < &db[NOBJTYPES]; dmp++)
	    n += dmp->nobj;
	return (n);
}

/* return the total current number of objects as of the last checkpoint.
 * this includes the NOBJ basic objects.
 */
int
db_n_cp()
{
	DBMem *dmp;
	int n;

	if (!DBINITED)
	    db_init();

	for (n = 0, dmp = db; dmp < &db[NOBJTYPES]; dmp++)
	    n += dmp->ncp;
	return (n);
}

/* given one of the basic ids in astro.h return pointer to its updated Obj in
 * the database.
 */
Obj *
db_basic(id)
int id;
{
	Obj *op;

	if (!DBINITED)
	    db_init();

	if (id < 0 || id >= NOBJ) {
	    printf ("db_basic(): bad id: %d\n", id);
	    exit (1);
	}

	op = OBJP(PLANET,id);
	if (op->type != UNDEFOBJ)
	    db_update(op);
	return (op);
}

/* load each file of objects listed in the DBinitialFiles resource
 * and inform all modules of the update.
 */
void
db_loadinitial()
{
	static char dbd[] = "DBdirectory";	/* dir resource name */
	static char dbi[] = "DBinitialFiles";	/* init files resource name */
	char *fns;		/* value of DBinitialFiles */
	char *dir;		/* value of DBdirectory */
	char dbicpy[2048];	/* local copy of dir */
	char *fnf[128];		/* ptrs into dbicpy[] at each ' ' */
	char fn[1024];		/* each dir/filename */
	char msg[256];
	FILE *fp;
	int nfn;
	int i;

	/* get the initial directory name */
	dir = XGetDefault (XtD, myclass, dbd);
	if (!dir) {
	    dir = ".";
	    (void) sprintf (msg, "Can not get %.100s.%.100s -- using %s",
							    myclass, dbd, dir);
	    xe_msg (msg, 0);
	}

	/* get the initial list of files */
	fns = XGetDefault (XtD, myclass, dbi);
	if (!fns) {
	    fns = "basic.edb";
	    (void) sprintf (msg ,"Can not get %.100s.%.100s -- using %s",
							myclass, dbi, fns);
	    xe_msg (msg, 0);
	}

	/* work on a copy since we are about to break into fields */
	dbicpy[sizeof(dbicpy)-1] = '\0';
	(void) strncpy (dbicpy, fns, sizeof(dbicpy)-1);
	nfn = get_fields (dbicpy, ' ', fnf);
	if (nfn > XtNumber(fnf)) {
	    /* we exit because we've clobbered our stack by now!
	     * TODO: pass the size of fnf to get_fields().
	     */
	    printf ("Too many entries in %s. Max is %d\n", dbi, XtNumber(fnf));
	    exit (1);
	}

	/* open and read each file.
	 * N.B. get_fields() will return 1 even if there are no fields.
	 */
	for (i = 0; i < nfn && fnf[i][0] != '\0'; i++) {
#ifdef VMS
	    /* VMS doesn't like the /'s between components.
	     * this from Harry Payne, payne@stsci.edu
	     */
	    (void) sprintf (fn, "%.500s%.500s", dir, fnf[i]);
#else
	    (void) sprintf (fn, "%.500s/%.500s", dir, fnf[i]);
#endif
	    fp = fopenh (fn, "r");
	    if (!fp) {
		(void) sprintf (msg, "Can not open %.150s: %.50s\n", fn,
								syserrstr());
		xe_msg (msg, 1);
		continue;
	    }
	    if (db_read (fp) < 0) {
		(void) sprintf (msg, "Error reading `%.200s'", fn);
		xe_msg (msg, 0);
	    }
	    (void) fclose (fp);
	}

	all_newdb(1);
}

/* set our official copy of the given user-defined object to *op */
void
db_setuserobj(id, op)
int id;	/* OBJX or OBJY */
Obj *op;
{
	if (id == OBJX || id == OBJY) {
	    Obj *bop = OBJP(PLANET,id);
	    *bop = *op;
	} else {
	    printf ("db_setuserobj(): bad id: %d\n", id);
	    exit (1);
	}
}

/* mark all db objects as old
 */
void
db_invalidate()
{
	if (!DBINITED)
	    db_init();

	db_age++;
}

/* initialize the given DBScan for a database scan. the idea is to call this
 *   once, then repeatedly call db_scan() to get all objects in the db.
 * return NULL when there are no more.
 * N.B. nothing should be assumed as to the order these are returned.
 * N.B. the s_ fields are *not* updated -- call db_update() when you need that.
 */
void
db_scaninit (sp)
DBScan *sp;
{
	sp->t = 0;
	sp->n = 0;
}

/* fetch the next object.
 * N.B. this will return the user defined objects which may still
 *   have op->type == UNDEFOBJ.
 */
Obj *
db_scan (sp)
DBScan *sp;
{
	Obj *op;

	if (!DBINITED)
	    db_init();

	while (sp->n >= db[sp->t].nobj) {
	    if (++sp->t == NOBJTYPES)
		return (0);
	    sp->n = 0;
	}

	op = OBJP(sp->t, sp->n);
	sp->n++;

	return (op);
}
	

/* see to it that all the s_* fields in the given db object are up to date.
 * always recompute the user defined objects because we don't know when
 * they might have been changed.
 * N.B. it is ok to call this even if op is not actually in the database
 *   although we guarantee an actual update occurs if it's not.
 */
void
db_update(op)
Obj *op;
{
	static char me[] = "db_update()";

	if (!DBINITED)
	    db_init();

	if (op->type == UNDEFOBJ) {
	    printf ("%s: called with UNDEFOBJ pointer\n", me);
	    exit (1);
	} 
	if ((int)op->type >= NOBJTYPES) {
	    printf ("%s: called with bad pointer\n", me);
	    exit (1);
	} 

	if (op->o_age != db_age ||
			op == OBJP(PLANET,OBJX) || op == OBJP(PLANET,OBJY)) {
	    if (obj_cir (mm_get_now(), op) < 0) {
		printf ("%s: bad object\n", me);
		exit(1);
	    }
	    op->o_age = db_age;
	}
}

/* delete all but the basic objects.
 */
void
db_del_all()
{
	DBMem *dmp;

	if (!DBINITED)
	    db_init();

	for (dmp = db; dmp < &db[NOBJTYPES]; dmp++) {
	    /* N.B. PLANET entries are fixed -- not malloced */
	    if (dmp == &db[PLANET])
		continue;
	    if (dmp->dblist) {
		int i;
		for (i = 0; i < dmp->nmem/DBCHUNK; i++)
		    free (dmp->dblist[i]);
		free ((char *)dmp->dblist);
		dmp->dblist = NULL;
	    }
	    dmp->nobj = 0;
	    dmp->nmem = 0;
	    dmp->ncp = 0;
	}
}

/* delete all objects back to the last checkpoint.
 * we don't actually free memory.
 */
void
db_del_cp ()
{
	DBMem *dmp;

	if (!DBINITED)
	    db_init();

	for (dmp = db; dmp < &db[NOBJTYPES]; dmp++)
	    dmp->nobj = dmp->ncp;
}

/* set the checkpoint to the current set of objects in the db.
 */
void
db_set_cp()
{
	DBMem *dmp;

	if (!DBINITED)
	    db_init();

	for (dmp = db; dmp < &db[NOBJTYPES]; dmp++)
	    dmp->ncp = dmp->nobj;
}

/* set the checkpoints to 0 then, if active, also eliminate dups.
 * N.B. but not the PLANT one.
 */
void
db_clr_cp()
{
	DBMem *dmp;

	if (!DBINITED)
	    db_init();

	for (dmp = db; dmp < &db[NOBJTYPES]; dmp++) {
	    /* N.B. PLANET entries are fixed -- not malloced */
	    if (dmp != &db[PLANET])
		dmp->ncp = 0;
	}

	if (db_wantnodups)
	    db_nodups();
}

/* called to indicate whether we want to eliminate dups.
 * if turning it on, we go ahead and eliminate them right now and set a
 *   flag so whenever new items are read dups are discarded then too.
 * if turning it off, we just reset the local flag.
 */
void
db_setnodups (whether)
int whether;
{
	if (whether) {
	    db_nodups();
	    all_newdb(0);
	}

	db_wantnodups = whether;
}

/* read the given database file into memory.
 * add to the existing list.
 * stop gracefully if we run out of memory.
 * return 0 if all ok, else -1.
 */
int
db_read (fp)
FILE *fp;
{
	char buf[MAXDBLINE];
	long len;
	int ret;

	if (!DBINITED)
	    db_init();

	/* set up to run the progress meter based on file position */
	(void) fseek (fp, 0L, 2);
	len = ftell (fp);
	(void) fseek (fp, 0L, 0);
	pm_set (0);

	/* read each line from the file and add to the db */
	ret = 0;
	while (nxt_db (buf, sizeof(buf), fp) == 0) {
	    pm_set ((int)(ftell(fp)*100/len)); /* update progress meter */
	    if (db_crack_line (buf) < 0) {
		ret = -1;
		break;
	    }
	}

	/* discard any duplicates */
	if (db_wantnodups)
	    db_nodups();

	/* should have hit eof */
	if (ret == 0)
	    ret = feof(fp) ? 0 : -1;

	return (ret);
}

/* assuming we can open it ok, connect the db fifo to a callback.
 * we close and reopen each time we are called.
 */
void
db_connect_fifo()
{
	static char dbfifores[] = "DBFIFO";	/* X resource naming db fifo */
	char *fn;

	/* get the fifo name from the X resource */
	fn = XGetDefault (XtD, myclass, dbfifores);
	if (!fn) {
	    char msg[256];
	    (void) sprintf (msg, "No `%.200s' resource.\n", dbfifores);
	    xe_msg (msg, 0);
	    return;
	}

	/* close if currently open */
	if (db_fifofd >= 0) {
	    XtRemoveInput (db_fifoid);
	    (void) close (db_fifofd);
	    db_fifofd = -1;
	}

	/* open for read/write. this assures open will never block, that
	 * reads (and hence select()) WILL block if it's empty, and let's
	 * processes using it come and go as they please.
	 */
	db_fifofd = openh (fn, 2);
	if (db_fifofd < 0) {
	    char msg[256];
	    (void) sprintf (msg, "Can not open %.150s: %.50s\n",fn,syserrstr());
	    xe_msg (msg, 0);
	    return;
	}

	/* wait for messages */
	db_fifoid = XtAppAddInput(xe_app, db_fifofd, (XtPointer)XtInputReadMask,
						    dbfifo_cb, (XtPointer)fn);
}

/* given a text buffer and a field id, and a PREF_DATE_FORMAT,
 *   set the corresponding member in *op.
 * return 0 if ok, else -1.
 */
int
db_set_field (bp, id, pref, op)
char bp[];
int id;
PrefDateFormat pref;
Obj *op;
{
	double tmp;

	/* include all the enums and in numeric order to give us the best
	 * possible chance the compiler will implement this as a jump table.
	 */
	switch (id) {
	case O_TYPE:
	    printf ("db_set_field: called with id==O_TYPE\n");
	    exit(1);
	    break;
	case O_NAME:
	    (void) strncpy (op->o_name, bp, sizeof(op->o_name)-1);
	    op->o_name[sizeof(op->o_name)-1] = '\0';
	    break;
	case F_RA:
	    f_scansex (radhr(op->f_RA), bp, &tmp);
	    op->f_RA = hrrad(tmp);
	    break;
	case F_DEC:
	    f_scansex (raddeg(op->f_dec), bp, &tmp);
	    op->f_dec = degrad(tmp);
	    break;
	case F_EPOCH:
	    tmp = op->f_epoch;
	    crack_year (bp, pref, &tmp);
	    op->f_epoch = tmp;
	    break;
	case F_MAG:
	    op->f_mag = atod (bp) * MAGSCALE;
	    break;
	case F_SIZE:
	    op->f_size = atod (bp);
	    break;
	case F_CLASS:
	    switch (bp[0]) {
	    case 'A': case 'B': case 'C': case 'D': case 'F': case 'G':
	    case 'H': case 'K': case 'J': case 'L': case 'M': case 'N':
	    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
	    case 'U': case 'V':
		op->f_class = bp[0];
		break;
	    default:
		return (-1);
	    }
	    break;
	case F_SPECT: {
	    int i, j;
	    /* fill f_spect all the way */
	    for (i = j = 0; i < sizeof(op->f_spect); i++)
		if ((op->f_spect[i] = bp[j]) != 0)
		    j++;
	    break;
	}

	case E_INC:
	    op->e_inc = atod (bp);
	    break;
	case E_LAN:
	    op->e_Om = atod (bp);
	    break;
	case E_AOP:
	    op->e_om = atod (bp);
	    break;
	case E_A:
	    op->e_a = atod (bp);
	    break;
	case E_N:
	    op->e_n = atod (bp);
	    break;
	case E_E:
	    op->e_e = atod (bp);
	    break;
	case E_M:
	    op->e_M = atod (bp);
	    break;
	case E_CEPOCH:
	    crack_year (bp, pref, &op->e_cepoch);
	    break;
	case E_EPOCH:
	    crack_year (bp, pref, &op->e_epoch);
	    break;
	case E_M1:
	    switch (bp[0]) {
	    case 'g':
		op->e_mag.whichm = MAG_gk;
		bp++;
		break;
	    case 'H':
		op->e_mag.whichm = MAG_HG;
		bp++;
		break;
	    default:
		/* leave type unchanged if no or unrecognized prefix */
		break;
	    }
	    op->e_mag.m1 = atod(bp);
	    break;
	case E_M2:
	    switch (bp[0]) {
	    case 'k':
		op->e_mag.whichm = MAG_gk;
		bp++;
		break;
	    case 'G':
		op->e_mag.whichm = MAG_HG;
		bp++;
		break;
	    default:
		/* leave type unchanged if no or unrecognized prefix */
		break;
	    }
	    op->e_mag.m2 = atod(bp);
	    break;
	case E_SIZE:
	    op->e_size = atod (bp);
	    break;

	case H_EP:
	    crack_year (bp, pref, &op->h_ep);
	    break;
	case H_INC:
	    op->h_inc = atod (bp);
	    break;
	case H_LAN:
	    op->h_Om = atod (bp);
	    break;
	case H_AOP:
	    op->h_om = atod (bp);
	    break;
	case H_E:
	    op->h_e = atod (bp);
	    break;
	case H_QP:
	    op->h_qp = atod (bp);
	    break;
	case H_EPOCH:
	    crack_year (bp, pref, &op->h_epoch);
	    break;
	case H_G:
	    op->h_g = atod (bp);
	    break;
	case H_K:
	    op->h_k = atod (bp);
	    break;
	case H_SIZE:
	    op->h_size = atod (bp);
	    break;

	case P_EP:
	    crack_year (bp, pref, &op->p_ep);
	    break;
	case P_INC:
	    op->p_inc = atod (bp);
	    break;
	case P_AOP:
	    op->p_om = atod (bp);
	    break;
	case P_QP:
	    op->p_qp = atod (bp);
	    break;
	case P_LAN:
	    op->p_Om = atod (bp);
	    break;
	case P_EPOCH:
	    crack_year (bp, pref, &op->p_epoch);
	    break;
	case P_G:
	    op->p_g = atod (bp);
	    break;
	case P_K:
	    op->p_k = atod (bp);
	    break;
	case P_SIZE:
	    op->p_size = atod (bp);
	    break;

	case ES_EPOCH:
	    crack_year (bp, pref, &op->es_epoch);
	    break;
	case ES_INC:
	    op->es_inc = atod (bp);
	    break;
	case ES_RAAN:
	    op->es_raan = atod (bp);
	    break;
	case ES_E:
	    op->es_e = atod (bp);
	    break;
	case ES_AP:
	    op->es_ap = atod (bp);
	    break;
	case ES_M:
	    op->es_M = atod (bp);
	    break;
	case ES_N:
	    op->es_n = atod (bp);
	    break;
	case ES_DECAY:
	    op->es_decay = atod (bp);
	    break;
	case ES_ORBIT:
	    op->es_orbit = (int) atod (bp);
	    break;

	default:
	    printf ("BUG! db_set_field: bad id: %d\n", id);
	    exit (1);
	}

	return (0);
}

/* crack the given database line and add to corresponding db list.
 * return 0 if ok else put up a message and return -1.
 */
static int
db_crack_line (s)
char s[];
{
	static char nomem[] = "Insufficient memory to load this database\n";
#define	MAXFLDS	20		/* must be more than on any expected line */
	char *flds[MAXFLDS];	/* point to each field for easy reference */
	char *sflds[MAXFLDS];	/* point to each sub field for easy reference */
	char copy[MAXDBLINE];	/* work copy; leave s untouched */
	char msg[512];		/* misc message buffer */
	int nf, nsf;		/* number of fields and subfields */
	Obj *op;
	int i;

	/* do all the parsing on a copy */
	(void) strcpy (copy, s);

	/* parse into main fields */
	nf = get_fields (copy, FLDSEP, flds);

	/* need at least 2: name and type */
	if (nf < 2) {
	    (void)sprintf(msg, "Too few fields in Database line: `%.480s'\n",s);
	    xe_msg (msg, 0);
	    return (-1);
	}

	/* switch out on type of object - the second field */
	switch (flds[1][0]) {
	case 'f': {
	    static int ids[] = {F_RA, F_DEC, F_MAG};
	    if (nf < 5 || nf > 7) {
		(void)sprintf(msg,
		    "Need ra,dec,mag[,epoch][,siz] for fixed object `%.460s'\n",
								    flds[0]);
		xe_msg (msg, 0);
		return (-1);
	    }
	    op = db_new(FIXED);
	    if (!op) {
		xe_msg (nomem, 0);
		return (-1);
	    }
	    op->type = FIXED;
	    nsf = get_fields(flds[1], SUBFLD, sflds);
	    if (nsf > 1 && db_set_field (sflds[1], F_CLASS, PREF_MDY, op) < 0) {
		(void) sprintf(msg,"Bad class `%c' for fixed object `%.450s'\n",
							    *sflds[1], flds[0]);
		xe_msg (msg, 0);
		return (-1);
	    }
	    if (nsf > 2)
		(void) db_set_field (sflds[2], F_SPECT, PREF_MDY, op);
	    for (i = 2; i < ASIZ(ids)+2; i++)
		(void) db_set_field (flds[i], ids[i-2], PREF_MDY, op);
	    (void) db_set_field (nf>5 && flds[5][0] ? flds[5] : "2000",
							F_EPOCH, PREF_MDY, op);
	    if (nf == 7)
		(void) db_set_field (flds[6], F_SIZE, PREF_MDY, op);
	    break;
	}

	case 'e': {
	    static int ids[] = {E_INC, E_LAN, E_AOP, E_A, E_N, E_E, E_M,
						E_CEPOCH, E_EPOCH, E_M1, E_M2
	    };
	    if (nf != 13 && nf != 14) {
		(void)sprintf (msg,
		    "Need i,O,o,a,n,e,M,E,D,H/g,G/k[,siz] for elliptical object `%.450s'\n",
								    flds[0]);
		xe_msg(msg, 0);
		return (-1);
	    }
	    op = db_new(ELLIPTICAL);
	    if (!op) {
		xe_msg (nomem, 0);
		return (-1);
	    }
	    op->type = ELLIPTICAL;
	    for (i = 2; i < ASIZ(ids)+2; i++)
		(void) db_set_field (flds[i], ids[i-2], PREF_MDY, op);
	    if (nf == 14)
		(void) db_set_field (flds[13], E_SIZE, PREF_MDY, op);
	    break;
	}

	case 'h': {
	    static int ids[]= {H_EP,H_INC,H_LAN,H_AOP,H_E,H_QP,H_EPOCH,H_G,H_K};
	    if (nf != 11 && nf != 12) {
		(void)sprintf (msg,
		    "Need T,i,O,o,e,q,D,g,k[,siz] for hyperbolic object `%.450s'\n",
								    flds[0]);
		xe_msg(msg, 0);
		return (-1);
	    }
	    op = db_new(HYPERBOLIC);
	    if (!op) {
		xe_msg (nomem, 0);
		return (-1);
	    }
	    op->type = HYPERBOLIC;
	    for (i = 2; i < ASIZ(ids)+2; i++)
		(void) db_set_field (flds[i], ids[i-2], PREF_MDY, op);
	    if (nf == 12)
		(void) db_set_field (flds[11], H_SIZE, PREF_MDY, op);
	    break;
	}

	case 'p': {
	    static int ids[] = {P_EP,P_INC,P_AOP,P_QP,P_LAN,P_EPOCH,P_G,P_K};
	    if (nf != 10 && nf != 11) {
		(void)sprintf (msg,
		    "Need T,i,o,q,O,D,g,k[,siz] for parabolic object `%.450s'\n",
								    flds[0]);
		xe_msg(msg, 0);
		return (-1);
	    }
	    op = db_new(PARABOLIC);
	    if (!op) {
		xe_msg (nomem, 0);
		return (-1);
	    }
	    op->type = PARABOLIC;
	    for (i = 2; i < ASIZ(ids)+2; i++)
		(void) db_set_field (flds[i], ids[i-2], PREF_MDY, op);
	    if (nf == 11)
		(void) db_set_field (flds[10], P_SIZE, PREF_MDY, op);
	    break;
	}

	case 'E': {
	    static int ids[] = {ES_EPOCH,ES_INC,ES_RAAN,ES_E,ES_AP,ES_M,ES_N,
							    ES_DECAY,ES_ORBIT};
	    if (nf != 11) {
		(void)sprintf (msg,
		    "Need E,i,ra,e,p,M,a,d,or for earth satellite`%.450s'\n",
								    flds[0]);
		xe_msg(msg, 0);
		return (-1);
	    }
	    op = db_new(EARTHSAT);
	    if (!op) {
		xe_msg (nomem, 0);
		return (-1);
	    }
	    op->type = EARTHSAT;
	    for (i = 2; i < ASIZ(ids)+2; i++)
		(void) db_set_field (flds[i], ids[i-2], PREF_MDY, op);
	    break;
	}

	default:
	    (void)sprintf (msg,
		"Unknown type for Object %s: `%.480s'\n", flds[0], flds[1]);
	    xe_msg(msg, 0);
	    return (-1);
	}

	/* load up name */
	(void) db_set_field (flds[0], O_NAME, PREF_MDY, op);

	return (0);
}

/* allocate and return zero'd room for a new object with the given type.
 * N.B we do *not* validate t.
 * N.B. the dup eliminator assumes newer objects will be assigned larger
 *   addresses within a given type array.
 * return NULL if can't get more room.
 */
static Obj *
db_new (t)
int t;
{
	DBMem *dmp = &db[t];
	Obj *op;

	/* allocate another chunk if this type can't hold another one */
	if (dmp->nmem <= dmp->nobj) {
	    int ndbl = dmp->nmem/DBCHUNK;
	    int newdblsz = (ndbl + 1) * sizeof(char *);
	    char **newdbl;
	    char *newchk;

	    newdbl = dmp->dblist ? (char **) realloc (dmp->dblist, newdblsz)
				 : (char **) malloc (newdblsz);
	    if (!newdbl)
		return (NULL);

	    newchk = malloc (dmp->size * DBCHUNK);
	    if (!newchk) {
		free ((char *)newdbl);
		return (NULL);
	    }

	    newdbl[ndbl] = newchk;

	    dmp->dblist = newdbl;
	    dmp->nmem += DBCHUNK;
	}

	op = OBJP (t, dmp->nobj);
	dmp->nobj++;
	zero_mem ((void *)op, dmp->size);
	return (op);
}

/* set up the basic database.
 */
static void
db_init()
{
	/* these must match the order in astro.h */
	static char *planet_names[] = {
	    "Mercury", "Venus", "Mars", "Jupiter", "Saturn",
	    "Uranus", "Neptune", "Pluto", "Sun", "Moon",
	};
	static Obj plan_objs[NOBJ];	/* OBJX/Y can be anything so use Obj */
	static char *plan_dblist[1] = {(char *)plan_objs};
	int i;

	/* init the object sizes.
	 * N.B. must do this before using the OBJP macro
	 */
	db[UNDEFOBJ].size	= 0;
	db[FIXED].size		= sizeof(ObjF);
	db[ELLIPTICAL].size	= sizeof(ObjE);
	db[HYPERBOLIC].size	= sizeof(ObjH);
	db[PARABOLIC].size	= sizeof(ObjP);
	db[EARTHSAT].size	= sizeof(ObjES);
	db[PLANET].size		= sizeof(Obj); /* *NOT* ObjPl */

	/* init the planets.
	 * because the number is fixed, we use static storage for the NOBJ
	 * Objects for the PLANET type.
	 */
	db[PLANET].dblist = plan_dblist;
	db[PLANET].nmem = NOBJ;
	db[PLANET].nobj = NOBJ;
	db[PLANET].ncp = NOBJ;
	for (i = MERCURY; i <= MOON; i++) {
	    Obj *op = OBJP (PLANET, i);
	    op->type = PLANET;
	    (void) strncpy (op->o_name, planet_names[i], sizeof(op->o_name)-1);
	    op->pl.code = i;
	}
}

/* qsort comparison function (for use by db_nodups()):
 */
static int
dup_fixedcmp (v1, v2)
qsort_arg v1, v2;
{
	int n1 = *((int *)v1);
	int n2 = *((int *)v2);
	Obj *op1 = OBJP (FIXED, n1);
	Obj *op2 = OBJP (FIXED, n2);
	double d;
	int di;

	/* first by dec */
	d = op1->f_dec - op2->f_dec;
	if (d < 0)
	    return (-1);
	if (d > 0)
	    return (1);

	/* then by ra */
	d = op1->f_RA - op2->f_RA;
	if (d < 0)
	    return (-1);
	if (d > 0)
	    return (1);

	/* then by the other things we check */
	di = (int)op1->f_class - (int)op2->f_class;
	if (di)
	    return(di);

	di = (int)op1->f_mag - (int)op2->f_mag;
	if (di)
	    return (di);

	di = (int)op1->f_spect[0] - (int)op2->f_spect[0];
	if (di)
	    return (di);

	return (0);
}

/* qsort n comparison function (for use by db_nodups()) */
static int
dup_ncmpf (v1, v2)
qsort_arg v1, v2;
{
	int n1 = *((int *)v1);
	int n2 = *((int *)v2);

	return (n1 - n2);
}

/* eliminate duplicate FIXED objects, ie, that are very close together and
 * which have similar characteristics. always keep the oldest one. never
 * discard any ahead of the checkpoint.
 */
static void
db_nodups()
{
	Now *np = mm_get_now();
	int t = FIXED;
	DBMem *dbm;
	Obj *op1, *op2;
	int *idx;
	int n, s;
	int i, j;
	int nxtidx;
	int ndups;
	float e;

	watch_cursor (1);

	/* handy references */
	dbm = &db[t];
	n = dbm->nobj;
	s = dbm->size;

	/* precess everything to epoch.
	 * store in float for better comparison with f_epoch.
	 */
	e = (float) ((epoch == EOD) ? mjd : epoch);
	for (i = 0; i < n; i++) {
	    Obj *op = OBJP (FIXED, i);
	    if (op->f_epoch != e) {
		double tra = op->f_RA, tdec = op->f_dec;
		precess (op->f_epoch, e, &tra, &tdec);
		op->f_epoch = e;
		op->f_RA = tra;
		op->f_dec = tdec;
	    }
	}

	/* build a list of indices sorted by the dup criteria */
	idx = (int *) malloc (n * sizeof(int));
	if (!idx) {
	    xe_msg ("No memory for dup temp array.", 0);
	    return;
	}
	for (i = 0; i < n; i++)
	    idx[i] = i;
	qsort ((void *)idx, n, sizeof(int), dup_fixedcmp);

	/* check each pair for equality, and move dups to the front of the list.
	 * this will leave us with ndups dups at the front of idx.
	 */
	for (i = 1, ndups = 0; i < n; i++) {
	    op1 = OBJP(t,idx[i]);
	    op2 = OBJP(t,idx[i-1]);

	    /* N.B. be sure the things we check for here are included in the
	     * checks within dup_fixedcmp() so they group adjacently.
	     */
	    if (op1->f_class == op2->f_class &&
		    op1->f_mag == op2->f_mag &&
		    op1->f_spect[0] == op2->f_spect[0] &&
		    op1->f_dec - op2->f_dec <= MAXDUPSEP &&
		    delra (op1->f_RA-op2->f_RA)*cos(op1->f_dec) <= MAXDUPSEP) {

		/* discard the newest entry -- the one at the highest op  */
		idx[ndups++] = op1 > op2 ? idx[i] : idx[i-1];

#ifdef WANT_DUPREPORT
		{
		    Obj *keep, *toss;

		    if (op1 > op2) {
			toss = op1;
			keep = op2;
		    } else {
			toss = op2;
			keep = op1;
		    }

		    printf ("Tossing %s [%g %g] due to %s [%g %g]\n",
			toss->o_name, radhr(toss->f_RA), raddeg(toss->f_dec),
			keep->o_name, radhr(keep->f_RA), raddeg(keep->f_dec));
		}
#endif /* WANT_DUPREPORT */
	    }
	}

	if (ndups == 0)
	    goto done;	/* no dups */

	/* sort the dup list in ascending order for faster checking */
	qsort ((void *)idx, ndups, sizeof(int), dup_ncmpf);

	/* now compress each of the ndups entries in idx out of the real db.
	 * don't discard anything prior to the last checkpoint.
	 * consider starting at idx[0] since it's the first dup to go.
	 * exit loop with j being the new net number of objects.
	 */
	nxtidx = 0;
	for (i = j = idx[0] > dbm->ncp ? idx[0] : dbm->ncp; i < n; i++) {
	    int in, k;

	    /* see if i is in idx[].
	     * nxtidx and testing idx[k] against i keeps a bound on the
	     * portion of idx which might contain i.
	     */
	    in = 0;
	    for (k = nxtidx; k < ndups && idx[k] <= i; k++)
		if (idx[k] == i) {
		    in = 1;
		    nxtidx = k+1;
		    break;
		}

	    /* if i is in idx[] then it's a dup and we don't copy it */
	    if (!in) {
		/* copy i to j (avoid a copy onto itself) */
		if (i > j) {
		    op1 = OBJP(t,i);
		    op2 = OBJP(t,j);
		    (void) memcpy ((void *)op2, (void *)op1, s);
		}
		j++;
	    }
	}

	dbm->nobj = j;

    done:
	free ((char *)idx);

	watch_cursor (0);
}

/* read database file fp and put next valid entry (sans trailing \n) into buf.
 * we only count those lines that begin with alpha or numeric chars.
 * return 0 if ok.
 * if eof: return -1; caller will find that feof(fp) is true;
 * other errors: print a message and return -1.
 */
static int
nxt_db (buf, blen, fp)
char buf[];
int blen;
FILE *fp;
{
	int l;

	for (;;) {
	    if (fgets (buf, blen, fp) == 0)
		return (-1);
	    l = strlen(buf);
	    if (buf[l-1] != '\n') {
		xe_msg("Database file line length is too long or corrupted.",0);
		return (-1);
	    }
	    if (line_candidate (buf) == 0) {
		buf[l-1] = '\0';
		return (0);
	    }
	}
}

/* return 0 if buf qualifies as a database line worthy of a cracking
 * attempt, else -1.
 */
static int
line_candidate (buf)
char *buf;
{
	char c = buf[0];

	return (isalpha(c) || isdigit(c) ? 0 : -1);
}

/* given either a decimal year (xxxx[.xxx]) or a calendar (x/x/x)
 * and a DateFormat preference convert it to an mjd and store it at *p.
 */
static void
crack_year (bp, pref, p)
char *bp;
PrefDateFormat pref;
double *p;
{
	int m, y;
	double d;

	mjd_cal (*p, &m, &d, &y);	/* init with current */
	f_sscandate (bp, pref, &m, &d, &y);
	cal_mjd (m, d, y, p);
}

/* given a null-terminated string, fill in fields[] with the starting addresses
 * of each field delimited by delim or '\0'.
 * N.B. each character matching delim is REPLACED BY '\0' IN PLACE.
 * N.B. 0-length fields count, so even if *s=='\0' we return 1.
 * return the number of fields.
 */
static int
get_fields (s, delim, fields)
char *s;
int delim;
char *fields[];
{
	int n;
	char c;

	*fields = s;
	n = 0;
	do {
	    c = *s++;
	    if (c == delim || c == '\0') {
		s[-1] = '\0';
		*++fields = s;
		n++;
	    }
	} while (c);

	return (n);
}

/* called whenever there is input readable from the db fifo.
 * read and crack what we can.
 * be prepared for partial lines split across reads.
 * N.B. do EXACTLY ONE read -- don't know that more won't block.
 * set the watch cursor while we work and call all_newdb() when we're done.
 *   we guess we are "done" when we end up without a partial line.
 */
/* ARGSUSED */
static void
dbfifo_cb (client, fdp, idp)
XtPointer client;       /* file name */
int *fdp;               /* pointer to file descriptor */
XtInputId *idp;         /* pointer to input id */
{
	static char partial[MAXDBLINE];	/* partial line from before */
	static int npartial;		/* length of stuff in partial[] */
	char buf[16*1024];		/* nice big read gulps */
	char *name = (char *)client;	/* fifo filename */
	char msg[1024];			/* error message buffer */
	int nr;				/* number of bytes read from fifo */

	/* turn on the watch cursor if there's no prior line */
	if (!npartial)
	    watch_cursor (1);

	/* catch up where we left off from last time */
	if (npartial)
	    (void) strcpy (buf, partial);

	/* read what's available up to the room we have left.
	 * if we have no room left, it will look like an EOF.
	 */
	nr = read (db_fifofd, buf+npartial, sizeof(buf)-npartial);

	if (nr > 0) {
	    char c, *lp, *bp, *ep;	/* last line, current, end */

	    /* process each whole line */
	    ep = buf + npartial + nr;
	    for (lp = bp = buf; bp < ep; ) {
		c = *bp++;
		if (c == '\n') {
		    bp[-1] = '\0';		      /* replace nl with EOS */
		    if (*lp == DBFIFO_MSG) {
			(void)sprintf(msg,"DBFIFO message: %s", lp+1);
			xe_msg (msg, 0);
		    } else if (line_candidate (lp) == 0) {
			if (db_crack_line (lp) < 0)
			    xe_msg ("Error in data format from fifo stream", 0);
		    }
		    lp = bp;
		}
	    }

	    /* save any partial line for next time */
	    npartial = ep - lp;
	    if (npartial > 0) {
		if (npartial > sizeof(partial)) {
		    (void)sprintf(msg,"Discarding long line in %.100s.\n",name);
		    xe_msg (msg, 0);
		    npartial = 0;
		} else {
		    *ep = '\0';
		    (void) strcpy (partial, lp);
		}
	    }

	} else {
	    if (nr < 0)
		(void) sprintf (msg, "Error reading %.150s: %.50s.\n",
							name, syserrstr());
	    else 
		(void) sprintf (msg, "Unexpected EOF on %.200s.\n", name);
	    xe_msg (msg, 1);
	    XtRemoveInput (db_fifoid);
	    (void) close (db_fifofd);
	    db_fifofd = -1;
	    npartial = 0;
	}

	/* turn off the watch cursor if there is not likely to be more coming.
	 * Also, throw out dups and inform everyone about all the new stuff.
	 */
	if (!npartial) {
	    if (db_wantnodups)
		db_nodups();
	    all_newdb (1);
	    watch_cursor (0);
	}
}
