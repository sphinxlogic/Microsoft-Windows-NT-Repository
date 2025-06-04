#pragma module MWAIT "X-2"

/*
 *
 * Copyright © 1997,1998 by Kari Salminen. This code may be freely distributed
 * and modified for non-commercial purposes as long as this copyright notice is
 * retained.
 *
 * This software is provided "AS IS". The author makes no representations or
 * warranties with respect to the software and specifically disclaim any implied
 * warranties of merchantability or fitness for any particular purpose.
 *
 * COMPONENT:	mwait.c
 *
 * FACILITY:	ANALYZE A PROCESS HANG
 *
 * ABSTRACT:
 *
 *	This image analyzes the reason for a process hang and displays
 *	approriate information of the target process.
 *
 * ENVIRONMENT:
 *
 *	Runs in User and Kernel mode. Raises IPL to SCHED.
 *
 *
 * VERSION:	01.00
 *
 * AUTHOR:	Kari Salminen 
 *
 * DATE:	24-APR-1997
 *
 *
 * MODIFIED BY:
 *
 *	V2.0	13-MAY-1997	Kari Salminen
 *				Port to VMS V7.x
 *				
 *	V2.1	14-JUL-1997	Kari Salminen
 *				Add EFN$C_ENF, thread upcall support.
 *
 *	V2.2	17-JUL-1997	Kari Salminen
 *				Add thread stuff
 *				Port the MACRO-32 subroutines into C.
 *
 *	V2.3	31-JUL-1997	Kari Salminen
 *				Fix AST pending.
 *				Add IRP's.
 *
 *	V2.4	18-SEP-1997	Kari Salminen
 *				Add Network IRP's and PORT's
 *
 *	V2.5	 4-DEC-1997	Kari Salminen
 *				Add Master, Owner and Sub-processes
 *
 *	V2.6	21-APR-1998	Kari Salminen
 *				Add OSI and TCP/IP version display
 *
 *	V2.7	10-JUN-1998	Kari Salminen
 *				Get xxx_ATB symbol values at runtime from
 *				NET$TRANSPORT_xxx.STB and LDR$GQ_IMAGE_LIST
 *
 *	V2.8	18-AUG-1998	Kari Salminen
 *				Add INET device, the CXB's are on BGn:'s
 *				Write output into a file
 *
 *
 *
 *
 *
*/

#pragma code_psect    "NONPAGED_CODE"
#pragma linkage_psect "NONPAGED_LINKAGE"


#include <acbdef.h>		/* AST control block definitions	 */
#include <bufiodef.h>		/* Buffered I/O Packet headers		 */
#include <ccbdef.h>		/* Channel control block definitions     */
#include <cebdef.h>		/* Common event flag wait definitions    */
#include <cxbdef.h>		/* Complex chained buffer Definitions    */
#include <dcdef.h>		/* Device adapter, class, and type defs  */
#include <ddbdef.h>		/* Device data block definitions	 */
#include <descrip.h>		/* VMS Argument Descriptor Formats 	 */
#include <devdef.h>		/* Device characteristics definitions	 */
#include <dispdef.h>		/* Change mode dispatcher offsets	 */
#include <dscdef.h>		/* VMS descriptors 			 */
#include <dyndef.h>		/* Data structure type definitions	 */
#include <eobjrecdef.h>		/* Object record definitions 		 */
#include <egsdef.h>		/* Global symbol definitions 		 */
#include <egstdef.h>		/* Universal Symbol Definitions		 */
#include <fcbdef.h>		/* File control block definitions	 */
#include <imcbdef.h>		/* Image control block definitions	 */
#include <iodef.h>		/* I/O function codes 			 */
#include <irpdef.h>		/* I/O request packet definitions	 */
#include <jibdef.h>		/* Job Information Block 		 */
#include <jpidef.h>		/* Job Process Information definitions	 */
#include <kferesdef.h>		/* File entry defs for resident sections */
#include <ldrimgdef.h>		/* System code loader definitions	 */
#include <lib$routines.h>	/* Library routines			 */
#include <pcbdef.h>		/* Process Control Block Definitions 	 */
#include <pdscdef.h>		/* Procedure descriptor offset defs 	 */
#include <phddef.h>		/* Process header definitions 		 */
#include <pscandef.h>		/* Process scan definitions		 */
#include <prdef.h>		/* Processor register definitions	 */
#include <pridef.h>		/* Priority increment class definitions	 */
#include <psldef.h>		/* Processor status longword definitions */
#include <rms.h>		/* RMS data structure definitions	 */
#include <rsndef.h>		/* Resource name definitions		 */
#include <sbdef.h>		/* System block definitions		 */
#include <secdef.h>             /* Process or global section definitions */
#include <splcoddef.h>		/* Bits for SMP$GL_FLAGS and spinlock codes */
#include <ssdef.h>		/* Define system status codes 		 */
#include <statedef.h>		/* Define SCH$C_xxxx 			 */
#include <stdio.h>		/* ANSI C Standard Input/Output		 */
#include <stdlib.h>             /* General Utilities                     */
#include <string.h>		/* String Handling			 */
#include <ucbdef.h>		/* UCB definitions			 */
#include <vcrpdef.h>		/* VAX Communication Request Packet defs */
#include <vms_macros.h>		/* VMS Exec/System macros		 */
#include <wcbdef.h>		/* Window control block definitions	 */



		/* These .H files are not in C .TLB's 	*/

/* #include "ucbnidef.h"		*/
	/* LAN definitions that follow the standard UCB fields	*/

typedef struct _ucbni {
/* Overlay the UCB structure at UCB$Q_NI_RCVMSG		*/
    char ucbni$$ni_ucbfill_1 [744];
    unsigned int ucb$q_ni_rcvreq;      /* S-Receive IRP waiting for messages */
    } UCBNI;


/* #include "proc_read_writedef.h"	*/
	/* Definitions for reading memory/regs 	 */

#define EACB$K_MEMORY 1                 /* virtual memory               */
#define EACB$K_GENERAL_REGISTER 2       /* R0-R29,PC,PS                 */
#define EACB$K_GEN_REGS_LENGTH 32	/* Register length		*/
#define EACB$K_R0 0			/* Register R0			*/
#define BUFFER_LENGTH 1024		/* .STB file read buffer size	*/
#define GSD_SIZ 8			/* GSD header size		*/	

#define MAX_THREADS 16		/* Maximum # of threads			 */
#define MAX_IRP 300		/* Maximum # of IRP's for a unit         */

#define PHASEV 0x10000	/* DECnet Phase V = 50000 (101), IV = 40000 (100) */

#define PID_SIZE 8		/* PID length 			   */
#define ESCAPE 27       	/* Code for ASCII escape character */


/* Note: the following structure information was found out with SDA,	*/
/*       without the source listings !, maybe not correct.		*/

typedef struct _ucbbg {
    char ucbbg$$bg_ucbfill_1 [336];
    unsigned int ucb$l_bg_irp;		/* BG device IRP */
    char ucbbg$$bg_ucbfill_2 [828];
    unsigned int ucb$l_bg_irp2;		/* BG device IRP */
    } UCBBG;


typedef struct _atb {
    unsigned int atb$l_atb;		/* ATB address		*/
/* Overlay the ATB$W_CTI_CUR_SLOTS structure */
    char atb$$_fill_1 [12];
    short int atb$w_cti_tot_slots;	/* PORT total slots  16	*/
    short int atb$w_cti_cur_slots;	/* PORT used slots   18	*/
    char atb$$_fill_2 [8];
    unsigned int atb$a_cti_next_table;	/* Next ATB table    28 */
    unsigned int atb$a_cti_slots;	/* PORT slot address 32 */
    } ATB;


typedef struct _port_slot {
    unsigned int port_slot$l_port;	/* Port Address in ATB  0 */
    } PORT_SLOT;

typedef struct _port {
    unsigned int port$l_flink;		/* Port Flink	    0	*/
    unsigned int port$l_blink;		/* Port Flink	    4	*/
    short int port$w_size;		/* Size		    8	*/
    char port$b_type;			/* Type		   10	*/
    char port$b_subtype;		/* Subtype	   12	*/
    char port$$_fill_0 [80];
    unsigned int port$l_tpu_connection_id;  /* SCL port ID 92   */
    char port$$_fill_1 [76];
    unsigned int port$a_id_list;	/* Port Id list   172	*/
    unsigned int port$a_eib_emaa;	/* Port EIB addr  174   */
    char port$$_fill_2 [8];
    unsigned int port$l_nsp_fl;		/* NSP PORT Flink 188	*/
    unsigned int port$l_nsp_bl;		/* NSP PORT Blink 192	*/
    char port$$_fill_3 [20];
    unsigned int port$l_osi_fl;		/* OSI PORT Flink 216	*/
    unsigned int port$l_osi_bl;		/* OSI PORT Blink 220	*/
    char port$$_fill_4 [32];
    unsigned int port$a_constemplatenamelist;
					/* NSP Port #	256	*/
    char port$$_fill_5 [84];
    unsigned int port$l_nsp_vcrp_fl;	/* NSP VCRP address 344	*/
    unsigned int port$l_nsp_vcrp_bl;	/* NSP VCRP address 348	*/

/* Overlay the OSI PORT structure at PORT$T_LI + 8		*/
    char port$$_fill_6 [88];
    unsigned int port$l_vcrp_fl;	/* OSI VCRP address 440	*/
    unsigned int port$l_vcrp_bl;	/* OSI VCRP address 444	*/
    } PORT;


typedef struct _eib {
    char eib$$_fill_1 [176];
    unsigned int eib$l_link_ringlatency;	/* Remote/local reference */
    } EIB;


typedef struct _idlist {
/* Overlay the ATB$W_CTI_CUR_SLOTS structure */
    char idlist$$_fill_1 [45];
    unsigned char idlist$b_port_name_len;	/* PORT name length  45	*/
    char idlist$t_port_name[20];		/* PORT name 	     46	*/
    } IDLIST;


typedef struct _itmlist {
        short  nambuflen;
        short  jpicode;
        void  *nambuf;
        int   *retbuflen;
} itmlist_t;


struct {
        short  buflen;
        short  jpicode;
        int    scanbuf;
        int    flags;
        int    terminator;
} itmlst_pscan;


	struct eobjrecdef *eobjrec;	/* Object record	*/
	struct egsdef     *egs;		/* Global symbols	*/
	struct egstdef    *egst;	/* Universal symbols	*/

	struct FAB fab;
	struct RAB rab;

	extern int EXE_STD$CVT_EPID_TO_PCB();
	extern int EXE_STD$CVT_EPID_TO_IPID();

	extern int SYS$ASSIGN();
	extern int SYS$CMKRNL();
	extern int SYS$CLOSE();
	extern int SYS$CONNECT();
	extern int SYS$DASSGN();
	extern int SYS$FAO();
	extern int SYS$GET();
	extern int SYS$GETJPIW();
	extern int SYS$LKWSET();
	extern int SYS$OPEN();
	extern int SYS$PROCESS_SCAN();
	extern int SYS$QIOW();

	extern int EXE$READ_PROCESS();		/* Read other process data  */

	extern int CTL$AG_CLIMAGE;		/* Address of CLI image	    */
	extern int CTL$GT_CLINAME;		/* CLI image name	    */
	extern int CTL$GL_CHINDX;		/* Max assigned chnl number */
	extern int CTL$GA_CCB_TABLE;		/* CCB address		    */

	extern int EXE$GL_DECNET_VERSION;	/* DECnet version	    */

	extern int IAC$GL_IMAGE_LIST;		/* Address of list head	    */
	extern int IOC$GL_DEVLIST;		/* DDB list head	    */
	extern int LDR$GQ_IMAGE_LIST;		/* Address of image list    */

	extern int SCH$GL_MAXPIX;		/* Max process index	    */
	extern int SCH$GL_SWPPID;		/* Swapper PID		    */

	extern int SCS$AR_LOCALSB;		/* Local system block 	    */

	extern int MMG$GQ_NEXT_FREE_S0S1_VA;	/* Highest system VA        */

	extern unsigned long exe$gl_abstim_tics;
	extern SMP smp$gl_flags; /* System-wide multiprocessing control flags */



/* *** Lock down the fields accessed from kernel mode and all code *** */

   void last_address ();

   unsigned int lock_code_addr[2];
   unsigned int lock_data_addr[2];

   int LOCK_D_START;		/* Dummy for data lock start */

   char cmdline[10];		/* Input line for PID 	*/
   char	ceb_name2[17];		/* EFN cluster 2 name	*/
   char	ceb_name3[17];		/* EFN cluster 3 name	*/
   char file_name[100];		/* File name		*/
   char	pcb_lname[17];		/* Process name		*/
   char	pcb_terminal[9];	/* Terminal name	*/
   char image_name[40];		/* Image name		*/
   char jib_username[13];	/* User name		*/
   char port_name[20];		/* OSI/NSP Port name	*/
   char	unit_string[5];		/* Unit # string 	*/
   char	tmp_string[20];		/* Temporary string 	*/
   char disk[100];		/* Disk name		*/
   char inbuf[BUFFER_LENGTH];	/* .STB file read buffer */
   char ddb_name[100];		/* Device name		*/
   char ddb_bg0[10];		/* BG Device name       */

   char	pcb_ast_k,pcb_aste_k,pcb_asta_k;
   char	pcb_ast_e,pcb_aste_e,pcb_asta_e;
   char	pcb_ast_s,pcb_aste_s,pcb_asta_s;
   char	pcb_ast_u,pcb_aste_u,pcb_asta_u;

   char busy[]  = {"    "};
   char busy0[] = {"    "};
   char busy1[] = {"Busy"};

   char fao_str[] = {"!UL"};

   char *filename[] = {
	"SYS$COMMON:[SYS$LDR]NET$TRANSPORT_OSI.STB",
	"SYS$COMMON:[SYS$LDR]NET$TRANSPORT_NSP.STB"};

   char *net_image_name[] = {
                "NET$TRANSPORT_OSI",
                "NET$TRANSPORT_NSP"};

   char *atb_name[] = {
	"OSITP$GA_ATB",
	"TP_NSP$GA_NSP_ATB"};

   char *modes[] = {
	"Kernel    ",
	"Executive ",
	"Supervisor",
	"User      "};

   char *pd_kind[] = {
	"Bound Procedure Descriptor",
	"Unknown Procedure Descriptor",
	"Unknown Procedure Descriptor",
	"Unknown Procedure Descriptor",
	"Unknown Procedure Descriptor",
	"Unknown Procedure Descriptor",
	"Unknown Procedure Descriptor",
	"Unknown Procedure Descriptor",
	"Null Frame Procedure Descriptor",
	"Stack Frame Procedure Descriptor",
	"Register Frame Procedure Descriptor"};


   char *psects[] = {
	"Address is not within a system or user image",
	"MAIN",
	"Merged",
	"GLOBAL",
	"Number 4",
	"Nonpaged read only",
	"Nonpaged read/write",
	"Paged read only",
	"Paged read/write",
	"Activated Image",
	"System Resident Code",
	"Shareable Address Data",
	"Shareable Read-Only Data",
	"Read-Write Data",
	"Demand Zero Data"};

   char *states[] = {
	"COLPG               Collided page wait",
	"MUTEX               Mutex/resource wait",
	"CEF                 Common event flag wait",
	"PFW                 Page fault wait",
	"LEF                 Local event flag wait",
	"LEFO                Local event flag wait, Outswapped",
	"HIB                 Hibernate wait",
	"HIBO                Hibernate wait, Outswapped",
	"SUSP                Suspended",
	"SUSPO               Suspended, Outswapped",
	"FPG                 Freepage wait",
	"COM                 Compute",
	"COMO                Compute, Outswapped",
	"CUR                 Current process"};

   char *process_status[] = {
	"RES       Resident, in balance set",
	"DELPEN    Delete pending",
	"FORCPEN   Force exit pending",
	"INQUAN    Initial quantum in progress",
	"PSWAPM    Process cannot be swapped",
	"RESPEN    Resume pending, skip suspend",
	"SSFEXC    System service exception enable (K)",
	"SSFEXCE   System service exception enable (E)",
	"SSFEXCS   System service exception enable (S)",
	"SSFEXCU   System service exception enable (U)",
	"SSRWAIT   System service resource wait disable",
	"SUSPEN    Suspend pending",
	"WAKEPEN   Wake pending, skip hibernate",
	"WALL      Wait for all events in mask",
	"BATCH     Process is a batch job",
	"NOACNT    No accounting for process",
	"NOSUSPEND Process cannot be suspended",
	"ASTPEN    AST pending",
	"PHDRES    Process header resident",
	"HIBER     Hibernate after initial image activate",
	"LOGIN     Login without reading UAF",
	"NETWRK    Network connect job",
	"PWRAST    Power fail AST",
	"NODELET   No delete",
	"DISAWS    Disable automatic WS adjustment",
	"INTER     Process is an interactive job",
	"RECOVER   Process can recover locks",
	"SECAUDIT  Mandatory security auditing enabled",
	"HARDAFF   Process is bound to particular CPU",
	"ERDACT    Exec mode rundown active",
	"SOFTSUSP  Process is in soft suspend",
	"PREEMPTED Hard suspend has preempted soft",
	};

   char *resource_wait[] = {
	"AST event/channel interlock",
	"Mailbox space",
	"Non-paged dynmic memory",
	"Paging file space",
	"Paged dynamic memory",
	"Terminal broadcast",
	"Image activation interlock",
	"Job pooled quota",
	"Lock ID's",
	"Swapping file space",
	"Modified page list empty",
	"Modified page writer busy",
	"System communication",
	"Cluster state transition",
	"CPU capability",
	"Cluster server",
	"Snapshot",
	"POSIX fork",
	"Inner mode access for Kthreads",
	"Exit handler for Kthread"};

   char *m_wait[] = {
	"RWAST               Wait for AST completion",
	"RWMBX               Mailbox full",
	"RWNPG               Wait for nonpaged dynamic memory",
	"RWPFF               Paging file is full",
	"RWPAG               Wait for paged dynamic memory",
	"RWBRK               Wait for breakthrough",
	"RWIMG               Wait for image activation lock",
	"RWQUO               Wait for job pooled quota",
	"RWLCK               Wait for lock identification database",
	"RWSWP               Wait for swap file space",
	"RWMPE               Modified page list empty",
	"RWMPB               Modified page list busy",
	"RWSCS               Wait for system communications",
	"RWCLU               Wait for cluster state transition",
	"RWCAP               Wait for CPU capability",
	"RWCSV               Wait for cluster server",
	"RWSNP               Wait for a system snapshot",
	"PSXFR               Posix fork wait",
	"INNER_MODE          Thread in an inner mode wait",
	"EXH                 Thread in exit handling wait"};


   char *iofun[] = {
	"IO$_NOP",
	"IO$_UNLOAD, IO$_LOADMCODE, IO$_START_BUS",
	"IO$_SEEK, IO$_SPACEFILE, IO$_STARTMPROC, IO$_STOP_BUS",
	"IO$_RECAL, IO$_DUPLEX, IO$_STOP, IO$_DEF_COMP",
	"IO$_DRVCLR, IO$_INITIALIZE, IO$_MIMIC, IO$_DEF_COMP_LIST",
	"IO$_RELEASE, IO$_SETCLOCKP, IO$_START_ANALYSIS",
	"IO$_OFFSET, IO$_ERASETAPE, IO$_STARTDATAP, IO$_STOP_ANALYSIS",
	"IO$_RETCENTER, IO$_QSTOP, IO$_START_MONITOR",
	"IO$_PACKACK, IO$_STOP_MONITOR",
	"IO$_SEARCH, IO$_SPACERECORD, IO$_READRCT",
	"IO$_WRITECHECK",
	"IO$_WRITEPBLK",
	"IO$_READPBLK",
	"IO$_WRITEHEAD, IO$_RDSTATS, IO$_CRESHAD",
	"IO$_READHEAD, IO$_ADDSHAD",
	"IO$_WRITETRACKD, IO$_COPYSHAD",
	"IO$_READTRACKD, IO$_REMSHAD",
	"IO$_AVAILABLE",
	"IO$_SETPRFPATH",
	"IO$_DISPLAY",
	"IO$_REMSHADMBR",
	"IO$_DSE",
	"IO$_REREADN, IO$_DISK_COPY_DATA",
	"IO$_MOUNTSHAD, IO$_REREADP, IO$_WHM, IO$_AS_SETCHAR",
	"IO$_WRITERET, IO$_WRITECHECKH, IO$_AS_SENSECHAR",
	"IO$_ADDSHADMBR, IO$_READPRESET, IO$_STARTSPNDL",
	"IO$_SETCHAR",
	"IO$_SENSECHAR",
	"IO$_WRITEMARK, IO$_COPYMEM, IO$_PSXSETCHAR",
	"IO$_WRTTMKR, IO$_DIAGNOSE, IO$_SHADMV, IO$_PSXSENSECHAR",
	"IO$_FORMAT, IO$_CLEAN, IO$_UPSHAD",
	"IO$_PHYSICAL",
	"IO$_WRITELBLK (or IO$_WRITEVBLK)",
	"IO$_READLBLK (or IO$_READVBLK)",
	"IO$_REWINDOFF, IO$_READRCTL",
	"IO$_SETMODE",
	"IO$_REWIND",
	"IO$_SKIPFILE, IO$_PSXSETMODE",
	"IO$_SKIPRECORD, IO$_PSXSENSEMODE",
	"IO$_SENSEMODE",
	"IO$_WRITEOF, IO$_TTY_PORT_BUFIO",
	"IO$_TTY_PORT, IO$_FREECAP",
	"IO$_FLUSH, IO$_AS_SETMODE",
	"IO$_READLCHUNK, IO$_AS_SENSEMODE",
	"IO$_WRITELCHUNK",
	"45 Not used",
	"46 Not used",
	"IO$_LOGICAL",
	"IO$_WRITEVBLK",
	"IO$_READVBLK",
	"IO$_ACCESS, IO$_PSXWRITEVBLK",
	"IO$_CREATE",
	"IO$_DEACCESS, IO$_PSXREADVBLK",
	"IO$_DELETE",
	"IO$_MODIFY, IO$_NETCONTROL",
	"IO$_READPROMPT, IO$_SETCLOCK, IO$_AUDIO",
	"IO$_ACPCONTROL, IO$_STARTDATA, IO$_IOCTLV",
	"IO$_MOUNT",
	"IO$_TTYREADALL, IO$_DISMOUNT",
	"IO$_TTYREADPALL",
	"IO$_CONINTREAD",
	"IO$_CONINTWRITE",
	"IO$_READDIR"};


  char  *str_ptr;
  char  *str_ptr2;

  char  outname[] = "12345678.OUT";	/* Pid is 8 chars	*/
  FILE *outfile;

  char  prcname[16];
  int   prclen;
  unsigned int tmp_pid[1];


