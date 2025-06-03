static char sccsid[] = "@(#)tslip.c	1.5 (13 Oct 1993)";

/*..........................................................................
 . Copyright 1993, Jim Avera.  All Rights Reserved.
 . 
 . This program contains new material owned by the above copyright holder,
 . and may also contain portions derived from existing works used according
 . to permission granted by the owners of those works.
 .
 . You are prohibited from copying, distributing, modifying, or using this
 . file (or the portions owned by the above copyright holder) except as 
 . described in the file "COPYRIGHT" which accompanies this program.
 ..........................................................................*/

/*
 * Copyright 1992 Petcom Industries Limited.  All rights reserved.
 *
 * As far as I'm concerned, you can do anything at all you want with
 * this code.  You must, however, retain this and all other copyright
 * notices in the source code, and abide by this and all other copyright
 * notices.  In particular, you are forbidden to market this code, or
 * derived code or binaries.
 */

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

/* THE APPARENT HISTORY OF THIS DRIVER:
 *  
 * Original (non-compressed) slip (and utilities)
 *     Intel Corp. by Sudji Husodo <sudji@indo.intel.com> 1/9/91
 *
 * VJ compression added by
 *     Petcom Industries Ltd.  rev 1.3.1.1 of 92/05/22.
 *
 * Auto-dial support (using Taylor UUCP) added by
 *     Jim Avera.   Packaged with the tslip-1.0 distribution of 6/23/93.
 */

/*
 * Petcom change log:
 *
 * $Header: /u3/src/slip/driver/io/RCS/slp.c,v 1.4.1.1 92/05/22 09:26:03 root rel $
 * $Author: root $
 *
 * $Log:	slp.c,v $
 * Revision 1.4.1.1  92/05/22  09:26:03  root
 * Changes:
 * 	- removed all the STRLOG statements - they were beginning to bug
 * 	  me.
 * 	- renamed the SL_DOCOMPRESS and SL_DOAUTOCOMP ioctls.
 * 	- added a new SLP_SET_MTU ioctl
 * 	- it is now possible to use ifconfig to modify compression
 * 	  parameters, instead of having to modify code to add
 * 	  SLP_DO_COMPRESS/SLP_DO_AUTOCOMP ioctls.  ifconfig sl0 -trailers
 * 	  will enable compression, and ifconfig sl0 -arp enables
 * 	  auto compression detection.
 * This is official patch 1 to slp 1.4.
 * 
 * Revision 1.4  92/05/08  12:02:31  root
 * Compression seems to be functioning well now.  Increased the
 * hi and lo water marks to attempt to get as much traffic localized
 * as possible, in order to increase the priority band effect.
 * 
 * Revision 1.3  92/05/01  16:41:40  root
 * VJ header compression installed, testing is proceeding.  Seems
 * to be working nicely.  Need a bit of work on either the
 * hi-water mark or the qband stuff.
 * 
 * Revision 1.2  92/04/28  09:49:46  root
 * Fixed a number of bugs.  Changed b_datap->db_base references
 * to b_rptr;  fixed bug in tsl_dl_cmds that never freed
 * an allocated message block on unitdata requests;  handling
 * of ifstats chain fixed.
 * 
 * Revision 1.1  92/04/28  09:46:34  root
 * Initial revision
 *
 */

/* Old RCS Header: /u3/src/slip/driver/io/RCS/slp.c,v 1.4.1.1 92/05/22 09:26:03 root rel */

/*
 * SLIP6 - serial line internet protocol streams multiplexor driver
 *	   for SVR4.  Van-Jacobssen header compression implemented.
 *
 *	 - the basic module came from the Intel PD slip release.  A
 *	   number of bugs were fixed, and then the 386BSD release of
 *	   slip header compression was added.   See comments below.
 */

/*
 * Serial Line Internet Protocol (SLIP) streams multiplexor driver for
 * Intel Unix System V/386 Release 4.0.
 *
 * The upper streams is supposed to be linked to the ip driver (/dev/ip)
 * and optionally linked to the slip hangup daemon. The lower streams can
 * linked to any number of serial driver.
 *
 * The slattach command builds the ip to slip to serial device.
 *
 * The packet framing protocol code, in the upper write and the lower
 * read service routine (tsl_uwsrv and tsl_lrsrv) is based from
 * tty_slip.c written by Rayan Zachariassen <rayan@ai.toronto.edu> and
 * Doug Kingston <dpk@morgan.com>.
 *
 * Author:
 *     Sudji Husodo <sudji@indo.intel.com> 1/9/91
 */

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/mkdev.h>	/* only for MAXMIN */
#include <sys/dlpi.h>
#include <sys/syslog.h>
#include <sys/strlog.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/log.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/ddi.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#ifdef DEBUG
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#endif

#include "../../patchlevel.h"
#include SLIP_HEADER	/* e.g., <sys/tslip.h> */

#define DL_PRIM_SIZE	sizeof (union DL_primitives)

/*
 * SLMAX is a hard limit on input packet size.  To simplify the code
 * and improve performance, we require that packets fit in an mbuf
 * cluster, and if we get a compressed packet, there's enough extra
 * room to expand the header into a max length tcp/ip header (128
 * bytes).  So, SLMAX can be at most
 *	MCLBYTES - 128
 *
 * SLMTU is a hard limit on output packet size.  To insure good
 * interactive response, SLMTU wants to be the smallest size that
 * amortizes the header cost.  (Remember that even with
 * type-of-service queuing, we have to wait for any in-progress
 * packet to finish.  I.e., we wait, on the average, 1/2 * mtu /
 * cps, where cps is the line speed in characters per second.
 * E.g., 533ms wait for a 1024 byte MTU on a 9600 baud line.  The
 * average compressed header size is 6-8 bytes so any MTU > 90
 * bytes will give us 90% of the line bandwidth.  A 100ms wait is
 * tolerable (500ms is not), so want an MTU around 296.  (Since TCP
 * will send 256 byte segments (to allow for 40 byte headers), the
 * typical packet size on the wire will be around 260 bytes).  In
 * 4.3tahoe+ systems, we can set an MTU in a route so we do that &
 * leave the interface MTU relatively high (so we don't IP fragment
 * when acting as a gateway to someone using a stupid MTU).
 *
 * Similar considerations apply to SLIP_HIWAT:  It's the amount of
 * data that will be queued 'downstream' of us (i.e., in clists
 * waiting to be picked up by the tty output interrupt).  If we
 * queue a lot of data downstream, it's immune to our t.o.s. queuing.
 * E.g., if SLIP_HIWAT is 1024, the interactive traffic in mixed
 * telnet/ftp will see a 1 sec wait, independent of the mtu (the
 * wait is dependent on the ftp window size but that's typically
 * 1k - 4k).  So, we want SLIP_HIWAT just big enough to amortize
 * the cost (in idle time on the wire) of the tty driver running
 * off the end of its clists & having to call back slstart for a
 * new packet.  For a tty interface with any buffering at all, this
 * cost will be zero.  Even with a totally brain dead interface (like
 * the one on a typical workstation), the cost will be <= 1 character
 * time.  So, setting SLIP_HIWAT to ~100 guarantees that we'll lose
 * at most 1% while maintaining good interactive response.
 */
#define	SLMTU		296		/* slip MTU - for 9600 baud */

/*
 * The following disgusting hack gets around the problem that IP TOS
 * can't be set yet.  We want to put "interactive" traffic on a high
 * priority queue.  To decide if traffic is interactive, we check that
 * a) it is TCP and b) one of its ports is telnet, rlogin or ftp control.
 */
static u_int interactive_ports[8] = {
	0,	513,	0,	0,
	0,	21,	0,	23,
};

static int
interactive(p)
u_short p;
{
	p = ntohs(p);
	if(interactive_ports[p & 7] == p) return 1;
		else return 0;
}

int tsl_devflag = 0;		/* V4.0 style driver */

/*
 * A single timeout periodically updates the last-access (atime) field
 * for all links which have seen traffic during the last interval.
 * The daemon is notified of any which have exceeded their idle time-out.
 * (Each link has an "idleflag" which is set by the timeout and cleared
 * when traffic flows).
 */
