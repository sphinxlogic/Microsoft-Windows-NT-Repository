/* OSDEP_VMS.C */
#include 	<stdio.h>
#include	<errno.h>
#include	<setjmp.h>
#include	<time.h>
#include	"osdep.h"
#include        "pico.h"
#include	"estruct.h"
#include        "edef.h"
#include        "efunc.h"
#ifdef __ALPHA
#include	<fcntl.h>
#endif
#include	<file.h>
#include	<types.h>
#include	<time.h>

#include <iodef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <lib$routines.h>
#include <descrip.h>

/* Output channel and buffers */
static short	TerminalChan;
#define	MAXBUF	1024		/* Save up to 1024 characters before sending them */
unsigned char	OutputBuffer[MAXBUF + 1];
int	OutputBufferCounter;

/* For setting and resting the PASTHRU flag: */
struct	{
	unsigned char	class, type;
	unsigned short	width;
	unsigned long	BasicChar,
			ExtendChar;
	} TerminalChar;


struct KBSTREE *kpadseqs = NULL;


#define	SIGTYPE	void

struct DESC {		/* String descriptor */
	short	length, type;
	char	*address;
	} ;

    int      kbseq();
    SIGTYPE  do_hup_signal();
    SIGTYPE  rtfrmshell();
#ifdef	TIOCGWINSZ
    SIGTYPE  winch_handler();
#endif


/*
 * for alt_editor arg[] building
 */
#define	MAXARGS	10
#ifdef HEBREW
#include "hebrew.h"

unsigned char xlat_tab[]={
  0xf9,  /* a - shin*/
  0xf0,  /* b - nun */
  0xe1,  /* c - bet */
  0xe2,  /* d - gimel*/
  0xf7,  /* e - kuf*/
  0xeb,  /* f - chaf */
  0xf2,  /* g - ayin */
  0xe9,  /* h - yud */
  0xef,  /* i - nun sofit */
  0xe7,  /* j - het */
  0xec,  /* k - lamed */
  0xea,  /* l - chaf sofit */
  0xf6,  /* m - zadik */
  0xee,  /* n - mem */
  0xed,  /* o - mem sofit */
  0xf4,  /* p - pey */
  0x2f,  /* q - / */ 
  0xf8,  /* r - raish */
  0xe3,  /* s - dalet */
  0xe0,  /* t - aleph */
  0xe5,  /* u - vav */
  0xe4,  /* v - hey */
  0x27,  /* w - ' */
  0xf1,  /* x - samech */
  0xe8,  /* y - tet */
  0xe6,  /* z - zayin */
};
unsigned char xlat_str1[]="`;',./";
unsigned char xlat_str2[]={
  ';',   /* ` - ; */
  0xf3,  /* ; - pey sofit */
  ',',   /* ' - , */
  0xfa,  /* , - tav */
  0xf5,  /* . - zadik sofit */
  '.',   /* / - . */
};
#endif

/*
 * ttopen - this function is called once to set up the terminal device 
 *          streams.  if called as pine composer, don't mess with
 *          tty modes, but set signal handlers.
 */
ttopen()
{
	long	status;
	struct DESC TerminalDesc;
	char	TerminalName[] = "SYS$OUTPUT:";

	TerminalDesc.address = TerminalName; TerminalDesc.type = 0;
	TerminalDesc.length = strlen(TerminalName);

	status = sys$assign(&TerminalDesc, &TerminalChan, (int)(0), (int)(0));
	if((status & 0x1) == 0) {
		printf("Can't assign channel to terminal\n");
		exit(status);
	}

    signal(SIGHUP,  do_hup_signal);	/* deal with SIGHUP */
    signal(SIGTERM, do_hup_signal);	/* deal with SIGTERM */
#ifdef	SIGTSTP
    signal(SIGTSTP, SIG_DFL);
#endif

/* Some default values since we do not have TERMCAP: */
	eolexist = FALSE;
	revexist = TRUE;
	delchar = FALSE;
	inschar = FALSE;
      scrollexist = TRUE;

	if(gmode&MDFKEY)
		HelpKeyNames = funckeynames;
	else
		HelpKeyNames = NULL;

	init_kpnames();

	OutputBufferCounter = 0;	/* Output buffer is empty */

/* Set the PASTHRU bit */
	status = sys$qiow((int)(0), TerminalChan, (short)(IO$_SENSEMODE),
			NULL, (int)(0), (int)(0),
			&TerminalChar, sizeof(TerminalChar),
			(int)(0), (int)(0), (int)(0), (int)(0));
	if((status & 0x1) == 0) {
		printf("Can't read for set terminal to /PASTHRU\n");
		exit(status);
	}

	TerminalChar.ExtendChar |= TT2$M_PASTHRU;
	status = sys$qiow((int)(0), TerminalChan, (short)(IO$_SETMODE),
			NULL, (int)(0), (int)(0),
			&TerminalChar, sizeof(TerminalChar),
			(int)(0), (int)(0), (int)(0), (int)(0));
	if((status & 0x1) == 0) {
		printf("Can't set terminal to /PASTHRU\n");
		exit(status);
	}

    ttgetwinsz();
    return(1);
}