const long int bit_mask[33] = {				/* Bits set */
	0X0,0X1,    0X2,            0X4,
	0X8, 0X10,   0X20,           0X40,
	0X80, 0X100,   0X200,         0X400,
	0X800, 0X1000,   0X2000,       0X4000,
	0X8000, 0X10000,  0X20000,      0X40000,
	0X80000, 0X100000,  0X200000,    0X400000,
	0X800000, 0X1000000,  0X2000000,  0X4000000,
	0X8000000, 0X10000000, 0X20000000, 0X40000000,
	0X80000000
	};

   itmlist_t itmlst[2]={
		sizeof(prcname)-1, JPI$_PRCNAM, prcname, &prclen,
		0, 0, 0, 0};

   itmlist_t itmlst_2[3]={
		sizeof(prcname)-1, JPI$_PRCNAM, prcname, &prclen,
		4, JPI$_PID, tmp_pid, 0,
		0, 0, 0, 0};


   int  cli_length  = 8;
   int  link_length = 4;
   int	arglist [3];
   int  cmp_len[2];
   int	cmdlen;
   int 	orig_ipl;
   int	return_status;
   int	size;
   int	sym_val;
   int  str_length;
   int  ucb_address;
   int  ucb_devchar;
   int  iocou;
   short int ucb_devtype;
   int  ucb_sts;
   int  unit;
   int  bufquo;

   int t_delta;
   int t_days;
   int t_hours;
   int t_minutes;
   int t_seconds;

   int read_process_ast_count;
   short int prc_index;
   int ctl_chindx;
   int ctl_chindx_length;
   int ccb_length;
   int ccb_table_address;
   int ccb_table[6400];		/* CCB table 200 entries	*/
   int ddb_name_length;		/* Device name length		*/
   int window;

   int PSTbase;
   int section;

   int kferes_link;
   int kferes_offset;
   int kferes_count;
   int kferes_block[KFERES$K_SECTION_LENGTH];

   int prc_registers[64];		/* Process registers		*/

   int found;
   int offset;
   int psect;
   int start;
   int end;

   int prvmod;
   int curmod;
   int ipl;
   int frame[32800];			/* Max RSA_OFFSET + PDSC size	*/
   int frame_count;
   int save_fp;
   int save_ra;
   int fp;
   int next_fp;
   int r26;
   int pd;
   int pdsc_b[8];			/* Procedure Descriptor (PDSC)	*/
   int pdsc_flags;
   int pdsc_kind;
   int pdsc_entry;
   short int pdsc_rsa_offset;
   unsigned int ireg_mask;

   int cli[2];
   int imcb_link;
   int imcb_block[IMCB$K_LENGTH+1];
   int wcb_block[WCB$K_LENGTH+1];
   int fcb_block[FCB$K_LENGTH+1];

   IRP *ucb_ioqfl_a;
   IRP *ucb_ioqfl;

   IRP *pcb_irpfl_a;
   IRP *pcb_irpfl;

   VCRP *vcrp_qfl_a;
   VCRP *vcrp_qfl;

   BUFIO *ucb_bufiofl_a;
   BUFIO *ucb_bufiofl;

   int irp_count;
   int irp_addr[MAX_IRP];
   int irp_efn[MAX_IRP];
   int irp_func[MAX_IRP];
   int irp_iost1[MAX_IRP];
   uint64 irp_iosb_a[MAX_IRP];
   uint64 irp_iosb[MAX_IRP];
   int irp_bcnt[MAX_IRP];
   uint64 irp_ast[MAX_IRP];
   uint64 irp_astprm[MAX_IRP];
   int irp_p1[MAX_IRP];
   int irp_p2[MAX_IRP];
   int irp_p3[MAX_IRP];
   int irp_p4[MAX_IRP];
   int irp_p5[MAX_IRP];
   int irp_p6[MAX_IRP];
   int irp_pid[MAX_IRP];
   int irp_epid[MAX_IRP];
   int irp_unit[MAX_IRP];

   unsigned int i_efwm;
   unsigned int i_wefc;
   unsigned int	target_pcb;	/* Process PCB */
   unsigned int	pid;		/* Process PID */

/*	Thread fields	*/

   unsigned int	pcb_state[MAX_THREADS];
   unsigned int	pcb_sts[MAX_THREADS];
            int	pcb_efwm[MAX_THREADS];	/* Signed int ! */
   unsigned int	pcb_wefc[MAX_THREADS];
   unsigned int	pcb_cpu_id[MAX_THREADS];
   unsigned int	pcb_efcs[MAX_THREADS];
   unsigned int	pcb_efcu[MAX_THREADS];
   unsigned int	pcb_efc2p[MAX_THREADS];
   unsigned int	pcb_efc3p[MAX_THREADS];



/*	PCB	*/

   unsigned int	pcb_epid;
   unsigned int	epid;
   unsigned int	pcb_pid;
   unsigned int	pcb_pri;
   unsigned int	pcb_prib;
   unsigned int	pcb_owner;
   unsigned int owner_epid;
   unsigned int	pcb_waitime;
   unsigned int	pcb_diocnt;
   unsigned int	pcb_diolm;
   unsigned int	pcb_biocnt;
   unsigned int	pcb_biolm;
   unsigned int	pcb_prccnt;
   unsigned int	pcb_dpc;
   unsigned int	pcb_astcnt;
   unsigned int	pcb_astact;
   unsigned int	pcb_phd;
   unsigned int pcb_wsquota;
   unsigned int	pcb_ktbvec_a[MAX_THREADS];
   unsigned int	pcb_kt_count;
   int	*pcb_ktbvec;

   unsigned int wsextent;

   int phd_pstbasoff;     
   unsigned int	phd_astlm;
   uint64 phd_astsr_asten;

   unsigned int jib_flags;
   unsigned int jib_bytcnt;
   unsigned int jib_bytlm;
   unsigned int jib_org_bytlm;
   unsigned int jib_filcnt;
   unsigned int jib_fillm;
   unsigned int jib_mpid;
   unsigned int master_epid;
   unsigned int jib_tqcnt;
   unsigned int jib_tqlm;
   unsigned int jib_pgflcnt;
   unsigned int jib_pgflquota;
   unsigned int jib_prclim;
   unsigned int	jib_prccnt;

   unsigned int abstim_tics;
   unsigned int mmg_fresva;	/* First free system VA	*/

   $DESCRIPTOR (cmdline_d, cmdline);
   $DESCRIPTOR (disk_d, disk);
   $DESCRIPTOR (fao_d, fao_str);
   $DESCRIPTOR (file_d, file_name);
   $DESCRIPTOR (str_d, unit_string);


   static $DESCRIPTOR (prompt_pid, "Please give target process PID : ");

   int get_fp_and_pdsc();
   int get_process_info();
   int get_process_channels();
   int get_device_name();
   int get_process_registers();
   int map_address();
   int read_stb_file();
   int get_symbol_offset();

   int  OSITP$GA_ATB;
   int  TP_NSP$GA_NSP_ATB;

   int  i;
   int  j;
   int  k;
   int  l;
   int  m;
   int  i_bit;
   int  threads;
   int  threads_ind;

   int  nsp_port;
   int  atb_end;
   int  atb_slots;
   int  port_count;
   int  net_port;
   int  scl;

   PORT *port_fl_a;
   PORT *port_fl;
   int  next_port;

   int  tmp;
   int  *tmp_ptr;


   ATB    *atb;
   BUFIO  *bufio;
   CCB 	  *ccb;
   CEB 	  *ceb;
   CXB    *cxb;
   DDB 	  *ddb;
   EIB 	  *eib;
   FCB 	  *fcb;
   IDLIST *idlist;
   IMCB   *imcb;
   IRP 	  *irp;
   JIB 	  *jib;
   KFERES_SECTION *kferes_section;
   KFERES *kferes;
   LDRIMG *ldrimg;
   PCB 	  *pcb;
   PHD 	  *phd;
   PORT   *port;
   PORT_SLOT *port_slot;
   SB  	  *sb;
   UCB 	  *ucb;
   UCB    *ucb_bg0;               /* BG0: UCB address     */
   UCB    *ucb_2;
   UCBBG  *ucbbg;
   UCBNI  *ucbni;
   VCRP	  *vcrp;
   MB_UCB *mb_ucb;
   WCB 	  *wcb;

   struct pdscdef *pdsc;

/*VMS70   SECDEF *sec;	*/


/* ******** Temp stuff ************* */

   unsigned int	pcb_tmp;
   unsigned int	ceb_tmp;
   unsigned int	ceb1_tmp;

   int  tmp2;
   int  tmp3;
   int  tmp4;
   int  tmp5;
   int  tmp6;
   int  tmp7;
   int  tmp8;
   int  tmp9;
   int  tmp10;
   int  tmp11;
   int  tmp12;
   int  tmp13;
   int  tmp14;
   int  tmp15;
   int  tmp16;
   int  tmp17;
   int  tmp20;
   int  tmp21;
   int  tmp22;
   int  tmp23;
   int  tmp24;
   int  tmp25;
   int  tmp26;
   int  tmp27;
   int  tmp28;
   int  tmp29;

   int  tempa[500];
   int  tempb[500];
   int  tempc[500];

   char tempstr[100];

/* ******** Temp stuff ************* */


   int LOCK_D_END;		/* End of data to lock */

/* ******************************************************************* */

