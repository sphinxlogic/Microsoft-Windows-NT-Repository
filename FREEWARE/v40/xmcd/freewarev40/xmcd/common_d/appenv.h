/*
 *   appenv - Common header file for xmcd, cda and libdi.
 *
 *	xmcd  - Motif(tm) CD Audio Player
 *	cda   - Command-line CD Audio Player
 *	libdi - CD Audio Player Device Interface Library
 *
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef __APPENV_H__
#define __APPENV_H__

#ifndef LINT
static char *_appenv_h_ident_ = "@(#)appenv.h	6.106 98/10/27";
#endif


/*
 * Whether STATIC should really be...
 */
#ifdef DEBUG
#define STATIC
#else
#define STATIC		static
#endif


/*
 * Basic constants
 */
#define STR_BUF_SZ	64			/* Generic string buf size */
#define ERR_BUF_SZ	512			/* Generic errmsg buf size */
#define FILE_PATH_SZ	256			/* Max file path length */
#define FILE_BASE_SZ	64			/* Max base name length */
#define MAXTRACK	100			/* Max number of tracks */
#define LEAD_OUT_TRACK	0xaa			/* Lead-out track number */
#define FRAME_PER_SEC	75			/* Frames per second */
#define MSF_OFFSET	150			/* Starting MSF offset */

#define CLIP_FRAMES	10			/* Frames to clip from end */
#define MAX_VOL		100			/* Max logical audio volume */


/*
 * Common header files to be included for all modules
 */

#ifndef __VMS
/* UNIX */

#include <sys/types.h>

#ifndef NO_STDLIB_H
#include <stdlib.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <memory.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

#ifdef BSDCOMPAT
#include <strings.h>
#else
#include <string.h>
#endif

#define PIPE		pipe
#define LINK		link
#define UNLINK		unlink
#define OPENDIR		opendir
#define READDIR		readdir
#define CLOSEDIR	closedir

#else
/* OpenVMS */

#define NOMKTMPDIR		/* Do not create TEMP_DIR */
#define CADDR_T		1	/* To avoid redundant typedefs */

#define _WSTOPPED	0177	/* Bit set if stopped */
#define _W_INT(i)	(i)
#define _WSTATUS(x)	(_W_INT(x) & _WSTOPPED)

#define WIFSIGNALED(x)	(_WSTATUS(x) != _WSTOPPED && _WSTATUS(x) != 0)
#define WTERMSIG(x)	(WIFSIGNALED(x) ? _WSTATUS(x) : -1)
#define WIFEXITED(x)	(_WSTATUS(x) == 0)
#define WEXITSTATUS(x)	(WIFEXITED(x) ? ((_W_INT(x) >> 8) & 0377) : -1)

#include <types.h>
#include <stat.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <utsname.h>
#include <descrip.h>
#include <dvidef.h>
#include <lnmdef.h>
#include <processes.h>

#ifndef __DEV_T
typedef unsigned int	uint_t;
typedef uint_t		uid_t;
typedef uint_t		gid_t;
typedef int		pid_t;
typedef uint_t		mode_t;
#endif

#ifndef S_IXUSR
#define S_IXUSR		0000100
#endif
#ifndef S_IXGRP
#define S_IXGRP		0000010
#endif
#ifndef S_IXOTH
#define S_IXOTH		0000001
#endif
#ifndef S_IRUSR
#define S_IRUSR		0000400
#endif

/* Prototype some external library functions */
extern uid_t		getuid(void);
extern gid_t		getgid(void);
extern int		setuid(uid_t);
extern int		setgid(gid_t);
extern void		*malloc(int);
extern void		free(void *);
extern void		exit(int);
extern char		*getenv(char *);

#define PIPE(fds)	pipe(fds, O_RDWR, 1024)
#define LINK		util_link
#define UNLINK		util_unlink

#ifdef VMS_USE_OWN_DIRENT

#define OPENDIR		util_opendir
#define READDIR		util_readdir
#define CLOSEDIR	util_closedir

struct dirent {
	char		d_name[FILE_PATH_SZ];
};

