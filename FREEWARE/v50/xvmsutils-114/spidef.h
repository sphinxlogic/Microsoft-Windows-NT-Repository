#ifndef	__SPIDEF_LOADED
#define __SPIDEF_LOADED    1

/* SPIDEF 
 For VMS V5.x
 Item code definitions for the Get System Performance Information used by
 the MONITOR program and contained in the protected shared image SPISHR.EXE.
 Extracted from the V5.0 fiche, assembly listing of SYSGETSPI.
 Editted for VAXC V3.1 to allow nested #include's on 06-Jul-1990 (FJN)
 Editted to fix incorrect ProcRecord field sizes on 08-Nov-1990 (FJN)
*/	  

#define SPI$_ACCESS	0X20F0		/* Number of file accesses */
#define SPI$_ACCLCK	0X2105		/* Access locks */
#define SPI$_ALLOC	0X20F1		/* Number of file extends */
#define SPI$_ARRLOCPK	0X104D		/* Arriving local packets */
#define SPI$_ARRTRAPK	0X104F		/* Arriving transit packets */
#define SPI$_BIGHOLE	0X1030		/* Largest block in dynamic memory */
#define SPI$_BLKAST	0X1046		/* Number of blocking AST's */
#define SPI$_BLKIN	0X1077		/* Blocking ASTs queued (incoming) */
#define SPI$_BLKLOC	0X1076		/* Blocking ASTs queued (local) */
#define SPI$_BLKOUT	0X1078		/* Blocking ASTs queued (outgoing) */
#define SPI$_BUFIO	0X1037		/* Count of buffered I/Os */
#define SPI$_CEF	0X100D		/* Common event flag wait */
#define SPI$_COLPG	0X1008		/* Collided page wait */
#define SPI$_COM	0X1016		/* Computable */
#define SPI$_COMO	0X1017		/* Outswapped and computable */
#define SPI$_CPUBUSY	0X100A		/* OBSOLETE item, returns 0 */
#define SPI$_CUR	0X1018		/* Currently executing */
#define SPI$_DEPLOCPK	0X104E		/* Departing local packets */
#define SPI$_DEQ	0X1045		/* Number of DEQ's */
#define SPI$_DEQIN	0X1074		/* Dequeues (incoming) */
#define SPI$_DEQLOC	0X1073		/* Dequeues (local) */
#define SPI$_DEQOUT	0X1075		/* Dequeues (outgoing) */
#define SPI$_DIRDATA_TRIES  0X1067	/* Count of Directory data cache attempts */
#define SPI$_DIRDATA_HIT    0X20FC	/* Count of Directory data cache hits */
#define SPI$_DIRFCB_HIT    0X20FA	/* Count of Directory block cache hits */
#define SPI$_DIRFCB_MISS    0X20FB	/* Count of Directory block cache misses */
#define SPI$_DIRFCB_TRIES   0X1066	/* Count of Directory block cache attempts */
#define SPI$_DIRIN	0X1079		/* Directory operations (incoming) */
#define SPI$_DIRIO	0X1036		/* Count of direct I/Os */
#define SPI$_DIROUT	0X107A		/* Directory operations (outgoing) */
#define SPI$_DISKS	0X106B		/* All disk data */
#define SPI$_DLCKFND	0X104A		/* Number of deadlocks found */
#define SPI$_DLCKMSGS	0X107B		/* Deadlock detection messages (in & out) */
#define SPI$_DLCKSRCH	0X1049		/* Number of deadlock searches */
#define SPI$_DYNINUSE	0X1033		/* Dynamic memory space in use */
#define SPI$_DZROFLTS	0X1027		/* Demand zero faults */
#define SPI$_ENQCVT	0X1044		/* Number of ENQ's (conversions) */
#define SPI$_ENQCVTLOC	0X1070		/* Lock conversion requests (local) */
#define SPI$_ENQCVTOUT	0X1072		/* Lock conversion requests (outgoing) */
#define SPI$_ENQCVTIN	0X1071		/* Lock conversion requests (incoming) */
#define SPI$_ENQNEWLOC	0X106D		/* New lock requests (local) */
#define SPI$_ENQNEW	0X1043		/* Number of ENQ's (new) */
#define SPI$_ENQNEWOUT	0X106F		/* New lock requests (outgoing) */
#define SPI$_ENQNEWIN	0X106E		/* New lock requests (incoming) */
#define SPI$_ENQNOTQD	0X1048		/* Number of ENQ's no queued */
#define SPI$_ENQWAIT	0X1047		/* Number of ENQ's forced to wait */
#define SPI$_EXTHIT	0X20FD		/* Count of Extent cache hits */
#define SPI$_EXTMISS	0X20FE		/* Count of Extent cache misses */
#define SPI$_EXT_TRIES	0X1068		/* Count of Extent cache attempts */
#define SPI$_FAULTS	0X101E		/* Page fault count */
#define SPI$_FCPCACHE	0X103E		/* Number of FCP cache hits */
#define SPI$_FCPCALLS	0X103B		/* Total FCP calls */
#define SPI$_FCPCPU	0X103F		/* Number of CPU tics used by FCP */
#define SPI$_FCPCREATE	0X20F2		/* Number of file creations */
#define SPI$_FCPERASE	0X20F5		/* Number of erase calls */
#define SPI$_FCPFAULT	0X1042		/* Number of FCP page faults */
#define SPI$_FCPHIT	0X1040		/* Number of window hits */
#define SPI$_FCPREAD	0X103C		/* Number of disk reads by FCP */
#define SPI$_FCPSPLIT	0X1041		/* Numer of split transfers */
#define SPI$_FCPTURN	0X20F4		/* Number of window turns */
#define SPI$_FCPWRITE	0X103D		/* Number of disk writes by FCP */
#define SPI$_FIDHIT	0X20F7		/* Count of File Id cache hits */
#define SPI$_FIDMISS	0X20F8		/* Count of File Id cache misses */
#define SPI$_FID_TRIES	0X1064		/* Count of File Id cache attempts */
#define SPI$_FILHDR_HIT	0X20F9		/* Count of File Header cache hits */
#define SPI$_FILHDR_TRIES   0X1065	/* Count of File Header cache attempts */
#define SPI$_FPG	0X1015		/* Fragmented paging wait */
#define SPI$_FREFLTS	0X1025		/* Faults from free page list */
#define SPI$_FRLIST	0X101C		/* Size of free page list */
#define SPI$_GVALFLTS	0X1023		/* Global valid page faults */
#define SPI$_HIB	0X1011		/* Hibernating */
#define SPI$_HIBO	0X1012		/* Outswapped and hibernating */
#define SPI$_HOLECNT	0X102F		/* Number of blocks in dynamic memory */
#define SPI$_HOLESUM	0X1032		/* Total available dynamic memory */
#define SPI$_IRPCNT	0X102B		/* Number of IRP packets available */
#define SPI$_IRPINUSE	0X102C		/* Number of IRP packets in use */
#define SPI$_ISWPCNT	0X1035		/* Total inswaps */
#define SPI$_LASTEWS	0X216B		/* Last item in exec writable table */
#define SPI$_LASTEXE	0X10F0		/* Last item in the EXE table */
#define SPI$_LEF	0X100F		/* Local event flag wait */
#define SPI$_LEFO	0X1010		/* Outswapped local event flag wait */
#define SPI$_LOGNAM	0X103A		/* Logical name translations */
#define SPI$_LRPCNT	0X1029		/* Number of LRP packets available */
#define SPI$_LRPINUSE	0X102A		/* Number of LRP packets in use */
#define SPI$_MBREADS	0X1038		/* Total mailbox reads */
#define SPI$_MBWRITES	0X1039		/* Total mailbox writes */
#define SPI$_MFYFLTS	0X1026		/* Faults from modified page list */
#define SPI$_MODES	0X1000		/* All modes counters on all CPU's */
#define SPI$_MODLIST	0X101D		/* SIze of modified page list */
#define SPI$_MSCP_ALL	0X108B		/* Collect all MSCP Server items */
#define SPI$_MSCP_BUFWAIT   0X1083	/* Rate incoming requests have to wait for a buffer */
#define SPI$_MSCP_FRAGMENT  0X1081	/* Rate at which I/O's are fragmented */
#define SPI$_MSCP_READ	0X107F		/* Read request rate to MSCP server */
#define SPI$_MSCP_REQUEST   0X107E	/* IO request rate to MSCP server */
#define SPI$_MSCP_SIZE1	0X1084		/* I/O rate for sizes 1-2 blocks */
#define SPI$_MSCP_SIZE2	0X1085		/* I/O rate for sizes 2-3 blocks */
#define SPI$_MSCP_SIZE3	0X1086		/* I/O rate for sizes 4-7 blocks */
#define SPI$_MSCP_SIZE4	0X1087		/* I/O rate for sizes 8-15 blocks */
#define SPI$_MSCP_SIZE5	0X1088		/* I/O rate for sizes 16-31 blocks */
#define SPI$_MSCP_SIZE6	0X1089		/* I/O rate for sizes 32-63 blocks */
#define SPI$_MSCP_SIZE7	0X108A		/* I/O rate for sizes 64-127 blocks */
#define SPI$_MSCP_SPLIT	0X1082		/* Rate at which I/O's are split */
#define SPI$_MSCP_WRITE	0X1080		/* Write request rate to MSCP server */
#define SPI$_MWAIT	0X100C		/* Misc. wait */
#define SPI$_NUMLOCKS	0X104B		/* Total locks */
#define SPI$_NUMRES	0X104C		/* Total resources */
#define SPI$_OPENS	0X20F6		/* Number of file opens */
#define SPI$_OTHSTAT	0X1019		/* OBSOLETE item, returns 0 */
#define SPI$_PFW	0X100E		/* Page fault wait */
#define SPI$_PREADIO	0X1022		/* Physical page read I/Os */
#define SPI$_PREADS	0X101F		/* Page reads */
#define SPI$_PROC	0X101B		/* Process information */
#define SPI$_PROCS	0X101A		/* Process count for SYSTEM class */
#define SPI$_PWRITES	0X1020		/* Page writes */
#define SPI$_PWRITIO	0X1021		/* Physical page write I/Os */
#define SPI$_QUOHIT	0X20FF		/* Count of Quota cache hits */
#define SPI$_QUOMISS	0X2100		/* Count of Quota cache misses */
#define SPI$_QUO_TRIES	0X1069		/* Count of Quota cache attempts */
#define SPI$_RCVBUFFL	0X1051		/* Receiver buffer failures */
#define SPI$_SCS	0X107C		/* All SCS information */
#define SPI$_SMALLCNT	0X1034		/* Number of blocks < 32 bytes in size */
#define SPI$_SMALLHOLE	0X1031		/* Smallest block in dynamic memory */
#define SPI$_SRPCNT	0X102D		/* Number of SRP packets available */
#define SPI$_SRPINUSE	0X102E		/* Number of SRP packets in use */
#define SPI$_STORAGMAP_TRIES	0X106A	/* Count of Storage bitmap cache attempts */
#define SPI$_STORAGMAP_HIT  0X2101	/* Count of Storage bitmap cache hits */
#define SPI$_SUSP	0X1013		/* Suspended */
#define SPI$_SUSPO	0X1014		/* Outswapped and suspended */
#define SPI$_SYNCHLCK	0X2103		/* Directory and file synch locks */
#define SPI$_SYNCHWAIT	0X2104		/* # of times XQP waited for a directory or file synch lock */
#define SPI$_SYSFAULTS	0X1028		/* System page faults */
#define SPI$_SYSTIME	0X107D		/* Current system time */
#define SPI$_TOTAL_LOCKS    0X106C	/* COMPUTED BY MONITOR */
#define SPI$_TRCNGLOS	0X1050		/* Transit congestion loss */
#define SPI$_VOLLCK	0X2102		/* Volume synchronization locks */
#define SPI$_VOLWAIT	0X20F3		/* # of times XQP waited for volume lock */
#define SPI$_WRTINPROG	0X1024		/* Faults from write-in-progress */
#define SPI$_XQPCACHEWAIT   0X2106	/* # of times XQP had to wait for free space in a cache */

