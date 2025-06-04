/***	LASER - A Single-Threaded Asynchronous Symbiont to drive
*		any Postscript LaserWriter.
*
*	Copyright (C) 1987, The University of Texas at Austin
*
*	Rick Watson
*	The University of Texas at Austin
*	Computation Center
*	Austin, TX 78712
*	512/471-3241
*
*	arpa:	ccaw001@utadnx.cc.utexas.edu
*	bitnet:	ccaw001@utadnx
*	span:	utspan::ccaw001
*	uucp:	...seismo!ut-ngp!rick
*
*	This user-writter symbiont is tailored specifically to the Apple
*	LaserWriter.  It has the following features:
*
*	1. Page counting is done by reading the page count from the
*	   laserwriter at the beginning and the end of each file.
*	   This mechanism also "synchronizes" the user job with the
*	   laserwriter.  If the laserwriter does not respond to the
*	   first request for a page count, the symbiont notifies the
*	   operator, and continues to poll the laserwriter.  This is
*	   usefull if the laserwriter is being shared with MAC users.
*
*	2. Parameters may be passed to postscript modules in the setup
*	   library via 2 mechanisms.  The text from a $PRINT /NOTE=<text>
*	   command will be sent first.
*	   The /PARAMETER switch may also be used.  Each entry can be
*	   one of two formats. An entry without the "=" character in
* 	   it is sent as (param<n>) <parameter>.  An entry with the 
*	   "=" character in is is sent as (<left side>) <right side>.
*	   The parameters are defined in an array called /param.
*	   Example:  /PARAMETER=(1,SIZE=5,"str=(a+b)") generates:
*	   /param [(param1) 1 (size) 5 (str) (a+b)] def
*	   All alphabetics are forced to lower case.  Using the /PARAMETER
*	   switch forces sending PARSER from LASER.TLB as part of the
*	   PostScript program sent to the printer.  Look at 
*	   other files in LASER.TLB for examples of using the PARSER
*	   module.
*
*	3. If the name of a setup module ends with "_HEX" the file to 
*	   be printed is "hexified" to the laser printer.  This is useful
*	   for sending bitimage files to the laserwriter.
*
*	4. A log of responses from the laserwriter is created.  This is 
*	   usefull for debugging postscript programs.  The file is created
*	   as <printfilespec>.LASER_LOG.  In the case of spooled output,
*	   the log file is written to LASER$LOG:<printfilespec>.LASER_LOG.
*	   These files are automatically deleted if empty.
*
*	5. Printer error messages from the laserwriter are sent to operators
*	   enabled to receive PRINT messages from OPCOM.
*
*	6. receiving a %%[Flushing from the printer aborts data until
*		the next 'EOD' or end of file.
*
*	7. Spooled files:  Spooled files normally default to FORMS=HEADERS
*	   if the recommended queue initialize procedure is followed.
*	   By specifying special filenames, the user can override the
*	   forms used, and pass parameters to the setup module.
*	   The syntax is:  "_<formname>._<parameter list>"
*	   where <formname> is the name of a form to use and
*	   <parameter list> is of the form: <parameter>-<value>[_...].
*	   (Note the use of "-" instead of "=" for a separator.)
*	   For example:  $ copy zeta.plt alw:_zeta._ncopies-5_scale-2
*	   (The kludge of the century, huh?)
*
*	8. Handles LAT devices, LAT passwords, laserwriter passwords,
*	   saves printers pages count into a file. Default parameters
*	   may be given to a queue, physical or generic with the
*	   LASER$PARAMETER_<queue name> logical. Handles stalling.
*
*	Outputs (in the following order) to the laserwriter:
*
*	1. "/note <notestring> def\r\n".
*	2.  /param definition and library module PARSER if /PARAMETER switch 
*	    present
*	3. If the setup module is "HEADERS", the string 
*		"/filename <filename> def" 
*	    and "/formname <name of form> def"
*	3. The first file or forms setup module if specified.
*	4. The file to be printed itself.  If the setup module ends in
*	   "_HEX" the file is hex-ified to the printer instead.
*
*	where: <notestring> is the string from $PRINT /NOTE=<notestring>
*	       <paramstring> is the string from $PRINT /PARAM=("<string>"...)
*	       <filename> is the file specification for the file to print.
*
*	Limitations, bugs, etc:
*
*	1. No checkpointing is done.
*	2. $PRINT/COPIES=n sends the job to the LaserWriter n times. This
*	   is not optimal, but is how the job controller handles things.
*	   It makes sense for most printers.  Multiple copy handling is
*	   better handled in most of the postscript setup files.  Also
*	   /note="/#copies n def" will usually work.
*	3. $PRINT/SPACE=n is not implemented.
*	4. BURST pages are not implemented. (But TRAILER pages are).
*	   Trailer pages are used because the stack on "top" of the job.
*	5. FORTRAN carriage control is incompletely implemented.
*	6. Does not handle lists of setup modules.
*
*	Recommended device setup:
*
*	$ define/system/exec/trans=(concealed,terminal) alw <term>: ! laser
*	$ set term /perm /speed=9600 /nomodem /notype /nohang alw
*	$ set term /perm /hostsync /pasthru /ttsync /eightbit /nobroad alw
*	$ set term /perm /noauto alw
*	$ set prot=o:rwlp /dev alw
*	$ set device /spooled=(ALW,sys$sysdevice:) alw
*
*	Recommended queue initialize:
*
*	$ initialize/queue/start -
*	/default=(noburst,nofeed,noflag,notrailer,form=headers) -
*	/separate=(noburst,noflag,trailer) - 
*	/library=laser /base_prio=8 /form=headers -
*	/processor=laser /on=alw: alw
*
*	Modification History:
*	12-Feb-1987 R. Watson	Created.
*	15-Apr-1987 R. Watson   Added some record format processing (cr,
*				fortran).
*	16-Apr-1987 R. Watson	Allow ':' as a parameter separator.
*	21-Apr-1987 R. Watson	Some mods to trailer page. Send PATCH module
*				if v 23.0 printer.
*	24-Apr-1987 R. Watson	This version submitted to Spring '87 Decus.
*	22-feb-1995 gg		This versin submitted to decus.
*/

/*
***	notes de réalisation:	(GG)
***	il ne faut *jamais* faire appel aux routines d'I/O C
***	car au premier appel vaxcio ouvre sys$error, ce qui fait raler
***	JOB_CONTROL car sys$error *est* la mbx de JOB_CONTROL ...
*/


#include <ctype.h>
#include <errno.h>
#include <signal.h>			/* GG pour sleep */
#include <stdarg.h>			/* GG */
#include <stdio.h>
#include <stdlib.h>			/* GG */
#include <string.h>			/* GG */

#include <acldef.h>			/* GG */
#include <armdef.h>			/* GG */
#include <assert.h>			/* GG */
#include <brkdef.h>			/* GG */
#include <chpdef.h>			/* GG */
#include <dcdef.h>			/* GG */
#include <descrip.h>
#include <dvidef.h>			/* GG */	
#include <iodef.h>
#include <jpidef.h>			/* GG */
#include <fscndef.h>			/* GG */
#include <latdef.h>			/* GG */
#ifdef __DECC
#include <latmsgdef.h>
#else
#include "latmsg.h"	/* LAT$_DISCONNECTED missing */
#endif
#include <lbrdef.h>
#include <libdtdef.h>			/* GG */
#include <lnmdef.h>			/* GG */
#include <opcdef.h>
#include <prvdef.h>
#include <quidef.h>
#include <rms.h>
#include <sjcdef.h>			/* GG */
#include <ssdef.h>
#include <syidef.h>			/* GG */
#include <ttdef.h>
#include <tt2def.h>

#include "laser.h"
#include "smbdef.h"

int
	SYS$HIBER(void),
	SYS$SETAST(int),
	SYS$WAKE();
int
	SYS$ASSIGN(),
	SYS$DASSGN(),
	SYS$QIO(),
	SYS$QIOW();
int
	SYS$CLOSE(),
	SYS$CREATE(),
	SYS$CONNECT(), 
	SYS$DISCONNECT(),
	SYS$GET(),
	SYS$OPEN(), 
	SYS$PUT(),
	SYS$UPDATE(); 
int
	STR$APPEND(),
	STR$COPY_DX(),
	STR$COPY_R(),
	STR$FREE1_DX();

typedef enum {false,true} boolean;	/* GG */
#define istrue(x) (x&1)			/* GG */
#define isfalse(x) ((x&1) == 0)		/* GG */

#define min(a,b) ((a>b) ? b : a)	/* GG */

typedef enum   {itm_string, itm_numeric, itm_uic, itm_time, itm_fid, itm_other,
		itm_vector}
		itm_type;

static	itm_type	itemtype[SMBMSG$K_MAX_ITEM_CODE];


/* debug data (GG) */
static int	debug;
static struct	FAB	dbg_fab;
static struct	RAB	dbg_rab;

static
void
dbg_init(file)
char * file;
{
static	const	char dbglogname[] = "LASERSMB";
static	const	char dbgdefname[] = "LASER$LOG:.DBG_LOG";
	int stat;

	dbg_fab = cc$rms_fab;		/* init fab */

	dbg_fab.fab$l_dna = (char *) &dbgdefname;
	dbg_fab.fab$b_dns = sizeof dbgdefname -1;

	if (file==NULL) {
		dbg_fab.fab$l_fna = (char *) &dbglogname;
		dbg_fab.fab$b_fns = sizeof dbglogname -1;
	}
	else	{
		dbg_fab.fab$l_fna = file;
		dbg_fab.fab$b_fns = strlen (file);
	}

	dbg_fab.fab$b_rfm = FAB$C_VAR;
	dbg_fab.fab$b_rat = FAB$M_CR;
	dbg_fab.fab$b_shr = FAB$M_SHRGET;
	dbg_fab.fab$w_mrs = 512;

	stat = SYS$CREATE(&dbg_fab);
	if (stat & 1) {
		dbg_rab = cc$rms_rab;			/* initialize rab */
		dbg_rab.rab$l_fab = &dbg_fab;
		stat = SYS$CONNECT(&dbg_rab);
	}
	debug= stat & 1;	
}

static
void
dprintf (const char * line,...)
{
	if (dbg_rab.rab$w_isi) {
		int SYS$FLUSH();
		char	buf[512];
		va_list ap;
		int sts;

		va_start (ap,line);
		sts= SYS$SETAST (0);
		(void) vsprintf(buf, line, ap);
		dbg_rab.rab$l_rbf= (char *) &buf;
		dbg_rab.rab$w_rsz= strlen (buf);
		if (buf[dbg_rab.rab$w_rsz-1]=='\n') dbg_rab.rab$w_rsz--;
		SYS$PUT (&dbg_rab);
		SYS$FLUSH (&dbg_rab);
		if (sts==SS$_WASSET) (void) SYS$SETAST (1);
		va_end (ap);
	}
}

static 
void
dbg_close()
{
	int	status, aststs;

	aststs= SYS$SETAST(0);

	status= SYS$DISCONNECT (&dbg_rab);
	if ((status&1) == 0) dprintf ("Error disconnecting trace: 0x%X", status);
	status= SYS$CLOSE (&dbg_fab);
	if ((status&1) == 0) dprintf ("Error closing trace: 0x%X", status);
	else debug= 0;

	if (aststs==SS$_WASSET) (void) SYS$SETAST(1);
}

static void
print_efs()
{
	unsigned long efn[2];
	int SYS$READEF();

	SYS$READEF (0, efn);
	SYS$READEF (32, efn+1);
	dprintf ("Event flags: %08X%08X", efn[1], efn[0]);
}


static void
print_astcnt(char * mess)
{
static	unsigned short astcnt;
static	struct {uint16  bufsiz, bufcod;
		ptr32 bufadr, retadr; uint32 endlst;} jpi_lst=
		{2, JPI$_ASTCNT, &astcnt, 0, 0};
	int	status;
	int	SYS$GETJPIW();

	status= SYS$GETJPIW (0, 0, 0, &jpi_lst, 0, 0, 0);
	dprintf ("ASTCNT %s: %d, status= %08X", mess, astcnt, status);
}

/* End of debug data	*/

/*
 *	checkstat - check value of system service return.
 */
static void
checkstat(stat, string)
int stat;
char *string;
{
	void LIB$STOP();
	dprintf ("checkstat %s: 0x%X\n", string, stat);
	if isfalse(stat) LIB$STOP(stat);
}

/*
 *	bug - aborts the symbiont
 */
static void
bug (string)
char *string;
{
	void LIB$STOP();
	dprintf ("Fatal bug: %s", string);
	LIB$STOP(SS$_BUGCHECK);
}

static	const char lw_crlf[] =	{'\r','\n'};
static	const char lw_abort[]= {'\003','\004'};	/* ^C^D */
static	const char lw_eot[]  = {'\004'};

#pragma nostandard
globalvalue LASER$_MAXITEMS;		/* exit statuses found in laser_err */
globalvalue LASER$_ITEMNOTFOUND;
globalvalue LASER$_STREAMNOTSTART;
globalvalue LASER$_TOOMANYTABS;
globalvalue LASER$_FLUSHED;
globalvalue LASER$_SETUPNOTFOUND;
#pragma standard

struct {
	int	item_code;		/* code */
	unsigned short 	item_size;	/* size of item */
	char	*buffer;		/* address of item */
} item[MAXITEMS];
int	itmax = 0;			/* max. item index */

static uint32	laser_efn;		/* laser read event flag # */
static uint32 	gp_efn;			/* general purpose EFN */
	/* must NEVER be used on an asynchronous operation */
	/* the default state of this EFN must be ON */
int	startpage;			/* task start page # */
int	endpage;			/* task end page # */
int	jobstart;			/* job start page # */
boolean	postscript;			/* GG true if postscript file */
					/* if false, no special meaning */
boolean	patchneeded = false;		/* true if patch module sent */

typedef struct {
	int	spooled_file;		/* true if spooled file */
	boolean	status_idle;		/* true if idle status received */
	boolean	readast /* =False */;	/* true si read ast active */
	short	chan /*=0*/;		/* laserwriter channel */
	int	alive;			/* laserwriter activity marker */
	boolean	timeout;		/* GG laserwrite timeout for keyin */
	boolean	lw_clear;		/* GG ignore laserwrite 'flush' msgs */
	boolean	lw_online;		/* GG laserwriter accessible */
	int	lw_flushcnt;		/* GG counter for 'flush' messages */
	int	lw_eotcnt;		/* GG counter for ^D chars */
	boolean	lw_sync;		/* GG laserwriter SYNCing */
	boolean	stalled;		/* GG laserwriter stall in process */
	boolean paused;			/* GG pause. unimplemented */
	boolean	nousermsg;		/* GG messages not sent to the user */
	boolean	user_job_active;	/* GG printing user file */
	boolean	task_active;		/* GG a task is active */
	boolean	hexdump;		/* ascii/hex switch */
	boolean setupsent;		/* a setup has been sent to the LW GG */
	boolean	QMSbug;			/* ^T renvoie toujours "busy" */

		/*+++ these variables must be accessed with AST disabled */
	boolean stop_task;		/* JOBCTL requested abort of task */
		/*--- these variables must be accessed with AST disabled */

	boolean	check_device;		/* device check needed for this stream*/
	boolean	empty_file;		/* GG: empty file */
	int	stop_reason;		/* stop task reason code */
#define SET_STOP_REASON(code) {if (stream.stop_reason==0) stream.stop_reason= code;}
	int	print_status;		/* print status (for accounting) */
	unsigned stallmsg_count;	/* counters to avoid excessive message*/
	unsigned stallerr_count;	/* logging when printer is stalled */
	unsigned long	debug;		/* GG stream debugging flags */
#define	DBG_DUMP	1		/* GG dump lw data */
#define DBG_OPEN	2		/* GG open debug log */
#define DBG_CLOSE	4		/* GG close debug log */
#define	DBG_VFC		8		/* GG debug VFC */
#define	DBG_JOBCTL	16		/* GG debug job control */
	enum {	print_unknown,		/* GG copy module as is */
		print_headers}		/* GG setup module is headers */
	job_type;			/* GG type d'impression */

	unsigned long id /* = 0*/;	/* stream id GG */
	char	queue[64];		/* nom de la queue GG */
	char	dev[64];		/* nom du device GG */
	char	product[64];		/* nom du modele d'imprimante GG */
	char	version[64];		/* version du soft de l'imprimante GG */
	long	task_start[2];		/* date de demarrage de l'impression */

	smbmsg$r_device_status	devsts;	/* laser writer device status (GG) */

	enum	{resrc_unknown,			/* resource state unknown */
		 resrc_loaded,			/* resource loaded */
		 resrc_notloaded}		/* resource not loaded */
		resrc_state;		/* temoin présence ressource GG */

	smbmsg$r_separation_control
		sepctl;

	boolean	prtfile_open /*=0*/;	/* temoin ouverture de fichier prt */
	char	logfile_lines /*=0*/;	/* temoin ouverture de fichier log */
	char	logfile_write /* =false*/;	/* write next messages to */
							/* log file */
	char 	printernode[40];	/* nom du noeud ou est l'imprimante */
	char	servicename [MAXSERVICESIZE+1];	/* nom du service LAT */

	char	lw_pw[CONFIG_PASSWORD_SIZE+1];	/* GG lw password */
	char	lat_pw[CONFIG_PASSWORD_SIZE+1];	/* GG lat password */
	char	initstr[CONFIG_INIT_SIZE+1];	/* GG init string */

	unsigned long bcnt;		/* GG STAT: bytes sent */
	}	stream_t;

static	stream_t stream;

static	const char	default_initstr[]= CONFIG_DEFAULT_INIT;

unsigned short laser_iosb[4];		/* laser read iosb */
char	laser_buf[1024];		/* laser read buffer */
static	struct	FAB main_fab;			/* main file rms stuff */
static	struct	NAM main_nam;
static	struct	RAB main_rab;
static	struct	XABRDT main_xabrdt;	/* gg */
static	struct	XABFHC main_xabfhc;	/* gg */
struct	FAB log_fab;			/* log file rms stuff */
struct	RAB log_rab;
struct  dsc$descriptor_d library_spec = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
struct	dsc$descriptor_d buffer_des = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};

