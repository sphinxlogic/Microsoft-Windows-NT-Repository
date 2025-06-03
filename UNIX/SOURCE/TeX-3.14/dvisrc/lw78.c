/* -*-C-*- lw78.c */
/*-->lw78*/
/**********************************************************************/
/******************************** lw78 ********************************/
/**********************************************************************/
/* <BEEBE.TEX.DVI>LW78.C.27, 29-Oct-87 16:01:55, Edit by BEEBE */
/* Added DOBE JSYS (dismiss process until output buffer empty) in */
/* Put_Packet(); this seems to have completely eliminated the I/O error */
/* conditions previously experienced; they were probably due to ALW+ */
/* buffer overruns.  A straight "COPY foo.ps ttynn:" got only 3 pages */
/* out before the ALW+ said "I/O error", which is what led me to */
/* suspect that overrun might be responsible for our ALW+ miserable */
/* performance. */
/* <BEEBE.TEX.DVI.NEW>LW78.C.9, 24-Sep-86 22:49:07, Edit by BEEBE */
/* Add -n (notify user) option */
/* <BEEBE.TEX.DVI.NEW>LW78.C.5, 19-Aug-86 14:13:53, Edit by BEEBE */
/* Change check for PostScript header to "%!" and add PShdrfile (-h) */
/* option support. */
/* <BEEBE.TEX.DVI>LW78.C.15,  2-Dec-85 18:41:26, Edit by BEEBE */
/* Add check for PostScript header in Copy_File */
/* <BEEBE.TEX.DVI>LW78.C.11, 21-Nov-85 12:05:20, Edit by BEEBE */
/* Add Time_Stamp() and calls thereto for each output line */
/***********************************************************************
Apple LaserWriter copy utility.

Copy one or more 7-bit files in binary mode to an 8-bit file, halting on
input from the output  device after printing the  contents of the  input
buffer.

Usage:
        lw78 [-aAcctFile] [-d] [-en] [-hPShdrfile] [-n] [-r] \
		-oOutfile infile1 [infile2] ... [infilek]

Letter case on the  switch is ignored.

The optional  -a switch  specifies a  page accounting  file to  which  a
single entry will be  made recording a time  stampe, username, and  page
count for each file processed.

The -d switch causes a delete and expunge of each file after  processing
it.

The -en switch optionally sends ASCII character n after each file.

The -h switch defines an optional header file which will be prefixed
to non-PostScript files to allow them to be printed.

The -n switch requests that each file owner be sent a message when the
file has been printed.

The -r switch causes the files to be processed in reverse order  (useful
because of the reversed stacking of the LaserWriter).

[24-Sep-86]
***********************************************************************/

#include <stdio.h>

#ifndef KCC_20
#define KCC_20 0
#endif

#ifndef PCC_20
#define PCC_20 0
#endif

#if    (KCC_20 | PCC_20)
#else
#undef PCC_20
#define PCC_20 1
#endif

#if    KCC_20
#include <jsys.h>

/* KCC-20 and PCC-20  have similar enough JSYS  interfaces that we  just
define values for KCC-20 using PCC-20 names,  and copy in definitions of
a few macros from PCC-20's tops20.h */
#define JSbin	BIN
#define JSbkjfn	BKJFN
#define JSbout	BOUT
#define JSbout	BOUT
#define JSclosf	CLOSF
#define JSdisms	DISMS
#define JSdobe	DOBE
#define JSdvchr	DVCHR
#define JSgfust	GFUST
#define JSgfust	GFUST
#define JSjfns	JFNS
#define JSmtopr	MTOPR
#define JSmtopr	MTOPR
#define JSmtopr	MTOPR
#define JSopenf	OPENF
#define JSsfcoc	SFCOC
#define JSsfmod	SFMOD
#define JSsibe	SIBE
#define JSsout	SOUT
#define JSstpar	STPAR

#define DVtty 012		/* terminal */
#define DV_typ 0777:35-17	/* device type field */

#define GFlwr 01		/* get file last writer */

#define GJ_fou 01:35-0		/* file is for output use */
#define GJ_sht 01:35-17		/* short call format */

#define MOslw 031		/* set width */
#define MOsll 033		/* set length */
#define MOsnt 034		/* set tty non-terminal status */
#define MOsmn 01		/* no system messages(i.e. suppress) */