/*
 SCS Class information
*/
struct SCSRecord {
    char nodename[8];			/* System node name */
    unsigned long dgsent;		/* Application datagrams sent */
    unsigned long dgrcvd;		/* Application datagrams received */
    unsigned long dgdiscard;		/* Application datagrams discarded */
    unsigned long msgsent;		/* Application messages sent */
    unsigned long msgrcvd;		/* Application messages received */
    unsigned long snddats;		/* Block send datas initiated */
    unsigned long kbytsent;		/* KBytes send via send datas */
    unsigned long reqdats;		/* Block request datas initiated */
    unsigned long kbytreqd;		/* KBytes received via request datas */
    unsigned long kbytmapd;		/* KBytes mapped for block transfers */
    unsigned long qcr_cnt;		/* Times connection queued for send credit*/
    unsigned long qbdt_cnt;		/* Times connection queued for buffer descriptor */
    };

/*
 DISK class information.
*/
struct DiskRecord {
    unsigned char allocclass;		/* Allocation class */
    char devname[4];			/* Device name */
    unsigned short unitnum;		/* Unit number */
    unsigned char flags;		/* Flags (low bit == served disk */
    char nodename[8];			/* Node name */
    char volname[12];			/* Volume name */
    unsigned long optcnt;		/* Operation count */
    unsigned long qcount;		/* Queue length accumulator */
    };

