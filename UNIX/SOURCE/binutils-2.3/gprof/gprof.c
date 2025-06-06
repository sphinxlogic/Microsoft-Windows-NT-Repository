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
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1983 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)gprof.c	5.6 (Berkeley) 6/1/90";
#endif /* not lint */

#include "gprof.h"

bfd	*abfd;

char	*whoami;

    /*
     *	things which get -E excluded by default.
     */
char	*defaultEs[] = { "mcount" , "__mcleanup" , 0 };

int discard_underscores = 1; /* Should we discard initial underscores? */
int bsd_style_output = 0; /* As opposed to FSF style output */

main(argc, argv)
    int argc;
    char **argv;
{
    char	**sp;
    nltype	**timesortnlp;

    whoami = argv[0];
    --argc;
    argv++;
    debug = 0;
    bflag = TRUE;
    while ( *argv != 0 && **argv == '-' ) {
	(*argv)++;
	switch ( **argv ) {
	case 'a':
	    aflag = TRUE;
	    break;
	case 'b':
	    bflag = FALSE;
	    break;
	case 'c':
	    cflag = TRUE;
	    break;
	case 'd':
	    dflag = TRUE;
	    (*argv)++;
	    debug |= atoi( *argv );
	    debug |= ANYDEBUG;
#	    ifdef DEBUG
		printf("[main] debug = %d\n", debug);
#	    else not DEBUG
		printf("%s: -d ignored\n", whoami);
#	    endif DEBUG
	    break;
	case 'E':
	    ++argv;
	    addlist( Elist , *argv );
	    Eflag = TRUE;
	    addlist( elist , *argv );
	    eflag = TRUE;
	    break;
	case 'e':
	    addlist( elist , *++argv );
	    eflag = TRUE;
	    break;
	case 'F':
	    ++argv;
	    addlist( Flist , *argv );
	    Fflag = TRUE;
	    addlist( flist , *argv );
	    fflag = TRUE;
	    break;
	case 'f':
	    addlist( flist , *++argv );
	    fflag = TRUE;
	    break;
	case 'k':
	    addlist( kfromlist , *++argv );
	    addlist( ktolist , *++argv );
	    kflag = TRUE;
	    break;
	case 's':
	    sflag = TRUE;
	    break;
	case 'T': /* "Traditional" output format */
	    bsd_style_output = 1;
	    break;
	case 'z':
	    zflag = TRUE;
	    break;
	default:
	    fprintf (stderr, "\
Usage: %s [-a] [-b] [-c] [-d[num]] [-E function-name] [-e function-name]\n\
       [-F function-name] [-f function-name] [-k from to] [-s] [-T] [-z]\n\
       [image-file] [profile-file...]\n", whoami);
	    exit (1);
	}
	argv++;
    }
    if ( *argv != 0 ) {
	a_outname  = *argv;
	argv++;
    } else {
	a_outname  = A_OUTNAME;
    }
    if ( *argv != 0 ) {
	gmonname = *argv;
	argv++;
    } else {
	gmonname = GMONNAME;
    }
	/*
	 *	turn off default functions
	 */
    for ( sp = &defaultEs[0] ; *sp ; sp++ ) {
	Eflag = TRUE;
	addlist( Elist , *sp );
	eflag = TRUE;
	addlist( elist , *sp );
    }
	/*
	 *	how many ticks per second?
	 *	if we can't tell, report time in ticks.
	 */
    hz = hertz();
    if (hz == 0) {
	hz = 1;
	fprintf(stderr, "time is in ticks, not seconds\n");
    }
	/*
	 *	get information about a.out file.
	 */
    getnfile();
	/*
	 *	get information about mon.out file(s).
	 */
    do	{
	getpfile( gmonname );
	if ( *argv != 0 ) {
	    gmonname = *argv;
	}
    } while ( *argv++ != 0 );
	/*
	 *	dump out a gmon.sum file if requested
	 */
    if ( sflag ) {
	dumpsum( GMONSUM );
    }
	/*
	 *	assign samples to procedures
	 */
    asgnsamples();
	/*
	 *	assemble the dynamic profile
	 */
    timesortnlp = doarcs();
	
    if (bsd_style_output) {
	printgprof( timesortnlp );	/* print the dynamic profile */
	printprof();  /* print the flat profile */
    } else {
	printprof();  /* print the flat profile */
	printgprof( timesortnlp );	/* print the dynamic profile */
    }
	/*
	 *	print the index
	 */
    printindex();	
    done();
}

    /*
     * Set up string and symbol tables from a.out.
     *	and optionally the text space.
     * On return symbol table is sorted by value.
     */