static	struct	dsc$descriptor_d
	savelog = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
		/* MAXLOGLINE first error lines for trailer */

static	unsigned short	mbxchan;	/* canal de surveillance GG */
static	char smb_nodename[20];		/* nom du noeud ou est le symbiont */
static	char smb_vmsvers[20];		/* numero de version de VMS */
static	long smb_maxbuf;		/* taille max d'un transfert */
static	long smb_maxbuf_lo=0;		/* taille ok pour un transfert */
static	long smb_maxbuf_ex=0;		/* taille trop grand  pour un transfert */
static	unsigned long	keyin_daytim[2];
static	unsigned long	stall_daytim[2];
static	const	$DESCRIPTOR(keyin_interval,"0 :0:30");	/* +30 sec */
static	const	$DESCRIPTOR(stall_interval,"0 :2:00");	/* +2 min */
#define	STALL_MINS	2		/* stall  interval, in minutes */

static	boolean	getitemdescrip();


	/*+++ macros to access AST protected variables */
#define PROT_START(s)	{ int protect$$sts= SYS$SETAST (0);\
		  	  dprintf ("PROT_START: %s status=%d\n",s,protect$$sts);
#define PROT_END(s)	if (protect$$sts== SS$_WASSET) SYS$SETAST (1);\
		  	  dprintf ("PROT_END: %s status=%d\n",s,protect$$sts);}
#define UNPROTECT(s) 	if (protect$$sts== SS$_WASSET) SYS$SETAST (1);\
		  	  dprintf ("UNPROTECT: %s status=%d\n",s,protect$$sts);
	/*--- macros to access AST protected variables */

/****** ACCOUNTING routines *******************
*/

#define ACC_KEYSZ 50
#define ACC_TIMESZ 20
#define ACC_RSZ 80

static struct FAB	cfab;
static struct RAB	crab;
static compta_status	= 0;

static
void compta_init (char * printer_name)
{
static	char comptafile[]	= "LASER$ACCOUNTING:";
static  char comptadefault[]	= "LASER_COUNTERS.DAT";
	int status;

	cfab= cc$rms_fab;
	crab= cc$rms_rab;
	cfab.fab$l_fna= (char *) &comptafile;
	cfab.fab$b_fns= sizeof comptafile-1;
	cfab.fab$l_dna= (char *) &comptadefault;
	cfab.fab$b_dns= sizeof comptadefault-1;
	cfab.fab$b_org= FAB$C_IDX;
	cfab.fab$b_fac= FAB$M_GET|FAB$M_PUT|FAB$M_UPD;
	cfab.fab$b_shr= FAB$M_SHRGET|FAB$M_SHRPUT|FAB$M_SHRUPD|FAB$M_SHRDEL;
	status= SYS$OPEN (&cfab);
	if (status&1) {
		crab.rab$l_fab= &cfab;
		status= SYS$CONNECT (&crab);
		if (status&1){
			char msg[ACC_RSZ+1];
			int i;

			sprintf (msg,"%-*s%*s%10d", ACC_KEYSZ, printer_name,
						   ACC_TIMESZ, "", 0);
			crab.rab$w_rsz= strlen (msg);
			crab.rab$l_rbf= msg;
			status= SYS$PUT (&crab);
			if (status==RMS$_DUP) status|= 1;
			checkstat (status, "Compta: $PUT record");
			crab.rab$l_kbf= msg;
			crab.rab$w_usz= strlen (msg);
			crab.rab$l_ubf= msg;
/**/
			crab.rab$b_rac= RAB$C_KEY;
			crab.rab$b_ksz= ACC_KEYSZ;
			crab.rab$l_kbf= msg;
			crab.rab$l_rop |= RAB$M_NLK;
			status= SYS$GET (&crab);
			crab.rab$b_rac= RAB$C_RFA;
			checkstat (status, "Compta: $GET record");
			crab.rab$l_rop &= ~RAB$M_NLK;
			compta_status= status;
		}
		else{	checkstat (status, "Compta: $CONNECT");
			 exit(status);
		}
	}
	else	dprintf ("Open comptafile returns %08X", status);
}

void
compta_update (int counter)
{
	char	timbuf[80];
	struct	dsc$descriptor_s timadr= {sizeof timbuf, 0, 0, 0};
	int	status;
	int	SYS$ASCTIM();

	if istrue (compta_status){
		char msg[ACC_RSZ+1];

		timadr.dsc$a_pointer= timbuf;
		crab.rab$l_ubf= msg;
		status= SYS$ASCTIM (0, &timadr, 0, 0);
		checkstat (status, "Compta: $GETTIM");
		status= SYS$GET(&crab);
		checkstat (status, "Compta: $GET");
		sprintf (msg+ACC_KEYSZ,"%*.*s%10d",
				ACC_TIMESZ, ACC_TIMESZ, timbuf, counter);
		status= SYS$UPDATE (&crab);
		checkstat (status, "Compta: $UPDATE");
	}
}


/******	L O G   F I L E routines **************
*	openuserlog - open user log file.
*
*	Create a file using the file spec of the file to be printed,
*	changing the EXT to .LASER_LOG.  Try the logical name 
*	LASER$LOG if this does not work (probably a spooled file).
*
*/
static boolean
openuserlog()
{
	int stat;
	struct dsc$descriptor_s user_d, file_d;
	char	printspec[NAM$C_MAXRSS+1],		/* print file spec */
		logdnaspec[NAM$C_MAXRSS+1];		/* log file spec */
	boolean	user_log= isfalse (stream.spooled_file);/* log in user dir. */
	int	SYS$CHECK_ACCESS();

	getitemdescrip(SMBMSG$K_USER_NAME, &user_d, true);

	log_fab = cc$rms_fab;		/* init fab */

/*  Build log file name.  If a spooled file, prepend the job name and */
/*  put the file in LASER$LOG:					      */

	getitemdescrip(SMBMSG$K_FILE_SPECIFICATION, &file_d, true);
	strncpy(printspec, file_d.dsc$a_pointer, file_d.dsc$w_length);
	printspec[file_d.dsc$w_length] = 0;

/*	we should allow an user log file only if the user has write access to */
/*	the containing directory. we do in fact allow it if the user has
/*	write access to the file being printed */

	if istrue (user_log) {
static	unsigned long chp_access = ARM$M_WRITE;
static	unsigned long chp_flags = CHP$M_WRITE;
static	struct {uint16 itmsiz, itmcod; ptr32 bufadr, retadr;}
		itmlst[] = {{sizeof chp_access, CHP$_ACCESS, &chp_access, NULL},
			    {sizeof chp_flags, CHP$_FLAGS, &chp_flags, NULL},
			    {0, 0, 0, 0}};
static	unsigned long objtyp= ACL$C_FILE;
		unsigned long status;

		status= SYS$CHECK_ACCESS (&objtyp,
					  &file_d,
					  &user_d,
					  &itmlst);
		dprintf ("$CHECK_ACCESS returns %08X", status);
		user_log= istrue (status);
	}

	if isfalse(user_log) {
		int i;
		char * cp;

		strncpy(logdnaspec, user_d.dsc$a_pointer, user_d.dsc$w_length);
		i = user_d.dsc$w_length;	/* trim trailing blanks */
		do {
			logdnaspec[i] = 0;
			if (logdnaspec[--i] != ' ') break;
		} while (i > 0);
		strcat(logdnaspec, "-");
		cp = strchr(printspec, ']') + 1;
		strcat(logdnaspec, cp);
	}
	else	strcpy (logdnaspec, printspec);
	
	log_fab.fab$l_dna = logdnaspec;
	log_fab.fab$b_dns = strlen(logdnaspec);
	if isfalse (user_log) {
		log_fab.fab$l_fna = "LASER$LOG:.laser_log;0";
		log_fab.fab$b_fns = 22;
	}
	else {
		log_fab.fab$l_fna = ".laser_log;0";
		log_fab.fab$b_fns = 12;
	}
	log_fab.fab$b_rfm = FAB$C_STMLF;	/* FAB$C_VAR */
	log_fab.fab$b_rat = FAB$M_CR;
	log_fab.fab$w_mrs = 512;

	stat = SYS$CREATE(&log_fab);
	if ((stat & 1) == 0) {
		dprintf ("Cannot $OPEN %s, status= %08X", logdnaspec, stat);
		return false;		/* if not possible */
	}

	log_fab.fab$b_rfm = FAB$C_UDF;		/* undef. format */
	log_rab = cc$rms_rab;			/* initialize rab */
	log_rab.rab$l_fab = &log_fab;

	stat = SYS$CONNECT(&log_rab);
	if ((stat & 1) == 0) {
		dprintf ("Cannot $CONNECT %s, status= %08X", logdnaspec, stat);
		return false;		/* if not possible */
	}

	stream.logfile_lines = 1;		/* temoin ouverture du log */
	return true;
}


/*
 *	closeuserlog - close user log file.
 */
static void
closeuserlog()
{
	int stat;

/*  If no records written, set for delete on close */

	stat = SYS$CLOSE(&log_fab);
	checkstat(stat,"close log");
	
	STR$FREE1_DX (&savelog);		/* release saved log */

	stream.logfile_lines = 0;		/* fermeture log */
}

/*
 *	writeuserlog - write line to user log file.
 *	line looks <printableline>\0<additional separators>
 *	first separator is saved in savsep, i don't see why there should
 *	be additional separators, but this is done this way...  
 */
