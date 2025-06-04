/*
 * From ut-ngp!ut-sally!caip!princeton!allegra!ulysses!mhuxr!mhuxt!
 * houxm!hropus!riccb!ihopa!ihnp4!inuxc!pur-ee!pucc-j!doc
 * Tue Jul 15 09:04:38 CDT 1986
 *
 * Reply-To: ihnp4!decwrl!cookie.dec.com!wecker@ee.purdue.edu
 * (DAVE TANSTAAFL WECKER)
 *
 * Enclosed is a first version of "shar.c" (v 860712 DBW) that will run on
 * ULTRIX (U**X 4.n), VMS and the AMIGA. I finally got fed up with ARCV
 * MKARCV and decided to do things right.
 *
 * The main item of interest is that it handles wild-cards on the AMIGA and will
 * also UNSHAR a file (for the AMIGA and VMS which don't necessarly have
 * the Bourne Shell). It will NOT create a SHAR file with sub directories. The
 * Amiga version was done with MANX-Aztec-C, it probably needs some changes to
 * run under Lattice.
 *
 *				regards,
 *				dave (ucbvax!decwrl!cookie.dec.com!wecker)
 *
 * Moderator's note:  This program will allow interchanging shar formats from
 * Unix and the Amiga.  I have tried several Unix shar formats and they have
 * all successfully unshared using this program on the amiga, and this will
 * also create shar files that will unshar under Bourne Shell without any
 * problems.
 *
 *
 * Creating a shar file:
 *
 *	AMIGA:	shar >foo.shar -a makefile *.h *.c
 *
 *	VMS:	$ define/user_mode sys$output foo.shar
 *		$ shar -a makefile *.h *.c
 *		or
 *		$ shar -o foo.shar -a makefile *.h *.c
 *
 *		On VMS, it's a good idea to always use the -b option,
 *		to avoid file name problems.
 *
 *	U**X:	shar -a makefile *.h *.c >foo.shar
 *
 *		(P.S. the -a switch means do all options)
 *
 * Extracting from a shar file:
 *
 *	AMIGA:	shar -u foo.shar
 *
 *	VMS:	shar -u foo.shar
 *
 *	U**X:	shar -u foo.shar	or    sh foo.shar
 *
 *
 * Installation:
 *
 *	AMIGA:	cc -DAMIGA shar.c
 *		ln -o shar shar.o -lc
 *
 *	VMS:	cc/DEFINE=VMS shar.c
 *		link shar,sys$library:vaxcrtl/lib
 *		shar :== $dev:[dir]shar
 *
 *	U**X:	cc -O -DULTRIX -o shar shar.c -lc
 *
 *
 * Just typing "shar" will yield a usage line. (Doc: thanks for the code to
 * base this off of).
 *
 *
 * Shar puts readable text files together in a package
 *  from which they are easy to extract.
 *
 *  v 860712 D. Wecker for ULTRIX and the AMIGA
 *	- stripped down.. does patterns but no directories
 *	- added a -u (unshar) switch
 */

#include <stdio.h>

#ifdef	AMIGA
#include <exec/types.h>
extern char *getenv(),*scdir(),*malloc(),*index();
#endif

#ifdef	ULTRIX
#include <sys/types.h>
extern char *getenv(),*scdir(),*malloc(),*index();
#endif

#ifdef	VMS
#include <types.h>
#include <descrip.h>
#include <rmsdef.h>
#include <ctype.h>
extern char *getenv(),*scdir(),*malloc();
#define	index	strchr
#endif


#define BADCH	((int)'?')
#define EMSG	""
#define tell(s)	{fputs(*nargv,stderr);fputs((s),stderr); \
		fputc(optopt,stderr);fputc('\n',stderr);return(BADCH);}
#define rescanopts()	(optind = 1)

int	optind = 1,		/* index into parent argv vector */
	optopt;			/* character checked for validity */
long	fsize;			/* length of file */
char	*optarg;		/* argument associated with option */
char	*sav[100];		/* saved file names */
int	savind;			/* save index */