#define ATIME_GRAN_US (5*1000*1000)	/* timeout granularity - microseconds */
static clock_t atime_gran_ticks;

/* If the daemon never responds to a CONNREQ or HANGUP msg (with I_LINK or 
 * SLP_CONNFAIL or I_UNLINK), the stream remains hung until NOTIF_TIMEOUT_US 
 * expires, after which the condition is cleared.
 * This lets us recover from an aborted/killed daemon.
 */
#define NOTIF_TIMEOUT_US (2*60*1000*1000) /* 2 minutes */
static clock_t notif_timeout_ticks;

#ifdef __STDC__
# define P(list) list
#else
# define P(list) ()
#endif

#ifdef DEBUG
# define STATIC
# define IFDEBUG(STMT) STMT
  STATIC void tsl_d_assert(int line);
  STATIC void tsl_trace_ip_pkt(struct ip *);
#else
# define STATIC static
# define IFDEBUG(STMT)
#endif

#define D_PRINTF(ARGLIST) IFDEBUG(printf ARGLIST)
#define D_ANNOUNCE(ARGLIST) IFDEBUG(tsl_announce ARGLIST)
#define D_ASSERT(EX,ACTION_IF_F) \
		IFDEBUG(if (EX) {} else { tsl_d_assert(__LINE__);ACTION_IF_F})

STATIC int tsl_open	
	P((queue_t *q, dev_t *devp, int flag, int sflag, struct cred *credp));
STATIC int  tsl_close		P((queue_t *q, cred_t *credp));
STATIC int  tsl_uwput		P((queue_t *q, mblk_t *mp));
STATIC int  tsl_uwsrv		P((queue_t *q));
STATIC int  tsl_lwput		P((queue_t *q, mblk_t *mp));
STATIC int  tsl_lwsrv		P((queue_t *q));
STATIC int  tsl_lrput		P((queue_t *q, mblk_t *mp));
STATIC int  tsl_lrsrv		P((queue_t *q));
STATIC void tsl_ioctl		P((queue_t *q, mblk_t *mp));
STATIC void tsl_dl_cmds	P((queue_t *q, mblk_t *mp));
STATIC void tsl_dl_info_ack	P((queue_t *q, mblk_t *response));
STATIC void tsl_dl_bind_ack	P((mblk_t *mp, mblk_t *response));
STATIC void tsl_dl_ok_ack	P((mblk_t *mp, mblk_t *response));
STATIC void tsl_dl_error_ack P((mblk_t *mp, mblk_t *response, ulong dl_errno));
STATIC void tsl_dl_unitdata_ind	P((queue_t *q, mblk_t *mp));
STATIC int  tsl_notify_daemon	P((queue_t *q, tsl_t *p_slip, int op));
STATIC void tsl_link		P((tsl_t *p_slip, queue_t *uwq, int lindex));
STATIC void tsl_unlink		P((queue_t *qbot));
STATIC void tsl_start_timeout	P(());
STATIC void tsl_idle_timeout	P((char *dummy));
STATIC void tsl_flushall	P((tsl_t *tsl_p));
STATIC void tsl_notif_timeout	P((tsl_t *tsl_p));
STATIC void tsl_cancel_notif_timeout	P((tsl_t *tsl_p));
STATIC void tsl_gotpacket	P((tsl_t *p_slip, queue_t *q));
STATIC void tsl_announce	(); /* (tsl_t *tsl_p, char *fmt, ...) */

#define KBYTE 1024

static char driver_name[] = DRIVER_NAME_STR;

static struct module_info minfo[5] = {
	{ SLIPM_ID, driver_name, 0, 8192, 64*KBYTE, 32*KBYTE },
	{ SLIPM_ID, driver_name, 0, 8192, 64*KBYTE, 32*KBYTE },
	{ SLIPM_ID, driver_name, 0, 8192, 64*KBYTE, 32*KBYTE },
	{ SLIPM_ID, driver_name, 0, 8192, 64*KBYTE, 32*KBYTE },
	{ SLIPM_ID, driver_name, 0, 8192, 64*KBYTE, 32*KBYTE }
};

static struct qinit urinit = {
	NULL,       NULL,       tsl_open, tsl_close, NULL, &minfo[0], NULL };

static struct qinit uwinit = {
	tsl_uwput, tsl_uwsrv, tsl_open, tsl_close, NULL, &minfo[1], NULL };

static struct qinit lrinit = {
	tsl_lrput, tsl_lrsrv, tsl_open, tsl_close, NULL, &minfo[3], NULL };

static struct qinit lwinit = {
	tsl_lwput, tsl_lwsrv, tsl_open, tsl_close, NULL, &minfo[4], NULL };

struct streamtab tsl_info = {
	&urinit, &uwinit, &lrinit, &lwinit };

STATIC tsl_t *tsl_daemon = (tsl_t *) 0;
STATIC int tsl_idle_toid = 0;

extern	struct	ifstats	*ifstats;

/* Declarations you would think exist in somewhere, but don't seem to... */
extern void panic(const char *fmt,...);
extern void printf(const char *fmt,...);
typedef int pl_t;	/* pl_t is in DDI/DKI ref., but not in ESIX4.0 */
extern pl_t splbase(void);
extern pl_t spltimeout(void);
extern pl_t spldisk(void);
extern pl_t splstr(void);
extern pl_t splhi(void);
extern pl_t splx(pl_t oldlevel);

/* state data is dynamically allocated at open time.  A single linked list of
 * all state structures is maintained.
 */
static tsl_t *tsl_listhead;

/* Locate the state structure corresponding to an open minor number */
static tsl_t *
tsl_find_state(minor_t m)
{
	register tsl_t *p;
	for (p=tsl_listhead; p!=NULL; p = p->next) {
		if (p->minor == m) return p;
	}
	return NULL;
}
/* Insert a new state structure into the list (part of open) */
static void
tsl_insert_state(tsl_t *p)
{
	p->next = tsl_listhead;
	tsl_listhead = p;
}
/* Remove a state from the list (part of close) */
static void
tsl_remove_state(tsl_t *pdel)
{
	register tsl_t *p;
	register tsl_t *prev = NULL;
	for (p=tsl_listhead; p!=pdel; p = p->next) {
		if (p == NULL) panic("tsl_remove_state m=%d",pdel->minor);
		prev = p;
	}
	if (prev) 
	    prev->next = pdel->next; 
        else 
	    tsl_listhead = pdel->next;
}

int npackets = 0;

/*
 * tsl_start - called once at boot time
 */
void
tsl_start(void)
{
	printf("TSLIP %s", VERSION);
	if (PATCHLEVEL) printf(" (patch %d)", PATCHLEVEL);
	printf ("\n");
}

pl_t splstr_pri;	/* ### debugging - see tsl_lrput */

/*
 * tsl_open
 */

STATIC int	/*ARGSUSED2*/
tsl_open (q, devp, flag, sflag, credp)
queue_t	*q;
dev_t	*devp;
int		flag;
int		sflag;
struct	cred	*credp;
{
	register tsl_t *p_slip;
	pl_t	 oldpri;
	mblk_t	 *bp;
	int	result = 0;
	major_t  nmajor = getmajor (*devp);
	minor_t  nminor = getminor (*devp);

	oldpri = splstr ();
	splstr_pri = splstr();	/* ### TEMP debugging */

