#pragma module DQDRIVER "X-1"

/************************************************************************/
/*									*/
/* Copyright © Digital Equipment Corporation, 1994 All Rights Reserved.	*/
/* Unpublished rights reserved under the copyright laws of the United	*/
/* States.								*/
/*									*/
/* The software contained on this media is proprietary to and embodies	*/
/* the confidential technology of Digital Equipment Corporation.	*/
/* Possession, use, duplication or dissemination of the software and	*/
/* media is authorized only pursuant to a valid written license from	*/
/* Digital Equipment Corporation.					*/
/*									*/
/* RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure by the	*/
/* U.S. Government is subject to restrictions as set forth in		*/
/* Subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR 52.227-19,	*/
/* as applicable.							*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/* Facility:								*/
/*	IDE Disk Driver							*/
/*									*/
/* Abstract:								*/
/*	This driver controls a standard IDE disk.			*/
/*									*/
/* Author:								*/
/*	Benjamin J. Thomas III / May 1994				*/
/*									*/
/* Dedication:								*/
/*									*/
/* My brother-in-law and nephew were killed in a small plane crash just	*/
/* off Nantucket island, on June 6, 1994, shortly after I started	*/
/* writing this driver.  This effort is dedicated to the memory of	*/
/* of Reginald Marden and Christopher Marden.  They will be missed.	*/
/*									*/
/*									*/
/* Revision History:							*/
/*									*/
/*	X-1		Benjamin J. Thomas III		May, 1994	*/
/*		Initial version.					*/
/*									*/
/************************************************************************/

/* Miscellaneous tidbits						*/
/*									*/
/*	o Always check the BUSY bit.  If set, none of the other bits	*/
/*	  can be fully trusted.  If clear, it's ok to believe the other	*/
/*	  bits and to issue commands.					*/
/*									*/
/*	o Make sure the correct drive is selected before any action.	*/
/*	  This means checking the BUSY bit for clear, then selecting	*/
/*	  the desired driver, then checking BUSY again for clear for	*/
/*	  that drive.  Note that VMS allows a channel concept, and this	*/
/*	  driver obtains ownership of the channel.  Therefore, we don't	*/
/*	  have to be quite so paranoid about ownership changes.  Get	*/
/*	  the channel, then get ownership...release the channel when	*/
/*	  all done.							*/
/*									*/
/*	o It's a good idea to keep sector count under 128.  This is	*/
/*	  just a caution against drives that may not handle the number	*/
/*	  as unsigned.							*/
/*									*/
/*	o Read ALT_STS for the status bits unless the direct intent is	*/
/*	  also to clear the interrupt					*/
/*									*/
/*	o Read/Write multiple commands don't seem to work very well	*/
/*	  all the time.  Based on advice, those commands seem aimed at	*/
/*	  treating the disk as if it had a different sector size.  This	*/
/*	  driver uses the simple read/write commands.  This means an	*/
/*	  interrupt per sector, which can't be avoided.  A DMA version	*/
/*	  could possibly help this (someday).  For now, read/writes	*/
/*	  will be done with the simple commands, and not exceeding 127	*/
/*	  blocks at a time (see previous bullet).			*/
/*									*/
/*									*/
/* Basic transfer algorithm						*/
/*									*/
/*	1) Use ALT_STATUS to check for BUSY = 0				*/
/*			Error -> RESET, then retry once			*/
/*	2) Select the proper drive					*/
/*	3) Check ALT_STATUS again, for BUSY=0, DRDY = 1			*/
/*			Error -> RESET, retry from step 1		*/
/*	4) Write the CSRs						*/
/*	5) Write the command						*/
/*	6) Wait for interrupt						*/
/*			Timeout -> RESET, retry from step 3		*/
/*	7) Read STATUS (which clears interrupt)				*/
/*	8) Transfer data is DRQ=1, BUSY=0				*/
/*			Error ->  DRQ=0 -> goto step 9 (ERR should =1)	*/
/*				  BUSY=1-> RESET, retry from step 3	*/
/*	9) Check that STATUS (saved from step 7) has ERR=0		*/
/*			Error -> handle error				*/
/*	10) If not done, goto step 6 (multisector transfers)		*/
/*	    If done, goto step 3 (next command/transfer)		*/
/*									*/
/* A write would modify as:						*/
/*									*/
/*	5a) Check ALT_STATUS for BUSY=0, DRQ=1				*/
/*	5b) Send all of the data					*/
/*	Remove step 8							*/
/*									*/
/* Powerup algorithm							*/
/*									*/
/*	1) Poll ALT_STATUS for up to 40 seconds for BUSY=0, DRDY=1	*/
/*			Error -> Fatal					*/
/*	2) Select drive 0						*/
/*	3) Read ALT_STATUS for BUSY=0, DRDY=1				*/
/*			Error -> Fatal					*/
/*	4) Drive 0 exists						*/
/*	5) Select drive 1						*/
/*	6) Read ALT_STATUS for BUSY=0, DRDY=1				*/
/*			Error->BUSY=1 fatal master/slave incompatability*/
/*			       DRDY=0  no drive 1			*/
/*	7) drive 1 exists						*/

/* Build instructions:							*/
/* ===================							*/
/*									*/
/*	$ CC_OPT = "/DEFINE=(BASEALIGN_SUPPORT)"			*/
/*	$ IF P1 .NES. ""						*/
/*	$ THEN								*/
/*	$  IF P1 .NES. "DEBUG" THEN EXIT %X14         ! SS$_BADPARAM	*/
/*	$  CC_OPT = "/DEBUG/NOOPTIMIZE/DEFINE=(DEBUG,BASEALIGN_SUPPORT)" */
/*	$ ENDIF								*/
/*	$								*/
/*	$! Compile the driver						*/
/*	$								*/
/*	$ CC/STANDARD=RELAXED_ANSI89/INSTRUCTION=NOFLOATING_POINT -	*/
/*		/EXTERN=STRICT 'CC_OPT' -				*/
/*		/LIS=DQDRIVER/MACHINE_CODE/OBJ=DQDRIVER -		*/
/*		DQDRIVER+SYS$LIBRARY:SYS$LIB_C.TLB/LIBRARY		*/
/*	$								*/
/*	$! Link the driver						*/
/*	$								*/
/*	$ LINK/ALPHA/USERLIB=PROC/NATIVE_ONLY/BPAGE=14/SECTION/REPLACE-	*/
/*	        /NODEMAND_ZERO/NOTRACEBACK/SYSEXE/NOSYSSHR-		*/
/*	        /SHARE=SYS$DQDRIVER-               ! Driver image	*/
/*	        /DSF=SYS$DQDRIVER-                 ! Debug symbol file	*/
/*	        /SYMBOL=SYS$DQDRIVER-              ! Symbol table	*/
/*	        /MAP=SYS$DQDRIVER/FULL/CROSS -     ! Map listing	*/
/*	        SYS$INPUT:/OPTIONS					*/
/*	SYMBOL_TABLE=GLOBALS						*/
/*	CLUSTER=VMSDRIVER,,,-						*/
/*	        DQDRIVER.OBJ,-						*/
/*	        SYS$LIBRARY:VMS$VOLATILE_PRIVATE_INTERFACES/INCLUDE=(BUGCHECK_CODES)/LIB,- */
/*	        SYS$LIBRARY:STARLET/INCLUDE:(SYS$DRIVER_INIT,SYS$DOINIT) */
/*	COLLECT=NONPAGED_EXECUTE_PSECTS/ATTRIBUTES=RESIDENT,-		*/
/*	        $CODE$							*/
/*	PSECT_ATTR=$LINK$,WRT						*/
/*	PSECT_ATTR=$INITIAL$,WRT					*/
/*	PSECT_ATTR=$LITERAL$,NOPIC,NOSHR,WRT				*/
/*	PSECT_ATTR=$READONLY$,NOPIC,NOSHR,WRT				*/
/*	PSECT_ATTR=$$$105_PROLOGUE,NOPIC				*/
/*	PSECT_ATTR=$$$110_DATA,NOPIC					*/
/*	PSECT_ATTR=$$$115_LINKAGE,WRT					*/
/*	COLLECT=NONPAGED_READWRITE_PSECTS/ATTRIBUTES=RESIDENT,$PLIT$, -	*/
/*              $INITIAL$,$GLOBAL$,$OWN$,$$$105_PROLOGUE,$$$110_DATA,	*/
/*		$$$115_LINKAGE,$BSS$,$DATA$,$LINK$,$LITERAL$,$READONLY$	*/
/*	PSECT_ATTR=EXEC$INIT_CODE,NOSHR					*/
/*	COLLECT=INITIALIZATION_PSECTS/ATTRIBUTES=INITIALIZATION_CODE,-	*/
/*	        EXEC$INIT_000, EXEC$INIT_001,EXEC$INIT_002,-		*/
/*		EXEC$INIT_CODE,EXEC$INIT_LINKAGE,EXEC$INIT_SSTBL_000,-	*/
/*		EXEC$INIT_SSTBL_001,EXEC$INIT_SSTBL_002			*/
/*	$QUIT:								*/
/*	$ EXIT $STATUS							*/

/* Usage Instructions:							*/
/* ===================							*/
/*									*/
/* This driver was written with a $19 ISA IDE controller plugged into	*/
/* an ISA bus.  In addition, on the Digital AlphaStation 400 4/233,	*/
/* there is a built-in (but not supported) IDE controller.  In either	*/
/* case, you need to make sure that everything is set up.		*/
/*									*/
/* For the Digital AlphaStation 400 4/233, you must run the ENABLE-IDE	*/
/* progam (from the Freeware CD) to enable the IDE port.  This port	*/
/* will use IRQ 14 and is on the ISA bus.  You may need to run the	*/
/* console's ISACFG utility to reserve IRQ 14 for the device.  For a	*/
/* separate ISA board, there is no need to do any special hardware	*/
/* setup.								*/
/*									*/
/*									*/
/* A sample ISACFG line, might look like:				*/
/*									*/
/* >>>isacfg -mk -slot 3 -dev 0 -iobase 1f0 -irq0 14 -etyp 1 -enadev 1	-handle IDE */
/*									*/
/* To load the driver, use the following command:			*/
/*									*/
/*	$ MC SYSMAN							*/
/*	SYSMAN> IO CONNECT DQA0:/DRIVER=SYS$DQDRIVER/Node=n/CSR=%X1F0 -	*/
/*		/Adap=x/Vector=y					*/
/*									*/
/* This command assumes (or requires):					*/
/*									*/
/*	CSR is %X1F0, this is the standard address for a primary IDE	*/
/*	IRQ is usually, 14, so vector y = 4 * IRQ = 56			*/
/*	Adapter is the ISA adapter, found from the TR number of the	*/
/*		ISA adapter from SYSMAN IO SHOW BUS.  This is the "TR"	*/
/*	node is the slot number into which the board was plugged.	*/

/* Caveats:								*/
/*									*/
/* This driver was written from the X3T9.2 specs, which may or may not	*/
/* accurately reflect working drives. The driver has been tested on	*/
/* a few different IDE drives, but the following are known items to	*/
/* watch out for:							*/
/*									*/
/*	o Driver has never been tested (nor does it support) DMA	*/
/*	o Driver has been used only with 16 bit data interfaces		*/
/*	o Driver hasn't yet been tested with 2 drives (units 0 & 1)	*/
/*									*/
/* I'm sure that there are others - good luck.				*/
/*									*/
/* One obvious place for improvement is the copying of data to and from	*/
/* the transfer buffer.  It would be better to move the data directly	*/
/* to/from the user buffer, but there were some reasons that I didn't.	*/
/* First, it's just plain a pain to get it right.  You have to account	*/
/* for the alignment issues;  this is no good if you are constantly	*/
/* taking alignment faults or if you are constantly loading/merging	*/
/* data.  Secondly, you need to always empty/fill the disk's buffer	*/
/* and that means handling non-sector-sized counts.  Overall, not too	*/
/* hard, but a pain.  Finally, if you *really* want performance, you 	*/
/* should just move to DMA, not programmed I/O.				*/

/* Registers - Here's my register cheat sheet.  The CSRs are expressed as ISA offset and then		*/
/* the offsets for two of the ISA machines that I tried it on.						*/
/*													*/
/* +=====-=====-=====-=====-=====-=====-=====-=====+							*/
/* |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |							*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* | BSY | DRDY| DWF | DSC | DRQ |CORR | IDX | ERR |	3F6 (1FB00) [7EC0] R	Alternate Status	*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* |  x  |   x |  x  |  x  |  1  | SRST| nIEN|  0  |	3F6 (1FB00) [7EC0] W	Device control 		*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* | HiZ | nWTG| nHS3| nHS2| nHS1| nHS0| nDS1| nDS0|	3F7 (1FB80) [7EE0] R	Drive address 		*/
/* +===============================================+							*/
/* |						   |	1F0 (F820)  [3E08] RW	Data (16 bits)		*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* | BBK | UNC | MC  | IDNF| MCR |ABRT |TK0NF|AMNF |	1F1 (F880)  [3E20] R	Error	 		*/
/* +===============================================+							*/
/* |					           |	1F1 (F880)  [3E20] W	Features		*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* |					           |	1F2 (F900)  [3E40] RW	Sector count		*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* |					           |	1F3 (F980)  [3E60] RW	Sector number (LBA0-7)	*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* |					           |	1F4 (FA00)  [3E80] RW	Cylinder low (LBA8-15)	*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* |						   |	1F5 (FA80)  [3EA0] RW	Cylinder high(LBA16-23)	*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* |  1  |  L  |  1  | DRV | HS3 | HS2 | HS1 | HS0 |	1F6 (FB00)  [3EC0] RW	Drive/head (LBA24-27)	*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* | BSY | DRDY| DWF | DSC | DRQ |CORR | IDX | ERR |	1F7 (FB80)  [3EE0] R	Status			*/
/* |-----+-----+-----+-----+-----+-----+-----+-----|							*/
/* |					           |	1F7 (FB80)  [3EE0] W	Command			*/
/* +===============================================+							*/

/* Define system data structure types and constants */

#include	ccbdef			/* Channel control block */
#include	crbdef			/* Controller request block */
#include	cramdef			/* Controller register access method */
#include	dcdef			/* Device codes */
#include	ddbdef			/* Device data block */
#include	ddtdef			/* Driver dispatch table */
#include	devdef			/* Device characteristics */
#include	dptdef			/* Driver prologue table */
#include	embdvdef		/* Error log entry for devices */
#include	fdtdef			/* Function decision table */
#include	fkbdef			/* Fork block */
#include	idbdef			/* Interrupt data block */
#include	iocdef			/* IOC constants */
#include	iodef			/* I/O function codes */
#include	irpdef			/* I/O request packet */
#include	orbdef			/* Object rights block */
#include	pagedef			/* Get page definitions and disk block size */
#include	pcbdef			/* Process control block */
#include	ptedef			/* Page Table entry definitions */
#include	ssdef			/* System service status codes */
#include	stddef			/* Common definitions */
#include	stsdef			/* Status value fields */
#include	ucbdef			/* Unit control block */
#include	vadef			/* Virtual address definitions */

/* Define function prototypes for system routines */

#include	acp_routines		/* ACP$ and ACP_STD$ routines */
#include	erl_routines		/* erl$ and erl_std$ routines */
#include	exe_routines		/* exe$ and exe_std$ routines */
#include	ioc_routines		/* ioc$ and ioc_std$ routines */
#include	ldr_routines		/* ldr$ and ldr_std$ routines */
/*#include	sch_routines		/* sch$ and sch_std$ routines */

/* Define various device driver macros */