main()
{
   ucb_bg0 = 0;			/* No BG0: UCB address yet    */

   printf ("\n\n          *** MWAIT /Alpha V2.8 - Process Hang Analyzer ***\n\n");

/*	cmdline_d.dsc$w_length  = 9;			*/
/*	cmdline_d.dsc$b_dtype   = DSC$K_DTYPE_T;	*/
/*	cmdline_d.dsc$b_class   = DSC$K_CLASS_D;	*/
/*	cmdline_d.dsc$a_pointer = cmdline;		*/
   
   return_status = LIB$GET_FOREIGN (&cmdline_d,0,&cmdlen,0);
   
/*	printf ("Foreign Command : %s\n",cmdline_d.dsc$a_pointer);	*/

   
   
     if (cmdlen == 0) {
     cmdline_d.dsc$w_length  = PID_SIZE;
     return_status = LIB$GET_INPUT ( &cmdline_d,
				     &prompt_pid, &cmdlen );
     } 
   
/*	printf ("Command line    : %s\n",cmdline);		*/

   
/*	Lock data	*/
   
   lock_data_addr[0] = (int) &LOCK_D_START;	/* Start of data to lock */
   lock_data_addr[1] = (int) &LOCK_D_END;	/* End of data to lock   */
   
   return_status = SYS$LKWSET (&lock_data_addr,0,0);
						/* Lock code and data */
   if (return_status != SS$_NORMAL) return (return_status);
      
   
/*	Lock code	*/
   
   pdsc = (struct pdscdef *) &main;	 	/* Main PDSC		  */
   lock_code_addr[0] = pdsc->pdsc$l_entry;	/* Start of code to lock */
   
   pdsc = (struct pdscdef *) &last_address; 	/* last_address PDSC	  */
   lock_code_addr[1] = pdsc->pdsc$l_entry;	/* End of code to lock   */
   
   return_status = SYS$LKWSET (&lock_code_addr,0,0);
						/* Lock code and data */
   if (return_status != SS$_NORMAL) return (return_status);
      
/* \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ */

/*	There is (?) no global pointer to OSITP$GA_ATB and 	*/
/* 	TP_NSP$GA_NSP_ATB. 					*/
/*	Read these values from .STB files and get the image 	*/
/*	offsets from the loaded image list.			*/

   return_status = read_stb_file ( filename[0], atb_name[0] );
						/* Get symbol value */
   arglist [0] = 1;
   arglist [1] = (int) net_image_name[0];

   return_status = SYS$CMKRNL(&get_symbol_offset, arglist);
						/* Get symbol offset */

   OSITP$GA_ATB = sym_val + offset; 		/* Symbol value		*/


   return_status = read_stb_file ( filename[1], atb_name[1] );
						/* Get symbol value */
   arglist [0] = 1;
   arglist [1] = (int) net_image_name[1];

   return_status = SYS$CMKRNL(&get_symbol_offset, arglist);
						/* Get symbol offset */

   TP_NSP$GA_NSP_ATB = sym_val + offset;	/* Symbol value		*/


/* \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ */
   
   return_status = LIB$CVT_HTB ( cmdlen, cmdline_d.dsc$a_pointer, &pid);

   
/* 	Open output file	*/

   for (i = 0; i < 8; i++)
	outname[i] = cmdline[i];	/* Use PID as file name */

   printf ("\n *** The output will be written into file %s *** \n", (char *) outname);

   outfile = fopen (outname, "w");
   if (outfile == 0) 
	{
	printf ("\n Can't open output file %s \n",outname);
	return (SS$_NORMAL);
	}

   fprintf (outfile, "\n\n          *** MWAIT /Alpha V2.8 - Process Hang Analyzer ***\n\n");

   
   arglist [0] = 1;
   arglist [1] = (int) pid;
   
   return_status =	SYS$CMKRNL(&get_process_info, arglist);
   if (return_status != SS$_NORMAL) return (return_status);
      
   
   
/* ********************************************************************
 *
 *	Analyze process data and output the results.
 *
 ********************************************************************** */
   
   printf ("\nProcess name     : %-15s", pcb_lname); 
   fprintf (outfile, "\nProcess name     : %-15s", pcb_lname); 

   printf ("     User name      : %s\n", jib_username); 
   fprintf (outfile, "     User name      : %s\n", jib_username); 

   
   if ( pcb_terminal[0] == 0 )
     {
     if ( pcb_owner == 0)             strncpy(pcb_terminal,"-Detached-", 16);
     else			      strncpy(pcb_terminal,"-Subprocess-", 16);
     if ( pcb_sts[0] & PCB$M_BATCH )  strncpy(pcb_terminal,"-Batch-", 16);
     if ( pcb_sts[0] & PCB$M_NETWRK ) strncpy(pcb_terminal,"-Network-", 16);
     }
      
   printf ("Extended PID     : %08X            Internal PID   : %08X\n", 
      							pcb_epid,pcb_pid);
   fprintf (outfile, "Extended PID     : %08X            Internal PID   : %08X\n", 
      							pcb_epid,pcb_pid);
   
   printf ("PCB address      : %8X",pcb_ktbvec_a[0]);
   fprintf (outfile, "PCB address      : %8X",pcb_ktbvec_a[0]);

   printf ("            Terminal name  : %-15s\n", pcb_terminal);
   fprintf (outfile, "            Terminal name  : %-15s\n", pcb_terminal);

   
   if (pcb_owner != 0) 
     {
     return_status = SYS$GETJPIW (0, &master_epid, 0, &itmlst, 0, 0, 0);
     printf ("Master PID       : %08X            Master Process : %s\n",master_epid,prcname);
     fprintf (outfile, "Master PID       : %08X            Master Process : %s\n",master_epid,prcname);


     return_status = SYS$GETJPIW (0, &owner_epid, 0, &itmlst, 0, 0, 0);
     printf ("Owner PID        : %08X            Owner Process  : %s\n",owner_epid,prcname);
     fprintf (outfile, "Owner PID        : %08X            Owner Process  : %s\n",owner_epid,prcname);

     }

/*
 * All (Sub)processes have the same Subprocess count JIB$L_PRCCNT
 *
 * Master PID is the same all (Sub)processes JIB$L_MPID
 *
 * Master Creator PID = 0 JIB$L_MPID
 *
 * Other Creator PID's = PID of the Creator (Sub)process PCB$L_OWNER
 *
 */

/*	\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	*/

    if ( jib_prccnt != 0)
     {

     epid = 0;				/* Initial value	*/

/*	 PSCAN$_OWNER		scan sub-processes of process	*/
/*	 PSCAN$_MASTER_PID	scan sub-processes of master	*/

    itmlst_pscan.buflen     = 0;		/* Buffer length        */
    itmlst_pscan.jpicode    = PSCAN$_MASTER_PID; /* Item code        */
    itmlst_pscan.scanbuf    = master_epid;	/* Master PID		*/
    itmlst_pscan.flags      = 0;		/* Item specific flags  */
    itmlst_pscan.terminator = 0;		/* Terminator*/


     return_status = SYS$PROCESS_SCAN (&epid, &itmlst_pscan);

     printf ("\nProcesses in this job: \n");
     fprintf (outfile, "\nProcesses in this job: \n");


     tmp = 1;

     while (return_status != SS$_NOMOREPROC)
      {
       return_status = SYS$GETJPIW (0, &epid, 0, &itmlst_2, 0, 0, 0);
       if (return_status == SS$_NORMAL) 
         {
	  if ( pcb_epid != tmp_pid[0] )
	    {
            printf ("Process %2d  PID  : %08X            Process name   : %s\n",tmp, tmp_pid[0], prcname);
            fprintf (outfile, "Process %2d  PID  : %08X            Process name   : %s\n",tmp, tmp_pid[0], prcname);
	    }

	  else
	    {
            printf ("Process %2d  PID  : %08X    (*)     Process name   : %s\n",tmp, tmp_pid[0], prcname);
            fprintf (outfile, "Process %2d  PID  : %08X    (*)     Process name   : %s\n",tmp, tmp_pid[0], prcname);
	    }
         }

       tmp = tmp + 1;
      }
     printf ("\n");
     fprintf (outfile, "\n");


     } /* End if jib_prccnt */



      
   printf ("JIB address      : %8X",jib);
   fprintf (outfile, "JIB address      : %8X",jib);

   printf ("            Cur/Base prior : %d/%d\n",pcb_pri,pcb_prib);
   fprintf (outfile, "            Cur/Base prior : %d/%d\n",pcb_pri,pcb_prib);

   
   if (phd !=0 ) 
      {
      printf ("PHD address      : %X\n",phd);
      fprintf (outfile, "PHD address      : %X\n",phd);
      }
   else
      {
      printf ("PHD address      : Process is outswapped, PHD not resident\n");
      fprintf (outfile, "PHD address      : Process is outswapped, PHD not resident\n");
      }

      
   printf ("KTB vector       : %8X",pcb_ktbvec);
   fprintf (outfile, "KTB vector       : %8X",pcb_ktbvec);

   printf ("            Threads        : %d\n",pcb_kt_count);
   fprintf (outfile, "            Threads        : %d\n",pcb_kt_count);

   
   
   
/********************** Thread loop ***********************************/
   
   threads = pcb_kt_count;
   
   while (threads != 0)
     {
     
     threads_ind = pcb_kt_count - threads;
     
     printf ("Thread %02d\n",threads_ind);
     fprintf (outfile, "Thread %02d\n",threads_ind);

     printf ("--------- \n");
     fprintf (outfile, "--------- \n");

     
     printf ("KTB address      : %8X",pcb_ktbvec_a[threads_ind]);
     fprintf (outfile, "KTB address      : %8X",pcb_ktbvec_a[threads_ind]);

     printf ("            Running on CPU : %d\n",pcb_cpu_id[threads_ind]);
     fprintf (outfile, "            Running on CPU : %d\n",pcb_cpu_id[threads_ind]);

     
     
     if ( pcb_state[threads_ind] != SCH$C_MWAIT )
	{
        printf ("Thread state     : %s\n", states[pcb_state[threads_ind]-1]);
        fprintf (outfile, "Thread state     : %s\n", states[pcb_state[threads_ind]-1]);
	}

     else
       {
       if ( pcb_efwm[threads_ind] < RSN$_MAX && pcb_efwm[threads_ind] > 0)
	 {
          printf ("Thread state     : %s\n", m_wait[pcb_efwm[threads_ind]-1]);
          fprintf (outfile, "Thread state     : %s\n", m_wait[pcb_efwm[threads_ind]-1]);
	 }

       else
	  {
          printf ("Thread state     : %s\n", states[pcb_state[threads_ind]-1]);
          fprintf (outfile, "Thread state     : %s\n", states[pcb_state[threads_ind]-1]);
	  }

       }
     
     printf ("Thread status    : %08X  ",pcb_sts[threads_ind]);
     fprintf (outfile, "Thread status    : %08X  ",pcb_sts[threads_ind]);

     
     if ( !(pcb_sts[threads_ind] & PCB$M_SSRWAIT) ) 
	{
        printf ("ssrwait   System service resource wait enabled\n                             ");
        fprintf (outfile, "ssrwait   System service resource wait enabled\n                             ");
	}

     
/*	Print Process status bit names		*/
     
     tmp = 0;
     
     for (i_bit = 0; i_bit < 33; i_bit++)  
       {
       if (( pcb_sts[threads_ind] & bit_mask[i_bit+1] ) >> i_bit) 
         {
         tmp = tmp + 1;
         if ( tmp == 1 )
	    {
            printf ("%s ", process_status[i_bit]);
            fprintf (outfile, "%s ", process_status[i_bit]);
	    }

         else
	    {
            printf ("\n                             %s ", process_status[i_bit]);
            fprintf (outfile, "\n                             %s ", process_status[i_bit]);
	    }

         }
       }
     
     
     printf ("\nEFN wait cluster : %X\n",pcb_wefc[threads_ind]);
     fprintf (outfile, "\nEFN wait cluster : %X\n",pcb_wefc[threads_ind]);

     printf ("EFN wait mask    : %08X",pcb_efwm[threads_ind]);
     fprintf (outfile, "EFN wait mask    : %08X",pcb_efwm[threads_ind]);

     
     if ( pcb_state[threads_ind] != SCH$C_MWAIT ) 	/* If not MWAIT */
       {
       
       
/*	MMG$GL_FRESVA	first free system VA	*/
       
/*	if (pcb_efwm[threads_ind] < mmg_fresva && pcb_efwm[threads_ind] > 0X80000000 )


/*	EFN wait state	*/
       
       if ( pcb_state[threads_ind] == SCH$C_CEF || pcb_state[threads_ind] == SCH$C_LEF ||
          pcb_state[threads_ind] == SCH$C_LEFO) 	/* EFN wait 	 */
         {
         
         i_efwm = pcb_efwm[threads_ind] ^ 0XFFFFFFFF;	/* XOR ==> bit set for EFN's	 */
         i_wefc = 32 * pcb_wefc[threads_ind];		/* Base EFN for this EFN cluster */
         
/*	Print EFN wait mask bits as EFN numbers 	*/
         
         
/*	Special case for EFN$C_ENF, thread upcall support EFN 	*/
         
         if (pcb_wefc[threads_ind] < 4 )
           {
           printf ("  EFN's = ");
           fprintf (outfile, "  EFN's = ");

           
           for (i_bit = 0; i_bit < 33; i_bit++)  
             {
             if (( i_efwm & bit_mask[i_bit+1] ) >> i_bit) 
		{
                printf ("%d ", i_bit + i_wefc);
                fprintf (outfile, "%d ", i_bit + i_wefc);
		}
             }
           }
         else
	    {
            printf ("  EFN = 128, EFN$C_ENF for thread upcall support");
            fprintf (outfile, "  EFN = 128, EFN$C_ENF for thread upcall support");
	    }

         
         } /* End if ( pcb_state[threads_ind] == SCH$C_CEF || pcb_state[threads_ind] == SCH$C_LEF ||  */
       
       
       }
     else	/*	Resource wait state	*/
       {
       
/*	RSN wait state	*/
       
       if ( pcb_efwm[threads_ind] < RSN$_MAX && pcb_efwm[threads_ind] > 0)
	  {
          printf ("\033[31m  Process is waiting for %s \033[39m \n",resource_wait[pcb_efwm[threads_ind] - 1]);
          fprintf (outfile, "\033[31m  Process is waiting for %s \033[39m \n",resource_wait[pcb_efwm[threads_ind] - 1]);
	  }

       else
         {
         if ( pcb_efwm[threads_ind] == (int) jib )
           {
           if (jib_flags & JIB$M_BYTCNT_WAITERS) 
	      {
              printf ("  JIB address,\033[31m waiting on BYTCNT \033[39m");
              fprintf (outfile, "  JIB address,\033[31m waiting on BYTCNT \033[39m");
	      }

           if (jib_flags & JIB$M_TQCNT_WAITERS)
	      {
              printf ("  JIB address,\033[31m waiting on TQCNT \033[39m");
              fprintf (outfile, "  JIB address,\033[31m waiting on TQCNT \033[39m");
	      }

           }
         else
	    {
            printf ("  is the waiting address");
            fprintf (outfile, "  is the waiting address");
	    }

         }
       
       } /* End if ( pcb_state[threads_ind] != SCH$C_MWAIT ) 	*/
     
     
     
     
     printf ("\n");
     fprintf (outfile, "\n");

     
     if (pcb_efcs[threads_ind] != 0)  printf (" efcs        %X\n",pcb_efcs[threads_ind]);
     if (pcb_efcs[threads_ind] != 0)  fprintf (outfile, " efcs        %X\n",pcb_efcs[threads_ind]);

     if (pcb_efcu[threads_ind] != 0)  printf (" efcu        %X\n",pcb_efcu[threads_ind]);
     if (pcb_efcu[threads_ind] != 0)  fprintf (outfile, " efcu        %X\n",pcb_efcu[threads_ind]);

     
     if (pcb_efc2p[threads_ind] != 0)
       {
       printf ("EFC2P address    : %X\n",pcb_efc2p[threads_ind]);
       fprintf (outfile, "EFC2P address    : %X\n",pcb_efc2p[threads_ind]);

       printf ("EFN2 Clustername : %s\n", ceb_name2);
       fprintf (outfile, "EFN2 Clustername : %s\n", ceb_name2);

       }
     
     if (pcb_efc3p[threads_ind] != 0)
       {
       printf ("EFC3P address    : %X\n",pcb_efc3p[threads_ind]);
       fprintf (outfile, "EFC3P address    : %X\n",pcb_efc3p[threads_ind]);

       printf ("EFN3 Clustername : %s\n", ceb_name3);
       fprintf (outfile, "EFN3 Clustername : %s\n", ceb_name3);

       }
     
     threads = threads - 1;
     
     } /************ End of thread loop ***********/
   
/* ******************************************************************* */
   
   
   printf ("\nDirect   I/O count/limit    [IO's] : %8d /%8d",pcb_diocnt,pcb_diolm);
   fprintf (outfile, "\nDirect   I/O count/limit    [IO's] : %8d /%8d",pcb_diocnt,pcb_diolm);

   printf ("  In use : %d\n", pcb_diolm - pcb_diocnt);
   fprintf (outfile, "  In use : %d\n", pcb_diolm - pcb_diocnt);

   if ( pcb_diocnt == 0 )
      {
      printf("\033[31m   *** Process has run out of Direct I/O quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has run out of Direct I/O quota ***\033[39m \n");
      }

   
/* 
 * Print the previous in RED color (= ESC[31m" ), change then back to BLACK
 * 
 * Escape Sequences for Colors:
 * 
 * 	BLACK   = "ESC[30m"
 * 	RED     = "ESC[31m"
 * 	GREEN   = "ESC[32m"
 * 	YELLOW  = "ESC[33m"
 * 	BLUE    = "ESC[34m"
 * 	CYAN    = "ESC[35m"
 * 	LBLUE   = "ESC[36m"
 * 	WHITE   = "ESC[37m"
 * 
 *      RESET_FG = "ESC[39m"	Reset foreground color
 * 
 * 
 */
   
   
   
   printf ("Buffered I/O count/limit    [IO's] : %8d /%8d",pcb_biocnt,pcb_biolm);
   fprintf (outfile, "Buffered I/O count/limit    [IO's] : %8d /%8d",pcb_biocnt,pcb_biolm);

   printf ("  In use : %d\n", pcb_biolm - pcb_biocnt);
   fprintf (outfile, "  In use : %d\n", pcb_biolm - pcb_biocnt);

   
   
   if ( pcb_biocnt == 0 )
      {
      printf("\033[31m   *** Process has run out of Buffered I/O quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has run out of Buffered I/O quota ***\033[39m \n");
      }

   
   
   
   printf ("Sub-process count/limit    [Procs] : %8d /%8d",pcb_prccnt,jib_prclim);
   fprintf (outfile, "Sub-process count/limit    [Procs] : %8d /%8d",pcb_prccnt,jib_prclim);

   printf ("  In use : %d\n", pcb_prccnt);
   fprintf (outfile, "  In use : %d\n", pcb_prccnt);

   if ( pcb_prccnt == jib_prclim )
      {
      printf("\033[31m   *** Process has run out of Sub-process quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has run out of Sub-process quota ***\033[39m \n");
      }


   
   printf ("Byte count/limit           [Bytes] : %8d /%8d",jib_bytcnt,jib_bytlm);
   fprintf (outfile, "Byte count/limit           [Bytes] : %8d /%8d",jib_bytcnt,jib_bytlm);

   printf ("  In use : %d\n", jib_bytlm - jib_bytcnt);
   fprintf (outfile, "  In use : %d\n", jib_bytlm - jib_bytcnt);

   if ( jib_bytcnt == 0 )
      {
      printf("\033[31m   *** Process has run out of Byte count quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has run out of Byte count quota ***\033[39m \n");
      }

   
   printf ("Byte count/orig. limit     [Bytes] : %8d /%8d",jib_bytcnt,jib_org_bytlm);
   fprintf (outfile, "Byte count/orig. limit     [Bytes] : %8d /%8d",jib_bytcnt,jib_org_bytlm);

   printf ("  In use : %d\n", jib_org_bytlm - jib_bytcnt);
   fprintf (outfile, "  In use : %d\n", jib_org_bytlm - jib_bytcnt);

   
   printf ("File count/limit           [Files] : %8d /%8d",jib_filcnt,jib_fillm);
   fprintf (outfile, "File count/limit           [Files] : %8d /%8d",jib_filcnt,jib_fillm);

   printf ("  In use : %d\n", jib_fillm - jib_filcnt);
   fprintf (outfile, "  In use : %d\n", jib_fillm - jib_filcnt);

   if ( jib_filcnt == 0 )
      {
      printf("\033[31m   *** Process has run out of File limit quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has run out of File limit quota ***\033[39m \n");
      }

   
   printf ("Timer queue count/limit   [Timers] : %8d /%8d",jib_tqcnt,jib_tqlm);
   fprintf (outfile, "Timer queue count/limit   [Timers] : %8d /%8d",jib_tqcnt,jib_tqlm);

   printf ("  In use : %d\n", jib_tqlm - jib_tqcnt);
   fprintf (outfile, "  In use : %d\n", jib_tqlm - jib_tqcnt);

   if ( jib_tqcnt == 0 )
      {
      printf("\033[31m   *** Process has run out of Timer count quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has run out of Timer count quota ***\033[39m \n");
      }

   
   
   printf ("Working set quota/limit [Pagelets] : %8d /%8d",
				   wsextent - pcb_wsquota,wsextent);
   fprintf (outfile, "Working set quota/limit [Pagelets] : %8d /%8d",
				   wsextent - pcb_wsquota,wsextent);

   printf ("  In use : %d\n", pcb_wsquota);
   fprintf (outfile, "  In use : %d\n", pcb_wsquota);

   if ( wsextent - pcb_wsquota == 0 )
      {
      printf("\033[31m   *** Process has used all of Working set quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has used all of Working set quota ***\033[39m \n");
      }

   
   printf ("Page file quota/limit      [Pages] : %8d /%8d",jib_pgflcnt,jib_pgflquota);
   fprintf (outfile, "Page file quota/limit      [Pages] : %8d /%8d",jib_pgflcnt,jib_pgflquota);

   printf ("  In use : %d\n", jib_pgflquota - jib_pgflcnt);
   fprintf (outfile, "  In use : %d\n", jib_pgflquota - jib_pgflcnt);

   if ( jib_pgflcnt == 0 )
      {
      printf("\033[31m   *** Process has run out of Page file quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has run out of Page file quota ***\033[39m \n");
      }

   
   if ( phd_astlm != -1 )
     {
     printf ("AST count/limit            [AST's] : %8d /%8d",pcb_astcnt,phd_astlm);
     fprintf (outfile, "AST count/limit            [AST's] : %8d /%8d",pcb_astcnt,phd_astlm);

     printf ("  In use : %d\n", phd_astlm - pcb_astcnt);
     fprintf (outfile, "  In use : %d\n", phd_astlm - pcb_astcnt);

     }
   else
     {
     printf ("AST count/limit            [AST's] : %8d/     n.a. \n",pcb_astcnt);
     fprintf (outfile, "AST count/limit            [AST's] : %8d/     n.a. \n",pcb_astcnt);
     }
   if ( pcb_astcnt == 0 )
      {
      printf("\033[31m   *** Process has run out of AST quota ***\033[39m \n");
      fprintf(outfile, "\033[31m   *** Process has run out of AST quota ***\033[39m \n");
      }

   
   pcb_aste_k = '-';
   pcb_aste_e = '-';
   pcb_aste_s = '-';
   pcb_aste_u = '-';
   
   pcb_asta_k = '-';
   pcb_asta_e = '-';
   pcb_asta_s = '-';
   pcb_asta_u = '-';
   
   if (phd_astsr_asten != -1)
     {
     if ( phd_astsr_asten & PR$M_ASTEN_KEN ) pcb_aste_k  = 'K';
     if ( phd_astsr_asten & PR$M_ASTEN_EEN ) pcb_aste_e  = 'E';
     if ( phd_astsr_asten & PR$M_ASTEN_SEN ) pcb_aste_s  = 'S';
     if ( phd_astsr_asten & PR$M_ASTEN_UEN ) pcb_aste_u  = 'U';
     
     if (pcb_aste_k != 'K' || pcb_aste_e != 'E' || pcb_aste_s != 'S' || pcb_aste_u != 'U')
	{
        printf("\033[31m");			/* RED	*/
        fprintf(outfile, "\033[31m");			/* RED	*/
	}

        
     printf ("AST's enabled               [KESU] :     %1c%1c%1c%1c \033[39m \n",
		        pcb_aste_k,pcb_aste_e,pcb_aste_s,pcb_aste_u);
     fprintf (outfile, "AST's enabled               [KESU] :     %1c%1c%1c%1c \033[39m \n",
		        pcb_aste_k,pcb_aste_e,pcb_aste_s,pcb_aste_u);

     }
   else
     {
     printf ("AST's enabled               [KESU] : Process is outswapped \n");
     fprintf (outfile, "AST's enabled               [KESU] : Process is outswapped \n");
     }
   
   if ( pcb_astact & PR$M_ASTEN_KEN ) pcb_asta_k  = 'K';
   if ( pcb_astact & PR$M_ASTEN_EEN ) pcb_asta_e  = 'E';
   if ( pcb_astact & PR$M_ASTEN_SEN ) pcb_asta_s  = 'S';
   if ( pcb_astact & PR$M_ASTEN_UEN ) pcb_asta_u  = 'U';
   
   if (pcb_asta_k != '-' || pcb_asta_e != '-' || pcb_asta_s != '-' || pcb_asta_u != '-')
      {
      printf("\033[31m");			/* RED	*/
      fprintf(outfile, "\033[31m");			/* RED	*/
      }

      
   printf ("AST's active                [KESU] :     %1c%1c%1c%1c \033[39m \n",
		      pcb_asta_k,pcb_asta_e,pcb_asta_s,pcb_asta_u);
   fprintf (outfile, "AST's active                [KESU] :     %1c%1c%1c%1c \033[39m \n",
		      pcb_asta_k,pcb_asta_e,pcb_asta_s,pcb_asta_u);

   
   
   if (pcb_ast_k != '-' || pcb_ast_e != '-' || pcb_ast_s != '-' || pcb_ast_u != '-')
      {
      printf("\033[31m");			/* RED	*/
      fprintf(outfile, "\033[31m");			/* RED	*/
      }

   
   printf ("AST's queued                [KESU] :     %1c%1c%1c%1c \033[39m \n",
		pcb_ast_k,pcb_ast_e,pcb_ast_s,pcb_ast_u);
   fprintf (outfile, "AST's queued                [KESU] :     %1c%1c%1c%1c \033[39m \n",
		pcb_ast_k,pcb_ast_e,pcb_ast_s,pcb_ast_u);

   
   printf ("Delete pending count (XQP event)   : %8d\n",pcb_dpc);
   fprintf (outfile, "Delete pending count (XQP event)   : %8d\n",pcb_dpc);

   
   
   printf ("\nAbsolute/Last event/Delta time     : %08X / %08X / %08X [hex] ticks\n",
		abstim_tics, pcb_waitime, abstim_tics - pcb_waitime);
   fprintf (outfile, "\nAbsolute/Last event/Delta time     : %08X / %08X / %08X [hex] ticks\n",
		abstim_tics, pcb_waitime, abstim_tics - pcb_waitime);

   
/*	1 tick = 10 milliseconds,  100 ticks = 1 second		*/
   
   t_delta   = abstim_tics - pcb_waitime;
   
   t_days    = t_delta/100/60/60/24;		/* Days		*/
   t_delta   = t_delta - t_days * 100 * 60 * 60 * 24;
   
   t_hours   = t_delta/100/60/60;		/* Hours	*/
   t_delta   = t_delta - t_hours * 100 * 60 * 60; 
   
   t_minutes = t_delta/100/60;			/* Minutes	*/
   t_delta   = t_delta - t_minutes * 100 * 60;
   
   t_seconds = t_delta/100;			/* Seconds	*/
   t_delta   = t_delta - t_seconds * 100;	/* 10 Milliseconds	*/
   
   printf ("Time since last event :");
   fprintf (outfile, "Time since last event :");

   
   if (t_days    != 0 ) printf (" %d days", t_days);
   if (t_days    != 0 ) fprintf (outfile, " %d days", t_days);

   if (t_hours   != 0 ) printf (" %d hours",t_hours);
   if (t_hours   != 0 ) fprintf (outfile, " %d hours",t_hours);

   if (t_minutes != 0 ) printf (" %d minutes",t_minutes);
   if (t_minutes != 0 ) fprintf (outfile, " %d minutes",t_minutes);

   if (t_seconds != 0 ) printf (" %d seconds",t_seconds);
   if (t_seconds != 0 ) fprintf (outfile, " %d seconds",t_seconds);


   printf (" %d milliseconds\n",t_delta * 10);
   fprintf (outfile, " %d milliseconds\n",t_delta * 10);

      
   
/*	Get process channels	*/
      
   prc_index = (short int) pcb_pid;
   
   arglist [0] = 2;
   arglist [1] = (int) prc_index;	/*	Index	*/
   arglist [2] = (int) pcb_pid;		/*	IPID	*/
   
   return_status =	SYS$CMKRNL(&get_process_channels, arglist);
   if (return_status == SS$_TIMEOUT)
      {
      printf ("%%MWAIT-F-NORESPONSE, can not read target process channels\n");
      fprintf (outfile, "%%MWAIT-F-NORESPONSE, can not read target process channels\n");
      }

   if (return_status != SS$_NORMAL) return (return_status);
      
/*		        V6	 V7
 *		       ----	----
 *	ccb_table[0] = UCB	UCB
 *	ccb_table[1] = WIND	STS
 *	ccb_table[2] = STS	IOC
 *	ccb_table[3] = IOC	DIRP
 *	ccb_table[4] = AMOD	AMOD
 *	ccb_table[5] = DIRP	WIND
 *	ccb_table[6] = CHAN	CHAN
 *
*/
   printf ("\nProcess active channels: \n");
   fprintf (outfile, "\nProcess active channels: \n");

   
   printf ("\nChnl  Window  IOC Sts  Device/file accessed\n");
   fprintf (outfile, "\nChnl  Window  IOC Sts  Device/file accessed\n");

   printf ("----  ------  --- ---  --------------------\n");
   fprintf (outfile, "----  ------  --- ---  --------------------\n");

   
   for ( i = 0; i < ctl_chindx; i++)
     {
     
     j=8*i;					/* CCB index		*/
     ccb = (CCB *) &ccb_table[j];		/* CCB entry address	*/
     
     if ( (ccb->ccb$b_amod != 0) && ( ccb->ccb$l_ucb != 0) )
       {
       if (ccb->ccb$l_ioc != 0)
          strncpy(busy,busy1,4);
       else
          strncpy(busy,busy0,4);
       
       
       for (k=0; k<100; k++)
         {
         ddb_name[k]  = 0;
         tempstr[k]   = 0;
         file_name[k] = 0;
         }
       
       arglist [0] = 2;
       arglist [1] = (int) ccb->ccb$l_ucb;  	/* UCB		*/
       arglist [2] = (int) ccb->ccb$l_ioc;	/* IO count	*/
       
       window = (int) ccb->ccb$l_wind;		/* Window	*/

       irp_count = 0;				/* Clear IRP #	*/
       
       return_status =	SYS$CMKRNL(&get_device_name, arglist);
       if (return_status != SS$_NORMAL) return (return_status);
          
       
       for (k=0; k < 5; k++)
         {
         unit_string[k] = 0;
         }
       
       return_status = SYS$FAO (&fao_d, &size, &str_d, unit);
       if (return_status != SS$_NORMAL) return (return_status);
          
/*	printf ("Unit = %d %s size %d\n", unit, unit_string, size);	*/
/*	fprintf (outfile, "Unit = %d %s size %d\n", unit, unit_string, size);	*/

       
       strcat (ddb_name,unit_string);		/* Insert unit #	*/
       strcat (ddb_name,":");			/* Insert :		*/
       
/*	Filenames only for directory structured devices	*/
       
       if ( (ucb_devchar & DEV$M_DIR) && !( (ucb_sts & UCB$M_MNTVERIP) || (ucb_sts & UCB$M_MSCP_MNTVERIP) ))
			/* Directory structured and no mount verify */
         {
         
/*	Get file name	*/
         
         if ( (int)ccb->ccb$l_wind > 0 )
            ccb->ccb$l_wind = (WCB *) window; 	/* PST index ==> window */
         
         if (ccb->ccb$l_wind != 0 )
           {
           fcb = (FCB *) fcb_block;		/* FCB local address	*/
           if ( fcb->fcb$b_type != DYN$C_FCB ) break;	/* Not a FCB	*/
              
/*	Set up disk name descriptor	*/
           
           disk_d.dsc$b_class   = DSC$K_CLASS_S;
           disk_d.dsc$b_dtype   = DSC$K_DTYPE_T;
           disk_d.dsc$w_length  = strlen (ddb_name);
           disk_d.dsc$a_pointer = (char *) ddb_name;
           
           return_status = lib$fid_to_name (&disk_d, fcb->fcb$w_fid,
				           &file_d, &size, 0, &tmp);
           if (return_status != SS$_NORMAL) return (return_status);
              
           
           for (k=0; k<100; k++)
             {
             tempstr[k]   = 0;
             }
           
/*	printf(" filename : %s  size = %d status = %d ACPstatus = %d \n",
		file_name,size,return_status,tmp);			*/
           
           str_ptr = (char *) file_name;
           str_ptr2 = strstr (file_name, "[");
           tmp = str_ptr2 - str_ptr;
           
/*	printf(" string = %08X  position = %08X diff = %d\n",
		str_ptr, str_ptr2, tmp);				*/
           
           strncpy (tempstr, file_name + tmp, size - tmp);
           
/*	printf(" filex = %s\n", tempstr);				*/
           
           strcat (ddb_name,tempstr);		/* Insert file name	*/
           }
         } /* End if UCB = DEV$M_DIR	*/
       
       
       if (ccb->ccb$l_ioc != 0) printf("\033[31m");	/* RED	*/
       if (ccb->ccb$l_ioc != 0) fprintf(outfile, "\033[31m");	/* RED	*/

          
       if (ddb_name[0] == 0x4d && ddb_name[1] == 0x42 )
	  {
          printf ("%04X %08X %3d %4s %s (Buffered I/O Quota available: %d bytes)\n",
            ccb->ccb$l_chan, ccb->ccb$l_wind, ccb->ccb$l_ioc, busy,ddb_name,bufquo);
          fprintf (outfile, "%04X %08X %3d %4s %s (Buffered I/O Quota available: %d bytes)\n",
            ccb->ccb$l_chan, ccb->ccb$l_wind, ccb->ccb$l_ioc, busy,ddb_name,bufquo);
	  }

       else
	  {
          printf ("%04X %08X %3d %4s %s\n",
       	      ccb->ccb$l_chan, ccb->ccb$l_wind, ccb->ccb$l_ioc, busy,ddb_name);
          fprintf (outfile, "%04X %08X %3d %4s %s\n",
       	      ccb->ccb$l_chan, ccb->ccb$l_wind, ccb->ccb$l_ioc, busy,ddb_name);
	  }

       
       if (ccb->ccb$l_ioc != 0) printf("\033[39m");	/* BLACK */
       if (ccb->ccb$l_ioc != 0) fprintf(outfile, "\033[39m");	/* BLACK */

          

	if ( net_port != 0)
	  {
	  printf ("Session Control Port : SCL$PORT$%08X \n",scl);
	  fprintf (outfile, "Session Control Port : SCL$PORT$%08X \n",scl);
	  }

	if ( net_port == 1)
	  {
	  printf ("OSI Transport Port   : %s\n\n",port_name);
	  fprintf (outfile, "OSI Transport Port   : %s\n\n",port_name);
	  }

	if ( net_port == 2)
	  {
	  printf ("NSP Transport Port   : %s\n\n",port_name);
	  fprintf (outfile, "NSP Transport Port   : %s\n\n",port_name);
	  }


	/*	IRP			*/
       
/*	printf ("IRP_COUNT = %d\n",irp_count);		*/
/*	fprintf (outfile, "IRP_COUNT = %d\n",irp_count);		*/

       
       if ( irp_count != 0 )
         {
         for (k=0; k<irp_count; k++)
           {

/* INET device I/O packets are on CXB's at BG devices	*/

   if ( (ddb_name[0] == 'I') && (ddb_name[1] == 'N') &&
        (ddb_name[2] == 'E') && (ddb_name[3] == 'T') )
	  {
           printf ("       The CXB is on   BG%d:  \n",irp_unit[k]);
           fprintf (outfile, "       The CXB is on   BG%d:  \n",irp_unit[k]);
							/* BG unit number */
	  }

           
           printf ("       I/O-Packet %3d       Hex / Decimal\n",k+1);
           fprintf (outfile, "       I/O-Packet %3d       Hex / Decimal\n",k+1);

           printf ("       --------------  ------------------\n");
           fprintf (outfile, "       --------------  ------------------\n");

           

/*	Mailbox read/write from an other process	*/
           
           if (irp_pid[k] != pcb_pid)
             {
             return_status = SYS$GETJPIW (0, &irp_epid[k], 0, &itmlst, 0, 0, 0);
             
             printf ("       Internal PID  : %08X    \033[31m       Process name: \033[39m \n",irp_pid[k]);
             fprintf (outfile, "       Internal PID  : %08X    \033[31m       Process name: \033[39m \n",irp_pid[k]);

             printf ("       PID           : %08X    \033[31m       %s \033[39m \n",irp_epid[k],prcname);
             fprintf (outfile, "       PID           : %08X    \033[31m       %s \033[39m \n",irp_epid[k],prcname);

             }
           
           printf ("       IRP address   : %08X\n",irp_addr[k]);
           fprintf (outfile, "       IRP address   : %08X\n",irp_addr[k]);

           printf ("       EFN           : %08X  %08d\n",irp_efn[k],irp_efn[k]);
           fprintf (outfile, "       EFN           : %08X  %08d\n",irp_efn[k],irp_efn[k]);

           printf ("       FUNC          : %08X  %08d %s\n",irp_func[k],irp_func[k],iofun[(irp_func[k] & IO$_VIRTUAL)]);
           fprintf (outfile, "       FUNC          : %08X  %08d %s\n",irp_func[k],irp_func[k],iofun[(irp_func[k] & IO$_VIRTUAL)]);

           printf ("       IOST1         : %08X\n",irp_iost1[k]);
           fprintf (outfile, "       IOST1         : %08X\n",irp_iost1[k]);

           printf ("       IOSB address  : %08X\n",irp_iosb_a[k]);
           fprintf (outfile, "       IOSB address  : %08X\n",irp_iosb_a[k]);

           printf ("       IOSB = [hex]  : %08X.%08X\n",irp_iosb[k]);
           fprintf (outfile, "       IOSB = [hex]  : %08X.%08X\n",irp_iosb[k]);

           printf ("       BCNT          : %08X  %08d Bytes\n",irp_bcnt[k],irp_bcnt[k]);
           fprintf (outfile, "       BCNT          : %08X  %08d Bytes\n",irp_bcnt[k],irp_bcnt[k]);

           printf ("       AST address   : %08X ",irp_ast[k]);
           fprintf (outfile, "       AST address   : %08X ",irp_ast[k]);

           
           if (irp_ast[k] != 0)
             {

	     for (l=0; l < 40; l++)	image_name[l] = '\0';

             arglist [0] = 2;
             arglist [1] = irp_ast[k];		/* PC	*/
             arglist [2] = irp_pid[k];		/* IPID	*/
             
             return_status =	SYS$CMKRNL(&map_address, arglist);
             if (return_status == SS$_TIMEOUT)
		{
                printf ("%%MWAIT-F-NORESPONSE, can not read target process image list\n");
                fprintf (outfile, "%%MWAIT-F-NORESPONSE, can not read target process image list\n");
		}

             if (return_status != SS$_NORMAL) return (return_status);
                
             if (found == 1)
               {
               printf ("          is in the image: \n\n%s\n", image_name);
               fprintf (outfile, "          is in the image: \n\n%s\n", image_name);

               
               printf ("Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n\n",
			               start,end,offset,psects[psect]);
               fprintf (outfile, "Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n\n",
			               start,end,offset,psects[psect]);

               }
             else
		{
                printf ("          is not within a system or user image\n");
                fprintf (outfile, "          is not within a system or user image\n");
		}

             
             } /* End irp_ast[k] != 0 */
           else
	      {
              printf ("\n");			/* <CR>	*/
              fprintf (outfile, "\n");			/* <CR>	*/
	      }

           
           
           printf ("       AST parameter : %08X  %08d\n",irp_astprm[k],irp_astprm[k]);
           fprintf (outfile, "       AST parameter : %08X  %08d\n",irp_astprm[k],irp_astprm[k]);

           printf ("       P1            : %08X  %08d\n",irp_p1[k],irp_p1[k]);
           fprintf (outfile, "       P1            : %08X  %08d\n",irp_p1[k],irp_p1[k]);

           printf ("       P2            : %08X  %08d\n",irp_p2[k],irp_p2[k]);
           fprintf (outfile, "       P2            : %08X  %08d\n",irp_p2[k],irp_p2[k]);

           printf ("       P3            : %08X  %08d\n",irp_p3[k],irp_p3[k]);
           fprintf (outfile, "       P3            : %08X  %08d\n",irp_p3[k],irp_p3[k]);

           printf ("       P4            : %08X  %08d\n",irp_p4[k],irp_p4[k]);
           fprintf (outfile, "       P4            : %08X  %08d\n",irp_p4[k],irp_p4[k]);

           printf ("       P5            : %08X  %08d\n",irp_p5[k],irp_p5[k]);
           fprintf (outfile, "       P5            : %08X  %08d\n",irp_p5[k],irp_p5[k]);

           printf ("       P6            : %08X  %08d\n\n",irp_p6[k],irp_p6[k]);
           fprintf (outfile, "       P6            : %08X  %08d\n\n",irp_p6[k],irp_p6[k]);

           
           
           } /* 	 */
         } /* 	 */
       } /* 	 */
     } /* 	 */
   
/* ************************************************************************ */
   
   arglist [0] = 1;
   arglist [1] = (int) pcb_pid;			/* IPID	*/
   
   return_status =	SYS$CMKRNL(&get_process_registers, arglist);
   if (return_status == SS$_TIMEOUT)
      {
      printf ("%%MWAIT-F-NORESPONSE, can not read target process registers\n");
      fprintf (outfile, "%%MWAIT-F-NORESPONSE, can not read target process registers\n");
      }	

   if (return_status != SS$_NORMAL) return (return_status);
      
   printf ("\nCurrent process registers: \n\n");
   fprintf (outfile, "\nCurrent process registers: \n\n");

   
   
/*	Print registers, NOTE:  we must swap the high and low order longword */
   
   printf ("R0   = %08X %08X  R1   = %08X %08X  R2   = %08X %08X\n",
	prc_registers[1],prc_registers[0],prc_registers[3],prc_registers[2],
	prc_registers[5],prc_registers[4]);
   fprintf (outfile, "R0   = %08X %08X  R1   = %08X %08X  R2   = %08X %08X\n",
	prc_registers[1],prc_registers[0],prc_registers[3],prc_registers[2],
	prc_registers[5],prc_registers[4]);

   
   printf ("R3   = %08X %08X  R4   = %08X %08X  R5   = %08X %08X\n",
	   prc_registers[7],prc_registers[6],prc_registers[9],prc_registers[8],
	   prc_registers[11],prc_registers[10]);
   fprintf (outfile, "R3   = %08X %08X  R4   = %08X %08X  R5   = %08X %08X\n",
	   prc_registers[7],prc_registers[6],prc_registers[9],prc_registers[8],
	   prc_registers[11],prc_registers[10]);

   
   printf ("R6   = %08X %08X  R7   = %08X %08X  R8   = %08X %08X\n",
	   prc_registers[13],prc_registers[12],prc_registers[15],prc_registers[14],
	   prc_registers[17],prc_registers[16]);
   fprintf (outfile, "R6   = %08X %08X  R7   = %08X %08X  R8   = %08X %08X\n",
	   prc_registers[13],prc_registers[12],prc_registers[15],prc_registers[14],
	   prc_registers[17],prc_registers[16]);

   
   printf ("R9   = %08X %08X  R10  = %08X %08X  R11  = %08X %08X\n",
	   prc_registers[19],prc_registers[18],prc_registers[21],prc_registers[20],
	   prc_registers[23],prc_registers[22]);
   fprintf (outfile, "R9   = %08X %08X  R10  = %08X %08X  R11  = %08X %08X\n",
	   prc_registers[19],prc_registers[18],prc_registers[21],prc_registers[20],
	   prc_registers[23],prc_registers[22]);

   
   printf ("R12  = %08X %08X  R13  = %08X %08X  R14  = %08X %08X\n",
	   prc_registers[25],prc_registers[24],prc_registers[27],prc_registers[26],
	   prc_registers[29],prc_registers[28]);
   fprintf (outfile, "R12  = %08X %08X  R13  = %08X %08X  R14  = %08X %08X\n",
	   prc_registers[25],prc_registers[24],prc_registers[27],prc_registers[26],
	   prc_registers[29],prc_registers[28]);

   
   printf ("R15  = %08X %08X  R16  = %08X %08X  R17  = %08X %08X\n",
	   prc_registers[31],prc_registers[30],prc_registers[33],prc_registers[32],
	   prc_registers[35],prc_registers[34]);
   fprintf (outfile, "R15  = %08X %08X  R16  = %08X %08X  R17  = %08X %08X\n",
	   prc_registers[31],prc_registers[30],prc_registers[33],prc_registers[32],
	   prc_registers[35],prc_registers[34]);

   
   printf ("R18  = %08X %08X  R19  = %08X %08X  R20  = %08X %08X\n",
	   prc_registers[37],prc_registers[36],prc_registers[39],prc_registers[38],
	   prc_registers[41],prc_registers[40]);
   fprintf (outfile, "R18  = %08X %08X  R19  = %08X %08X  R20  = %08X %08X\n",
	   prc_registers[37],prc_registers[36],prc_registers[39],prc_registers[38],
	   prc_registers[41],prc_registers[40]);

   
   printf ("R21  = %08X %08X  R22  = %08X %08X  R23  = %08X %08X\n",
	   prc_registers[43],prc_registers[42],prc_registers[45],prc_registers[44],
	   prc_registers[47],prc_registers[46]);
   fprintf (outfile, "R21  = %08X %08X  R22  = %08X %08X  R23  = %08X %08X\n",
	   prc_registers[43],prc_registers[42],prc_registers[45],prc_registers[44],
	   prc_registers[47],prc_registers[46]);

   
   printf ("R24  = %08X %08X  R25  = %08X %08X  R26  = %08X %08X\n",
	   prc_registers[49],prc_registers[48],prc_registers[51],prc_registers[50],
	   prc_registers[53],prc_registers[52]);
   fprintf (outfile, "R24  = %08X %08X  R25  = %08X %08X  R26  = %08X %08X\n",
	   prc_registers[49],prc_registers[48],prc_registers[51],prc_registers[50],
	   prc_registers[53],prc_registers[52]);

   
   printf ("R27  = %08X %08X  R28  = %08X %08X  FP   = %08X %08X\n",
	   prc_registers[55],prc_registers[54],prc_registers[57],prc_registers[56],
	   prc_registers[59],prc_registers[58]);
   fprintf (outfile, "R27  = %08X %08X  R28  = %08X %08X  FP   = %08X %08X\n",
	   prc_registers[55],prc_registers[54],prc_registers[57],prc_registers[56],
	   prc_registers[59],prc_registers[58]);

   
   printf ("PC   = %08X %08X  PS   = %08X %08X\n",
   prc_registers[61],prc_registers[60],prc_registers[63],prc_registers[62]);
   fprintf (outfile, "PC   = %08X %08X  PS   = %08X %08X\n",
   prc_registers[61],prc_registers[60],prc_registers[63],prc_registers[62]);

   
   prvmod = prc_registers[62] & 0x3;	  	/* Extract previous mode */
   curmod = (prc_registers[62] & 0x18) >> 3;	/* Extract current mode  */
   ipl    = prc_registers[62] & 0x1F00;	  	/* Extract IPL	   */
   
   printf ("\nCurrent mode     : %10s\nPrevious mode    : %10s\n",
					   modes[curmod],modes[prvmod]);
   fprintf (outfile, "\nCurrent mode     : %10s\nPrevious mode    : %10s\n",
					   modes[curmod],modes[prvmod]);

   printf ("Current IPL      : %d \n",ipl);
   fprintf (outfile, "Current IPL      : %d \n",ipl);

   
/*	Map Current PC	*/
   
   for (k=0; k<40; k++)
     {
     image_name[k] = 0;
     }
   
   arglist [0] = 2;
   arglist [1] = prc_registers[60];		/* PC	*/
   arglist [2] = (int) pcb_pid;			/* IPID	*/
   
   return_status =	SYS$CMKRNL(&map_address, arglist);
   if (return_status == SS$_TIMEOUT)
      {
      printf ("%%MWAIT-F-NORESPONSE, can not read target process image list\n");
      fprintf (outfile, "%%MWAIT-F-NORESPONSE, can not read target process image list\n");
      }

   if (return_status != SS$_NORMAL) return (return_status);
      
/*   printf (" tmp = %08X  ldrimg = %08X  imcb = %08X start = %08X  end = %08X found = %d\n",
/*   fprintf (outfile, " tmp = %08X  ldrimg = %08X  imcb = %08X start = %08X  end = %08X found = %d\n",

					tmp,ldrimg,imcb,start,end,found);  */
   
/*  printf (" start = %08X  end = %08X found = %d kfe_sect = %08X kfe_offs = %08X\n",
/*  fprintf (outfile, " start = %08X  end = %08X found = %d kfe_sect = %08X kfe_offs = %08X\n",

			start,end,found,kferes_section,kferes_offset);	*/
   
   if (found == 1)
     {
     printf ("\nThe current PC:        %08X is in the image: \n%s\n",
			prc_registers[60],image_name);
     fprintf (outfile, "\nThe current PC:        %08X is in the image: \n%s\n",
			prc_registers[60],image_name);

     
     printf ("Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n",
			start,end,offset,psects[psect]);
     fprintf (outfile, "Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n",
			start,end,offset,psects[psect]);

     
     }
   else
      {
      printf ("\nThe current PC:  %08X is not within a system or user image\n",
			prc_registers[60]);
      fprintf (outfile, "\nThe current PC:  %08X is not within a system or user image\n",
			prc_registers[60]);
      }

   
   
   
   
/*	Map R26 (Return address)	*/
   
   for (k=0; k<40; k++)	image_name[k] = 0;
      
   arglist [0] = 2;
   arglist [1] = prc_registers[52];		/* R26	*/
   arglist [2] = (int) pcb_pid;			/* IPID	*/
   
   return_status =	SYS$CMKRNL(&map_address, arglist);
   if (return_status == SS$_TIMEOUT)
      {
      printf ("%%MWAIT-F-NORESPONSE, can not read target process image list\n");
      fprintf (outfile, "%%MWAIT-F-NORESPONSE, can not read target process image list\n");
      }

   if (return_status != SS$_NORMAL) return (return_status);
      
/*  printf (" tmp = %08X  ldrimg = %08X  imcb = %08X start = %08X  end = %08X found = %d\n",
					tmp,ldrimg,imcb,start,end,found);    */
/*  fprintf (outfile, " tmp = %08X  ldrimg = %08X  imcb = %08X start = %08X  end = %08X found = %d\n",
					tmp,ldrimg,imcb,start,end,found);    */

   
/*  printf (" start = %08X  end = %08X found = %d kfe_sect = %08X kfe_offs = %08X\n",
			start,end,found,kferes_section,kferes_offset);	*/
/*  fprintf (outfile, " start = %08X  end = %08X found = %d kfe_sect = %08X kfe_offs = %08X\n",
			start,end,found,kferes_section,kferes_offset);	*/

   
   if (found == 1)
     {
     printf ("\nR26 (Return address):  %08X is in the image: \n%s\n",
		     prc_registers[52],image_name);
     fprintf (outfile, "\nR26 (Return address):  %08X is in the image: \n%s\n",
		     prc_registers[52],image_name);

     
     printf ("Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n",
		     start,end,offset,psects[psect]);
     fprintf (outfile, "Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n",
		     start,end,offset,psects[psect]);

     }
   else
      {
      printf ("\nR26 (Return address):  %08X is not within a system or user image \nand is not a return address\n",
		   prc_registers[52]);
      fprintf (outfile, "\nR26 (Return address):  %08X is not within a system or user image \nand is not a return address\n",
		   prc_registers[52]);
      }

   
   
/* *********** Get call frames ********************************** */
   
   fp      = prc_registers[58];
   next_fp = fp;
   
   
   frame_count = 0;
   
   while ( next_fp != 0 )
     {
     
     frame_count = frame_count + 1;
     
     printf ("\n******************* Call Frame %d ***************************************\n",
		     frame_count);
     fprintf (outfile, "\n******************* Call Frame %d ***************************************\n",
		     frame_count);

     
     arglist [0] = 1;
     arglist [1] = prc_registers[58];		/* FP	*/
     
     return_status =	SYS$CMKRNL(&get_fp_and_pdsc, arglist);
     
     if (return_status == SS$_NODATA)
       {
       printf ("\n%%MWAIT-F-NOTINPHYS, Virtual data not in physical memory: \n");
       fprintf (outfile, "\n%%MWAIT-F-NOTINPHYS, Virtual data not in physical memory: \n");

       printf ("FP = %08X ==> PDSC = %08X\n", fp,pd);
       fprintf (outfile, "FP = %08X ==> PDSC = %08X\n", fp,pd);

       return (SS$_NORMAL);
       }
     
     if (return_status == SS$_INSFRAME)
       {
       printf ("\n%%MWAIT-F-INVFRAME, Invalid Call Frame: \n");
       fprintf (outfile, "\n%%MWAIT-F-INVFRAME, Invalid Call Frame: \n");

       printf ("Unknown procedure descriptor kind, FP = %08X, Kind = %02X\n",
			       fp,pdsc_kind);
       fprintf (outfile, "Unknown procedure descriptor kind, FP = %08X, Kind = %02X\n",
			       fp,pdsc_kind);

       return (SS$_NORMAL);
       }
     
     if (return_status == SS$_ITEMNOTFOUND)
       {
       printf ("\n%%MWAIT-F-INVFRAME, Invalid Call Frame: \n");
       fprintf (outfile, "\n%%MWAIT-F-INVFRAME, Invalid Call Frame: \n");

       printf ("Procedure descriptor not found at FP = %08X ==> %08X\n", fp,pd);
       fprintf (outfile, "Procedure descriptor not found at FP = %08X ==> %08X\n", fp,pd);

       return (SS$_NORMAL);
       }
     
     if (return_status != SS$_NORMAL) return (return_status);
        
     printf ("\nFP      = %08X \nPDSC    = %08X  %s\nNext FP = %08X\n",
		     fp, pd, pd_kind[pdsc_kind], next_fp);
     fprintf (outfile, "\nFP      = %08X \nPDSC    = %08X  %s\nNext FP = %08X\n",
		     fp, pd, pd_kind[pdsc_kind], next_fp);

     
     
/*
	printf ("flags = %08X    kind = %08X \n", pdsc_flags,pdsc_kind);
	fprintf (outfile, "flags = %08X    kind = %08X \n", pdsc_flags,pdsc_kind);

	printf ("entry = %08X    offset = %08X \n", pdsc_entry,pdsc_rsa_offset);
	fprintf (outfile, "entry = %08X    offset = %08X \n", pdsc_entry,pdsc_rsa_offset);

	printf ("mask = %08X tmp = %d tmp2 = %d  \n", ireg_mask,tmp,tmp2);
	fprintf (outfile, "mask = %08X tmp = %d tmp2 = %d  \n", ireg_mask,tmp,tmp2);

	printf ("save_fp = %08X save_ra = %08X\n", save_fp,save_ra);
	fprintf (outfile, "save_fp = %08X save_ra = %08X\n", save_fp,save_ra);


	printf ("frame[1] frame[0] = %08X %08X \n", frame[1],frame[0]);
	fprintf (outfile, "frame[1] frame[0] = %08X %08X \n", frame[1],frame[0]);

	printf ("frame[3] frame[2] = %08X %08X \n", frame[3],frame[2]);
	fprintf (outfile, "frame[3] frame[2] = %08X %08X \n", frame[3],frame[2]);

	printf ("frame[5] frame[4] = %08X %08X \n", frame[5],frame[4]);
	fprintf (outfile, "frame[5] frame[4] = %08X %08X \n", frame[5],frame[4]);

	printf ("frame[7] frame[6] = %08X %08X \n", frame[7],frame[6]);
	fprintf (outfile, "frame[7] frame[6] = %08X %08X \n", frame[7],frame[6]);

	printf ("frame[9] frame[8] = %08X %08X \n", frame[9],frame[8]);
	fprintf (outfile, "frame[9] frame[8] = %08X %08X \n", frame[9],frame[8]);

	printf ("frame[11] frame[10] = %08X %08X \n", frame[11],frame[10]);
	fprintf (outfile, "frame[11] frame[10] = %08X %08X \n", frame[11],frame[10]);

*/
     
     fp = next_fp;
     
/*	Map Procedure entry	*/
     
     for (k=0; k<40; k++)	image_name[k] = 0;
        
     arglist [0] = 2;
     arglist [1] = pdsc_b[2];			/* Procedure entry	*/
     arglist [2] = (int) pcb_pid;		/* IPID	*/
     
     return_status =	SYS$CMKRNL(&map_address, arglist);
     if (return_status == SS$_TIMEOUT)
	{
        printf ("%%MWAIT-F-NORESPONSE, can not read target process image list\n");
        fprintf (outfile, "%%MWAIT-F-NORESPONSE, can not read target process image list\n");
	}

     if (return_status != SS$_NORMAL) return (return_status);
        
/*  printf (" start = %08X  end = %08X found = %d kfe_sect = %08X kfe_offs = %08X\n",
/*  fprintf (outfile, " start = %08X  end = %08X found = %d kfe_sect = %08X kfe_offs = %08X\n",

			start,end,found,kferes_section,kferes_offset);	*/
     
     if (found == 1)
       {
       printf ("\nProcedure Entry: %08X is in the image: \n%s\n",
		       pdsc_b[2],image_name);
       fprintf (outfile, "\nProcedure Entry: %08X is in the image: \n%s\n",
		       pdsc_b[2],image_name);

       
       printf ("Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n",
		       start,end,offset,psects[psect]);
       fprintf (outfile, "Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n",
		       start,end,offset,psects[psect]);

       
       }
     else
	{
        printf ("\nProcedure Entry: %08X is not within a system or user image\n",
		       pdsc_b[2]);
        fprintf (outfile, "\nProcedure Entry: %08X is not within a system or user image\n",
		       pdsc_b[2]);
	}
     
     
/*	Map R26 (Return address)	*/
     
     for (k=0; k<40; k++)	image_name[k] = 0;
        
     arglist [0] = 2;
     arglist [1] = r26;				/* R26	*/
     arglist [2] = (int) pcb_pid;		/* IPID	*/
     
     return_status =	SYS$CMKRNL(&map_address, arglist);
     if (return_status == SS$_TIMEOUT)
	{
        printf ("%%MWAIT-F-NORESPONSE, can not read target process image list\n");
        fprintf (outfile, "%%MWAIT-F-NORESPONSE, can not read target process image list\n");
	}

     if (return_status != SS$_NORMAL) return (return_status);
        
/*  printf (" tmp = %08X  ldrimg = %08X  imcb = %08X start = %08X  end = %08X found = %d\n",
/*  fprintf (outfile, " tmp = %08X  ldrimg = %08X  imcb = %08X start = %08X  end = %08X found = %d\n",

					tmp,ldrimg,imcb,start,end,found);    */
     
/*  printf (" start = %08X  end = %08X found = %d kfe_sect = %08X kfe_offs = %08X\n",
/*  fprintf (outfile, " start = %08X  end = %08X found = %d kfe_sect = %08X kfe_offs = %08X\n",

			start,end,found,kferes_section,kferes_offset);	*/
     
     if (found == 1)
       {
       printf ("\nReturn address:  %08X is in the image: \n%s\n",
			r26,image_name);
       fprintf (outfile, "\nReturn address:  %08X is in the image: \n%s\n",
			r26,image_name);

       
       printf ("Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n",
			start,end,offset,psects[psect]);
       fprintf (outfile, "Base      End       Image Offset  Psect type\n%08X  %08X  %08X      %s\n",
			start,end,offset,psects[psect]);

       
       }
     else
	{
        printf ("\nReturn address:  %08X is not within a system or user image\n",
			r26);
        fprintf (outfile, "\nReturn address:  %08X is not within a system or user image\n",
			r26);
	}

     
     } /* end while */
   
   fclose (outfile);		/* Close output file	*/

   printf ("\n *** The output was written into file %s *** \n", (char *) outname);
   
   return (SS$_NORMAL);
   
   }