#define OF_bsz 077:35-5		/* byte size */
#define OF_rd 01:35-19		/* read */
#define OF_wr 01:35-20		/* write */

#define TT_mff 01:35-1		/* mechanical formfeed present */
#define TT_tab 01:35-2		/* mechanical tab present */
#define TT_lca 01:35-3		/* lower case capabilities present */
#define TT_eco 01:35-24		/* echos on */
#define TT_dam 03:35-29		/* data mode */
#define TTbin 00		/* binary */
#define TT_pgm 01:35-34		/* page mode */

#define Absmask(name) ( (1?name) << (0?name) )    /* maybe use this one */
#define Getmask(name) 	 ( 1?name )
#define Getshift(name)	 ( 0?name )

#define getfield(var, name)	( (var) >> Getshift(name) & Getmask(name) )
#define getright(b)	( (b) & 0777777 )
#define makefield(name, value)	( ((value) & Getmask(name)) << Getshift(name) )
#define makeword(l, r)	( ((l) << 18) | (r) )
#define setfield(var, name, value) ( (var) = ((var) & ~Absmask(name)) |\
	makefield(name, value) )
#else /* must be PCC_20 */
#include <file.h>
#include <monsym.h>
#endif /* KCC_20 */

typedef int BOOLEAN;

#define ACCOUNTING 1			/* define for accounting code */

#define TRUE 1
#define FALSE 0

/* global constants */

#define INTERVAL (5*512)                /* how often to check input buffer */
#define MAXERRMSG 512                   /* error message size */
#define MAXFILENAME 144			/* longest filename */
#define MAXIDLE (10*1000)               /* msec to wait for idle status */
#define MAXLINE 128			/* maximum id line size */
#define MAXPACKET (5*512)               /* packet size to output */
#define MAXPAUSE (500)                  /* msec to wait for new byte */
#define MAXUSERNAME 256			/* longest username */
#define MAXWAIT (5*1000)                /* msec to wait after each file */

#define PSHEADER "%!"			/* what every PostScript file has */

/* global variables */

char author[MAXUSERNAME+1];
BOOLEAN del_flag;                       /* file delete flag */
int eof_char;                           /* end-of-file character */
char errmsg[MAXERRMSG+1];               /* string from output device */
int nbytes;                             /* global input byte count */
BOOLEAN notify_owner;			/* notify file owner when printed */
int npacket;				/* packet[] index */
int packet[MAXPACKET+1];		/* want space for 1 extra eof_char */
char timestr[27];
long timeval;

#if    ACCOUNTING
FILE *acctfile;				/* accounting file */
#endif

FILE *PShdrfile = (FILE *)NULL;		/* PostScript header file */

/* Function macro definitions */

#define BYTEPTR(x) (0004400000000 | (int)((x)-1)) /* x must be int*, not int */
                                        /* PCC uses 36-bit bytes */
#define CTL(c) ((char)(((int)(c)) & 037))
#define GETBYTE(thejfn) (ac1 = thejfn, (void)jsys(JSbin, acs), ac2)
#define HAVE_INPUT(thejfn) (ac1 = thejfn, (void)jsys(JSsibe, acs), (ac2 > 0))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

/* Procedure macro definitions */

#define WAITMS(msec) {ac1 = msec; (void)jsys(JSdisms, acs);}

/* Function/Procedure declarations */

void Copy_File();
char *ctime();
char *fgetname();
int Get_Byte();
void Get_Msg();
void Init_Terminal();
BOOLEAN Input_Error();
void Put_Packet();
long time();
void Time_Stamp();
void Wait_For_Printer_Ready();

/***********************************************************************
It would be nice  to have done this  entirely with getc/putc, but  alas,
PCC will not open an  8-bit file to a  terminal, because it attempts  to
issue an  illegal SFBSZ%  JSYS to  set the  file byte  size, instead  of
checking to see if it is a terminal, and then just setting the byte size
in the OPENF% JSYS.  We therefore use the JSYS interface routines to  do
our own GTJFN%, OPENF%, and BOUT% calls.

Sigh....

***********************************************************************/