	if (sflag != CLONEOPEN) {
	    /* Open another stream to an existing device (if it's open) */
	    p_slip = tsl_find_state(nminor);
	    if (p_slip == NULL) {
		D_PRINTF(("tsl_open: minor %ld is not open.\n",(long)nminor);)
		result = ENXIO;
	    }
    	} else {
            /* CLONEOPEN - the only way to initially open a minor device.
             * Find the lowest unused minor number.
             */
            for (nminor=0; (p_slip=tsl_find_state(nminor)) != NULL; nminor++) {}
            if (nminor >= MAXMIN) {
                printf("tsl_open: MAXMIN exceeded\n");
        	splx (oldpri);
                return ENXIO;
            }
    
            *devp = makedevice (nmajor, nminor);
    
            /* Allocate a state structure and initialize it */
            p_slip = (tsl_t *) kmem_alloc(sizeof(tsl_t), KM_NOSLEEP);
            if (p_slip == NULL) {
                printf("tsl_open: out of kernel memory\n");
        	splx (oldpri);
                return ENXIO;
            }
            bzero((caddr_t)p_slip, sizeof(*p_slip));
            p_slip->state    = DL_UNBOUND;
            p_slip->qtop     = q;
            p_slip->qbot     = NULL;
            p_slip->bptr     = p_slip->buf + BUFOFFSET;
            p_slip->qt_blocked = 0;
            drv_getparm (PPID, &p_slip->pid);   /* opening process's PID */
    
            p_slip->escape   = p_slip->overrun = p_slip->inlen = 0;
            p_slip->flags    = IFF_UP | IFF_POINTOPOINT;
    
            /* initialize interface and its statistics */
            p_slip->stats.ifs_name       = p_slip->statsnamebuf;
            p_slip->statsnamebuf[0]	 = '\0';
            p_slip->stats.ifs_unit       = 0;
            p_slip->stats.ifs_active     = 0;
            p_slip->stats.ifs_mtu            = SLMTU;
            p_slip->stats.ifs_ipackets   = p_slip->stats.ifs_opackets = 0;
            p_slip->stats.ifs_ierrors    = p_slip->stats.ifs_oerrors  = 0;
            p_slip->stats.ifs_collisions = 0;
    
            tsl_compress_init(&p_slip->slcomp); /* redundant -- see tsl_link */
            p_slip->sc_flags = 0;
            p_slip->xmit_hiband = p_slip->recv_hiband = 0;
            p_slip->uw_blocked = 0;
    
            /* initialize read and write queue pointers to private data */
            /* Put the structure into the open-minor linked list. */
            q->q_ptr = (caddr_t) p_slip;
            WR(q)->q_ptr = (caddr_t) p_slip;
    
            p_slip -> minor = nminor;
            tsl_insert_state(p_slip);
    
            /* set up the correct stream head flow control parameters */
    
            if ((bp = allocb (sizeof (struct stroptions), BPRI_MED)) != NULL) {
                struct stroptions *sop = (struct stroptions *) bp->b_rptr;
                bp->b_datap->db_type = M_SETOPTS;
                bp->b_wptr += sizeof (struct stroptions);
                sop->so_flags = SO_HIWAT | SO_LOWAT;
                sop->so_hiwat = minfo [2].mi_hiwat;
                sop->so_lowat = minfo [2].mi_lowat;
                putnext (q, bp);
            }
    
            D_ANNOUNCE((p_slip, "tsl_open dev:(%ld,%ld) flag=%d sflag=%d",
                       (long)nmajor,(long)nminor,flag,sflag);)
	}
        splx (oldpri);
	return result;
}

void
dumpq(label, q)
char *label;
queue_t *q;
{
	queue_t *qp;

	for(qp=q; qp; qp=qp->q_next) {
		printf("%s: name <%s>, hiwat %d, lowat %d",
			label,
			qp->q_qinfo->qi_minfo->mi_idname,
			qp->q_hiwat,
			qp->q_lowat);
		if(qp->q_qinfo->qi_srvp) printf(" SRV\n");
			else printf("\n");
	}
}

/*
 * tsl_close ()
 */

STATIC int /*ARGSUSED1*/
tsl_close (q, credp)
queue_t	*q;
cred_t *credp;
{
	tsl_t	*p_slip, *pp;
	pl_t	oldpri;
	struct ifstats *ifp, *pifp;

	p_slip = (tsl_t *) q->q_ptr;
	if (!p_slip->qtop) panic("tsl_close");
	D_ANNOUNCE((p_slip, "tsl_close");)

	oldpri = splstr ();

	if (p_slip == tsl_daemon) {
		/* Unblock any which might be waiting for the daemon */
		tsl_daemon = NULL;
		for (pp=tsl_listhead; pp!=NULL; pp = pp->next) {
		    qenable(WR(pp->qtop));
		}
	}
	tsl_cancel_notif_timeout(p_slip);
	if (p_slip->qbot) 
	    tsl_unlink(p_slip->qbot);
	for(ifp = ifstats, pifp = NULL; ifp; ifp = ifp->ifs_next) {
		if(ifp == &p_slip->stats) {
			if(pifp) pifp->ifs_next = ifp->ifs_next;
				else ifstats = ifp->ifs_next;
			break;
		}
		pifp = ifp;
	}

	tsl_remove_state(p_slip);
	if (p_slip->autodialp)
	    kmem_free (p_slip->autodialp, strlen(p_slip->autodialp)+1);
	bzero((caddr_t)p_slip, sizeof(*p_slip)); /* debugging */
	kmem_free(p_slip, sizeof(tsl_t));
	
	splx (oldpri);
	return 0; /* success */
}

/*
 *	tsl_ioctl ()
 */