/* ******************************************************************* */
/* ******************************************************************* */
/* ******************************************************************* */


/* Routine get_fp_and_pdsc (int frame_pointer)
 *
 *
 * FUNCTION
 *
 * 	This routine gets the PDSC and next FP in Kernel mode
 *
 * INPUT
 *
 *	FP Frame pointer for the call frame
 *
 * IMPLICIT INPUT
 *
 *      PID process ID
 *
 * EXPLICIT OUTPUT
 *
 *      ss$_normal      Success.  
 *	ss$_nonexpr	Nonexistent process
 *
 */

int get_fp_and_pdsc (int frame_pointer)
   {
   
/*	Get Procedure Descriptor pointer (R27) */
   
   return_status = EXE$READ_PROCESS(pcb_pid,link_length,fp,
		   &pd,EACB$K_MEMORY,&read_process_ast_count);
   if (return_status != SS$_NORMAL) return (return_status);
      
   if ( pd == 0 ) pd = fp;		/* Bound procedure descriptor */
      
   
/*	IF (FP)<2:0> is 0, then FP points to a quadword that contains
	a pointer to the procedure descriptor for the current procedure.

	IF (FP)<2:0> is not 0, then FP points to the procedure descriptor
	for the current procedure.
 */
   if ( pd & 7 )
      pd = fp;
   
/*	if ( pd & 7 ) return (SS$_ITEMNOTFOUND);	*/
      
   
/*	Get Procedure Descriptor */
   
   return_status = EXE$READ_PROCESS(pcb_pid,PDSC$K_MIN_LENGTH_SF,pd,
	   &pdsc_b,EACB$K_MEMORY,&read_process_ast_count);
   if (return_status != SS$_NORMAL) return (return_status);
      
   pdsc = (struct pdscdef *) pdsc_b;		/* PDSC 	*/
   
   
   pdsc_flags = pdsc->pdsc$w_flags;		/* PDSC flags 	*/
   
   pdsc_kind  = (pdsc_flags & 0Xf );		/* PDSC$V_KIND  */
   
   if (!(pdsc_kind == PDSC$K_KIND_BOUND    ||
	 pdsc_kind == PDSC$K_KIND_NULL     ||
	 pdsc_kind == PDSC$K_KIND_FP_STACK ||
	 pdsc_kind == PDSC$K_KIND_FP_REGISTER) )	return (SS$_INSFRAME);
   
/* ******** Stack Frame Procedure *********** */
   
   if ( pdsc_flags & PDSC$M_BASE_REG_IS_FP)
     {
     
     pdsc_entry = pdsc->pdsc$l_entry;		/* Code entry	*/
     
     pdsc_rsa_offset = pdsc->pdsc$w_rsa_offset; /* Register save offset */
     
     ireg_mask = pdsc->pdsc$l_ireg_mask;	/* IREG mask	*/
     
     tmp = 0;
     for (i_bit = 0; i_bit < 33; i_bit++)
       {
       if (( ireg_mask & bit_mask[i_bit+1] ) >> i_bit)
          tmp = tmp + 1;			/* Count saved registers */
       }
     
     tmp2 = 8 * tmp + pdsc_rsa_offset;		/* Call frame size to be read */
						/* from stack (before FMASK) */	
     
/*	Get IREG's from the call frame on stack	 */
     
     return_status = EXE$READ_PROCESS(pcb_pid,tmp2,fp+pdsc_rsa_offset,
	     &frame,EACB$K_MEMORY,&read_process_ast_count);
     if (return_status != SS$_NORMAL) return (return_status);
        
     
     r26     = frame[0];			/* Return Address	*/
     next_fp = frame[2*tmp];			/* Next FP		*/
     
     }
   
/* ******** Register Frame Procedure *********** */
   
   if ( pdsc_kind == PDSC$K_KIND_FP_REGISTER )
     {
     
     save_fp = pdsc->pdsc$b_save_fp;		/* Saved FP reg	*/
     save_ra = pdsc->pdsc$b_save_ra;		/* Saved RA reg	*/
     
     next_fp = prc_registers[2*save_fp];	/* Get FP	*/
     r26     = prc_registers[2*save_ra];	/* Get RA	*/
     
     pdsc_entry = pdsc->pdsc$l_entry;		/* Code entry	*/
     
     }
   
   
   if ( pdsc_kind == PDSC$K_KIND_BOUND )
     {
     
     next_fp = pdsc->pdsc$l_proc_value;		/* Get FP	*/
     
     }
   
   
   return (return_status);
   
   }