#include	vms_drivers		/* Device driver support macros */
#include	vms_macros		/* Define bug_check and such */

/* Define the DEC C functions used by this driver */

#include	builtins		/* C builtin functions */
#include	string			/* String rtns from "kernel CRTL" */

/* Define some useful types */

typedef unsigned short int WORD;	/* Define a WORD (16 bits) */
typedef unsigned char BYTE;		/* Define a BYTE (8 bits) */
typedef unsigned int UINT;		/* Usigned int (32 bits) */

/* Define constants specific to this driver */

enum {				/* Timeout times */
    TIMEOUT_TIME=   10,			/* I/O Timeout time (seconds) */
    DRQ_TIME	= 1000*1000,		/* DRQ wait time (1 millisecond) */
    RESET_TIME	=    2,			/* Reset time (seconds) */
    READY_TIME	= 1000*100		/* Ready time (100 microseconds) */
};

enum {				/* Miscellaneous constants: */
    NUMBER_CRAMS=   18,			/* Number of CRAMs needed */
    MODEL_LENGTH=   40,			/* Model string length */
    ERR_BYTES	=   EMB$C_DV_LENGTH+12+5+8,	/* Size of error log buffer (in bytes) */
    RDSTATS_LEN	=   (TIMEOUT_TIME + 9) * 4,	/* Size of RDSTATS_LEN buffer */
    MAX_RETRY	=    8			/* Maximum number of retries */
};

enum {				/* Controller constants */
    DEVICE_IPL	=   21			/* IPL of device */
};

enum {				/* Geometry and transfer constants */
    MAX_SECTOR	=  256,			/* Max sectors per track */
    MAX_HEAD	=   16,			/* Max heads per cylinder */
    MAX_CYLINDER= 8192,			/* Max cylinders per drive */
    MAX_XFER	=  127,			/* Max transfer size (sectors) */
    BLK_SIZE	= IOC$C_DISK_BLKSIZ,	/* Size of a disk block (in bytes) */
    BLK_MASK	= IOC$M_BLOCK_BYTEMASK,	/* "Byte within block" mask */
    BLK_SHIFT	=    9			/* Shift factor for blocks to bytes */
};

/* External references */

extern	int	MMG$GL_BWP_MASK;	/* Byte-within-page mask */
extern	int	MMG$GL_PAGE_SIZE;	/* Page size in bytes */
extern	int	MMG$GL_VPN_TO_VA;	/* Page to byte shift count */
extern	PTE	*MMG$GL_SPTBASE;	/* Base of system page table */
extern	int	MMG$GL_PTE_OFFSET_TO_VA;/* Shift for PTE to VA conversion */
extern	DDT	driver$ddt;		/* Prototype DDT */
extern	DPT	driver$dpt;		/* Prototype DPT */
extern	FDT	driver$fdt;		/* Prototype FDT */
#ifdef DEBUG
extern void ini$brk(void);		/* Breakpoint routine */
#endif

/* Shortcuts for some of the external references */

#define	_ddt	driver$ddt		/* Abbreviation for DDT */
#define	_dpt	driver$dpt		/* Abbreviation for DPT */
#define	_fdt	driver$fdt		/* Abbreviation for FDT */

/* Define the proper WFIKPCH routine */

#ifdef DEBUG
#define	WFIKPCH	wfikpch_hist
#else
#define	WFIKPCH	ioc$kp_wfikpch
#endif

/* Define the IDE disk controller CSRs.	*/

/* Here are the customary values for PC AT compatible machines.	*/
/* Note that the CSRs may be anywhere, and are defined here as	*/
/* offsets from a base.  That base is the address of the DATA	*/
/* register.							*/
/*				Primary		Secondary	*/
/* Data/Control Ports		1F0-1F7h	170-177h	*/
/* Control/Status Ports		3F7-3F6h	377-376h	*/

	/* Offsets for control block registers */

#define REG_ALT_STS	0x206		/* READ: Alternate status */
#define REG_DEV_CTL	0x206		/* WRITE:Device control */
#define REG_DRV_ADDR	0x207		/* READ: Drive address */

	/* Offsets for command block registers */

#define REG_DATA	0		/* R/W:  Data */
#define REG_ERROR	1		/* READ: Error */
#define REG_FEATURES	1		/* WRITE:Features */
#define REG_SEC_CNT	2		/* R/W:  Sector count */
#define REG_SECTOR	3		/* R/W:  Sector number */
#define REG_CYL_LO	4		/* R/W:  Cylinder (low) */
#define REG_CYL_HI	5		/* R/W:  Cylinder (high) */
#define REG_DRV_HD	6		/* R/W:  Drive / Head */
#define REG_STATUS	7		/* READ: Status */
#define REG_CMD		7		/* WRITE:Command */

	/* LBA fields (read and write) */

#define REG_LBA_0	3		/* LBA bits 0-7 */
#define REG_LBA_8	4		/* LBA bits 8-15 */
#define REG_LBA_16	5		/* LBA bits 16-23 */
#define REG_LBA_24	6		/* LBA bits 24-27 */

/* Device Control Register */

enum ctl_masks {
    CTL_M_nIEN		= 0x01,		/* Interrupt enable bit for the host */
    CTL_M_SRST		= 0x02,		/* Host software reset bit */
    CTL_M_MBO		= 0x04		/* Bit 3 must always be set to 1 */
};

/* Drive/head register */

enum head_masks {
    DRVHD_M_MBO		= 0xA0,		/* Bits 5 and 7 must be 1 */
    DRVHD_M_LBA		= 0x40		/* LBA addressing bit */
};

/* Status (and alternate status) register */

enum sts_masks {
    STS_M_ERR	= 0x01,			/* Error */
    STS_M_IDX	= 0x02,			/* Index mark */
    STS_M_CORR	= 0x04,			/* Corrected data */
    STS_M_DRQ	= 0x08,			/* Data request */
    STS_M_DSC	= 0x10,			/* Drive seek complete */
    STS_M_DWF	= 0x20,			/* Drive write fault */
    STS_M_DRDY	= 0x40,			/* Drive ready */
    STS_M_BSY	= 0x80			/* Busy */
};

/* Commands */

#define	CMD_READ	0x20		/* Read Sector (w/retry) */
#define	CMD_READ_VERIFY	0x40		/* Read Verify (w/retry) */
#define	CMD_INIT_DRV	0x91		/* Initialize drive parameters */
#define	CMD_IDENTIFY	0xec		/* Identify Drive */
#define	CMD_SEEK	0x70		/* SEEK command */
#define	CMD_WRITE	0x30		/* Write Sector (w/retry) */

/* Set up the table for CRAM initialization.  This table contains the	*/
/* CSR offset, the command used in this CRAM and the byte lane shift	*/
/* value.  The byte lane shift value is computed at run time.		*/

typedef struct {			/* The CRAM initialization structure */
    int	cmd;				/* Command index */
    int	offset;				/* Register offset */
    int	shift;				/* Byte lane shift count */
   } cram_item;

/* Define the indices in this (and the UCB) table */

#define	RD_ALT_STS	0
#define	RD_DRV_ADDR	1
#define	WT_DEV_CTL	2
#define	RD_DATA		3
#define	WT_DATA		4
#define	RD_ERROR	5
#define	RD_SEC_CNT	6
#define	WT_SEC_CNT	7
#define	RD_SECTOR	8
#define	WT_SECTOR	9
#define	RD_CYL_LO	10
#define	WT_CYL_LO	11
#define	RD_CYL_HI	12
#define	WT_CYL_HI	13
#define	RD_DRV_HD	14
#define	WT_DRV_HD	15
#define	RD_STATUS	16
#define	WT_CMD		17

#define	cram_def(cmd,csr) CRAMCMD$K_##cmd##32, REG_##csr, ((REG_##csr & 3) <<3)

cram_item	cram_init[NUMBER_CRAMS] = {
    cram_def(RDBYTE,ALT_STS),
    cram_def(RDBYTE,DRV_ADDR),
    cram_def(WTBYTE,DEV_CTL),
    cram_def(RDWORD,DATA),
    cram_def(WTWORD,DATA),
    cram_def(RDBYTE,ERROR),
    cram_def(RDBYTE,SEC_CNT),
    cram_def(WTBYTE,SEC_CNT),
    cram_def(RDBYTE,SECTOR),
    cram_def(WTBYTE,SECTOR),
    cram_def(RDBYTE,CYL_LO),
    cram_def(WTBYTE,CYL_LO),
    cram_def(RDBYTE,CYL_HI),
    cram_def(WTBYTE,CYL_HI),
    cram_def(RDBYTE,DRV_HD),
    cram_def(WTBYTE,DRV_HD),
    cram_def(RDBYTE,STATUS),
    cram_def(WTBYTE,CMD)
    };

/* Define Device-Dependent Unit Control Block with extensions for DQ device */


typedef struct {
    DT_UCB	ucb$r_dtucb;		/* Generic UCB */
    union {
        UINT	lbn;			/* Block number */
        struct {
            BYTE	sec;		/* Sector number */
            BYTE	trk;		/* Track number */
            WORD	cyl;		/* Cylinder number */
        } pa;
    } ucb$l_media;
    int		ucb$l_bcr;		/* Byte count remaining */
    UINT	ucb$l_org_media;	/* Original LBN */
    void	*ucb$l_org_svapte;	/* Original SVAPTE address */
    UINT	ucb$l_org_bcnt;		/* Original byte count */
    UINT	ucb$l_org_boff;		/* Original byte offset */
    UINT	ucb$l_drv_head;		/* Drive/head info */
    UINT	ucb$l_read_cmd;		/* Proper read command */
    UINT	ucb$l_write_cmd;	/* Proper write command */
    union {
        UINT	lw;			/* Flags */
        struct {
            UINT	lba : 1;	/* LBA addressing */
            UINT	dma : 1;	/* DMA capable */
        } bits;
    } ucb$l_flags;
    KPB		*ucb$ps_kpb;		/* KPB pointer */
    CRAM	*ucb$ps_crams[NUMBER_CRAMS];	/* Table of CRAMs */
    UINT	*ucb$ps_xfer;		/* Transfer buffer pointer */
    PTE		*ucb$ps_s0_svapte;	/* Pointer to base SPTE */
    BYTE	*ucb$ps_s0_va;		/* Pointer to user buffer */
#ifdef DEBUG
    int		ucb$l_total_ints;	/* Total interrupts count */
    int		ucb$l_unsol_ints;	/* Count of unsolicited interrupts */
    int		ucb$l_timeout[TIMEOUT_TIME+2];	/* Timeout histogram vector */
#endif
    } DQ_UCB;

#define ucb$r_dq_ucb ucb$r_dtucb.ucb$r_dpucb.ucb$r_erlucb.ucb$r_ucb
#define ucb$r_dq_erl ucb$r_dtucb.ucb$r_dpucb.ucb$r_erlucb
#define ucb$r_dq_dp  ucb$r_dtucb.ucb$r_dpucb
#define ucb$r_dq_dt  ucb$r_dtucb

#define	baseucb	ucb->ucb$r_dq_ucb

/* Define the Identify Drive information buffer */
/*	Use the nomember_alignment to make sure that this structure	*/
/*	matches what the drive uses					*/

#pragma	member_alignment	save
#pragma nomember_alignment

typedef struct {
    WORD	config;			/* Configuration information */
    WORD	cyls;			/* Number of cylinders */
    WORD	rsvd2;			/* Reserved word */
    WORD	heads;			/* Number of heads */
    WORD	ubytes_track;		/* Unformatted bytes/track */
    WORD	ubytes_sector;		/* Unformatted bytes/sector */
    WORD	sectors;		/* Number of sectors */
    WORD	unique7[3];		/* Vendor unique */
    char	serial_number[20];	/* ASCII serial number */
    WORD	buffer_type;		/* Buffer type */
    WORD	buffer_size_blocks;	/* Buffer size (in blocks) */
    WORD	ecc_bytes;		/* Number of ECC bytes/sector */
    char	firmware_revision[8];	/* ASCII firmware revision */
    char	model_number[MODEL_LENGTH]; /* ASCII drive model */
    BYTE	rw_multiple;		/* Number of sectors/interrupt */
    BYTE	unique47;    		/* Vendor unique */
    WORD	dblword_io;		/* Doubleword I/O flag */
    WORD	capabilities;		/* Capabilities */
    WORD	rsvd50;			/* Reserved */
    WORD	pio_cycle;		/* Programmed I/O cycle times */
    WORD	dma_cycle;		/* DMA I/O cycle times */
    WORD	valid54_58;		/* Valid bit for next 4 fields */
    WORD	curr_cyls;		/* 1) Current cylinder count */
    WORD	curr_heads;		/* 2) Current head count */
    WORD	curr_sectors;		/* 3) Current sector count */
    int		max_sectors;		/* 4) Maximum sector number */
    WORD	multiple_sectors;	/* Current sectors/interrupt setting */
    int		lba_maxblock;		/* LBA mode maximum block number */
    WORD	single_word_dma;	/* Single word DMA info */
    WORD	multi_word_dma;		/* Multi word DMA info */
    BYTE	rsvd64[64];		/* Reserved */
    BYTE	unique128[32];		/* Vendor unique */
    BYTE	rsvd160[96];		/* Reserved */
    } ID_PAGE;
#pragma	member_alignment	restore

/* Capabilities bits */

enum cap_bits {
    CAP_M_LBA	= 0x200,		/* Handles LBA mode */
    CAP_M_DMA	= 0x100			/* Handles DMA */
};


#define	IS_SET(reg,bits) ( (reg & bits) == bits )
#define	IS_CLEAR(reg,bits) ( (reg & bits) == 0 )

#define	$SUCCESS(code)	( (code & STS$M_SUCCESS) == 1)
#define	$FAIL(code)	( (code & STS$M_SUCCESS) == 0)

#define	TRUE	1
#define	FALSE	0

/* Prototypes for driver routines defined in this module */

int	driver$init_tables();
void	struc_init(CRB *crb, DDB *ddb, IDB *idb, ORB *orb, DQ_UCB *ucb);
void	struc_reinit(CRB *crb, DDB *ddb, IDB *idb, ORB *orb, DQ_UCB *ucb);
int	ctrl_init(IDB *idb, DDB *ddb, CRB *crb);
int	unit_init(IDB *idb, DQ_UCB *ucb);
void	regdump(BYTE *buffer, int arg_2, DQ_UCB *ucb);
void	unit_init_fork(void *fr3, IDB *idb, DQ_UCB *ucb);
void	startio(KPB *kpb);
void	isr(IDB *idb);
int	rct_fdt(IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb);
int	rdstats_fdt(IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb);
int	seek(DQ_UCB *ucb);
int	drvclr(DQ_UCB *ucb);
int	packack(DQ_UCB *ucb);
int	fetch_drive_info(DQ_UCB *ucb);
int	process_drive_info(DQ_UCB *ucb);
int	no_drive_info(DQ_UCB *ucb);
int	check_geom(DQ_UCB *ucb);
int	set_geom(DQ_UCB *ucb);
int	read_sector(int sector, int head, int cylinder, DQ_UCB *ucb);
int	read(DQ_UCB *ucb);
int	read_segment(DQ_UCB *ucb,int xfer_req, int *xfer_cnt,BYTE *buffer);
int	datacheck(DQ_UCB *ucb);
int	write(DQ_UCB *ucb);
int	write_segment(DQ_UCB *ucb,int xfer_req,int *xfer_cnt,int *byte_cnt);
int	readrct(DQ_UCB *ucb);
int	readstats(DQ_UCB *ucb);
BYTE	*map_user_buffer(DQ_UCB *ucb,int offset,int length);
void	move_sec_from_drive(DQ_UCB *ucb, BYTE **buffer);
void	compute_address(DQ_UCB *ucb,int *sec, int *head, int *cyl);
int	unload(DQ_UCB *ucb);
int	wait_ready(DQ_UCB *ucb);
int	wait_busy(DQ_UCB *ucb);
int	wait_drq(DQ_UCB *ucb);
int	reset_ctrl(DQ_UCB *ucb);
BYTE	inp(int reg,DQ_UCB *ucb);
WORD	inpw(int reg,DQ_UCB *ucb);
void	out(int reg, BYTE data,DQ_UCB *ucb);
void	outw(int reg, WORD data,DQ_UCB *ucb);