STATIC void
tsl_ioctl (q, mp)
queue_t	*q;
mblk_t	*mp;
{
	struct iocblk *iocp;
	struct ifreq  *ifr;
	tsl_t *p_slip, *pp;
	tsl_info_t *ip;
	pl_t	oldpri;
	struct	linkblk	*lp;
	char	*p;
	int	n;
	queue_t	*qbot;
	dev_t	minor;
	uint_t	reply_type;

	p_slip  = (tsl_t *) q->q_ptr;
	iocp = (struct iocblk *) mp->b_rptr;
	reply_type = M_IOCACK;

	D_ASSERT( iocp->ioc_count != TRANSPARENT,  /* else... */
		  	printf("slp: TRANSPARENT ioctl!\n");
		  	mp->b_datap->db_type = M_IOCNAK; 
		  	qreply(q,mp); 
		  	return; )

	oldpri = splstr();

	switch (iocp->ioc_cmd) {

	case SLP_REG_SLIPD:
		if (tsl_daemon) {
		    reply_type = M_IOCNAK;
		}
		else {
		    D_ANNOUNCE((p_slip, "REG_SLIPD");)
		    notif_timeout_ticks = drv_usectohz((clock_t)NOTIF_TIMEOUT_US);
	            atime_gran_ticks = drv_usectohz((clock_t)ATIME_GRAN_US);
		    tsl_daemon = p_slip;
		    tsl_start_timeout();
		}
		break;

	case SLP_UNREG_SLIPD:
		D_ANNOUNCE((p_slip, "UNREG_SLIPD");)
		tsl_daemon = NULL;
		break;

	case SLP_DO_COMPRESS:
		p_slip->sc_flags |= SC_COMPRESS;
		p_slip->flags |= IFF_NOTRAILERS;
		break;

	case SLP_DO_AUTOCOMP:
		p_slip->sc_flags |= SC_AUTOCOMP;
		p_slip->flags |= IFF_NOARP;
		break;

	case SLP_SET_MTU:
		p_slip->stats.ifs_mtu = *(int *)mp->b_cont->b_rptr;
		break;

	case SLP_SET_IDLETO:
		p_slip->idletime = *(time_t *)mp->b_cont->b_rptr;
		D_ANNOUNCE((p_slip, "Idle timeout is %ld seconds", 
			    (long)p_slip->idletime);)
		break;

	case SLP_SET_CONNP:
		p = (char *)mp->b_cont->b_rptr; 
		n = (char *)mp->b_cont->b_wptr - p;
		if (n<=0 || p[n-1] != '\0' || n != (strlen(p)+1)) {
		    printf("slp: SLP_SET_CONNP - bad string\n");
		    reply_type = M_IOCNAK;
		    break;
		}
		if (p_slip->autodialp)
		    kmem_free( p_slip->autodialp, strlen(p_slip->autodialp)+1 );
	        p_slip->autodialp = (u_char *) kmem_alloc (n, KM_SLEEP);
		bcopy( (caddr_t)p, (caddr_t)p_slip->autodialp, n );
		D_ANNOUNCE((p_slip, "Auto-connect parameters: %s", 
			    p_slip->autodialp);)
		break;

	case SLP_MOVE_LOWER:
		/* Move the driver linked below this stream to another
		 * minor number, passed in the M_DATA part of the message.
		 * The daemon uses this to supply an existing slip interface.
		 */
		minor = *(int *) mp->b_cont->b_rptr;
		D_ANNOUNCE((p_slip, "MOVE_LOWER minor=%ld",minor);)
		pp = tsl_find_state(minor);
		if (pp->qtop == NULL || pp->qbot != NULL) {
		        /* not open or already has a driver linked below it */
		        reply_type = M_IOCNAK;
		}
		else {
			qbot = p_slip-> qbot;
			n = p_slip-> lindex;
			tsl_unlink(qbot);
			tsl_link(pp, qbot, n);
		}
		break;

	case SLP_CONNFAIL:
		minor = *(int *) mp->b_cont->b_rptr;
		D_ANNOUNCE((p_slip, "CONNFAIL minor %ld", (long)minor));
		pp = tsl_find_state(minor);
		if (pp == NULL) {
			reply_type = M_IOCNAK;
		} else {
			tsl_flushall(pp);
		}
		break;

	case SLP_GETINFO:
		minor = *(int *) mp->b_cont->b_rptr;
		D_ANNOUNCE((p_slip, "GETINFO minor %ld", (long)minor));
		pp = tsl_find_state(minor);
		if (pp == NULL) {
			reply_type = M_IOCNAK;
			break;
		} 
		ip = (tsl_info_t *)mp->b_cont->b_rptr;
		if ( ((u_char *)ip + sizeof(tsl_info_t)) >
						mp->b_cont->b_datap->db_lim ) {
			tsl_announce(pp, "GETINFO: msg too short");
		    	reply_type = M_IOCNAK;
		} else {
		    	mp->b_cont->b_wptr = (u_char *)ip + sizeof(tsl_info_t);
			bcopy((caddr_t)pp->origifn, (caddr_t)ip->origifn, 
							sizeof(ip->origifn));
			ip->lindex = pp->lindex;
			iocp->ioc_error = iocp->ioc_rval = 0;
		}
		break;

	case I_LINK:
	case I_PLINK:
		lp = (struct linkblk *) mp->b_cont->b_rptr;
		tsl_link(p_slip, lp->l_qbot, lp->l_index);
		iocp->ioc_error = iocp->ioc_rval = iocp->ioc_count = 0;
		break;

	case I_UNLINK:
	case I_PUNLINK:
		lp = (struct linkblk *) mp->b_cont->b_rptr;
		tsl_unlink(lp->l_qbot);
		iocp->ioc_error = iocp->ioc_rval = iocp->ioc_count = 0;
		break;

	case SIOCSIFNAME:
		/* copy interface name to local slip structure */

		ifr = (struct ifreq *) mp->b_cont->b_rptr;

		/* Prevent setting a non-uniqe name/unit# */
		for (pp=tsl_listhead; pp!=NULL; pp = pp->next) {
		    D_ASSERT(pp->qtop != NULL, panic("tsl_ioctl:qt0");)
		    if (pp != p_slip &&
			0==strcmp(pp->origifn,ifr->ifr_name) &&
		        pp->stats.ifs_unit == pp->stats.ifs_unit) break;
		}
		if (pp != NULL) {
			reply_type = M_IOCNAK;
			break;
		}

		strncpy (p_slip->origifn, ifr->ifr_name, IFNAMSIZ);

		/*
		 * interface name (ifr->ifr_name) contains the name and unit,
		 * e.g., "sl0", "sl12", "emd0", "wd1", etc. 
		 * These must be stored separately in the stats structure.
		 * If unit is not supplied, e.g. "slip", then unit number is 
		 * assumed to be zero.
		 */
		strncpy (p_slip->stats.ifs_name, ifr->ifr_name, IFNAMSIZ);
		n = 0;

		/* starting from the last char, find the first non-digit char */
		p = p_slip->stats.ifs_name + strlen(p_slip->stats.ifs_name) - 1;
		while ('0' <= *p && *p <= '9')
			p--;

		/* calculate integer, replace digits with nulls */
		while (*++p) {
			n = 10*n + (*p-'0');
			*p = '\0';
		}
		p_slip->stats.ifs_unit = n;	/* set ifs unit number  */

		/* set ifstats pointers, used for statistics by the 
		   netstat command. */
		p_slip->stats.ifs_next = ifstats;
		ifstats = &p_slip->stats;

		break;

	case SIOCGIFFLAGS:
		((struct iocblk_in *)iocp)->ioc_ifflags = p_slip->flags;
		break;

	case SIOCSIFFLAGS:
		p_slip->flags = ((struct iocblk_in *)iocp)->ioc_ifflags;
		p_slip->sc_flags &= ~(SC_COMPRESS | SC_AUTOCOMP);
		if(p_slip->flags & IFF_NOARP) p_slip->sc_flags |= SC_AUTOCOMP;
		if(p_slip->flags & IFF_NOTRAILERS) p_slip->sc_flags |= SC_COMPRESS;
		break;

	case SIOCSIFADDR:
		p_slip->flags |= IFF_RUNNING;
		((struct iocblk_in *)iocp)->ioc_ifflags |= IFF_RUNNING;
		break;

	default:
		break;
	}
	splx (oldpri);
	mp->b_datap->db_type = reply_type; /* M_IOCACK or M_IOCNAK */
	qreply (q, mp);
}

/*
 * tsl_uwput ()
 */

STATIC int
tsl_uwput (q, mp)
queue_t	*q;
mblk_t	*mp;
{
	switch (mp->b_datap->db_type) {

	case M_FLUSH:
		D_ANNOUNCE((q->q_ptr, "uwput M_FLUSH");)
		if (*mp->b_rptr & FLUSHW) {
			/* JimA: Shouldn't the lower stream be flushed also? */
			flushq(q, FLUSHALL);
			*mp->b_rptr &= ~FLUSHW;
		}
		if (*mp->b_rptr & FLUSHR)
			qreply(q, mp);
		else
			freemsg(mp);
		break;

	case M_IOCTL:
		tsl_ioctl (q, mp);
		break;

	case M_PROTO:
	case M_PCPROTO:
		tsl_dl_cmds (q, mp);
		break;

	default:
		tsl_announce ( (tsl_t *)q->q_ptr, "uwput UNK msg type %d",
				mp->b_datap->db_type );
		freemsg(mp); 
		/* ...the code I inherited passed unk msgs to the driver...
		 * if (((tsl_t *)q->q_ptr)->qbot)
		 *    putnext (((tsl_t *)q->q_ptr)->qbot, mp);
		 */
		break;
	}
	return 0; /* (ignored) */
}

/*
 * tsl_uwsrv ()
 */

