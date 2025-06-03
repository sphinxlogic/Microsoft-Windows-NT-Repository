/*
**    rms_stats.C    This program can be used to display RMS File Statistics.
** 
** It requires CMEXEC and target file access priviliges to work.
** It provides an alternative to ANAL/SYSTEM ... SHOW PROC/RMS=FSB
** which is only available since OpenVMS V6.1
** Contrary to MONITOR RMS it displays raw numbers, not rates per second.
** If a repeat time is selected, then the display can be absolute numbers 
** or incremental number for that period chosen.
** The terminal handling is admitted rather limited and should be improved.
** Use @list.com to provide a list of (wildcarded) files as input.
** Usage: Define foreign command to point to executable and pass options
** on the command line. For example:
**   $FSB = "$some_devive:[some_directory]FSB.EXE "
**   $FSB	                ! Outputs minimal help informantion.
**   $FSB file.ext              ! Outputs single shot file stats summary line.
**   $FSB -o=a -r=5 -i file.ext ! All numbers, incremental, every 5 seconds.
** 
** Have fun, Hein van den Heuvel, Digital, NOV-1994
*/
#include signal
#include descrip
#include prvdef
#include dvidef
#include psldef
#include secdef
#include jpidef
#include iodef
#include ssdef
#include rms
#include stdio
#include signal
#include stdlib
#include string
#define DFMBC 535   /* GETJPI item only available on VMS V6.1 and later */
#define MAX_DEVLOCKNAM 16
#define FSB_NAMLEN 36
#ifndef XAB$C_ITM
struct XABITM { char xab$b_cod, xab$b_bln, fill1[2],
	 *xab$l_nxt, *xab$l_itemlist, xab$b_mode, fill2[19] ;} ;
#define XAB$_STAT_ENABLE 64
#define XAB$C_ITM 36
#define XAB$C_ITMLEN 32
#endif

typedef struct { short len, cod; void *address; int *retlen; } item;
typedef struct { long count; void *address; } desc;
#pragma nostandard /* variant struct */
typedef union {
	    int all;
	    variant_struct {
		unsigned summary_stats : 1;	
		unsigned file_stats : 1;
		unsigned locks_stats : 1;
		unsigned record_stats : 1;
		unsigned cache_stats : 1; } option_bits;
		} options_type;

#pragma standard 

int	sys$close(), sys$open(), sys$parse(), sys$search(),
	sys$connect(), sys$get(), sys$assign(), sys$qiow(),
	sys$getjpiw(), sys$getlkiw(), sys$getdvi(), ots$move3(),
	sys$setprv(), sys$fao(), sys$crmpsc(), sys$cmexec(), lib$wait();