/*
 * ttclose - this function gets called just before we go back home to 
 *           the command interpreter.  If called as pine composer, don't
 *           worry about modes, but set signals to default, pine will 
 *           rewire things as needed.
 */
ttclose()
{
	int	status;

	ttflush();

/* Clear the PASTHRU flag */
	TerminalChar.ExtendChar &= ~TT2$M_PASTHRU;
	status = sys$qiow((int)(0), TerminalChan, (short)(IO$_SETMODE),
			NULL, (int)(0), (int)(0),
			&TerminalChar, sizeof(TerminalChar),
			(int)(0), (int)(0), (int)(0), (int)(0));
	if((status & 0x1) == 0) {
		printf("Can't set terminal to /NOPASTHRU\n");
		exit(status);
	}
	sys$dassgn(TerminalChan);

    return(1);
}


/*
 * ttspeed - return TRUE if tty line speed < 9600 else return FALSE
 */
ttisslow()
{
	return(TRUE);
}


/*
 * ttgetwinsz - set global rows and columns values and return
 */
ttgetwinsz()
{
#define TT_STRING_SIZE 20
       char string[TT_STRING_SIZE] ;   /* string for holding tt:'s dev name */
       int rows, col ;                 /* row / column vars */
       unsigned long status ;          /* status return var */
       unsigned long terminal_item = 797 ; /* JPI$_TERMINAL */
       unsigned long tt_page_item = 170 ;  /* DVI$_TT_PAGE */
       unsigned long devbufsz_item = 8 ;   /* DVI$_DEVBUFSIZ */
       struct dsc$descriptor_vs tt_sdesc =
                               {TT_STRING_SIZE,/* max string size */
                               DSC$K_DTYPE_VT, /* descriptor type */
                               DSC$K_CLASS_VS, /* descriptor class = var str */
                               string} ;       /* pointer to var string */


/* Set the defaults */
          term.t_nrow = 23;
          term.t_ncol = 80;

               /* Get our terminal's device name */
       status = lib$getjpi(&terminal_item,NULL,NULL,NULL,&tt_sdesc,NULL) ;
       if ((status&1) == 1)
          {
               /* Get our terminal's page size */
          status = lib$getdvi(&tt_page_item,NULL,&tt_sdesc,&rows,NULL,NULL) ;
               /* Get our terminal's page width */
	  if((status & 0x1) == 1) {
	          status = lib$getdvi(&devbufsz_item,NULL,&tt_sdesc,&col,NULL,NULL) ;
		if((status & 0x1) == 1) {
	               /* Set the globals for rows and columns */
		          term.t_nrow = rows-1 ;
		          term.t_ncol = col ;
			}
		}
          }
       else
          printf("Can't find process's terminal -- using defaults.\n") ;
}

/*
 | Physically write a string to the terminal.
 */
write_terminal(string, size)
char	*string;
int	size;
{
	long	status;
	short	iosb[4];

	status = sys$qiow((int)(0), TerminalChan, (short)(IO$_WRITEVBLK),
			iosb, (int)(0), (int)(0),
			string, size, (int)(0), (int)(0), (int)(0), (int)(0));
	if(((status & 0x1) == 0) || ((iosb[0] & 0x1) == 0)) {
		printf("Can't write to terminal; status=%d, iosb=%d\n",
			status, iosb[0]);
		exit(status);
	}
	return 0;	/* Success */
}


/*
 * __ttputc - Write a character to the display. Actually save it in our buffer
 * and write it only if the buffer is becoming full.
 * This function is called by TTPUTC().
 */
__ttputc(c)
{
	OutputBuffer[OutputBufferCounter++] = c;
	if(OutputBufferCounter >= MAXBUF)
		ttflush();
	return c;
}

/*
 * ttputc - Write a character to the display. Actually save it in our buffer
 * and write it only if the buffer is becoming full.
 */
#ifndef HEBREW
ttputc(c)
{
	return(__ttputc(c));
}
#else
ttputc(c)
{
extern int ttrow,ttcol;
int ret;

if(compose_heb && hebmode){      
	ret = __ttputc(c);
    (*term.t_move)(ttrow, ttcol+1);
}
/* else ret=write_terminal(&c, 1); */
	else ret = __ttputc(c);
	return c;
}

#endif


/*
 * ttflush - Flush our buffer to the terminal.
 */
ttflush()
{
	if(OutputBufferCounter > 0) {
		write_terminal(OutputBuffer, OutputBufferCounter);
		OutputBufferCounter = 0;
	}
}


/*
 * ttgetc - Read a character from the terminal, performing no editing 
 *          and doing no echo at all.
 */