getnfile()
{
  int		valcmp();

  abfd = bfd_openr (a_outname, NULL);

  if (abfd == NULL) {
    perror (a_outname);
    done();
  }

  if (!bfd_check_format (abfd, bfd_object)) {
    fprintf (stderr, "%s: %s: bad format\n", whoami, a_outname);
    done();
  }

/*  getstrtab(nfile); */
  getsymtab(abfd);
  gettextspace( abfd );
  qsort(nl, nname, sizeof(nltype), valcmp);

#   ifdef DEBUG
  if ( debug & AOUTDEBUG ) {
    register int j;
    
    for (j = 0; j < nname; j++){
      printf("[getnfile] 0X%08x\t%s\n", nl[j].value, nl[j].name);
    }
  }
#   endif DEBUG
}

/*
 * Read in symbol table
 */
getsymtab(abfd)
bfd	*abfd;
{
    register long	i;
    int			askfor;
    int			nosyms;
    asymbol		**syms;
    i = get_symtab_upper_bound (abfd);	/* This will probably give us more
					 * than we need, but that's ok.
					 */
    syms = (asymbol**)xmalloc (i);
    nosyms = bfd_canonicalize_symtab (abfd, syms);

    nname = 0;
    for (i = 0; i < nosyms; i++) {
      if (!funcsymbol (syms[i]))
	continue;
      nname++;
    }

    if (nname == 0) {
      fprintf(stderr, "%s: %s: no symbols\n", whoami , a_outname );
      done();
    }
    askfor = nname + 1;
    nl = (nltype *) calloc( askfor , sizeof(nltype) );
    if (nl == 0) {
	fprintf(stderr, "%s: No room for %d bytes of symbol table\n",
		whoami, askfor * sizeof(nltype) );
	done();
    }

    /* pass2 - read symbols */
    npe = nl;
    nname = 0;
    for (i = 0; i < nosyms; i++) {
      if (!funcsymbol (syms[i])) {
#	    ifdef DEBUG
	if ( debug & AOUTDEBUG ) {
	  printf( "[getsymtab] rejecting: 0x%x %s\n" ,
		 syms[i]->value, syms[i]->name);
	}
#	    endif DEBUG
	continue;
      }
      /* Symbol offsets are always section-relative. */
      npe->value = syms[i]->value + syms[i]->section->vma;
      npe->name = syms[i]->name;

      /* If we see "main" without an initial '_', we assume
	 names are *not* prefixed by '_'. */
      if (npe->name[0] == 'm' && discard_underscores
	  && strcmp(npe->name, "main") == 0)
	  discard_underscores = 0;

#	ifdef DEBUG
      if ( debug & AOUTDEBUG ) {
	printf( "[getsymtab] %d %s 0x%08x\n" ,
	       nname , npe -> name , npe -> value );
      }
#	endif DEBUG
      npe++;
      nname++;
    }
    npe->value = -1;
}

/*
 *	read in the text space of an a.out file
 */
gettextspace( abfd )
     bfd	*abfd;
{
  asection	*texsec;
    
  if ( cflag == 0 ) {
    return;
  }

  texsec = bfd_get_section_by_name (abfd, ".text");
  if (texsec == NULL) {
    return;
  }

  textspace = (u_char *) malloc( texsec->_cooked_size );

  if ( textspace == 0 ) {
    fprintf( stderr , "%s: ran out room for %d bytes of text space:  " ,
	    whoami , texsec->_cooked_size);
    fprintf( stderr , "can't do -c\n" );
    return;
  }
  bfd_get_section_contents (abfd, texsec, textspace, texsec->filepos, 
			    texsec->_cooked_size);
}
/*
 *	information from a gmon.out file is in two parts:
 *	an array of sampling hits within pc ranges,
 *	and the arcs.
 */