main(argc,argv)
int argc;
char* argv[];
{
        BOOLEAN reverse_order;
        register int c,k,len;
        int fp,outjfn,nfile,kfile;
        register FILE* infile;          /* 7-bit file */

        if (argc < 2)
        {
            (void)fprintf(stderr,
              "%%Usage: %s [-aAcctFile] [-d] [-en] [-hPShdrfile] [-n] [-r] \
-ooutfile infile1 [infile2] ... [infilek]\n",
                argv[0]);
            exit(1);
        }

        del_flag = FALSE;
        eof_char = -1;
        nfile = 0;
	notify_owner = FALSE;
        outjfn = -1;
        reverse_order = FALSE;
	PShdrfile = (FILE *)NULL;

#if    ACCOUNTING
	acctfile = (FILE *)NULL;
#endif

        for (k = 1; k < argc; ++k)
        {
            if (*argv[k] != '-')        /* not switch, must be file name */
                nfile++;
#if    ACCOUNTING
            else if ((strncmp("-a",argv[k],2) == 0) ||
                (strncmp("-A",argv[k],2) == 0))
            {
		if ((acctfile = fopen(argv[k]+2,"a")) == (FILE *)NULL)
		{
		    (void)fprintf(stderr,"?Cannot open accounting file [%s]\n",
		        argv[k]+2);
		    exit(1);
		}
	    }
#endif
            else if ((strncmp("-d",argv[k],2) == 0) ||
                (strncmp("-D",argv[k],2) == 0))
                del_flag = TRUE;
            else if ((strncmp("-e",argv[k],2) == 0) ||
                (strncmp("-E",argv[k],2) == 0))
                eof_char = atoi(argv[k]+2);
            else if ((strncmp("-h",argv[k],2) == 0) ||
                (strncmp("-H",argv[k],2) == 0))
            {
		if ((PShdrfile = fopen(argv[k]+2,"r")) == (FILE *)NULL)
		{
		    (void)fprintf(stderr,"?Cannot open PShdrfile [%s]\n",
		        argv[k]+2);
		    exit(1);
		}
	    }
            else if ((strncmp("-n",argv[k],2) == 0) ||
                (strncmp("-N",argv[k],2) == 0))
		notify_owner = TRUE;
            else if ((strncmp("-r",argv[k],2) == 0) ||
                (strncmp("-R",argv[k],2) == 0))
                reverse_order = TRUE;
            else if ((strncmp("-o",argv[k],2) == 0) ||
                (strncmp("-O",argv[k],2) == 0))
            {
                if ((outjfn = _gtjfn(argv[k]+2, makefield(GJ_fou,1) |
                    makefield(GJ_sht,1))) != -1)      /* then got valid jfn */
                {
                    ac1 = outjfn = getright(outjfn);
                    ac2 = makefield(OF_bsz, 8);
                    ac2 = setfield(ac2,OF_wr,1);
                    ac2 = setfield(ac2,OF_rd,1);
                    if (jsys(JSopenf, acs) != JSok)
                        outjfn = -1;    /* open failed */
                }
            }
            else
            {
                (void)fprintf(stderr,"?Unknown switch [%s]\n",argv[k]);
                exit(1);
            }
        }
        if (outjfn == -1)
        {
            (void)fprintf(stderr,"?Cannot open output file\n");
            exit(1);
        }
        else                /* open okay */
            Init_Terminal(outjfn);

        kfile = 0;
        for (k = (reverse_order ? (argc-1) : 1);
            (0 < k) && (k < argc);
            (reverse_order ? --k : ++k))
        {
            if (*argv[k] == '-')
                ;                       /* ignore switch field */

#if    KCC_20
            else if ((infile = fopen(argv[k],"rb7")) != (FILE*)NULL)
#endif
#if    PCC_20
            else if ((fp = open(argv[k],
                FATT_RDONLY | FATT_SETSIZE | FATT_BINARY,7)) >= 0)
#endif
            {
#if    PCC_20
                infile = fdopen(fp,"rb");
#endif
                Wait_For_Printer_Ready(outjfn);

		Time_Stamp(stderr);
                (void)fprintf(stderr,"[%d of %d: %s]",++kfile,nfile,argv[k]);
                Copy_File(infile,outjfn);
		if (notify_owner)
		{
		    ac1 = makeword(GFlwr,jfnof(fileno(infile)));
		    ac2 = POINT(author);
		    (void)jsys(JSgfust, acs);
		    timeval = time((long*)NULL);
		    strcpy(timestr,ctime(&timeval));
		    len = strlen(timestr) - 1;	/* ctime has its own \n */
		    timestr[len] = '\0';        /* so kill it */
		    (void)sprintf(errmsg,
		        "send %s %s: %s printed on LaserWriter\n",
		        author,timestr,argv[k]);
		    (void)system(errmsg);
		}
                (void)fclose(infile);

                if (del_flag)
                {
                    (void)unlink(argv[k]);      /* delete the file */
                    (void)fprintf(stderr," [OK - deleted]");
                }
                else
                    (void)fprintf(stderr," [OK]");

                if (kfile < nfile)      /* delay next output to try to */
                    WAITMS(MAXWAIT);      /* avoid timeout bug! */
            }
            else
	    {
	        Time_Stamp(stderr);
                (void)fprintf(stderr,"?Cannot open input file [%s]",argv[k]);
	    }
        }

        ac1 = outjfn;
        (void)jsys(JSclosf, acs);
	Time_Stamp(stderr);
	(void)fprintf(stderr,"Spooler terminated\n\n\n\n");
}