ttgetc()
{
	unsigned char c;
	int i;
	long	status;
	unsigned long Terminator[4] = { -1, -1, -1, -1};
	struct DESC	TerminatorDesc = { sizeof(Terminator), 0, Terminator };
	short	iosb[4];

	status = sys$qiow((int)(0), TerminalChan,
		(short)(IO$_READLBLK | IO$M_NOFORMAT | IO$M_NOFILTR | IO$M_NOECHO | IO$M_TRMNOECHO),
			iosb, (int)(0), (int)(0),
			&c, 1, (int)(0), &TerminatorDesc, (int)(0), (int)(0));
	if(((status & 0x1) == 0) || ((iosb[0] & 0x1) == 0)) {
		printf("Can't read from terminal; status=%d, iosb=%d\n",
			status, iosb[0]);
		exit(status);
	}

	c &= 0xff;
	if(c == 155) c = 27;	/* Convert 8th bit escape to 7bit one */
	return((int)(c));
}


/*
 * GetKey - Read in a key.
 * Do the standard keyboard preprocessing. Convert the keys to the internal
 * character set.  Resolves escape sequences and returns no-op if global
 * timeout value exceeded.
 */
GetKey()
{
    int    c;
#ifdef HEBREW
    unsigned char *ptr;
#endif

/*~~~ HUJI/VMS - No timeout is supported on VMS to make it simple... */

    if ((c = (*term.t_getchar)()) == METACH) { /* Apply M- prefix      */
	int status;
	    
	/*
	 * this code should intercept special keypad keys
	 */
	switch(status = kbseq(&c)){
	  case 0 : 	/* no dice */
	    return(c);
	  case  K_PAD_UP		:
	  case  K_PAD_DOWN		:
	  case  K_PAD_RIGHT		:
	  case  K_PAD_LEFT		:
	  case  K_PAD_PREVPAGE	:
	  case  K_PAD_NEXTPAGE	:
	  case  K_PAD_HOME		:
	    return(status);
	  case F1  :
	  case F2  :
	  case F3  :
	  case F4  :
	  case F5  :
	  case F6  :
	  case F7  :
	  case F8  :
	  case F9  :
	  case F10 :
	  case F11 :
	  case F12 :
	    return(status);
	  case BADESC :
	    if(c == '\033'){
		c = (*term.t_getchar)();
		if(islower(c))	/* canonicalize c */
		  c = toupper(c);

		return((isalpha(c) || c == '@' || (c >= '[' && c <= '_'))
		       ? (CTRL | c) : c);
	    }

	  default :				/* punt the whole thing	*/
	    (*term.t_beep)();
	    break;
	}
    }

    if (c>=0x00 && c<=0x1F)                 /* C0 control -> C-     */
      c = CTRL | (c+'@');
#ifdef HEBREW
      if(compose_heb && (!message_mode || search_mode) && (hebmain!=eng_in_heb)){
	if(c >= 'a' && c <= 'z')c = xlat_tab[c-'a'];
	else{
	  ptr = (unsigned char *) strchr(xlat_str1, c);
	  if(ptr != NULL) c = xlat_str2[(int)(ptr - xlat_str1)];
	}
    }
#endif
    return (c);

}



/* 
 * kbseq - looks at an escape sequence coming from the keyboard and 
 *         compares it to a trie of known keyboard escape sequences, and
 *         performs the function bound to the escape sequence.
 * 
 *         returns: BADESC, the escaped function, or 0 if not found.
 */
kbseq(c)
int	*c;
{
    register char	b;
    register int	first = 1;
    register struct	KBSTREE	*current = kpadseqs;
#ifdef HEBREW
    register int        firsth = 1;
#endif

    if(kpadseqs == NULL)			/* bag it */
      return(BADESC);

    while(1){
	*c = b = (*term.t_getchar)();
#ifdef HEBREW	
	if(firsth){
	  firsth=0;
	  if(strchr(escfuns,b&0xffdf)){
	      *c &= 0xffdf;
	      *c |= META;
	      return 0;
	    }
	}
#endif
	while(current->value != b){
	    if(current->left == NULL){		/* NO MATCH */
		if(first)
		  return(BADESC);
		else
		  return(0);
	    }
	    current = current->left;
	}

	if(current->down == NULL)		/* match!!!*/
	  return(current->func);
	else
	  current = current->down;

	first = 0;
    }
}



/*
 * alt_editor - Use normal SYSTEM() and not fork.
 */