typedef struct {
	struct dirent	*dd_buf;
} DIR;

#else

#include <dirent.h>

#define OPENDIR		opendir
#define READDIR		readdir
#define CLOSEDIR	closedir

#endif	/* VMS_USE_OWN_DIRENT */

#endif	/* __VMS */


/*
 * Define these just in case the OS does not support the POSIX definitions
 */
#ifndef S_ISBLK
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#endif
#ifndef S_ISCHR
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#endif
#ifndef S_ISDIR
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif
#ifndef SIGCHLD
#define SIGCHLD		SIGCLD
#endif


/*
 * Data type definitions: for portability
 */
typedef unsigned char	byte_t;			/* 8-bit unsigned */
typedef char		sbyte_t;		/* 8-bit signed */
typedef unsigned short	word16_t;		/* 16-bit unsigned */
typedef short		sword16_t;		/* 16-bit signed */
typedef unsigned int	word32_t;		/* 32-bit unsigned */
typedef int		sword32_t;		/* 32-bit signed */
typedef char		bool_t;			/* Boolean */


/*
 * Endianess
 */
#define _L_ENDIAN_	1234
#define _B_ENDIAN_	4321

#if defined(i286) || defined(__i286__) || \
    defined(i386) || defined(__i386__) || \
    defined(i486) || defined(__i486__) || \
    defined(__alpha) || \
    defined(vax) || \
    (defined(mips) && defined(MIPSEL))
#define _BYTE_ORDER_	_L_ENDIAN_
#else
#define _BYTE_ORDER_	_B_ENDIAN_
#endif


/*
 * Platform-specific stuff
 */
#if defined(macII) || defined(sony_news)

#ifndef WEXITSTATUS
#define WEXITSTATUS(x)	(((x.w_status) >> 8) & 0377)
#endif
#ifndef WTERMSIG
#define WTERMSIG(x)	((x.w_status) & 0177)
#endif

typedef union wait	waitret_t;

#else	/* macII */

typedef int		waitret_t;

#endif	/* macII */


#ifdef __VMS
#define WAITPID			util_waitpid
#else
#ifdef sony_news
typedef int			pid_t;
typedef int			mode_t;
#define WAITPID(pid, svp, op)	wait4(pid, svp, 0, 0)
#else
#define WAITPID			waitpid
#endif	/* sony_news */
#endif	/* __VMS */


#if defined(macII) || defined(sony_news)
#define MKFIFO(path, mode)	mknod((path), S_IFIFO | (mode), 0)
#else
#define MKFIFO(path, mode)	mkfifo((path), (mode))
#endif


#ifdef __VMS
#define FORK		vfork
#else
#define FORK		fork
#endif


#ifdef S_IFLNK
#define LSTAT		lstat
#else
#define LSTAT		stat
#endif


#ifdef BSDCOMPAT
#ifndef strchr
#define strchr		index
#endif
#ifndef strrchr
#define strrchr		rindex
#endif
#endif	/* BSDCOMPAT */


/*
 * Errno compatibility
 */
#if !defined(ETIME) && defined(ETIMEDOUT)
#define ETIME		ETIMEDOUT
#endif


/*
 * Boolean flags
 */
#ifndef FALSE
#define FALSE		0
#endif
#ifndef TRUE
#define TRUE		1
#endif


/* Memory allocators */
#define _MEM_ALLOC		malloc
#define _MEM_REALLOC		realloc
#define _MEM_CALLOC		calloc
#define _MEM_FREE		free

#ifdef MEM_DEBUG
#define MEM_ALLOC(s,l)		util_dbg_malloc((s),(l))
#define MEM_REALLOC(s,p,l)	util_dbg_realloc((s),(p),(l))
#define MEM_CALLOC(s,n,l)	util_dbg_calloc((s),(n),(l))
#define MEM_FREE(p)		util_dbg_free((p))
#else
#define MEM_ALLOC(s,l)		_MEM_ALLOC((l))
#define MEM_REALLOC(s,p,l)	_MEM_REALLOC((p),(l))
#define MEM_CALLOC(s,n,l)	_MEM_CALLOC((n),(l))
#define MEM_FREE(p)		_MEM_FREE((p))
#endif