/* This should be in MMG_ROUTINES.H, but isn't yet */

void	mmg$tbi_single(BYTE *addr);

#ifdef DEBUG
int wfikpch_hist(KPB *kpb, int tmo, int newipl);
#endif

/* DRIVER$INIT_TABLES - Initialize Driver Tables
/*									*/
/* This routine is used to initialize the driver tables.  The DPT, DDT	*/
/* and FDT structures are set up.					*/
/*									*/
/* Usage:								*/
/*	status = driver$init_tables();					*/
/*									*/
/* Input:								*/
/*	none								*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Return value:							*/
/*	SS$_NORMAL	tables successfully set up			*/

int driver$init_tables()  {

/* Finish initialization of the Driver Prologue Table (DPT) */

    ini_dpt_name        (&_dpt,"DQDRIVER");	/* Driver name */
    ini_dpt_adapt       (&_dpt,AT$_ISA);	/* ISA bus device */
    ini_dpt_flags	(&_dpt,DPT$M_SMPMOD);	/* Set flags */
    ini_dpt_maxunits    (&_dpt,2);		/* 2 units max */
    ini_dpt_ucbsize     (&_dpt,sizeof(DQ_UCB));	/* UCB size */
    ini_dpt_struc_init  (&_dpt,struc_init);	/* Structure init rtn */
    ini_dpt_struc_reinit(&_dpt,struc_reinit);	/* Structure reinit rtn */
    ini_dpt_ucb_crams   (&_dpt,NUMBER_CRAMS);	/* Allocate some CRAMs */
    ini_dpt_end         (&_dpt);

/* Finish initialization of the Driver Dispatch Table (DDT) */

    ini_ddt_ctrlinit    (&_ddt,ctrl_init);	/* Controller init rtn */
    ini_ddt_unitinit    (&_ddt,unit_init);	/* Unit init rtn */
    ini_ddt_start       (&_ddt,exe_std$kp_startio); /* Exec's Start I/O rtn */
    ini_ddt_kp_startio	(&_ddt,startio);	/* KP's Start I/O rtn */
    ini_ddt_kp_stack_size(&_ddt,KPB$K_MIN_IO_STACK); /* KP stack size */
    ini_ddt_kp_reg_mask	(&_ddt,KPREG$K_HLL_REG_MASK);/* KP register mask */
    ini_ddt_cancel      (&_ddt,ioc_std$cancelio); /* Cancel rtn */
    ini_ddt_regdmp	(&_ddt,regdump);	/* Register dump routine */
    ini_ddt_erlgbf	(&_ddt,ERR_BYTES);	/* Set error log size */
    ini_ddt_end         (&_ddt);

/* Finish initialization of the Function Decision Table (FDT) */

    ini_fdt_act(&_fdt,IO$_READLBLK,  acp_std$readblk,	DIRECT);
    ini_fdt_act(&_fdt,IO$_READPBLK,  acp_std$readblk,	DIRECT);
    ini_fdt_act(&_fdt,IO$_READVBLK,  acp_std$readblk,	DIRECT);
    ini_fdt_act(&_fdt,IO$_WRITECHECK,acp_std$readblk,	DIRECT);

    ini_fdt_act(&_fdt,IO$_WRITELBLK, acp_std$writeblk,	DIRECT);
    ini_fdt_act(&_fdt,IO$_WRITEPBLK, acp_std$writeblk,	DIRECT)
    ini_fdt_act(&_fdt,IO$_WRITEVBLK, acp_std$writeblk,	DIRECT);

    ini_fdt_act(&_fdt,IO$_ACCESS,    acp_std$access,	BUFFERED);
    ini_fdt_act(&_fdt,IO$_CREATE,    acp_std$access,	BUFFERED);

    ini_fdt_act(&_fdt,IO$_DEACCESS,  acp_std$deaccess,	BUFFERED);

    ini_fdt_act(&_fdt,IO$_ACPCONTROL,acp_std$modify,	BUFFERED);
    ini_fdt_act(&_fdt,IO$_DELETE,    acp_std$modify,	BUFFERED);
    ini_fdt_act(&_fdt,IO$_MODIFY,    acp_std$modify,	BUFFERED);

    ini_fdt_act(&_fdt,IO$_MOUNT,     acp_std$mount,	BUFFERED);

    ini_fdt_act(&_fdt,IO$_READRCT,   rct_fdt,		DIRECT);
    ini_fdt_act(&_fdt,IO$_RDSTATS,   rdstats_fdt,	DIRECT);

    ini_fdt_act(&_fdt,IO$_UNLOAD,    exe_std$lcldskvalid,BUFFERED);
    ini_fdt_act(&_fdt,IO$_AVAILABLE, exe_std$lcldskvalid,BUFFERED);
    ini_fdt_act(&_fdt,IO$_PACKACK,   exe_std$lcldskvalid,BUFFERED);

    ini_fdt_act(&_fdt,IO$_NOP,	     exe_std$zeroparm,	BUFFERED);
    ini_fdt_act(&_fdt,IO$_DRVCLR,    exe_std$zeroparm,	BUFFERED);
    ini_fdt_act(&_fdt,IO$_RELEASE,   exe_std$zeroparm,	BUFFERED);

    ini_fdt_act(&_fdt,IO$_SEEK,      exe_std$oneparm,	BUFFERED);
    ini_fdt_act(&_fdt,IO$_FORMAT,    exe_std$oneparm,	BUFFERED);

    ini_fdt_act(&_fdt,IO$_SETMODE,   exe_std$setchar,	BUFFERED);
    ini_fdt_act(&_fdt,IO$_SETCHAR,   exe_std$setchar,	BUFFERED);

    ini_fdt_act(&_fdt,IO$_SENSEMODE, exe_std$sensemode,	BUFFERED);
    ini_fdt_act(&_fdt,IO$_SENSECHAR, exe_std$sensemode,	BUFFERED);
    ini_fdt_end(&_fdt);

/* If we got this far then everything worked, so return success. */

    return SS$_NORMAL;			/* Return with success status */
}

/* STRUC_INIT - Device Data Structure Initialization Routine
/*									*/
/* This routine is used to initialize the data structures at driver	*/
/* loading time.							*/
/*									*/
/* Usage:								*/
/*	struc_init(crb, ddb, idb, orb, ucb)				*/
/*									*/
/* Input:								*/
/*	crb	pointer to CRB						*/
/*	ddb	pointer to DDB						*/
/*	idb	pointer to IDB						*/
/*	orb	pointer to ORB						*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Return value:							*/
/*	none								*/

void struc_init(CRB *crb, DDB *ddb, IDB *idb, ORB *orb, DQ_UCB *ucb) {

/* Initialize the fork lock and device IPL fields */

    baseucb.ucb$b_flck = SPL$C_IOLOCK8;	/* set up fork lock index */
    baseucb.ucb$b_dipl = DEVICE_IPL;	/*  and device IPL */

/* Initialize some UCB fields */

    baseucb.ucb$l_devchar = (DEV$M_DIR + DEV$M_FOD + DEV$M_AVL + DEV$M_ELG +
                             DEV$M_IDV + DEV$M_ODV + DEV$M_SHR + DEV$M_RND);
    baseucb.ucb$l_devchar2 = DEV$M_NNM;		/* Use "node$" device names */
    baseucb.ucb$b_devclass = DC$_DISK; 		/* Device class is a disk */
    baseucb.ucb$b_devtype  = DT$_GENERIC_DK;	/* Device type for DDR */
    baseucb.ucb$w_devbufsiz= BLK_SIZE;		/* Size of a block */
    baseucb.ucb$l_devsts   = UCB$M_NOCNVRT;	/* Do NOT convert LBNs */
    return;
}

/* STRUC_REINIT - Device Data Structure Re-Initialization Routine
/*									*/
/* This routine is used to reinitialize the data structures at driver	*/
/* reloading time.							*/
/*									*/
/* Usage:								*/
/*	struc_init(crb, ddb, idb, orb, ucb)				*/
/*									*/
/* Input:								*/
/*	crb	pointer to CRB						*/
/*	ddb	pointer to DDB						*/
/*	idb	pointer to IDB						*/
/*	orb	pointer to ORB						*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Return value:							*/
/*	none								*/

void struc_reinit (CRB *crb, DDB *ddb, IDB *idb, ORB *orb, DQ_UCB *ucb) {
    
    ddb->ddb$ps_ddt = &_ddt;		/* Point ddb to the ddt */
    dpt_store_isr(crb, isr);		/* Set up ISR address */

    return;				/* Return to caller */
}

/*	rct_fdt	- IO$_READRCT FDT Processing				*/
/*									*/
/*	This routine is the FDT processing routine for the RCT function	*/
/* code.  The LBN and size are checked and, if ok, the buffer is locked	*/
/* down and the I/O handed off to be processed.				*/
/*									*/
/* Input:								*/
/*	irp	pointer to IRP						*/
/*	pcb	pointer to PCB						*/
/*	ucb	pointer to UCB						*/
/*	ccb	pointer to CCB						*/
/*									*/
/* Output:								*/
/*									*/
/* Return value:							*/
/*	SS$_FDT_COMPL	shows that the routine completed correctly	*/

int rct_fdt(IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb) {

int	status;				/* Returned routine status */

    irp->irp$l_bcnt = irp->irp$l_qio_p2;	/* Copy the byte count */
    irp->irp$l_media= irp->irp$l_qio_p3;	/* and the LBN */

/* Check that LBN = 0 and byte count is less than 1 block */

    if ( (irp->irp$l_bcnt <= BLK_SIZE) && (irp->irp$l_media == 0)) {
        status = exe_std$readlock(irp,pcb,(UCB *)ucb,ccb,
	             (void *)irp->irp$l_qio_p1,irp->irp$l_bcnt,0);
        exe_std$qiodrvpkt(irp,(UCB *)ucb);/* Queue the packet */
        return SS$_FDT_COMPL;		/*  and exit */
    } else {
        irp->irp$l_iost1 = SS$_BADPARAM; /* Load error code */
        irp->irp$l_iost2 = 0;		 /* Clear high IOSB */
        exe_std$finishio(irp,(UCB *) ucb);/* Finish with error */
    }

    return SS$_FDT_COMPL;		/* exit */
}

/*	rdstats_fdt	- IO$_RDSTATS FDT Processing			*/
/*									*/
/*	This routine is the FDT processing routine for the RDSTATS	*/
/* function code.  If the DEBUG conditional is on, several statistics	*/
/* are returned to the caller.  Otherwise, the SS$_NODATA error is	*/
/* returned.								*/
/*									*/
/* Input:								*/
/*	irp	pointer to IRP						*/
/*	pcb	pointer to PCB						*/
/*	ucb	pointer to UCB						*/
/*	ccb	pointer to CCB						*/
/*									*/
/* Output:								*/
/*									*/
/* Return value:							*/
/*	SS$_FDT_COMPL	shows that the routine completed correctly	*/

int rdstats_fdt(IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb) {

int	status;				/* Returned routine status */
int	*bp;				/* Longword buffer pointer */
int	i;				/* Loop counter */

/* Check that LBN = 0 and byte count is large enough  */

#ifdef DEBUG
    irp->irp$l_iost1 = SS$_BADPARAM;	/* Assume an error - Load error code */

    if ( (irp->irp$l_qio_p2 >= RDSTATS_LEN) && (irp->irp$l_qio_p3 == 0)) {
        bp = (void *) irp->irp$l_qio_p1;	/* Point to the buffer */
        *bp = ucb->ucb$l_total_ints;	/* Get count of all interrupts */
        bp++;				/* Move to next longword */
        *bp = ucb->ucb$l_unsol_ints;	/* Get count of unsolicited interrupts */
        bp++;				/* Move to next longword */
        *bp = NUMBER_CRAMS;		/* Copy over the number of CRAMS */
        bp++;
        *bp = (int) ucb->ucb$ps_xfer;	/* Transfer buffer address */
        bp++;
        *bp = (int) ucb->ucb$ps_s0_svapte; /* Base SPTE address */
        bp++;
        *bp = (int) ucb->ucb$ps_s0_va;	/* S0 VA (user buffer) */
        bp++;
        *bp = TIMEOUT_TIME+2;		/* Save size of TIMEOUT vector */
        bp++;				/* Move to next location */

	/* Copy over the timeout histogram vector */

           for (i=0; i <= (TIMEOUT_TIME + 2); i++) {
           *bp = ucb->ucb$l_timeout[i];	/* Copy over the timeout count */
           bp++;			/* Advance pointer */
           }

        irp->irp$l_iost1 = (RDSTATS_LEN << 16) + SS$_NORMAL;
    }
#else
    irp->irp$l_iost1 = SS$_NODATA;	/* Load error code */
#endif

    irp->irp$l_iost2 = 0;		/* Clear high IOSB */
    exe_std$finishio(irp,(UCB *) ucb);	/* Finish the I/O */
    return SS$_FDT_COMPL;		/*  and exit */
}

/* CTRL_INIT - Controller Initialization Routine			*/
/*									*/
/* This routine is used to perform controller specific initialization	*/
/* and is called by 1) system startup, 2) during driver loading and	*/
/* 3) during power failure recovery.					*/
/*									*/
/* Usage:								*/
/*									*/
/*	status = ctrl_init (idb, ddb, crb)				*/
/*									*/
/* Input:								*/
/*	idb	pointer to the idb					*/
/*	ddb	pointer to the ddb					*/
/*	crb	Pointer to the crb					*/
/*									*/
/* Output:								*/
/*   None.								*/
/*									*/
/* Return value:							*/
/*   status     SS$_NORMAL - unit was initialized successfully.		*/
 
int ctrl_init (IDB *idb, DDB *ddb, CRB *crb)  {

#ifdef DEBUG
        ini$brk ();			/* Request breakpoint */
#endif

    return SS$_NORMAL;			/* Return SUCCESS */
}

/* UNIT_INIT - Unit Initialization Routine				*/
/*									*/
/* This routine is used to perform unit specific initialization		*/
/* and is called by 1) system startup, 2) during driver loading and	*/
/* 3) during power failure recovery.					*/
/*									*/
/* This routine does very little work.  Its primary job is to start up	*/
/* the fork process that will do the bulk of the unit initialization.	*/
/*									*/
/* Usage:								*/
/*									*/
/*	status = unit_init (idb, ucb)					*/
/*									*/
/* Input:								*/
/*	idb	pointer to the IDB					*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*   None.								*/
/*									*/
/* Return value:							*/
/*   status     SS$_NORMAL - unit was initialized successfully.		*/
 