/*  Private variant on module FSB from SYS$LIB_C.TLB
    Simplified File statistics block (fsb) definitions.
    the QUAD fields (FSB$Q_) are defined as two INTs to 
    avoid  strugling with alignment issue on Alpha 
*/
typedef struct _fsb {
    int fsb$l_fill_hein[3];
    unsigned int fsb$l_seqgets;         /* # of sequential gets to file     */
    unsigned int fsb$l_keygets;         /* # of keyed gets to file          */
    unsigned int fsb$l_rfagets;         /* # of gets by RFA to file         */
    unsigned int fsb$q_getbytes;    /* Total size of all gets in bytes  */
    unsigned int fsb$q_getbytes2;    /* Total size of all gets in bytes  */
    unsigned int fsb$l_seqputs;         /* # of sequential puts             */
    unsigned int fsb$l_keyputs;         /* # of puts by key                 */
    unsigned int fsb$q_putbytes;    /* Total size of all puts in bytes  */
    unsigned int fsb$q_putbytes2;    /* Total size of all puts in bytes  */
    unsigned int fsb$l_updates;         /* # of updates                     */
    unsigned int fsb$q_updatebytes; /* Total size of all updates in bytes */
    unsigned int fsb$q_updatebytes2; /* Total size of all updates in bytes */
    unsigned int fsb$l_deletes;         /* # of deletes                     */
    unsigned int fsb$l_truncates;       /* # of truncates                   */
    unsigned int fsb$l_truncblks;       /* Total size in blocks of all truncates */
    unsigned int fsb$l_seqfinds;        /* # of sequential finds            */
    unsigned int fsb$l_keyfinds;        /* # of keyed finds                 */
    unsigned int fsb$l_rfafinds;        /* # of finds by RFA                */
    unsigned int fsb$l_reads;           /* # of $READs to file              */
    unsigned int fsb$q_readbytes;   /* # of bytes of all $READs         */
    unsigned int fsb$q_readbytes2;   /* # of bytes of all $READs         */
    unsigned int fsb$l_connects;        /* # of connects to this file       */
    unsigned int fsb$l_disconnects;     /* # of disconnects from file       */
    unsigned int fsb$l_extends;         /* # of extends of file             */
    unsigned int fsb$l_extblocks;       /* # of blocks file has been extended */
    unsigned int fsb$l_flushes;         /* # of flushes of file             */
    unsigned int fsb$l_rewinds;         /* # of rewinds of file             */
    unsigned int fsb$l_writes;          /* # of $WRITEs to file             */
    unsigned int fsb$q_writebytes;  /* # of bytes of all $WRITEs to file */
    unsigned int fsb$q_writebytes2;  /* # of bytes of all $WRITEs to file */
    unsigned int fsb$l_flckenqs;        /* # of file lock ENQ's             */
    unsigned int fsb$l_flckdeqs;        /* # of file lock DEQ's             */
    unsigned int fsb$l_flckcnvs;        /* # of file lock conversions       */
    unsigned int fsb$l_lblckenqs;       /* # of local buffer lock ENQ's     */
    unsigned int fsb$l_lblckdeqs;       /* # of local buffer lock DEQ's     */
    unsigned int fsb$l_lblckcnvs;       /* # of local buffer lock conversions */
    unsigned int fsb$l_gblckenqs;       /* # of global buffer lock ENQ's    */
    unsigned int fsb$l_gblckdeqs;       /* # of global buffer lock DEQ's    */
    unsigned int fsb$l_gblckcnvs;       /* # of global buffer lock conversions */
    unsigned int fsb$l_gslckenqs;       /* # of global section lock ENQ's   */
    unsigned int fsb$l_gslckdeqs;       /* # of global section lock DEQ's   */
    unsigned int fsb$l_gslckcnvs;       /* # of global section lock conversions */
    unsigned int fsb$l_rlckenqs;        /* # of record lock ENQ's           */
    unsigned int fsb$l_rlckdeqs;        /* # of record lock DEQ's           */
    unsigned int fsb$l_rlckcnvs;        /* # of record lock conversions     */
    unsigned int fsb$l_applckenqs;      /* # of append lock ENQ's           */
    unsigned int fsb$l_applckdeqs;      /* # of append lock DEQ's           */
    unsigned int fsb$l_applckcnvs;      /* # of append lock conversions     */
    unsigned int fsb$l_flblkasts;       /* # of file lock blocking ASTs queued */
    unsigned int fsb$l_lblblkasts;      /* # of local buffer lock blocking ASTs queued */
    unsigned int fsb$l_gblblkasts;      /* # of global buffer lock blocking ASTs queued */
    unsigned int fsb$l_appblkasts;      /* # of shared append lock blocking ASTs queued */
    unsigned int fsb$l_lcachehits;      /* # of cache hits on local buffers */
    unsigned int fsb$l_lcache_attempts; /* # of attempts to use the local buffer cache */
    unsigned int fsb$l_gcachehits;      /* # of cache hits on global buffers */
    unsigned int fsb$l_gcache_attempts; /* # of attempts to use the global buffer cache */
    unsigned int fsb$l_gbrdirios;       /* # of direct io's due to global buffer reads */
    unsigned int fsb$l_gbwdirios;       /* # of direct io's due to global buffer writes */
    unsigned int fsb$l_lbrdirios;       /* # of direct io's due to local buffer reads */
    unsigned int fsb$l_lbwdirios;       /* # of direct io's due to local buffer writes */
    unsigned int fsb$l_bktsplt;         /* # of 2 bucket splits             */
    unsigned int fsb$l_mbktsplt;        /* # of multi-bucket splits         */
    unsigned int fsb$l_opens;           /* # of times the file is opened    */
    unsigned int fsb$l_closes;          /* # of times the file is closed    */
    unsigned int fsb$l_gsblkasts;       /* # of global section lock blocking ASTs queued */
    unsigned int fsb$l_xqpqios;         /* Count of XQP QIOs requested by RMS */
    unsigned int fsb$l_flwaits;         /* # of waits forced by getting the file lock */
    unsigned int fsb$l_lbwaits;         /* # of waits forced by getting local buffer locks */
    unsigned int fsb$l_gbwaits;         /* # of waits forced by getting global buffer locks */
    unsigned int fsb$l_gswaits;         /* # of waits forced by getting the global section lock */
    unsigned int fsb$l_rlwaits;         /* # of waits forced by getting record locks */
    unsigned int fsb$l_apwaits;         /* # of waits forced by getting the Append lock */
    unsigned int fsb$l_totwaits;        /* Total # of waits (or stalls) performed by RMS.  */
    unsigned int fsb$l_outbufquo;       /* Number of times a process runs out of global buffer quota */
					/* fsb$l_rmsdev fields removed */
    } FSB;

