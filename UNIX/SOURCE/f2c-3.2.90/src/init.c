/****************************************************************
Copyright 1990 by AT&T Bell Laboratories and Bellcore.

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the names of AT&T Bell Laboratories or
Bellcore or any of their entities not be used in advertising or
publicity pertaining to distribution of the software without
specific, written prior permission.

AT&T and Bellcore disclaim all warranties with regard to this
software, including all implied warranties of merchantability
and fitness.  In no event shall AT&T or Bellcore be liable for
any special, indirect or consequential damages or any damages
whatsoever resulting from loss of use, data or profits, whether
in an action of contract, negligence or other tortious action,
arising out of or in connection with the use or performance of
this software.
****************************************************************/

#include "defs.h"
#include "output.h"
#include "iob.h"

/* State required for the C output */
char *fl_fmt_string;		/* Float format string */
char *db_fmt_string;	    	/* Double format string */
char *cm_fmt_string;		/* Complex format string */
char *dcm_fmt_string;		/* Double complex format string */

chainp new_vars = CHNULL;	/* List of newly created locals in this
				   function.  These may have identifiers
				   which have underscores and more than VL
				   characters */
chainp used_builtins = CHNULL;	/* List of builtins used by this function.
				   These are all Addrps with UNAM_EXTERN
				   */
chainp io_blocks = CHNULL;	/* ioblocks used by this function */
chainp assigned_fmts = CHNULL;	/* assigned formats */
chainp allargs;			/* union of args in all entry points */
char main_alias[52];		/* PROGRAM name, if any is given */
int tab_size = 4;


FILEP infile	= {
	stdin };
FILEP diagfile	= {
	stderr };

FILEP c_file;
FILEP pass1_file;
FILEP initfile;
FILEP blkdfile;
long int headoffset;


char token[MAXTOKENLEN];
int toklen;
long lineno;			/* Current line in the input file, NOT the
				   Fortran statement label number */
char *infname;
int needkwd;
struct Labelblock *thislabel	= NULL;
flag profileflag	= NO;
flag optimflag	= NO;
int nerr;
int nwarn;

flag saveall;
flag substars;
int parstate	= OUTSIDE;
flag headerdone	= NO;
int blklevel;
int impltype[26];
int implleng[26];
int implstg[26];

int tyint	= TYLONG ;
int tylogical	= TYLONG;
int typesize[NTYPES] = {
	1, SZADDR, SZSHORT, SZLONG, SZLONG, 2*SZLONG,
	    2*SZLONG, 4*SZLONG, SZLONG, 1, 1, 0,
		4*SZLONG + SZADDR,	/* sizeof(cilist) */
		4*SZLONG + 2*SZADDR,	/* sizeof(icilist) */
		4*SZLONG + 5*SZADDR,	/* sizeof(olist) */
		2*SZLONG + SZADDR,	/* sizeof(cllist) */
		2*SZLONG,		/* sizeof(alist) */
		11*SZLONG + 15*SZADDR	/* sizeof(inlist) */
		};

int typealign[NTYPES] = {
	1, ALIADDR, ALISHORT, ALILONG, ALILONG, ALIDOUBLE,
	ALILONG, ALIDOUBLE, ALILONG, 1, 1, 1,
	ALILONG, ALILONG, ALILONG, ALILONG, ALILONG, ALILONG};

int type_choice[4] = { TYDREAL, TYSHORT, TYLONG,  TYSHORT };

char *typename[] = {
	"<<unknown>>",
	"address",
	"shortint",
	"integer",
	"real",
	"doublereal",
	"complex",
	"doublecomplex",
	"logical",
	"char"	/* character */
	};

int type_pref[NTYPES] = { 0, 0, 2, 4, 5, 7, 6, 8, 3, 1 };

char *protorettypes[] = {
	"?", "??", "shortint", "integer", "real", "doublereal",
	"C_f", "Z_f", "logical", "H_f", "int"
	};

char *casttypes[TYSUBR+1] = {
	"U_fp", "??bug??",
	"J_fp", "I_fp", "R_fp",
	"D_fp", "C_fp", "Z_fp",
	"L_fp", "H_fp", "S_fp"
	};
char *usedcasts[TYSUBR+1];

char *dfltarg[] = {
	0, 0,
	"(shortint *)0", "(integer *)0", "(real *)0",
	"(doublereal *)0", "(complex *)0", "(doublecomplex *)0",
	"(logical *)0", "(char *)0"
	};