/*
 * File path definitions
 */
#ifndef __VMS
#define TEMP_DIR	"/tmp/.cdaudio"		/* Temporary directory */
#define SYS_CMCFG_PATH	"%s/config/common.cfg"	/* System common cfg file */
#define USR_CMCFG_PATH	"%s/.xmcdcfg/common.cfg"/* User common cfg file */
#define SYS_DSCFG_PATH	"%s/config/%s"		/* System devspec cfg file */
#define USR_DSCFG_PATH	"%s/.xmcdcfg/%s"	/* User devspec cfg file */
#define USR_VINIT_PATH	"%s/.xmcdcfg/%s"	/* User version init file */
#define REL_DBDIR_PATH	"%s/cddb/%s"		/* CDDB rel dir path */
#define CDDBFILE_PATH	"%s/%08x"		/* CDDB file path */
#define HELPFILE_PATH	"%s/help/%s"		/* Help file path */
#define DOCFILE_PATH	"%s/docs/"		/* Documentation file path */
#define CONCAT_PATH	"%s/%s"			/* Concatenation of path */
#else
#define TEMP_DIR	"SYS$LOGIN:TMP.DIR"	/* Temporary directory */
#define SYS_CMCFG_PATH	"%s]common.cfg"		/* System common cfg file */
#define USR_CMCFG_PATH	"%s.xmcdcfg]common.cfg"	/* User common cfg file */
#define SYS_DSCFG_PATH	"%s]device.cfg"		/* System devspec cfg file */
#define USR_DSCFG_PATH	"%s.xmcdcfg]device.cfg"	/* User devspec cfg file */
#define USR_VINIT_PATH	"%s.xmcdcfg]%s"		/* User version init file */
#define REL_DBDIR_PATH	"%s.cddb.%s]"		/* CDDB rel dir path */
#define CDDBFILE_PATH	"%s%08x."		/* CDDB file path */
#define HELPFILE_PATH	"%s.help]%s"		/* Help file path */
#define DOCFILE_PATH	"%s.docs]"		/* Documentation file path */
#define CONCAT_PATH	"%s%s"			/* Concatenation of path */
#endif


/* 
 * Web search engine information 
 */
typedef struct srcheng_ent {
	char		*name;			/* Search engine name */
	char		*action;		/* Action URL+args */
	void		*aux;			/* Auxiliary pointer */
	int		acnt;			/* Number of %A s */
	int		dcnt;			/* Number of %D s */
	int		tcnt;			/* Number of %T s */
	struct srcheng_ent *next;		/* Link to next entry */
} srcheng_ent_t;


/*
 * Defines for the type field in trkinfo_t
 */
#define TYP_AUDIO	1			/* Audio track */
#define TYP_DATA	2			/* Data track */


/*
 * Defines for the mode field in curstat_t
 */
#define MOD_NODISC	0			/* No disc loaded */
#define MOD_PLAY	1			/* Play mode */
#define MOD_PAUSE	2			/* Pause mode */
#define MOD_STOP	3			/* Stop mode */
#define MOD_A		4			/* First half of A->B mode */
#define MOD_AB		5			/* Running A->B mode */
#define MOD_SAMPLE	6			/* Sample mode */
#define MOD_BUSY	7			/* Drive is busy */


/*
 * Defines for the flags field in curstat_t
 */
#define STAT_SAVE	0x0001			/* Save CDDB before clear */
#define STAT_CLEAR	0x0002			/* Clear CDDB after save */
#define STAT_EJECT	0x0004			/* Eject CD after clear */
#define STAT_EXIT	0x0008			/* Exit program after clear */
#define STAT_CHGDISC	0x0010			/* Change disc after clear */


/*
 * Defines for the time_dpy field in curstat_t
 */
#define TIMEDPY_MAX_MODES	4		/* Max number of modes */
#define T_ELAPSED		0		/* Per-track elapsed time */
#define T_ELAPSED_DISC		1		/* Whole-disc elapsed time */
#define T_REMAIN_TRACK		2		/* Per-track remaining time */
#define T_REMAIN_DISC		3		/* Whole-disc remaining time */