/*
 PROCESSES class data block.
*/
struct ProcRecord {
    unsigned long ipid;			/* Internal PID */
    unsigned long uic;			/* UIC (member is low-order word) */
    unsigned short state;		/* State value */
    unsigned char pri;			/* Priority (negative value) */
    struct {
	unsigned char count;		/* Text length count */
	char text[15];
	} lname;			/* Process name (counted string) */
    unsigned long gpgcnt;		/* Global page count */
    unsigned long ppgcnt;		/* Process page count */
    unsigned long sts;			/* PCB Status Vector
					   (PCB$V_RES bit clear -> swapped out) */
    unsigned long diocnt;		/* Direct I/O count */
    unsigned long pageflts;		/* Page fault count */
    unsigned long cputim;		/* Accumulated CPU time (in ticks) */
    unsigned long biocnt;		/* Buffered I/O count */
    unsigned long epid;			/* Extended PID */
    unsigned long efwm;			/* Event flag wait mask (for MWAITs) */
    };

/*
 Begin items specific to the RMS class.  These items are maintained by
 RMS and collected from global sections named RMS$xxxxxxxxyyyyyy where
 x is the volume lock id and y is the fid of the file being monitored.
*/

#define SPI$_APPBLKASTS	0X422C		/* total # of shared append lock blocking ASTs */
#define SPI$_APPLCKCNVS	0X4228		/* total # of append lock conversions */
#define SPI$_APPLCKDEQS	0X4227		/* total # of append lock DEQs */
#define SPI$_APPLCKENQS	0X4226		/* total # of append lock ENQs */
#define SPI$_APWAITS	0X423F		/* total # of append lock ENQs forced to wait */
#define SPI$_BKTSPLT	0X4235		/* total # of 2 bucket splits done for this file */
#define SPI$_CLOSES	0X4238		/* total # of times file has been closed */
#define SPI$_CONNECTS	0X420F		/* total # of CONNECT requests */
#define SPI$_DELETES	0X4207		/* total # of DELETEs to file */
#define SPI$_DISCONNECTS    0X4210	/* total # of DISCONNECT requests */
#define SPI$_EXTBLOCKS	0X4212		/* total # of blocks file has been extended */
#define SPI$_EXTENDS	0X4211		/* total # of EXTENDs */
#define SPI$_FLBLKASTS	0X4229		/* total # of file lock blocking ASTs */
#define SPI$_FLCKCNVS	0X4219		/* total # of file lock conversions */
#define SPI$_FLCKDEQS	0X4218		/* total # of file lock DEQs */
#define SPI$_FLCKENQS	0X4217		/* total # of file lock ENQs */
#define SPI$_FLUSHES	0X4213		/* total # of calls to $FLUSH */
#define SPI$_FLWAITS	0X423A		/* total # of file lock ENQs forced to wait */
#define SPI$_GBLBLKASTS	0X422B		/* total # of global buffer lock blocking ASTs */
#define SPI$_GBLCKCNVS	0X421F		/* total # of global buffer lock conversions */
#define SPI$_GBLCKDEQS	0X421E		/* total # of global buffer lock DEQs */
#define SPI$_GBLCKENQS	0X421D		/* total # of global buffer lock ENQs */
#define SPI$_GBRDIRIOS	0X4231		/* total # of direct IOs caused by global buffer cache reads */
#define SPI$_GBWAITS	0X423C		/* total # of global buffer ENQs forced to wait */
#define SPI$_GBWDIRIOS	0X4232		/* total # of direct IOs caused by global buffer cache writes */
#define SPI$_GCACHEHITS	0X422F		/* total cache hits on buckets in global buffers */
#define SPI$_GCACHETRIES    0X4230	/* total cache attempts on buckets in global buffers */
#define SPI$_GETBYTES	0X4201		/* total size in bytes of all GETs */
#define SPI$_GSBLKASTS	0X4239		/* total # of global section lock block ing ASTs */
#define SPI$_GSLCKCNVS	0X4222		/* total # of global section lock conversions */
#define SPI$_GSLCKDEQS	0X4221		/* total # of global section lock DEQs */
#define SPI$_GSLCKENQS	0X4220		/* total # of global section lock ENQs */
#define SPI$_GSWAITS	0X423D		/* total # of global section ENQs forced to wait */
#define SPI$_KEYFINDS	0X420B		/* total # of keyed FINDs */
#define SPI$_KEYGETS	0X41FF		/* total # of keyed GETs */
#define SPI$_KEYPUTS	0X4203		/* total # of PUTs by key */
#define SPI$_LBLBLKASTS	0X422A		/* total # of local buffer lock blocking ASTs */
#define SPI$_LBLCKCNVS	0X421C		/* total # of local buffer lock conversions */
#define SPI$_LBLCKDEQS	0X421B		/* total # of local buffer lock DEQs */
#define SPI$_LBLCKENQS	0X421A		/* total # of local buffer lock ENQs */
#define SPI$_LBRDIRIOS	0X4233		/* total # of direct IOs caused by local buffer cache reads */
#define SPI$_LBWAITS	0X423B		/* total # of local buffer ENQs forced to wait */
#define SPI$_LBWDIRIOS	0X4234		/* total # of direct IOs caused by local buffer cache writes */
#define SPI$_LCACHEHITS	0X422D		/* total cache hits on buckets in local buffers */
#define SPI$_LCACHETRIES    0X422E	/* total cache attempts on buckets in local buffers */
#define SPI$_MBKTSPLT	0X4236		/* total # of multi-bucket splits done for this file */
#define SPI$_OUTBUFQUO	0X4241		/* # of times a process ran out of buffer quota */
#define SPI$_PUTBYTES	0X4204		/* total # of bytes PUT to file */
#define SPI$_READBYTES	0X420E		/* total # of bytes from $READS */
#define SPI$_READS	0X420D		/* total # of $READs */
#define SPI$_REWINDS	0X4214		/* total # of calls to $REWIND */
#define SPI$_RFAFINDS	0X420C		/* total # of RFA FINDs */
#define SPI$_RFAGETS	0X4200		/* total # of GETs by RFA */
#define SPI$_RLCKCNVS	0X4225		/* total # of record lock conversions */
#define SPI$_RLCKDEQS	0X4224		/* total # of record lock DEQs */
#define SPI$_RLCKENQS	0X4223		/* total # of record lock ENQs */
#define SPI$_RLWAITS	0X423E		/* total # of record lock ENQs forced to wait */
#define SPI$_RMSOPENS	0X4237		/* total # of times file has been opened */
#define SPI$_RMS_ORG	0X4263		/* RMS file org */
#define SPI$_RMS_STATS	0X41FD		/* All of the RMS statistics */
#define SPI$_SEQFINDS	0X420A		/* total # of sequential FINDs */
#define SPI$_SEQGETS	0X41FE		/* total # of sequential GETs */
#define SPI$_SEQPUTS	0X4202		/* total # of sequential PUTs */
#define SPI$_TOTWAITS	0X4240		/* total # of ENQs forced to wait */
#define SPI$_TRUNCATES	0X4208		/* # of times file has been truncated */
#define SPI$_TRUNCBLKS	0X4209		/* # of blocks file has been truncated */
#define SPI$_UPDATEBYTES    0X4206	/* total # of bytes UPDATEd in file */
#define SPI$_UPDATES	0X4205		/* total # of UPDATEs to file */
#define SPI$_WRITEBYTES	0X4216		/* total # of bytes written by $WRITE */
#define SPI$_WRITES	0X4215		/* total # of calls to $WRITE */
#define SPI$_XQPQIOS	0X4251		/* # of XQP operations for this file by RMS */


#endif					/* __SPIDEF_LOADED */
