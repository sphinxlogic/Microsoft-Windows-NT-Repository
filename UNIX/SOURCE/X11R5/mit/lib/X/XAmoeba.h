/*
 * Xamoeba.h
 *
 * $XFree86: mit/lib/X/XAmoeba.h,v 1.1 1993/03/20 03:28:01 dawes Exp $
 *
 */
#define event __event
#define interval __interval

#include <amoeba.h>
#include <ampolicy.h>
#include <semaphore.h>
#include <cmdreg.h>
#include <stdcom.h>
#include <stderr.h>
#include <vc.h>
#include <circbuf.h>
#include <exception.h>
#include <fault.h>
#include <signal.h>
#include <module/signals.h>
#include <server/x11/Xamoeba.h>

#undef interval
#undef event

#define	MAX_TCPIP_RETRY	4
#define	CIRCBUFSIZE	1024

/*
 */
typedef struct _XAmChanDesc {
    int			state;		/* current state of connection */
    int			type;		/* type of connection */
    signum		signal;		/* signal to kill TCP/IP reader */
    semaphore		*sema;		/* select semaphore */
    struct vc		*virtcirc;	/* virtual circuit for Amoeba */
    struct circbuf	*circbuf;	/* circular buffer for TCP/IP */
    capability		chancap;	/* TCP/IP channel capability */
} XAmChanDesc;

/* Amoeba channel descriptor states */
#define	ACDS_FREE	0		/* unused */
#define	ACDS_USED	1		/* intermediate state */
#define	ACDS_CLOSED	2		/* just closed */

/* Amoeba channel types */
#define	ACDT_TCPIP	1		/* TCP/IP connection */
#define	ACDT_VIRTCIRC	2		/* Amoeba virtual circuit connection */

extern XAmChanDesc *XAmAllocChanDesc();
extern XAmChanDesc *XAmFdToChanDesc();
extern int XAmChanDescToFd();
extern void XAmFreeChanDesc();