getpfile(filename)
    char *filename;
{
    FILE		*pfile;
    FILE		*openpfile();
    struct rawarc	arc;
    struct veryrawarc	rawarc;

    pfile = openpfile(filename);
    readsamples(pfile);
	/*
	 *	the rest of the file consists of
	 *	a bunch of <from,self,count> tuples.
	 */
    while ( fread( &rawarc , sizeof rawarc , 1 , pfile ) == 1 ) {
      arc.raw_frompc = bfd_get_32 (abfd, (bfd_byte *) rawarc.raw_frompc);
      arc.raw_selfpc = bfd_get_32 (abfd, (bfd_byte *) rawarc.raw_selfpc);
      arc.raw_count  = bfd_get_32 (abfd, (bfd_byte *) rawarc.raw_count);
#	ifdef DEBUG
	    if ( debug & SAMPLEDEBUG ) {
		printf( "[getpfile] frompc 0x%x selfpc 0x%x count %d\n" ,
			arc.raw_frompc , arc.raw_selfpc , arc.raw_count );
	    }
#	endif DEBUG
	    /*
	     *	add this arc
	     */
	tally( &arc );
    }
    fclose(pfile);
}

FILE *
openpfile(filename)
    char *filename;
{
    struct hdr	tmp;
    struct rawhdr raw;
    FILE	*pfile;

    if((pfile = fopen(filename, "r")) == NULL) {
	perror(filename);
	done();
    }
    if (sizeof(struct rawhdr) !=  fread(&raw, 1, sizeof(struct rawhdr), pfile))
      {
	fprintf(stderr, "%s: file too short to be a gmon file\n", filename);
	done();
      }    
    tmp.lowpc  = (UNIT *)bfd_get_32 (abfd, (bfd_byte *) &raw.lowpc[0]);
    tmp.highpc = (UNIT *)bfd_get_32 (abfd, (bfd_byte *) &raw.highpc[0]);
    tmp.ncnt   =         bfd_get_32 (abfd, (bfd_byte *) &raw.ncnt[0]);

    if ( s_highpc != 0 && ( tmp.lowpc != h.lowpc ||
	 tmp.highpc != h.highpc || tmp.ncnt != h.ncnt ) ) {
	fprintf(stderr, "%s: incompatible with first gmon file\n", filename);
	done();
    }
    h = tmp;
    s_lowpc = (unsigned long) h.lowpc;
    s_highpc = (unsigned long) h.highpc;
    lowpc = (unsigned long)h.lowpc / sizeof(UNIT);
    highpc = (unsigned long)h.highpc / sizeof(UNIT);
    sampbytes = h.ncnt - sizeof(struct rawhdr);
    nsamples = sampbytes / sizeof (UNIT);
#   ifdef DEBUG
	if ( debug & SAMPLEDEBUG ) {
	    printf( "[openpfile] hdr.lowpc 0x%x hdr.highpc 0x%x hdr.ncnt %d\n",
		h.lowpc , h.highpc , h.ncnt );
	    printf( "[openpfile]   s_lowpc 0x%x   s_highpc 0x%x\n" ,
		s_lowpc , s_highpc );
	    printf( "[openpfile]     lowpc 0x%x     highpc 0x%x\n" ,
		lowpc , highpc );
	    printf( "[openpfile] sampbytes %d nsamples %d\n" ,
		sampbytes , nsamples );
	}
#   endif DEBUG
    return(pfile);
}

tally( rawp )
    struct rawarc	*rawp;
{
    nltype		*parentp;
    nltype		*childp;

    parentp = nllookup( rawp -> raw_frompc );
    childp = nllookup( rawp -> raw_selfpc );
    if ( kflag
	 && onlist( kfromlist , parentp -> name )
	 && onlist( ktolist , childp -> name ) ) {
	return;
    }
    childp -> ncall += rawp -> raw_count;
#   ifdef DEBUG
	if ( debug & TALLYDEBUG ) {
	    printf( "[tally] arc from %s to %s traversed %d times\n" ,
		    parentp -> name , childp -> name , rawp -> raw_count );
	}
#   endif DEBUG
    addarc( parentp , childp , rawp -> raw_count );
}

/*
 * dump out the gmon.sum file
 */