alt_editor(f, n)
{
    char   eb[NLINE];				/* buf holding edit command */
    char   *fn;					/* tmp holder for file name */
    char   *cp;
    char   *args[MAXARGS];			/* ptrs into edit command */
    long   l;
    int    i, pid, done = 0;

    if(Pmaster == NULL)
      return;

    if(gmode&MDSCUR){
	emlwrite("Alternate editor not available in restricted mode", NULL);
	return;
    }

    if(Pmaster->alt_ed == NULL){
	if(!(gmode&MDADVN)){
	    emlwrite("\007Unknown Command",NULL);
	    return;
	}

	if(getenv("EDITOR"))
	  strcpy(eb, (char *)getenv("EDITOR"));
	else
	  *eb = '\0';

	while(!done){
	    pid = mlreplyd("Which alternate editor ? ", eb, NLINE, QDEFLT);

	    switch(pid){
	      case ABORT:
		return(-1);
	      case HELPCH:
		emlwrite("no alternate editor help yet", NULL);

/* take sleep and break out after there's help */
		sleep(3);
		break;
	      case (CTRL|'L'):
		sgarbf = TRUE;
		update();
		break;
	      case TRUE:
	      case FALSE:			/* does editor exist ? */
		if(*eb == '\0'){		/* leave silently? */
		    mlerase();
		    return(-1);
		}

		done++;
		break;
	      default:
		break;
	    }
	}
    }
    else
      strcpy(eb, Pmaster->alt_ed);

    if((fn=writetmp(0, 1)) == NULL){		/* get temp file */
	emlwrite("Problem writing temp file for alt editor", NULL);
	return(-1);
    }

    strcat(eb, " ");
    strcat(eb, fn);

    cp = eb;
    for(i=0; *cp != '\0';i++){			/* build args array */
	if(i < MAXARGS){
	    args[i] = NULL;			/* in case we break out */
	}
	else{
	    emlwrite("Too many args for command!", NULL);
	    return(-1);
	}

	while(isspace(*cp))
	  if(*cp != '\0')
	    cp++;
	  else
	    break;

	args[i] = cp;

	while(!isspace(*cp))
	  if(*cp != '\0')
	    cp++;
	  else
	    break;

	if(*cp != '\0')
	  *cp++ = '\0';
    }

    args[i] = NULL;

    if(Pmaster)
      (*Pmaster->raw_io)(0);			/* turn OFF raw mode */

	system(args);
    if(Pmaster)
      (*Pmaster->raw_io)(1);		/* turn ON raw mode */

    /*
     * replace edited text with new text 
     */
    curbp->b_flag &= ~BFCHG;		/* make sure old text gets blasted */
    readin(fn, 0);			/* read new text overwriting old */
    unlink(fn);				/* blast temp file */
    curbp->b_flag |= BFCHG;		/* mark dirty for packbuf() */
    ttopen();				/* reset the signals */
    refresh(0, 1);			/* redraw */
}



/*
 *  bktoshell - suspend and wait to be woken up
 */
bktoshell()		/* suspend MicroEMACS and wait to wake up */
{
	emlwrite("\007Unknown command: ^Z", NULL);
	return;
}


/* 
 * rtfrmshell - back from shell, fix modes and return
 */
SIGTYPE
rtfrmshell()
{
}


/*
 * do_hup_signal - jump back in the stack to where we can handle this
 */
SIGTYPE
do_hup_signal()
{
    if(Pmaster){
	extern jmp_buf finstate;

	signal(SIGHUP, SIG_IGN); 		/* don't bother us. */
	signal(SIGTERM, SIG_IGN);
	longjmp(finstate, COMP_GOTHUP);
    }
    else{
	/*
	 * if we've been interrupted and the buffer is changed,
	 * save it...
	 */
	if(anycb() == TRUE){			/* time to save */
	    if(curbp->b_fname[0] == '\0'){	/* name it */
		strcpy(curbp->b_fname, "pico.save");
	    }
	    else{
		strcat(curbp->b_fname, ".save");
	    }
	    writeout(curbp->b_fname);
	}
	vttidy();
	exit(1);
    }
}


unlink(fn)
char	*fn;
{
	delete(fn);
}

/*
 * big bitmap of ASCII characters allowed in a file name
 * (needs reworking for other char sets)
 */
unsigned char okinfname[32] = {
      0,    0, 			/* ^@ - ^G, ^H - ^O  */
      0,    0,			/* ^P - ^W, ^X - ^_  */
      0xff, 0xff,		/* SP - ' ,  ( - /   */
      0xff, 0xff,		/*  0 - 7 ,  8 - ?   */
      0xff, 0xff,		/*  @ - G ,  H - O   */
      0xff, 0xff,		/*  P - W ,  X - _   */
      0xff, 0xff,		/*  ` - g ,  h - o   */
      0xff, 0xff,		/*  p - w ,  x - DEL */
      0,    0, 			/*  > DEL   */
      0,    0,			/*  > DEL   */
      0,    0, 			/*  > DEL   */
      0,    0, 			/*  > DEL   */
      0,    0 			/*  > DEL   */
};


/*
 * fallowc - returns TRUE if c is allowable in filenames, FALSE otw
 */