int unit_init (IDB *idb, DQ_UCB *ucb)  {

    if (baseucb.ucb$v_power)		/* Is this power recovery ? */
        return SS$_NORMAL;		/* Power recovery - just exit */

    baseucb.ucb$v_online = 0;		/* Start with unit offline */
    ucb->ucb$l_flags.lw = 0;		/* Clear all the flags */
    ucb->ucb$l_read_cmd  = CMD_READ;	/* Default read command is 1 sector */
    ucb->ucb$l_write_cmd = CMD_WRITE;	/* Default write command is 1 sector */

/* Set up drive/head bits for later use in commands */

    ucb->ucb$l_drv_head = DRVHD_M_MBO + (baseucb.ucb$w_unit << 4);

/* Set up and queue fork process to complete the unit initialization */

    baseucb.ucb$l_fpc = &unit_init_fork;/* Point to fork routine address */
    exe_std$primitive_fork(0,(int64)idb,(FKB *) ucb);/* Start fork process */
    return SS$_NORMAL;			/* Return with success */
}

/* UNIT_INIT_FORK - Unit Initialization Fork Routine			*/
/*									*/
/* This is the fork routine that does the bulk of the unit		*/
/* initialization work.							*/
/*									*/
/* Usage:								*/
/*									*/
/*	unit_init_fork ( fr3, idb, ucb)					*/
/*									*/
/* Input:								*/
/*	fr3	Fork routine parameter (unused)				*/
/*	idb	pointer to the IDB					*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*   None.								*/
/*									*/
/* Return value:							*/
/*	none								*/

void unit_init_fork(void *fr3, IDB *idb, DQ_UCB *ucb) {

CRAM	*cram_ptr;			/* Pointer to a CRAM */
int	index;				/* Index for walking the CRAM list */
ADP	*adp;				/* Address of ADP */
int	isa_base;        	    	/* Slot I/O address if ISA option */
int	device_data;  	          	/* Data from or for CRAM */
int	status;				/* Return status value */
int	page_cnt;			/* Number of pages to allocate */
int	offset;				/* PTE offset in page table */
int	csr_base;			/* Base CSR address */
IDB	*idb_ptr;			/* CRAM IDB pointer value to use */

/* Set up all of the CRAMs that were allocated */

    adp	= baseucb.ucb$ps_adp;		/* Get ADP address */
    cram_ptr = baseucb.ucb$ps_cram;	/* Point to the first CRAM */

	/* Ok, here's a hack.  I'm going to pick up the IDB$Q_CSR value.*/
	/* If it's < 0x8000, then it's treated as an offset from the	*/
	/* the base of ISA space.  For example, 0x1F0.  If not, it's	*/
	/* treated as the actual VA of the base address.  This leads to	*/
	/* two cases:							*/
	/*	1 - ISA offset.  Clear the CRAM IDB pointer so that	*/
	/*	    only the ADP$Q_CSR field is used.  Use the IDB CSR	*/
	/*	    value as the offset to the register (csr_base).	*/
	/*	2 - VA of CSR.  Don't clear the CRAM IDB pointer, so	*/
	/*	    the CRAM_CMD code will use IDB$Q_CSR as the base	*/
	/*	    address.  Also, set csr_base to 0, as the VA is	*/
	/*	    assumed to point directly to the base address.	*/
	/*								*/
	/* All of this allows the SYSMAN IO CONNECT command to use	*/
	/* either value in the /CSR switch.				*/

    if (idb->idb$q_csr > 0x8000) {	/* Check if it's in ISA space */
        idb_ptr= idb;			/* Use pointer to IDB in CRAM */
        csr_base = 0;			/* No base, assume correct VA */
    } else {
        idb_ptr= NULL;			/* Use no IDB pointer in CRAM */
        csr_base = idb->idb$q_csr;	/* Use the CSR as the base */
    }

    for (index=0; cram_ptr != 0; cram_ptr = cram_ptr->cram$l_flink,index++) {
        cram_ptr->cram$l_idb = idb_ptr;	/* Set IDB pointer in the CRAM */
        ucb->ucb$ps_crams[index] = cram_ptr;	/* Set up UCB table */
        status = ioc$cram_cmd(cram_init[index].cmd,
                              csr_base+cram_init[index].offset,
                              adp, cram_ptr, 0);
        cram_ptr->cram$l_idb = idb;	/* Set the IDB pointer correctly */
        if ($FAIL(status))		/* Check the return status */
            return;			/* Return if error */
        cram_ptr->cram$v_der = 1;	/* Disable error reporting */
    }

/* Allocate transfer buffer */

    page_cnt = ((MAX_XFER * BLK_SIZE) + (MMG$GL_PAGE_SIZE-1)) >>
                 MMG$GL_VPN_TO_VA;	/* Compute pages for MAX_XFER buffer */
    status = exe_std$alophycntg(page_cnt,(void *)&ucb->ucb$ps_xfer);
    if ($FAIL(status))
        return;				/* Just exit on failure */

/* Allocate SPTEs for double mapping the user buffer (plus guard + spillage) */

    status = ldr_std$alloc_pt(page_cnt+3,(void *)&ucb->ucb$ps_s0_svapte);
    if ($FAIL(status))
        return;				/* Just exit on failure */

/* Compute S0 address of the double map buffer.  Note that "offset" will */
/* be the number of PTEs, not the offset from SPTBASE.  So, the shift is */
/* page number to VA, not PTE offset to VA.  A small factor of PTE size. */

    offset = ucb->ucb$ps_s0_svapte - MMG$GL_SPTBASE;
    ucb->ucb$ps_s0_va = (BYTE *)((offset << MMG$GL_VPN_TO_VA) | VA$M_SYSTEM);
    
/* Enable interrupts */

    status = ioc$node_function(baseucb.ucb$l_crb, IOC$K_ENABLE_INTR);
    if ($FAIL(status))			/* Check status and */
        return;				/*  simply exit if error */

#ifdef DEBUG

/* Clear the histogram buffer counts.  Clear each entry from 0 to */
/* TIMEOUT_TIME and the overflow count at the end of the vector. */

    for (index = 0 ; index < TIMEOUT_TIME+2; index++) {
        ucb->ucb$l_timeout[index] = 0;	/* Clear the count */
    }

#endif

/* Mark the device as "on line" and ready to accept I/O requests */

    baseucb.ucb$v_online = 1;		/* Set the unit as ONLINE */
    return;				/*  and return to the caller */
}

/* REGDUMP - Register Dump Routine					*/
/*									*/
/* This is the register dump routine.  It is used to dump the registers	*/
/* at the time of an error.  It is called at device IPL.		*/
/*									*/
/* Input:								*/
/*	buffer	address of buffer to store registers			*/
/*	arg_2	additional argument passed by caller			*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	none								*/

void regdump(BYTE *buffer, int arg_2, DQ_UCB *ucb) {

/* For some reason, the error packet isn't displaying well.  So, hack	*/
/* it.  Fudge the pointer based on empirical results and add "ssss" and	*/
/* "eeee" to bracket the packet.					*/

    buffer += 5;			/* Advance pointer */

    *buffer++ = 's';			/* Bracket the buffer */
    *buffer++ = 's';
    *buffer++ = 's';
    *buffer++ = 's';

/* Put all of the registers into the buffer.  Pad to an even longword */

    *buffer++ = arg_2;			/* Copy over the marker */
    *buffer++ = inp(RD_ALT_STS,ucb);	/* Get alternate status register */
    *buffer++ = inp(RD_DRV_ADDR,ucb);	/* Get drive address */
    *buffer++ = inp(RD_ERROR,ucb);	/* Get error */
    *buffer++ = inp(RD_SEC_CNT,ucb);	/* Get sector count */
    *buffer++ = inp(RD_SECTOR,ucb);	/* Get sector number */
    *buffer++ = inp(RD_CYL_LO,ucb);	/* Get cylinder number (low) */
    *buffer++ = inp(RD_CYL_HI,ucb);	/* Get cylinder number (high) */
    *buffer++ = inp(RD_DRV_HD,ucb);	/* Get drive/head information */
    *buffer++ = inp(RD_STATUS,ucb);	/* Get status  */
    *buffer++ = 0;			/* Round up to an even */
    *buffer++ = 0;			/*  number of longwords */

/* Add trailer to help ID this in the unformatted packets */

    *buffer++ = 'e';			/* Add tail of buffer bracket */
    *buffer++ = 'e';
    *buffer++ = 'e';
    *buffer++ = 'e';
}

/* STARTIO - Start I/O Routine						*/
/*									*/
/* This is the driver start I/O routine.  This routine processes each	*/
/* of the I/O requests.							*/
/*									*/
/* Input:								*/
/*	irp        Pointer to I/O request packet			*/
/*	ucb        Pointer to unit control block			*/
/*									*/
/* Output:								*/
/*	none								*/

void startio(KPB *kpb) {

int	iost1, iost2;			/* IOSB fields */
int	temp;				/* Temporary value */
DQ_UCB	*ucb;				/* Pointer to UCB */
IRP	*irp;				/* Pointer to IRP */

/* Set up necessary pointers */

    ucb = (DQ_UCB *)kpb->kpb$ps_ucb;	/* Get UCB pointer */
    irp = kpb->kpb$ps_irp;		/* Get IRP pointer */
    ucb->ucb$ps_kpb = kpb;		/* Save the KPB address */

/* Check that either volume is valid or this is a physical I/O */

    if ( !irp->irp$v_physio && !baseucb.ucb$v_valid) {
        ioc_std$reqcom(SS$_VOLINV,0,(UCB *)ucb);/* Finish I/O */
        return;				/* And exit */
    }

/* Get LBN from media field.  Interpret according to PHYSIO bit */

    ucb->ucb$l_media.lbn = irp->irp$l_media;/* Copy the disk address */
    ucb->ucb$l_bcr = baseucb.ucb$l_bcnt;/* Copy remaining byte count */

    if (irp->irp$v_physio) {		/* Convert from physical format */
        ucb->ucb$l_media.lbn = ((((ucb->ucb$l_media.pa.cyl*baseucb.ucb$b_tracks)+
                                   ucb->ucb$l_media.pa.trk)*baseucb.ucb$b_sectors)+
                                   ucb->ucb$l_media.pa.sec-1);
    }

/* Remember the transfer parameters */

    ucb->ucb$l_org_media = ucb->ucb$l_media.lbn;	/* LBN */
    ucb->ucb$l_org_svapte= baseucb.ucb$l_svapte;	/* Page table address */
    ucb->ucb$l_org_bcnt  = baseucb.ucb$l_bcnt;		/* Byte count */
    ucb->ucb$l_org_boff  = baseucb.ucb$l_boff;		/* Byte offset */

/* Handle based on function code */

    iost1 = ioc$kp_reqchan(kpb,KPB$K_LOW); /* Get the data channel */
    if ($FAIL(iost1)) {			/* Check for failure to get channel */
        ioc_std$reqcom(iost1,0,(UCB *)ucb);/* Finish I/O */
        return;				/* And exit */
    }
        
    iost1 = SS$_ILLIOFUNC;		/* Assume illegal I/O function */
    iost2 = 0;				/* Assume no data transferred */

    switch (irp->irp$v_fcode) {
        case IO$_NOP:
            iost1 = SS$_NORMAL;		/* Status is "normal" */
            break;			/*  and complete the I/O */

        case IO$_AVAILABLE:
        case IO$_UNLOAD:
	    iost1 = unload(ucb);	/* Call the unload function */
            break;			/*  and complete the I/O */

        case IO$_SEEK:
            iost1 = seek(ucb);		/* Call the SEEK function */
            break;			/*  and complete the I/O */

        case IO$_DRVCLR:
            iost1 = drvclr(ucb);	/* Call the DRIVE CLEAR function */
            break;			/*  and complete the I/O */

        case IO$_PACKACK:
            iost1 = packack(ucb);	/* Call PACKACK */
            break;			/*  and complete the I/O */

        case IO$_WRITECHECK:
        case IO$_READLBLK:
        case IO$_READPBLK:
            iost1 = read(ucb);		/* Read the required blocks */
            if (IS_SET(irp->irp$l_func,IO$M_DATACHECK))
                iost1 = datacheck(ucb);	/*  Yes, do the datacheck */
            temp  = baseucb.ucb$l_bcnt - ucb->ucb$l_bcr;
            iost1 = (iost1 & 0xFFFF) + (temp << 16);
            break;			/*  and complete the I/O */

        case IO$_WRITELBLK:
        case IO$_WRITEPBLK:
            iost1 = write(ucb);		/* Write the required blocks */
            if (IS_SET(irp->irp$l_func,IO$M_DATACHECK))
                iost1 = datacheck(ucb);	/*  Yes, do the datacheck */
            temp  = baseucb.ucb$l_bcnt - ucb->ucb$l_bcr;
            iost1 = (iost1 & 0xFFFF) + (temp << 16);
            break;			/*  and complete the I/O */

        case IO$_READRCT:
            iost1 = readrct(ucb);	/* Get back the drive data */
            iost1 = (iost1 & 0xFFFF) + (baseucb.ucb$l_bcnt << 16);
            break;                      /*  and complete the I/O */

        case IO$_FORMAT:
            iost1 = SS$_UNSUPPORTED;	/* Return UNSUPPORTED error for now */
            break;			/*  and complete the I/O */

        default:
            break;
    }

/* I/O is done, release channel and return information about the I/O */

    ioc_std$relchan((UCB *)ucb);	/* Release the data channel */
    ioc_std$reqcom(iost1,iost2,(UCB *)ucb);	/* Finish I/O */

    return;
}

/* PACKACK - Perform PACKACK operation					*/
/*									*/
/* This routine is used to determine information about the drive so	*/
/* that is can be mounted and put to use.				*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Return value:							*/
/*	status value							*/
/*		SS$_NORMAL - success					*/
/*		SS$_NODATA - failed to get drive information		*/

int packack(DQ_UCB *ucb) {

int	status;				/* Routine return status */
int	orig_ipl;			/* Original IPL */
BYTE	err;				/* Error register */

/* Get the drive info.  If it fails, do it manually.  Else, process */
/* the data */

    status = fetch_drive_info(ucb);	/* Get the drive infrmation */
    if ($FAIL(status)) {		/* If it fails ... */
        status = no_drive_info(ucb);	/* Try it the hard way */
        if ($FAIL(status)) {		/* If that fails, too... */
            return status;		/* exit with error */
        }
    } else {
        status = process_drive_info(ucb);/* Process the drive info */
        if ($FAIL(status))		/* If that failed... */
            return status;		/*   exit with error */

	/* Validate the geometry and if invalid, determine it empirically */

        status = check_geom(ucb);	/* Check the geometry */
        if ($FAIL(status))		/* Check for success */
            status = no_drive_info(ucb);/*  Failed - determine it manually */
    }

/* Done.  Set the drive as valid and return to caller with success */

    if ($SUCCESS(status)) 		/* If all is well, then */
        baseucb.ucb$v_valid = 1;	/*   set the VALID bit */

    return SS$_NORMAL;			/* Return to caller with success */
}

/* fetch_drive_info - This routine is used to read the drive information*/
/* page.								*/
/*									*/
/* Input:								*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Status:								*/
/*	SS$_NORMAL	success						*/
/*	SS$_NODATA	error reading the page				*/
/*	SS$_TIMEOUT	device timeout					*/

int fetch_drive_info(DQ_UCB *ucb) {

int	status;				/* Routine return status */
int	orig_ipl;			/* Original IPL */

/* Wait for drive to be ready for a command */

    status = wait_ready(ucb);		/* Wait for drive to be ready */
    if ($FAIL(status))			/* Check the status for failure */
        return status;			/* Return with error */

/* Select the drive, then ask for drive information */

	/* Obtain devicelock and write registers and send the command */

    device_lock(baseucb.ucb$l_dlck,RAISE_IPL, &orig_ipl);
    out(WT_DRV_HD,ucb->ucb$l_drv_head,ucb); /* Select drive and head 0 */
    out(WT_CMD,CMD_IDENTIFY,ucb);	/* Ask for drive info */

/* Wait for the interrupt and all that goes with that */

    status = WFIKPCH(ucb->ucb$ps_kpb,TIMEOUT_TIME,orig_ipl);

/* If TIMEOUT, then FORK and return with SS$_TIMEOUT status */
/*  else, if other error, then use DEVICE UNLOCK and return with error */

    if (status == SS$_TIMEOUT) {
        erl_std$devictmo(0,(UCB *)ucb);	/* Handle the timeout */
        status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);
        return SS$_TIMEOUT;		/* Return with timeout */
    } else if ($FAIL(status)) {
        device_unlock(baseucb.ucb$l_dlck,orig_ipl,SMP_RESTORE);
        return status;			/* Return with status */
    }