/*
 * CD per-track information
 */
typedef struct {
	sword32_t	trkno;			/* Track number */
	word32_t	addr;			/* Absolute offset block */
	byte_t		min;			/* Absolute offset minutes */
	byte_t		sec;			/* Absolute offset seconds */
	byte_t		frame;			/* Absolute offset frame */
	byte_t		type;			/* track type */
} trkinfo_t;


/*
 * Structure containing current status information
 */
typedef struct {
	char		*curdev;		/* Current device */
	byte_t		mode;			/* Player mode */
	byte_t		time_dpy;		/* Time display mode */
	word16_t	flags;			/* State flags */
	sword32_t	first_disc;		/* First disc */
	sword32_t	last_disc;		/* Last disc */
	sword32_t	cur_disc;		/* Current disc */
	sword32_t	prev_disc;		/* Previous disc */
	sword32_t	first_trk;		/* First track */
	sword32_t	last_trk;		/* Last track */
	byte_t		tot_trks;		/* Total number of tracks */
	byte_t		tot_min;		/* Total minutes */
	byte_t		tot_sec;		/* Total seconds */
	byte_t		tot_frame;		/* Total frame */
	word32_t	tot_addr;		/* Total block length */
	sword32_t	cur_trk;		/* Current track */
	sword32_t	cur_idx;		/* Current index */
	byte_t		cur_tot_min;		/* Current absolute minutes */
	byte_t		cur_tot_sec;		/* Current absolute seconds */
	byte_t		cur_tot_frame;		/* Current absolute frame */
	byte_t		cur_trk_min;		/* Current relative minutes */
	byte_t		cur_trk_sec;		/* Current relative seconds */
	byte_t		cur_trk_frame;		/* Current relative frame */
	byte_t		reserved2[2];		/* reserved */
	word32_t	cur_tot_addr;		/* Current absolute block */
	word32_t	cur_trk_addr;		/* Current relative block */
	word32_t	sav_iaddr;		/* Saved index abs block */
	trkinfo_t	trkinfo[MAXTRACK];	/* Per-track information */
	sword32_t	playorder[MAXTRACK];	/* Prog/Shuf sequence */
	word32_t	rptcnt;			/* Repeat iteration count */
	bool_t		devlocked;		/* Device locked */
	bool_t		repeat;			/* Repeat mode */
	bool_t		shuffle;		/* Shuffle mode */
	bool_t		program;		/* Program mode */
	bool_t		onetrk_prog;		/* Trk list 1-track program */
	bool_t		caddy_lock;		/* Caddy lock */
	byte_t		cddb;			/* CD Database entry type */
	byte_t		prog_tot;		/* Prog/Shuf total tracks */
	byte_t		prog_cnt;		/* Prog/Shuf track counter */
	byte_t		level;			/* Current volume level */
	byte_t		level_left;		/* Left channel vol percent */
	byte_t		level_right;		/* Right channel vol percent */
	char		vendor[9];		/* CD-ROM drive vendor */
	char		prod[17];		/* CD-ROM drive model */
	char		revnum[5];		/* CD-ROM firmware revision */
} curstat_t;


/*
 * Default remote CDDB port number
 */
#define CDDB_PORT		888

/*
 * Default network timeout interval
 */
#define CDDB_TIMEOUT		60


/*
 * Default Send CDDB site and command
 */
#define CDDB_MAILSITE		"cddb-submit@submit.cddb.com"
#define CDDB_MAILCMD		"mailx -s '%S' %A < %F"

/*
 * Default message strings
 */