void
Copy_File(infile,outjfn)
register FILE* infile;
register int outjfn;
{
    register int c;
    register int k;
    char filename[MAXFILENAME+1];
    char idline[MAXLINE+1];

#if    ACCOUNTING
    int page_count;
    int *pbyte;
#endif

    (void)
    fgets(idline,MAXLINE,infile);	/* peek at first line */
    rewind(infile);

    if (strncmp(idline,PSHEADER,strlen(PSHEADER)) != 0)
    {
	if (PShdrfile != (FILE *)NULL)
	{
		rewind(PShdrfile);
		nbytes = 0;
		while ((npacket = read(fileno(PShdrfile),packet,MAXPACKET)) > 0)
		{
		    nbytes += npacket;
		    Put_Packet(outjfn,packet,npacket);
		    if (Input_Error(PShdrfile,outjfn))
		        return;
		}
	}
	else /* reject non-PostScript file */
	{
            k = MAX(2,strlen(idline));
	    if (idline[k-1] == '\n')
	        idline[k-1] = '\0';		/* kill NL */
	    if (idline[k-2] == '\r')
		idline[k-2] = '\0';		/* kill CR */
	    Time_Stamp(stderr);
	    (void)fprintf(stderr,
	        "?File may not contain PostScript.  First line has\n");
	    (void)fprintf(stderr,"\t[%s] instead of [%s] -- file discarded",
		idline,PSHEADER);
	    return;
	}
    }

#if    ACCOUNTING
    page_count = 0;
#endif

    nbytes = 0;
#if    (INTERVAL-MAXPACKET)		/* copy one at a time */
    npacket = 0;
    while ((c = getc(infile)) != EOF)
    {
#if    ACCOUNTING
        if (c == '\f')
	    page_count++;		/* each FF counts one page */
#endif
        if ((!((++nbytes) % INTERVAL)) && Input_Error(infile,outjfn))
            return;
        if (npacket >= MAXPACKET)
        {
            Put_Packet(outjfn,packet,npacket);
            npacket = 0;
        }
        packet[npacket++] = c;
    }
#else           /* copy in block mode */
    while ((npacket = read(fileno(infile),packet,MAXPACKET)) > 0)
    {
        nbytes += npacket;
        Put_Packet(outjfn,packet,npacket);

#if    ACCOUNTING
	if (acctfile != (FILE*)NULL)
	{
	    for ((pbyte = packet, k = npacket); k; (--k, ++pbyte))
	        if (*pbyte == '\f')
		    page_count++;	/* each FF counts one page */
	}
#endif

        if (Input_Error(infile,outjfn))
            return;
    }
    if (npacket < 0)
    {
        Time_Stamp(stderr);
	(void)fprintf(stderr,"?I/O error on input file");
    }
    npacket = 0;
#endif

    if (eof_char >= 0)
        packet[npacket++] = eof_char;
    if (npacket > 0)

        Put_Packet(outjfn,packet,npacket);

#if    ACCOUNTING
    if (acctfile != (FILE*)NULL)
    {
	ac1 = makeword(GFlwr,jfnof(fileno(infile)));
	ac2 = POINT(author);
	(void)jsys(JSgfust, acs);
	Time_Stamp(acctfile);
	(void)fprintf(acctfile,"%8d\t%s\t%s",
	    MAX(1,page_count),author,fgetname(infile,filename));
    }
#endif

}