static void
writeuserlog(iosb, savsep)
uint16	*iosb;
char	savsep;
{
static	struct dsc$descriptor_s temp =	{0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
	int stat;

	if isfalse(stream.user_job_active) return;
	if istrue(stream.nousermsg) return;

	PROT_START("writeuserlog")

	if (stream.logfile_lines==0) {
		char hdr_buf[128];
		if isfalse (openuserlog()) {

			UNPROTECT("writeuserlog")

			return;	/* cannot open */
		}

		/* write a header in the log file */

		sprintf (hdr_buf, "Messages logged by printer %s, node %s\n",
				  stream.queue, smb_nodename);
		log_rab.rab$l_rbf = hdr_buf;
		log_rab.rab$w_rsz = strlen(hdr_buf);
		stat = SYS$PUT (&log_rab, 0, 0);
	}

		/* save printable line for trailer page */

	if (stream.logfile_lines<=MAXLOGLINE) {	/* save first 5 lines */
		stream.logfile_lines++;
		temp.dsc$w_length = iosb[1]+1;	/* incl '\0' */
		temp.dsc$a_pointer = laser_buf;
		STR$APPEND (&savelog, &temp);
	}

		/* save anything in log file */

	laser_buf[iosb[1]]= savsep;	/* restore separator */
	log_rab.rab$l_rbf = laser_buf;
	log_rab.rab$w_rsz = iosb[1]+iosb[3];

	stat = SYS$PUT (&log_rab, 0, 0);
	checkstat(stat, "write log");

	PROT_END("writeuserlog")

}
	/******	end of  L O G   F I L E S routines	*****/
	/****** start of print file routines GG		*****/
/*
 *	openfile - open the main file.
 *	return:	false if open problems.
 */
static
openfile()
{
	int stat;
	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
static	struct dsc$descriptor_s fid =
		{sizeof main_nam.nam$t_dvi+sizeof main_nam.nam$w_fid,
		 DSC$K_DTYPE_T, DSC$K_CLASS_S, main_nam.nam$t_dvi};

	if istrue(stream.prtfile_open) return true;

	getitemdescrip(SMBMSG$K_FILE_IDENTIFICATION, &temp, true);
	
	main_fab = cc$rms_fab;			/* init fab */
	main_fab.fab$l_nam = &main_nam;		/* use NAM to open by FID */
	main_fab.fab$l_fop |= FAB$M_NAM;
	
	main_nam = cc$rms_nam;			/* init nam */
	STR$COPY_DX (&fid, &temp);

	main_xabrdt = cc$rms_xabrdt;		/* gg: XAB pour recuperer */
	main_fab.fab$l_xab= (void *) &main_xabrdt;		/* la rdt */
	main_xabfhc = cc$rms_xabfhc;		/* gg: XAB pour recuperer */
	main_xabrdt.xab$l_nxt= (void *) &main_xabfhc;	/* le file header */

	stat = SYS$OPEN(&main_fab);
	if isfalse(stat) {			/* if open error */
		dprintf ("$OPEN prfile returns %08X", stat);
		SET_STOP_REASON (stat);
		return false;
	}
	
	main_rab = cc$rms_rab;			/* initialize rab */
	main_rab.rab$l_fab = &main_fab;

	stat = SYS$CONNECT(&main_rab);
	if isfalse(stat) {			/* if connect error */
		dprintf ("$CONNECT prfile returns %08X", stat);
		SET_STOP_REASON (stat);
		return false;
	}

	stream.prtfile_open= true;
	return true;
}
/*
 *	closefile - close main file.
 */
static void
closefile()
{
	int stat;

	stat = SYS$CLOSE(&main_fab, 0, 0);

	stream.prtfile_open= false;
	/*  ignore error */
}	/****** end of print file routines GG		*****/

/*
 *	sendmess - operator message routines	GG
 *	mess: asciz message sent to printer operator and/or job originator user
 *	context: AST or synchronous
 */
#define	MSG_OPER 1
#define MSG_USER 2
#define OPC_MAX_MSG_SIZE 255	/* Cf system services reference manual */

static void
sendmess (mess,to)
char * mess;
int	to;
{
	int	stat;
	int	SYS$SNDOPR(),
		SYS$BRKTHRUW();

	if (to&MSG_OPER) {
static		struct {		/* operator message to printer */
			struct {
				char type;
				char target;
				short fill;
				long id;
			} hdr;
			char text[OPC_MAX_MSG_SIZE+1];
		} msg = {{OPC$_RQ_RQST, OPC$M_NM_PRINT, 0, 0},""};
static		struct dsc$descriptor_s oprmsg_dsc = 
			{0, DSC$K_DTYPE_T, DSC$K_CLASS_S, (char *) &msg};
		int sendsz;
		int hdrsz;

		sprintf (msg.text, "Message from printer %s, queue %s\r\n",
			stream.printernode, stream.queue);
		hdrsz= strlen (msg.text);
		sendsz= hdrsz + strlen (mess);
		if (sendsz > OPC_MAX_MSG_SIZE) {
			sendsz= OPC_MAX_MSG_SIZE;
			dprintf ("Sendmess: message %s tronqué", mess);
		}
		strncpy (msg.text+hdrsz, mess, sendsz-hdrsz);
		oprmsg_dsc.dsc$w_length = sendsz + sizeof msg.hdr;
		stat= SYS$SNDOPR (&oprmsg_dsc, 0);
		if (stat!=SS$_MBFULL)	/* OPCOM saturated... message is lost */
			checkstat (stat, "$SNDOPR");
	}

	if ( (to & MSG_USER) && isfalse(stream.nousermsg) ) {
static	struct	dsc$descriptor_s user = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
		if (getitemdescrip (SMBMSG$K_USER_NAME, &user, false)) {
			char  * cp;
			int	iosb[2];
			struct	dsc$descriptor_s messdesc = {0, 0, 0, 0};

			messdesc.dsc$w_length= strlen(mess);
			messdesc.dsc$a_pointer= mess;
			stat= SYS$BRKTHRUW (gp_efn, &messdesc, &user, BRK$C_USERNAME, 
				            iosb, 0, BRK$M_CLUSTER, BRK$C_QUEUE,
					    5, 0, 0);
			checkstat (stat, "$BRKTHRUW");
		}
	}
}

/*
 *	various system routines
 */
static void 
wake_me()
{
	int status;
	status= SYS$WAKE (0, 0);
	checkstat (status, "$WAKE");
}
static void
hibernate()
{
	int status;
	status= SYS$HIBER ();
	checkstat (status, "$HIBER");
}
static void
send_request (sp, request, accounting, status, msg)
stream_t * sp;
unsigned request;
void * accounting;
unsigned long status;
char * msg;
{
	int syssts;
	int SMB$SEND_TO_JOBCTL();
	unsigned vec[2];
static	const uint32 vbn=1;
	/* queman says garbage if chkpt data is not present (v6.2)*/
static  const struct {int l; const ptr32 p;} vbn_d= {4, (ptr32) &vbn};

	if (status) { vec[0]= 1; vec[1]= status; }

	syssts= SMB$SEND_TO_JOBCTL (&sp->id, &request, accounting,
				   &vbn_d, /*checkpoint */
				   &sp->devsts, (status) ? vec : 0);
	dprintf ("%s SEND_TO_JOBCTL: status= %08x request= %d devsts=%08X", 
					 msg,syssts, request,
				         sp->devsts.smbmsg$l_device_flags);
	if isfalse (syssts) checkstat (syssts, msg);
}

/**	check_operator - checks for a laserwriter message that should
*			 go to the operator.
*	returns false if message should be written in the log
*/
static void
wake_stream (if_sync)
boolean if_sync;	/* wake even if sync in progress */
{
	if (istrue (if_sync) || isfalse (stream.lw_sync)) wake_me();
}

static boolean
check_operator()
{
	int i;

	if (strncmp(laser_buf,"%%[ PrinterError:", 17) == 0) {
		sendmess (laser_buf, MSG_USER|MSG_OPER);
		return true;
	}

	if (strncmp(laser_buf,"%%[ Product =", 12) == 0) {
		strncpy (stream.product, laser_buf+13, sizeof stream.product-1);
		stream.product[sizeof stream.product-1] = 0;
		wake_stream (true);	/* sync may need this */
		return true;
	}

	if (strncmp(laser_buf,"%%[ Version =", 12) == 0) {
		strncpy (stream.version, laser_buf+13, sizeof stream.version-1);
		stream.version[sizeof stream.version-1] = 0;
		wake_stream (true);	/* sync may need this */
		return true;
	}

	if (strncmp(laser_buf,"%%[ exitserver:", 15) == 0) {
		sendmess (laser_buf, MSG_OPER);
		return true;
	}

	if (strncmp(laser_buf, "%%[ status: ", 12) == 0) {
		if (strncmp(laser_buf, "%%[ status: idle ]%%", 20) == 0) {
			stream.status_idle = true;
			wake_stream (true);
		}
		return true;
	}

	if (strncmp(laser_buf, "%%[ patch needed ]%%", 20) == 0) {
		patchneeded = true;
		return true;
	}

	if (strncmp(laser_buf, "%%[ start page #=", 17) == 0) {
		sscanf(&laser_buf[17], "%d", &startpage);
		wake_stream (false);
		return true;
	}

	if (strncmp(laser_buf, "%%[ end page   #=", 17) == 0) {
		sscanf(&laser_buf[17], "%d", &endpage);
		wake_stream (false);
		return true;
	}

	if (strcmp(laser_buf, "%%[ resource loaded ]%%") == 0) {
		stream.resrc_state= resrc_loaded;
		wake_stream (false);
		return true;
	}

	if (strcmp(laser_buf, "%%[ resource not loaded ]%%") == 0) {
		stream.resrc_state= resrc_notloaded;
		wake_stream (false);
		return true;
	}

	if (strcmp(laser_buf, "^C") == 0)		/* Control-C */
		return stream.lw_clear;

		/* pollution created by ms-word */

	if (strncmp(laser_buf, "%%[ document:", 13)==0) return true;
	if (strncmp(laser_buf, "%%[ job:", 8)==0) return true;

/*  Abort the job if we see "%%[ Flushing:"		     */
/*  Don't override an already existing stop_reason condition */

	if ((strncmp(laser_buf, "%%[ Flushing:", 13) == 0) ||
	    (strncmp(laser_buf, "%%[ Flushing ", 13) == 0)) {	/* GG */
		stream.lw_flushcnt++;			/* count flushs */
		if istrue(stream.lw_clear) {
			dprintf ("Ignoring flush");
			return true;
		}
		else{
			dprintf ("flushing the job");
			sendmess (laser_buf, MSG_USER);
			SET_STOP_REASON (LASER$_FLUSHED);
			return false;			/* write in log */
		}
	}

	/* a partir de ce niveau, tous les messages sont */
	/* envoyes a l'originateur du job */
                                              
	sendmess (laser_buf, MSG_USER);
	
	return false;
}

	/****** start of laserwriter I/O routines GG	*****/
/*
 *	laserwrite - write on laser
 */
static void
laserwrite (bufadr, bufsize)
char * bufadr;
int bufsize;
{
	unsigned short iosb[4];
	unsigned status;
	int	len;
	char * cp;

	for (len = bufsize, cp=bufadr; len > 0; len -= smb_maxbuf, 
						cp += smb_maxbuf) {

		int l;	/* actual size written in one QIO */

		do {

			if (smb_maxbuf < smb_maxbuf_ex-1) { /* need half > 0 */
				smb_maxbuf += (smb_maxbuf_ex-smb_maxbuf)/2;
				dprintf ("smb_maxbuf > -> %d", smb_maxbuf);
			}
			l= min (smb_maxbuf, len);
			status= SYS$QIOW (0, stream.chan,
					  IO$_WRITELBLK | IO$M_NOFORMAT,
					  &iosb, 0, 0,
					  cp, l, 0, 0, 0, 0);
			if (status == SS$_EXQUOTA) {
				smb_maxbuf_ex= smb_maxbuf;
				if (smb_maxbuf_lo < smb_maxbuf) {
					smb_maxbuf -= 
						(smb_maxbuf+1-smb_maxbuf_lo)/2;
				}
				else	smb_maxbuf /= 2;
				smb_maxbuf_lo= smb_maxbuf;
				dprintf ("smb_maxbuf %d -> %d (%d)", 
				      smb_maxbuf_ex, smb_maxbuf, smb_maxbuf_lo);
				if (smb_maxbuf==0) bug ("smb_maxbuf zeroed");
			}
		} while (status==SS$_EXQUOTA);


		if (status&1) status= iosb[0];
		if ((status&1) == 0) {
			if (status==SS$_ACCVIO) 
					checkstat (status, "Write Laser");
			dprintf ("Error during laserwrite: (%d) status= 0x%08X",
				 l, status);
			SET_STOP_REASON (status);
			break;
		}

		if (stream.debug & DBG_DUMP) {
#define DBG_DISP_MAX 64
			int lr;
			char * cpr;

			dprintf ("->:%.*s\n", min (DBG_DISP_MAX, l), cp);
			for (lr= l-DBG_DISP_MAX, cpr= cp+DBG_DISP_MAX;
				lr > 0; 
				     lr -= DBG_DISP_MAX, cpr += DBG_DISP_MAX)
			   dprintf ("  :%.*s\n", min (DBG_DISP_MAX, lr), 
					cpr);
		}

		stream.bcnt += l;				/* stat */
	}
/***		checkstat (status, "Write Laser");	***/
}

#define ADDR_LEN(str) str, sizeof str-1


static
void
cancelread (sp)
stream_t * sp;
{
	int 	stat;
	int	SYS$CANCEL();

	PROT_START("cancelread")

	print_astcnt("avant $cancel");
	stat = SYS$CANCEL (sp->chan);			/* cancel pending read*/
	checkstat (stat, "$CANCEL read");
	sp->readast = false;
	print_astcnt("apres $cancel");
	stat= SYS$QIO (0, sp->chan,
			IO$_SETMODE|IO$M_CTRLYAST, 0,
			0, 0,
			0, 0, 0, 0, 0, 0);

	checkstat (stat, "$CANCEL ^AST");
	print_astcnt("apres $cancel ^YAST");
	PROT_END("cancelread")
}
/* 
 *	postlaserread - post non blocking read for stream.chan.
 */
static	void 	laser_read_ast();	/* laser read completion ast routine */
static
void
postlaserread (sp, purge)
stream_t * sp;					/* context pointer */
int purge;					/* purge type-ahead ? */
{
static 	const	int term_block[2] = {0,-1};	/* end read on any control */
	int 	stat;

	PROT_START("postlaserread")		/* protect sp->readast */

	if isfalse(sp->readast) {
		stat = SYS$QIO (laser_efn, sp->chan, 
				IO$_READPBLK | ((purge) ? IO$M_PURGE : 0) ,
				laser_iosb, laser_read_ast, sp,
				laser_buf, sizeof laser_buf-1, 0, term_block, 0, 0);

		sp->readast = istrue (stat);
		if isfalse(stat)	checkstat(stat, "$QIO laser read");
	}
	else {
		dprintf	("BUG: postread multiple");
	}

	PROT_END("postlaserread")
}

/*
 *	laser_read_ast - laser read ast completion routine.
 */
static
void
laser_read_ast(sp)
stream_t * sp;
{
	int	iosb_stat;
	char	savterm;

	iosb_stat = laser_iosb[0];

	if ((iosb_stat == SS$_ABORT) || (iosb_stat == SS$_CANCEL)) return;

	/* pour debug seulement */

	if ((laser_iosb[1]==0) && (laser_iosb[3]==1))
		dprintf ("<-X:%02X", laser_buf[0]);
	else	{		       /* imprimer le message + terminateur */
		char prbuf [80];
		int i;
		char * cp, * cpb;

		for (i=0, cp=prbuf, cpb= laser_buf+laser_iosb[1]; 
						 i<laser_iosb[3];
						    i++, cp+= 2)
			sprintf (cp, "%02X", *cpb++);
		dprintf ("<-:%.*s <-X:%s", laser_iosb[1], laser_buf, prbuf);
	}

	/* fin debug */

	if ((laser_iosb[1]+laser_iosb[3]) != 0)
		sp->alive++;			/* note message received */

	savterm= laser_buf[laser_iosb[1]];	/* save terminator */
	laser_buf[laser_iosb[1]] = 0;		/* terminate string */

	if (iosb_stat != SS$_NORMAL){ 		/* if error or strange sts */
		dprintf ("Badly received message %04X: %s\n", 
			  iosb_stat, laser_buf);
		if (iosb_stat==SS$_HANGUP) {
			SET_STOP_REASON (iosb_stat);
			sp->lw_online= false;
			stream.product[0]= '\0';	/* undefine product */
		}
		if isfalse(sp->lw_online) return;	/* plus d'imprimante */
		checkstat (iosb_stat,"laser read AST");	/* plus de symbiont */
	}

	if (laser_iosb[1] != 0)				/* if bytes in buffer */
		stream.logfile_write= isfalse(check_operator());

	if istrue (stream.logfile_write) writeuserlog(laser_iosb, savterm);

	sp->readast= false;
	postlaserread (sp, 0);				/* issue next read */
}

/*	***	routines de surveillance du terminal	***	*/

/* 
 *	postmbxattn	-	set attention AST for mailbox	GG
 */

static	void mbx_read_ast();

static
void
postmbxattn()
{
	int stat;
	int iosb[2];

	stat = SYS$QIOW (0, mbxchan, IO$_SETMODE | IO$M_WRTATTN,
			iosb, 0, 0,
			mbx_read_ast, 0, 0, 0, 0, 0);
	checkstat(stat, "mbx write attn qio");
}

/*
 *	mbx_read_ast - read mbx attn message	GG
 */
static
void
mbx_read_ast(param)
int param;
{
	int 	stat;
	short	iosb[4];
	struct	{unsigned short msg_type;
		 char msg_data[78];} mbx_buf;

	postmbxattn();

	stat= SYS$QIOW (0, mbxchan, IO$_READVBLK|IO$M_NOW, iosb, 0, 0, 
			& mbx_buf, sizeof mbx_buf, 0, 0, 0, 0);
	dprintf ("MBX msg: status= %08X; iosb status= %04X; code= %d",
			stat, iosb[0], mbx_buf.msg_type);
}

	/***** Start of PostScript routines GG 		*****/
#define Makestring_p(c)  if (obufsiz==0) checkstat(LASER$_TOOMANYTABS,"makestring");\
			 else { obufsiz--; *cpo++ = c;}
static char *
makestring (ibuf, obuf, obufsiz)
char * ibuf;
char * obuf;
int	obufsiz;
{
	char * cpi, * cpo;

	cpo = obuf;
	Makestring_p('(');
	for (cpi= ibuf; *cpi; cpi++) {
		if (isprint(*cpi)) {
			switch (*cpi) {
			case ')':
			case '(':	/* yes ! */
			case '\\':
				Makestring_p('\\');
			default: ;
			}
			Makestring_p(*cpi);
		}
		else {	char sbuf[5];
			sprintf (sbuf,"\\%03o", (int) * (unsigned char *) cpi);
			obufsiz -= 4;
			if (obufsiz <= 0) checkstat (LASER$_TOOMANYTABS, "makestring control char");
			strcpy (cpo, sbuf);
			cpo += 4;
		}
	}
	Makestring_p (')');
	Makestring_p ('\0');
	dprintf ("makestring returns %s", obuf);

	return obuf;
}
	/***** End of PostScript routines GG 		*****/


static void

get_config (sp)
stream_t *sp;	/* inout stream */
{
	FILE	*config;
	int	rsiz;
	char	buffer[CONFIG_LINESIZE+1];
	char 	errbuf[CONFIG_LINESIZE+50+1];
	char	*cpb;
	int	i;

	sp->lw_pw[0]= 0;	/* assume not found */
	sp->lat_pw[0]= 0;
	if (sizeof default_initstr <= sizeof sp->initstr)
		strcpy (sp->initstr, default_initstr);
	else {
		sp->initstr[0]= 0;
		sendmess ("get_config: default INITSTR too long; ignored", MSG_OPER);
	}

	config= fopen (CONFIG_FILE, "r", "ctx=rec",CONFIG_DEFAULT_FILE);
	if (config==NULL) {
		dprintf ("Erreur a l'ouverture du fichier de config: %s",
			  strerror (errno, vaxc$errno));
	}
	else{
	 	for (cpb=NULL;;) {
			char	* cpl;
			char 	* cpq;

			buffer[sizeof buffer-1]= 0;
			if (fgets (buffer, sizeof buffer-1, config)==NULL)
				break;

			rsiz= strlen (buffer);
			if (buffer[rsiz-1] == '\n') buffer[rsiz-1]= 0;
			else	if (rsiz == sizeof buffer -1) {
				buffer[rsiz] = 0;
				sprintf (errbuf, "Config: ligne trop longue: %s", buffer);
				sendmess (errbuf, MSG_OPER);

				continue;	/* invalid configuration line */
			}

		/* on cherche le nom de la queue en majuscules */

			cpl= buffer;		/* saute les blancs initiaux */
			for ( ; isspace (*cpl) ; ) cpl++;

			for (cpq= sp->queue; *cpq; cpq++,cpl++) {
				char cb;

				cb= (islower (*cpl)) ? toupper (*cpl) : *cpl;
				if (cb != *cpq) break;
			}

			for ( ; isspace (*cpl) ; ) cpl++; /* saut des blancs */
			if ( *cpq || (*cpl!=CONFIG_SEP)) {
				cpl[1]= 0;	/* pour debug */
				dprintf ("Pas bon: q= %s, ligne=[%s]",
					 sp->queue, buffer);
				continue;
			}	

			cpb= cpl + 1;
			break;
		}

		if ( (cpb==NULL) && (feof (config) == 0) ) {
			sprintf (errbuf, "Config: erreur de lecture: %s", 
				 strerror (errno, vaxc$errno));
			sendmess (errbuf, MSG_OPER);
			/* erreur de lecture */
		}

		fclose (config);

		if (cpb==NULL) {
			dprintf ("Config line not found");
		}
		else {
			char * cp;

			dprintf ("Found config line %s", cpb);

	/* premier champ: password laser = chaine de caracteres sans blancs*/

			for ( ; isspace (*cpb) ; ) cpb++;
	
			for (i=1, cp= sp->lw_pw ; i<sizeof sp->lw_pw; i++) {
				if ( (*cpb==0) || (*cpb==CONFIG_SEP) || isspace(*cpb) ) break;
				*cp++ = *cpb++;
			}
			*cp = 0;

			for ( ; isspace (*cpb) ; ) cpb++;
			if (*cpb==0) return;

			if ( *cpb != CONFIG_SEP ) {
			sprintf (errbuf, "Config: ligne trop longue: %s", buffer);
				sendmess (errbuf, MSG_OPER);
				return;	/* invalid configuration line */
			}

	/* deuxieme champ: password lat = chaine de caracteres sans blancs */

			cpb++;
			for ( ; isspace (*cpb) ; ) cpb++;
	
			for (i=1, cp= sp->lat_pw ; i<sizeof sp->lat_pw; i++) {
				if ( (*cpb==0) || (*cpb==CONFIG_SEP) || isspace(*cpb) ) break;
				*cp++ = *cpb++;
			}
			*cp = 0;

			for ( ; isspace (*cpb) ; ) cpb++;
			if (*cpb==0) return;

			if ( *cpb != CONFIG_SEP ) {
				sprintf (errbuf, "Config: ligne invalide: %s", buffer);
				sendmess (errbuf, MSG_OPER);
				return;	/* invalid configuration line */
			}

	/* troisieme champ: chaine de caracteres avec blancs supprimés */

			cpb++;

			for (i=1, cp= sp->initstr; i<sizeof sp->initstr; i++) {
				for (;isspace(*cpb);) cpb++;
				if ( (*cpb==0) || (*cpb==CONFIG_SEP)) break;
				*cp++ = *cpb++;
			}
			*cp = 0;

			if ( (*cpb!=0) ) {
				sprintf (errbuf, "Config: ligne invalide: %s", buffer);
				sendmess (errbuf, MSG_OPER);
				return;	/* invalid configuration line */
			}
		}
	}
}


/*
 *	utilitaires varies
 */
static
int
trnlnm (logname, logval, valsiz, vallen)
char * logname, *logval;	/* nom a traduire, buffer de traduction */
int valsiz, * vallen;		/* taille buffer traduction, taille valeur lue*/
{
	int status;
	uint16 retlen;
	struct {uint16 bufsiz, bufcod;
		ptr32  bufadr, retadr;
		uint32 endlist;}
		itmlist= {0, LNM$_STRING, NULL, NULL, 0};
static	const	unsigned long log_attr= LNM$M_CASE_BLIND;
static	const	$DESCRIPTOR (tab_desc,"LNM$SYSTEM");
	struct dsc$descriptor log_desc= {0, 0, 0, 0};
	int	SYS$TRNLNM();

	itmlist.bufsiz= valsiz-1;
	itmlist.bufadr= logval;
	itmlist.retadr= &retlen;
	log_desc.dsc$w_length= strlen (logname);
	log_desc.dsc$a_pointer= logname;
	status= SYS$TRNLNM (&log_attr, &tab_desc, &log_desc, 0, &itmlist);
	if isfalse(status) retlen= 0;

	logval [retlen]= 0;
	dprintf ("trnlnm %s returns %s status=0x%X", logname, logval, status);
	*vallen= retlen;
	return status&1;
}

static
void
get_debug (name)
char * name;
{
	char dbgbuf[256];
	int	buflen;
	char buflnm[10];

	sprintf (dbgbuf,"LASER$DEBUG_%s",name);
	if (trnlnm (dbgbuf, buflnm, sizeof buflnm, &buflen)){
		unsigned long new_debug;
		new_debug= atol (buflnm);
		if (new_debug != stream.debug) {
			stream.debug= new_debug;
			dprintf ("New debug flags: 0x%X", stream.debug);
			if ( (stream.debug&DBG_OPEN) && (debug==0)){
				dbg_init (stream.queue);
				if isfalse(debug) dbg_init(NULL);
			}
			else if ( (stream.debug&DBG_CLOSE) && debug) 
					dbg_close();
		}
	}
	else	if istrue (debug){
		dbg_close();
		stream.debug = 0;
	}
}

static
char *
to_date (dateptr)
int dateptr;
{
static	char	date[24];
static	const	$DESCRIPTOR (date_desc, date);
	unsigned short	retlen;
	unsigned long	stat;
	int	SYS$ASCTIM();

	stat= SYS$ASCTIM (&retlen, &date_desc, dateptr, 0);
	if isfalse(stat) checkstat (stat, "$ASCTIM");
	date[retlen]= 0;
	return date;
}
static
char *
to_uic (num)
int num;
{
static	char	uicval[80];
static	const	$DESCRIPTOR (uicval_d, uicval);
static	const	$DESCRIPTOR (ctrstr_d,"!%I");
	int	stat;
	uint16	retlen;
	int	SYS$FAO();

	stat=	SYS$FAO (&ctrstr_d, &retlen, &uicval_d, num);
	checkstat (stat,"$FAO uic");
	return	uicval;
}

/******	A T T E N T I O N: la routine suivante utilise une QIO non supportee **/
static
void
getlatinfo ()
{
	unsigned long stat, iosb[2];
	char buf[256], buf1[258];

	stat= SYS$QIOW (0, stream.chan, IO$_TTY_PORT | IO$M_LT_READPORT, &iosb, 0, 0,
			buf, sizeof buf, 0, 0, 0, 0);
	if ( (stat&1) && (iosb[0]&1) ) {
		char * cp = buf+buf[0]+1;
		sprintf (buf1, "%.*s%/%.*s", cp[0], cp+1, buf[0], buf+1);
		strncpy (stream.printernode, buf1, sizeof stream.printernode-1);
		stream.printernode [min(sizeof stream.printernode-1, strlen(buf1))]=0;
	}
}

/*	celle-la est supportée... Si Latmaster est installé */

static
boolean
latmaster_init (sp)
stream_t * sp;		/* current stream */
{
	int st;
	unsigned short iosb [4];
	uint8 buf[400];
	uint8 * cp = buf;
	int len;

	/* on lit toutes les caractéristiques du port */

	st= SYS$QIOW (0, stream.chan, IO$_TTY_PORT | IO$M_LT_SENSEMODE, &iosb,
			0, 0,
	   		buf, sizeof buf, 
			LAT$C_ENT_PORT | (LAT$M_SENSE_FULL << 16), 0, 0, 0);
		
	dprintf ("LT sense: status= %08X, IOSB: %08x %04x %04x\n",
		  st, * (long *) (iosb+2), iosb[1], iosb[0]);

	if ( (st&iosb[0]&1) ==0) return false;	/* Bôf */

	len = iosb[2];

	for (; cp < buf+len; cp+=2)	{
		char namebuf[80];
		char * fullname;
		int itmname= * (unsigned short *) cp;

		switch (itmname) {
			case LAT$_ITM_PORT_TYPE: fullname= "Port type"; break;
			case LAT$_ITM_QUEUED: fullname= "Enable queuing"; break;
			case LAT$_ITM_DISCONNECT_REASON: fullname= "Disconnect reason"; break;
			case LAT$_ITM_PORT_NAME: fullname= "Port name"; break;
			case LAT$_ITM_TARGET_SERVICE_NAME: fullname= "Target service name"; break;
			case LAT$_ITM_TARGET_NODE_NAME: fullname= "Target node name"; break;
			case LAT$_ITM_TARGET_PORT_NAME: fullname= "Target port name"; break;
			case LAT$_ITM_SERVICE_CLASS: fullname= "Port service class"; break;
			case LAT$_ITM_DISPLAY_NUMBER: fullname= "X display number"; break;
#ifdef __ALPHA
			case LAT$_ITM_COUNTERS: fullname= "Port counters"; break;
#endif
			default:{
				sprintf (namebuf, "Item code #0d%d: ", itmname);
				fullname= namebuf;
			}
		}
		if ( (itmname & LAT$M_STRING) != 0) {
			int strsz = cp[2];
			int copysz;

#ifdef __ALPHA
			if (itmname==LAT$_ITM_COUNTERS) {
				int rmsiz= strsz;
				uint8 * ctrptr= cp+3;

				dprintf ("\t%s:\n", fullname);
				while (rmsiz > 0) {
					uint16	ctr_name= * (uint16 *) ctrptr;
					uint32	ctr_val;
					uint8	ctr_siz;
					char namebuf[80];
					char * fullname;

					if (ctr_name&LAT$M_STRING){
						ctr_siz= ctrptr[2];
						ctrptr++; 
							/* account for str len*/
					}
					else{
						ctr_siz= 4;
					}

					switch (ctr_name) {
			case LAT$_ITM_CTPRT_SSZ: 
				fullname= "Seconds since zeroed"; break;
			case LAT$_ITM_CTPRT_BYTR: 
				fullname= "Bytes received"; break;
			case LAT$_ITM_CTPRT_BYTT: 
				fullname= "Bytes transmitted"; break;
			case LAT$_ITM_CTPRT_LCL: 
				fullname= "Local accesses"; break;
			case LAT$_ITM_CTPRT_RMT: 
				fullname= "Remote accesses"; break;
			case LAT$_ITM_CTPRT_SLCA: 
				fullname= "Solicitations accepted"; break;
			case LAT$_ITM_CTPRT_SLCR: 
				fullname= "Solicitations refused"; break;
			case LAT$_ITM_CTPRT_ISOLA:
				fullname= "Incoming solicitations accepted"; break;
			case LAT$_ITM_CTPRT_ISOLR: 
				fullname= "Incoming solicitations refused"; break;
			case LAT$_ITM_CTPRT_FRAMERR: 
				fullname= "Framing errors"; break;
			case LAT$_ITM_CTPRT_PARERR:
				fullname= "Parity errors"; break;
			case LAT$_ITM_CTPRT_OVERRUN:
				fullname= "Overruns"; break;
			case LAT$_ITM_PASSWORD_FAILURES:
				fullname= "Password failures"; break;
			default:
				sprintf (namebuf, "Counter #0d%d: ", ctr_name);
				fullname= namebuf;
					}

					ctr_val= *(uint32 *) (ctrptr+2);

					dprintf ("\t\t%s\t: %8u (%u %u)\n",
							fullname, 
							(unsigned) ctr_val,
							ctr_siz,
							rmsiz);
					ctrptr += 2+ctr_siz;
					rmsiz -=  2+ctr_siz;
				}
				if (rmsiz != 0) 
					dprintf ("BUG:rmsiz=%d\n",rmsiz);
			}
			else
#endif
				dprintf ("\t%s: %.*s\n", fullname, strsz, cp+3);

			switch (itmname) {
			case LAT$_ITM_TARGET_NODE_NAME:
				copysz= min (strsz,sizeof stream.printernode-1);
				strncpy (stream.printernode, (char *) cp+3, copysz);
				stream.printernode[copysz]= 0;
				break;
			case LAT$_ITM_TARGET_SERVICE_NAME:
				copysz= min (strsz,sizeof stream.servicename-1);
				strncpy (stream.servicename, (char *) cp+3, copysz);
				stream.servicename[copysz]= 0;
				break;
			}
			cp+= strsz+1;
		}
		else	{
			dprintf ("\t%s: %lu\n", fullname, *(long *) (cp+2));
			cp+= 4;
		}
	}

	/* on signale une fin anormale si la longueur du buffer ne */
	/* correspond pas à celle signalée dans l'IOSB */

	if  (cp!= buf+len) 
		dprintf ("Fin de buffer anormale: cp=%08X, exp=%08X",
				cp, buf+len);
	else 	dprintf ("Noeud: %s, Service: %s\n",stream.printernode,
						    stream.servicename);

	/* si le password est non nul, alors on fait setmode du port */
	/* pour specifier ce password */

	len= strlen (sp->lat_pw);
	if (len) {
		struct { unsigned short code; 
			unsigned char length;	/* password asciz */
			char password[CONFIG_PASSWORD_SIZE+1];
		      }	setbuf;

		setbuf.code= LAT$_ITM_SERVICE_PASSWORD;
		strcpy (setbuf.password, sp->lat_pw);
		setbuf.length= len;

		dprintf ("setbuf %u %d %s", (unsigned) setbuf.code, len, setbuf.password);

		st= SYS$QIOW (0, stream.chan, IO$_TTY_PORT | IO$M_LT_SETMODE,
			      &iosb, 0, 0,
			      & setbuf, 3+setbuf.length, 
			      LAT$C_ENT_PORT| (LAT$C_ENTS_OLD << 16), 0, 0, 0);
	
		dprintf ("LT setmode status: %08X, IOSB: %08x %04x %04x\n",
				st, * (long *) (iosb+2), iosb[1], iosb[0]);
	}

	return true;
}
	
/**	findandclritem - find and clear item.
*
*	Finds an item in the item list. Releases the dynamic string
*	for that item. If the item doesn't exist in the item list,
*	create it.
*
*	return:	index to item in item list
*/
static int
findandclritem(whichitem)
int whichitem;
{
static	struct dsc$descriptor_d temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
	int i;

	for (i = 0; i < itmax; i++) {		/* search item list */
		if (whichitem == item[i].item_code) {
			temp.dsc$w_length = item[i].item_size;	/* release */
			temp.dsc$a_pointer = item[i].buffer;	/*  string */
			STR$FREE1_DX (&temp);
			item[i].buffer = 0;
			return i;
		}
	}
		/* new item */
	if (itmax++ > MAXITEMS)  checkstat (LASER$_MAXITEMS, "findandclritem");
	item[i].item_code = whichitem;
	item[i].item_size = 0;
	item[i].buffer = 0;
	return i;			/* return index */
}

/**	checkspooled - check for a spooled file.
*
*	If the filename has "[]_" in it, then this is a spooled file
*	that the user wants to specify the form name for. Further,
*	if the extension starts with "_", then it contains the
*	user parameter list.  We forge the item list to make things
*	look right.
*/
checkspooled()
{
static	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
	struct dsc$descriptor_d dynd = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
	char *cp, *pp, spoolfile[255];
	int len, i, stat;
	int ourindex;
	char setup[257];			/* where to put setup name */
	long iosb[2];				/* for $getquiw */
	uint16 setupl;				/* returned length of setup */
	struct {				/* item list for $getquiw */
		uint16	bufsiz1, bufcod1;
		ptr32	bufadr1, retadr1;
		uint16	bufsiz2, bufcod2;
		ptr32	bufadr2, retadr2;
		uint32	end;
	} itmlst = {0, QUI$_SEARCH_NAME, NULL, NULL,
		    0, QUI$_FORM_SETUP_MODULES, NULL, NULL,
		    0};
	int	SYS$GETQUIW();

	itmlst.bufsiz2= sizeof setup -1;
	itmlst.bufadr2= setup;
	itmlst.retadr2= &setupl;
	stream.spooled_file = false;
	
/*  get the file specification */

	if (!getitemdescrip(SMBMSG$K_FILE_SPECIFICATION, &temp, false)) return;
	
	strncpy(spoolfile, temp.dsc$a_pointer, temp.dsc$w_length);
	spoolfile[temp.dsc$w_length] = 0;
		
/*  check for []_ */
	
	if (!(cp = strchr(spoolfile, '['))) return;
	if (!(*++cp == ']')) return;
	stream.spooled_file = true;			/* file is spooled */
	if (!(*++cp == '_')) return;
	
/*  find the extension and terminate the name string */	
	
	if (!(pp = strchr(cp, '.'))) return;
	*pp = 0;
	cp++;

/*  translate the form name into setup module names */
/*  If it won't translate, then pass on the form name */

	itmlst.bufsiz1 = strlen(cp);			/* point to name */
	itmlst.bufadr1 = cp;
	stat = SYS$GETQUIW(0, QUI$_DISPLAY_FORM, 0, &itmlst, iosb, 0, 0);
	checkstat(stat, "getquiw");
	if ((iosb[0] & 1) != 1) 		/* if no translation */
		strcpy(setup, cp);
	else
		setup[setupl] = 0;

/*  forge the file setup module to have our setup module name */
/*  Note that the forge transfers the dynamic string "dynd" to "item" */

	ourindex = findandclritem(SMBMSG$K_FILE_SETUP_MODULES);
	dynd.dsc$w_length = 0;		/* get dyamic string */
	dynd.dsc$a_pointer = 0;
	len = strlen(setup);
	STR$COPY_R (&dynd, &len, setup);
	item[ourindex].item_size = dynd.dsc$w_length;	/* complete forge */
	item[ourindex].buffer = dynd.dsc$a_pointer;

/*  forge the form name to have our form name */
	
	ourindex = findandclritem(SMBMSG$K_FORM_NAME);
	dynd.dsc$w_length = 0;		/* get dyamic string */
	dynd.dsc$a_pointer = 0;
	len = strlen(cp);			/* pointer to form name */
	STR$COPY_R (&dynd, &len, cp);
	item[ourindex].item_size = dynd.dsc$w_length;	/* complete forge */
	item[ourindex].buffer = dynd.dsc$a_pointer;
	
/*  now check to see if there are parameters */

	cp = ++pp;				/* set extension address */
	if (pp = strchr(cp,';')) *pp = 0;	/* terminate at ";" */
	
	for (i = 0; i < 8; i++) {
		if (*cp != '_') return;		/* check next char */
		cp++;				/* advance over it */

		/* if there is another _, point to it, else point to the */
		/* end of the string 					 */
		if ((pp = strchr(cp,'_')) == 0) pp = cp + strlen(cp);

		len = pp - cp;			/* find length of string */
		if (len == 0) continue;		/* if null parameter */

		/*  forge the item */
		ourindex = findandclritem(SMBMSG$K_PARAMETER_1+i);
		dynd.dsc$w_length = 0;		/* get dynamic string */
		dynd.dsc$a_pointer = 0;
		STR$COPY_R (&dynd, &len, cp);
		item[ourindex].item_size = dynd.dsc$w_length;
		item[ourindex].buffer = dynd.dsc$a_pointer;
		
		cp = pp;
	}
}

/**	expand_hex - expand buffer into hex bytes.
*/
static void
expand_hex(ibuf, obuf, rsz)
unsigned char *ibuf;			/* pointer to input buffer */
char *obuf;				/* pointer to output buffer */
int *rsz;				/* pointer to buffer size */
{
static	const	char hex[] = "0123456789ABCDEF";
	int i;
	int c;
	
	for (i = 0; i < *rsz; i++) {
		obuf[i*2] = hex[(ibuf[i] & 0xF0) >> 4];
		obuf[i*2+1] = hex[ibuf[i] & 0xF];
	}

	*rsz = (*rsz) * 2;
}

	
/**	expand_ascii - expansion des tabulations
*			et des caracteres spéciaux.
*/
#define SEQ(c1,c2) {*cpo++=c1; *cpo++= '\b' ; *cpo++= c2; col++;}; break
static void
expand_ascii(ibuf, obuf, rsz, obufsiz)
char *ibuf;				/* input buffer */
char *obuf;				/* output buffer */
int *rsz;				/* pointer to buffer size */
int obufsiz;				/* output buffer size GG */
{
	int col;				/* nº de colonne courante */
	char * cpi, *cpo= obuf, *obufend= obuf+obufsiz;

	assert((obufsiz&7) == 0);	/* sinon on risque un ecrasement mem. */

	col = 0;				/* preset column */

	for (cpi= ibuf; cpi < ibuf+ *rsz; cpi++) {

		if ( (stream.job_type==print_headers) &&
		     ( (*cpi=='\003') || (*cpi=='\004') || (*cpi=='\024') ||
		       (*cpi & 0x80)  || (*cpi == '\\') )  ) {
			if (cpo+4 >= obufend) {
				cpo= obufend;
				break;
			}
			sprintf (cpo,"\\%d", (int) * (unsigned char *) cpi); /* oui: %d */
			cpo += 4;
			col++;
		}
		else
			switch (*cpi) {
			case '\t':		/* tab */
				do {
					*cpo++ = ' ';	/* fill spaces */
					col++;		/* advance column */
				} while (col & 7);
				break;
			case '\b':		/* backspace */
				if (col > 0) {
					*cpo++ = *cpi;	/* insert backspace */
					col--;
				}
				break;

				*cpo++ = '.';
				break;
/*
/*			/* caracteres de control postscript */
/*			case '\003':	/* ^C */
/*			case '\004':	/* ^D */
/*				*cpo++ = '.';
/*				break;
/**/
			/* caractère non particulier */
			default: *cpo++ = *cpi;	/* move char */
				  col++;
			}
		if (cpo == obufend) break;
	}
	*rsz = cpo-obuf;				/* return new length */
}


/**	getitems - get item list from job controller.
*/
static
void getitems()
{
	int	stat;
	int	context;			/* read_message_item context */
	int	item_code;			/* item code */
	uint16	item_size;		/* item size */
static	struct dsc$descriptor_d item_buffer =
				{0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
	int	SMB$READ_MESSAGE_ITEM();

	context = 0;				/* init context */

/*  Save each item in a list */
	
	do {
		item_buffer.dsc$a_pointer = 0;	/* force d string allocation */

		stat = SMB$READ_MESSAGE_ITEM (&buffer_des, &context,
			 		&item_code, &item_buffer, &item_size);

		if (stream.debug & DBG_JOBCTL)
			dprintf ("Get items: item #%d, status=0x%X\n",
				 item_code, stat);

		if isfalse(stat) break;

		if (itmax >= MAXITEMS) checkstat (LASER$_MAXITEMS, "getitems");

		item[itmax].item_code = item_code;
		item[itmax].item_size = item_size;
		item[itmax].buffer = item_buffer.dsc$a_pointer;
		itmax++;

	} while (true);
}


/**	getitemdescrip - return item descriptor.
*
*	return:	true if item found
*		false if (item not found) && NOT FATAL
*		exit if (item not found) && FATAL
*/
static
boolean
getitemdescrip(code, descrip, fatal)
int code;					/* item code to search for */
struct dsc$descriptor_s *descrip;		/* pointer to descriptor */
boolean fatal;					/* error severity */
{
	int	i;

	for (i = 0; i < itmax; i++) {
		if (item[i].item_code == code) {	/* if found */
			descrip->dsc$w_length = item[i].item_size;
			descrip->dsc$a_pointer = item[i].buffer;
			return true;
		}
	}

		/* item not in list */

	if istrue(fatal)
		checkstat (LASER$_ITEMNOTFOUND, "getitemdescrip");
	else	return false;
}


/**	getcopies - send copy count to laserwriter.
*
*	Leave this here for now, but unfortunately, if you specify
*	/copies=n to the print command, the job contoller sends
*	the file that many times.  Not what we want.
*/
getcopies()
{
	int stat, n;
	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
	char defcopy[80];

	if (!getitemdescrip(SMBMSG$K_FILE_COPIES, &temp, false))
		if (!getitemdescrip(SMBMSG$K_JOB_COPIES, &temp, false))
			return;			/* if no copies */

	n = * (long*) temp.dsc$a_pointer;
	if (n == 1) return;			/* if only 1 copy */
	sprintf(defcopy, "/#copies %d def\r\n", n);

	laserwrite(defcopy,strlen(defcopy));
}

/*
 *	getnote - get /note=<Text> to laserwriter.
 */
static void
getnote()
{
static	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
	int stat;
	
	if (!getitemdescrip(SMBMSG$K_NOTE, &temp, false)) return; /* none */
	
	laserwrite (temp.dsc$a_pointer,temp.dsc$w_length);
	laserwrite (lw_crlf, sizeof lw_crlf);			/* cr-lf */
}

/**	copymodule - write module from library.
*
*	return:	true if module could be found (even if it
*			could not be sent properly)
*		false if library not found OR module not found
*/
static
boolean
copymodule(key)
struct dsc$descriptor_s key;
{
	int stat, len;
	int library_index;
	long txtrfa[2];
	char inbuf[255];
	struct dsc$descriptor_s inbufdes = 
		{sizeof inbuf , DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL};
	struct dsc$descriptor_s outbufdes;
	int
		LBR$CLOSE(),
		LBR$GET_RECORD(),
		LBR$INI_CONTROL(),
		LBR$LOOKUP_KEY(),
		LBR$OPEN();

	if (library_spec.dsc$a_pointer == 0) return false;

	inbufdes.dsc$a_pointer= inbuf;
	dprintf ("Copy module %.*s", key.dsc$w_length, key.dsc$a_pointer);

	stat = LBR$INI_CONTROL (&library_index, &LBR$C_READ, &LBR$C_TYP_TXT, 0);
	checkstat(stat, "LBR ini_control");

	stat = LBR$OPEN (&library_index, &library_spec, 0, 0, 0, 0, 0);
	checkstat(stat, "LBR open");

	stat = LBR$LOOKUP_KEY (&library_index, &key, txtrfa);
	if isfalse (stat) {			/* if bad key */
		stat = LBR$CLOSE (&library_index);
		checkstat(stat, "close library");
		return false;
	}

	do {
		stat = LBR$GET_RECORD (&library_index, &inbufdes, &outbufdes);
		if isfalse(stat) break;
		
		len = outbufdes.dsc$w_length;
#ifdef DBGMODULE
		dprintf ("Module: %.*s", len, inbuf);
#endif
		inbuf[len++] = '\r';
		inbuf[len++] = '\n';

		laserwrite(inbuf,len);
		
	} while (stream.stop_reason==0);

	if ( (stream.stop_reason==0) && (stat != RMS$_EOF) )
		checkstat (stat, "Copymodule: read");

	stat = LBR$CLOSE (&library_index);
	checkstat(stat, "LBR close");

	return true;
}

static boolean	/* param parsed successfully */
parse1param (def, str, numpar)
char * def;	/* raw definition */
char * str;	/* parsed definition */
char * numpar;	/* parameter number */
{
	char * eq, * cp;

	if (strlen (def) == 0) return false;

	for (cp=def; *cp; cp++)	/* lowercase string */
		if (isupper(*cp)) *cp = tolower (*cp);

/*  if there is an = separator (or - for spooled files), use it */
	if ((eq = strchr(def, '=')) || (eq = strchr(def, '-'))
		|| (eq = strchr(def, ':'))) {
		*eq++ = 0;			/* bust string in 2 */

	/* we allow: anything beginning like data_type */
	/* and a value that is exactly "postscript"     */

		if (!strncmp (def,"data_type",eq-def-1) &&		/* GG */
		    !strcmp (eq,"postscript")) {  	  /* check parameter  */
			postscript= true;   	    /* "data_type=postscript" */
			return false;		/* to force "POSTSCRIPT" form */
		}							/* GG */

		sprintf(str, "(%s) %s\r\n", def, eq);
	}
	else sprintf(str, "(param%d) %s\r\n", numpar, def);
	return true;
}
/*
 *	getparams - get user parameters.
 *	if present write /param [ ... ] def
 *		if parameter name given ... = (<paramname>) <paramval>
 *		else			... = (param<param#>) <paramval>
 *		and copy PARSER module
 */
static void
getparams()
{
static	const	$DESCRIPTOR(parser,"PARSER");	/* parser module name */
	int numpar;
	int first=true;
static	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

	postscript= false;			/* GG */

	for (numpar = 0; numpar < 8; numpar++) {/* loop for 8  parameters */
		char param[255], str[255+15];

		if (!getitemdescrip(SMBMSG$K_PARAMETER_1+numpar, &temp, false))
			continue;		/* if no parameter */

		if (temp.dsc$w_length == 0) continue;	/* if null string */

		strncpy(param, temp.dsc$a_pointer, temp.dsc$w_length);
		param[temp.dsc$w_length] = 0;

		if isfalse (parse1param (param, str, numpar+1)) continue;

		if (first) {				/* if first one */
			first = false;
			laserwrite (ADDR_LEN("/params [\r\n"));
		}

		laserwrite(str,strlen(str));
	}

	if istrue (first) { /* if no parameters given, use the submit queue */
			    /* default parameters */
		char lognam [256];
		char logval [256];
		int  logsiz;
		int  numpar;
		char * endstr, * start;

		getitemdescrip(SMBMSG$K_QUEUE, &temp, true);
		sprintf (lognam, "LASER$PARAMETER_%.*s", temp.dsc$w_length,
							 temp.dsc$a_pointer);
		lognam [sizeof lognam -1] = 0;
		trnlnm (lognam, logval, sizeof logval, &logsiz);

		for (numpar=1, start= logval, endstr= logval
				; endstr != NULL
				; numpar ++, start= endstr+1) {
			char str[255];

			endstr= strchr (start, ',');
			if (endstr!= NULL) *endstr= '\0';
			if isfalse(parse1param (start, str, numpar)) continue;
			if (first) {			/* if first one */
				first = false;
				laserwrite (ADDR_LEN("/params [\r\n"));
			}
			laserwrite(str,strlen(str));
		}
	}

	if isfalse (first) {			/* if some params found */
		laserwrite (ADDR_LEN("] def\r\n"));
		copymodule(parser);		/* write the parser module */
	}
}

/*	getsetup - get setup module to laserwriter.
 *
 * 	cette routine est appellée au début de chaque JOB
 *
 *	return:	true if no problems with setup module.
 *	        false if setup module nonexistant
 */
static
boolean
getsetup()
{
static	struct dsc$descriptor_s key =  {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
		
	stream.hexdump = false;			/* assume main file is ascii */
	stream.setupsent= true;			/* flag setups (for error    */
						/* detection) GG */

/*  Try for a file setup module first, then a form setup module.  For now,
    we only support one setup module, the first found. We should support
    lists of setup modules, and all found. */
	
	if (!getitemdescrip(SMBMSG$K_FILE_SETUP_MODULES, &key, false)) {
		if (!getitemdescrip(SMBMSG$K_FORM_SETUP_MODULES, &key, false))
			return true;		/* if no setup, OK */
	}
	if (key.dsc$w_length == 0) return true;	/* assume no setup => OK */
	
/*  Get out if no library. Error because the user specified a setup. */

	if (library_spec.dsc$a_pointer == 0) {
		SET_STOP_REASON (LASER$_SETUPNOTFOUND);
		return false;
	}

/*  If the setup module name ends in "_HEX", set the hexdump file flag */

	if (key.dsc$w_length >= 4)
	   if (strncmp(&key.dsc$a_pointer[key.dsc$w_length-4], "_HEX", 4) == 0)
		stream.hexdump = true;
	
/*  if this is the setup module HEADERS, assume it needs to have the file
    name defined. */

    	if (strncmp(key.dsc$a_pointer, "HEADERS", 7) == 0) {
static		struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
		char inbuf[255];

		if (postscript) return true;	/* if postscript we ignore*/
						/* the HEADERS setup GG */

		stream.job_type= print_headers;

		getitemdescrip(SMBMSG$K_FILE_SPECIFICATION, &temp, true);
		sprintf (inbuf, "/filename (%.*s) def \r\n",
				temp.dsc$w_length,
				temp.dsc$a_pointer);
		laserwrite(inbuf,strlen(inbuf));

		/*  Also send the form name if it exists */

		if (getitemdescrip(SMBMSG$K_FORM_NAME, &temp, false)) {
			sprintf (inbuf, "/formname (%.*s) def \r\n",
					temp.dsc$w_length,
					temp.dsc$a_pointer);
			laserwrite(inbuf,strlen(inbuf));
		}
	}
	
	/*  Send the setup module */

	if isfalse(copymodule(key)) {
		SET_STOP_REASON (LASER$_SETUPNOTFOUND);
		return false;
	}

	stream.setupsent= true;
	
	return true;
}

static void
keyin_timer_ast(sp)
stream_t *sp;
{
	sp->timeout= true;
	dprintf ("Keyin timeout");
	wake_me();
}
/*
 *	keyin:	 ask a question to the printer. wait for response
 *	returns	>0: keyin done successfully
 *		 0: stream.stop_reason <> 0
 *		<0: lw has gone disconnected
 */
static
int
keyin (quest, quest_l,
       test, ignerr)
char * quest;	/* question */
int quest_l;	/* longueur question */
int (* test)(int t, int * at);/* routine de test (renvoie >0 si fin de keyin) */
		/* sinon, pour le prochain intervalle de temps: */
		/*  renvoie  0 s'il faut reposer la question */
		/*  renvoie <0 s'il ne faut pas reposer la question */
		/* argument t: nombre de périodes de 5mn écoulées depuis */
		/* le début de l'appel a keyin */
boolean	ignerr;	/* continue the keyin sequence regardless of stop_reason */
{
	int	stat;				/* status VMS */
	int	timeoutcount;			/* compteur de timeouts */
	int	timeoutwarn;			/* quand avertir l'opérateur */
	int	tval= 0;			/* renvoye par le test */
	int	SYS$CANCEL();
	int	SYS$SETIMR();
	int	SYS$CANTIM();

	dprintf ("Keyin: %.*s", quest_l, quest);

	stream.timeout= true;			/* timer requested */

	for (timeoutcount = 0, timeoutwarn = 0;;) {

		stream.alive= 0;		/* counts messages received */

		if (tval >= 0 ) laserwrite (quest, quest_l);

		if istrue (stream.timeout) {
			stat = SYS$SETIMR (0,	&keyin_daytim, 
						&keyin_timer_ast,
						&stream,
					   0);
			checkstat (stat, "$SETIMR keyin");
			stream.timeout= false;
		}

		hibernate();

		if isfalse (stream.lw_online) {
			tval= -1;
			break;
		}

/*	if stop_reason is set, we exit from keyin with a 0 value */
/*	unless ignerr is set, in which case we continue */

		if (isfalse(ignerr) && (stream.stop_reason)) {	
			tval= 0;
			break;
		}

			/*  Notify the operator on 1 minute intervals. */

		if istrue (stream.timeout) {
		   timeoutcount++;
		   if ((stream.alive==0) &&	/* rien recu de l'imprimante  */
		       (timeoutcount > timeoutwarn)) { /* tous les n(n+1)/2 */
			 if (timeoutcount > 120) timeoutwarn += 120;/* 1H max */
			 else		timeoutwarn += timeoutcount;
			 if (timeoutcount > 20) /* after at least 10 mn */
				 sendmess ("LaserWriter possibly hung",	
							MSG_OPER|MSG_USER);
			}
		}

		/* si nous avons été réveillés par un wake, mais en n'ayant */
		/* rien recu de l'imprimante, si l'AST du timer n'a pas été */
		/* executée, on ne fait rien et on ne repose pas la question */

		if ( (stream.alive==0) && isfalse (stream.timeout) ){
			dprintf ("Spurious wake");
			tval= -1;
		}
		else	tval= (* test) (timeoutcount, &timeoutcount);

		if (tval > 0) break;	/* Ok */
	}

	(void) SYS$CANTIM (&stream, 0);	/* trop de precautions a prendre pour */
					/* faire ca proprement */

	return tval;
}

/*
 *	determine product type (before Sync'ing) because ^T does NOT
 *	work on some printers (e.g. QMS)
 *	will wait at most 6*keyin intervals
 */
static
int
getproduct_t(i)
int i;
{
	int retval = stream.product[0] != 0;
	if (i>2) retval=1;	/* don't hold for long if no answer */
	dprintf ("Get product_t: %d", retval);
	return retval;
}
static
int
getversion_t(i)
int i;
{
	int retval = stream.version[0] != 0;
	if (i>2) retval=1;	/* don't hold for long if no answer */
	dprintf ("Get version_t: %d", retval);
	return retval;
}
static
void
getproduct()
{
static const char questprod[]= 
"\004 statusdict begin \
(\\n%%[ Product =) print product print (\\n) print flush \
end\004";
static const char questvers[]= 
"\004 statusdict begin \
(\\n%%[ Version =) print version print (\\n) print flush \
end\004";

	keyin (questprod, sizeof questprod-1, getproduct_t, true);
	dprintf ("Product: [%s]", stream.product);
	if (strcmp (stream.product, "QMS-PS 1700") == 0) {
		keyin (questvers, sizeof questvers-1, getversion_t, true);
		dprintf ("version: [%s]", stream.version);
		/* bug seen with firmware version 22 */
#ifdef QMS1700_BUG
		stream.QMSbug= strcmp(stream.version,"52.4")==0;
#endif
	}
}
/*
 * getidle: get idle status from printer
 *	input: ignerr= ignore stop_task condition
 */
static
int
getidle_t(time, timeaddr)			/* test synchronisation */
int	time;
int	*timeaddr;
{

	if istrue(stream.status_idle) return 1;		/* if sync occured */

	if (stream.product[0] == 0) {
		getproduct();				/* in case of .. */
		if (stream.product[0] != 0) return 1;	/* maybe the question */
							/* needs to be changed*/
		if istrue(stream.status_idle) return 1;	/* if sync occured */
		time+= 6;	    /* account for time lost in getproduct */
		*timeaddr += 6;
		stream.timeout= true;	/* account for klutzy code */
	}

	/* au bout de MAXBUSY intervalles, on casse tout */

	if (time > MAXBUSY){
		stream.lw_clear= true;
			/*  Abort whatever is running */
		laserwrite (&lw_abort, sizeof lw_abort);
		dprintf ("getidle: abort sent");
		return 1;		/* retry */
	}


	/*	sinon, au bout d'1 minute, on envoie des CR/LF	*/

	else	if (time > 2) laserwrite (&lw_crlf, sizeof lw_crlf);

	if istrue(stream.stop_task) return 1;	/* goto check ignerr */

	return 0;		       /* on reste dans keyin sans changement */
}

static boolean
getidle(ignerr)
boolean ignerr;		/* ignore stop_task condition */
{
static	const	char	syncchar='\024';		/* Control-T */
static	const	char	QMSquest[]= 
	"(\\n%%[ status: idle ]%%] QMSBUG\\n) print flush\004";
	int	kstat;

	stream.status_idle = false;		/* par défaut: pas IDLE */
	stream.lw_sync = true;
	stream.nousermsg= true;

	dprintf ("Getidle >>> %s", to_date(0));

	/* on envoie ^T pour avoir le status, et on attend le résultat */

	do {
		if istrue(stream.QMSbug)
			kstat= keyin (QMSquest, sizeof QMSquest-1,
				      getidle_t, true);
		else	kstat= keyin (&syncchar, 1, 
				      getidle_t, true);
		/* loop until idle or lw offline */
		/*  or (task killed and not ignerr) */
	} while (  (isfalse(stream.status_idle) && (kstat>=0))
		&&(istrue(ignerr) || isfalse(stream.stop_task)) );

	stream.lw_sync = false;
	stream.nousermsg= false;
	stream.lw_clear= false;	/* invariant */

	dprintf ("Getidle <<< %s", to_date(0));
	return stream.status_idle;
}

/*
 *	syncprinter - get synchronized with printer.
 *	output:	stop_reason is 0 if all succeeded
 */
static
int
getstart_t()			/* test startpage */
{
	return 	startpage;		/* if sync occured */
}
/*	temps (en unités de 30 secondes) au bout duquel aborter la lw */
static boolean
syncprinter()
{
static	const	char getstart[] = {
"\004\
version (23.0) eq { statusdict /Patch1Installed known not { \
(%%[ patch needed ]%%\n) print } if } if \
statusdict begin (%%[ start page #=) print pagecount pstack pop \
flush end \r\n\004"};
static	const	$DESCRIPTOR(patch, "PATCH");		/* PATCH module name */

	startpage = 0;				/* should be read in stat msg */

	dprintf ("Syncprinter >>> %s", to_date(0));

	if isfalse (getidle(false)) return false;
	if istrue (stream.stop_task) return false;
	stream.stop_reason= 0;

	/*	si l'imprimante est toujours là, */
	/* on récupère les compteurs de page de l'imprimante */

	if (keyin (getstart, strlen (getstart), getstart_t, false) <= 0) 
			return false;

	if istrue(stream.QMSbug) {
		int savstart;
		do {
			savstart= startpage;
			sleep(10);
			if (keyin (getstart, strlen (getstart),
				   getstart_t, false) <= 0) return false;
		} while (savstart!=startpage);
	}

	/*  This looks like a good place to send the patch module */

	if istrue(patchneeded) {		/* if patch needed */
		patchneeded = false;
		copymodule(patch);		/* write the patch module */
		laserwrite (lw_eot, sizeof lw_eot);
	}

	dprintf ("Syncprinter <<< %s", to_date(0));

	compta_update (startpage);

	return true;
}

/*
 *	check_resource: verify a resource is loaded
 *	inputs: resource name
 *	outputs: 	true => resource is loaded
 *			false=> resource is not loaded or abort in progress
 */
static int
check_resource_t()
{
	return (stream.resrc_state!=resrc_unknown);
}
static boolean
check_resource(resnam)
char * resnam;
{
static	const	char verifresource[] = {
"\004\
userdict /%s known \
{(%%%%[ resource loaded ]%%%%\\n)}\
{(%%%%[ resource not loaded ]%%%%\\n)}\
ifelse print flush\
\004"};
	char	msg[sizeof verifresource+CONFIG_RESOURCE_SIZE+1];

	stream.resrc_state= resrc_unknown;
	sprintf (msg, verifresource, resnam);

	/* on envoie veriflogo et on attend le résultat */

	(void) keyin (msg, strlen (msg), check_resource_t, true);

	return (stream.resrc_state==resrc_loaded) ? true : false;
}
/* 
 *	load_resource:	download resource
 */
static void
load_resource (resfile)
char * resfile;		/* resource module name */
{
	struct	dsc$descriptor_s init_desc= {0, 0, 0, NULL};
static	const	char exitserver0[]= "serverdict begin 0 exitserver\r\n";
static	const	char exitserver[]= "serverdict begin %s exitserver\r\n";
	char	msg [sizeof exitserver+CONFIG_PASSWORD_SIZE+1];

	init_desc.dsc$w_length= strlen(resfile);
	init_desc.dsc$a_pointer= resfile;	
	stream.nousermsg= true;		/* suppression des messages "user" */

	if (stream.lw_pw[0] == '\0') 
		laserwrite (exitserver0, sizeof exitserver0 -1);
	else {
		sprintf (msg, exitserver, stream.lw_pw);
		laserwrite (msg, strlen (msg));
	}

	copymodule (init_desc);
	
	laserwrite (lw_eot, sizeof lw_eot);

		/* synchronize regardless of the error code */

	(void) getidle (true);

		/* we ignore setup errors unless the task was killed */
			/* or the lw has gone offline */

	if (isfalse (stream.stop_task) && istrue(stream.lw_online))
			stream.stop_reason= 0;

	stream.nousermsg= false;
}
/*
 *	init_lw - verify inits are loaded	(GG)
 *	returns false if cannot synch with printer
 *	outputs message if invalid configuration, but does not stop stream
 */
static boolean
init_lw()
{
	char	module_name[CONFIG_INIT_SIZE+1];
	char	module_keyword[CONFIG_INIT_SIZE+1];
	char 	* cp, *cq;
	int	loadcount=0;	/* nb of modules loaded by this procedure */

	for (cp= stream.initstr; (cp!=NULL) && (*cp!=0); ){
		cq= strchr (cp, '=');
		if ( (cq==NULL) || (cq==cp+1) ) {
			char errmsg [CONFIG_INIT_SIZE+30+1];

			sprintf (errmsg, "Invalid init substring %s", cp);
			sendmess (errmsg, MSG_OPER);
			*cp= 0;
			break;			/* invalid configuration */
		}
		strncpy (module_name, cp, cq-cp);
		module_name[cq-cp]= 0;

		cp= strchr (cq, ',');
		if (cp==NULL) 	strcpy (module_keyword, cq+1);
		else {	strncpy (module_keyword, cq+1, cp-cq-1);
			module_keyword [cp-cq-1]= 0;
			cp= cp+1;
		}
		if ( strlen (module_keyword) == 0) {
			char errmsg [CONFIG_INIT_SIZE+30+1];

			sprintf (errmsg, "Invalid init substring %s", cq);
			sendmess (errmsg, MSG_OPER);
			*cq= 0;
			break;			/* invalid configuration */
		}

		if isfalse(stream.lw_online) break;

		if isfalse(check_resource(module_keyword)){
			load_resource (module_name);
			loadcount++;
		}
	}

	if (loadcount!=0)	/* if we loaded new modules, check the */
		return getidle(false);	/* state of the laser writer  */
	else	return true;	/* otherwise, assume it is ok (save a synch) */
}

/* 
 *	doflag - do flag page if needed (GG)
 */
static void
doflag(job_flag)
boolean job_flag;
{
#define	NOTSMB	0x800
static	const	$DESCRIPTOR (flag_desc, "FLAG");
static	const	char * varnames[]={"the_jfname","the_filespec",
		"the_account","the_uic","the_printq",
		"the_submitq","the_priority",
		"the_note","the_clientuser","tdatestart",
		"tdatesubmit","tjobnum",
		"jfswitch","burst","tprintnode","vmsvers",
		"tmodified","recstring","tlongrec","t_filelen"};
/*	jfswitch doit valoir JOB ou FILE */
static	const	smbcodes[]= {NOTSMB|11, SMBMSG$K_FILE_SPECIFICATION,
		SMBMSG$K_ACCOUNT_NAME,SMBMSG$K_UIC,SMBMSG$K_EXECUTOR_QUEUE,
		SMBMSG$K_QUEUE,SMBMSG$K_PRIORITY,
		NOTSMB|1,NOTSMB,NOTSMB|2,
		SMBMSG$K_TIME_QUEUED,SMBMSG$K_ENTRY_NUMBER,
		NOTSMB|3,NOTSMB|4,NOTSMB|5,NOTSMB|6,
		NOTSMB|7,NOTSMB|8,NOTSMB|9,NOTSMB|10};
	char buf[256], wbuf[80];
	int	i;
	char	* ptr;
	int	fileok;
	int	SYS$FILESCAN();

	fileok= openfile();		/* get a few infos about file */

	for (i= 0; (i<sizeof smbcodes / sizeof smbcodes[0]) 
		    && (stream.stop_reason==0) ; i++){
		struct dsc$descriptor_s tmp;
		if (smbcodes[i]&NOTSMB) {
			switch (smbcodes[i]& ~NOTSMB) {
			case 0:		/* the client user */
				getitemdescrip (SMBMSG$K_USER_NAME,&tmp,true);
				sprintf (wbuf,"%s::%.*s", smb_nodename,
					 tmp.dsc$w_length, tmp.dsc$a_pointer);
				ptr= wbuf;
				break;
			case 1:		/* the note */
				if (getitemdescrip (SMBMSG$K_NOTE,&tmp,false)){
					sprintf (wbuf,"%.*s", tmp.dsc$w_length, 
						 tmp.dsc$a_pointer);
					ptr= wbuf;
				}
				else ptr= "";
				break;
			case 2:		/* date started */
				ptr= to_date (stream.task_start); break;
			case 3:		/* job/flag switch */
				ptr= istrue (job_flag) ? "JOB" : "FILE"; break;
			case 5:		/* printer node */
				ptr= stream.printernode; break;
			case 6:		/* VMS version */
				ptr= smb_vmsvers; break;
			case 7: 	/* Revision date */
				ptr= (fileok) ? to_date (&main_xabrdt.xab$q_rdt)
					      : ""; 
				break;
			case 9:		/* max record size */
				if (fileok) {
					sprintf (wbuf,"%d",
						 main_xabfhc.xab$w_lrl);
					ptr= wbuf;
				}
				else	ptr= "";
				break;
			case 10:	/* file size */
				if (fileok) {
					union {unsigned short word[2];
					       unsigned long vbn;
					      } pdp;
					pdp.vbn= main_xabfhc.xab$l_ebk; 
					sprintf (wbuf,"%d",
						pdp.word[2]||pdp.word[0]<<16);
					ptr= wbuf;
				}
				else	ptr= "";
				break;
			case 11:	/* job/file name */
				if istrue(job_flag)
					if (getitemdescrip (SMBMSG$K_JOB_NAME,
							    &tmp,false)){
						sprintf (wbuf,"%.*s",
							 tmp.dsc$w_length, 
							 tmp.dsc$a_pointer);
						ptr= wbuf;
					}
					else ptr= "";
				else {
					if (getitemdescrip (SMBMSG$K_FILE_SPECIFICATION,
							    &tmp,false)){
static						struct {unsigned short len,cod;
							unsigned long addr;}
							fscnlst[]= {
							{0, FSCN$_NAME,0},
							{0, FSCN$_TYPE,0},
							{0, 0, 0}};
						int stat;

						stat= SYS$FILESCAN (&tmp,
								    &fscnlst,
								    0);
						checkstat (stat, "$FILESCAN");
						sprintf (wbuf,"%.*s",
							 fscnlst[0].len+
							  fscnlst[1].len,
							 fscnlst[0].addr);
						ptr= wbuf;
					}
					else ptr= "";
				}
				break;

				default:	ptr= "Internal err";
			}
			tmp.dsc$a_pointer= ptr;
			tmp.dsc$w_length= strlen (ptr);
		}
		else {
			if isfalse(getitemdescrip (smbcodes[i], &tmp, false)){
static				const $DESCRIPTOR (not_present,"Not Present");
				tmp = not_present;
			}
			else{
				switch (itemtype[smbcodes[i]]){
				case	itm_string: break;
				case	itm_numeric: sprintf (wbuf,"%d",
					*(long *) tmp.dsc$a_pointer); break;
				case	itm_time: strcpy (wbuf, 
					 to_date (tmp.dsc$a_pointer)); break;
				case	itm_uic:  strcpy (wbuf, to_uic 
						(* (long *) tmp.dsc$a_pointer));
						break;
				default: sprintf (wbuf,"type %d not printable",
					 itemtype[smbcodes[i]]);
				}
				if (itemtype[smbcodes[i]]!=itm_string){
					tmp.dsc$a_pointer= wbuf;
					tmp.dsc$w_length= strlen (wbuf);
				}
			}
		}
		sprintf (buf,"/%s (%.*s) def\r\n", varnames[i],
				tmp.dsc$w_length, tmp.dsc$a_pointer);
		dprintf ("FLAG: %s", buf);
		laserwrite (buf,strlen(buf));
	}	/* for */

	if (stream.stop_reason==0) copymodule (flag_desc);

		/* ignore errors on flag page */

	if istrue (stream.lw_online) {
		laserwrite (lw_eot, sizeof lw_eot);
		syncprinter();	/* wait for completion */
		stream.stop_reason= 0;
	}
}
	
/*
 *	dotrailer - issue job trailer.
 *	not reentrant.
 */
static void
dotrailer()
{
	char	buf[1024];			/* scratch buffer */
	char	wbuf[512];		/* another scratch buffer */
	char	user[20], job[255], file[255], message[257];

static 	const	$DESCRIPTOR(key, "TRAILER");
static 	const	$DESCRIPTOR(key_empty, "TRAILER_EMPTY");

static	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL};

static	char time[24];
static	const	struct dsc$descriptor_s date=
		{sizeof time -1, DSC$K_DTYPE_T, DSC$K_CLASS_S, time};

	struct dsc$descriptor_s msgd =
		{sizeof message -1, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL};

static	const char z = 0;
static	const struct dsc$descriptor_s zero =	
		{1, DSC$K_DTYPE_T, DSC$K_CLASS_S, (char*) &z};
	int	LIB$DATE_TIME();
	int	SYS$GETMSG();

	msgd.dsc$a_pointer= message;
	/*  save page count if first file of job */
	if (stream.sepctl.smbmsg$v_first_file_of_job) jobstart = startpage;

	/* send trailer if there is a file trailer or a job trailer	*/
	/*	or if the job was errored				*/
	/*	or if the file was empty and the TRAILER_EMPTY module	*/
	/*		is present in the library			*/

	dprintf ("Trailer: empty= %d\n", stream.empty_file);

	if (  (stream.sepctl.smbmsg$v_job_trailer==0) 
	    &&(stream.sepctl.smbmsg$v_file_trailer== 0)
	    &&(stream.print_status == 0)
	    &&isfalse (stream.empty_file)	) return;
	
/*  send the trailer module from the library */

	dprintf ("trailers: job=%d, file=%d, status=%08x, empty=%d", 
		 stream.sepctl.smbmsg$v_job_trailer,
		 stream.sepctl.smbmsg$v_file_trailer,
		 stream.print_status, 
		 stream.empty_file);

	if istrue(stream.lw_online) {
		int savreason;
		boolean copyst;

		laserwrite (lw_eot, sizeof lw_eot);	/* leading EOF */
		savreason= stream.stop_reason;
		stream.stop_reason= 0;			/* nécessaire pour */
							/* copier le module */

		/* copy the empty file flag if the file was empty and 
			no error was detected */

		if (istrue(stream.empty_file)) {
			copyst= copymodule(key_empty);
			if (stream.stop_reason!=0) copyst= false;
			/* if the 'file empty' module could not be copied, */
			/* if no flag page is requested, just return */
			if (  isfalse (copyst)
			    &&(stream.sepctl.smbmsg$v_job_trailer==0) 
			    &&(stream.sepctl.smbmsg$v_file_trailer== 0)) {
				stream.stop_reason= savreason;
				return;
			}
			if isfalse (copyst) copyst=copymodule(key);
		}
		else 	copyst= copymodule(key);

		if (stream.stop_reason!=0) copyst= false;

		stream.stop_reason= savreason;
		if isfalse(copyst) 	return;	/* pas pu copier le module */
	}	else 			return;	/* lw offline... leave. */

	getitemdescrip(SMBMSG$K_USER_NAME, &temp, true);
	strncpy(user, temp.dsc$a_pointer, temp.dsc$w_length);
	user[temp.dsc$w_length] = 0;

	if istrue(getitemdescrip(SMBMSG$K_JOB_NAME, &temp, false)) {
		strncpy(job, temp.dsc$a_pointer, temp.dsc$w_length);
		job[temp.dsc$w_length] = 0;
	}
	else	job[0]= '\0';	/* seen in VMS V5.5-1: no JOB_NAME */

	getitemdescrip(SMBMSG$K_FILE_SPECIFICATION, &temp, true);
	strncpy(file, temp.dsc$a_pointer, temp.dsc$w_length);
	file[temp.dsc$w_length] = 0;

	LIB$DATE_TIME (&date);
	time[sizeof time-1] = 0;

	sprintf(buf,
		 "setuppage (%s) h-job (%s) h-user (%s) h-file (%s) h-time  ",
		  job, user, file, time);
	dprintf ("trailer: %s", buf);
	laserwrite(buf,strlen(buf));

/*  only print page count on last trailer page */

	if (stream.sepctl.smbmsg$v_last_file_of_job) {	/* if last one */
		sprintf(buf, "(%d) h-pages ", jobstart);
		dprintf ("trailer: %s", buf);
		laserwrite(buf,strlen(buf));
	}

/*  si fin de job anormale (stop_task de JOBCTL ou stop_reason du symbiont) */

	if (stream.print_status != 0) {
		int	stat;
		unsigned short msglen;

		msglen = 0;			/* in case of error */
		stat = SYS$GETMSG(stream.print_status, &msglen, &msgd, 15, 0);
		checkstat(stat,"$getmsg");
		message[msglen] = 0;
		sprintf(buf, "%s h-error ",
			     makestring (message, wbuf, sizeof wbuf));
		laserwrite(buf,strlen(buf));
		dprintf ("trailer: %s", buf);
	}

/*	si log file, alors le recopier sur la trailer page */
	
	if (stream.logfile_lines) {			/* if log file */
		char *cp;

		STR$APPEND (&savelog, &zero);		/* marque la fin */
		for (cp=savelog.dsc$a_pointer; *cp; cp+=strlen(cp)+1) {	
			sprintf(buf, "(%s) h-logline\n", cp);
			laserwrite(buf,strlen(buf));
			dprintf ("trailer: %s", buf);
		}
	}

	laserwrite (ADDR_LEN(" showpage \004"));

	/* in debug mode, we sync with the printer not to lose any message */

	if (stream.debug!=0) (void) getidle(true);
}

/*
 *	inititems - initialize item list.
 */
inititems()
{
	int	i;
static	const	unsigned char numeric [] = 
	{SMBMSG$K_ALIGNMENT_PAGES, SMBMSG$K_BOTTOM_MARGIN,
	SMBMSG$K_DEVICE_STATUS, SMBMSG$K_ENTRY_NUMBER, SMBMSG$K_FILE_COPIES,
	SMBMSG$K_FILE_COUNT, SMBMSG$K_FIRST_PAGE, SMBMSG$K_FORM_LENGTH,
	SMBMSG$K_FORM_WIDTH, SMBMSG$K_JOB_COPIES, SMBMSG$K_JOB_COUNT, 
	SMBMSG$K_LAST_PAGE, SMBMSG$K_LEFT_MARGIN, SMBMSG$K_PRINT_CONTROL,
	SMBMSG$K_PRIORITY, SMBMSG$K_RELATIVE_PAGE, SMBMSG$K_REQUEST_CONTROL,
	SMBMSG$K_RIGHT_MARGIN, SMBMSG$K_SEPARATION_CONTROL, SMBMSG$K_TOP_MARGIN,
	SMBMSG$K_STOP_CONDITION};
	for (i = 0; i< sizeof numeric / sizeof numeric[0]; i++)
		itemtype[numeric[i]]= itm_numeric;
	itemtype[SMBMSG$K_TIME_QUEUED]= itm_time;
	itemtype[SMBMSG$K_MESSAGE_VECTOR]= itm_vector;
	itemtype[SMBMSG$K_FILE_IDENTIFICATION] = itm_fid;
	itemtype[SMBMSG$K_CHARACTERISTICS]= itm_other;
	itemtype[SMBMSG$K_AFTER_TIME]= itm_time;
	itemtype[SMBMSG$K_UIC]= itm_uic;
	for (i = 0; i < MAXITEMS; i++) item[i].buffer = 0;
}


/*
 *
 */
typedef struct {uint16 stat;
		uint16 latv1code;
		uint32 latv2code;
		} lat_iosb_t;
static
void lat_err (sp, stat, iosb)
stream_t *sp;
int stat;
lat_iosb_t * iosb;
{
	char * latmsg, errbuf[80], msgbuf[256];

	dprintf ("Lat Error: stat= 0x%08X; iosb= 0x%04X 0x%04X 0x%08X\n",
		  stat, iosb->stat, iosb->latv1code, iosb->latv2code);
	latmsg= errbuf;

	if (stat==SS$_NORMAL)
	  if (iosb->stat==SS$_ABORT)
	     switch (iosb->latv2code) {
	     case	LAT$_DISCONNECTED: latmsg="Session disconnected"; break;
	     default: switch	(iosb->latv1code) {
		case 0:	latmsg= "Unknown reason"; break;
		case 2:	latmsg= "Shutdown in progress"; break;
		case 5: latmsg= "Insufficient server resources"; break;
		case 6: latmsg= "Port/service in use"; break;
		case 7: sprintf (errbuf, "No such service %s", sp->servicename); break;
		case 8: latmsg= "Service is disabled"; break;
		case 9: latmsg= "Service not offered on requested port"; break;
		case 10:latmsg= "Port name is unknown"; break;
		case 11:latmsg= "Incorrect password"; break;
		case 12:latmsg= "Entry not in queue"; break;
		case 13:latmsg= "Immediate access rejected"; break;
		case 14:latmsg= "Acces denied"; break;
		case 15:latmsg= "Corrupted request"; break;
		case 16:latmsg= "Requested function not supported"; break;
		case 17:latmsg= "Session cannot be started"; break;
		case 18:latmsg= "Queue entry deleted by server"; break;
		case 19:latmsg= "Illegal request parameters"; break;
		default:
			sprintf	(errbuf,"Illegal LAT rejection code %d",
				 iosb->latv1code);
	        }
	     }
	   else	if (iosb->stat==SS$_TIMEOUT)
			latmsg= "Server not available or unknown";
		else{
			sprintf (errbuf, "Unknown IOSB status: 0x%04X", 
					 iosb->stat);
			latmsg= errbuf;
		}
	else{
			sprintf (errbuf, "Unknown connect QIO status: 0x%X", stat);
			latmsg= errbuf;
		}
	dprintf ("Lat error reason: %s", latmsg);
	checkstat (stat," Lat connect abort msg");

	if istrue (stream.stalled) {
		if (stream.stallerr_count++ < stream.stallmsg_count) return;
		/* message time ! */
		if (stream.stallmsg_count*2*STALL_MINS < 60)
			stream.stallmsg_count *= 2;
		else	stream.stallmsg_count = 60/STALL_MINS;
		stream.stallerr_count= 0;
	}

	sprintf (msgbuf, 
"Error connecting LAT device %s for queue %s\r\n-%s", stream.dev, stream.queue,
				latmsg);
	sendmess (msgbuf, MSG_OPER);

}

/*	Connect/Disconnect the laserwriter	*/
static	void ctrlyast();
static
boolean
connect_laserwriter (sp, on)
stream_t * sp;
boolean on;
{
	lat_iosb_t iosb;
static	long	devtype, devclass, devchar;
static	struct	{uint16 count, type;
		 ptr32 addr, retlen;}
		itmlst[]=
		{{sizeof devtype, DVI$_DEVTYPE | DVI$M_SECONDARY, &devtype, 0},
		 {sizeof devclass, DVI$_DEVCLASS | DVI$M_SECONDARY, &devclass, 0},
		 {0, 0, 0, 0}};
	boolean	con_ok;
	int	ast_st;
	int	stat;
	int	SYS$GETDVIW();

	stat= SYS$GETDVIW (0, sp->chan, 0, &itmlst, 0, 0, 0, 0);

	dprintf	("Stat: 0x%08x; Class: %d; type: %d", stat, devclass, devtype);

	con_ok= true;	/* si succes soit pas d'AST, soit iosb status OK */

	PROT_START("connect_laserwriter")	/* protect against ^Y AST */

	if istrue(on) stream.lw_online= true;	/* assume success */

	print_astcnt("avant connect");

	stat= SYS$QIOW	(laser_efn, sp->chan,
			 IO$_TTY_PORT | (on ? IO$M_LT_CONNECT: IO$M_LT_DISCON),
			 &iosb, 0, 0,
			 0, 0, 0, 0, 0, 0);
	dprintf ("Connect LT %s: stat=%08x iosb= 0x%04X 0x%04X 0x%08X\n",
		 istrue (on)? "On" : "Off", stat, iosb.stat, iosb.latv1code,
							     iosb.latv2code);

	print_astcnt("apres connect");

	if (on) 
		switch	(stat) {
		case	SS$_ILLIOFUNC: 			/* pas un port LAT */
		case	SS$_DEVACTIVE:	break;		/* deja connecte */
		case	SS$_NORMAL:
			if (iosb.stat==SS$_NORMAL){
				if (sp->printernode[0]==0) getlatinfo();
				stat= SYS$QIO (0, sp->chan,
						IO$_SETMODE|IO$M_CTRLYAST, 0,
						0, 0,
						ctrlyast, 0, 0, 0, 0, 0);
				print_astcnt("apres connect ^YAST");
				break;
			}
		default:	lat_err (sp, stat, &iosb);
				con_ok= false;
		}

	if istrue (con_ok)
		if istrue (on){
			postlaserread (sp, 1);	/* post read  & purge */
			sp->devsts.smbmsg$v_unavailable= false;
		}
		else {
			cancelread (sp);
			sp->lw_online = false;
		}
		

	print_astcnt("fin connect");

	PROT_END("connect_laserwriter")

	return	con_ok;
}

/*	this routine can be triggered by connect lt on	*/
static
void
ctrlyast()
{
	dprintf ("Control-Y");		/* par la boucle d'attente du symbiont*/
	SET_STOP_REASON (SS$_HANGUP);
	stream.lw_online= false;
	stream.product[0]= '\0';
	(void) connect_laserwriter(&stream,false);	/* bug SS$_DEVACTIVE */
	wake_me();			/* un wake surnuméraire sera avalé */
}
	/*****	End of laserwriter I/O routines GG	*****/


/**	set_typeahead.
*
*	Set /typeahead on the terminal device so characters will be
*	buffered.
*/
static int
set_typeahead(chan)
int chan;					/* laserwriter channel */
{
	int mode[3];
	int stat;

	stat = SYS$QIOW(0, chan, IO$_SENSEMODE, 0, 0, 0,
		mode, sizeof mode, 0, 0, 0, 0);
	if (!(stat & 1)) return stat;
	
	mode[1] &= ~TT$M_NOTYPEAHD;		/* clear notypeahead */
	mode[1] |= TT$M_NOECHO;			/* no echo */
	mode[2] |= TT2$M_ALTYPEAHD;		/* alternate type ahead */
	mode[2] |= TT2$M_PASTHRU;		/* pass thru mode*/
	mode[2] |= TT2$M_XON;			/* resume output */
	stat = SYS$QIOW(0, chan, IO$_SETMODE, 0, 0, 0,	
		mode, sizeof mode, 0, 0, 0, 0);

	return stat;
}

/*
 *	Compta.
 *	we use print_status to ignore errors after user job.
 */
static void
finish_task (do_accounting)
boolean do_accounting;
{
static	smbmsg$r_accounting_data acctrec;		/* accounting record */
static	struct dsc$descriptor_s accounting =
	     {sizeof acctrec , DSC$K_DTYPE_T, DSC$K_CLASS_S, (char *) &acctrec};
	int stat, request;
	int print_status;

	if istrue (do_accounting) {
		int np;

		np = endpage - startpage;	 /* compute page count */
						 /* if no obvious error */
		if (np >= 0){
			acctrec.smbmsg$l_pages_printed= np;
			compta_update (endpage);
		}
		else{
			acctrec.smbmsg$l_pages_printed= 0;
			dprintf ("Invalid np value: startpage %d endpage %d",
				 startpage, endpage);
		}
	}


		/*  If stop_task, we were stopped by the job controller.*/
	if istrue(stream.stop_task)	request = SMBMSG$K_STOP_TASK;
		/*  else, ok or we were stopped by some other error */
	else				request = SMBMSG$K_TASK_COMPLETE;

	if (stream.print_status) print_status = stream.print_status;
	else 			 print_status= SS$_NORMAL;	

	/* le stream ne peut etre stalled que si il y a une tache active ... */
	/* bug queue manager V5.5-1 qui laisse la queue stalled */

	stream.devsts.smbmsg$v_stalled = false;

	stream.task_active= false;	/* task is no longer active */
	stream.stop_task=   false;	/* meaningless if no task active */

	send_request (&stream, request,
		      istrue (do_accounting) ? &accounting : 0, 
		      print_status,
		      "finish_job");
}

/*
 *	do_stall:	requeues current task, if any. enters stall mode
 */
static void
do_stall()
{
	int	stat;
	int	SYS$SCHDWK();

	stream.stalled= true;
	stream.stallmsg_count= 1;	/* not 0 !!! */
	stream.stallerr_count= 0;
	stat = SYS$SCHDWK(0, 0, stall_daytim, stall_daytim); /* wake up later */
	checkstat(stat, "schdwk stall");

	/* if a task is active, tell the job controller about the stall */

	if istrue (stream.task_active) {
		stream.devsts.smbmsg$v_stalled = true;
		send_request (&stream, SMBMSG$K_TASK_STATUS, 0, 0,
			     "do_stall");
	}
}

/*
 *	perform unstall operation
 *	we are called periodically with a timer and on any event
 *	with a task active or not
 *	the context is *NOT* AST
 *	this routine is therefore not reentrant
 */
static void
do_unstall(requeue)
boolean requeue;	/* requeue task */
{
	boolean stat;

	stat = connect_laserwriter (&stream, true);
	if istrue (stat) {		/* try to cause to the imprimante */

		getproduct();

		(void) getidle (true);		/* wait for idle state */

		stat= istrue (stream.lw_online);

		(void) connect_laserwriter (&stream, false);
		if istrue (stat) {
			int SYS$CANWAK();

			(void) SYS$CANWAK (0,0);

			/* if active task then requeue task, if allowed */

			if (istrue(stream.task_active) && istrue (requeue)
				&& isfalse(stream.stop_task)) {
				int	SYS$SNDJBCW();
static				struct 	{uint16 buflen, itmcod;
					 ptr32  bufadr, retadr;}
				jbclst [] = 
					{{0, SJC$_QUEUE, 0, 0},
					 {0, SJC$_REQUEUE, 0, 0},
					 {0, 0, 0, 0}};
				struct {uint32 status;
					uint32 whatever;} jbciosb;

				jbclst[0].buflen = strlen (stream.queue);
				jbclst[0].bufadr = &stream.queue;
				stat= SYS$SNDJBCW (gp_efn, SJC$_ABORT_JOB, 0, 
						   &jbclst, &jbciosb, 0, 0);

				if istrue (stat) stat= jbciosb.status;
	
				/* we were stopped in the meanwhile */
				/* ignore $SNDJBC	status	*/

				if isfalse(stream.stop_task)
					checkstat (stat, "$SNDJBCW abort job");
			}

			stream.stalled= false;
		}
	}
}

/*
 *	releaseitems - release item list.
 *
 *	release dynamic strings gotten by read_items.
 */
releaseitems()
{
	int	i;
static	struct dsc$descriptor_d temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
	
	itmax = 0;					 /* clear item index */

	for (i = 0; i < MAXITEMS; i++) {
		if (item[i].buffer == 0) return;	/* if end of list */
		temp.dsc$w_length = item[i].item_size;
		temp.dsc$a_pointer = item[i].buffer;
		STR$FREE1_DX (&temp);
		item[i].buffer = 0;
	}
}


/*
 *	sendendsequence - send end of job sequence.
 *			  cleans up after printing the user files.
 */
static
int
getendidle_t(time)			/* test synchronisation */
int	time;
{
static	const char eot= '\004';

	if istrue(stream.status_idle) return 1;		/* if sync occured */

	/* au bout de MAXBUSY intervalles, on casse tout */

	if (time > MAXBUSY){
		stream.lw_clear= true;
			/*  Abort whatever is running */
		laserwrite (&lw_abort, sizeof lw_abort);
		dprintf ("Getendidle: abort sent");
		return 1;		/* retry */
	}


	/*	sinon, au bout de 5 minutes on envoie des ^D */

	else	if (time > 10) laserwrite (&eot, 1);

	return 0;		       /* on reste dans keyin sans changement */
}

static boolean
getendidle()
{
static	const	char	syncchar='\024';		/* Control-T */
static	const	char	QMSquest[]= 
	"(\\n%%[ status: idle ]%%] QMSBUG\\n) print flush\004";
	int	kstat;

	stream.status_idle = false;		/* par défaut: pas IDLE */
	stream.lw_sync = true;

	dprintf ("Getendidle >>> %s", to_date(0));

	/* on envoie ^T pour avoir le status, et on attend le résultat */

	do {
		if istrue(stream.QMSbug)
			kstat= keyin (QMSquest, sizeof QMSquest-1,
				      getendidle_t, true);
		else	kstat= keyin (&syncchar, 1, 
				      getendidle_t, true);
		/* loop until idle or lw offline */
	} while (  isfalse(stream.status_idle) && (kstat>=0) );

	stream.lw_sync = false;

	dprintf ("Getendidle <<< %s", to_date(0));
	return stream.status_idle;
}

static int
sendend_t(timeoutcount)
int	timeoutcount;
{
static	boolean	abort_sent;

	if (endpage) return 1;			/* Ok, got page count */

	if (timeoutcount == 0) abort_sent= false;	/* 1er passage */

	if isfalse (stream.lw_online) return -1;	/* lw deconnectée */

	/* if job deleted or LW not responding, cancel the print */

	if ( istrue(stream.stop_task) || (timeoutcount > MAXBUSY) ) {
		if isfalse(abort_sent) {
			dprintf ("Sendend: sending ABORT");
			abort_sent = true;
			laserwrite (lw_abort, sizeof lw_abort);	/* ^C ^D */
		}
	}

	/*  Notify the operator on 10 minute intervals. */
		
	if ( (timeoutcount+1 %20) == 0) {
		sendmess ("LaserWriter job possibly hung", MSG_OPER);
	}

	/* ask question every 2 minutes */

	return (((timeoutcount+1) %4) == 0) ? 0 : -1;
}

static void
sendendsequence()
{
static const char getend[] = 
{"\004statusdict begin (%%[ end page   #=) print \
pagecount pstack pop flush end \r\n\004"};

	endpage = 0;
	dprintf (">>> Sendend %s", to_date(0));

	(void) getendidle();		/* for QMS 1700 */

	if ((stream.lw_flushcnt!=0)&&(stream.lw_flushcnt <= stream.lw_eotcnt)){
		dprintf ("Unflushing the job...");
		PROT_START("sendendsequence")	/* stop_reason && stop_task */
						/* must be consistent */
		if isfalse (stream.stop_task) stream.stop_reason= 0;
		PROT_END("sendendsequence")
	}

	(void) keyin (getend, sizeof getend-1, sendend_t, true);

	if istrue(stream.QMSbug) {
		int savend;
		do {
			savend= endpage;
			sleep(10);

			(void) keyin (getend, sizeof getend-1, sendend_t, true);
		} while (savend!=endpage);
	}

	dprintf ("<<< Sendend %s", to_date(0));
}

/*
 *	sendfile - send the main file to the printer.
 */

static
void
sendfile()
{
	int stat, end;
	int i;
	int line = 0;
	boolean undefined = 
		main_fab.fab$b_rfm==FAB$C_UDF;	/* undefined record format */
	boolean print;				/* fichier: PRN format */
	char ibuf[IBUFSIZE+1];			/* input buffer */
	char obuf[2+((IBUFSIZE*4+7) & -8)];	/* output buffer */
	unsigned long	start_time[2],		/* DEBUG/STATS */
			end_time[2],
			delta_time[2],
			duree,
			delta_bytes;
	int	SYS$GETTIM();

	struct	{unsigned char prefix,postfix;}
					rhb;	/* record header buffer (GG) */

	dprintf ("File size is %d blocks", main_fab.fab$l_alq);

	SYS$GETTIM (start_time);
	stream.bcnt= 0;

	if ((main_fab.fab$b_fsz==sizeof rhb)
	    && main_fab.fab$v_prn) {		  /* print cobol (UGH) */
		main_rab.rab$l_rhb= (char *)&rhb; /* address of record header */
		print= true;
	}
	else {
		main_rab.rab$l_rhb= 0;
		rhb.prefix= 0;
		rhb.postfix= 0;
		print= false;
	}

	main_rab.rab$l_ubf = ibuf;
	main_rab.rab$w_usz = IBUFSIZE;
	stream.lw_flushcnt= 0;			/* reset flush counter */
	stream.lw_eotcnt= 0;			/* reset ^D counter */
	stream.logfile_write= false;		/* don't write to logfile */
			/* send the file */

	do {
		char *cp;			/* pointeur record courant*/
		int rsz;			/* record size */
		
		stat = SYS$GET(&main_rab, 0, 0);	/* get a record */

		line ++;

		if ((stat & 1)==0) {
			dprintf ("SYS$GET returns %08X at line %d (%d bytes)",
				 stat, line, main_rab.rab$w_rsz);
			if (stat == RMS$_EOF) break;
			else	SET_STOP_REASON (stat);	/* erreur RMS */
		}

		if isfalse(stream.lw_online) break;	/* low level LZW error*/

		if istrue (stream.stop_task) break;	/* job killed by user */

	/* postscript text may contain embedded ^D */
	/* so, "flushing" may not be an error	   */

		if (stream.stop_reason) {
			if (stream.job_type==print_headers) break;
			if (stream.lw_flushcnt==0) break;
			if (stream.lw_flushcnt>MAXFLUSH) {
				stream.lw_flushcnt= 0x40000000; /* kludge */
				break; /* too much flushes */
			}
		}

		rsz = main_rab.rab$w_rsz;

		if (stream.job_type!=print_headers) {	/* count ^D's */
			ibuf[rsz]= '\0';
			if (strchr (ibuf, '\004') != NULL) stream.lw_eotcnt++;
		}

		if istrue(undefined) {	/* undefined format: just send bytes */
			dprintf ("UDF: read %d bytes", rsz); 
			laserwrite (ibuf, rsz);
			continue;
		}
		
/*  process record control */
		
		cp = ibuf;
		
		/* on retire les caracteres superflus	*/
		/* colonne 1 si RAT == FORTRAN		*/
		/* si rat==cr alors on retire les \r et \n terminaux */

		if isfalse(stream.hexdump) {
			if istrue(print) { 
				if (stream.debug & DBG_VFC)
				dprintf ("VFC: prefix 0x%X postfix 0x%X",
					 (int) rhb.prefix, (int) rhb.postfix);

				/* sequence normale: prefix = 1 LF */
				if ((rhb.prefix&0x80) == 0) /* nb line-feed */
					for (i=1; i++<rhb.prefix;)
						laserwrite (&lw_crlf,
							    sizeof lw_crlf);
				else {	/* cross fingers & output char */
					unsigned char c= rhb.prefix &0x7f;
					laserwrite (&c,1);
				}

				/* sequence normale: postfix = CR */
				if ( (rhb.postfix&0x80) )
						if (rsz< sizeof ibuf)
					cp[rsz++]= rhb.postfix & 0x7f;	
			}

			if (rsz) {		/* remove possible col 1 */
				if (main_fab.fab$b_rat & FAB$M_FTN) {
					static const char ff='\f';
					char control= *cp++;/* ignore column 1 */
					switch (control) {
					case '0': laserwrite (&lw_crlf,
							      sizeof lw_crlf);
						  break;/* une ligne blanche */
					case '1': laserwrite (&ff,1); break;
					case '+':	/* je sais pas faire */
					case '$':	/* ca non plus */
					case ' ':	/* cas normal */
					default :;	/* on ne fait rien */
					}
					rsz--;
				}
			}

/*			if ((main_fab.fab$b_rat & FAB$M_CR) == 0) {
				if (rsz) if (cp[rsz-1] == '\n') rsz--;
				if (rsz) if (cp[rsz-1] == '\r') rsz--;
			}
*/
		}
			
		if isfalse(stream.hexdump){	/* if ascii (vs HEX) format */
			expand_ascii (cp, obuf, &rsz, sizeof obuf-2);
			if (rsz == sizeof obuf-2 ) 
				checkstat (LASER$_TOOMANYTABS, "sendfile");

			if (main_fab.fab$b_rat & FAB$M_CR) {
				obuf[rsz++] = '\r';
				obuf[rsz++] = '\n';
			}
		}
		else {				/* if file to be hexified */
			expand_hex(cp, obuf, &rsz);
		}

		/* si on est en print avec RHB.prefix==0 il faut faire */
		/* de la surimpression. On sait pas faire, donc: poubelle */

		if (isfalse(print) || (rhb.prefix!=0)) laserwrite(obuf,rsz);

		if istrue(print) {
			if ((rhb.postfix&0x80) == 0)
				for (i=0; i++<rhb.postfix;)
					laserwrite (&lw_crlf,
						    sizeof lw_crlf);
				
		}

	} while (true);

	if (stream.stop_reason==0)	  /* if the print was OK, but the */
		stream.empty_file= stream.bcnt == 0;	/* file was empty */

	if istrue(stream.hexdump) {
		laserwrite(lw_crlf, sizeof lw_crlf);
	}

/*  send abort (^C) if stop_task, else eof (^D) */

	if istrue (stream.stop_task) end = 3; else end = 4;
	laserwrite(&end,1);

	/* statistiques */

	{
static const unsigned long libdtop= LIB$K_DELTA_SECONDS;
		int	LIB$SUB_TIMES();
		int	LIB$CVT_FROM_INTERNAL_TIME();

		SYS$GETTIM (end_time);
		delta_bytes= stream.bcnt;
		stat= LIB$SUB_TIMES (end_time, start_time, delta_time);
		checkstat (stat, "sub times");
		stat= LIB$CVT_FROM_INTERNAL_TIME (&libdtop,
						  &duree, delta_time);
		checkstat (stat, "LIB$CVTIME");
		if (duree==0) dprintf ("Durée nulle (%d bytes)??", delta_bytes);
		else 	dprintf ("Envoi de %d bytes en %d secondes (%d bit/s)", 
					delta_bytes, duree, 
					(10*delta_bytes) / duree);
	
	}
}

/*
 *	processtask - process a file.
 */
static
void
processtask()
{
static 	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
	int stat;
	boolean	print_ok;	/* temoin pas de rupture de ligne */
	boolean devunav;
	int	SYS$GETTIM();

	/* on verifie qu'on a notre periph */
	
	if (stream.chan == 0) checkstat (LASER$_STREAMNOTSTART, "processtask");

	/* on previent JOBCTL qu'on a recu la requete */

	send_request (&stream, SMBMSG$K_START_TASK, 0, 0, "processtask start");

	get_debug (stream.queue);

	stream.job_type= print_unknown;
	stream.empty_file= false;
	devunav= stream.devsts.smbmsg$v_unavailable;

	/* the lat port may already be connected if several files in the job */

	if (istrue(stream.lw_online) || 
	    istrue(connect_laserwriter (&stream, true))) {/* connect LT port (GG) */

		/* if the unavailable device was set, tell QUEUE MANAGER */
		/* the device is available (prettier in 'show queue') */

	   if (istrue (devunav) && isfalse (stream.devsts.smbmsg$v_unavailable))
		send_request (&stream, SMBMSG$K_TASK_STATUS, 0, 0,
			      "Task status");

		print_astcnt("apres connect laser");
		
	   getitemdescrip(SMBMSG$K_SEPARATION_CONTROL, &temp, true);
	   stream.sepctl.smbmsg$l_separation_flags = * (long*) temp.dsc$a_pointer;
	   dprintf	("ProcessTask: separation control= 0x%X\n",
			stream.sepctl.smbmsg$l_separation_flags);

	   SYS$GETTIM (&stream.task_start);	/* sauvegarde date debut */

	   if istrue (syncprinter()) {		/* synchronize with printer */

	     if istrue (init_lw()) {

  		    print_astcnt("debut impression");

		   if (stream.sepctl.smbmsg$v_job_flag) doflag(true);
	
		   checkspooled();		/* check for spooled file */

		   if (openfile()) {		/* open main file */

		   /*	getcopies();	*/	/* send copy count */
		   /*	getnote();	*/	/* output possible note */

			if (stream.sepctl.smbmsg$v_file_flag) doflag(false);

			stream.user_job_active = true;

			getparams();		/* get user parameters */

			if (istrue (stream.lw_online) &&/* if Laserwriter still there */
			    istrue(getsetup()))	/* output setup */
				sendfile();	/* send main file if setup ok */

			closefile();		/* close main file */
		   }	/* opened file */
		}	/* init_lw */

		sendendsequence();		/* send end job sequence */

		/* print is Ok if lw is online OR if lw is offline, but
		   after the job has finished */

		print_ok= istrue(stream.lw_online) || (stream.stop_reason == 0);

	     }	/* synchronized with printer */

	   else { /* could not synch with printer: hardware pb or STOP_TASK */
		if isfalse(stream.stop_task) print_ok= false;
	   }

	   stream.print_status= stream.stop_reason;

	   stream.user_job_active = false;

	   if istrue(print_ok) dotrailer();	/* send trailer */

	   if (stream.sepctl.smbmsg$v_last_file_of_job)
		   connect_laserwriter (&stream,false);
	}

	else	{	/* could not connect to laserwriter */
		SET_STOP_REASON (LASER$_STREAMNOTSTART);
		stream.print_status= LASER$_STREAMNOTSTART;
		print_ok = false;
	}

	releaseitems();				/* release item list */

	if (stream.logfile_lines) {		/* if errors logged */
		SET_STOP_REASON (SS$_CREATED-1);
		closeuserlog();			/* close user log */
	}

		/* if stream stalled, loop until task is aborted or */
			/* stream is unstalled */

	if isfalse (print_ok) {
		do_stall();
		while (istrue (stream.stalled) && isfalse (stream.stop_task)) {
			hibernate();
			do_unstall(true);
			get_debug (stream.queue);
		}
	}

		/* on envoie l'accounting a JOBCTL */

	finish_task (print_ok);

	get_debug (stream.queue);
}

/*
 *	resetstream - reset the stream. (AST context)
 *	emergency symbiont shutdown
 */
static
void
resetstream()
{
	void SYS$EXIT();

	send_request (&stream, SMBMSG$K_RESET_STREAM, 0, 0, "Reset stream");
	
	SYS$EXIT (SS$_NORMAL);
}

/*
 *	verification que le terminal est bien la. Contexte: non AST
 */

static
void
check_device()
{
	boolean stat;

	stat= connect_laserwriter (&stream, true);
	stream.devsts.smbmsg$v_unavailable= isfalse(stat);

	if istrue(stat) { /* check device (GG) */
		getproduct();
		(void) syncprinter();	/* accounting: get page counter */
		(void) connect_laserwriter (&stream, false);
	}
	else	do_stall ();
}

/*
 *	startstream - start the stream.	-- called at AST level !
 */
static
void
startstream()
{
static 	const	msg_size= 80, msg_bytlm=256;
static	struct dsc$descriptor_s device = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
static	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
	int	stat;
	int	startstatus;
	int	SYS$ALLOC();
	int	SYS$CREMBX();

	/* reset all values to known values (not useful now, since static=0) */
	/* remember false = 0 => all boolean values are false*/

	memset (&stream, 0, sizeof stream);
assert ((int)false==0);		/* all values are false by default */

		/*	get device port name	*/

	getitemdescrip(SMBMSG$K_DEVICE_NAME, &device, true);
	dprintf ("Start stream: device= %.*s\n", device.dsc$w_length,
						device.dsc$a_pointer);
	sprintf (stream.dev, "%.*s",
				min(device.dsc$w_length, sizeof stream.dev-1),
				device.dsc$a_pointer);

		/*	get queue name */

	getitemdescrip(SMBMSG$K_EXECUTOR_QUEUE, &temp, true);
	dprintf ("Start stream: queue= %.*s\n", temp.dsc$w_length,
					temp.dsc$a_pointer);
	sprintf (stream.queue, "%.*s",
				min(temp.dsc$w_length, sizeof stream.queue-1),
				temp.dsc$a_pointer);
	if (debug) {
		char qname[255];
		get_debug (stream.queue);
		dbg_close();
		if (stream.debug!=0) dbg_init(stream.queue);
	}

	get_debug (stream.queue);

		/*  Copy the library file specification, if it exists */

	if (getitemdescrip(SMBMSG$K_LIBRARY_SPECIFICATION, &temp, false))
		STR$COPY_DX (&library_spec, &temp);
	dprintf ("Start stream: library= %.*s\n", temp.dsc$w_length,
						  temp.dsc$a_pointer);

		/* allocate device */

	stat = SYS$ALLOC (&device, 0, 0, 0, 0);
	dprintf ("Start stream: alloc returns 0x%X\n", stat);

		/* associate control mbx */

	if (stat & 1) {

static		const $DESCRIPTOR (mbx_nam,"Laser$ctlmbx");

		stat = SYS$CREMBX (0, &mbxchan, msg_size, msg_bytlm, 
					 0, 0, &mbx_nam);
		dprintf ("Crembx: stat= 0x%X", stat);
		if (stat&1) stat = SYS$ASSIGN (&device, &stream.chan,
						   0, &mbx_nam);
		dprintf ("Start stream: asn wth mbx returns 0x%X\n", stat);
		compta_init (stream.dev);
	}

	startstatus= stat;
	stream.devsts.smbmsg$l_device_flags= 0;
	stream.devsts.smbmsg$v_lowercase= 1;
	stream.devsts.smbmsg$v_terminal= 1;

	if (stat&1) {
		postmbxattn();

		stat= set_typeahead(stream.chan);  /* set terminal typeahead */

		dprintf ("Start stream: set typeahead returns 0x%X\n", stat);

		stream.devsts.smbmsg$v_stop_stream= 0;
	}

	send_request (&stream, SMBMSG$K_START_STREAM, 0, startstatus, 
					"startstream: send to jobctl");

	stream.paused = false;
	stream.user_job_active = false;
	stream.stalled = false;
	stream.debug= 0;


	if istrue(startstatus){		/* si le périph est utilisable */
		stream.check_device= true;
		get_config (&stream);		/* get stream config */
	}

}


/* 
 *	stopstream - stop the stream. -- called at AST level !!
 */
static void
stopstream()
{
	int	stat;
	void	SYS$EXIT();

	if (stream.chan!=0) {

		stat= SYS$DASSGN (mbxchan);
		checkstat(stat, "stopstream: dassgn mbx");

		(void) connect_laserwriter (&stream, false);

		stat = SYS$DASSGN(stream.chan);
		checkstat(stat, "stopstream: dassgn term");
		stream.chan = 0;
	}

	send_request (&stream, SMBMSG$K_STOP_STREAM, 0, 0,
				"stopstream: send to jobctl");
	
	SYS$EXIT (SS$_NORMAL);
}

/* 
 *	jobctl_ast - ast routine called by the job controller.
 */
static
void
jobctl_ast()
{	
	int	stat;
	int	request;			/* jobctl request */
	int	stream_id;
	int	SMB$READ_MESSAGE();

	struct dsc$descriptor_s temp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

	stat = SMB$READ_MESSAGE (&stream_id, &buffer_des, &request);
	/* stream_id should be 0 */
	checkstat (stat, "SMB$READ_MESSAGE");

/*  Process the request */

	dprintf ("JOBCTL AST: request=%d\n", request);

	switch (request) {

		case SMBMSG$K_START_TASK:
		print_efs();
		getitems();
		print_efs();
		stream.task_active= true;	/* a task is now active */
		stream.stop_task = false;	/* and has not been killed */
		stream.stop_reason =   0;
		wake_me();
		print_efs();
		break;

		case SMBMSG$K_STOP_TASK:
		getitems();
		getitemdescrip(SMBMSG$K_STOP_CONDITION, &temp, true);
		SET_STOP_REASON (* (long*) temp.dsc$a_pointer);
		stream.stop_task = true;
		if istrue(stream.paused) wake_me();
		break;

		case SMBMSG$K_START_STREAM:
		getitems();
		startstream();
		break;

		case SMBMSG$K_PAUSE_TASK:
		stream.paused= true;
		send_request(&stream, SMBMSG$K_PAUSE_TASK, 0, 0, "Pause");
		break;

		case SMBMSG$K_RESUME_TASK:
		getitems();
		wake_me();
		stream.paused= false;
		send_request(&stream, SMBMSG$K_RESUME_TASK, 0, 0, "Resume");
		break;

		case SMBMSG$K_STOP_STREAM:
		getitems();
		stopstream();
		break;

		case SMBMSG$K_RESET_STREAM:
		resetstream();
		break;
	}
}

static
getsysinfo()
{
static	unsigned short	nodelen, vmsverslen;
static 	const	struct {uint16 itmsiz, itmcod;
			ptr32  retval, retlen;}
			syilist[]= {
		{sizeof	smb_nodename-1, SYI$_NODENAME,&smb_nodename, &nodelen},
		{sizeof smb_vmsvers, SYI$_VERSION, &smb_vmsvers, &vmsverslen},
		{sizeof smb_maxbuf, SYI$_MAXBUF, &smb_maxbuf, 0},
		{0,0,0,0}};
	unsigned long stat;
	int	SYS$GETSYIW();

	stat= SYS$GETSYIW (0, 0, 0, &syilist, 0, 0, 0);
	dprintf ("MAXBUF = %lu", smb_maxbuf);

	checkstat (stat, "$GETSYIW");
	smb_nodename[nodelen]= 0;
	smb_vmsvers [vmsverslen]= 0;
	strcpy (stream.printernode, smb_nodename);
}
/*
 *	setprivs - set process privs.
 *	GG rajout TMPMBX (pour mbx surveillance)
 *		  OPER,WORLD (pour $BRKTHRUW)
 */
static void
setprivs()
{
	int stat;
static	const	privs[2] = {
  PRV$M_ALLSPOOL|PRV$M_PHY_IO|PRV$M_SYSPRV|PRV$M_TMPMBX|PRV$M_OPER|PRV$M_WORLD, 
		0};
	int	SYS$SETPRV();

	stat = SYS$SETPRV(1, privs, 0, 0);
	checkstat(stat, "set privs");
}
/* 
 *	initsymb - initialize printer symbiont.
 *	only 1 thread	GG
 */
initsymb()
{
static	const	initreq= SMBMSG$K_STRUCTURE_LEVEL;
	int	stat;
	int	SMB$INITIALIZE();

	stat = SMB$INITIALIZE (&initreq, jobctl_ast, 0);
	checkstat (stat, "initsymb");
}
/*
 *	preset - preset the symbiont.
 */
static void
preset()
{
	int stat;
	int SYS$BINTIM(),LIB$GET_EF();

	assert	( sizeof (int16) == 2);
	assert	( sizeof (uint16) == 2);
	assert	( sizeof (int32) == 4);
	assert	( sizeof (uint32) == 4);
	assert	( sizeof (ptr32) == 4);

	/* convert duration information into VMS internal format */

	stat = SYS$BINTIM(&keyin_interval, keyin_daytim);
	checkstat(stat, "bintim keyin");
	stat = SYS$BINTIM(&stall_interval, stall_daytim);
	checkstat(stat, "bintim stall");

	stat = LIB$GET_EF(&laser_efn);	/* allocate event flag */
	checkstat (stat, "Allocate Laser efn");
	stat = LIB$GET_EF(&gp_efn);	/* allocate event flag */
	checkstat (stat, "Allocate GP efn");
	
	setprivs();			/* set process privs */
	inititems();			/* init item list */
	getsysinfo();			/* collect sys. info. GG */
	initsymb();			/* do jobctl init */
}

/*
 *	validate a printer: test LAT connection, and printer type
 */
static void
validate_stream(sp)
stream_t * sp;
{
	latmaster_init (&stream);	/* init LAT device if any */
	check_device();			/* check connected printer */
	sp->check_device= false;	/* check done */
}

/*
 *	**************** Main program *******************
 */
main()
{
	int	stat;

	dbg_init(NULL);
	
	preset();				/* initialize symbiont */
	
	do {

		while (isfalse (stream.task_active) || istrue (stream.stalled)){
			hibernate();

	/* don't requeue an active task because no work has been done yet */

			if istrue (stream.stalled) do_unstall(false);
			if istrue(stream.check_device) validate_stream(&stream);
		}

	/* invariant: stream.stalled= false */

		print_astcnt("avant processtask");
		processtask();			/* do a job */
		print_astcnt("apres processtask");

	} while (true);
}