/* OPTIONS */
int 	Verbose = 0;           /* provide append/extract feedback */
int 	Basename = 0;          /* extract into basenames */
int 	Count = 0;             /* count characters to check transfer */
char	*Delim = "SHAR_EOF";   /* put after each file */
char	Filter[100] = "cat";   /* used to extract archived files */
char	*Prefix = NULL;        /* line prefix to avoid funny chars */
int	UnShar = 0;	       /* do we unshar an input file? */

#define	SED "sed 's/^%s//'"    /* used to remove prefix from lines */
#define	USAGE "[-u archive] [[-a] [-p prefix] [-o file] [-d delim] [-bcv] files > archive]"
#define	OPTSTRING "u:ap:o:d:bcv"

int header(ppchFiles)
char *ppchFiles[];
{
    extern char *ctime();
    register int i;
    auto long clock;
    register int  problems = 0;
    register char **ppchList;
    register char *pchOrg = getenv("ORGANIZATION");
    register char *pchName = getenv("NAME");

    puts("#\tThis is a shell archive.");
    puts("#\tRemove everything above and including the cut line.");
    puts("#\tThen run the rest of the file through sh.");
    puts("#----cut here-----cut here-----cut here-----cut here----#");
    puts("#!/bin/sh");
    puts("# shar:    Shell Archiver");
    puts("#\tRun the following text with /bin/sh to create:");
    for (ppchList = ppchFiles; *ppchList; ++ppchList)
        printf("#\t%s\n", *ppchList);
    (void) time(& clock);
    printf("# This archive created: %s", ctime(&clock));
    if (pchName)
        printf("# By:\t%s (%s)\n", pchName, 
        pchOrg ? pchOrg : "Dave Wecker Midnight Hacks");
    return(0);
}

int archive(input, output)
char *input, *output;
{
    auto char line[BUFSIZ];
    register FILE *ioptr;

    if (ioptr = fopen(input, "r")) {
        printf("%s << \\%s > %s\n", Filter, Delim, output);
        while(fgets(line, BUFSIZ, ioptr)) {
            if (Prefix) fputs(Prefix, stdout);
            fputs(line, stdout);
            if (Count) fsize += strlen(line);
        }
        puts(Delim);
        (void) fclose(ioptr);
        return(0);
    } 
    else {
        fprintf(stderr, "shar: Can't open '%s'\n", input);
        return(1);
    }
}


void shar(file)
char *file;
{
    register char *basefile = file;
    if (!strcmp(file, "."))
        return;
    fsize = 0;
    if (Basename) {
        while(*basefile)
            basefile++;        /* go to end of name */
#ifndef	VMS
        while(basefile > file && *(basefile-1) != '/')
#else
        while(basefile > file && *(basefile-1) != ']')
#endif
            basefile--;
    }
    if (Verbose) printf("echo shar: extracting %s\n", basefile);
    if (archive(file, basefile)) exit(66);
    if (Count) {
        printf("if test %ld -ne \"`wc -c %s`\"\n",fsize,basefile);
        printf("then\necho shar: error transmitting %s ",basefile);
        printf("'(should have been %ld characters)'\nfi\n",fsize);
    }
}