typedef struct file_control_block {
	struct file_control_block *next_file;		
	int new_buffer;
	int nok;
	FSB *exec_mode_fsb;
	FSB fsb_buffers[2];
	char name[26]; } FCB;

#define COUNT( f1 )              (new->f1 - old->f1)
#define TOTAL( f1, f2, f3)       (COUNT(f1) + COUNT(f2) + COUNT(f3))
#define LL3( f1, f2, f3 )         COUNT(f1),  COUNT(f2),  COUNT(f3)
#define LL4( f1, f2, f3, f4 )     COUNT(f1),  COUNT(f2),  COUNT(f3), COUNT(f4)
#define LL5( f1, f2, f3, f4, f5 ) COUNT(f1),  COUNT(f2),  COUNT(f3), COUNT(f4),\
				  COUNT(f5)

    FCB	   *first_file = NULL;
    int	    files=0, found=0, mode=0, splits=0, statistics;
    options_type options;	

void format_fsb_file( FSB *new, FSB *old )
{
    printf ("Open:    %8d  Close: %8d  Con:   %8d  Discon: %9d\n", LL4(
	fsb$l_opens, fsb$l_closes, fsb$l_connects, fsb$l_disconnects));

    printf ("Rewind:  %8d  Flush: %8d  Extend %8d  blocks: %9d\n", LL4(
	fsb$l_rewinds, fsb$l_flushes, fsb$l_extends, fsb$l_extblocks));

    printf ("Outbufquo:%7d  Xqpqio:%8d  Truncate%7d  blocks: %9d\n", LL4(
	fsb$l_outbufquo, fsb$l_xqpqios, fsb$l_truncates, fsb$l_truncblks));

    printf ("Reads    %8d  Bytes: %8d  Writes %8d  bytes:  %9d\n\n", LL4(
	fsb$l_reads, fsb$q_readbytes, fsb$l_writes, fsb$q_writebytes));
}

void format_fsb_record( FSB *new, FSB *old  )
{
    printf ("Find seq:%8d  key:   %8d  rfa:   %8d\n", LL3(
	fsb$l_seqfinds,	fsb$l_keyfinds,	fsb$l_rfafinds));

    printf ("Get  seq:%8d  key:   %8d  rfa:   %8d  bytes:  %9d\n", LL4(
	fsb$l_seqgets,	fsb$l_keygets,	fsb$l_rfagets,	fsb$q_getbytes));

    printf ("Put  seq:%8d  key:   %8d                   bytes:  %9d\n", LL3(
	fsb$l_seqputs,	fsb$l_keyputs,	fsb$q_putbytes));

    printf ("Deletes: %8d                   Updates%8d  bytes:  %9d\n", LL3(
	fsb$l_deletes,	fsb$l_updates,	fsb$q_updatebytes));

    if (splits) printf ("Splits:  %8d  multy: %8d  OutbufQuo%6d\n", LL3(
	fsb$l_bktsplt,	fsb$l_mbktsplt,	fsb$l_outbufquo));
}

void format_fsb_cache( FSB *new, FSB *old  )
{
/*  printf ("Local  cache attempts:%8d  hits:%8d  read:%8d  write:%7d\n", */
/*  printf ("LCL cache%8d  hits:  %8d  reads: %8d  writes: %9d\n",	  */

    printf ("Local  Cache trials%8d  hit%7d  read%6d  write%6d\n", 
	LL4( fsb$l_lcache_attempts, fsb$l_lcachehits, 
	    fsb$l_lbrdirios, fsb$l_lbwdirios));

    printf ("Global Cache trials%8d  hit%7d  read%6d  write%6d\n\n", 
	LL4( fsb$l_gcache_attempts, fsb$l_gcachehits,
	    fsb$l_gbrdirios, fsb$l_gbwdirios));

    if (COUNT(fsb$l_outbufquo)) printf ("GBLBUFQUO exceeded:  :%8d\n\n",
	    COUNT(fsb$l_outbufquo));
}