#define MAIN_TITLE		"CD Audio Player"
#define STR_CDDBLOCAL		"loc-db"
#define STR_CDDBREMOTE		"rmt-db"
#define STR_CDDBQUERY		"query"
#define STR_PROGMODE		"prog"
#define STR_ELAPSE		"elapse"
#define STR_ELAPSE_DISC		"e-disc"
#define STR_REMAIN_TRK		"r-trac"
#define STR_REMAIN_DISC		"r-disc"
#define STR_PLAY		"play"
#define STR_PAUSE		"pause"
#define STR_READY		"ready"
#define STR_SAMPLE		"samp"
#define STR_USAGE		"Usage:"
#define STR_BADOPTS		"The following options are unrecognized:"
#define STR_NODISC		"no disc"
#define STR_BUSY		"cd busy"
#define STR_UNKNDISC		"unknown disc title"
#define STR_UNKNTRK		"unknown track title"
#define STR_DATA		"data"
#define STR_WARNING		"Warning"
#define STR_FATAL		"Fatal Error"
#define STR_CONFIRM		"Confirm"
#define STR_INFO		"Information"
#define STR_ABOUT		"About"
#define STR_QUIT		"Really Quit?"
#define STR_NOMEMORY		"Out of memory!"
#define STR_TMPDIRERR		"Cannot create or access directory %s!"
#define STR_LIBDIRERR		\
"Neither the XMcd.libdir resource nor the XMCD_LIBDIR environment is defined!"
#define STR_LONGPATHERR		"Path or message too long."
#define STR_NOMETHOD		"Unsupported deviceInterfaceMethod parameter!"
#define STR_NOVU		"Unsupported driveVendorCode parameter!"
#define STR_NOHELP		"The help file on this topic is not installed!"
#define STR_NOLINK		"There is no likely CDDB entry to link to."
#define STR_NODB		"No CD database directory."
#define STR_NOCFG		"Cannot open configuration file \"%s\"."
#define STR_NOTROM		"Device is not a CD-ROM!"
#define STR_NOTSCSI2		"Device is not SCSI-II compliant."
#define STR_SEND		"Mail current CDDB entry to \"%s\"?"
#define STR_MAILERR		"Mail command failed."
#define STR_MODERR		"Binary permissions error."
#define STR_STATERR		"Cannot stat device \"%s\"."
#define STR_NODERR		"\"%s\" is not the appropriate device type!"
#define STR_DBINCMPLERR		"Cannot send: Incomplete CD database entry."
#define STR_SEQFMTERR		"Program sequence string format error."
#define STR_INVPGMTRK		"Invalid track(s) deleted from program."
#define STR_RECOVERR		"Recovering from audio playback error..."
#define STR_MAXERR		"Too many errors."
#define STR_SAVERR_FORK		"File not saved:\nCannot fork. (errno %d)"
#define STR_SAVERR_SUID		"File not saved:\nCannot setuid to %d."
#define STR_SAVERR_OPEN		"File not saved:\nCannot open file for writing."
#define STR_SAVERR_CLOSE	"File not saved:\nCannot save file."
#define STR_SAVERR_WRITE	"File save error!"
#define STR_SAVERR_KILLED	"File not saved:\nChild killed. (signal %d)"
#define STR_LNKERR_FORK		"File not linked:\nCannot fork. (errno %d)"
#define STR_LNKERR_SUID		"File not linked:\nCannot setuid to %d."
#define STR_LNKERR_LINK		"File not linked:\nLink failed."
#define STR_LNKERR_KILLED	"File not linked:\nChild killed. (signal %d)"
#define STR_SRCHDB		\
"Searching CD database for potential links...\n\
This may take some time."
#define STR_RMTMATCH		\
"The CD database server cannot find an exact\n\
match for this CD, but has located a possible\n\
entry:\n\n\
Category: %s\n\
Title: %s\n\n\
Is this the correct CD?"
#define STR_CHGSAVE		\
"%s\n\n\
The on-screen CD database information\n\
has changed and was not saved.\n\
Do you want to save it now?"
#define STR_DEVLIST_UNDEF	"The deviceList parameter is undefined."
#define STR_DEVLIST_COUNT	\
"The number of devices in the deviceList parameter is incorrect."
#define STR_MEDCHG_NOINIT	\
"Cannot initialize medium changer device:\n\
Running as a single-disc player"
#define STR_PROXYERR		"HTTP proxy server configuration error."
#define STR_AUTHFAIL		\
"Proxy authorization failed.\n\
Do you want to re-enter your user name and password?"
#define STR_BROWSER_START	"Starting web browser..."
#define STR_BROWSER_FAIL	\
"Cannot access web browser.\n\
Check your configuration."
#ifdef __VMS
#define STR_APPDEF		\
"The XMCD.DAT resource file cannot be located, or is the wrong\n\
version.  A correct version of this file must be present in the\n\
appropriate directory in order for xmcd to run.  Please check\n\
your xmcd installation."
#else
#define STR_APPDEF		\
"The app-defaults/XMcd file cannot be located, or is the wrong\n\
version.  A correct version of this file must be present in the\n\
appropriate directory in order for xmcd to run.  Please check\n\
your xmcd installation."
#endif
#define STR_KPMODEDSBL		\
"The %s mode is enabled.\n\
Using the keypad will disable it.\n\
Do you want to proceed?"