static char *dflt0proc[] = {
	0, 0,
	"(shortint (*)())0", "(integer (*)())0", "(real (*)())0",
	"(doublereal (*)())0", "(complex (*)())0", "(doublecomplex (*)())0",
	"(logical (*)())0", "(char (*)())0", "(int (*)())0"
	};

char *dflt1proc[] = { "(U_fp)0", "(??bug??)0",
	"(J_fp)0", "(I_fp)0", "(R_fp)0",
	"(D_fp)0", "(C_fp)0", "(Z_fp)0",
	"(L_fp)0", "(H_fp)0", "(S_fp)0"
	};

char **dfltproc = dflt0proc;

char *ftn_types[] = { "external", "??",
	"integer*2", "integer", "real",
	"double precision", "complex", "double complex",
	"logical", "character", "subroutine"
	};

int procno;
int lwmno;
int proctype	= TYUNKNOWN;
char *procname;
int rtvlabel[NTYPES0];
int fudgelabel;
Addrp retslot;			/* Holds automatic variable which was
				   allocated the function return value
				   */
Addrp retslotx[NTYPES0];	/* for multiple entry points */
int cxslot	= -1;
int chslot	= -1;
int chlgslot	= -1;
int procclass	= CLUNKNOWN;
int nentry;
int nallargs;
int nallchargs;
flag multitype;
ftnint procleng;
long lastiolabno;
int lastlabno;
int lastvarno;
int lastargslot;
int argloc;
int autonum[TYVOID];
char *av_pfix[TYVOID] = {"??TYUNKNOWN??", "a","s","i","r","d","q","z","L","ch",
			 "??TYSUBR??", "??TYERROR??","ci", "ici",
			 "o", "cl", "al", "ioin" };

extern int maxctl;
struct Ctlframe *ctls;
struct Ctlframe *ctlstack;
struct Ctlframe *lastctl;

Namep regnamep[MAXREGVAR];
int highregvar;
int nregvar;

extern int maxext;
struct Extsym *extsymtab;
struct Extsym *nextext;
struct Extsym *lastext;

extern int maxequiv;
struct Equivblock *eqvclass;

extern int maxhash;
struct Hashentry *hashtab;
struct Hashentry *lasthash;

extern int maxstno;		/* Maximum number of statement labels */
struct Labelblock *labeltab;
struct Labelblock *labtabend;
struct Labelblock *highlabtab;

int maxdim	= MAXDIM;
struct Rplblock *rpllist	= NULL;
struct Chain *curdtp	= NULL;
flag toomanyinit;
ftnint curdtelt;
chainp templist[TYVOID];
chainp holdtemps;
int dorange	= 0;
struct Entrypoint *entries	= NULL;

chainp chains	= NULL;

flag inioctl;
int iostmt;
int nioctl;
int nequiv	= 0;
int eqvstart	= 0;
int nintnames	= 0;

struct Literal litpool[MAXLITERALS];
int nliterals;

char dflttype[26];
char hextoi_tab[256];

FILE *freopen ();

fileinit()
{
	register char *s;
	register int i;

	procno = 0;
	lwmno = 0;
	lastiolabno = 100000;
	lastlabno = 0;
	lastvarno = 0;
	nliterals = 0;
	nerr = 0;

	memset(dflttype, tyreal, 26);
	memset(dflttype + 'i' - 'a', tyint, 6);
	memset(hextoi_tab, 16, sizeof(hextoi_tab));
	for(i = 0, s = "0123456789abcdef"; *s; i++, s++)
		hextoi(*s) = i;
	for(i = 10, s = "ABCDEF"; *s; i++, s++)
		hextoi(*s) = i;

	ctls = ALLOCN(maxctl, Ctlframe);
	extsymtab = ALLOCN(maxext, Extsym);
	eqvclass = ALLOCN(maxequiv, Equivblock);
	hashtab = ALLOCN(maxhash, Hashentry);
	labeltab = ALLOCN(maxstno, Labelblock);

	ctlstack = ctls - 1;
	lastctl = ctls + maxctl;
	nextext = extsymtab;
	lastext = extsymtab + maxext;
	lasthash = hashtab + maxhash;
	labtabend = labeltab + maxstno;
	highlabtab = labeltab;
	main_alias[0] = '\0';
	if (forcedouble)
		dfltproc[TYREAL] = dfltproc[TYDREAL];

/* Initialize the routines for providing C output */

	output_init ();
}