void format_fsb_summary( FSB *new, FSB *old, FCB *file )
{
    /*
    ** A Put,Update or Delete is expected to take at least one write per key.
    ** More will be needed for bucket splits, index updates, area updates.
    ** Less might be needed is Deferred write and/or Fast Delete are used.
    ** Updated are simply assumed NOT to modify any keys (could use XAB$V_CHG).
    ** This routine attempts to 'normalize' the IOs per Operation by deducting
    ** for the expected minimum number of writes as those are 'required'.
    ** This should make 0 IO/Ops the target irrespective of read/write ratio.
    */
 
float ios_per_ops, ios;
int   ghits, lhits, attempts, ops, write_ops, read_ios, write_ios, updates;
	    
    ops       = TOTAL(fsb$l_seqgets,  fsb$l_keygets,  fsb$l_rfagets) +
	        TOTAL(fsb$l_seqfinds, fsb$l_keyfinds, fsb$l_rfafinds);
    write_ops = TOTAL(fsb$l_seqputs,  fsb$l_keyputs,  fsb$l_deletes);
    updates   = COUNT(fsb$l_updates);
    ops       = ops + write_ops + updates;
    read_ios  = COUNT(fsb$l_gbrdirios) + COUNT(fsb$l_lbrdirios);
    write_ios = COUNT(fsb$l_gbwdirios) + COUNT(fsb$l_lbwdirios) -
		updates - ((file->nok) * write_ops);
    if (write_ios < 0) write_ios = 0;	/* must have been overly pessimistic */
    ios = (read_ios + write_ios);	/* need a floating point for divide  */
    ios_per_ops =0;		        /* used if would divide by zero      */
    if (ops) ios_per_ops = ios / ops;	/* how much in tune is this file?    */

    lhits = COUNT(fsb$l_lcache_attempts);
    ghits = COUNT(fsb$l_gcache_attempts);
    if (lhits) lhits = 100 * COUNT(fsb$l_lcachehits) / lhits;
    if (ghits) ghits = 100 * COUNT(fsb$l_gcachehits) / ghits;

    printf ("%6.2f IO/Ops.  Ops:%7d  Cache:  %2d%%\
 Global: %2d%%  %s\n", ios_per_ops, ops, lhits, ghits, file->name);
}

void format_fsb_locks( FSB *new, FSB *old  )
{
    printf ("Locks:             Enqueue     Dequeue     Convert         Wait   Block-ast\n");
    printf ("-----------------+----------+-----------+-----------+------------+----------\n");
    printf (" Shared file:     %9d   %9d   %9d    %9d   %9d\n", 
	LL5( fsb$l_flckenqs,	fsb$l_flckdeqs,	fsb$l_flckcnvs,
	    fsb$l_flwaits, fsb$l_flblkasts));

    printf (" Local buffer:    %9d   %9d   %9d    %9d   %9d\n", 
	LL5( fsb$l_lblckenqs, fsb$l_lblckdeqs, fsb$l_lblckcnvs,
	    fsb$l_lbwaits, fsb$l_lblblkasts));

    printf (" Global buffer:   %9d   %9d   %9d    %9d   %9d\n", 
	LL5( fsb$l_gblckenqs, fsb$l_gblckdeqs, fsb$l_gblckcnvs,
	    fsb$l_gbwaits, fsb$l_gblblkasts));

    printf (" Append locks:    %9d   %9d   %9d    %9d   %9d\n", 
	LL5( fsb$l_applckenqs, fsb$l_applckdeqs, fsb$l_applckcnvs,
	    fsb$l_apwaits, fsb$l_appblkasts));
    printf (" Global section:  %9d   %9d   %9d    %9d   %9d\n", 
	LL5( fsb$l_gslckenqs,	fsb$l_gslckdeqs, fsb$l_gslckcnvs,
	fsb$l_gswaits, fsb$l_gsblkasts));

    printf (" Data record:     %9d   %9d   %9d    %9d\n\n", 
	LL4( fsb$l_rlckenqs,fsb$l_rlckdeqs, fsb$l_rlckcnvs, fsb$l_rlwaits));
}