/* Drop back to fork IPL */

    status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);

/* Check the error status.  */

    if ( IS_SET(inp(RD_ALT_STS,ucb),STS_M_ERR) )
        return SS$_NODATA;		/* Return error */

/* Success */

    return SS$_NORMAL;			/* Return success */
}

/* process_drive_info - This routine is used to read and process the	*/
/* information returned by the drive in the ID page.			*/
/*									*/
/* Input:								*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Status:								*/
/*	SS$_NORMAL	success						*/

int process_drive_info(DQ_UCB *ucb) {

#include	<dtndef>		/* Define the DTN */

char	model[DTN$K_NAMELEN_MAX+1];	/* ASCIZ model name */
int	mod_len;			/* Length of model string */
ID_PAGE	*id_ptr;			/* Pointer to the ID page */
DTN	*dtn;				/* Dummy DTN pointer */
int	status;				/* Returned routine status */
int	i;				/* String index */
WORD	datal, datah;			/* Data words */
BYTE	*xfer;				/* Pointer to buffer */

/* Read the data from the sector buffer into the transfer buffer */

    xfer = (BYTE *) ucb->ucb$ps_xfer;	/* Point to the transfer buffer */
    move_sec_from_drive(ucb,&xfer);	/* Get the sector */

    id_ptr=(ID_PAGE *)ucb->ucb$ps_xfer;	/* Get ID Page pointer */

/* Do some simple checks on the geometry to make sure its' valid */

    if ( (id_ptr->sectors > MAX_SECTOR) || /* Check too many sectors */
         (id_ptr->heads > MAX_HEAD) ||	/*  and too many heads */
         (id_ptr->sectors == 0) ||	/* or too few sectors */
         (id_ptr->heads == 0) ||	/* or too few heads */
         (id_ptr->cyls == 0) )		/* or too few cylinders */
        return SS$_IVADDR;		/* Sanity failed - exit */

/* Copy over the geometry information */

    baseucb.ucb$b_sectors  = id_ptr->sectors;	/* Set the sectors */
    baseucb.ucb$b_tracks   = id_ptr->heads;	/*  and tracks */
    baseucb.ucb$w_cylinders= id_ptr->cyls;	/*   and cylinders */
    ucb->ucb$r_dq_dt.ucb$l_maxblock = id_ptr->sectors *
                             id_ptr->heads *
                             id_ptr->cyls;	/*    and MAXLBN */

    set_geom(ucb);			/* Set the geometry in the drive */

/* Set up for proper read/write command and I/O sizes */

    ucb->ucb$l_read_cmd  = CMD_READ;	/*  Single block read */
    ucb->ucb$l_write_cmd = CMD_WRITE;	/*   and write commands */
      
/* Set flags based on capabilities flag */

    ucb->ucb$l_flags.bits.lba = 0;	/* Assume no LBA */
    ucb->ucb$l_flags.bits.dma = 0;	/* Assume no DMA */

    if (IS_SET(id_ptr->capabilities,CAP_M_LBA)) { /* If LBA capable */
        ucb->ucb$l_flags.bits.lba = 1;		/* Set the LBA flag */
    }

    if (IS_SET(id_ptr->capabilities,CAP_M_DMA)) { /* If DMA capable */
        ucb->ucb$l_flags.bits.dma = 1;		/* Set the DMA flag */
    }

/* Add the device type name */

/* Ok, this is brain damaged, but we have to do it.  Each of the bytes in */
/* the ASCII string is byte swapped.  So, swap them back */

    mod_len =(MODEL_LENGTH>DTN$K_NAMELEN_MAX) ? 
              DTN$K_NAMELEN_MAX : MODEL_LENGTH; /* Set the length of string */

    for (i=0; i < (mod_len>>1)<<1; i += 2) { /* Copy the swapped bytes */
        model[i] = id_ptr->model_number[i+1];
        model[i+1] = id_ptr->model_number[i];
    }

    if ( (mod_len & 1) == 1)		/* Get the odd last byte if needed */
        model[mod_len-1] = id_ptr->model_number[mod_len];

    model[mod_len] = '\0';		/* Make the string ASCIZ */

/* Now, remove trailing spaces */

    for (i=1; i < mod_len; i++) {
        if (model[mod_len - i] != ' ')	/* Is this a space ? */
            break;			/* Non-space - leave loop */
        model[mod_len - i] = '\0';	/* Terminate string at space */
    }

    mod_len = strlen(model);		/* Get the new length */

/* Now, add the device type and name for Dynamic Device Recognition */

    status = ioc$add_device_type(model,mod_len,(UCB *)ucb,&dtn);
    baseucb.ucb$b_devtype = DT$_GENERIC_DK;	/* Device type for DDR */

    return SS$_NORMAL;			/* Return success to caller */
}

/* no_drive_info - This routine is used to determine information about	*/
/* the drive when the IDENTIFY_DRIVE command isn't supported.  The	*/
/* geometry is calculated from a number of reads.			*/
/*									*/
/* Input:								*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Status:								*/
/*	SS$_NORMAL	success						*/

int no_drive_info(DQ_UCB *ucb) {

int	status;				/* Return status from routine */
int	sector;				/* Trial sector number */
int	head;				/* Trial head number */
int	cyl;				/* Trial cylinder number */
int	drv_head;			/* Drive/head value */
int	low,high;			/* Cylinders for binary search */
BYTE	err;				/* Error register contents */

    if (!(status = wait_ready(ucb)))	/*  Wait for drive ready */
        return status;			/* Return with error */

/* Clear the geometry information */

    baseucb.ucb$b_sectors  = 0;		/* Clear the sectors */
    baseucb.ucb$b_tracks   = 0;		/*  and tracks */
    baseucb.ucb$w_cylinders= 0;		/*   and cylinders */
    ucb->ucb$r_dq_dt.ucb$l_maxblock = 0;/*    and MAXLBN */

/* Determine the number of sectors per track */
/*	Read each sector on head 0, cylinder 0 until an error occurs */

    for (sector = 1; sector <= MAX_SECTOR; sector++) {
        status = read_sector(sector,0,0,ucb); /* Try a read of this sector */
        if ($FAIL(status))		/* Check status */
            break;			/* At failure, exit */
        else
            baseucb.ucb$b_sectors++;	/* Else, bump sector count */
    }

/* Determine the number of tracks per cylinder */
/*	Read each track on sector 1, cylinder 0 until an error occurs */

    for (head = 0; head < MAX_HEAD; head++) {
        status = read_sector(1,head,0,ucb); /* Try a read */
        if ($FAIL(status))		/* Check the status */
            break;			/* If failure, exit loop */
        else
            baseucb.ucb$b_tracks++;	/* Else, bump head count */
    }

/* Determine the number of cylinders */
/*	Perform a binary search by reading cylinders with head 0 and	*/
/*	sector 1 until the highest cylinder number is determined.	*/

    sector = baseucb.ucb$b_sectors;	/* Get highest sector number */
    head   = baseucb.ucb$b_tracks-1;	/* Get highest head number */

    low = 1;				/* Set low boundary */
    high=MAX_CYLINDER;			/* Set the high boundary */
    while (high >= 1) {			/* Loop until found */
        cyl = (low + high) >> 1;	/* Pick the middle to read */
        status = read_sector(sector,head,cyl,ucb);/* Issue the test read */
        if ($FAIL(status)) {		/* Check for success */
            if (high <= low) {		/* Are we down to LOW ? */
                cyl--;			/* Cylinder is 1 too high */
                break;			/*  Yes - exit */
	    }
            high= cyl - 1;		/*  Failed - lower the high */
        } else {
            if (low >= high)		/* Are we at the end ? */
                break;			/*  Yes, just bail out */
            low = cyl + 1;		/* No, raise the bottom */
            }
        }

/* Update the geometry information */

    baseucb.ucb$w_cylinders = cyl + 1;	/* Set number of cylinders */
    ucb->ucb$r_dq_dt.ucb$l_maxblock = baseucb.ucb$b_sectors *
                                      baseucb.ucb$b_tracks *
                                      baseucb.ucb$w_cylinders;

    return SS$_NORMAL;			/* Return with success */
}

/* check_geom - this routine is used to verify that the geometry is	*/
/* correct.  Reads for the last block on the higest cylinder and the	*/
/* highest+1 cylinders are done.  If the maximum LBN fails, or the	*/
/* maximum cyl + 1 succeed, then the geometry is suspect.		*/
/*									*/
/* Input:								*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Status:								*/
/*	SS$_NORMAL	success, geometry is ok				*/
/*	SS$_BADPARAM	geometry is incorrect				*/

int check_geom(DQ_UCB *ucb) {

int	sector;				/* Sector number */
int	head;				/* Head number */
int	cyl;				/* Cylinder number */
int	status;				/* Status returned from routines */

/* Attempt to read the maximum block */

    sector= baseucb.ucb$b_sectors;	/* Use highest sector number */
    head  = baseucb.ucb$b_tracks - 1;	/* Use highest head number */
    cyl   = baseucb.ucb$w_cylinders - 1;/* Use highest cylinder number */

    status = read_sector(sector,head,cyl,ucb); /* Try the read */
    if ($FAIL(status))			/* Check for success */
        return SS$_BADPARAM;		/* It failed - return with failure */

/* Attempt to read the end of the next cylinder.  Return an error if this works. */

    cyl++;				/* Point to nonexistent cylinder */
    status = read_sector(sector,head,cyl,ucb);	/* Try the read */
    if ($SUCCESS(status))		/* Check for success */
        return SS$_BADPARAM;		/*  It worked - return with failure */

    return SS$_NORMAL;			/* Return with success */
}

/* set_geom - this routine is used to set the current geometry in the	*/
/* drive.								*/
/*									*/
/* Input:								*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Status:								*/
/*	SS$_NORMAL	success, geometry is ok				*/
/*	SS$_BADPARAM	geometry is incorrect				*/

int set_geom(DQ_UCB *ucb) {

int	sector;				/* Sector number */
int	head;				/* Head number */
int	cyl;				/* Cylinder number */
int	drv;				/* Drive/head information */
int	status;				/* Status returned from routines */
int	orig_ipl;			/* Original IPL */
BYTE	sts;				/* Status CSR */
BYTE	err;				/* Error CSR */

/* Attempt to read the maximum block */

    sector= baseucb.ucb$b_sectors;	/* Use highest sector number */
    drv   = ucb->ucb$l_drv_head+(baseucb.ucb$b_tracks-1); /* Use highest head number */
    cyl   = baseucb.ucb$w_cylinders - 1;/* Use highest cylinder number */

/* Obtain devicelock and write registers and send the command */

    device_lock(baseucb.ucb$l_dlck,RAISE_IPL, &orig_ipl);
    out(WT_SEC_CNT,sector,ucb);		/* Set sectors/track */
    out(WT_DRV_HD,drv,ucb);		/* Set heads/cylinder */
    out(WT_CMD,CMD_INIT_DRV,ucb);	/* Set the drive parameters */

/* Wait for the interrupt and all that goes with that */

    status = WFIKPCH(ucb->ucb$ps_kpb,TIMEOUT_TIME,orig_ipl);

/* If TIMEOUT, then FORK and return with SS$_TIMEOUT status */
/*  else, if other error, then use DEVICE UNLOCK and return with error */

    if (status == SS$_TIMEOUT) {
        erl_std$devictmo(1,(UCB *)ucb);	/* Handle the device timeout */
        status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);
        return SS$_TIMEOUT;		/* Return with timeout */
    } else if ($FAIL(status)) {
        device_unlock(baseucb.ucb$l_dlck,orig_ipl,SMP_RESTORE);
        return status;			/* Return with error */
    }

/* Drop back to fork IPL */

    status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);

/* Check the error status */

    sts = inp(RD_STATUS,ucb);		/* Get the current status */
    if ( IS_SET(sts,STS_M_ERR) ) {	/* Check for ERR bit */
        err = inp(RD_ERROR,ucb);	/* Get the error code */
        return SS$_IVADDR;		/*  and return an error */
    }

    if (IS_CLEAR(sts,STS_M_DRDY)) 	/* If not READY */
        return SS$_DRVERR;		/*  return with DRIVE ERROR */

    return SS$_NORMAL;			/* Return with success */
}

/* read_sector - this routine is used to perform a simple read.  No data*/
/* is transferred to any buffers.  The routine simply tests whether or	*/
/* not a read may be performed to a particular address.			*/
/*									*/
/* Input:								*/
/*	sector	sector number						*/
/*	head	head number						*/
/*	cylinder cylinder number					*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Status:								*/
/*	SS$_NORMAL	success						*/
/*	SS$_IVADDR	failed to read sector				*/
/*	SS$_TIMEOUT	timeout (lower level routine failure)		*/
/*	SS$_DRVERR	drive error (not ready after operation)		*/

int read_sector(int sector, int head, int cylinder, DQ_UCB *ucb) {

int	status;				/* Routine return status code */
BYTE	err;				/* Error register */
BYTE	sts;				/* Status CSR */
int	drv_head;			/* Drive/head register value */
int	orig_ipl;			/* Original IPL */
int	index;				/* Index for flushing data */
WORD	data;				/* Dummy for storing data CSR word */

/* Wait for drive to be ready for a command */

    if (!(status = wait_ready(ucb)))	/*  Wait for drive ready */
        return status;			/* Return with error */

/* Select and set up the drive */

    if (sector >= MAX_SECTOR)		/* Check the sector number */
        return SS$_IVADDR;		/* Return invalid address error */
    if (head >= MAX_HEAD)		/* Check the head number */
        return SS$_IVADDR;		/* Return invalid address error */

    drv_head = ucb->ucb$l_drv_head+head;/* Compute drive and head info */

/* Obtain devicelock and write registers and send the command */

    device_lock(baseucb.ucb$l_dlck,RAISE_IPL, &orig_ipl);
    out(WT_DRV_HD,drv_head,ucb);	/* Select drive and head */
    out(WT_SEC_CNT,1,ucb);		/* Ask for 1 sector */
    out(WT_SECTOR,sector,ucb);		/* Put in the sector number */
    out(WT_CYL_LO,cylinder,ucb);	/* Low order cylinder bits */
    out(WT_CYL_HI,cylinder>>8,ucb);	/* High order cylinder bits */
    out(WT_CMD,CMD_READ_VERIFY,ucb);	/* Attempt to read the sector */

/* Wait for the interrupt and all that goes with that */

    status = WFIKPCH(ucb->ucb$ps_kpb,TIMEOUT_TIME,orig_ipl);

/* If TIMEOUT, then FORK and return with SS$_TIMEOUT status */
/*  else, if other error, then use DEVICE UNLOCK and return with error */

    if (status == SS$_TIMEOUT) {
        erl_std$devictmo(2,(UCB *)ucb);	/* Handle the device timeout */
        status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);
        return SS$_TIMEOUT;		/* Return with timeout */
    } else if ($FAIL(status)) {
        device_unlock(baseucb.ucb$l_dlck,orig_ipl,SMP_RESTORE);
        return status;			/* Return with error */
    }