dumpsum( sumfile )
    char *sumfile;
{
    register nltype *nlp;
    register arctype *arcp;
    struct rawarc arc;
    FILE *sfile;

    if ( ( sfile = fopen ( sumfile , "w" ) ) == NULL ) {
	perror( sumfile );
	done();
    }
    /*
     * dump the header; use the last header read in
     */
    if ( fwrite( &h , sizeof h , 1 , sfile ) != 1 ) {
	perror( sumfile );
	done();
    }
    /*
     * dump the samples
     */
    if (fwrite(samples, sizeof (UNIT), nsamples, sfile) != nsamples) {
	perror( sumfile );
	done();
    }
    /*
     * dump the normalized raw arc information
     */
    for ( nlp = nl ; nlp < npe ; nlp++ ) {
	for ( arcp = nlp -> children ; arcp ; arcp = arcp -> arc_childlist ) {
	    arc.raw_frompc = arcp -> arc_parentp -> value;
	    arc.raw_selfpc = arcp -> arc_childp -> value;
	    arc.raw_count = arcp -> arc_count;
	    if ( fwrite ( &arc , sizeof arc , 1 , sfile ) != 1 ) {
		perror( sumfile );
		done();
	    }
#	    ifdef DEBUG
		if ( debug & SAMPLEDEBUG ) {
		    printf( "[dumpsum] frompc 0x%x selfpc 0x%x count %d\n" ,
			    arc.raw_frompc , arc.raw_selfpc , arc.raw_count );
		}
#	    endif DEBUG
	}
    }
    fclose( sfile );
}

valcmp(p1, p2)
    nltype *p1, *p2;
{
    if ( p1 -> value < p2 -> value ) {
	return LESSTHAN;
    }
    if ( p1 -> value > p2 -> value ) {
	return GREATERTHAN;
    }
    return EQUALTO;
}

readsamples(pfile)
    FILE	*pfile;
{
  register i;

    
  if (samples == 0) {
    samples = (int *) calloc (nsamples, sizeof(int));
    if (samples == 0) {
      fprintf( stderr , "%s: No room for %d sample pc's\n", 
	      whoami , nsamples);
      done();
    }
  }
  for (i = 0; i < nsamples; i++) {
    UNIT	raw;
    int value;
      
    fread(raw, sizeof (raw), 1, pfile);
    value = bfd_get_16 (abfd, (bfd_byte *) raw);
    if (feof(pfile))
      break;
    samples[i] += value;
  }
  if (i != nsamples) {
    fprintf(stderr,
	    "%s: unexpected EOF after reading %d/%d samples\n",
	    whoami , --i , nsamples );
    done();
  }
}

/*
 *	Assign samples to the procedures to which they belong.
 *
 *	There are three cases as to where pcl and pch can be
 *	with respect to the routine entry addresses svalue0 and svalue1
 *	as shown in the following diagram.  overlap computes the
 *	distance between the arrows, the fraction of the sample
 *	that is to be credited to the routine which starts at svalue0.
 *
 *	    svalue0                                         svalue1
 *	       |                                               |
 *	       v                                               v
 *
 *	       +-----------------------------------------------+
 *	       |					       |
 *	  |  ->|    |<-		->|         |<-		->|    |<-  |
 *	  |         |		  |         |		  |         |
 *	  +---------+		  +---------+		  +---------+
 *
 *	  ^         ^		  ^         ^		  ^         ^
 *	  |         |		  |         |		  |         |
 *	 pcl       pch		 pcl       pch		 pcl       pch
 *
 *	For the vax we assert that samples will never fall in the first
 *	two bytes of any routine, since that is the entry mask,
 *	thus we give call alignentries() to adjust the entry points if
 *	the entry mask falls in one bucket but the code for the routine
 *	doesn't start until the next bucket.  In conjunction with the
 *	alignment of routine addresses, this should allow us to have
 *	only one sample for every four bytes of text space and never
 *	have any overlap (the two end cases, above).
 */
asgnsamples()
{
    register int	j;
    int		ccnt;
    double		time;
    unsigned long	pcl, pch;
    register int	i;
    unsigned long	overlap;
    unsigned long	svalue0, svalue1;

    /* read samples and assign to namelist symbols */
    scale = highpc - lowpc;
    scale /= nsamples - 1;
    alignentries();
    for (i = 0, j = 1; i < nsamples; i++) {
	ccnt = samples[i];
	if (ccnt == 0)
		continue;
	pcl = lowpc + scale * i;
	pch = lowpc + scale * (i + 1);
	time = ccnt;
#	ifdef DEBUG
	    if ( debug & SAMPLEDEBUG ) {
		printf( "[asgnsamples] pcl 0x%x pch 0x%x ccnt %d\n" ,
			pcl , pch , ccnt );
	    }
#	endif DEBUG
	totime += time;
	for (j = j - 1; j < nname; j++) {
	    svalue0 = nl[j].svalue;
	    svalue1 = nl[j+1].svalue;
		/*
		 *	if high end of tick is below entry address, 
		 *	go for next tick.
		 */
	    if (pch < svalue0)
		    break;
		/*
		 *	if low end of tick into next routine,
		 *	go for next routine.
		 */
	    if (pcl >= svalue1)
		    continue;
	    overlap = min(pch, svalue1) - max(pcl, svalue0);
	    if (overlap > 0) {
#		ifdef DEBUG
		    if (debug & SAMPLEDEBUG) {
			printf("[asgnsamples] (0x%x->0x%x-0x%x) %s gets %f ticks %d overlap\n",
				nl[j].value/sizeof(UNIT), svalue0, svalue1,
				nl[j].name, 
				overlap * time / scale, overlap);
		    }
#		endif DEBUG
		nl[j].time += overlap * time / scale;
	    }
	}
    }
#   ifdef DEBUG
	if (debug & SAMPLEDEBUG) {
	    printf("[asgnsamples] totime %f\n", totime);
	}
#   endif DEBUG
}