STATIC int
tsl_uwsrv (q)
register queue_t *q;
{
	register mblk_t	*mp, *mpd, *mp2;
	register u_char	*cp, *cpend, c;
	register int	pktlen, num;
	register tsl_t *p_slip;
	register struct ip *ip;

	p_slip = (tsl_t *) q->q_ptr;

	while ((mp = getq(q)) != NULL) {

		p_slip->idleflag = 0;
#ifdef DEBUG
		printf("T ");
		tsl_trace_ip_pkt( (struct ip *)mp->b_cont->b_rptr );
#endif

		/* N.B. we notify only if there is actually work queued */
		if (p_slip->qbot == NULL) {
		    if (tsl_daemon) {
			putbq (q, mp);
			tsl_notify_daemon(q, p_slip, SLPD_CONNREQ);
			return 0;
		    } else {
			tsl_announce(p_slip,"NO DAEMON (flushing)");
			freemsg(mp);
			tsl_flushall(p_slip);
			break;
		    }
		}

		pktlen = 0;

		if((ip=(struct ip *)mp->b_cont->b_rptr)->ip_p == IPPROTO_TCP) {
			register mblk_t *mptr;
			u_short tmp;
			register int p = ((int *)ip)[ip->ip_hl];

			tmp = ntohs(ip->ip_len);

			/* CORRECT the output byte wptr */
			for(mptr=mp->b_cont; mptr->b_cont; mptr=mptr->b_cont) {
				tmp -= (u_short)(mptr->b_wptr - mptr->b_rptr);
			}
			mptr->b_wptr = mptr->b_rptr + tmp;

			if (interactive(p >> 16) || interactive(p & 0xffff)) {
				p = 1;
				mp->b_band = 1;
			} else {
				p = 0;
			}
			if (p_slip->sc_flags & SC_COMPRESS) {
				/*
				 * The last parameter turns off connection id
				 * compression for background traffic:  Since
				 * fastq traffic can jump ahead of the background
				 * traffic, we don't know what order packets will
				 * go on the line.
				 */
				p = tsl_compress_tcp(mp->b_cont, ip,
					&p_slip->slcomp, p);
				*((u_char *) mp->b_cont->b_rptr) |= p;
			} else {
				/*EMPTY*/
			}
		} else 
		if (ip->ip_p == IPPROTO_ICMP) {
		        if (p_slip->sc_flags & SC_NOICMP) {
				freemsg(mp);
				continue;
		    	}
		}
		if (!bcanput(p_slip->qbot,mp->b_band)) {
			putbq (q, mp);
			p_slip->qt_blocked = 1;
			p_slip->uw_blocked++;
			return 0;
		}
		/*
		 * count the number of special characters (END & ESC)
		 */

		num = 2;		/* END char is put at the start and end of packet */

		for (mpd = mp->b_cont; mpd != 0; mpd = mpd->b_cont) {
			pktlen += (mpd->b_wptr - mpd->b_rptr);

			for (cp=mpd->b_rptr, cpend=mpd->b_wptr; cp<cpend; cp++){
				if (*cp == END || *cp == ESC)
					num++;
			}
		}
		/*
		 * allocate message block to be sent down stream
		 */

		if ((mp2 = allocb (pktlen + num, BPRI_MED)) == NULL) {
			p_slip->stats.ifs_oerrors++;
			freemsg (mp);
			return 0;
		}
		/*
		 * frame packet, escape special characters ESC and END
		 */

		*mp2->b_wptr++ = END;

		for (mpd = mp->b_cont; mpd != 0; mpd = mpd->b_cont) {
			for (cp=mpd->b_rptr, cpend=mpd->b_wptr; cp<cpend; cp++){
				if ((c = *cp) == END) {
					*mp2->b_wptr++ = ESC;
					*mp2->b_wptr++ = ESC_END;
				}
				else if (c == ESC) {
					*mp2->b_wptr++ = ESC;
					*mp2->b_wptr++ = ESC_ESC;
				}
				else
					*mp2->b_wptr++ = c;
			}
		}
		*mp2->b_wptr++ = END;

		mp2->b_datap->db_type = M_DATA;
		mp2->b_band = mp->b_band;
		p_slip->xmit_hiband += mp->b_band;
		p_slip->stats.ifs_opackets++;
		freemsg (mp);
		tsl_lwput (p_slip->qbot, mp2);
	}
	return 0; /* (ignored) */
}

void
dump_slstats(p_slip)
tsl_t *p_slip;
{
	struct tslcompress *sc = &p_slip->slcomp;
	if (sc) {
	    printf("packets %d compr %d search %d miss %d\n",
		sc->sls_packets, sc->sls_compressed,
		sc->sls_searches, sc->sls_misses);
	    printf("uncompin %d, compin %d, errorin %d, tossedin %d\n",
		sc->sls_uncompressedin,
		sc->sls_compressedin,
		sc->sls_errorin,
		sc->sls_tossed);
	}
	printf("hisend %d hirecv %d uwblocked %d\n",p_slip->xmit_hiband,
		p_slip->recv_hiband,p_slip->uw_blocked);
}
/*
 * tsl_lwput
 */

STATIC int
tsl_lwput (q, mp)
queue_t *q;
mblk_t  *mp;
{
	if (bcanput(q->q_next,mp->b_band)) putnext(q, mp);
	else putq(q, mp);
	return 0; /* (ignored) */
}

/*
 * tsl_lwsrv (q)
 */

STATIC int
tsl_lwsrv (q)
queue_t *q;
{
	mblk_t  *mp;
	tsl_t *p_slip;

	while ((mp = getq(q))) {
		if (bcanput(q->q_next,mp->b_band)) putnext(q, mp);
		else {
			putbq(q, mp);
			return 0;
		}
	}
	p_slip = (tsl_t *) q->q_ptr;

	if (p_slip==NULL) {
		D_PRINTF(("tsl_lwsrv: q_ptr==NULL!!\n");)
		return 0;	/* dangling driver on closed slip */
	}

	if (p_slip->qt_blocked) {
		/*
		 * qtop is the upper read q and
		 * we want to enable the upper write q
		 */
		qenable (WR(p_slip->qtop));
		p_slip->qt_blocked = 0;
	}
	return 0; /* (ignored) */
}

/*
 * tsl_lrput ()
 *
 * NOTE: tsl_lrput might be called from the serial driver interrupt handler
 *	 if it has no read-service routine.  Therefore, we might interrupt our
 *	 own M_UNLINK processing code, and so must be careful to avoid
 *	 race conditions.  We can't call splstr(), because that would 
 *	 lower rather than raise the priority level if we are already at 
 *	 an interrupt priority.
 */

STATIC int
tsl_lrput (q, mp)
queue_t	*q;
mblk_t	*mp;
{
	tsl_t 	*p_slip;
	queue_t	*qtop;

	p_slip = (tsl_t *) q->q_ptr;

	if (p_slip==NULL) {
		D_PRINTF(("tsl_lrput: q_ptr==NULL!!\n");)
		return 0;	/* dangling driver on closed slip */
	}

	/* ### TEMP debugging
	 * Peter Wemm reported (some time ago) corrupted streams data structs
	 * using tslip.  The symptoms sounded exactly like those of the
	 * sas-1.5 serial driver bug, but he said he wasn't using that driver
	 * at the time.  TSLIP does almost everything at slpstr().  The only
	 * exception might be this put routine which could be entered at a
	 * higher priority, and presumably could preempt code running at 
	 * splstr.  I rewrite this code to avoid bad effects of preempting
	 * ourself, but I want to know if it is ever actually needed.
	 * Therefore, the following prints a message on the console when
	 * invoked at a priority higher than splstr.
	 */
	{ pl_t entrypri;
	  entrypri = splhi(); 
	  splx(entrypri);
	  if (entrypri > splstr_pri)
	      printf("### tsl_lrput at pri %d\n", entrypri);
	}
	
	switch (mp->b_datap->db_type) {

	case M_DATA:
	case M_HANGUP:
		putq (q, mp);
		break;

	case M_FLUSH:
		if (*mp->b_rptr & FLUSHR)
			flushq(q, FLUSHALL);
		if (*mp->b_rptr & FLUSHW) {
			*mp->b_rptr &= ~FLUSHR;
			flushq(WR(q), FLUSHALL);
			qreply(q, mp);
		} else
			freemsg(mp);
		return 0;

	default:
		qtop = p_slip->qtop;
		if (qtop == NULL) {
			printf("###tsl_lrput: qtop==NULL!!\n");
			return 0;	/* just now unlinked? */
		} else {
			putnext (qtop, mp);
		}
		break;
	}
	return 0; /* (ignored) */
}

/*
 * tsl_lrsrv ()
 */

STATIC int
tsl_lrsrv (q)
queue_t	*q;
{
	register	u_char 	*cp;
	register	tsl_t 	*p_slip;
	mblk_t		*bp, *mp;
	pl_t		oldpri;

	p_slip = (tsl_t *) q->q_ptr;

	oldpri = splstr ();

	while ((mp = getq(q)) != NULL) {

		if (p_slip==NULL) {	/* driver just unlinked from slip? */
			/* D_PRINTF(("tsl_lrsrv: q_ptr==NULL!!\n");) */
			printf("tsl_lrsrv: q_ptr==NULL!!\n");
			freemsg(mp);
			continue;
		}	

		if (mp->b_datap->db_type == M_HANGUP) {
			if (tsl_daemon) {
			    if (!tsl_notify_daemon(q, p_slip, SLPD_HANGUP)) {
				putbq (q, mp);
			        splx (oldpri);
				return 0;
			    } else {
				p_slip-> hangup_pend = 1;
				freemsg(mp);
			    }
			} else {
		 	    /* no daemon -- pass the message upstream */
			    putnext (p_slip->qtop, mp);
			}
			continue;
		}

		if (!bcanput (p_slip->qtop->q_next,mp->b_band)) {
			D_PRINTF(("### !bcanput\n");)
			putbq (q, mp);
			splx (oldpri);
			return 0;
		}

		for (bp = mp; bp != 0; bp = bp->b_cont) {
			for (cp = bp->b_rptr; cp < bp->b_wptr; cp++) {
				if (*cp == END) {
					if (p_slip->inlen < 3) /*EMPTY*/ ; /* ignore packet */
					else if (p_slip->overrun) {
						printf("SLIP OVERRUN size %d!\n",p_slip->inlen);
						p_slip->stats.ifs_ierrors++;
						p_slip->inlen = 0;
						p_slip->bptr = p_slip->buf +
							BUFOFFSET;
						p_slip->overrun = 0;
						p_slip->escape = 0;
					} else
						tsl_gotpacket(p_slip,q);
				}
				else if (p_slip->inlen > SLIPMTU)
					p_slip->overrun = 1;

				else if (*cp == ESC)					/* if data is ESC */
					p_slip->escape = 1;

				else if (p_slip->escape) {
					p_slip->escape = 0;

					if (*cp == ESC_END)
						*(p_slip->bptr + p_slip->inlen++) = END;
					else if (*cp == ESC_ESC)
						*(p_slip->bptr + p_slip->inlen++) = ESC;
					else
						*(p_slip->bptr + p_slip->inlen++) = *cp;
				}
				else
					*(p_slip->bptr + p_slip->inlen++) = *cp;
			}
			bp->b_rptr = cp;
		}
		freemsg (mp);
	}
	splx (oldpri);
	return 0; /* (ignored) */
}