int
Get_Byte(thejfn)                        /* get byte (maybe waiting) */
register int thejfn;                    /* returns EOF if no input */
{
    if (HAVE_INPUT(thejfn))
        return(GETBYTE(thejfn));

    WAITMS(MAXPAUSE);

    if (HAVE_INPUT(thejfn))
        return(GETBYTE(thejfn));
    else                                /* still no input */
        return(EOF);
}

void
Get_Msg(thejfn)
int thejfn;
{
    register int cm1,cm2,c,k;

    Time_Stamp(stderr);
    k = 0;
    c = 0;
    cm1 = 0;
    cm2 = 0;
    while (!((cm2 == ']') && (cm1 == '%') && (c == '%'))) /* until "]%%" */
    {   /* skip <garbage> and collect %%[ ... ]%% */
        cm2 = cm1;
        cm1 = c;
        if ((c = Get_Byte(thejfn)) == EOF)
            break;                      /* no input yet */
        if ((cm2 == '%') && (cm1 == '%') && (c == '['))
        {                               /* restart message collection */
            k = 2;
            errmsg[0] = '%';
            errmsg[1] = '%';
        }
        if (k < MAXERRMSG)
            errmsg[k++] = c;
        putc(c,stderr);
    }


    errmsg[k] = '\0';                   /* terminate string */

    while (TRUE)                        /* "infinite" loop to discard any */
                                        /* remaining input up to start of */
                                        /* next message */
    {

        if ((c = Get_Byte(thejfn)) == EOF)
            break;                      /* no input, so exit loop */

        if (c == '%')                   /* if start of new message then */
        {                               /* put character back and exit */
            ac1 = thejfn;
            (void)jsys(JSbkjfn,acs);
            break;
        }

	if (!((c == '\r') || (c == '\n')))/* echo the character */
	    putc(c,stderr);		/* but discard terminal CR LF */
    }					/* to avoid unwanted blank lines */
}

void
Init_Terminal(thejfn)
register int thejfn;
{
    ac1 = thejfn;
    (void)jsys(JSdvchr, acs);           /* get device characteristics */

    if (getfield(ac2,DV_typ) == DVtty)
    {                                   /* have terminal, set its features */
        ac1 = thejfn;
        ac2 = MOsnt;
        ac3 = MOsmn;                    /* no system messages */
        (void)jsys(JSmtopr, acs);

        ac1 = thejfn;
        ac2 = makefield(TT_mff,1);      /* has formfeed */
        ac2 = setfield(ac2,TT_tab,1);   /* has tab */
        ac2 = setfield(ac2,TT_lca,1);   /* has lowercase */
        ac2 = setfield(ac2,TT_pgm,1);   /* has X-on/X-off */
        (void)jsys(JSstpar, acs);

        ac1 = thejfn;
        ac2 = makefield(TT_dam,TTbin);  /* binary output */
        ac2 = setfield(ac2,TT_eco,0);   /* no echo */
        (void)jsys(JSsfmod, acs);

        ac1 = thejfn;
        ac2 = 0525252525252;
        ac3 = 0525252525252;            /* no CTL-char translation */
        (void)jsys(JSsfcoc, acs);

        ac1 = thejfn;
        ac2 = MOslw;
        ac3 = 0;                        /* line width = 0 */
        (void)jsys(JSmtopr, acs);

        ac1 = thejfn;
        ac2 = MOsll;
        ac3 = 0;                        /* page length = 0 */
        (void)jsys(JSmtopr, acs);
    }
}