unsigned long
min(a, b)
    unsigned long a,b;
{
    if (a<b)
	return(a);
    return(b);
}

unsigned long
max(a, b)
    unsigned long a,b;
{
    if (a>b)
	return(a);
    return(b);
}

    /*
     *	calculate scaled entry point addresses (to save time in asgnsamples),
     *	and possibly push the scaled entry points over the entry mask,
     *	if it turns out that the entry point is in one bucket and the code
     *	for a routine is in the next bucket.
     */
alignentries()
{
    register struct nl	*nlp;
    unsigned long	bucket_of_entry;
    unsigned long	bucket_of_code;

    for (nlp = nl; nlp < npe; nlp++) {
	nlp -> svalue = nlp -> value / sizeof(UNIT);
	bucket_of_entry = (nlp->svalue - lowpc) / scale;
	bucket_of_code = (nlp->svalue + UNITS_TO_CODE - lowpc) / scale;
	if (bucket_of_entry < bucket_of_code) {
#	    ifdef DEBUG
		if (debug & SAMPLEDEBUG) {
		    printf("[alignentries] pushing svalue 0x%x to 0x%x\n",
			    nlp->svalue, nlp->svalue + UNITS_TO_CODE);
		}
#	    endif DEBUG
	    nlp->svalue += UNITS_TO_CODE;
	}
    }
}

bool
funcsymbol( symp )
     asymbol *symp;
{
  extern char	*strtab;	/* string table from a.out */
  extern int	aflag;		/* if static functions aren't desired */
  CONST char	*name;
  int i;
  char		symprefix;

  /*
   *	must be a text symbol,
   *	and static text symbols don't qualify if aflag set.
   */
  

  if (!symp->section)
    return FALSE;

  if (aflag && (symp->flags&BSF_LOCAL)) {
#if defined(DEBUG)
    fprintf (stderr, "%s(%d):  %s:  not a function\n", __FILE__, __LINE__, symp->name);
#endif
    return FALSE;
  }

  symprefix = bfd_get_symbol_leading_char (abfd);
  i = bfd_decode_symclass (symp);
#if defined(DEBUG) && 0
  if (i != 'T' && i != 't')
    fprintf (stderr, "%s(%d):  %s is of class %c\n", __FILE__, __LINE__, symp->name, i);
#endif

  /*
   * Any external text symbol should be okay.  (Only problem would be
   * variables in the text section.)
   */

  if (i == 'T')
    return TRUE;

  /*
   * 't' is static text; -a says to ignore it.  So if it's not
   * a static text symbol, *or* it is and the user gave -a, we
   * ignore it.
   */

  if (i != 't' || aflag)
    return FALSE;

  /*
   *	can't have any `funny' characters in name,
   *	where `funny' includes	`.', .o file names
   *			and	`$', pascal labels.
   */
  if (!symp->name)
    return FALSE;

  for (name = symp->name; *name; name++) {
    if ( *name == '.' || *name == '$' ) {
      return FALSE;
    }
  }

  /* On systems where the C compiler adds an underscore to all names,
   * static names without underscores seem usually to be labels in
   * hand written assembler in the library.  We don't want these
   * names.  This is certainly necessary on a Sparc running SunOS 4.1
   * (try profiling a program that does a lot of division). I don't
   * know whether it has harmful side effects on other systems.
   * Perhaps it should be made configurable.
   */

  if (symprefix && symprefix != *symp->name)
    return FALSE;

  return TRUE;
}

done()
{

    exit(0);
}