fallowc(c)
char c;
{
    return(okinfname[c>>3] & 0x80>>(c&7));
}


/*
 * fexist - returns TRUE if the file exists with mode passed in m, 
 *          FALSE otherwise.  By side effect returns length of file in l
 */
fexist(file, m, l)
char *file;
char *m;					/* files mode: r, w or rw */
long *l;
{
    struct stat	sbuf;

    if(l)
      *l = 0L;

    if(stat(file, &sbuf) < 0){
	switch(errno){
	  case ENOENT :				/* File not found */
	    return(FIOFNF);
#ifdef	ENAMETOOLONG
	  case ENAMETOOLONG :			/* Name is too long */
	    return(FIOLNG);
#endif
	  default:				/* Some other error */
	    return(FIOERR);
	}
    }

    if(l)
      *l = sbuf.st_size;

    if((sbuf.st_mode&S_IFMT) == S_IFDIR)
      return(FIODIR);

    if(m[0] == 'r')				/* read access? */
      return((S_IREAD&sbuf.st_mode) ? FIOSUC : FIONRD);
    else if(m[0] == 'w')			/* write access? */
      return((S_IWRITE&sbuf.st_mode) ? FIOSUC : FIONWT);
    else if(m[0] == 'x')			/* execute access? */
      return((S_IEXEC&sbuf.st_mode) ? FIOSUC : FIONEX);
    return(FIOERR);				/* what? */
}


/*
 * isdir - returns true if fn is a readable directory, false otherwise
 *         silent on errors (we'll let someone else notice the problem;)).
 */
isdir(fn, l)
char *fn;
long *l;
{
    struct stat sbuf;

    if(l)
      *l = 0;

    if(stat(fn, &sbuf) < 0)
      return(0);

    if(l)
      *l = sbuf.st_size;
    return((sbuf.st_mode&S_IFMT) == S_IFDIR);
}



/*
 * gethomedir - returns the users home directory
 *              Note: home is malloc'd for life of pico
 */
char *
gethomedir(l)
int *l;
{
    static char *home = NULL;
    static short hlen = 0;

    if(home == NULL){
	strcpy(s, "~");
	fixpath(s, NLINE);		/* let fixpath do the work! */
	hlen = strlen(s);
	if((home=(char *)malloc((strlen(s) + 1) * sizeof(char))) == NULL){
	    emlwrite("Problem allocating space for home dir", NULL);
	    return(0);
	}
	strcpy(home, s);
    }

    if(l)
      *l = hlen;

    return(home);
}


/*
 * homeless - returns true if given file does not reside in the current
 *            user's home directory tree. 
 */
homeless(f)
char *f;
{
    char *home;
    int   len;

    home = gethomedir(&len);
    return(strncmp(home, f, len));
}



/*
 * errstr - return system error string corresponding to given errno
 *          Note: strerror() is not provided on all systems, so it's 
 *          done here once and for all.
 */
char *
errstr(err)
int err;
{
    extern char *sys_errlist[];
    extern int  sys_nerr;

    return((err >= 0 && err < sys_nerr) ? sys_errlist[err] : NULL);
}



/*
 * getfnames - return all file names in the given directory in a single 
 *             malloc'd string.  n contains the number of names
 */
char *
getfnames(dn, n)
char *dn;
int  *n;
{
	int	context = 0;	/* Used nu LIB$FIND_FILE */
	struct	DESC	file_mask, file_name;
	int	status;
	char	*p, *OutputPointer;
	char	*RetFiles = malloc(8192);	/* Hopefully it will be enough... */
	char	FileName[256];	/* Temporary place */

	*n = 0;		/* Init - no files yet */
	if(RetFiles == NULL) return;
	*(OutputPointer = RetFiles) = '\0';

	file_mask.length = strlen(dn); file_mask.address = dn;
	file_mask.type = 0;

	for(;;) {
		file_name.address = FileName; file_name.length = sizeof(FileName) - 1;
		file_name.type = 0;

		status = LIB$FIND_FILE(&file_mask, &file_name, &context);
		if((status & 0x1) == 0) {
			if(context != 0)
				LIB$FIND_FILE_END(&context);
			context = 0;	/* Init for next search */
			return RetFiles;
		}

/* File found - Remove the leading directory name and the trailing spaces */
		if((p = strchr(FileName, ' ')) != NULL) *p = '\0';
		if((p = strrchr(FileName, ']')) != NULL) p++;
		else					p = FileName;
		while((*OutputPointer++ = *p++) != '\0');
		*n += 1;
	}
}


/*
 * fioperr - given the error number and file name, display error
 */
