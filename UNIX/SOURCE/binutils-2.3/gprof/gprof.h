/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)gprof.h	5.9 (Berkeley) 6/1/90
 */

#include <ansidecl.h>
#include "sysdep.h"
#include "bfd.h"
#include "gmon.h"

/* AIX defines hz as a macro.  */
#undef hz

#ifdef	MACHINE_H
#	include	MACHINE_H
#else
#	if vax
#	   include "vax.h"
#	endif
#	if sun
#		include "sun.h"
#	endif
#	if tahoe
#		include "tahoe.h"
#	endif
#endif


    /*
     *	who am i, for error messages.
     */
char	*whoami;

    /*
     * booleans
     */
typedef int	bool;
#define	FALSE	0
#define	TRUE	1

    /*
     *	ticks per second
     */
long	hz;

typedef	unsigned char UNIT[2];		/* unit of profiling */
char	*a_outname;
#define	A_OUTNAME		"a.out"

char	*gmonname;
#define	GMONNAME		"gmon.out"
#define	GMONSUM			"gmon.sum"

extern int bsd_style_output;
extern int discard_underscores;

    /*
     *	a constructed arc,
     *	    with pointers to the namelist entry of the parent and the child,
     *	    a count of how many times this arc was traversed,
     *	    and pointers to the next parent of this child and
     *		the next child of this parent.
     */
struct arcstruct {
    struct nl		*arc_parentp;	/* pointer to parent's nl entry */
    struct nl		*arc_childp;	/* pointer to child's nl entry */
    long		arc_count;	/* how calls from parent to child */
    double		arc_time;	/* time inherited along arc */
    double		arc_childtime;	/* childtime inherited along arc */
    struct arcstruct	*arc_parentlist; /* parents-of-this-child list */
    struct arcstruct	*arc_childlist;	/* children-of-this-parent list */
};
typedef struct arcstruct	arctype;

    /*
     * The symbol table;
     * for each external in the specified file we gather
     * its address, the number of calls and compute its share of cpu time.
     */
struct nl {
    CONST char		*name;		/* the name */
    unsigned long	value;		/* the pc entry point */
    unsigned long	svalue;		/* entry point aligned to histograms */
    double		time;		/* ticks in this routine */
    double		childtime;	/* cumulative ticks in children */
    long		ncall;		/* how many times called */
    long		selfcalls;	/* how many calls to self */
    double		propfraction;	/* what % of time propagates */
    double		propself;	/* how much self time propagates */
    double		propchild;	/* how much child time propagates */
    bool		printflag;	/* should this be printed? */
    int			index;		/* index in the graph list */
    int			toporder;	/* graph call chain top-sort order */
    int			cycleno;	/* internal number of cycle on */
    struct nl		*cyclehead;	/* pointer to head of cycle */
    struct nl		*cnext;		/* pointer to next member of cycle */
    arctype		*parents;	/* list of caller arcs */
    arctype		*children;	/* list of callee arcs */
};
typedef struct nl	nltype;

nltype	*nl;			/* the whole namelist */
nltype	*npe;			/* the virtual end of the namelist */
int	nname;			/* the number of function names */

    /*
     *	flag which marks a nl entry as topologically ``busy''
     *	flag which marks a nl entry as topologically ``not_numbered''
     */
#define	DFN_BUSY	-1
#define	DFN_NAN		0

    /* 
     *	namelist entries for cycle headers.
     *	the number of discovered cycles.
     */
nltype	*cyclenl;		/* cycle header namelist */
int	ncycle;			/* number of cycles discovered */

    /*
     * The header on the gmon.out file.
     * gmon.out consists of one of these headers,
     * and then an array of ncnt samples
     * representing the discretized program counter values.
     *	this should be a struct phdr, but since everything is done
     *	as UNITs, this is in UNITs too.
     */
struct hdr {
    UNIT	*lowpc;
    UNIT	*highpc;
    int	ncnt;
};


struct rawhdr {
    char lowpc[4];
    char highpc[4];
    char ncnt[4];
};

struct hdr	h;

int	debug;

    /*
     * Each discretized pc sample has
     * a count of the number of samples in its range
     */
int 	*samples;

unsigned long	s_lowpc;	/* lowpc from the profile file */
unsigned long	s_highpc;	/* highpc from the profile file */
unsigned lowpc, highpc;		/* range profiled, in UNIT's */
unsigned sampbytes;		/* number of bytes of samples */
int	nsamples;		/* number of samples */
double	actime;			/* accumulated time thus far for putprofline */
double	totime;			/* total time for all routines */
double	printtime;		/* total of time being printed */
double	scale;			/* scale factor converting samples to pc
				   values: each sample covers scale bytes */
char	*strtab;		/* string table in core */
off_t	ssiz;			/* size of the string table */
unsigned char	*textspace;		/* text space of a.out in core */

    /*
     *	option flags, from a to z.
     */
bool	aflag;				/* suppress static functions */
bool	bflag;				/* blurbs, too */
bool	cflag;				/* discovered call graph, too */
bool	dflag;				/* debugging options */
bool	eflag;				/* specific functions excluded */
bool	Eflag;				/* functions excluded with time */
bool	fflag;				/* specific functions requested */
bool	Fflag;				/* functions requested with time */
bool	kflag;				/* arcs to be deleted */
bool	sflag;				/* sum multiple gmon.out files */
bool	zflag;				/* zero time/called functions, too */

    /*
     *	structure for various string lists
     */
struct stringlist {
    struct stringlist	*next;
    char		*string;
};
struct stringlist	*elist;
struct stringlist	*Elist;
struct stringlist	*flist;
struct stringlist	*Flist;
struct stringlist	*kfromlist;
struct stringlist	*ktolist;

    /*
     *	function declarations
     */
/*
		addarc();
*/
int		arccmp();
arctype		*arclookup();
/*
		asgnsamples();
		printblurb();
		cyclelink();
		dfn();
*/
bool		dfn_busy();
/*
		dfn_findcycle();
*/
bool		dfn_numbered();
/*
		dfn_post_visit();
		dfn_pre_visit();
		dfn_self_cycle();
*/
nltype		**doarcs();
/*
		done();
		findcalls();
		flatprofheader();
		flatprofline();
*/
bool		funcsymbol();
/*
		getnfile();
		getpfile();
		getstrtab();
		getsymtab();
		gettextspace();
		gprofheader();
		gprofline();
		main();
*/
unsigned long	max();
int		membercmp();
unsigned long	min();
nltype		*nllookup();
FILE		*openpfile();
/*
		printchildren();
		printcycle();
		printgprof();
		printmembers();
		printname();
		printparents();
		printprof();
		readsamples();
*/
int		printnameonly();
unsigned long	reladdr();
/*
		sortchildren();
		sortmembers();
		sortparents();
		tally();
		timecmp();
		topcmp();
*/
int		totalcmp();
/*
		valcmp();
*/

#define	LESSTHAN	-1
#define	EQUALTO		0
#define	GREATERTHAN	1

#define	DFNDEBUG	1
#define	CYCLEDEBUG	2
#define	ARCDEBUG	4
#define	TALLYDEBUG	8
#define	TIMEDEBUG	16
#define	SAMPLEDEBUG	32
#define	AOUTDEBUG	64
#define	CALLDEBUG	128
#define	LOOKUPDEBUG	256
#define	PROPDEBUG	512
#define	ANYDEBUG	1024