hashclear()	/* clear hash table */
{
	register struct Hashentry *hp;
	register Namep p;
	register struct Dimblock *q;
	register int i;

	for(hp = hashtab ; hp < lasthash ; ++hp)
		if(p = hp->varp)
		{
			frexpr(p->vleng);
			if(q = p->vdim)
			{
				for(i = 0 ; i < q->ndim ; ++i)
				{
					frexpr(q->dims[i].dimsize);
					frexpr(q->dims[i].dimexpr);
				}
				frexpr(q->nelt);
				frexpr(q->baseoffset);
				frexpr(q->basexpr);
				free( (charptr) q);
			}
			if(p->vclass == CLNAMELIST)
				frchain( &(p->varxptr.namelist) );
			free( (charptr) p);
			hp->varp = NULL;
		}
	}

procinit()
{
	register struct Labelblock *lp;
	struct Chain *cp;
	int i;
	extern struct memblock *curmemblock, *firstmemblock;
	extern char *mem_first, *mem_next, *mem_last, *mem0_last;
	extern void frexchain();

	curmemblock = firstmemblock;
	mem_next = mem_first;
	mem_last = mem0_last;
	iob_list = 0;
	for(i = 0; i < 9; i++)
		io_structs[i] = 0;

	parstate = OUTSIDE;
	headerdone = NO;
	blklevel = 1;
	saveall = NO;
	substars = NO;
	nwarn = 0;
	thislabel = NULL;
	needkwd = 0;

	++procno;
	proctype = TYUNKNOWN;
	procname = "MAIN_";
	procclass = CLUNKNOWN;
	nentry = 0;
	nallargs = nallchargs = 0;
	multitype = NO;
	retslot = NULL;
	for(i = 0; i < NTYPES0; i++) {
		frexpr(retslotx[i]);
		retslotx[i] = 0;
		}
	cxslot = -1;
	chslot = -1;
	chlgslot = -1;
	procleng = 0;
	blklevel = 1;
	lastargslot = 0;

	for(lp = labeltab ; lp < labtabend ; ++lp)
		lp->stateno = 0;

	hashclear();

/* Clear the list of newly generated identifiers from the previous
   function */

	frexchain(&new_vars);
	frexchain(&used_builtins);
	frchain(&io_blocks);
	frchain(&assigned_fmts);
	frchain(&allargs);

	nintnames = 0;
	highlabtab = labeltab;

	ctlstack = ctls - 1;
	for(i = TYADDR; i < TYVOID; i++) {
		for(cp = templist[i]; cp ; cp = cp->nextp)
			free( (charptr) (cp->datap) );
		frchain(templist + i);
		autonum[i] = 0;
		}
	holdtemps = NULL;
	dorange = 0;
	nregvar = 0;
	highregvar = 0;
	entries = NULL;
	rpllist = NULL;
	inioctl = NO;
	eqvstart += nequiv;
	nequiv = 0;
	dcomplex_seen = 0;

	for(i = 0 ; i<NTYPES0 ; ++i)
		rtvlabel[i] = 0;
	fudgelabel = 0;

	if(undeftype)
		setimpl(TYUNKNOWN, (ftnint) 0, 'a', 'z');
	else
	{
		setimpl(tyreal, (ftnint) 0, 'a', 'z');
		setimpl(tyint,  (ftnint) 0, 'i', 'n');
	}
	setimpl(-STGBSS, (ftnint) 0, 'a', 'z');	/* set class */
	setlog();
}




setimpl(type, length, c1, c2)
int type;
ftnint length;
int c1, c2;
{
	int i;
	char buff[100];

	if(c1==0 || c2==0)
		return;

	if(c1 > c2)
	{
		sprintf(buff, "characters out of order in implicit:%c-%c", c1, c2);
		err(buff);
	}
	else
		if(type < 0)
			for(i = c1 ; i<=c2 ; ++i)
				implstg[i-'a'] = - type;
		else
		{
			type = lengtype(type, (int) length);
			if(type != TYCHAR)
				length = 0;
			for(i = c1 ; i<=c2 ; ++i)
			{
				impltype[i-'a'] = type;
				implleng[i-'a'] = length;
			}
		}
}