/* Drop back to fork IPL */

    status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);

/* Throw away the sector to keep the drive happy */

    for (index = 0; index < (BLK_SIZE/2); index++) {
        data = inpw(RD_DATA,ucb);	/* Get a word */
    }

/* Check the error status */

    sts = inp(RD_STATUS,ucb);		/* Get the current status */
    if ( IS_SET(sts,STS_M_ERR) ) {	/* Check for ERR bit */
        err = inp(RD_ERROR,ucb);	/* Get the error code */
        return SS$_IVADDR;		/*  and return an error */
    }

    if (IS_CLEAR(sts,STS_M_DRDY)) 	/* If not READY */
        return SS$_DRVERR;		/*  return with DRIVE ERROR */

    return SS$_NORMAL;			/* Return with success */
}

/* seek - Perform SEEK operation					*/
/*									*/
/* IDE drives will return immediately upon issuing the first seek	*/
/* command.  Subsequent commands will actually wait until the seek	*/
/* is completed.  This routine issues only one seek command and then	*/
/* completes the I/O.  Any subsequent command will be stalled until the	*/
/* seek is completed.							*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/
/*		SS$_NORMAL	seek complete				*/

int seek(DQ_UCB *ucb) {

int	status;				/* Status of calls */
int	orig_ipl;			/* Original IPL */
int	cyl;				/* Cylinder number */
BYTE	sts;				/* Status CSR */
BYTE	err;				/* Error CSR */

    if (!(status = wait_ready(ucb)))	/*  Wait for drive ready */
        return status;			/* Return with error */

/* Set up seek parameters */

    cyl = ucb->ucb$l_media.lbn;		/* Get the cylinder number */

/* Obtain devicelock and write registers and send the command */

    device_lock(baseucb.ucb$l_dlck,RAISE_IPL, &orig_ipl);
    out(WT_DRV_HD,ucb->ucb$l_drv_head,ucb);	/* Select drive and head */
    out(WT_SECTOR,1,ucb);		/* Put in the sector number */
    out(WT_CYL_LO,cyl,ucb);		/* Low order cylinder bits */
    out(WT_CYL_HI,cyl>>8,ucb);		/* High order cylinder bits */
    out(WT_CMD,CMD_SEEK,ucb);		/* Attempt to seek to the sector */

/* Wait for the interrupt and all that goes with that */

    status = WFIKPCH(ucb->ucb$ps_kpb,TIMEOUT_TIME,orig_ipl);

/* If TIMEOUT, then FORK and return with SS$_TIMEOUT status */
/*  else, if other error, then use DEVICE UNLOCK and return with error */

    if (status == SS$_TIMEOUT) {
        erl_std$devictmo(3,(UCB *)ucb);	/* Handle the device timeout */
        status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);
        return SS$_TIMEOUT;		/* Return with timeout */
    } else if ($FAIL(status)) {
        device_unlock(baseucb.ucb$l_dlck,orig_ipl,SMP_RESTORE);
        return status;			/* Return with error */
    }

/* Drop back to fork IPL */

    status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);

/* Check the status */

    sts = inp(RD_STATUS,ucb);		/* Get the status byte */
    if (IS_SET(sts,STS_M_ERR) ) {	/* Check for ERROR setting */
        err = inp(RD_ERROR,ucb);	/* Get the error byte */
        return SS$_DRVERR;		/* Return with DRIVE ERROR status */
    }

    return SS$_NORMAL;			/* Return with success */
}

/* DRVCLR - Perform Drive Clear operation				*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/

int drvclr(DQ_UCB *ucb) {

    return SS$_NORMAL;
}

/* read - Performs IO$_READxBLK driver function				*/
/*									*/
/* This routine issues READ commands to the drive.  This routine will	*/
/* break up the request into segments of not more than 127 sectors per	*/
/* command.								*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/

int read(DQ_UCB *ucb) {

int	xfer_size;			/* Size (in sectors) */
int	xfer_cnt;			/* Number of sectors transferred */
int	base_lbn;			/* Index for read */
int	byte_cnt;			/* Number of bytes moved */
int	xfer_req;			/* Number of sectors requested */
int	status;				/* Routine return status */
int	retry_cnt;			/* Error retry count */
int	buf_ofs;			/* Offset into user buffer */
BYTE	*user_va;			/* Returned user buffer address */

/* Wait for drive to be ready for a command */

    status = wait_ready(ucb);		/* Wait for drive ready */
    if ($FAIL(status))			/* Check status for error */
        return status;			/* Return with error */

/* Compute number of blocks and set up */

    xfer_size = (baseucb.ucb$l_bcnt + BLK_SIZE-1) >> BLK_SHIFT;
    if (xfer_size == 0)			/* Was there any work to do ? */
        return SS$_NORMAL;		/* Exit with success if not */

/* Loop over each segment.  BASE_LBN increments by MAX_XFER blocks per loop */

    retry_cnt = 0;			/* Clear the retry counter */
    for (base_lbn = 0; base_lbn < xfer_size;) {
        xfer_req = xfer_size-base_lbn;	/* Compute sectors to read */
        status = read_segment(ucb,xfer_req,&xfer_cnt,(BYTE *)ucb->ucb$ps_xfer);

	/* Check the status. */
	/* On success - clear the retry count and continue */
	/* On error - if the xfer_cnt is non-zero, then we are making	*/
	/*	progress, so just continue.  If the xfer_cnt is 0, then	*/
	/*	we are losing ground.  Retry for a while, then reset	*/
	/*	and then try some more.  Finally, give up.		*/

        if ($FAIL(status)) {
            if (xfer_cnt == 0) {
                retry_cnt++;		/* Update retry count */
                if (retry_cnt == MAX_RETRY/2)
                    reset_ctrl(ucb);	/* Attempt a reset */
                if (retry_cnt > MAX_RETRY) /* Were there too many retries ? */
                    return status;	/* Yes, exit with error */
            }
        } else {
            if (xfer_cnt > 0)		/* Was any data transferred ? */
                retry_cnt = 0;		/* Clear retry count on each success */
        }

        if (xfer_cnt == 0)		/* Check that we got something */
            return status;		/* Just exit with status if not */

	/* This segment is now in the transfer buffer.  Move it to the user */

        byte_cnt = xfer_cnt << BLK_SHIFT; /* Get byte count for this segment */
        if (byte_cnt > ucb->ucb$l_bcr) 	/* Check for too large */
            byte_cnt = ucb->ucb$l_bcr;	/* Minimize it */

        buf_ofs = (ucb->ucb$l_media.lbn - ucb->ucb$l_org_media) * BLK_SIZE;
        user_va = map_user_buffer(ucb,buf_ofs,byte_cnt); /* Map user buffer */
        memcpy(user_va,ucb->ucb$ps_xfer,byte_cnt);

	/* Update transfer information */

        ucb->ucb$l_bcr -= byte_cnt;	/* Update the byte count */
        ucb->ucb$l_media.lbn += xfer_cnt;/* Bump the LBN */
        base_lbn += xfer_cnt;		/* Update the block number */
    }

    return SS$_NORMAL;			/* Return with success */
}

/* read_segment - read one segment					*/
/*									*/
/* This routine performs the read of a single I/O segment.  Each segment*/
/* is a single read command of not more the MAX_XFER sectors.  The	*/
/* overall read I/O routine calls this routine for each of the segments	*/
/* until the entire read is completed.					*/
/*									*/
/* Input:								*/
/*	ucb		pointer to UCB					*/
/*	xfer_req	number of blocks remaining to transfer		*/
/*	buffer		address of buffer to transfer data into		*/
/*									*/
/* Output:								*/
/*	xfer_cnt	actual count of sectors transferred		*/
/*	status value							*/

int read_segment(DQ_UCB *ucb,int xfer_req,int *xfer_cnt,BYTE *buffer) {

int	sec, head, cyl;			/* Disk location (sector/head/cyl) */
int	drv_head;			/* Drive/head register value */
int	int_cnt;			/* Interrupt (sector) counter */
BYTE	*xfer;				/* Pointer to transfer buffer */
int	status;				/* Returned status from routine */
int	orig_ipl;			/* Saved IPL */
BYTE	sts;				/* Status CSR */
BYTE	err;				/* Error CSR */
WORD	datal, datah;			/* Input data */
BYTE	*user_va;			/* Mapped user buffer address */
int	buf_ofs;			/* Offset to start of transfer */
int	buf_len;			/* Length of user buffer to map */

/* Set up for the transfer */

    *xfer_cnt = 0;			/* Set "sectors transferred" to 0 */
    status = SS$_BADPARAM;		/* Init status to weird failure */
    drv_head= ucb->ucb$l_drv_head;	/* Get drive info */
    if (xfer_req > MAX_XFER)		/* Check for too large */
        xfer_req = MAX_XFER;		/*  and minimize if too big */
    if (ucb->ucb$l_flags.bits.lba)	/* If LBA mode, ... */
        drv_head |= DRVHD_M_LBA;	/* Set the LBA bit */
    compute_address(ucb,&sec,&head,&cyl); /* Compute physical address */

/* Obtain device lock and ask for the sectors */

    device_lock(baseucb.ucb$l_dlck,RAISE_IPL, &orig_ipl);
    out(WT_DRV_HD,drv_head|head,ucb);	/* Select drive and head */
    out(WT_SEC_CNT,xfer_req,ucb);	/* Ask for "n" sectors */
    out(WT_SECTOR,sec,ucb);		/* Put in the sector number */
    out(WT_CYL_LO,cyl,ucb);		/* Low order cylinder bits */
    out(WT_CYL_HI,cyl>>8,ucb);		/* High order cylinder bits */
    out(WT_CMD,ucb->ucb$l_read_cmd,ucb);/* Attempt to read the sector */

/* Now, wait for each of the interrupts that will come with each sector */

    xfer = buffer;				/* Point to transfer buffer */
    for (int_cnt = 0; int_cnt < xfer_req; int_cnt++) {
        status = WFIKPCH(ucb->ucb$ps_kpb,TIMEOUT_TIME,orig_ipl);

/* If TIMEOUT, then FORK and return with SS$_TIMEOUT status */
/*  else, if other error, then use DEVICE UNLOCK and return error */

        if (status == SS$_TIMEOUT) {
            erl_std$devictmo(4,(UCB *)ucb);	/* Handle the device timeout */
            exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);
            break;			/* Exit the loop with STATUS intact */
        } else if ($FAIL(status)) {
            device_unlock(baseucb.ucb$l_dlck,orig_ipl,SMP_RESTORE);
            break;			/* Exit the loop */
        }

/* Read STATUS (dismissing interrupt) and drop back to fork IPL */

        sts = inp(RD_STATUS,ucb);	/* Get the status byte */
        status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);

/* Get the data from the silo and into the transfer buffer */

        *xfer_cnt += 1;			/* Increment sector counter */
        move_sec_from_drive(ucb,&xfer);	/* Get sector from the drive */

/* Check the status (saved from above) */

        if (IS_SET(sts,STS_M_ERR) ) { 	/* If there was an error */
           err = inp(RD_ERROR,ucb);	/*  get the error byte */
           status = SS$_DRVERR;		/* Set the error status */
           break;			/*  and exit the loop */
        }

/* All is well for this sector - set the status to success */

        status = SS$_NORMAL;		/* Set "success" status */
    }

    return status;			/* Exit with status */
}

/* datacheck - Performs data check function for read and write		*/
/* requests.								*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/
/*		SS$_NORMAL - success					*/
/*		SS$_DATACHECK - data failed to compare			*/

int datacheck(DQ_UCB *ucb) {

int	xfer_size;			/* Size (in sectors) */
int	xfer_cnt;			/* Number of sectors transferred */
int	base_lbn;			/* Index for read */
int	byte_cnt;			/* Number of bytes moved */
int	xfer_req;			/* Number of sectors requested */
int	status;				/* Routine return status */
int	retry_cnt;			/* Error retry count */
int	buf_ofs;			/* Offset into user buffer */
BYTE	*user_va;			/* Returned user buffer address */

/* First, reset all the parameters that we'll need */

    ucb->ucb$l_bcr = ucb->ucb$l_org_bcnt;	/* Get byte count */
    ucb->ucb$l_media.lbn = ucb->ucb$l_org_media;/* Get first block number */
    baseucb.ucb$l_bcnt = ucb->ucb$l_org_bcnt;	/* Get byte count */
    baseucb.ucb$l_svapte = ucb->ucb$l_org_svapte; /* Get SVAPTE */
    baseucb.ucb$l_boff = ucb->ucb$l_org_boff;	/* Get BOFF */

/* Wait for drive to be ready for a command */

    status = wait_ready(ucb);		/* Wait for drive to be ready */
    if ($FAIL(status))			/* Check return status */
        return status;			/*  and exit on error */

/* Compute number of blocks and set up */

    xfer_size = (baseucb.ucb$l_bcnt + BLK_SIZE-1) >> BLK_SHIFT;
    if (xfer_size == 0)			/* Was there any work to do ? */
        return SS$_NORMAL;		/* Exit with success if not */

    retry_cnt = 0;			/* Clear the retry counter */

/* Loop over each segment. BASE_LBN increments by MAX_XFER blocsk per loop */

    for (base_lbn = 0; base_lbn < xfer_size;) {
        xfer_req = xfer_size-base_lbn;	/* Compute sectors to read */
        status = read_segment(ucb,xfer_req,&xfer_cnt,(BYTE *)ucb->ucb$ps_xfer);

	/* Check the status. */
	/* On success - clear the retry count and continue */
	/* On error - if the xfer_cnt is non-zero, then we are making	*/
	/*	progress, so just continue.  If the xfer_cnt is 0, then	*/
	/*	we are losing ground.  Retry for a while, then reset	*/
	/*	and then try some more.  Finally, give up.		*/

        if ($FAIL(status)) {
            if (xfer_cnt == 0) {
                retry_cnt++;		/* Update retry count */
                if (retry_cnt == MAX_RETRY/2)
                    reset_ctrl(ucb);	/* Attempt a reset */
                if (retry_cnt > MAX_RETRY) /* Were there too many retries ? */
                    return status;	/* Yes, exit with error */
            }
        } else {
            if (xfer_cnt > 0)		/* Was any data transferred ? */
                retry_cnt = 0;		/* Clear retry count on each success */
        }

	/* This segment is now in the xfer buffer */

        byte_cnt = xfer_cnt<<BLK_SHIFT;	/* Compute byte count */
        if (byte_cnt > ucb->ucb$l_bcr)	/* Check for too large */
            byte_cnt = ucb->ucb$l_bcr;	/* Minimize it */

	/* Compare xfer buffer and user buffer */

        buf_ofs = (ucb->ucb$l_media.lbn - ucb->ucb$l_org_media) * BLK_SIZE;
        user_va = map_user_buffer(ucb,buf_ofs,byte_cnt);	/* Map the user buffer */
        status = memcmp(ucb->ucb$ps_xfer,user_va,byte_cnt);
        if (status != 0)		/* Check comparison results */
            return SS$_DATACHECK;	/* Failed - return DATACHECK error */

	/* Update transfer parameters */

        ucb->ucb$l_bcr -= byte_cnt;	/* Update the byte count */
        ucb->ucb$l_media.lbn += xfer_cnt;/* Bump the LBN */
        base_lbn += xfer_cnt;		/* Update the block number */

    }

    return SS$_NORMAL;			/* Return with success */
}