STATIC void
tsl_gotpacket(p_slip,q)
register tsl_t *p_slip;
queue_t *q;
{
	mblk_t *mp1, *mp2;
	register u_int type;
	pl_t oldpri,band;
	register struct ip *ip;

	oldpri = splstr();
	if ((type = (*p_slip->bptr & 0xf0)) != (IPVERSION << 4)) {
		if (type & 0x80) {
			type = TYPE_COMPRESSED_TCP;
		} else if (type == TYPE_UNCOMPRESSED_TCP) {
			*p_slip->bptr &= 0x4f;
		}
		/*
		 * We've got something that's not an IP packet.
		 * If compression is enabled, try to decompress it.
		 * Otherwise, if `auto-enable' compression is on and
		 * it's a reasonable packet, decompress it and then
		 * enable compression.  Otherwise, drop it.
		 */
		if(p_slip->sc_flags & SC_COMPRESS) {
			p_slip->inlen = tsl_uncompress_tcp(&p_slip->bptr,
				p_slip->inlen, type, &p_slip->slcomp);
			if (p_slip->inlen <= 0) {
				D_ANNOUNCE((p_slip,"COMPRESS:Bad pkt");)
				p_slip->stats.ifs_ierrors++;
				goto done;
			}
		} else if ((p_slip->sc_flags & SC_AUTOCOMP) &&
		    type == TYPE_UNCOMPRESSED_TCP && p_slip->inlen >= 40) {
			p_slip->inlen = tsl_uncompress_tcp(&p_slip->bptr, p_slip->inlen,
						type, &p_slip->slcomp);
			if (p_slip->inlen <= 0) {
				D_ANNOUNCE((p_slip,"AUTOCOMP:Bad pkt");)
				p_slip->stats.ifs_ierrors++;
				goto done;
			}
			p_slip->sc_flags |= SC_COMPRESS;
			p_slip->flags |= IFF_NOTRAILERS;
			D_ANNOUNCE((p_slip,"AUTOCOMP:Compressed pkt detected");)
		} else {
			D_ANNOUNCE((p_slip,"PLAIN:Bad pkt");)
			p_slip->stats.ifs_ierrors++;
			goto done;
		}
	}
#ifdef DEBUG
	printf("R ");
	tsl_trace_ip_pkt( (struct ip *)p_slip->bptr );
#endif
	band = 0;
	if((ip=(struct ip *)p_slip->bptr)->ip_p == IPPROTO_TCP) {
		register int p = ((int *)ip)[ip->ip_hl];

		if (interactive(p >> 16) || interactive(p & 0xffff)) {
			band = 1;
			p_slip->recv_hiband++;
		}
	}

	if ((mp1=allocb (DL_UNITDATA_IND_SIZE,BPRI_MED))==NULL)
		p_slip->stats.ifs_ierrors++;
	else if ((mp2 = allocb (p_slip->inlen, BPRI_MED)) == NULL) {
		p_slip->stats.ifs_ierrors++;
		freemsg (mp1);
	} else {
		p_slip->stats.ifs_ipackets++;	/* send unit data */
		linkb (mp1, mp2);		/* indication up */
		tsl_dl_unitdata_ind (q, mp1);	/* stream */
		mp1->b_band = band;
		putnext (p_slip->qtop, mp1);
	}
done:
	p_slip->inlen = 0;			/* reset info for */
	p_slip->bptr = p_slip->buf + BUFOFFSET;
	p_slip->overrun = 0;			/* receiving data */
	p_slip->escape = 0;
	splx(oldpri);
}

/*
 * tsl_dl_cmds ()
 */

STATIC void
tsl_dl_cmds (q, mp)
queue_t	*q;
mblk_t	*mp;
{
	mblk_t				*response;
	tsl_t				*p_slip;
	union DL_primitives		*p_dl;
	pl_t		oldpri;

	p_slip = (tsl_t *)q->q_ptr;

	p_dl = (union DL_primitives *) mp->b_rptr;

	if(p_dl->dl_primitive == DL_UNITDATA_REQ) {
		if (p_slip->state == DL_IDLE) {
			putq (q, mp);
			return;
		}
	}

	if ((response = allocb (DL_PRIM_SIZE, BPRI_MED)) == NULL) {
		freemsg (mp);
		return;
	}

	switch (p_dl->dl_primitive) {

	case DL_INFO_REQ:
		tsl_dl_info_ack (q, response);
		break;

	case DL_BIND_REQ:
		if (p_slip->state == DL_UNBOUND) {
			oldpri = splstr ();
			p_slip->sap   = ((dl_bind_req_t *)p_dl)->dl_sap;
			p_slip->state = DL_IDLE;
			p_slip->stats.ifs_active = 1;
			splx (oldpri);

			tsl_dl_bind_ack (mp, response);
		}
		else
			tsl_dl_error_ack (mp, response, DL_OUTSTATE);
		break;

	case DL_UNBIND_REQ:
		if (p_slip->state == DL_IDLE) {
			oldpri = splstr ();
			p_slip->state = DL_UNBOUND;
			p_slip->stats.ifs_active = 0;
			splx (oldpri);

			flushq (q, FLUSHDATA);						/* Flush both q's */
			flushq (RD(q), FLUSHDATA);

			tsl_dl_ok_ack (mp, response);
		}
		else
			tsl_dl_error_ack (mp, response, DL_OUTSTATE);

		break;

	case DL_UNITDATA_REQ:
		tsl_dl_error_ack (mp, response, DL_OUTSTATE);
		break;

	default:
		tsl_dl_error_ack (mp, response, DL_UNSUPPORTED);
		break;
	}
	freemsg (mp);
	putnext (RD(q), response);
}

/*
 *  tsl_dl_info_ack ()
 */

STATIC void
tsl_dl_info_ack (q, response)
queue_t *q;
mblk_t	*response;
{
	dl_info_ack_t	*p_info_ack;
	tsl_t			*p_slip;

	p_slip = (tsl_t *) q->q_ptr;

	p_info_ack = (dl_info_ack_t *) response->b_wptr;
	p_info_ack->dl_primitive     = DL_INFO_ACK;
	p_info_ack->dl_max_sdu       = SLMTU;

	p_info_ack->dl_min_sdu       = 46;                  /* ????? */
	p_info_ack->dl_addr_length   = 0;					/* ????? MAC_ADD_SIZE*/
	p_info_ack->dl_mac_type      = DL_CHAR;				/* ????? */

	p_info_ack->dl_current_state = p_slip->state;
	p_info_ack->dl_service_mode  = DL_CLDLS;			/* connecionless DL */

	p_info_ack->dl_qos_length       = 0;				/* ???? */
	p_info_ack->dl_qos_offset       = 0;				/* ???? */
	p_info_ack->dl_qos_range_length = 0;				/* ???? */
	p_info_ack->dl_qos_range_offset = 0;				/* ???? */

	p_info_ack->dl_provider_style = DL_STYLE1;

	p_info_ack->dl_addr_offset = 0;						/* ???? */
	p_info_ack->dl_growth = 0;

	response->b_datap->db_type = M_PCPROTO;
	response->b_wptr           += DL_INFO_ACK_SIZE;
}