BOOLEAN
Input_Error(infile,thejfn)		/* return TRUE if error */
register FILE* infile;
register int thejfn;
{
    register int c;

    if (HAVE_INPUT(thejfn))
    {
        Get_Msg(thejfn);
        if (strncmp(errmsg,"%%[ Error: timeout;",19) == 0)
        {
            Time_Stamp(stderr);
	    (void)fprintf(stderr,"[Error occurred in input byte range %d..%d]",
                MAX(0,nbytes-32),nbytes);
            Time_Stamp(stderr);
	    (void)fprintf(stderr,
            "Attempting to recover from Apple LaserWriter BUG of \
timeout error");
            (void)fflush(stderr);
            return(FALSE);
        }
	else if (strncmp(errmsg,"%%[ PrinterError: out of paper",30) == 0)
	{
	    Time_Stamp(stderr);
	    (void)fprintf(stderr,"\t[Waiting 10 sec for paper]");
	    (void)fflush(stderr);
	    WAITMS(10000);
	    return(FALSE);
	}
	else if (strncmp(errmsg,"%%[ PrinterError: no paper tray",31) == 0)
	{
	    Time_Stamp(stderr);
	    (void)fprintf(stderr,"\t[Waiting 10 sec for paper tray]");
	    (void)fflush(stderr);
	    WAITMS(10000);
	    return(FALSE);
	}
        else if (strncmp(errmsg,"%%[ Error: VMerror;",19) == 0)
        {
            Time_Stamp(stderr);
	    (void)fprintf(stderr,
            "Attempting to recover from Apple LaserWriter BUG of VMerror");
            (void)fflush(stderr);
	    Wait_For_Printer_Ready(thejfn);
	    do				/* skip to next page or end-of-job */
	    {
		c = getc(infile);
	    }
	    while ((c != EOF) && (c != CTL('D')) && (c != CTL('L')));
	    npacket = 0;		/* discard last packet */
            return(FALSE);
        }
        else if (strcmp(errmsg,"%%[ status: idle ]%%") == 0)
            return(FALSE);
        else
        {
            Time_Stamp(stderr);
	    (void)fprintf(stderr,"[Error occurred in input byte range %d..%d]",
                MAX(0,nbytes-32),nbytes);
	    Time_Stamp(stderr);
            (void)fprintf(stderr,"[Job flushed]");
            (void)fflush(stderr);
            return(TRUE);
        }
    }
    else                                /* no input yet */
        return(FALSE);
}

void
Put_Packet(thejfn,packet,n)
int thejfn;
int packet[];
int n;
{
    ac1 = thejfn;
    (void)jsys(JSdobe, acs);		/* wait until output buffer empty */

    ac1 = thejfn;
    ac2 = BYTEPTR(packet);
    ac3 = -n;                           /* exact byte count */
    ac4 = 0;
    if (n > 0)
    {
        if (jsys(JSsout, acs) != JSok)
        {
	    Time_Stamp(stderr);
            (void)fprintf(stderr,"?SOUT error");
            exit(1);
        }
    }

}

void
Time_Stamp(fp)
FILE *fp;
{
    int k;

    timeval = time((long*)NULL);
    strcpy(timestr,ctime(&timeval));
    k = strlen(timestr) - 1;	/* ctime has its own \n */
    timestr[k] = '\0';          /* so kill it */
    (void)fprintf(fp,"\n%s\t",timestr);
}

void
Wait_For_Printer_Ready(thejfn)
register int thejfn;
{
    while (TRUE)                        /* "infinite" loop */
    {
        ac1 = thejfn;
        ac2 = CTL('T');                 /* CTL-T for status message */
        (void)jsys(JSbout, acs);

        ac1 = thejfn;
        (void)jsys(JSdobe, acs);        /* wait until output sent */

        Get_Msg(thejfn);
        if (strcmp(errmsg,"%%[ status: idle ]%%") == 0)
            break;                      /* printer ready to go */

        ac1 = thejfn;
        ac2 = CTL('D');                 /* CTL-D to terminate previous job */
        (void)jsys(JSbout, acs);

	Time_Stamp(stderr);
        (void)fprintf(stderr,"\t[Waiting %d sec]",MAXIDLE/1000);

        WAITMS(MAXIDLE);
    }
}

char *
fgetname(fp,buffer)
FILE *fp;
char buffer[];
{
    ac1 = POINT(buffer);
    ac2 = jfnof(fileno(fp));
    ac3 = 0;
    ac4 = 0;
    buffer[0] = '\0';
    (void)jsys(JSjfns, acs);
    return(buffer);
}