int main(argc, argv)
int argc;
char **argv;    
{
    auto char *ppchFiles[256];
    register int  C;
    register char **ppchList = ppchFiles;
    register int errflg = 0;
    FILE *f, *fopen();

    while(EOF != (C = getopt(argc, argv, OPTSTRING))) {
        switch(C) {
        case 'v': 
            Verbose++; 
            break;
        case 'c': 
            Count++; 
            break;
	case 'o':
		if ((f = fopen(optarg,"w")) == NULL) {
			fprintf(stderr,"shar: can't open %s for output\n",optarg);
			exit(1);
		}
		stdout = f;	/* redirect output the easy way */
		break;		
        case 'b': 
            Basename++; 
            break;
        case 'd': 
            Delim = optarg; 
            break;
        case 'a': /* all the options */
            optarg = "XX";
            Verbose++;
            Count++;
            Basename++;
            /* fall through to set prefix */
        case 'p': 
            (void) sprintf(Filter, SED, Prefix = optarg); 
            break;
        case 'u':
            UnShar++;
            dounshar(optarg);
            break;
        default: 
            errflg++;
        }
    }

    if (UnShar) exit(0);

    C = getarg(argc, argv);
    if (errflg || EOF == C) {
        if (EOF == C)
            fprintf(stderr, "shar: No input files\n");
        fprintf(stderr, "usage: shar %s\n", USAGE);
        exit(1);
    }

    savind = 0;
    do {
        if (getpat(optarg)) exit(2);
    } 
    while (EOF != (C = getarg(argc, argv)));

    sav[savind] = 0;
    header(sav);
    for (ppchList = sav; *ppchList; ++ppchList) shar(*ppchList);
    puts("#\tEnd of shell archive");
    puts("exit 0");
    exit(0);
}