void
fioperr(e, f)
int  e;
char *f;
{
    switch(e){
      case FIOFNF:				/* File not found */
	emlwrite("\007File \"%s\" not found", f);
	break;
      case FIOEOF:				/* end of file */
	emlwrite("\007End of file \"%s\" reached", f);
	break;
      case FIOLNG:				/* name too long */
	emlwrite("\007File name \"%s\" too long", f);
	break;
      case FIODIR:				/* file is a directory */
	emlwrite("\007File \"%s\" is a directory", f);
	break;
      case FIONWT:
	emlwrite("\007Write permission denied: %s", f);
	break;
      case FIONRD:
	emlwrite("\007Read permission denied: %s", f);
	break;
      case FIONEX:
	emlwrite("\007Execute permission denied: %s", f);
	break;
      default:
	emlwrite("\007File I/O error: %s", f);
    }
}



/*
 * pfnexpand - pico's function to expand the given file name if there is 
 *	       a leading '~'
 */
char *pfnexpand(fn, len)
char *fn;
int  len;
{
    char	*pw;
    register char *x, *y, *z;
    char name[256];
    
    if(*fn == '~') {
        for(x = fn+1, y = name; *x != '/' && *x != '\0'; *y++ = *x++);
        *y = '\0';
        if(x == fn + 1) 
          pw = getenv("HOME");
        else
          pw = NULL;
        if(pw == NULL)
          return(NULL);

	/* make room for expanded path */
	for(z=x+strlen(x),y=fn+strlen(x)+strlen(pw);
	    z >= x;
	    *y-- = *z--);
	/* and insert the expanded address */
	for(x=fn,y=pw; *y != '\0'; *x++ = *y++);
    }
    return(fn);
}



/*
 * fixpath - do nothing...
 */
fixpath(name, len)
char *name;
int  len;
{
}


/*
 * compresspath - given a base path and an additional directory, collapse
 *                ".." and "." elements and return absolute path (appending
 *                base if necessary).  
 *
 *                returns  1 if OK, 
 *                         0 if there's a problem
 *                         new path, by side effect, if things went OK
 */
compresspath(base, path, len)
char *base, *path;
int  len;
{
    register int i;
    int  depth = 0;
    char *p;
    char *stack[32];

#define PUSHD(X)  (stack[depth++] = X)
#define POPD()    ((depth > 0) ? stack[--depth] : "")

    if(*path == '~'){
	fixpath(path, len);
	strcpy(s, path);
    }
    else if(*path != C_FILESEP)
      sprintf(s, "%s%c%s", base, C_FILESEP, path);
    else
      strcpy(s, path);

    p = s;
    for(i=0; s[i] != '\0'; i++){		/* pass thru path name */
	if(s[i] == '/'){
	    if(p != s)
	      PUSHD(p);				/* push dir entry */
	    p = &s[i+1];			/* advance p */
	    s[i] = '\0';			/* cap old p off */
	    continue;
	}

	if(s[i] == '.'){			/* special cases! */
	    if(s[i+1] == '.'			/* parent */
	       && (s[i+2] == '/' || s[i+2] == '\0')){
		if(!strcmp(POPD(),""))		/* bad news! */
		  return(0);

		i += 2;
		p = (s[i] == '\0') ? "" : &s[i+1];
	    }
	    else if(s[i+1] == '/' || s[i+1] == '\0'){		/* no op */
		i++;
		p = (s[i] == '\0') ? "" : &s[i+1];
	    }
	}
    }

    if(*p != '\0')
      PUSHD(p);					/* get last element */

    path[0] = '\0';
    for(i = 0; i < depth; i++){
	strcat(path, S_FILESEP);
	strcat(path, stack[i]);
    }

    return(1);					/* everything's ok */
}


/*
 * tmpname - return a temporary file name in the given buffer
 */
void
tmpname(name)
char *name;
{
    sprintf(name, "/SYS$SCRATCH/pico.%d", getpid());	/* tmp file name */
}


/*
 * Take a file name, and from it
 * fabricate a buffer name. This routine knows
 * about the syntax of file names on the target system.
 * I suppose that this information could be put in
 * a better place than a line of code.
 */
void
makename(bname, fname)
char    bname[];
char    fname[];
{
    register char   *cp1;
    register char   *cp2;

    cp1 = &fname[0];
    while (*cp1 != 0)
      ++cp1;

    while (cp1!=&fname[0] && cp1[-1]!='/')
      --cp1;

    cp2 = &bname[0];
    while (cp2!=&bname[NBUFN-1] && *cp1!=0 && *cp1!=';')
      *cp2++ = *cp1++;

    *cp2 = 0;
}


/*
 * copy - copy contents of file 'a' into a file named 'b'.  Return error
 *        if either isn't accessible or is a directory
 */