/* ******************************************************************* */



/* Routine get_process_info(int pid)
 *
 *
 * FUNCTION
 *
 * 	This routine gets the process information in Kernel mode
 *
 * INPUT
 *
 *      PID process ID
 *
 * IMPLICIT INPUT
 *
 * EXPLICIT OUTPUT
 *
 *      ss$_normal      Success.  
 *	ss$_nonexpr	Nonexistent process
 *
 */

int get_process_info (int pid)
   {
   sys_lock (SCHED, 1, &orig_ipl);		/* Get SCHED lock	*/
   
   
   pcb = (PCB *) EXE_STD$CVT_EPID_TO_PCB(pid);	/* Convert PID to PCB   */
   tmp = EXE_STD$CVT_EPID_TO_IPID(pid);		/* Convert PID to IPID  */
   
/*	SWAPPER is illegal, it has no JIB or normal process quotas	*/
   
   if (pcb == 0 || tmp <= SCH$GL_SWPPID ) 
     {	
     sys_unlock (SCHED, orig_ipl, 0);		/* Release SCHED lock	*/
     return (SS$_NONEXPR);			/* Nonexistent process	*/
     }
   
/************** PCB *************************************************/
   
   
/* 	__PAL_PROBER( address, length, mode ) 				*/
   
   if (! ( __PAL_PROBER (pcb->pcb$l_sqfl, 4, PR$C_PS_KERNEL) ) )
      return (SS$_ACCVIO);		/* Can PCB be read in Kernel mode ? */
   
   pcb_ktbvec   = pcb->pcb$l_ktbvec;		/* KTB vector adddress  */
   pcb_kt_count = pcb->pcb$l_kt_count;		/* Kthread count	*/
   
/*	First get the length of the process name, the pcb$t_lname field
 *	can have 'leftovers'.
 */
   strncpy (tmp_string,pcb->pcb$t_lname, 2); 	/* Process name	*/
   str_length = (int) tmp_string[0];		/* Process name length  */
   strncpy (pcb_lname, pcb->pcb$t_lname+1, str_length); 
   
   strncpy (tmp_string,pcb->pcb$t_terminal, 2); /* Terminal name	 */
   str_length = (int) tmp_string[0];		/* Terminal name length  */
   strncpy (pcb_terminal, pcb->pcb$t_terminal+1, str_length);
   
   pcb_epid   = pcb->pcb$l_epid;		/* EPID			*/
   pcb_pid    = pcb->pcb$l_pid;		/* IPID			*/
   pcb_pri    = PRI$C_NUM_PRI - 1 - pcb->pcb$l_pri;
						/* Current priority	*/
   pcb_prib   = PRI$C_NUM_PRI - 1 - pcb->pcb$l_prib;
						/* Base priority	*/
   pcb_owner  = pcb->pcb$l_owner;		/* Owner		*/
   owner_epid = EXE_STD$CVT_IPID_TO_EPID(pcb->pcb$l_owner); /* Owner EPID */
   
   pcb_waitime  = pcb->pcb$l_waitime;		/* Waitime in 10ms ticks */
   pcb_diocnt   = pcb->pcb$l_diocnt;		/* DIO count		*/
   pcb_diolm    = pcb->pcb$l_diolm;		/* DIO limit		*/
   pcb_biocnt   = pcb->pcb$l_biocnt;		/* BIO count		*/
   pcb_biolm    = pcb->pcb$l_biolm;		/* BIO limit		*/
   pcb_prccnt   = pcb->pcb$l_prccnt;		/* Sub-process count	*/
   pcb_dpc      = pcb->pcb$l_dpc;		/* Delete pending count	*/
   pcb_astcnt   = pcb->pcb$l_astcnt;		/* AST count		*/
   pcb_astact   = pcb->pcb$l_astact;		/* AST's active		*/
   
   
/* 	Get WSQUOTA = Process page count + Global page countin in WS	*/
/*	pagelets = 16 * pages						*/
   
   pcb_wsquota= 16 * ( pcb->pcb$l_ppgcnt + pcb->pcb$l_gpgcnt );
   
   
   pcb_ast_k = '-';
   pcb_ast_e = '-';
   pcb_ast_s = '-';
   pcb_ast_u = '-';
   
/*	An AST is pending if AST queue forward link != forward link address */
   
   if ((ACB *) pcb->pcb$l_astqfl_k != (ACB *) &pcb->pcb$l_astqfl_k) pcb_ast_k = 'K';
   if ((ACB *) pcb->pcb$l_astqfl_e != (ACB *) &pcb->pcb$l_astqfl_e) pcb_ast_e = 'E';
   if ((ACB *) pcb->pcb$l_astqfl_s != (ACB *) &pcb->pcb$l_astqfl_s) pcb_ast_s = 'S';
   if ((ACB *) pcb->pcb$l_astqfl_u != (ACB *) &pcb->pcb$l_astqfl_u) pcb_ast_u = 'U';
   
      
/************** PCB/KTB thread ********************************************/
   
/*	Get KTBVEC contents	*/
      
   
   if (! ( __PAL_PROBER ( pcb_ktbvec, 4*MAX_THREADS, PR$C_PS_KERNEL) ) )
      return (SS$_ACCVIO);		/* Can KTB be read in Kernel mode ? */
   
   memcpy (&pcb_ktbvec_a, pcb_ktbvec, 4*MAX_THREADS);
   
   
   for (threads_ind = 0; threads_ind < pcb_kt_count; threads_ind++)
     {
     
     if ( pcb_ktbvec_a[threads_ind] != 0 )
       {
       
       pcb = (PCB *) pcb_ktbvec_a[threads_ind];
       
       }
     
     
     pcb_state[threads_ind]  = pcb->pcb$l_state;  /* Process state	*/
     pcb_sts[threads_ind]    = pcb->pcb$l_sts;	  /* Process status	*/
     pcb_efwm[threads_ind]   = pcb->pcb$l_efwm;	  /* Event flag wait mask */
     pcb_wefc[threads_ind]   = pcb->pcb$l_wefc;	  /* EFN cluster number	*/
     pcb_cpu_id[threads_ind] = pcb->pcb$l_cpu_id; /* CPU id		*/
     pcb_efcs[threads_ind]   = pcb->pcb$l_efcs;	  /* System EFN mask	*/
     pcb_efcu[threads_ind]   = pcb->pcb$l_efcu;	  /* User EFN mask	*/
     pcb_efc2p[threads_ind]  = pcb->pcb$l_efc2p;  /* EFC 2		*/
     
     if (pcb_efc2p[threads_ind] != 0 )
       {
       ceb  = ( CEB *) pcb->pcb$l_efc2p;	  /* EFC 2		*/
       ceb_tmp = (unsigned int) ceb->ceb$t_efcnam;
       
       strncpy (tmp_string,ceb->ceb$t_efcnam, 2); /* EFN cluster name */
       str_length = (int) tmp_string[0];	  /* EFN cluster name length */
       strncpy (ceb_name2, ceb->ceb$t_efcnam+1, str_length); 
       }
     
     pcb_efc3p[threads_ind]  = pcb->pcb$l_efc3p;  /* EFC 3		*/
     
     if (pcb_efc3p[threads_ind] != 0 )
       {
       ceb  = ( CEB *) pcb->pcb$l_efc3p;	  /* EFC 3		*/
       ceb1_tmp = (unsigned int) ceb->ceb$t_efcnam;
       
       strncpy (tmp_string,ceb->ceb$t_efcnam, 2); /* EFN cluster name */
       str_length = (int) tmp_string[0];	  /* EFN cluster name length  */
       strncpy (ceb_name3, ceb->ceb$t_efcnam+1, str_length);
       }
     
     } /* end if PCB/KTB thread */
   
   
/************** PHD *************************************************/
   
   phd_astlm  = -1;
   pcb_phd    = -1;
   phd_astsr_asten = -1;
   phd_pstbasoff = -1;
   
   if ( pcb->pcb$l_sts & PCB$M_PHDRES)		/* If PHD resident 	*/
     {
/*	    if (! ( __PAL_PROBER ( (void *) phd , 4, PR$C_PS_KERNEL) ) )    */
/*		return (SS$_ACCVIO);					*/
					/* Can PHD be read in Kernel mode ? */
     
     pcb_phd   = (int) pcb->pcb$l_phd;		/* PHD			*/
     phd       = pcb->pcb$l_phd;		/* PHD			*/
     phd_astlm = phd->phd$l_astlm;		/* AST limit		*/
     phd_astsr_asten = phd->phd$q_astsr_asten;
						 /* AST's served/enabled */
     phd_pstbasoff = phd->phd$l_pst_base_offset; /* PST base offset */
     
/*	Maximum working set is PHD$L_WSAUTHEXT - PHD$L_WSLIST + 1 pages	*/
     
     wsextent  = 16 * (phd->phd$l_wsauthext - phd->phd$l_wslist + 1);
     
     }
   
/************** JIB *************************************************/
   
   jib = pcb->pcb$l_jib;			/* JIB			*/
   
/* if (!(__PAL_PROBER (jib->jib$l_mtlfl,4,PR$C_PS_KERNEL))) return (SS$_ACCVIO); */
				/* Can JIB be read in Kernel mode ? */
   
   strncpy (jib_username, jib->jib$t_username, 12); /* User name	*/
   
   jib_flags   = jib->jib$l_flags;		/* JIB wait flags	*/
   jib_bytcnt  = jib->jib$l_bytcnt;		/* Byte count		*/
   jib_bytlm   = jib->jib$l_bytlm;		/* Byte limit		*/
   jib_org_bytlm = jib->jib$l_org_bytlm;	/* Original Byte limit	*/
   jib_filcnt  = jib->jib$l_filcnt;		/* File count		*/
   jib_fillm   = jib->jib$l_fillm;		/* File limit		*/
   jib_prclim  = jib->jib$l_prclim;		/* Sub-process limit	*/
   jib_prccnt  = jib->jib$l_prccnt;		/* Sub-process # 	*/
   jib_mpid    = jib->jib$l_mpid;		/* PID of master process */
   master_epid = EXE_STD$CVT_IPID_TO_EPID(jib->jib$l_mpid); /* Master EPID */

   jib_tqcnt   = jib->jib$l_tqcnt;		/* Timer count		*/
   jib_tqlm    = jib->jib$l_tqlm;		/* Timer limit		*/
   jib_pgflcnt = jib->jib$l_pgflcnt;		/* Page file count	*/
   jib_pgflquota = jib->jib$l_pgflquota;	/* Page file quota	*/
   
   
   abstim_tics = exe$gl_abstim_tics; 		/* Absolute time	*/
   
   mmg_fresva  = MMG$GQ_NEXT_FREE_S0S1_VA;	/* First free system VA */
   
   
   sys_unlock (SCHED, orig_ipl, 0);		/* Release SCHED lock	*/
   
   return (SS$_NORMAL);				/* Success		*/
   }