/*
 * Application resource/option data
 */
typedef struct {
	char		*libdir;		/* Library path */

	/* X resources */
	char		*version;		/* app-defaults file version */
	int		main_mode;		/* Default main window mode */
	int		modechg_grav;		/* Which corner is fixed */
	int		normal_width;		/* Normal mode width */
	int		normal_height;		/* Normal mode height */
	int		basic_width;		/* Basic mode width */
	int		basic_height;		/* Basic mode height */
	int		blinkon_interval;	/* Display blink on (ms) */
	int		blinkoff_interval;	/* Display blink off (ms) */
	bool_t		main_showfocus;		/* Highlight kbd focus? */

	/*
	 * Common config parameters
	 */
	char		*device;		/* Default CD-ROM device */
	char		*cddb_path;		/* CDDB paths */
	char		*cddb_filemode;		/* CDDB file permissions */
	char		*cddb_mailsite;		/* Send CDDB mail site */
	char		*cddb_mailcmd;		/* Send CDDB mail command */
	char		*proxy_server;		/* http proxy host:port */
	char		*browser_dir;		/* Web browser direct cmd */
	char		*browser_rmt;		/* Web browser remote cmd */
	char		*site1_url;		/* Web site 1 URL */
	char		*site2_url;		/* Web site 2 URL */
	char		*site3_url;		/* Web site 3 URL */
	char		*exclude_words;		/* Keywords to exclude */
	int		cddb_port;		/* Remote CDDB default port */
	int		cddb_timeout;		/* Remote CDDB timeout (secs) */
	int		stat_interval;		/* Status poll interval (ms) */
	int		ins_interval;		/* Insert poll interval (ms) */
	int		prev_threshold;		/* Previous track/index
						 * threshold (blocks)
						 */
	int		sample_blks;		/* Sample play blocks */
	int		timedpy_mode;		/* Default time display mode */
	int		tooltip_delay;		/* Tool-tip delay interval */
	int		tooltip_time;		/* Tool-tip active interval */
	bool_t		ins_disable;		/* Insert poll disable */
	bool_t		cddb_rmtdsbl;		/* Remote query disable */
	bool_t		cddb_rmtautosave;	/* CDDB remote auto-save */
	bool_t		http_proxy;		/* Using http proxy server */
	bool_t		scsierr_msg;		/* Print SCSI error msg? */
	bool_t		sol2_volmgt;		/* Solaris 2.x Vol Mgr */
	bool_t		write_curfile;		/* Enable curr.XXX output */
	bool_t		tooltip_enable;		/* Enable tool-tips */
	bool_t		debug;			/* Verbose debug output */

	/*
	 * Device-specific config parameters
	 */

	/* Privileged */
	int		devnum;			/* Logical device number */
	char		*devlist;		/* CD-ROM device list */
	int		di_method;		/* Device interface method */
	int		vendor_code;		/* Vendor command set code */
	int		numdiscs;		/* Number of discs */
	int		chg_method;		/* Medium change method */
	int		base_scsivol;		/* SCSI volume value base */
	int		min_playblks;		/* Minimum play blocks */
	bool_t		play10_supp;		/* Play Audio (10) supported */
	bool_t		play12_supp;		/* Play Audio (12) supported */
	bool_t		playmsf_supp;		/* Play Audio MSF supported */
	bool_t		playti_supp;		/* Play Audio T/I supported */
	bool_t		load_supp;		/* Motorized load supported */
	bool_t		eject_supp;		/* Motorized eject supported */
	bool_t		msen_dbd;		/* Set DBD bit for msense */
	bool_t		mselvol_supp;		/* Audio vol chg supported */
	bool_t		balance_supp;		/* Indep vol chg supported */
	bool_t		chroute_supp;		/* Channel routing support */
	bool_t		pause_supp;		/* Pause/Resume supported */
	bool_t		caddylock_supp;		/* Caddy lock supported */
	bool_t		curpos_fmt;		/* Fmt 1 of RdSubch command */
	bool_t		play_notur;		/* No Tst U Rdy when playing */

	/* User-modifiable */
	int		vol_taper;		/* Volume control taper */
	int		startup_vol;		/* Startup volume preset */
	int		ch_route;		/* Channel routing */
	int		skip_blks;		/* FF/REW skip blocks */
	int		skip_pause;		/* FF/REW pause (msec) */
	int		skip_spdup;		/* FF/REW speedup count */
	int		skip_vol;		/* FF/REW percent volume */
	int		skip_minvol;		/* FF/REW minimum volume */
	bool_t		load_spindown;		/* Spin down disc on CD load */
	bool_t		load_play;		/* Auto play on CD load */
	bool_t		done_eject;		/* Auto eject on done */
	bool_t		exit_eject;		/* Eject disc on exit? */
	bool_t		exit_stop;		/* Stop disc on exit? */
	bool_t		eject_exit;		/* Exit upon disc eject? */
	bool_t		eject_close;		/* Close upon disc eject? */
	bool_t		caddy_lock;		/* Lock caddy on CD load? */
	bool_t		repeat_mode;		/* Repeat enable on startup */
	bool_t		shuffle_mode;		/* Shuffle enable on startup */
	bool_t		multi_play;		/* Multi-CD playback */
	bool_t		reverse;		/* Multi-CD reverse playback */
	bool_t		internal_spkr;		/* Enable built-in speakers */

	/*
	 * Various application message strings
	 */
	char		*main_title;		/* Main window title */
	char		*str_cddblocal;		/* loc-db */
	char		*str_cddbremote;	/* rmt-db */
	char		*str_cddbquery;		/* query */
	char		*str_progmode;		/* prog */
	char		*str_elapse;		/* elapse */
	char		*str_elapsedisc;	/* e-disc */
	char		*str_remaintrk;		/* r-trac */
	char		*str_remaindisc;	/* r-disc */
	char		*str_play;		/* play */
	char		*str_pause;		/* pause */
	char		*str_ready;		/* ready */
	char		*str_sample;		/* sample */
	char		*str_usage;		/* Usage */
	char		*str_badopts;		/* Bad command-line options */
	char		*str_nodisc;		/* No disc */
	char		*str_busy;		/* Device busy */
	char		*str_unkndisc;		/* unknown disc title */
	char		*str_unkntrk;		/* unknown track title */
	char		*str_data;		/* Data */
	char		*str_warning;		/* Warning */
	char		*str_fatal;		/* Fatal error */
	char		*str_confirm;		/* Confirm */
	char		*str_info;		/* Information */
	char		*str_about;		/* About */
	char		*str_quit;		/* Really Quit? */
	char		*str_nomemory;		/* Out of memory */
	char		*str_tmpdirerr;		/* tempdir problem */
	char		*str_libdirerr;		/* libdir not defined */
	char		*str_longpatherr;	/* Path or message too long */
	char		*str_nomethod;		/* Invalid di_method */
	char		*str_novu;		/* Invalid vendor code */
	char		*str_nohelp;		/* No help available on item */
	char		*str_nolink;		/* No likely CDDB link */
	char		*str_nodb;		/* No CDDB directory */
	char		*str_nocfg;		/* Can't open config file */
	char		*str_notrom;		/* Not a CD-ROM device */
	char		*str_notscsi2;		/* Not SCSI-II compliant */
	char		*str_send;		/* Send CDDB confirm msg */
	char		*str_mailerr;		/* Mail command failed */
	char		*str_moderr;		/* Binary perms error */
	char		*str_staterr;		/* Can't stat device */
	char		*str_noderr;		/* Not a character device */
	char		*str_dbincmplerr;	/* Incomplete CDDB entry err */
	char		*str_seqfmterr;		/* Pgm sequence format err */
	char		*str_invpgmtrk;		/* Inv program trk deleted */
	char		*str_recoverr;		/* Recovering audio play err */
	char		*str_maxerr;		/* Too many errors */
	char		*str_saverr_fork;	/* File save err: fork */
	char		*str_saverr_suid;	/* File save err: setuid */
	char		*str_saverr_open;	/* File save err: open */
	char		*str_saverr_close;	/* File save err: close */
	char		*str_saverr_write;	/* File save err: write */
	char		*str_saverr_killed;	/* File save err: child kill */
	char		*str_lnkerr_fork;	/* File link err: fork */
	char		*str_lnkerr_suid;	/* File link err: setuid */
	char		*str_lnkerr_link;	/* File link err: link */
	char		*str_lnkerr_killed;	/* File link err: child kill */
	char		*str_srchdb;		/* Link search dialog msg */
	char		*str_rmtmatch;		/* CDDB match dialog msg */
	char		*str_chgsave;		/* Save change dialog msg */
	char		*str_devlist_undef;	/* deviceList no defined */
	char		*str_devlist_count;	/* deviceList count incorrect */
	char		*str_medchg_noinit;	/* Cannot init medium changer */
	char		*str_proxyerr;		/* Proxy server config error */
	char		*str_authfail;		/* Proxy auth failure */
	char		*str_browser_start;	/* Starting web browser */
	char		*str_browser_fail;	/* Cannot start browser */
	char		*str_appdef;		/* app-defaults file error */
	char		*str_kpmodedsbl;	/* Keypad mode dsbl prompt */

	/*
	 * Short-cut key definitions
	 */
	char		*mode_key;		/* Basic/Normal mode */
	char		*lock_key;		/* Lock */
	char		*repeat_key;		/* Repeat */
	char		*shuffle_key;		/* Shuffle */
	char		*eject_key;		/* Eject */
	char		*quit_key;		/* Quit */
	char		*dbprog_key;		/* Database/Program popup */
	char		*help_key;		/* Help popup */
	char		*options_key;		/* Options */
	char		*time_key;		/* Time */
	char		*ab_key;		/* A->B */
	char		*sample_key;		/* Sample */
	char		*keypad_key;		/* Keypad popup */
	char		*playpause_key;		/* Play/Pause */
	char		*stop_key;		/* Stop */
	char		*prevdisc_key;		/* Prev Disc */
	char		*nextdisc_key;		/* Next Disc */
	char		*prevtrk_key;		/* Prev track */
	char		*nexttrk_key;		/* Next track */
	char		*previdx_key;		/* Prev index */
	char		*nextidx_key;		/* Next index */
	char		*rew_key;		/* REW */
	char		*ff_key;		/* FF */
	char		*keypad0_key;		/* Keypad 0 */
	char		*keypad1_key;		/* Keypad 1 */
	char		*keypad2_key;		/* Keypad 2 */
	char		*keypad3_key;		/* Keypad 3 */
	char		*keypad4_key;		/* Keypad 4 */
	char		*keypad5_key;		/* Keypad 5 */
	char		*keypad6_key;		/* Keypad 6 */
	char		*keypad7_key;		/* Keypad 7 */
	char		*keypad8_key;		/* Keypad 8 */
	char		*keypad9_key;		/* Keypad 9 */
	char		*keypadclear_key;	/* Keypad Clear */
	char		*keypadenter_key;	/* Keypad Enter */
	char		*keypadcancel_key;	/* Keypad Cancel */
} appdata_t;


#endif	/* __APPENV_H__ */