/* write - Performs IO$_WRITExBLK driver functions			*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/

int write(DQ_UCB *ucb) {

int	xfer_size;			/* Size (in sectors) */
int	xfer_cnt;			/* Number of sectors transferred */
int	base_lbn;			/* Index for read */
int	byte_cnt;			/* Number of bytes written */
int	xfer_req;			/* Number of sectors requested */
int	status;				/* Routine status */
int	retry_cnt;			/* Retry counter */

/* Wait for drive to be ready for a command */

    status = wait_ready(ucb);		/* Wait for drive to be ready */
    if ($FAIL(status))			/* Check the error */
        return status;			/*  if an error, then return */

/* Compute number of blocks */

    xfer_size = (baseucb.ucb$l_bcnt + BLK_SIZE-1) >> BLK_SHIFT;
    if (xfer_size == 0)			/* Was there any work */
        return SS$_NORMAL;		/* Exit with success if not */

/* Loop over each segment */

    for (base_lbn = 0; base_lbn < xfer_size;) {
        xfer_req = xfer_size - base_lbn;/* Compute blocks left to xfer */
        status = write_segment(ucb,xfer_req,&xfer_cnt,&byte_cnt);

	/* Check the status. */
	/* On success - clear the retry count and continue */
	/* On error - if the xfer_cnt is non-zero, then we are making	*/
	/*	progress, so just continue.  If the xfer_cnt is 0, then	*/
	/*	we are losing ground.  Retry for a while, then reset	*/
	/*	and then try some more.  Finally, give up.		*/

        if ($FAIL(status)) {
            if (xfer_cnt == 0) {
                retry_cnt++;		/* Update retry count */
                if (retry_cnt == MAX_RETRY/2)
                    reset_ctrl(ucb);	/* Attempt a reset */
                if (retry_cnt > MAX_RETRY) /* Were there too many retries ? */
                    return status;	/* Yes, exit with error */
            }
        } else {
            if (xfer_cnt > 0)		/* Was any data transferred ? */
                retry_cnt = 0;		/* Clear retry count on each success */
        }

        ucb->ucb$l_bcr -= byte_cnt;	/* Update byte count remaining */
        ucb->ucb$l_media.lbn += xfer_cnt; /* Update the LBN */
        base_lbn += xfer_cnt;		/* Update LBN in loop */
    }

    return SS$_NORMAL;			/* Return to caller with success */
}

/* write_segment - write one segment					*/
/*									*/
/* This routine performs the write of a single I/O segment.  Each	*/
/* segment is a single read command of not more the MAX_XFER sectors.	*/
/* The overall read I/O routine calls this routine for each of the	*/
/* segments until the entire read is completed.				*/
/*									*/
/* Input:								*/
/*	ucb             pointer to UCB					*/
/*	xfer_req        number of blocks remaining to transfer		*/
/*									*/
/* Output:								*/
/*	xfer_cnt        actual count of sectors transferred		*/
/*	byte_cnt        actual count of bytes transferred		*/
/*	status value							*/

int write_segment(DQ_UCB *ucb,int xfer_req,int *xfer_cnt,int *byte_cnt) {

int	sec;				/* Sector number and count */
int	int_cnt;			/* Interrupt counter */
int	lw_cnt;				/* Data buffer index */
int	head;				/* Head number */
int	cyl;				/* Cylinder number and components */
int	idx;				/* Zero fill index */
int	xfer_idx;			/* Buffer index */
int	status;				/* Routine status value */
void	*temp;				/* Dummy for IOC$MOVFRUSER use */
int	drv_head;			/* Drive and head bits */
BYTE	*buffer;			/* Pointer to disk buffer */
int	orig_ipl;			/* Original IPL */
BYTE	sts, err;			/* STATUS and ERROR CSRs */
WORD	data;				/* Data word */
int	remainder;			/* Bytes left at end of block */
BYTE	*user_va;			/* Mapped user buffer address */
int	buf_ofs;			/* Offset into user buffer */

/* Set up for the transfer */

    *xfer_cnt = 0;			/* Clear count of sectors xfered */
    *byte_cnt = 0;			/* Clear count of bytes xfered */
    status = SS$_BADPARAM;		/* Init status to weird failure */
    drv_head = ucb->ucb$l_drv_head;	/* Get base drive info */
    if (ucb->ucb$l_flags.bits.lba)	/* If LBA mode, ... */
        drv_head |= DRVHD_M_LBA;	/*  ... set the LBA bit */
    compute_address(ucb,&sec,&head,&cyl); /* Compute the address */
    if (xfer_req > MAX_XFER)		/* Check for too large a transfer */
        xfer_req = MAX_XFER;		/*  and limit it if so */

/* Move the data segment from the user */

    *byte_cnt = xfer_req << BLK_SHIFT;	/* Compute byte count */
    if (*byte_cnt > ucb->ucb$l_bcr)	/* Check for too large */
        *byte_cnt = ucb->ucb$l_bcr;	/* Minimize it */

    buf_ofs = (ucb->ucb$l_media.lbn - ucb->ucb$l_org_media) * BLK_SIZE;
    user_va = map_user_buffer(ucb,buf_ofs,*byte_cnt);	/* Map the user buffer */
    memcpy(ucb->ucb$ps_xfer,user_va,*byte_cnt);

/* If less than a full block, then zero the remainder */

    remainder = *byte_cnt & BLK_MASK;	/* Compute remainder */
    if ( remainder > 0) {
        remainder = BLK_SIZE - remainder; /* Compute bytes left */
        buffer = (BYTE *) ucb->ucb$ps_xfer; /* Point to buffer */
        for (idx=0; idx < remainder; idx++) {
            buffer[*byte_cnt+idx]=0;	/* Zero the byte */
        }        
    }

    xfer_idx = 0;			/* Byte index in buffer */

/* Take out the device lock, raise IPL, and write the registers */

    device_lock(baseucb.ucb$l_dlck,RAISE_IPL, &orig_ipl);
    out(WT_DRV_HD,drv_head|head,ucb);	/* Select drive and head */
    out(WT_SEC_CNT,xfer_req,ucb);	/* Ask for "n" sectors */
    out(WT_SECTOR,sec,ucb);		/* Put in the sector number */
    out(WT_CYL_LO,cyl,ucb);		/* Low order cylinder bits */
    out(WT_CYL_HI,cyl>>8,ucb);		/* High order cylinder bits */
    out(WT_CMD,ucb->ucb$l_write_cmd,ucb); /* Attempt to write the sector */

/* Loop over each sector in the transfer segment */

    for (int_cnt = 0; int_cnt < xfer_req; int_cnt++) {
        status = wait_drq(ucb);		/* Wait for data request */
        if ($FAIL(status))		/* Check for error */
            break;			/*  and exit if so */

	/* Push out the sector to the drive */

        for (lw_cnt = 0; lw_cnt < BLK_SIZE/4; lw_cnt++) {
            outw(WT_DATA,ucb->ucb$ps_xfer[xfer_idx],ucb);	/* Write out the data word */
            outw(WT_DATA,ucb->ucb$ps_xfer[xfer_idx]>>16,ucb); /* Write out the data word */
            xfer_idx++;		/* Move to next longword in block */
        }

	/* Wait for the interrupt and all that goes with that */

        status = WFIKPCH(ucb->ucb$ps_kpb,TIMEOUT_TIME,orig_ipl);

	/* If TIMEOUT, then FORK and return with SS$_TIMEOUT status */
	/*  else, if other error, then use DEVICE UNLOCK and return error */

        if (status == SS$_TIMEOUT) {
            erl_std$devictmo(5,(UCB *)ucb);	/* Handle the device timeout */
            exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);
            break;			/*  and exit with status intact */
        } else if ($FAIL(status)) {
            device_unlock(baseucb.ucb$l_dlck,orig_ipl,SMP_RESTORE);
            break;			/*  and exit with status */
        }

	/* Read STATUS (dismissing interrupt) and drop back to fork IPL */

        sts = inp(RD_STATUS,ucb);	/* Get the status byte */
        status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);
        *xfer_cnt += 1;			/* Update sectors transferred */

	/* Check the status */

        if (IS_SET(sts,STS_M_ERR) ) { 	/* If there was an error */
            err = inp(RD_ERROR,ucb);	/*  get the error byte */
            status = SS$_DRVERR;	/* Return with DRIVE ERROR status */
            break;			/*  and exit loop */
        }

/* All is well, set the status for exit */

        status = SS$_NORMAL;		/* Set sucess status */
    }

/* Make sure that the returned "byte_cnt" is accurate */

    *byte_cnt = *xfer_cnt << BLK_SHIFT;	/* Compute bytes transferred */
    if (*byte_cnt > ucb->ucb$l_bcr)	/* Check for too large */
        *byte_cnt = ucb->ucb$l_bcr;	/* Minimize it */

    return status;			/* Return to caller */
}

/* READRCT - Perform READRCT operation					*/
/*									*/
/* This routine returns the drive information page.			*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Return value:							*/
/*	status value							*/
/*		SS$_NORMAL - success					*/
/*		SS$_NODATA - failed to get drive information		*/

int readrct(DQ_UCB *ucb) {

int	status;				/* Routine return status */
int	orig_ipl;			/* Original IPL */
BYTE	err;				/* Error register */
WORD	datal, datah;			/* Data from drive */
int	index;				/* Data index */
void	*temp;				/* Dummy for IOC$MOVTOUSER call */
BYTE	*xfer;				/* Pointer to transfer buffer */

/* Wait for drive to be ready for a command */

    status = wait_ready(ucb);		/*  Wait for drive ready */
    if ($FAIL(status))			/* Check for error */
        return status;			/*  and return if there is one */

/* Select the drive, then ask for drive information */

	/* Obtain devicelock and write registers and send the command */

    device_lock(baseucb.ucb$l_dlck,RAISE_IPL, &orig_ipl);
    out(WT_DRV_HD,ucb->ucb$l_drv_head,ucb); /* Select drive and head 0 */
    out(WT_CMD,CMD_IDENTIFY,ucb);	/* Ask for drive info */

/* Wait for the interrupt and all that goes with that */

    status = WFIKPCH(ucb->ucb$ps_kpb,TIMEOUT_TIME,orig_ipl);

/* If TIMEOUT, then FORK and return with SS$_TIMEOUT status */
/*  else, if other error, then use DEVICE UNLOCK and return with error */

    if (status == SS$_TIMEOUT) {
        erl_std$devictmo(6,(UCB *)ucb);	/* Handle the device timeout */
        status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);
        return SS$_TIMEOUT;		/* Return with timeout */
    } else if ($FAIL(status)) {
        device_unlock(baseucb.ucb$l_dlck,orig_ipl,SMP_RESTORE);
        return status;			/* Return with error */
    }

/* Drop back to fork IPL */

    status = exe$kp_fork(ucb->ucb$ps_kpb,(FKB *) ucb);

/* Check the error status and exit if an error occurred */

    if ( IS_SET(inp(RD_STATUS,ucb),STS_M_ERR) ) /* Check for an error */
        return SS$_NODATA;		/* Failed - exit with error */

/* Get the data from the drive */

    xfer = (BYTE *) ucb->ucb$ps_xfer;	/* Point to transfer buffer */
    move_sec_from_drive(ucb,&xfer);	/* Move sector to buffer */

/* Move the data to the user */

    ioc_std$movtouser(ucb->ucb$ps_xfer,baseucb.ucb$l_bcnt,(UCB *)ucb,&temp);

    return SS$_NORMAL;			/* Return to caller with success */
}

BYTE *map_user_buffer(DQ_UCB *ucb,int offset,int length) {

/* map_user_buffer - this routine is used to directly map a section of	*/
/* the users buffer.							*/
/*									*/
/* Input:								*/
/*	ucb	pointer to the UCB					*/
/*	offset	offset to start of buffer to be mapped			*/
/*	length	total length to map					*/
/*									*/
/* Output:								*/
/*	user_va	returned address of mapped buffer			*/
/*									*/

int	pfn;				/* PFN */
int	first_pte;			/* First PTE number */
int	pte_cnt;			/* Number of pages to map */
int	i;				/* Loop counter */
int	byte_ofs;			/* Byte offset */
PTE	*user_pte;			/* Current PTE pointer */
BYTE	*s0_va;				/* Current S0 address */
PTE	*s0_pte;			/* Current S0 PTE address */
BYTE	*user_va;			/* Mapped buffer address */
uint64	*clr_pte;			/* Pointer used to clear PTE */

#define PTE_BITS PTE$C_KOWN + PTE$C_KW + PTE$M_VALID + PTE$M_ASM

/* Calculate sizes, base PTE addresses and such */

    offset += baseucb.ucb$l_boff;	/* Compute true offset from page */
    byte_ofs = offset & MMG$GL_BWP_MASK; /* Compute byte offset in page */
    first_pte = (offset >> MMG$GL_VPN_TO_VA) *
                PTE$C_BYTES_PER_PTE;	/* Compute PTE offset */
    pte_cnt = (((offset & MMG$GL_BWP_MASK) + length) + MMG$GL_BWP_MASK) >>
              MMG$GL_VPN_TO_VA;		/* Compute page count */

    user_pte = (PTE *)((int)baseucb.ucb$l_svapte + first_pte); /* Compute first PTE address */
    s0_va   = ucb->ucb$ps_s0_va;	/* S0 address of mapped region */
    s0_pte  = ucb->ucb$ps_s0_svapte;	/* Get S0 PTE address */

/* Loop over all of the PTEs and set them to double map the user buffer */

    for (i=0; i<pte_cnt; i++) {
        if (user_pte->pte$v_valid)	/* Check for VALID user PTE */
            pfn = user_pte->pte$v_pfn;	/* It is - get copy of PFN */
        else
            pfn = ioc_std$ptetopfn(user_pte);	/* Find PFN the hard way */

	/* The following should be set field by field, but PTEDEF	*/
	/* doesn't have a proper definition for this, and frankly it's	*/
	/* a pain !  So, define some bits and use them directly.	*/
/*      s0_pte->pte$v_own = PTE$C_KOWN; /* Owner = Kernel */
/*      s0_pte->pte$v_prot = PTE$C_KW;	/* Protection = Kernel Write */
/*      s0_pte->pte$v_valid = 1;	/* Valid page */
/*      s0_pte->pte$v_asm = 1;		/* Address space match */

        clr_pte = (void *)s0_pte;	/* Point to the PTE */
        *clr_pte = PTE_BITS;		/* Clear the PTE and set constant bits */
        s0_pte->pte$v_pfn = pfn;	/* Now, include the PFN */
        mmg$tbi_single(s0_va);		/* Invalidate the address */
        s0_va += MMG$GL_PAGE_SIZE;	/* Point to the next page */
        s0_pte++;			/* Point to next S0 PTE */
        user_pte++;			/* Point to next user PTE */
        }

/* Now, make a guard page */

    clr_pte = (void *)s0_pte;		/* Get PTE address */
    *clr_pte= 0;			/*  and clear it */
    mmg$tbi_single(s0_va);		/* Invalidate the address */

/* Return the S0 VA of the user buffer */

    user_va = (BYTE *)((int)ucb->ucb$ps_s0_va + byte_ofs);
    return	user_va;		/* Return with the address */
}

/* move_sec_from_drive - This routine is used to move a sector from the	*/
/* disk drive on a READ operation.					*/
/*									*/
/* Input:								*/
/*	ucb	pointer to the UCB					*/
/*	buffer	address of pointer to buffer to place data		*/
/*									*/
/* Output:								*/
/*	buffer	updated buffer pointer					*/
/*	none								*/