/* ******************************************************************* */

/*
 *	if( smp$gl_flags.smp$v_enabled )
 *		
 *
 *
*/


/* ******************************************************************* */

/* Routine get_process_channels(int index, int pid)
 *
 *
 * FUNCTION
 *
 * 	This routine gets the process channel information in Kernel mode
 *
 * INPUT
 *
 *      PID process ID
 *
 * IMPLICIT INPUT
 *
 * EXPLICIT OUTPUT
 *
 *      ss$_normal      Success.  
 *	ss$_nonexpr	Nonexistent process
 *
 */

int get_process_channels (int index, int pid)
   {
   ctl_chindx_length = 4;			/* Channel index length	*/
   
/*	Check if valid PID	*/
   
   if (SCH$GL_MAXPIX > index)
     {
     
/* Get number of channels			*/
     
     return_status =	EXE$READ_PROCESS(pid,ctl_chindx_length,&CTL$GL_CHINDX,
	     &ctl_chindx,EACB$K_MEMORY,&read_process_ast_count);
     if (return_status != SS$_NORMAL) return (return_status);
        
     ccb_length = ctl_chindx * CCB$K_LENGTH;	/* Table size */
     
/* Get address of CCB table			*/
     
     return_status = EXE$READ_PROCESS(pid,ctl_chindx_length,&CTL$GA_CCB_TABLE,
	     &ccb_table_address,EACB$K_MEMORY,&read_process_ast_count);
     
     if (return_status != SS$_NORMAL) return (return_status);
        
     
/* ?? CLEAR TABLE ?? */
     
     
/* Get CCB table	*/
     
     return_status = EXE$READ_PROCESS(pid,ccb_length,ccb_table_address,
	     &ccb_table,EACB$K_MEMORY,&read_process_ast_count);
     if (return_status != SS$_NORMAL) return (return_status);
        }
   
   else
     {
     return (SS$_NONEXPR);
     }
   
   return (SS$_NORMAL);
   
   }

/* *********************************************************************** */

/* Routine get_device_name(int ucb_address, int iocou)
 *
 *
 * FUNCTION
 *
 * 	This routine gets the process channel information in Kernel mode
 * INPUT
 *
 *      UCB 	Unit control block address
 *
 *
 * IMPLICIT INPUT
 *
 *	Window	CCB Window
 *
 * EXPLICIT OUTPUT
 *
 *      ss$_normal     Success
 *      ss$_accvio     Access violation
 *
 */

int get_device_name(int ucb_address, int iocou)

   {
	net_port = 0;			/* Clear Port name flag */

   ucb    = (UCB *) (ucb_address);		/* UCB address		*/
   mb_ucb = (MB_UCB *) (ucb_address);		/* MB_UCB address	*/
   
   if (! ( __PAL_PROBER (ucb->ucb$l_ddb, 4, PR$C_PS_KERNEL) ) )
      return (SS$_ACCVIO);    		/* Can UCB be read in Kernel mode ? */
   
   unit   = ucb->ucb$w_unit;		/* Unit number	          */
   bufquo = ucb->ucb$w_bufquo;		/* Buffered quota left    */
   ucb_devchar = ucb->ucb$l_devchar;	/* Device characteristic  */
   ucb_sts     = ucb->ucb$l_sts;	/* Device unit status	  */
   ucb_devtype = ucb->ucb$b_devtype;	/* Device type		  */

   irp_count = 0;			/* Clear IRP #	*/

/* *********************************************************************** */
   
   ddb = ucb->ucb$l_ddb;			/* DDB address	*/
   
   if (! ( __PAL_PROBER (ddb->ddb$t_name, 4, PR$C_PS_KERNEL) ) )
	      return (SS$_ACCVIO);    /* Can DDB be read in Kernel mode ? */
   
   ddb_name_length = 0;				/* Clear name length	*/
   
   sb = (SB *) ddb->ddb$l_sb;			/* System control block	*/
   
/* 	If remote device ==> Insert node name in device name	*/
   
   if ( SCS$AR_LOCALSB != ddb->ddb$l_sb )
     {
     strncpy (tmp_string,sb->sb$t_nodename,2); 	/* Node name */
     ddb_name_length = (int) tmp_string[0];	/* Node name length */
     strncpy (ddb_name, sb->sb$t_nodename+1, ddb_name_length);
     
     strcat (ddb_name,"$");			/* Insert $	*/
     }

   strncpy (tmp_string,ddb->ddb$t_name,2); 	/* Device name	*/
   str_length = (int) tmp_string[0];       	/* Device name length  */
   strncpy (tempstr, ddb->ddb$t_name+1, str_length);
   
   strcat (ddb_name,tempstr);			/* Insert device name	*/
   
   
/*	We have PST index, get the window 	*/
   
   if ( (window > 0) && (phd_pstbasoff != -1) )	/* Global section index */
     {
     PSTbase  = (int) phd + phd_pstbasoff;	/* PST address */
     
     section = PSTbase - window;		/* PST entry address	  */
						/* window = Section Index */
     
     section = section + 16;	   /* Address of window = sec->sec$l_window */
     
/*	Get the window	*/
     
     return_status = EXE$READ_PROCESS(pcb_pid,link_length,section,
	     &window,EACB$K_MEMORY,&read_process_ast_count);
	     if (return_status != SS$_NORMAL) return (return_status);

     }
   
/*	Get file name, if a window and low bit is clear (= interlocked)	*/
/*	And not a NET device						*/

   if ( (window != 0) && ((window & bit_mask[0]) == 0) && 
					!(ucb_devchar & DEV$M_NET) )
     {
     
     if ( window > 0X80000000 )
       {
       
/*	Real window 	*/

       return_status = EXE$READ_PROCESS(pcb_pid,WCB$K_LENGTH,window,
	       &wcb_block,EACB$K_MEMORY,&read_process_ast_count);
	       if (return_status != SS$_NORMAL) return (return_status);
          
       wcb = (WCB *) wcb_block;			/* WCB local address	*/
       
       if ( wcb->wcb$b_type != DYN$C_WCB ) return (SS$_NORMAL);
          
       fcb = wcb->wcb$l_fcb;				/* FCB address	*/
       
       if ( (int) fcb == 0 ) return (SS$_NORMAL);	/* No FCB	*/
          
       return_status = EXE$READ_PROCESS(pcb_pid,FCB$K_LENGTH,(int) fcb,
	       &fcb_block,EACB$K_MEMORY,&read_process_ast_count);
	       if (return_status != SS$_NORMAL) return (return_status);
       }
     
     }
   
/* *************** Look for IRP's ************************************** */
   
/*	Is there an IRP on UCB ?	*/

/* ???	if ( (ucb_sts & UCB$M_BSY) && (iocou != 0) )    Unit busy and IOC > 0 */

   
   if ( (ucb->ucb$l_irp != 0) && (iocou != 0) && 
	 (!( (ucb_devchar & DEV$M_NET) && (ucb_devchar & DEV$M_MBX) ) ) )
				/* IRP and IOC > 0 and not NET only device */
     {
     
     irp       = ucb->ucb$l_irp;		/* IRP			*/
     
/*	Loop through IRP's	*/

	/* Can IRP be read in Kernel mode and a PID ? */
     
     if ( ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL) &&
			  irp->irp$l_pid != 0 ) )
       {
       while ( (iocou > 0) && (ucb == irp->irp$l_ucb) )
					/* I/O count > 0 and this UCB */
         {
         
         if ( irp->irp$b_type == DYN$C_IRP )		/* IRP ?	 */
           {
           irp_addr[irp_count]   = (int) irp;		/* IRP address   */
           irp_efn[irp_count]    = irp->irp$b_efn;
           irp_func[irp_count]   = irp->irp$l_func;
           irp_iost1[irp_count]  = irp->irp$l_iost1;
           irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
           irp_bcnt[irp_count]   = irp->irp$l_bcnt;
           irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
           irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
           irp_p1[irp_count]     = irp->irp$q_qio_p1;
           irp_p2[irp_count]     = irp->irp$q_qio_p2;
           irp_p3[irp_count]     = irp->irp$q_qio_p3;
           irp_p4[irp_count]     = irp->irp$q_qio_p4;
           irp_p5[irp_count]     = irp->irp$q_qio_p5;
           irp_p6[irp_count]     = irp->irp$q_qio_p6;
           
           irp_pid[irp_count]    = irp->irp$l_pid;
           irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
						/* Convert IPID to EPID  */
           
/*	Read IOSB, use PID from IRP, it might be from a different process */
           
           return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
	           irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
	           &read_process_ast_count);
           }

         iocou     = iocou - 1;			/* Next I/O		*/
         irp_count = irp_count + 1;		/* IRP count		*/
         irp       = irp->irp$l_ioqfl;		/* Next IRP 		*/
         
         if (! ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL) ) )
            break;			/* Can IRP be read in Kernel mode ? */
         
         
         } /* End while iocou */
       }
     
     } /* End if IRP and IOC > 0 and not NET device */
   
   ucb_ioqfl_a = (IRP *) &ucb->ucb$l_ioqfl; /* IO queue forward link address */
   ucb_ioqfl   = ucb->ucb$l_ioqfl;	 	/* IO queue forward link */
   
   irp       = ucb_ioqfl;			/* IRP		*/

   
/*	Scan the IO queue 	*/
   
   while ( (ucb_ioqfl_a != ucb_ioqfl) && (irp_count < MAX_IRP) )
     {
     if (! ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL) ) )
        return (SS$_ACCVIO);    /* Can IRP be read in Kernel mode ? */
     
     if ( irp->irp$b_type == DYN$C_IRP )		/* IRP ?	*/
       {
       irp_addr[irp_count]   = (int) irp;		/* IRP address */
       irp_efn[irp_count]    = irp->irp$b_efn;
       irp_func[irp_count]   = irp->irp$l_func;
       irp_iost1[irp_count]  = irp->irp$l_iost1;
       irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
       irp_bcnt[irp_count]   = irp->irp$l_bcnt;
       irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
       irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
       irp_p1[irp_count]     = irp->irp$q_qio_p1;
       irp_p2[irp_count]     = irp->irp$q_qio_p2;
       irp_p3[irp_count]     = irp->irp$q_qio_p3;
       irp_p4[irp_count]     = irp->irp$q_qio_p4;
       irp_p5[irp_count]     = irp->irp$q_qio_p5;
       irp_p6[irp_count]     = irp->irp$q_qio_p6;
       
       irp_pid[irp_count]    = irp->irp$l_pid;
       irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
					/* Convert IPID to EPID  */
       
/*	Use PID from IRP, might be from a different process	*/
       
       return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
	       irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
	       &read_process_ast_count);
       }
     else
        break;				/* No, should not happen */
     
     irp_count = irp_count + 1;		/* Next IRP		*/
     irp       = irp->irp$l_ioqfl;	/* Next IRP		*/
     ucb_ioqfl = irp;			/* Update queue pointer	*/
     
     } /* End while */

/* *********** LAN devices have an own IRP waiting queue ****************** */


   if ( (ucb_devchar & DEV$M_NET) && !(ucb_devchar & DEV$M_MBX) ) 
					/* LAN device ? , not a NET device */
     {
     ucbni       = (UCBNI *) ucb;			/* UCB address	*/
     ucb_ioqfl_a = (IRP *) &ucbni->ucb$q_ni_rcvreq;	/* NET dev queue addr */
     ucb_ioqfl   = (IRP *)  ucbni->ucb$q_ni_rcvreq;	/* NET device queue   */
     
     irp         = ucb_ioqfl;			/* IRP		*/

/*	Scan the LAN device UCB$Q_NI_RCVREQ queue	*/
     
     while ( (ucb_ioqfl_a != ucb_ioqfl) && (irp_count < MAX_IRP) &&
	     (ucb_ioqfl != 0) && (__PAL_PROBER (irp, 4, PR$C_PS_KERNEL)) )
       {
       if (! ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL) ) )
          return (SS$_ACCVIO);    /* Can IRP be read in Kernel mode ? */
       
       if ( irp->irp$b_type == DYN$C_IRP )		/* IRP ?	*/
         {
         irp_addr[irp_count]   = (int) irp;		/* IRP address */
         irp_efn[irp_count]    = irp->irp$b_efn;
         irp_func[irp_count]   = irp->irp$l_func;
         irp_iost1[irp_count]  = irp->irp$l_iost1;
         irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
         irp_bcnt[irp_count]   = irp->irp$l_bcnt;
         irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
         irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
         irp_p1[irp_count]     = irp->irp$q_qio_p1;
         irp_p2[irp_count]     = irp->irp$q_qio_p2;
         irp_p3[irp_count]     = irp->irp$q_qio_p3;
         irp_p4[irp_count]     = irp->irp$q_qio_p4;
         irp_p5[irp_count]     = irp->irp$q_qio_p5;
         irp_p6[irp_count]     = irp->irp$q_qio_p6;
         
         irp_pid[irp_count]    = irp->irp$l_pid;
         irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
					/* Convert IPID to EPID  */
         
/*	Use PID from IRP, might be from a different process	*/
         
         return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
	         irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
	         &read_process_ast_count);
         }
       else
          break;				/* No, should not happen */
       
       irp_count = irp_count + 1;		/* Next IRP		*/
       irp       = irp->irp$l_ioqfl;		/* Next IRP		*/
       ucb_ioqfl = irp;				/* Update queue pointer	*/
       
       } /* End while 'UCB$Q_NI_RCVREQ' */


     } /* End DEV$M_NET */