/*
 *  tsl_dl_bind_ack ()
 */

STATIC void
tsl_dl_bind_ack (mp, response)
mblk_t	*mp;
mblk_t	*response;
{
	dl_bind_req_t	*p_dl;
	dl_bind_ack_t	*p_bind;

	p_dl = (dl_bind_req_t *) mp->b_rptr;

	p_bind = (dl_bind_ack_t *) response->b_wptr;
	p_bind->dl_primitive   = DL_BIND_ACK;
	p_bind->dl_sap         = p_dl->dl_sap;
	p_bind->dl_addr_length = 0;
	p_bind->dl_addr_offset = 0;

	response->b_wptr           += DL_BIND_ACK_SIZE;
	response->b_datap->db_type = M_PCPROTO;
}

/*
 *  tsl_dl_ok_ack ()
 */

STATIC void
tsl_dl_ok_ack (mp, response)
mblk_t	*mp;
mblk_t	*response;
{
	union DL_primitives	*p_dl;
	dl_ok_ack_t		*p_ok_ack;

	p_dl = (union DL_primitives *) mp->b_rptr;

	p_ok_ack = (dl_ok_ack_t *)(response->b_wptr);
	p_ok_ack->dl_primitive         = DL_OK_ACK;
	p_ok_ack->dl_correct_primitive = p_dl->dl_primitive;

	response->b_wptr           += DL_OK_ACK_SIZE;
	response->b_datap->db_type = M_PCPROTO;
}

/*
 * tsl_dl_error_ack
 */

STATIC void
tsl_dl_error_ack (mp, response, dl_errno)
mblk_t	*mp;
mblk_t	*response;
ulong	dl_errno;
{
	union DL_primitives	*p_dl;
	dl_error_ack_t		*p_error;

	p_dl = (union DL_primitives *) mp->b_rptr;

	p_error = (dl_error_ack_t *) response->b_wptr;
	p_error->dl_primitive       = DL_ERROR_ACK;
	p_error->dl_error_primitive = p_dl->dl_primitive;
	p_error->dl_errno           = dl_errno;
	p_error->dl_unix_errno      = 0;

	response->b_wptr           += DL_ERROR_ACK_SIZE;
	response->b_datap->db_type = M_PCPROTO;
}

/*
 * tsl_dl_unitdata_ind ()
 */

STATIC void
tsl_dl_unitdata_ind (q, mp)
queue_t *q;
mblk_t	*mp;
{
	dl_unitdata_ind_t	*p_dl;
	tsl_t				*p_slip;

	p_dl = (dl_unitdata_ind_t *) mp->b_wptr;
	p_dl->dl_primitive = DL_UNITDATA_IND;
	p_dl->dl_dest_addr_length = 0;
	p_dl->dl_dest_addr_offset = DL_UNITDATA_IND_SIZE;
	p_dl->dl_src_addr_length = 0;
	p_dl->dl_src_addr_offset = p_dl->dl_dest_addr_offset + p_dl->dl_dest_addr_length;

	mp->b_wptr += DL_UNITDATA_IND_SIZE;
	mp->b_datap->db_type = M_PROTO;

	/* copy packet received to the next message block */

	p_slip = (tsl_t *) q->q_ptr;

	bcopy ((caddr_t)p_slip->bptr, (caddr_t)mp->b_cont->b_wptr, p_slip->inlen);

	mp->b_cont->b_wptr += p_slip->inlen;
}

/*
 * Notify daemon - returns zero if bufcall has been called (in which case
 * 		   the caller must put something on the queue which will reissue
 *		   the notify request later).
 *
 * A watchdog timeout is started in case the daemon never responds.
 */
STATIC int
tsl_notify_daemon(bufcall_q, p_slip, op)
queue_t	*bufcall_q;
tsl_t	*p_slip;
int	op;
{
	static volatile mblk_t	*resp = NULL;
	mblk_t	*resp2;
	tslipd_t	*slipd_p;
	int	p2len;
	pl_t	oldpri;

	if (tsl_daemon == NULL) {
		D_PRINTF(("slp: No daemon to notify\n");)
		return 1;
	}

        p2len = (p_slip->autodialp ? (strlen(p_slip->autodialp)+1) : 0);

	/* We need 2 buffers.  To avoid race/deadlock conditions, we
	 * reuse the first if we have to return to wait for the second.
	 */
 	oldpri = splstr ();

	if (resp == NULL) resp = allocb( sizeof(tslipd_t), BPRI_MED );

        if (resp == NULL || (resp2 = allocb( p2len, BPRI_MED ))==NULL ) {
		D_PRINTF(("slp: bufcall for daemon msg!!\n");)
		if (bufcall_q)
		    bufcall( sizeof(tslipd_t) + p2len,
			     BPRI_MED, qenable,(long)bufcall_q);
 		splx (oldpri);
		return 0;
	}

	/* Start failsafe timeout in case daemon is broken */
	tsl_cancel_notif_timeout(p_slip);
	p_slip->notif_toid = 
	    timeout(tsl_notif_timeout, (caddr_t)p_slip, notif_timeout_ticks);

	resp->b_datap->db_type = M_PCPROTO;
	slipd_p = (tslipd_t *) resp->b_rptr;
	resp->b_wptr += sizeof (tslipd_t);

	slipd_p-> op     = op;
	slipd_p-> pid    = p_slip-> pid;
	slipd_p-> lindex = p_slip-> lindex;
	slipd_p-> atime  = p_slip-> atime;
	slipd_p-> minor  = p_slip-> minor;
	bcopy( (caddr_t)p_slip->origifn, (caddr_t)slipd_p->origifn, 
						sizeof(slipd_p->origifn) );
	resp->b_cont = resp2;
	resp2->b_wptr += p2len;
	bcopy( (caddr_t)p_slip->autodialp, (caddr_t)resp2->b_rptr, p2len );

#ifdef DEBUG
	switch (op) {
	case SLPD_CONNREQ:
	  tsl_announce( p_slip, "CONNREQ %s m%ld l%d \"%s\"",
		  slipd_p->origifn, (long)slipd_p->minor, slipd_p->lindex,
		  (p2len==0 ? "<not set>" : (char *)resp2->b_rptr) );
	  break;
	case SLPD_TIMEOUT:
	  tsl_announce( p_slip, "TIMEOUT after %ld idle secs", 
			 (long)p_slip->idletime );
	  break;
	case SLPD_HANGUP:
	  tsl_announce( p_slip, "HANGUP");
	  break;
	default:
	  tsl_announce( p_slip, "BAD op=%d!", op );
	  break;
	}
#endif

	putnext (tsl_daemon->qtop, resp);
	resp = NULL;	/* don't re-use this message buffer */
 	splx (oldpri);

	return 1;
}

STATIC void
tsl_notif_timeout(p_slip)
tsl_t	*p_slip;
{
	pl_t oldpri = splstr ();

	p_slip -> notif_toid = 0;

	tsl_announce(p_slip, "NO RESPONSE FROM DAEMON after %ld secs\n",
		      NOTIF_TIMEOUT_US/1000000);

	if (p_slip->qbot) {
	    if (p_slip->hangup_pend) {
		/* hangup -- try again */
		queue_t *bufcall_q = RD(p_slip->qbot);
		tsl_notify_daemon(bufcall_q, p_slip, SLPD_HANGUP);
	    } 
 	    /* else timeout -- forget it */	
	} 
	else {
		/* connection request or wierd state - dump all packets */
		tsl_flushall(p_slip);
	}
 	splx( oldpri );
}

STATIC void
tsl_cancel_notif_timeout(p_slip)
tsl_t	*p_slip;
{
	pl_t oldpri = splstr ();
	if (p_slip-> notif_toid) {
		untimeout( p_slip-> notif_toid );
		p_slip-> notif_toid = 0;
	}
 	splx( oldpri );
}