int find_some_files (struct FAB *fab)
/*
**  Main setup code to establish a list of files to display statistics for.
**  For each file found with enabled statistics allocate a file control
**  structure, MAP the RMS FSB in exec mode, save mapped address and name.
*/
{
    FCB	    *file;
    struct NAM *nam;
    short   iosb[4];
    desc    devnam, fsb_nam = { FSB_NAMLEN, 0 };
    int	    i, l, status, retadr[2] = {0,0};
    const   char    fao_string[] = "_RMS$FSB!AS!XW!XW!XW";
    static  char    devlocknam_buf[MAX_DEVLOCKNAM];
#pragma nostandard /* initializing using local variable addresses */
    int	    crmpsc_args[] = {12, (int) retadr, (int) retadr,PSL$C_EXEC,
       SEC$M_GBL | SEC$M_SYSGBL | SEC$M_WRT |
       SEC$M_DZRO | SEC$M_PAGFIL | SEC$M_EXPREG,
       (int) &fsb_nam, 0, 0, 0, 1, 0, 0, 0 };
    item    getdvi_items[] = { MAX_DEVLOCKNAM, DVI$_DEVLOCKNAM,
				devlocknam_buf, 0, 0, 0, 0, 0 } ;
    desc    fao_control = { strlen(fao_string), (char *) fao_string };
    desc    devlocknam = { MAX_DEVLOCKNAM, devlocknam_buf};
#pragma standard

    char    fsb_nambuf[FSB_NAMLEN], *name;
    char    expanded_name[256], resultand_name[256];

    fsb_nam.address = fsb_nambuf;
    nam = fab->fab$l_nam;
    status = sys$parse(fab);
    if (status & 1) status = sys$search (fab);
    while (status & 1)
	{
	found++;
	statistics = 0;
	status=sys$open(fab);
	if (status & 1) status=sys$close(fab);
	name = nam->nam$l_rsa;
	name[nam->nam$b_rsl] = 0;
	if (nam->nam$l_fnb & NAM$M_WILDCARD) {
	    if (status == RMS$_FLK) status = 1; /* locked is fine */
	    } else if (!statistics) 
	       printf ("RMS Statistics NOT enabled on: %s\n", name);

	if (statistics) {
	    files++;
	    file = malloc (sizeof(*file));
	    memset (file, 0, sizeof (*file));
	    file->next_file = first_file;
	    file->nok = ((struct XABSUM *) fab -> fab$l_xab) -> xab$b_nok;
	    first_file = file;
	    l = (nam->nam$l_ver - nam->nam$l_name);
	    if (l > sizeof (file->name)) l = sizeof (file->name);
	    memcpy (file->name, nam->nam$l_name, l);
	    file->name[l]=0;
	    devnam.count = nam->nam$b_dev;
	    devnam.address = nam->nam$l_dev;
	    if (status & 1) status = sys$getdvi ( 0, 0, &devnam, getdvi_items,0,0,0,0);
	    if (status & 1) status = sys$fao ( &fao_control, 0, &fsb_nam, 
		    &devlocknam, nam->nam$w_fid[0], 
		    nam->nam$w_fid[1], nam->nam$w_fid[2]);
	    if (status & 1) status = sys$cmexec (&sys$crmpsc, crmpsc_args);
	    file->exec_mode_fsb = (FSB *) retadr[0];
	}
	if (status & 1) status = sys$search (fab);
    }
    if ( status == RMS$_NMF) status = 1;
    return status;
}