/* *********** UCX BG devices have an own IRP field ****************** */


   if ( (iocou != 0) && (ddb_name[0] == 0x42) && (ddb_name[1] == 0x47) )
					/* BG device ? and IOC > 0 */
     {
     ucbbg       = (UCBBG *) ucb;		/* UCB address	*/

     irp = (IRP *) ucbbg->ucb$l_bg_irp;		/* IRP		*/

     if ( irp == 0 )
       irp = (IRP *) ucbbg->ucb$l_bg_irp2;	/* IRP 		*/

/* \\\\\ !!!! There is ucb$l_bg_irp3 too !!!!! \\\\\\\\\\\\\\\\\ */

     if (irp != 0)
       {
       if (! ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL) ) )
          return (SS$_ACCVIO);    /* Can IRP be read in Kernel mode ? */
       
       if ( irp->irp$b_type == DYN$C_IRP )		/* IRP ?	*/
         {
         irp_addr[irp_count]   = (int) irp;		/* IRP address */
         irp_efn[irp_count]    = irp->irp$b_efn;
         irp_func[irp_count]   = irp->irp$l_func;
         irp_iost1[irp_count]  = irp->irp$l_iost1;
         irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
         irp_bcnt[irp_count]   = irp->irp$l_bcnt;
         irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
         irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
         irp_p1[irp_count]     = irp->irp$q_qio_p1;
         irp_p2[irp_count]     = irp->irp$q_qio_p2;
         irp_p3[irp_count]     = irp->irp$q_qio_p3;
         irp_p4[irp_count]     = irp->irp$q_qio_p4;
         irp_p5[irp_count]     = irp->irp$q_qio_p5;
         irp_p6[irp_count]     = irp->irp$q_qio_p6;
         
         irp_pid[irp_count]    = irp->irp$l_pid;
         irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
					/* Convert IPID to EPID  */
         
/*	Use PID from IRP, might be from a different process	*/
         
         return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
	         irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
	         &read_process_ast_count);
       
         irp_count = irp_count + 1;		/* Next IRP		*/
         }

       } /* if IRP != 0 */
       
     } /* End BG device */


/* *********** UCX INET devices use CXB's ****************************	*/
/*
 * 	The pointer to an INET IRP is in the BGn: IRP field
 *	IRP$Q_ACB64_ASTPRM = CXB, where the CXB$L_IRP field
 * 	contains the address of the IRP.
 *
*/

   if ( (iocou != 0) && (ddb_name[0] == 'I') && (ddb_name[1] == 'N') &&
	(ddb_name[2] == 'E') && (ddb_name[3] == 'T') )
					/* INET device ? and IOC > 0 */
     {

/* *** Get BG0: UCB *** */

   if (ucb_bg0 == 0)		/* BG0: not yet defined	*/
    {
    ddb_bg0[0] = 'B';
    ddb_bg0[1] = 'G';
    ddb_bg0[2] = '\0';			/* Terminator	*/

    sys_lock (SCHED, 1, &orig_ipl);		/* Get SCHED lock       */

    for (ddb = (DDB *) IOC$GL_DEVLIST; ddb != NULL; ddb = (DDB *) ddb->ddb$l_link )
      {
      if (!strcmp(ddb_bg0, ddb->ddb$t_name_str)) break;    /* Found DDB */
      }

    if (ddb != NULL) ucb_bg0 = (UCB *) ddb->ddb$l_ucb;	 /* BG0: UCB address */

    sys_unlock (SCHED, orig_ipl, 0);		/* Release SCHED lock   */

    } /* End if ucb_bg0 == 0 */

/* ************ Scan trough all BGn: devices for INET IRP's *********** */
/*
 *	This is necessary because we don't now which BG's are
 *	using this INET device
 */

    if (ucb_bg0 != 0)				/* If BG devices	*/
    {

    sys_lock (SCHED, 1, &orig_ipl);              /* Get SCHED lock       */

    for (ucb_2 = ucb_bg0->ucb$l_link; ucb_2 != NULL; ucb_2 = ucb_2->ucb$l_link)
      {

      ucbbg = (UCBBG *) ucb_2;			/* BG0: UCB address 	*/

      irp = (IRP *) ucbbg->ucb$l_bg_irp;		/* IRP		*/

      if ( irp == 0 )
        irp = (IRP *) ucbbg->ucb$l_bg_irp2;	/* IRP 		*/

/* \\\\\ !!!! There is ucb$l_bg_irp3 too !!!!! \\\\\\\\\\\\\\\\\ */


      if ( (irp != 0) && ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL)) )
       {
       
       if ( irp->irp$b_type == DYN$C_IRP )		/* IRP ?	*/
	{

/*** Scan CXB's  ****/

	for ( cxb = (CXB *) irp->irp$q_acb64_astprm; cxb != NULL; cxb = cxb->cxb$l_link )
	{

        if ( __PAL_PROBER (cxb->cxb$l_irp, 4, PR$C_PS_KERNEL) ) 
            			/* Can CXB be read in Kernel mode ? */
	 {

         if ( cxb->cxb$b_type == DYN$C_CXB )		/* CXB ?	*/
	  {

          irp = cxb->cxb$l_irp;		/* INET IRP on this BGn:	*/

          if ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL) ) 
          				/* Can IRP be read in Kernel mode ? */
	   {

           if ( (irp->irp$b_type == DYN$C_IRP) && (irp->irp$l_ucb == ucb) )
					/* IRP and this INET UCB ?	*/
            {
            irp_addr[irp_count]   = (int) irp;		/* IRP address */
            irp_efn[irp_count]    = irp->irp$b_efn;
            irp_func[irp_count]   = irp->irp$l_func;
            irp_iost1[irp_count]  = irp->irp$l_iost1;
            irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
            irp_bcnt[irp_count]   = irp->irp$l_bcnt;
            irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
            irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
            irp_p1[irp_count]     = irp->irp$q_qio_p1;
            irp_p2[irp_count]     = irp->irp$q_qio_p2;
            irp_p3[irp_count]     = irp->irp$q_qio_p3;
            irp_p4[irp_count]     = irp->irp$q_qio_p4;
            irp_p5[irp_count]     = irp->irp$q_qio_p5;
            irp_p6[irp_count]     = irp->irp$q_qio_p6;
         
            irp_pid[irp_count]    = irp->irp$l_pid;
            irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
					/* Convert IPID to EPID  */
         
/*	Use PID from IRP, might be from a different process	*/
         
            return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
	         irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
	         &read_process_ast_count);

   	    irp_unit[irp_count]   = ucb_2->ucb$w_unit;	/* BG unit number */
       
            irp_count = irp_count + 1;		/* Next IRP		*/

            } /* End of irp = irp$b_type and INET ucb */
           } /* End of PROBER irp */
          } /* End of cxb = cxb$b_type */
         } /* End of PROBER cxb */
	} /* End for cxb */
	} /* End of irp = irp$b_type */
       } /* if IRP != 0 */
      } /* End for ucb_2 */

      sys_unlock (SCHED, orig_ipl, 0);		/* Release SCHED lock   */

      } /* End if ucb_bg0 != 0 */
     } /* End INET device */

   
/* ********* Mailbox has an own read IO queue ****************************** */


   if ( (ucb_devchar & DEV$M_MBX) && (ucb_devtype == DT$_MBX) ) /* Mailbox ? */
     {
     ucb_ioqfl_a = (IRP *) &mb_ucb->ucb$l_mb_readqfl; /* Mailbox read queue addr */
     ucb_ioqfl   = mb_ucb->ucb$l_mb_readqfl;	/* Mailbox read queue     */
     
     irp         = ucb_ioqfl;			/* IRP		*/

/*	Scan the Mailbox UCB$L_MB_READQFL queue	*/
     
     while ( (ucb_ioqfl_a != ucb_ioqfl) && (irp_count < MAX_IRP) )
       {
       if (! ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL) ) )
          return (SS$_ACCVIO);    /* Can IRP be read in Kernel mode ? */
       
       if ( irp->irp$b_type == DYN$C_IRP )		/* IRP ?	*/
         {
         irp_addr[irp_count]   = (int) irp;		/* IRP address */
         irp_efn[irp_count]    = irp->irp$b_efn;
         irp_func[irp_count]   = irp->irp$l_func;
         irp_iost1[irp_count]  = irp->irp$l_iost1;
         irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
         irp_bcnt[irp_count]   = irp->irp$l_bcnt;
         irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
         irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
         irp_p1[irp_count]     = irp->irp$q_qio_p1;
         irp_p2[irp_count]     = irp->irp$q_qio_p2;
         irp_p3[irp_count]     = irp->irp$q_qio_p3;
         irp_p4[irp_count]     = irp->irp$q_qio_p4;
         irp_p5[irp_count]     = irp->irp$q_qio_p5;
         irp_p6[irp_count]     = irp->irp$q_qio_p6;
         
         irp_pid[irp_count]    = irp->irp$l_pid;
         irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
					/* Convert IPID to EPID  */
         
/*	Use PID from IRP, might be from a different process	*/
         
         return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
	         irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
	         &read_process_ast_count);
         }
       else
          break;				/* No, should not happen */
       
       irp_count = irp_count + 1;		/* Next IRP		*/
       irp       = irp->irp$l_ioqfl;		/* Next IRP		*/
       ucb_ioqfl = irp;				/* Update queue pointer	*/
       
       } /* End while 'UCB$L_MB_READQFL' */
     
     
/* **** Mailbox has own BUFIO queue at UCB$L_MB_MSGQFL / UCB$L_MB_MSGQBL *** */
     
/*	Scan the Mailbox UCB$L_MB_MSGQFL queue	*/

     
     ucb_bufiofl_a = (BUFIO *) &ucb->ucb$l_mb_msgqfl; /* MBX BUFIO que adr */
     ucb_bufiofl   = ucb->ucb$l_mb_msgqfl;	/* MBX BUFIO queue */
     
     bufio         = ucb_bufiofl;		/* BUFIO	*/

     while ( (ucb_bufiofl_a != ucb_bufiofl) && (irp_count < MAX_IRP) )
       {
       if (bufio->bufio$b_type == DYN$C_BUFIO)
         {
         
/*
 *	$BUFIODEF
 *
 * typedef struct _bufio {
 *    void *bufio$ps_pktdata;          = Ptr to the buffered data within packet
 *    void *bufio$ps_uva32;            = 32-bit ptr to user's buffer, or BUFIO$K_64 (-1)
 *    unsigned short int bufio$w_size; = Total packet size
 *    unsigned char bufio$b_type;      = Structure type, DYN$C_BUFIO
 *    char bufio$b_fill_1;	       = Function code
 *    int bufio$l_fill_2;              = IRP
 *    unsigned __int64 bufio$pq_uva64; = 64-bit Address of User Buffer
 *
 *
 *	The IRP address is in the BUFIO field BUFIO$L_FILL_2
 *
 */         

	 irp = (IRP *) bufio->bufio$l_fill_2;		/* IRP address */

	 if ( (irp != 0) && 
	      (__PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL)) )
					/* Can IRP be read in Kernel mode ? */
	 {         
         if ( irp->irp$b_type == DYN$C_IRP )		/* IRP ?	*/
           {
           irp_addr[irp_count]   = (int) irp;		/* IRP address */
           irp_efn[irp_count]    = irp->irp$b_efn;
           irp_func[irp_count]   = irp->irp$l_func;
           irp_iost1[irp_count]  = irp->irp$l_iost1;
           irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
           irp_bcnt[irp_count]   = irp->irp$l_bcnt;
           irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
           irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
           irp_p1[irp_count]     = irp->irp$q_qio_p1;
           irp_p2[irp_count]     = irp->irp$q_qio_p2;
           irp_p3[irp_count]     = irp->irp$q_qio_p3;
           irp_p4[irp_count]     = irp->irp$q_qio_p4;
           irp_p5[irp_count]     = irp->irp$q_qio_p5;
           irp_p6[irp_count]     = irp->irp$q_qio_p6;
           
           irp_pid[irp_count]    = irp->irp$l_pid;
           irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
					/* Convert IPID to EPID  */
           
/*	Use PID from IRP, might be from a different process	*/
           
           return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
	           irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
	           &read_process_ast_count);
           } /* End if IRP */
         else
            break;				/* No, should not happen */
         
       irp_count   = irp_count + 1;		/* Next IRP		*/

	 } /* End if irp != 0 */

        } /* End if DYN$C_BUFIO*/

       bufio       = bufio->bufio$ps_pktdata;	/* Next BUFIO		*/
       ucb_bufiofl = bufio;		   	/* Update queue pointer	*/
       
       } /* End while */
     


/* **** If the process is in RWMBX state, the Mailbox can have 	*/
/* **** IRP's queued at PCB$L_ASTQFL_U.				*/


     if ( pcb_efwm[0] == 2 )		/* If RWMBX 	*/
     {     

/*	Scan the PCB ASTQFL_U queue	*/

     pcb_irpfl_a = (IRP *) &pcb->pcb$l_astqfl_u;  /* PCB astqfl_u adr */
     pcb_irpfl   = (IRP *) pcb->pcb$l_astqfl_u;	  /* PCB astqfl_u     */
     
     irp = pcb_irpfl;				  /* IRP address */

     while ( (pcb_irpfl_a != pcb_irpfl) && (irp_count < MAX_IRP) )
       {

       if (! ( __PAL_PROBER (irp->irp$l_ioqfl, 4, PR$C_PS_KERNEL) ) )
					/* Can IRP be read in Kernel mode ? */
         break;				/* Cancel	*/

         if ( irp->irp$b_type == DYN$C_IRP )		/* IRP ?	*/
           {
           irp_addr[irp_count]   = (int) irp;		/* IRP address */
           irp_efn[irp_count]    = irp->irp$b_efn;
           irp_func[irp_count]   = irp->irp$l_func;
           irp_iost1[irp_count]  = irp->irp$l_iost1;
           irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
           irp_bcnt[irp_count]   = irp->irp$l_bcnt;
           irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
           irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
           irp_p1[irp_count]     = irp->irp$q_qio_p1;
           irp_p2[irp_count]     = irp->irp$q_qio_p2;
           irp_p3[irp_count]     = irp->irp$q_qio_p3;
           irp_p4[irp_count]     = irp->irp$q_qio_p4;
           irp_p5[irp_count]     = irp->irp$q_qio_p5;
           irp_p6[irp_count]     = irp->irp$q_qio_p6;
           
           irp_pid[irp_count]    = irp->irp$l_pid;
           irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
					/* Convert IPID to EPID  */
           
/*	Use PID from IRP, might be from a different process	*/
           
           return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
	           irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
	           &read_process_ast_count);

           irp_count   = irp_count + 1;		/* Next IRP		*/

           } /* End if IRP */

         else
            break;				/* Not an IRP 		*/

        irp       = irp->irp$l_ioqfl;		/* Next IRP             */
        pcb_irpfl = irp;		   	/* Update queue pointer	*/

       } /* End while */

     }     

     } /* End Mailbox */

   
/*  ******** NET Devices ************************************************* */

/*	IRP's for DECnet Phase V (DECnet-Plus)	*/

   if ( (EXE$GL_DECNET_VERSION & PHASEV))     /* Phase V ? */
     {
     
	/* IRP and IOC > 0 and NET device */
     
     if ( (ucb_devchar & DEV$M_NET) && (ucb_devchar & DEV$M_MBX) &&
        (iocou != 0) && (ddb_name[0] == 0x4E) )
       {
       
       for (m=0; m < 2; m++)
         {
         
         if (m == 0) 
            tmp = OSITP$GA_ATB;	/* DECNET OSI ATB list head	*/
         
         if (m == 1) 
            tmp = TP_NSP$GA_NSP_ATB; /* DECNET NSP ATB list head	*/
         
         atb = (ATB *) tmp;		/* DECNET ATB list head	*/
         
         if (! ( __PAL_PROBER (atb, 4, PR$C_PS_KERNEL) ) )
            return (SS$_ACCVIO);    /* Can ATB be read in Kernel mode ? */
         
         atb = (ATB *) atb->atb$l_atb;	/* First ATB address		*/
         
/* \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ */
         
         atb_end = 0;			/* Clear last ATB flag	*/
         
/*	Check if we have only one ATB	*/
         
         if (atb->atb$a_cti_next_table == 0)
            atb_end = 1;				/* Flag last ATB   */
						/* 0 = Not last	   */
						/* 1 = Last, do it */
						/* 2 = Last done   */
         
         while (	atb_end != 2 ) 			/* Last ATB */
           {
           if (! ( __PAL_PROBER (atb, 4, PR$C_PS_KERNEL) ) )
              return (SS$_ACCVIO);    /* Can ATB be read in Kernel mode ? */
           
           tmp = atb->atb$w_cti_cur_slots;	/* # of current slots	*/
           atb_slots = atb->atb$w_cti_tot_slots; 	/* # of total slots 	*/
           
/* ****	Find first OSI/NSP port from Association Block Table	*/
           
           if ( tmp > 0 )				/* Slot # > 0	*/
             {
             port_slot = (PORT_SLOT *) atb->atb$a_cti_slots;
						/* Port slot 0 address	*/
             for (k=0; k < atb_slots; k++)
               {
               port_slot = (PORT_SLOT *) ( (int) port_slot + 4);
						/* Next slot address	*/
/* Note: port_slot = port_slot + 4; Generates PORT_SLOT + 144 ??	*/
               
               if (port_slot != 0) 
                 {
                 
                 if (! ( __PAL_PROBER (port_slot, 4, PR$C_PS_KERNEL) ) )
                    return (SS$_ACCVIO);   /* Can PORT_SLOT be read in Kernel mode ? */
                 
                 if ( !(port_slot->port_slot$l_port & 1) )	/* Low bit clear */
                   {
                   port = (PORT *) port_slot->port_slot$l_port; /* OSI PORT address */
                   break;
                   }
                 }
               } /* End for k */
             
             if (! ( __PAL_PROBER (port, 4, PR$C_PS_KERNEL) ) )
                return (SS$_ACCVIO);   /* Can PORT be read in Kernel mode ? */
             
/* ***** OSI or NSP Port		*/
             
             if (m == 0) 			/* OSI	*/
               {
               port_fl_a = (PORT *) &port->port$l_osi_fl; /* OSI port Flink address */
               port_fl   = (PORT *)  port->port$l_osi_fl; /* OSI port Flink 	     */
               }
             
             if (m == 1) 			/* NSP  */
               {
               port_fl_a = (PORT *) &port->port$l_nsp_fl; /* NSP port Flink address */
               port_fl   = (PORT *)  port->port$l_nsp_fl; /* NSP port Flink         */
               }
             
/* ***** Find VCRP		*/
             
             port_count = 0;				/* Clear port counter	*/
             
             while ( (port_fl_a != port_fl) && (irp_count < MAX_IRP) )
               {
               
               port_count = port_count + 1;		/* Inc port counter	*/
               
               if (m == 0) 					/* OSI */
                 {
                 vcrp_qfl_a = (VCRP *) &port->port$l_vcrp_fl;	/* Flink address */
                 vcrp_qfl   = (VCRP *)  port->port$l_vcrp_fl;	/* VCRP address  */
                 }
               
               if (m == 1) 					/* NSP  */
                 {
                 vcrp_qfl_a = (VCRP *) &port->port$l_nsp_vcrp_fl;  /* Flink address */
                 vcrp_qfl   = (VCRP *)  port->port$l_nsp_vcrp_fl;  /* VCRP address  */
                 }
               
               vcrp       = vcrp_qfl;			/* VCRP address  */
               
/*  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	*/
/*					*/
/* ADD: if vcrp_qfl_a != vcrp_qfl	*/
/*					*/
/*  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	*/
               
               if ( vcrp != 0)
                 {
                 
                 if (! ( __PAL_PROBER (vcrp, 4, PR$C_PS_KERNEL) ) )
                    return (SS$_ACCVIO);   /* Can VCRP be read in Kernel mode ? */
                 
                 irp = (IRP *) vcrp->vcrp$l_creator_data1;	/* IRP address	*/
                 
                 if (! ( __PAL_PROBER (irp, 4, PR$C_PS_KERNEL) ) )
                    return (SS$_ACCVIO);   /* Can IRP be read in Kernel mode ? */
                 
                 if ( (pcb_pid == vcrp->vcrp$l_pid) && (irp->irp$b_type == DYN$C_IRP) && (ucb == irp->irp$l_ucb) )
				/* Our VCRP and IRP and this UCB	*/
                   {
                   
/*	 nsp_port = port->port$a_constemplatenamelist;	*/
						/* NSP Port #	*/
                   
                   idlist = (IDLIST *) port->port$a_id_list;	/* Port Id list */
                   
                   str_length = idlist->idlist$b_port_name_len;	/* PORT name length  */
                   
                   strncpy (port_name, idlist->idlist$t_port_name, str_length);
						/* Copy OSI/NSP Port name */
                   
                   eib = (EIB*) port->port$a_eib_emaa;	/* EIB address		*/
                   scl = port->port$l_tpu_connection_id;	/* SCL port ID		*/
                   
                   if (m == 0) 				/* OSI */
                      net_port = 1;				/* Flag OSI Port name there */
                   else					/* NSP */
                      net_port = 2;				/* Flag NSP Port name there */
                   
                   irp_addr[irp_count]   = (int) irp;		/* IRP address */
                   irp_efn[irp_count]    = irp->irp$b_efn;
                   irp_func[irp_count]   = irp->irp$l_func;
                   irp_iost1[irp_count]  = irp->irp$l_iost1;
                   irp_iosb_a[irp_count] = (uint64) irp->irp$pq_iosb;
                   irp_bcnt[irp_count]   = irp->irp$l_bcnt;
                   irp_ast[irp_count]    = (uint64) irp->irp$pq_acb64_ast;
                   irp_astprm[irp_count] = irp->irp$q_acb64_astprm;
                   irp_p1[irp_count]     = irp->irp$q_qio_p1;
                   irp_p2[irp_count]     = irp->irp$q_qio_p2;
                   irp_p3[irp_count]     = irp->irp$q_qio_p3;
                   irp_p4[irp_count]     = irp->irp$q_qio_p4;
                   irp_p5[irp_count]     = irp->irp$q_qio_p5;
                   irp_p6[irp_count]     = irp->irp$q_qio_p6;
                   
                   irp_pid[irp_count]    = irp->irp$l_pid;
                   irp_epid[irp_count]   = EXE_STD$CVT_IPID_TO_EPID(irp->irp$l_pid);
					/* Convert IPID to EPID  */
                   
/*	Use PID from IRP, might be from a different process	*/
                   
                   return_status = EXE$READ_PROCESS(irp_pid[irp_count],cli_length,
                   irp_iosb_a[irp_count],&irp_iosb[irp_count],EACB$K_MEMORY,
                   &read_process_ast_count);
                   
                   irp_count = irp_count + 1;	/* Next IRP		*/
                   
                   } /* End pid and IRP */
                 } /* End VRCP != 0	*/
               
               
               if (m == 0) 
                  next_port = port->port$l_osi_fl;   /* Next OSI PORT Flink address */
					     /* Note: not PORT address !    */
               if (m == 1) 
                  next_port = port->port$l_nsp_fl;   /* Next NSP PORT Flink address */
               
/* Bump the port FL pointer once ==> the last port in the queue is done too */
               
               if (port_count == 1)
                  port_fl_a = port_fl;			/* Update PORT fl address   */
               
               
/* **** Note: Last FL points to a location, not to a PORT	*/
/*	This location points to the first PORT Flink		*/
               
               if (m == 0) 
                  port = (PORT *) (next_port - 0xD8);	/* Next OSI PORT address */
               if (m == 1) 
                  port = (PORT *) (next_port - 0xBC);	/* Next NSP PORT address */
               
/*	Check if an OSI Port or the above pointer	*/
               
               if ( (port->port$b_type != DYN$C_DECNET) &&
                  ( (port->port$b_subtype != DYN$C_NET_OSITP_PORT) ||
               (port->port$b_subtype != DYN$C_NET_NSP_PORT)) )
               
                 {
                 port      = (PORT *) next_port;	   /* PORT pointer address	      */
                 next_port = port->port$l_flink;	   /* Next (first) PORT Flink address */
                 if (m == 0) 
                    port = (PORT *) (next_port - 0xD8);	/* Next OSI PORT address */
                 if (m == 1) 
                    port = (PORT *) (next_port - 0xBC);	/* Next NSP PORT address */
                 }
               
               
               if (! ( __PAL_PROBER (port, 4, PR$C_PS_KERNEL) ) )
                  return (SS$_ACCVIO);   /* Can PORT be read in Kernel mode ? */
               
               
               if (m == 0) 
                  port_fl = (PORT *) port->port$l_osi_fl;	/* OSI Port flink */
               if (m == 1) 
                  port_fl = (PORT *) port->port$l_nsp_fl;   	/* NSP PORT Flink */
               
               } /* End while fl_a != a */
             } /* End slot # > 0 */
           
           
           if (atb->atb$a_cti_next_table == 0)
              atb_end = atb_end + 1;			/* Flag last ATB   */
							/* 0 = Not last	   */
							/* 1 = Last, do it */
							/* 2 = Last done   */
           
           if (atb_end == 0)				/* Not last ATB	*/
              atb = (ATB *) atb->atb$a_cti_next_table;	/* Next ATB */
           
           
           } /* End while atb_end != 2 */
         } /* End for m	     */
       } /* End NET Device */
     
     } /* End PHASE V */
   
   
   return (SS$_NORMAL);
   } 