getpat(pattern)
char *pattern;
{
    register char *ptr;

#ifdef VMS
    register char *tail, *cp;
    char resbuf[201];
    unsigned long context;
    struct dsc$descriptor_s pat, result;
    int lib$find_file(), lib$find_file_end(), status;

    /* pattern descriptor	*/
    pat.dsc$w_length = strlen(pattern);
    pat.dsc$a_pointer = pattern;
    pat.dsc$b_dtype = DSC$K_DTYPE_T;
    pat.dsc$b_class = DSC$K_CLASS_S;

    /* result descriptor	*/
    result.dsc$w_length = sizeof(resbuf) - 1;
    result.dsc$a_pointer = resbuf;
    result.dsc$b_dtype = DSC$K_DTYPE_T;
    result.dsc$b_class = DSC$K_CLASS_S;

    resbuf[sizeof(resbuf)] = '\0';	/* sentinel	  	*/
    context = 0L;			/* search context	*/

    for (;;) {
	status = lib$find_file(&pat,&result,&context);/* look	*/
	if (status != RMS$_NORMAL)
		break;
	if (tail = index(resbuf,';'))	/* drop version #	*/
		*tail = '\0';
	if ((tail > resbuf) && (*(tail - 1) == '.'))	/* drop trailing . */
		*(--tail) = '\0';
	ptr = resbuf;			/* return file name	*/
#else
#ifdef AMIGA
    while (ptr = scdir(pattern)) {
#else
    ptr = pattern;	/* Ultrix shell already did expansion for us */
    {
#endif
#endif
	for (cp = ptr; *cp; cp++)		/* convert to lower case */
		if (isupper(*cp))
			*cp = tolower(*cp);
	sav[savind] = malloc(strlen(ptr)+1);	/* save file name */
	strcpy(sav[savind++],ptr);
	if (access(ptr,4)) {
	    printf("No read access for file: %s\n",ptr);
	    return(-1);
	}
    }
#ifdef	VMS
    lib$find_file_end(&context);
#endif
    return(0);
}

/*
 * get option letter from argument vector
 */
int
getopt(nargc, nargv, ostr)
int nargc;
char **nargv, *ostr;
{
    register char    *oli;        /* option letter list index */
    static char    *place = EMSG;    /* option letter processing */

    if(!*place) {            /* update scanning pointer */
        if(optind >= nargc || *(place = nargv[optind]) != '-' || !*++place) return(EOF);
        if (*place == '-') {    /* found "--" */
            ++optind;
            return EOF;
        }
    }                /* option letter okay? */
    if ((optopt = (int)*place++) == (int)':' || !(oli = index(ostr,optopt))) {
        if(!*place) ++optind;
        tell(": illegal option -- ");
    }
    if (*++oli != ':') {        /* don't need argument */
        optarg = NULL;
        if (!*place)
            ++optind;
    } 
    else {                /* need an argument */
        if (*place) {            /* no white space */
            optarg = place;
        } 
        else if (nargc <= ++optind) {    /* no arg */
            place = EMSG;
            tell(": option requires an argument -- ");
        } 
        else {
            optarg = nargv[optind];    /* white space */
        }
        place = EMSG;
        ++optind;
    }
    return optopt;            /* dump back option letter */
}

int
getarg(nargc, nargv)
int nargc;
char **nargv;
{
    if (nargc <= optind) {
        optarg = (char *) 0;
        return EOF;
    } 
    else {
        optarg = nargv[optind++];
        return 0;
    }
}

dounshar(ArcNam)
char *ArcNam;
{
    register int i,j;
    register FILE *inptr,*outptr;
    auto char line[BUFSIZ];
    int DirNum = -1;
    int Prefix = 0;
    char Dirs[5][40],FilNam[128],Delim[40],ScrStr[128];
    char *ptr;

    if (!(inptr = fopen(ArcNam,"r"))) {
        fprintf(stderr,"shar: Can't open archive '%s'\n", ArcNam);
        return;
    }
    while (fgets(line,BUFSIZ,inptr)) {
        if (strncmp(line,"sed ",4) == 0) {
            Prefix = 0;
            if (!(ptr = index(line,'/'))) goto getfil;
            if (*++ptr == '^') ++ptr;
            while (*ptr++ != '/') Prefix++;
            goto getfil;
        }
        else if (strncmp(line,"cat ",4) == 0) {
            Prefix = 0;
            ;
getfil:

#if    VMS
            strcpy(FilNam,"[");
#else
            FilNam[0] = 0;
#endif

            for (i = 0; i <= DirNum; i++) {

#if    VMS
                strcat(FilNam,".");
                strcat(FilNam,Dirs[i]);
#else
                strcat(FilNam,Dirs[i]);
                strcat(FilNam,"/");
#endif

            }


#if    VMS
            strcat(FilNam,"]");
#endif

            getshpar(line,">",ScrStr);
            strcat(FilNam,ScrStr);
            getshpar(line,"<<",Delim);
            fprintf(stderr,"Creating %s ...",FilNam);
            outptr = fopen(FilNam,"w");
            while (fgets(line,BUFSIZ,inptr)) {
                if (strncmp(line,Delim,strlen(Delim)) == 0) break;
                if (outptr) fputs(&line[Prefix],outptr);
            }
            if (outptr) {
                fclose(outptr);
                fprintf(stderr,"...done\n");
            }
            else fprintf(stderr,"...error in creating file\n");
        }
        else if (strncmp(line,"mkdir ",6) == 0) {
            sprintf(stderr,"Need to make directory: %s\n",&line[6]);
        }
        else if (strncmp(line,"chdir ",6) == 0) {
            if (line[6] == '.' && line[7] == '.') DirNum--;
            else strcpy(Dirs[++DirNum],&line[6]);
            if (DirNum < -1) DirNum = -1;
        }
        else if (strncmp(line,"cd ",3) == 0) {
            if (line[3] == '.' && line[4] == '.') DirNum--;
            else strcpy(Dirs[++DirNum],&line[3]);
            if (DirNum < -1) DirNum = -1;
        }
    }
    fclose(inptr);
}

getshpar(line,sea,par)
char *line,*sea,*par;
{
    register int i,j,k;
    register char *scr1,*scr2;

    while (*line) {
        scr1 = line;
        scr2 = sea;
        while (*scr1 && *scr2 && *scr1 == *scr2) { 
            scr1++; 
            scr2++; 
        }
        if (*scr2 == 0) {
            if (*scr1 == 0) { 
                *par = 0; 
                return; 
            }
            while ( *scr1 == ' ' || *scr1 == '\t' ||
                *scr1 == '\\' || *scr1 == '\'' || *scr1 == '"') scr1++;
            while ( *scr1 != 0 && *scr1 != ' ' && *scr1 != '\t' &&
                *scr1 != '\\' && *scr1 != '\'' && *scr1 != '"' &&
                *scr1 != '\n' && *scr1 != '\r') *par++ = *scr1++;
            *par = 0;
            return;
        }
        line++;
    }
    *par = 0;
}