void move_sec_from_drive(DQ_UCB *ucb, BYTE **buffer) {

int	lw_cnt;				/* Counter for data transfer */
WORD	datal, datah;			/* Input data */
UINT	*bp;				/* LONGWORD buffer pointer */

/* Read all of the data from the silo into the driver's buffer */

    bp = (UINT *) *buffer;		/* Use as a longword pointer */

        for (lw_cnt = 0; lw_cnt < (BLK_SIZE/4) ; lw_cnt++) {
           datal = inpw(RD_DATA,ucb);	/* Get the data word */
           datah = inpw(RD_DATA,ucb);	/* Get the data word */
           *bp = (datah<< 16) + datal;	/* Move data to the buffer */
           bp++;			/* And move to the next longword */
        }

    *buffer = (BYTE *)bp;		/* Copy back updated pointer */

    return;
}

/* COMPUTE_ADDRESS - this routine is used to compute the head, sector	*/
/* and track information from a logical block number.  Note that on IDE	*/
/* disks, sector numbers start at 1.  Head and cylinder numbers start	*/
/* at 0.								*/
/*									*/
/* LBA mode addressing is handled if the LBA flag is set.  In LBA mode,	*/
/* the address is still returned in the sec, head and cyl locations,	*/
/* but it is simply the sections of the LBA.  The callers of this	*/
/* routine simply write these value to the registers, so this all	*/
/* works just fine.							*/
/*									*/
/* Input:								*/
/*	ucb	pointer to the UCB					*/
/*									*/
/* Output:								*/
/*		CHS mode		LBA mode			*/
/*	sec	sector number		LBA[0:7]			*/
/*	head	head number		LBA[24:27]			*/
/*	cyl	cylinder number		LBA[8:23]			*/

void compute_address(DQ_UCB *ucb,int *sec, int *head, int *cyl) {

int temp;				/* Temporary value */

    if (ucb->ucb$l_flags.bits.lba == 0) {
        *sec  = ucb->ucb$l_media.lbn % baseucb.ucb$b_sectors + 1;
        temp  = ucb->ucb$l_media.lbn / baseucb.ucb$b_sectors;
        *head = temp % baseucb.ucb$b_tracks;
        *cyl  = temp / baseucb.ucb$b_tracks;
    } else {
        *sec  =  ucb->ucb$l_media.lbn        & 0x00FF;	/* Bits 0-7 */
        *cyl  = (ucb->ucb$l_media.lbn >> 8)  & 0xFFFF;	/* Bits 8 - 23 */
        *head = (ucb->ucb$l_media.lbn >> 24) & 0x000F;	/* Bits 24 - 27 */
    }
}

/* unload - Perform IO$_UNLOAD driver function				*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/
/*		SS$_NORMAL	function completed successfully		*/

int unload(DQ_UCB *ucb) {

    baseucb.ucb$v_valid = 0;		/* Clear the VALID bit */
    ucb->ucb$l_read_cmd  = CMD_READ;	/* Default read command is 1 sector */
    ucb->ucb$l_write_cmd = CMD_WRITE;	/* Default write command is 1 sector */
    return SS$_NORMAL;			/* Return with success */
}

/* wait_ready - Wait Until The Drive Is Ready.  This means that the	*/
/* BSY status bit is clear and the DRDY status bit is set.		*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/
/*		SS$_NORMAL	function completed successfully		*/
/*		SS$_DEVACTIVE	BUSY never cleared			*/

int wait_ready(DQ_UCB *ucb) {

int	status;				/* Routine status value */
BYTE	sts;				/* Status byte */

/* First, check that BUSY is clear so we can select the drive */

    status = wait_busy(ucb);		/* Wait for BUSY to clear */
    if ($FAIL(status))			/* Check status for error */
        return status;			/* Exit with the error code */    

/* Then select the drive that we're really interested in */

    out(WT_DRV_HD,ucb->ucb$l_drv_head,ucb); /* Select drive and head 0 */

/* Then, wait for BUSY to clear on this drive */

    status = wait_busy(ucb);		/* Wait for BUSY to clear */
    if ($FAIL(status))			/* Check status for error */
        return status;			/* Exit with the error code */    

/* Finally, check that the drive is ready */

    sts = inp(RD_ALT_STS,ucb);		/* Get the status byte */
    if ( IS_CLEAR(sts,STS_M_DRDY) )	/*  Check for drive READY */
        return SS$_DEVACTIVE;		/*   and exit with failure if not */

/* Drive is ready */

    return SS$_NORMAL;			/* Return success */
}

/* wait_drq - Wait for DRQ to be set and BSY to be clear		*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/

int wait_drq(DQ_UCB *ucb)

{
int	status;				/* Routine status value */
BYTE	sts;				/* Status byte */
__int64	delta_time;			/* Timedwait delta time */
__int64	end_value;			/* Timedwait end value */

/* Check to see if the drive is ready right now */

    sts = inp(RD_ALT_STS,ucb);		/* Get the status byte */
    if ( IS_CLEAR(sts,STS_M_BSY)) {	/* If not busy, then */
        if ( IS_SET(sts,STS_M_DRQ) )	/*  get the DRQ bit */
            return SS$_NORMAL;		/* Drive is ready - exit */
    }

/* Drive is busy or DRQ not set - wait a bit for it */

	/* Set up the timedwait */

    delta_time = DRQ_TIME;		/* Set DRQ wait time */
    status = exe$timedwait_setup(&delta_time,&end_value);
    if ($FAIL(status) )			/* Check for success */
        return status;			/* Return with the failure status */

	/* Spin until ready or timeout */

    while((status=exe$timedwait_complete(&end_value)) == SS$_CONTINUE) {
        sts = inp(RD_ALT_STS,ucb);	/* No, so read status byte */
        if ( IS_CLEAR(sts,STS_M_BSY)) {
            if ( IS_SET(sts,STS_M_DRQ) ) {
                status = SS$_NORMAL;	/* Looks ok - set new status */
                break;			/* And exit */
            }
        }
    }

    return status;			/* Return with status code */
}

/* wait_busy - Wait for BSY to be clear					*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/

int wait_busy(DQ_UCB *ucb)

{
int	status;				/* Routine status value */
BYTE	sts;				/* Status byte */
__int64	delta_time;			/* Timedwait delta time */
__int64	end_value;			/* Timedwait end value */

/* Check to see if the drive is ready right now */

    sts = inp(RD_ALT_STS,ucb);		/* Get the status byte */
    if ( IS_CLEAR(sts,STS_M_BSY))	/* If not busy, then */
        return SS$_NORMAL;		/* Drive is ready - exit */

/* Drive is busy - wait a bit for it */

	/* Set up the timedwait */

    delta_time = DRQ_TIME;		/* Set DRQ wait time */
    status = exe$timedwait_setup(&delta_time,&end_value);
    if ($FAIL(status) )			/* Check for success */
        return status;			/* Return with the failure status */

	/* Spin until ready or timeout */

    while((status=exe$timedwait_complete(&end_value)) == SS$_CONTINUE) {
        sts = inp(RD_ALT_STS,ucb);	/* Read status byte */
        if ( IS_CLEAR(sts,STS_M_BSY))	/* Check for it to be clear */
            return SS$_NORMAL;		/* BUSY is clear - exit */
    }

/* Ok - still not ready.  Let's reset the controller and try again */

    reset_ctrl(ucb);			/* Reset the drive */
    sts = inp(RD_ALT_STS,ucb);		/* Get the status byte */
    if ( IS_CLEAR(sts,STS_M_BSY))	/* If not busy, then */
        return SS$_NORMAL;		/*  return with success */
    else
        return SS$_CTRLERR;		/* Exit with controller error */
}

#ifdef DEBUG
/* wfikpch_hist - Wait for Interrupt and Keep Channel w/Histogram	*/
/*									*/
/* This routine is a jacket around the normal ioc$kp_wfikpch.  This	*/
/* routine will keep the time completion histogram up to date.		*/
/*									*/
/* Input:								*/
/*	kpb	pointer to the KPB					*/
/*	tmo	timeout value						*/
/*	newipl	new IPL value						*/
/*									*/
/* Output:								*/
/*	status value							*/

int wfikpch_hist(KPB *kpb, int tmo, int newipl) {

int	before, after;			/* ABSTIM values */
int	status;				/* Returned routine status */
DQ_UCB	*ucb;				/* Pointer to UCB */
extern	int	EXE$GL_ABSTIM;		/* Current time (seconds) */

    before = EXE$GL_ABSTIM;		/* Get the current time */
    status = ioc$kp_wfikpch(kpb,tmo,newipl);
    after  = EXE$GL_ABSTIM;		/* Get the current time */
    ucb	= (DQ_UCB *)kpb->kpb$ps_ucb;	/* Get UCB pointer */
    if ( (after-before) <= TIMEOUT_TIME )
        ucb->ucb$l_timeout[after-before]++; /* Bump histogram */
    else
        ucb->ucb$l_timeout[TIMEOUT_TIME+1]; /* Else inc overflow */
    return status;			/*  and return with status */
}
#endif

/* reset_ctrl - Reset the controller					*/
/*									*/
/* This routine issues a RESET to the controller.  It then waits for	*/
/* the BUSY bit to clear.  If the BUSY bit isn't cleared within a	*/
/* certain time, we decide that the controller is dead.			*/
/*									*/
/* Input:								*/
/*	ucb	pointer to UCB						*/
/*									*/
/* Output:								*/
/*	status value							*/
/*		SS$_NORMAL	successful reset			*/
/*		SS$_CTRLERR	controller failed to RESET		*/

int reset_ctrl(DQ_UCB *ucb)

{
int	status;				/* Routine status value */
int	dev_value;			/* DEV_CTL value to write */
BYTE	sts;				/* Status byte */
int	loop;				/* Loop counter */

/* Set and clear the RESET bit.  It's edge triggered */

    dev_value = CTL_M_MBO | CTL_M_SRST | CTL_M_nIEN; /* Reset + no ints */
    out(WT_DEV_CTL,dev_value,ucb);	/* Set the reset bit */
    dev_value = CTL_M_MBO;		/* Turn off RESET and ints on */
    out(WT_DEV_CTL,dev_value,ucb);	/* Set the reset bit */

/* Check to see if the drive is ready right now */

    sts = inp(RD_ALT_STS,ucb);		/* Get the status byte */
    if ( IS_CLEAR(sts,STS_M_BSY))	/* If not busy, then */
        return SS$_NORMAL;		/* Drive is ready - exit */

/* Drive is busy - wait a bit for it */

    for (loop=0 ; loop < RESET_TIME; loop++) {
        status	= exe$kp_fork_wait(ucb->ucb$ps_kpb,(FKB *) ucb);
        if ($FAIL(status))		/* Check the KP status */
            return status;		/* Failed - exit w/error */
        sts = inp(RD_ALT_STS,ucb);	/* Get the status byte */
        if ( IS_CLEAR(sts,STS_M_BSY))	/* If not busy, then */
            return SS$_NORMAL;		/* Drive is ready - exit */
     }

/* Controller is stuck on BUSY - return a fatal error for this */

    return SS$_CTRLERR;			/* Exit with controller error */
}

/* ISR - Interrupt Service Routine					*/
/*									*/
/*	This is the interrupt service routine for the driver.		*/
/*									*/
/* Usage:								*/
/*	ISR (idb)							*/
/*									*/
/* Input:								*/
/*	idb	pointer to IDB						*/
/*									*/
/* Output:								*/
/*	none								*/
/*									*/
/* Return value:							*/
/*	none								*/

void isr(IDB *idb)  {

DQ_UCB *ucb;				/* Pointer to the UCB */

/* Get pointer to the UCB;  If null, then there is none and we just exit */

    ucb = (DQ_UCB *) idb->idb$ps_owner;	/* Get UCB address from the IDB */
    if (ucb == NULL)
        return;				/* Unowned and unexpected - dismiss */
#ifdef DEBUG
    ucb->ucb$l_total_ints++;		/* Increment interrupt count */
#endif

/* There's an owner.  If the interrupt is expected, then restart the KP */

    device_lock(baseucb.ucb$l_dlck, NORAISE_IPL, NOSAVE_IPL);
					/* Acquire devicelock */

    if (baseucb.ucb$v_int) {		/* If this is an expected int... */
        baseucb.ucb$v_int = 0;		/* Clear "interrupt expected" */
        baseucb.ucb$v_tim = 0;		/* Clear TIMEOUT expected bit */
        exe$kp_restart(ucb->ucb$ps_kpb,SS$_NORMAL);
					/* ... then restart stalled KPB */
    }
#ifdef DEBUG
    else
        ucb->ucb$l_unsol_ints++;	/* Increment unsolicited interrupt count */
#endif

    device_unlock(baseucb.ucb$l_dlck,NOLOWER_IPL,SMP_RESTORE);
					/* Release the devicelock */
    return;				/* Return to interrupt dispatcher */
}

/* INP - This routine is used to read a byte from a CSR.	*/
/*								*/
/* Input:							*/
/*	reg	register index					*/
/*	ucb	pointer to the UCB				*/
/*								*/
/* Output:							*/
/*	none							*/
/*								*/
/* Return value:						*/
/*	byte of data read from the CSR				*/

BYTE inp(int reg,DQ_UCB *ucb) {

CRAM	*cram_ptr;			/* Pointer to CRAM */
int	status;				/* Routine status */
BYTE	data;				/* Data byte */

    cram_ptr = ucb->ucb$ps_crams[reg];	/* Point to the CRAM */
    status   = ioc$cram_io(cram_ptr);	/* Read the byte */
    data = (cram_ptr->cram$q_rdata >> cram_init[reg].shift) & 0xFF;
    return data;			/* Return the value */
}

/* INPW - This routine is used to read a word from a CSR.	*/
/*								*/
/* Input:							*/
/*	reg	register index					*/
/*	ucb	pointer to the UCB				*/
/*								*/
/* Output:							*/
/*	none							*/
/*								*/
/* Return value:						*/
/*	word of data read from the CSR				*/

WORD inpw(int reg,DQ_UCB *ucb) {

CRAM	*cram_ptr;			/* Pointer to the CRAM */
int	status;				/* Routine status value */
WORD	data;				/* Data value */

    cram_ptr = ucb->ucb$ps_crams[reg];	/* Point to CRAM */
    status   = ioc$cram_io(cram_ptr);	/* Read the word */
    data = cram_ptr->cram$q_rdata >> cram_init[reg].shift & 0xFFFF;
    return data;			/* Send back the data */
}

/* OUT - This routine is used to write a byte to a CSR.		*/
/*								*/
/* Input:							*/
/*	reg	register index					*/
/*	data	data byte to be written to the CSR		*/
/*	ucb	pointer to the UCB				*/
/*								*/
/* Output:							*/
/*	none							*/

void out(int reg,BYTE data,DQ_UCB *ucb) {

CRAM	*cram_ptr;			/* Pointer to the CRAM */
int	status;				/* Returned status */

    cram_ptr = ucb->ucb$ps_crams[reg];	/* Get correct CRAM */
    cram_ptr->cram$q_wdata = data << cram_init[reg].shift;/* Position data */
    status   = ioc$cram_io(cram_ptr);	/* Perform the write */
}

/* OUTW - This routine is used to write a word of data to a CSR.*/
/*								*/
/* Input:							*/
/*	reg	register index					*/
/*	data	data word to be written to the CSR		*/
/*	ucb	pointer to the UCB				*/
/*								*/
/* Output:							*/
/*	none							*/

void outw(int reg,WORD data,DQ_UCB *ucb) {

CRAM	*cram_ptr;			/* Pointer to CRAM */
int	status;				/* Routine status */

    cram_ptr = ucb->ucb$ps_crams[reg];	/* Point to the CRAM */
    cram_ptr->cram$q_wdata = data << cram_init[reg].shift; /* Position the data */
    status   = ioc$cram_io(cram_ptr);	/* Write the word */
}