/* ************************************************************************* */

/* Routine get_process_registers(int index, int pid)
 *
 *
 * FUNCTION
 *
 * 	This routine gets the process registers in Kernel mode
 *
 * INPUT
 *
 *      PID process ID
 *
 * IMPLICIT INPUT
 *
 * EXPLICIT OUTPUT
 *
 *      ss$_normal      Success.  
 *	ss$_nonexpr	Nonexistent process
 *
 */

int get_process_registers (int pid)
   {
   
/* Get registers			*/
   
   return_status =	EXE$READ_PROCESS(pid,8*EACB$K_GEN_REGS_LENGTH,EACB$K_R0,
	   &prc_registers,EACB$K_GENERAL_REGISTER,&read_process_ast_count);
   
   return (return_status);
   
   }

/* ************************************************************************* */

/* Routine map_address(int addr, int pid)
 *
 *
 * FUNCTION
 *
 * 	This routine maps a given address
 *
 * INPUT
 *
 *      addr address to map
 *
 * IMPLICIT INPUT
 *
 *	LDR$GQ_IMAGE_LIST - Header of image list
 *
 * EXPLICIT OUTPUT
 *
 *      ss$_normal      Success
 *	ss$_nonexpr	Nonexistent process
 *
 */
int map_address (int addr, int pid)
   {
/*	Map the address			*/
   
   found = -1;
   psect = 0;
   kferes_offset = 0;
   
   if ( addr == 0 ) return (SS$_NORMAL);
      
/*	Address is in system space	*/
   
   if ( addr >= 0X80000000 )
     {
     tmp = (int) &LDR$GQ_IMAGE_LIST;  	     /* Address of Image Header list */
     ldrimg = (LDRIMG *) LDR$GQ_IMAGE_LIST;  /* First entry	*/
     
     found = 0;
     
/*	Scan the image list	*/
     
     while ( found == 0 )
       {
       
       if ( (int) ldrimg == tmp )
          found = -1;				/* Flag not found */
       else
         {
         
/*	Nonpaged read only	*/
         
         start = (int) ldrimg->ldrimg$l_nonpag_r_base;
         end   = start + (int) ldrimg->ldrimg$l_nonpag_r_len;
         
/* ??? \\\ if ( ( start <= addr ) && ( end >= addr ) && end != 0 )
/* 						end == 0 image is not loaded */
         
         if ( ( start <= addr ) && ( end >= addr ) )
           {
           found = 1;				/* Flag  found	*/
           offset = addr - start + ldrimg->ldrimg$l_nonpag_r_offset;
						/* Image offset	*/
           psect = 5;				/* Psect type	*/
           break;
           }
         
/*	Nonpaged read/write	*/
         
         start = (int) ldrimg->ldrimg$l_nonpag_w_base;
         end   = start + (int) ldrimg->ldrimg$l_nonpag_w_len;
         
         if ( ( start <= addr ) && ( end >= addr ) )
           {
           found = 1;				/* Flag  found	*/
           offset = addr - start + ldrimg->ldrimg$l_nonpag_w_offset;
						/* Image offset	*/
           psect = 6;				/* Psect type	*/
           break;
           }
         
         
         
/*	Paged read only	*/
         
         start = (int) ldrimg->ldrimg$l_pag_r_base;
         end   = start + (int) ldrimg->ldrimg$l_pag_r_len;
         
         if ( ( start <= addr ) && ( end >= addr ) )
           {
           found = 1;				/* Flag  found	*/
           offset = addr - start + ldrimg->ldrimg$l_pag_r_offset;
						/* Image offset	*/
           psect = 7;				/* Psect type	*/
           break;
           }
         
         
/*	Paged read/write 	*/
         
         start = (int) ldrimg->ldrimg$l_pag_w_base;
         end   = start + (int) ldrimg->ldrimg$l_pag_w_len;
         
         if ( ( start <= addr ) && ( end >= addr ) )
           {
           found = 1;				/* Flag  found	*/
           offset = addr - start + ldrimg->ldrimg$l_pag_w_offset;
						/* Image offset	*/
           psect = 8;				/* Psect type	*/
           break;
           }
         
         
         
         ldrimg = ldrimg->ldrimg$l_flink;	/* Next entry   */
         }
       
       } /* end while */
     
/*???	look at the drivers	*/
     
/*	IOC$GL_DPTLIST		list of drivers	*/	
/*	DPT$W_SIZE(R6)		size of driver	*/
     
/*???	look extra images (MSCP, TMSCP, VAXEMUL, FPEMUL)	*/
     
/*      SCS$GL_MSCP		MSCP */
/*	SLV$L_CODESIZE		size of MSCP	*/
     
/*	SCS$GL_TMSCP			*/
/*	SLV$L_CODESIZE		size of 	*/
     
     
     
     if (found == 1)
       {
       str_length = ldrimg->ldrimg$b_imgnamlen;   /* Image name length  */
       strncpy (image_name, ldrimg->ldrimg$t_imgnam, str_length);
       return (SS$_NORMAL);
       }	
     } /* end if addr >= 0X80000000	*/
   
   
/************ Address is in P0/P1 space ***********************************/
   
/*	Address was not in system address space, look for image space*/
   
   if (found == -1)
     {
     tmp  = (int) &IAC$GL_IMAGE_LIST;		/* Address of list head */
     
     
/*	Get first list head element */
     
     return_status = EXE$READ_PROCESS(pid,link_length,&IAC$GL_IMAGE_LIST,
	     &imcb_link,EACB$K_MEMORY,&read_process_ast_count);
	     if (return_status != SS$_NORMAL) return (return_status);
        
     imcb = (IMCB *) imcb_block;		/* Local IMCB array	*/
     found = 0;
     
/*	Scan the user image	*/
     
     while ( found == 0 )
       {
       if ( imcb_link == tmp )
          found = -1;				/* Flag not found */
       else
         {
         
         return_status = EXE$READ_PROCESS(pid,IMCB$K_LENGTH,imcb_link,
	         &imcb_block,EACB$K_MEMORY,&read_process_ast_count);
	         if (return_status != SS$_NORMAL) return (return_status);
            
         start = (int) imcb->imcb$l_starting_address;	/* Start address */
         end   = (int) imcb->imcb$l_end_address;	/* End address   */
         
         if ( ( start <= addr ) && ( end >= addr ) )
           {
           found = 1;                   		/* Flag  found  */
           psect = (int) imcb->imcb$b_act_code;		/* Psect type   */
           break;
           }
         
/*	Check if a sliced image */
         
         if (found != 1) 
           {
           
           if ( imcb->imcb$l_flags & IMCB$M_DISCONTIGUOUS )
             {
             kferes = (KFERES *) imcb->imcb$l_kferes_ptr;
             
/* 	Get # of resident sections */
             
             kferes_link = (int) kferes;
             
             return_status = EXE$READ_PROCESS(pid,KFERES$K_FIXED_LENGTH,kferes_link,
             &kferes_block,EACB$K_MEMORY,&read_process_ast_count);
             if (return_status != SS$_NORMAL) return (return_status);
                
             kferes = (KFERES *) kferes_block;
             
             kferes_count = kferes->kferes$l_count;
             
             kferes_link = kferes_link + KFERES$K_FIXED_LENGTH;
						/* Address of first section */
             
/*	Read KFERES section's	*/
             
             while ( kferes_count > 0 )
               {
               return_status = EXE$READ_PROCESS(pid,KFERES$K_SECTION_LENGTH,kferes_link,
               &kferes_block,EACB$K_MEMORY,&read_process_ast_count);
               if (return_status != SS$_NORMAL) return (return_status);
                  
               kferes_section = (KFERES_SECTION *) kferes_block;
               
               if ( kferes_section->kferes$l_section_type == KFERES$K_CODE )
                 {
                 start = (int) kferes_section->kferes$l_va;  /* Start address */
                 end   = start + kferes_section->kferes$l_length;  /* End address   */
                 
                 if ( ( start <= addr ) && ( end >= addr ) )
                   {
                   found = 1; 			/* Flag  found  */
                   kferes_offset = (int) kferes_section->kferes$l_image_offset;
						/* Image offset	*/
                   psect = 10;			/* Psect type System Resident Code  */
                   break;
                   }
                 
                 
                 } /* End if KFERES$K_CODE */
               
               kferes_link = kferes_link + KFERES$K_SECTION_LENGTH;
							/* Next section*/
               kferes_count = kferes_count - 1;
               
               }  /* End while kferes_count */
             
             } /* End if IMCB$M_DISCONTIGUOUS */
           
           } /* End if found != 1	*/
         
/* ********************************** */
         
/* 	Get address of next element */
         
         return_status = EXE$READ_PROCESS(pid,link_length,imcb_link,
	         &imcb_link,EACB$K_MEMORY,&read_process_ast_count);
	         if (return_status != SS$_NORMAL) return (return_status);
            
         
         } /* end else */
       
       } /* end while found */
     
     if (found == 1)
       {
       offset = addr + kferes_offset;		/* Image offset	*/
       
       if ( (int) imcb->imcb$b_act_code != IMCB$K_MAIN_PROGRAM )
          offset = offset - start;  /* Substract base if not main image */
       
       strncpy (tmp_string,imcb->imcb$t_image_name,2); /* Image name	*/
       str_length = (int) tmp_string[0];               /* Image name length  */
       strncpy (image_name, imcb->imcb$t_image_name+1, str_length);
       
       return (SS$_NORMAL);
       }	
     
/*	Address is not within a system or user image	*/
     
     if ( found <= 0 )
       {
       return_status = EXE$READ_PROCESS(pid,cli_length,&CTL$AG_CLIMAGE,
	       &cli,EACB$K_MEMORY,&read_process_ast_count);
	       if (return_status != SS$_NORMAL) return (return_status);
          
       start = cli[0];				/* Start address */
       end   = cli[1];				/* End address   */
       
       if ( ( start <= addr ) && ( end >= addr ) )
         {
         found  = 1;                          	/* Flag  found  */
         offset = addr - start; 		/* Image offset */
         
         strncpy (tmp_string, (char *) &CTL$GT_CLINAME,2); /* CLI name */
         str_length = (int) tmp_string[0];		   /* CLI name length */
         strncpy (image_name, (char *) &CTL$GT_CLINAME+1, str_length);
         
         strcat (image_name,".EXE (CLI Image, in P1 Space)");
         
         
         psect  = 2;				/* Psect type   */
         return (SS$_NORMAL);
         }
       
       }
     
     }
   
   return (return_status);
}

/* ************************************************************************* */

/* Routine get_symbol_offset ( char img_nam[] )
 *
 *
 * FUNCTION
 *
 * 	This routine gets the OSI symbol table offset
 *
 * INPUT
 *
 *      img_nam		Image name
 *
 * IMPLICIT INPUT
 *
 *	LDR$GQ_IMAGE_LIST - Header of image list
 *
 * OUTPUT
 *
 *      offset		NET$TRANSPORT_xxx.EXE symbol offset
 *
 * EXPLICIT OUTPUT
 *
 *      ss$_normal      Success
 *
 */
int get_symbol_offset ( char img_nam[] )
   {

   if (! ( __PAL_PROBER (img_nam, 4, PR$C_PS_KERNEL) ) )
      return (SS$_ACCVIO);        /* Can Image name be read in Kernel mode ? */

   tmp = (int) &LDR$GQ_IMAGE_LIST;  	     /* Address of Image Header list */
   ldrimg = (LDRIMG *) LDR$GQ_IMAGE_LIST;  /* First entry	*/
     
   found = 0;
     
/*	Scan the image list	*/
     
   while ( found == 0 )
    {
       
     if ( (int) ldrimg == tmp )
       found = -1;				/* Flag not found */
     else
       {
         
/*	Nonpaged read/write	*/
         
        if ( !(strncmp(ldrimg->ldrimg$t_imgnam, img_nam, strlen(img_nam) )) )
	  {
          found  = 1;				/* Flag  found	*/

          start  = (int) ldrimg->ldrimg$l_nonpag_w_base;
          end    = (int) ldrimg->ldrimg$l_nonpag_w_offset;
          offset = start - end;		/* STB offset	*/

          break;
          }
         
        ldrimg = ldrimg->ldrimg$l_flink;	/* Next entry   */

       } /* end else */
    } /* end while */

   return_status = found;
     
   return (return_status);
   }




/*	Dummy routine for end of code to lock	*/

void last_address ()
   {
   }

/* ******************************************************************** */


/* Routine read_stb_file ( char filnam[], char atbnam[] )
 *
 *
 * FUNCTION
 *
 *      This routine reads the xxx_ATB symbol values from the
 *	NET$TRANSPORT_xxx.STB files
 *
 * INPUT
 *
 *	filnam		File name
 *	atbnam		Symbol name
 *
 * IMPLICIT INPUT
 *
 *      None
 *
 * OUTPUT
 *
 *      sym_val		NET$TRANSPORT_xxx.EXE symbol offset
 *
 * EXPLICIT OUTPUT
 *
 *      ss$_normal      Success
 *
 */

int read_stb_file ( char filnam[], char atbnam[]  )
   {

   fab 	         = cc$rms_fab;			/* Build the FAB 	*/
   fab.fab$l_fna = filnam;			/* File name		*/
   fab.fab$b_fns = strlen(fab.fab$l_fna);	/* File name length  	*/
   fab.fab$l_dna = filnam;			/* File name            */
   fab.fab$b_dns = strlen(fab.fab$l_dna);	/* File name length     */
   fab.fab$b_fac = FAB$M_GET;		
   fab.fab$b_shr = FAB$M_SHRGET;		/* Allow shared read 	*/


   rab 	         = cc$rms_rab;			/* Build the RAB */
   rab.rab$l_fab = &fab;
   rab.rab$l_ubf = inbuf;
   rab.rab$w_usz = BUFFER_LENGTH - 1;
   rab.rab$l_rbf = inbuf;

   return_status = SYS$OPEN(&fab);		/* Open input file */
     if (!(return_status&1)) return (return_status);

   return_status = SYS$CONNECT(&rab);		/* Connect input RAB to FAB */
     if (!(return_status&1)) return (return_status);

/* ******************************************************************* */

   eobjrec = (struct eobjrecdef *) &inbuf[0];
   egs     = (struct egsdef *) &inbuf[0];

   eobjrec->eobj$w_rectyp = EOBJ$C_EMH;	/* Clear EEOM	*/

   while ( eobjrec->eobj$w_rectyp != EOBJ$C_EEOM )
     {

      return_status = SYS$GET(&rab);		/* Get first record from input file */
      if (!(return_status&1)) return (return_status);

/* ********  */

      egst = (struct egstdef *) (&inbuf[0] + GSD_SIZ); /* Point to first subrecord */


      if ( eobjrec->eobj$w_rectyp == EOBJ$C_EGSD )
       {
       if ( egs->egsd$w_gsdtyp == EGSD$C_SYMG )
        {
        size = eobjrec->eobj$w_size;		/* GSD Record size	*/
        size = size - GSD_SIZ;			/* less header record	*/

        while (size > 0 )
          {
          if ( !(strncmp(egst->egst$t_name, atbnam, strlen(atbnam) )) )
           {
/*	printf("Symbol name : %s \n",  egst->egst$t_name);	*/
/*	fprintf(outfile, "Symbol name : %s \n",  egst->egst$t_name);	*/

/*	printf(" Value      : %X \n",  egst->egst$l_value);	*/
/*	fprintf(outfile, " Value      : %X \n",  egst->egst$l_value);	*/


           sym_val = egst->egst$l_value;	 /* Symbol value	*/
           eobjrec->eobj$w_rectyp = EOBJ$C_EEOM; /* Flag "end of file"	*/
           }
        size = size - egst->egst$w_size;	/* GSD Subrecord size	*/
        egst = (struct egstdef  *) ((int)egst + egst->egst$w_size);
						/* Point to next subrecord */
        } /* End while size     */
       } /* End if EGSD$C_SYMG  */
     } /* End if EOBJ$C_EGSD    */
   } /* End while EOBJ$C_EEOM   */
                                    
   return_status = SYS$CLOSE(&fab);	/* Close input file */

   return_status = SS$_NORMAL;		/* Success */
   return (return_status);
}

