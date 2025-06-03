/* @(#)tslip.h	1.4 (13 Oct 1993) */
/*
 *            Copyright 1991, Intel Corporation
 *                  All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and
 * that both the copyright notice appear in all copies and that both
 * the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior premission.
 * 
 * COMPANY AND/OR INTEL DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO
 * EVENT SHALL COMPANY NOR INTEL BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

/*
 * Header file for Intel Unix System V/386 Release 4.0 SLIP streams driver.
 */	
/* TSLIP modifications by Jim Avera (jima@netcom.com) */

#define BUFOFFSET	128		/* space required at START of buf */

/* The following defines is taken from RFC 1005 */

#define SLIPMTU	1500			/* maximum slip packet size */

#define	END	0300			/* frame end character */
#define	ESC	0333			/* frame escape character */
#define	ESC_END	0334			/* transposed frame end */
#define	ESC_ESC	0335			/* transposed frame esc */

#include <sys/tslip_comp.h>

typedef struct tslip tsl_t;

struct tslip {
	minor_t	minor;				/* minor number of this dev. */
	tsl_t	*next;				/* minor lookup linked list */

	u_int	state;				/* state of the entry */
	u_int	sap;				/* service access point */
	queue_t	*qtop;				/* upper streams read queue */
	queue_t	*qbot;				/* lower streams write queue */
	u_int	qt_blocked;			/* blocked upper write service flag */
	pid_t	pid;				/* PID of opening process */
	u_char	buf[SLIPMTU+BUFOFFSET];		/* incoming packet buffer */
	u_char	*bptr;				/* pointer to packet start */
	int	inlen;				/* length of captured data */
	short	escape;				/* flag if an ESC is detected */
	short	overrun;			/* flag if incoming data exceeds SLIPMTU */
	short	flags;				/* flag to be set (read/write) by user */
	char	statsnamebuf[IFNAMSIZ];	/* (pointed to from inside .stats) */
	u_char	sc_flags;		/* slip compression flags */
	u_char	hangup_pend;		/* hangup not acknowledged by daemon */
	struct	ifstats	stats;		/* slip interface statistics */
 	struct	tslcompress slcomp;	/* tcp compression data */
	int	xmit_hiband;		/* # of hiband xmissions */
	int	recv_hiband;		/* and receptions */
	int	uw_blocked;		/* # of uwsrv blockages */
	int	lindex;			/* link index of driver linked below */	
	int	notif_toid;		/* daemon notification timeout id */
	time_t	idletime;		/* notify daemon after ~ idle seconds */
	time_t	atime;			/* approx last access (for idle t.o.) */
	u_char	idleflag;		/* timeout intr. occurred since use */
	u_char	enab_to_notif;		/* ok to send daemon notification? */
	char	origifn[IFNAMSIZ];	/* original slip ifc name ("sl00") --
					 * N.B. leading zeroes are lost in the
					 * in uname + stats.ifs_unit combo. */
	u_char	*autodialp;		/* auto-dial parameter string */
};

/* the following are definitions for slip special I_STR ioctl */

/*#define SLP_REG_HUP	1	/* obsolete (for old slhangupd) */
/*#define SLP_UNREG_HUP	2	/* obsolete (for old slhangupd) */
#define SLP_DO_COMPRESS	3	/* enable compression */
#define SLP_DO_AUTOCOMP	4	/* enable auto comp detection */
#define SLP_SET_MTU	5	/* set maximum transfer unit */
#define SLP_REG_SLIPD	6	/* register daemon */
#define SLP_UNREG_SLIPD	7	/* unregister daemon */
#define SLP_SET_IDLETO  8	/* set idle timeout (data=time_t seconds)*/
#define SLP_SET_CONNP	9	/* set auto-connect params (data=string) */
#define SLP_MOVE_LOWER	10	/* relocate a lower dev (data=minor_t target) */
#define SLP_CONNFAIL	11	/* connection request failed (data=minor_t #) */
#define SLP_GETINFO	12	/* return tsl_info_t */

/* masks for sc_flags */
#define	SC_COMPRESS	0x02	/* compress TCP traffic */
#define	SC_NOICMP	0x04	/* supress ICMP traffic */
#define	SC_AUTOCOMP	0x08	/* auto-enable TCP compression */

/* 
 * Control message structure for messages sent to the daemon.
 * The associated data message contains the null-terminated
 * auto-dial parameter string.
 */
typedef struct {
	int	op;			/* purpose of this message */
	pid_t	pid;			/* process id of application */
	time_t	atime;			/* approx time of most recent xfer */
	dev_t	minor;			/* slip upper stream internal unit# */
	dev_t	lindex;			/* driver link index */
	u_char	origifn[IFNAMSIZ];	/* original slip ifc name ("sl00") */
} tslipd_t;

/* tslipd_t op values */
#define SLPD_CONNREQ	1		/* Request for auto-dial */
#define SLPD_TIMEOUT	2		/* Idle timeout occurred */
#define SLPD_HANGUP	3		/* HANGUP received */

/*
 * State message returned by GETINFO
 */
typedef struct {
	u_char	origifn[IFNAMSIZ];	/* original slip ifc name ("sl00") */
	int	lindex;			/* link index of serial driver */	
} tsl_info_t;

#define NULL_LINDEX (-1)		/* lindex value when nothing is there */