copy(a, b)
char *a, *b;
{
    int    in, out, n, rv = 0;
    char   *cb;
    struct stat tsb, fsb;
    extern int  errno;

    if(stat(a, &fsb) < 0){		/* get source file info */
	emlwrite("Can't Copy: %s", errstr(errno));
	return(-1);
    }

    if(!(fsb.st_mode&S_IREAD)){		/* can we read it? */
	emlwrite("\007Read permission denied: %s", a);
	return(-1);
    }

    if((fsb.st_mode&S_IFMT) == S_IFDIR){ /* is it a directory? */
	emlwrite("\007Can't copy: %s is a directory", a);
	return(-1);
    }

    if(stat(b, &tsb) < 0){		/* get dest file's mode */
	switch(errno){
	  case ENOENT:
	    break;			/* these are OK */
	  default:
	    emlwrite("\007Can't Copy: %s", errstr(errno));
	    return(-1);
	}
    }
    else{
	if(!(tsb.st_mode&S_IWRITE)){	/* can we write it? */
	    emlwrite("\007Write permission denied: %s", b);
	    return(-1);
	}

	if((tsb.st_mode&S_IFMT) == S_IFDIR){	/* is it directory? */
	    emlwrite("\007Can't copy: %s is a directory", b);
	    return(-1);
	}

	if(fsb.st_dev == tsb.st_dev && fsb.st_ino == tsb.st_ino){
	    emlwrite("\007Identical files.  File not copied", NULL);
	    return(-1);
	}
    }

    if((in = open(a, O_RDONLY)) < 0){
	emlwrite("Copy Failed: %s", errstr(errno));
	return(-1);
    }

    if((out=creat(b, fsb.st_mode&0xfff)) < 0){
	emlwrite("Can't Copy: %s", errstr(errno));
	close(in);
	return(-1);
    }

    if((cb = (char *)malloc(NLINE*sizeof(char))) == NULL){
	emlwrite("Can't allocate space for copy buffer!", NULL);
	close(in);
	close(out);
	return(-1);
    }

    while(1){				/* do the copy */
	if((n = read(in, cb, NLINE)) < 0){
	    emlwrite("Can't Read Copy: %s", errstr(errno));
	    rv = -1;
	    break;			/* get out now */
	}

	if(n == 0)			/* done! */
	  break;

	if(write(out, cb, n) != n){
	    emlwrite("Can't Write Copy: %s", errstr(errno));
	    rv = -1;
	    break;
	}
    }

    free(cb);
    close(in);
    close(out);
    return(rv);
}


/*
 * Open a file for writing. Return TRUE if all is well, and FALSE on error
 * (cannot create).
 */
ffwopen(fn)
char    *fn;
{
    extern FILE *ffp;

    if ((ffp=fopen(fn, "w")) == NULL) {
        emlwrite("Cannot open file for writing", NULL);
        return (FIOERR);
    }

    return (FIOSUC);
}


/*
 * Close a file. Should look at the status in all systems.
 */
ffclose()
{
    extern FILE *ffp;

    if (fclose(ffp) != FALSE) {
        emlwrite("Error closing file", NULL);
        return(FIOERR);
    }

    return(FIOSUC);
}


/*
 * P_open - run the given command in a sub-shell returning a file pointer
 *	    from which to read the output
 *
 * note:
 *	For OS's other than unix, you will have to rewrite this function.
 *	Hopefully it'll be easy to exec the command into a temporary file, 
 *	and return a file pointer to that opened file or something.
 */
FILE *P_open(s)
char *s;
{
	printf("Popen not supported on VMS yet.\n");
	return NULL;
}



/*
 * P_close - close the given descriptor
 *
 */
P_close(fp)
FILE *fp;
{
    return;
}



/*
 * worthit - generic sort of test to roughly gage usefulness of using 
 *           optimized scrolling.
 *
 * note:
 *	returns the line on the screen, l, that the dot is currently on
 */
worthit(l)
int *l;
{
    int i;			/* l is current line */
    unsigned below;		/* below is avg # of ch/line under . */

    *l = doton(&i, &below);
    below = (i > 0) ? below/(unsigned)i : 0;

    return(below > 3);
}



/*
 * pico_new_mail - just checks mtime and atime of mail file and notifies user 
 *	           if it's possible that they have new mail.
 */
pico_new_mail()
{
    int ret = 0;
    static time_t lastchk = 0;
    struct stat sbuf;
    char   inbox[256], *p;

	sprintf(inbox,"SYS$LOGIN:MAIL.MAI");

    if(stat(inbox, &sbuf) == 0){
	ret = sbuf.st_atime <= sbuf.st_mtime &&
	  (lastchk < sbuf.st_mtime && lastchk < sbuf.st_atime);
	lastchk = sbuf.st_mtime;
	return(ret);
    }
    else
      return(ret);
}



/*
 * time_to_check - checks the current time against the last time called 
 *                 and returns true if the elapsed time is > timeout
 */
time_to_check()
{
    static time_t lasttime = 0L;

    if(!timeout)
      return(FALSE);

    if(time((long *) 0) - lasttime > (time_t)timeout){
	lasttime = time((long *) 0);
	return(TRUE);
    }
    else
      return(FALSE);
}