/*
 * The daemon was unable to auto-dial and establish a connection
 * Flush queued packets and do nothing until more work arrives.
 */
STATIC void
tsl_flushall(p_slip)
tsl_t	*p_slip;
{
	queue_t	*urq, *uwq;

	if (p_slip->qbot) {
	    /*EMPTY*/
	    D_PRINTF(("tsl_flushall: minor %d is NOW CONNECTED!\n",
			   p_slip->minor);)
	} else {
	    if ((urq = p_slip->qtop) != NULL) {
		uwq = WR(urq);
		flushq(uwq, FLUSHALL);
		qenable(uwq);
#ifdef NOTDEF
It's unclear whether sending a flush up to IP will confuse it.  So don't.
		if (mp = allocb (1, BPRI_MED)) {
		    mp->b_datap->db_type = M_FLUSH;
		    mp->b_wptr++;
		    *mp->b_rptr = FLUSHRW;
		    putnext(urq, mp);
		}
#endif
	    } else {
		/*EMPTY*/
		D_PRINTF(("tsl_flushall: minor %d not open\n", p_slip->minor);)
	    }
	}
}

STATIC void
tsl_link(p_slip, lwq, lindex)
tsl_t	*p_slip;
queue_t	*lwq;
int	lindex;
{
	D_ANNOUNCE((p_slip, "link lindex %d", lindex);)
	D_ASSERT(p_slip->qbot==NULL, return; )

	p_slip->qbot = lwq;
	lwq->q_ptr = (char *) p_slip;
	OTHERQ (lwq)->q_ptr = (char *) p_slip;

	/* Toss any state from a previous connection */
	tsl_compress_init(&p_slip->slcomp);
	if ((p_slip->sc_flags & SC_AUTOCOMP) != 0) {
	    p_slip->sc_flags &= ~SC_COMPRESS;
	    p_slip->flags &= ~IFF_NOTRAILERS;
	}

	p_slip->lindex	 = lindex;
	p_slip->idleflag = 0;
	p_slip->enab_to_notif = 1;	/* enable idle timeout notification */

	tsl_cancel_notif_timeout(p_slip);

	/* Enable upper-write service in case waiting for auto-dial: */
	qenable (OTHERQ(p_slip->qtop));
	p_slip->qt_blocked = 0;
}

STATIC void
tsl_unlink(qbot)
queue_t	*qbot;
{
	register tsl_t	*pp;

	D_ASSERT( OTHERQ(qbot)->q_ptr == qbot->q_ptr, return; )

	pp = qbot->q_ptr;
	if (pp) {
		D_ANNOUNCE((pp, "unlink lindex %d", pp->lindex);)

		D_ASSERT( pp->qbot == qbot,  /* else... */
			  ; )

		tsl_cancel_notif_timeout(pp);
		pp->hangup_pend = 0;
		pp->enab_to_notif = 0;
		pp->lindex = NULL_LINDEX;
		qbot->q_ptr = NULL;
		OTHERQ(qbot)->q_ptr = NULL;
		pp->qbot = NULL;
	}
}

/*
 * Periodically scan all slip devices, updating the "atime" (approx last-access
 * time) field if the stream has been used since the last check.  If a stream's
 * idle timeout interval has been exceeded, send a message to the auto-dial
 * daemon, which may unlink and close the connection.
 */
STATIC void /*ARGSUSED*/
tsl_idle_timeout( dummy )
char *dummy;
{
	register tsl_t *p_slip;
	time_t currtime;
	pl_t	 oldpri;

	tsl_idle_toid = 0;
	if (!tsl_daemon) return;
	drv_getparm(TIME, &currtime);

	oldpri = splstr ();

	for (p_slip=tsl_listhead; p_slip!=NULL; p_slip = p_slip->next) {
	    if (!p_slip->qtop) {
		tsl_announce(p_slip,"###tsl_idle_timeout:found qtop==NULL");
		continue;
	    }
	    if (p_slip == tsl_daemon) continue;
	    if (p_slip-> idletime) {
		/* link has an idle-timeout parameter assigned */
		if (!p_slip-> idleflag) {
		    p_slip-> idleflag = 1;
		    p_slip-> atime = currtime;
		}
		if ((p_slip->atime + p_slip->idletime) < currtime &&
		     p_slip-> enab_to_notif) {
		    /* idle time has expired, and no prior notification */
		    if (tsl_notify_daemon(NULL, p_slip, SLPD_TIMEOUT))
			p_slip -> enab_to_notif = 0;
		}
	    }
	}
	splx (oldpri);
	tsl_start_timeout();
}

STATIC void
tsl_start_timeout()
{
	pl_t	 oldpri;
	oldpri = splstr ();
	if (tsl_idle_toid == 0 && tsl_daemon) {
	    tsl_idle_toid = timeout(tsl_idle_timeout, NULL, atime_gran_ticks);
	    if (tsl_idle_toid == 0)
	        printf("slp: TIMEOUT FAILED\n");
	}
	splx (oldpri);
}

STATIC void
tsl_announce (tsl_p, fmt, a,b,c,d,e,f,g)
tsl_t *tsl_p;
char *fmt;
char *a, *b, *c, *d, *e, *f, *g;
{
#ifdef DEBUG
	/* Prepend 3-digit timestamp (seconds) */
	static time_t starttime = 0;
	time_t currtime;
	drv_getparm(TIME, &currtime);
	if (starttime==0) starttime=currtime;
	printf("%d ", ((int)currtime-starttime) % 1000);
#endif
	printf("%s[%d]: ", driver_name, tsl_p->minor);
	if (fmt)
	    printf(fmt,a,b,c,d,e,f,g);
	printf("\n");
}

#ifdef DEBUG
STATIC void
tsl_d_assert(line)
{
	printf("slp: ASSERT FAIL - line %d\n",line);
}
#endif

#ifdef DEBUG
STATIC void
tsl_printf_ipadd(struct in_addr *a, u_short port)
{
	long ipadd;
	ipadd = ntohl(a->s_addr);
	printf("%d.%d.%d.%d", 
		(ipadd>>24) & 0xFF, (ipadd>>16) & 0xFF, 
		(ipadd>>8 ) & 0xFF, (ipadd    ) & 0xFF);
	if ((u_short)port != (u_short)~0) {
	    port = ntohs(port);
	    switch(port) {
	    case 20:  printf (":ftp-data"); break;
	    case 21:  printf (":ftp"); break;
	    case 23:  printf (":telnet"); break;
	    case 25:  printf (":smtp"); break;
	    case 53:  printf (":domain"); break;
	    case 109: printf (":pop-2"); break;
	    case 110: printf (":pop-3"); break;
	    default:  printf(":%d", port);
	    }
	}
	printf(" ");
}

STATIC void 
tsl_trace_ip_pkt(struct ip *ip)
{
	struct tcphdr *tcp;
	struct udphdr *udp;
	struct icmp *icmp;

	switch(ip->ip_p) {
	case IPPROTO_TCP:
	    tcp = (struct tcphdr *)((char *)ip + sizeof(struct ip));
	    tsl_printf_ipadd(&ip->ip_src, tcp->th_sport);
	    tsl_printf_ipadd(&ip->ip_dst, tcp->th_dport);
	    printf("TCP f=%d\n", tcp->th_flags);
	    break;
	case IPPROTO_UDP:
	    udp = (struct udphdr *)((char *)ip + sizeof(struct ip));
	    tsl_printf_ipadd(&ip->ip_src, udp->uh_sport);
	    tsl_printf_ipadd(&ip->ip_dst, udp->uh_dport);
	    printf("UDP ul=%d\n", ntohs(udp->uh_ulen));
	    break;
	case IPPROTO_ICMP:
	    icmp = (struct icmp *)((char *)ip + sizeof(struct ip));
	    tsl_printf_ipadd(&ip->ip_src, ~0);
	    tsl_printf_ipadd(&ip->ip_dst, ~0);
	    printf("ICMP type=%d code=%d\n", icmp->icmp_type, icmp->icmp_code);
	    break;
	default:
	    tsl_printf_ipadd(&ip->ip_src, ~0);
	    tsl_printf_ipadd(&ip->ip_dst, ~0);
	    printf("?ip? p=%d\n", ip->ip_p);
	    break;
	}
}
#endif