main(int argc, char *argv[])
    {
    struct FAB fab, list_fab;
    struct RAB list_rab;
    struct NAM nam;
    struct XABSUM sum;
    struct XABITM itm;
    FCB	   *file;
    int	    privs[] = { PRV$M_CMEXEC | PRV$M_SYSGBL , 0};
    int	    status, i, incremental = 0, repeat_count = 1, sleep_time = 0;
    int	    copy_fsb_args[] = {3, sizeof (FSB), 0, 0};
    item    statistics_items[] = { sizeof (int), XAB$_STAT_ENABLE, 
				&statistics, 0, 0, 0, 0, 0 };
    item    getjpi_items[] = { sizeof (int), 0, &mode, 0, 0, 0, 0, 0 };
    char    *arg;
    short   iosb[4], channel=0;
    char    expanded_name[256], resultand_name[256], search_name[256], c;
    $DESCRIPTOR (input, "SYS$INPUT:");
/*
**  RMS Parse, Search and Open data initialization.
*/
    fab = cc$rms_fab;
    fab.fab$l_fop = FAB$M_NAM;
    fab.fab$l_dna = ".DAT";
    fab.fab$b_dns = strlen(fab.fab$l_dna);
    fab.fab$b_shr = FAB$M_UPD;
    fab.fab$l_nam = &nam;
    fab.fab$l_xab = (char *) &sum;  /* must be first for find_some_files */
    
    nam = cc$rms_nam;
    nam.nam$l_rsa = resultand_name;
    nam.nam$b_rss = 255;
    nam.nam$l_esa = expanded_name;
    nam.nam$b_ess = 255;

    sum = cc$rms_xabsum;
    sum.xab$l_nxt = (char *) &itm;

    itm.xab$b_bln = XAB$C_ITMLEN;   /* no template available ?! */
    itm.xab$b_cod = XAB$C_ITM;
    itm.xab$l_itemlist = (char *) &statistics_items;
    itm.xab$b_mode = 1  /* XAB$K_SENSEMODE */;
    itm.xab$l_nxt = 0;


/*
** Argument parsing. Here we find out what to do on which files.
*/

options.all = 0;
options.summary_stats=1;
while (--argc > 0 &&  **++argv == '-' )
    { 

    arg = *argv;
    switch (*++arg) {
    case 'r':
	if (repeat_count<2) repeat_count = 999999;
	c = *++arg;
	if (c!=0) {
	    if ((c != '=') && (c != ':')) {
		argc = 0;
		} else sscanf( ++arg, "%d", &sleep_time);
	    } else sleep_time = 10;
	break;

    case 'i':
	if (repeat_count<2) repeat_count = 2;
	if (sleep_time==0) sleep_time = 10;
	incremental = 1;	
	break;
	
    case 'n':
	if (sleep_time==0) sleep_time = 10;
	c = *++arg;
	if (c!=0) {
	    if ((c != '=') && (c != ':')) {
		argc = 0;
		} else sscanf( ++arg, "%d", &repeat_count);
	    } else repeat_count = 2;
	break;
	
    case 'o':
	options.all = 0;
	c = *++arg;
	if (c!=0) {
	    if ((c != '=') && (c != ':')) {
		argc = 0;
		} 
	    else while ( (c = *++arg)!=0 ) {
		if (c=='a' | c=='f') options.file_stats=1;
		if (c=='a' | c=='l') options.locks_stats=1;
		if (c=='a' | c=='r') options.record_stats=1;
		if (c=='a' | c=='c') options.cache_stats=1;
		if (c=='a' | c=='s') options.summary_stats=1;
		};
	    } else options.all = -1; /* all options */
	break;

    default:
	printf ("SIDR: Illegal option -%c.\n", *arg);
	argc = 0;
	break;
	}
    }

if ( argc != 1 )  {
    printf ("\nUsage: $RMS_STATS [-o=afrlcs] [-r=#seconds] [-i] [@]file.dat\n");
    printf ("	-o[=afrlcs] Output Options. One line summary default.\n");
    printf ("               [All, File, Record, Lock, Cache, Summary]\n");
    printf ("	-r[=n]      Repeat. Optional time. Default 10 seconds.\n");
    printf ("	-n[=n]	    Repeat. Optional Number of repeats. Default 2\n");
    printf ("	-i	    Incremental values (default repeat = 10 sec) \n");
    printf ("	[@]list.dat [File of] wildcarded file specification.\n\n");
    printf ("The summary line, has a summary field labeled 'IO/Ops' which\n");
    printf ("reflects the average number of IOs per record operation.\n");
    printf ("The number is discounted for 'expected' write IOs and thus\n");
    printf ("the closer IO/Ops is to 0, the more the file is 'in tune'!\n\n");
    printf ("Hope this helps! Hein van den Heuvel, Xmas 1994.\n");
    return 268435456; /* trivia... go figure */
    }
    /* DMFBC Only valid in 6.1 which is when RMS started to count splits */
    getjpi_items[0].cod = DFMBC;    
    splits = 1 & sys$getjpiw ( 0, 0, 0, getjpi_items, iosb, 0, 0);
    getjpi_items[0].cod = JPI$_MODE;
    status = sys$getjpiw ( 0, 0, 0, getjpi_items, iosb, 0, 0);
    status = sys$setprv ( 1, privs, 0, 0);
    if (!(status & 1)) {
	printf ("Sorry... could not get required CMEXEC privs.\n");
	return status;
	}

    if (**argv == '@') {
	list_fab = cc$rms_fab;
	list_fab.fab$l_fna = *argv + 1;
	list_fab.fab$b_fns = strlen(list_fab.fab$l_fna);
	list_rab = cc$rms_rab;
	list_rab.rab$l_ubf = (char *) search_name;
	list_rab.rab$w_usz = 255;
	list_rab.rab$l_fab = &list_fab;
	fab.fab$l_fna = (char *) search_name;
	status = sys$open( &list_fab);
	if (status & 1 ) status = sys$connect (&list_rab);
	if (status & 1 ) status = sys$get (&list_rab);
	while (status & 1) {
	    fab.fab$b_fns = list_rab.rab$w_rsz;
	    status = find_some_files (&fab);
	    if (status & 1 ) status = sys$get (&list_rab);
	    }
	(void) sys$close( &list_fab);
	if (status == RMS$_EOF) status = 1;	    /* no problem */
	} else {
	fab.fab$l_fna = *argv;
	fab.fab$b_fns = strlen( *argv );
	status = find_some_files (&fab);
	}
    if ( ! ( status & 1 )) return status;
    if ( found > 1 ) printf("%2d Files matched specifications.\n", found);
    if ( files > 1 ) printf("%2d Files with statistics  found.\n\n",files);
    if ( !files ) {
	expanded_name[nam.nam$b_esl] = 0;
	if ( found > 1 ) printf ("NO files with statistics enabled found: %s\n", expanded_name);
	return 1;
	}

/*
**  Main processing. Loop through all files for as long as requested
**  For each file, copy RMS FSB information in EXEC mode to control 
**  structure and report with formatted numbers as requested.
*/

    while ( repeat_count-- > 0) {
	
	int buf;
	FCB *file;
	FSB *new, *old;

	file = first_file;
	while (file != NULL) {
							/* locate buffers */
	    buf = file->new_buffer;
	    new = &file->fsb_buffers[buf];
	    buf = 1 - buf;
	    old = &file->fsb_buffers[buf];
							/* get new data */
	    copy_fsb_args[2] = (int) file->exec_mode_fsb;
	    copy_fsb_args[3] = (int) new;
	    (void) sys$cmexec (&ots$move3, copy_fsb_args);

							/* requested stats */
	    if (options.summary_stats) {
		if (options.all != options.summary_stats) printf ("\n");
		format_fsb_summary(new, old, file);
		} else if (files > 1) printf( "\n%s\n", file->name);

	    if (options.cache_stats)   format_fsb_cache  (new, old);
	    if (options.locks_stats)   format_fsb_locks  (new, old);
	    if (options.file_stats)    format_fsb_file   (new, old);
	    if (options.record_stats)  format_fsb_record (new, old);

	    if (incremental) file->new_buffer = buf;	/* swaps buffers    */

/*	    if ( (repeat_count!=0) && (options.all == -1)) printf ("\n");   */

	    file = file->next_file ;
	    }

	if (sleep_time!=0 && repeat_count!=0) {		/* wait if desired  */
	    if (mode == JPI$K_INTERACTIVE) {		/* terminal user?   */
		if (!channel) status = sys$assign ( &input, &channel,0,0,0);
		if (status & 1) status = sys$qiow (0,channel, 
			IO$_READVBLK | IO$M_TIMED, iosb, 0, 0, 
			search_name, 255, sleep_time, 0, 0, 0);
		if (status & 1) status = iosb[0];	/* real io status?  */
		if ((status == SS$_NORMAL) && 
		    (iosb[2] == 26)) repeat_count = 0;	/* control-Z  seen? */
		if (status == SS$_TIMEOUT) status = 1;	/* timeout no error */
		if (!(status & 1)) repeat_count = 0;	/* done if error    */
		} else {
		float   fleep_time;
		fleep_time = sleep_time;
		lib$wait(&fleep_time);			/* batch mode sleep */
		}    
	    }
	}
    return status;			         
}