/*
 * sstrcasecmp - compare two pointers to strings case independently
 */
sstrcasecmp(s1, s2)
QcompType *s1, *s2;
{
    register char *a, *b;

    a = *(char **)s1;
    b = *(char **)s2;
    while(toupper(*a) == toupper(*b++))
	if(*a++ == '\0')
	  return(0);

    return(toupper(*a) - toupper(*--b));
}


#ifdef	TIOCGWINSZ
/*
 * winch_handler - handle window change signal
 */
SIGTYPE winch_handler()
{
    struct winsize win;
    extern int resize_pico();

    signal(SIGWINCH, winch_handler);

    if (ioctl(0, TIOCGWINSZ, &win) == 0) {
	if (win.ws_col && win.ws_row)
	  resize_pico(win.ws_row - 1, win.ws_col);
    }
}
#endif	/* TIOCGWINSZ */


/* Copied from TCAP */
hide_cur()
{
write_terminal("\033[?25l", 6);
}
show_cur()
{
write_terminal("\033[?25h", 6);
}

o_insert()
{
	return 0;
}

o_delete()
{
	return 0;
}


init_kpnames()
{
	kpinsert("OP",F1);
	kpinsert("OQ",F2);
	kpinsert("OR",F3);
	kpinsert("OS",F4);
	kpinsert("Op",F5);
	kpinsert("Oq",F6);
	kpinsert("Or",F7);
	kpinsert("Os",F8);
	kpinsert("Ot",F9);
	kpinsert("Ou",F10);
	kpinsert("Ov",F11);
	kpinsert("Ow",F12);

	/*
	 * special keypad functions
	 */
	kpinsert("[4J",K_PAD_PREVPAGE);
	kpinsert("[3J",K_PAD_NEXTPAGE);
	kpinsert("[2J",K_PAD_HOME);
	kpinsert("[N",K_PAD_END);

	/* 
	 * ANSI mode.
	 */
	kpinsert("[=a",F1);
	kpinsert("[=b",F2);
	kpinsert("[=c",F3);
	kpinsert("[=d",F4);
	kpinsert("[=e",F5);
	kpinsert("[=f",F6);
	kpinsert("[=g",F7);
	kpinsert("[=h",F8);
	kpinsert("[=i",F9);
	kpinsert("[=j",F10);
	kpinsert("[=k",F11);
	kpinsert("[=l",F12);

    kpinsert("OA",K_PAD_UP);	/* DEC vt100, ANSI and cursor key mode. */
    kpinsert("OB",K_PAD_DOWN);
    kpinsert("OD",K_PAD_LEFT);
    kpinsert("OC",K_PAD_RIGHT);

    kpinsert("[A",K_PAD_UP);	/* DEC vt100, ANSI, cursor key mode reset. */
    kpinsert("[B",K_PAD_DOWN);
    kpinsert("[D",K_PAD_LEFT);
    kpinsert("[C",K_PAD_RIGHT);

    kpinsert("A",K_PAD_UP);	/* DEC vt52 mode. */
    kpinsert("B",K_PAD_DOWN);
    kpinsert("D",K_PAD_LEFT);
    kpinsert("C",K_PAD_RIGHT);

    kpinsert("[215z",K_PAD_UP); /* Sun Console sequences. */
    kpinsert("[221z",K_PAD_DOWN);
    kpinsert("[217z",K_PAD_LEFT);
    kpinsert("[219z",K_PAD_RIGHT);
}


#define	newnode()	(struct KBSTREE *)malloc(sizeof(struct KBSTREE))
/*
 * kbinsert - insert a keystroke escape sequence into the global search
 *	      structure.
 */
kpinsert(kstr, kval)
char	*kstr;
int	kval;
{
    register	char	*buf;
    register	struct KBSTREE *temp;
    register	struct KBSTREE *trail;

    if(kstr == NULL)
      return;

    temp = trail = kpadseqs;
    if(kstr[0] == '\033')
      buf = kstr+1;			/* can the ^[ character */ 
    else
      buf = kstr;

    for(;;) {
	if(temp == NULL){
	    temp = newnode();
	    temp->value = *buf;
	    temp->func = 0;
	    temp->left = NULL;
	    temp->down = NULL;
	    if(kpadseqs == NULL)
	      kpadseqs = temp;
	    else
	      trail->down = temp;
	}
	else{				/* first entry */
	    while((temp != NULL) && (temp->value != *buf)){
		trail = temp;
		temp = temp->left;
	    }

	    if(temp == NULL){   /* add new val */
		temp = newnode();
		temp->value = *buf;
		temp->func = 0;
		temp->left = NULL;
		temp->down = NULL;
		trail->left = temp;
	    }
	}

	if (*(++buf) == '\0'){
	    break;
	}
	else{
	    trail = temp;
	    temp = temp->down;
	}
    }
    
    if(temp != NULL)
      temp->func = kval;
}
