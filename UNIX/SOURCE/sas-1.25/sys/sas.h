/* $Header: /home/roman/src/fas/sas/RCS/sas.h,v 1.25 1992/03/14 03:02:00 roman Exp $ */
/*
 * $Log: sas.h,v $
 * Revision 1.25  1992/03/14  03:02:00  roman
 * all rx/tx buffers are now mblk_t
 *
 * Revision 1.11  1992/03/05  00:16:05  roman
 * changed rx fifo level from 4 to 8
 * to reduces rx interuppts at high speed
 *
 * Revision 1.1  1992/02/17  17:05:01  roman
 * Initial revision
 *
 *
 */

/* This file contains various defines for the SAS async driver.
   If you change anything here you have to recompile the driver module.
*/

#if !defined (M_I286)
#ident	"@(#)sas.h	2.09"
#endif

/* Uncomment the following line if you need asyputchar and asygetchar.
   This is only required if you link the kernel without the original
   asy driver and these functions aren't provided by any other kernel
   module.
*/
/* #define NEED_PUT_GETCHAR	/* */
#define STRID_SAS 97

/* Uncomment the following line if you have VP/ix support in the
   kernel.
*/
/* #define HAVE_VPIX	/* */

/* Uncomment the following line if you need init8250. DosMerge needs
   this function, but only if you link the kernel without the original
   asy driver.
*/
/* #define NEED_INIT8250	/* */

#if defined (VPIX)
#undef VPIX
#endif

#if defined (HAVE_VPIX)
#define VPIX
#endif

#include <sys/param.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/buf.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/errno.h>
#include <sys/conf.h>
#include <sys/sysinfo.h>
#include <sys/file.h>
#include <sys/termio.h>
#include <sys/ioctl.h>
/* #include <macros.h>
#include <sys/stat.h>
*/
#if defined (HAVE_VPIX)
#include <sys/tss.h>
#include <sys/immu.h>
#include <sys/region.h>
#include <sys/proc.h>
#include <sys/v86.h>
#endif

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/termios.h>
#include <sys/termiox.h>
#ifdef SCO
#include <sys/tty.h>
#else
#include <sys/euc.h>
#include <sys/eucioctl.h>
#include <sys/strtty.h>
#endif
#include <sys/stermio.h>
#include <sys/jioctl.h>
#include <sys/cmn_err.h>


#if defined (TRUE)
#undef TRUE
#endif
#define	TRUE	(1)

#if defined (FALSE)
#undef FALSE
#endif
#define FALSE	(0)

/* Initial line control register.  Value will only be meaningful for
   asyputchar and asygetchar and they are only meaningful if
   NEED_PUT_GETCHAR is defined.
*/
#define	INITIAL_LINE_CONTROL	LC_WORDLEN_8

/* Initial baud rate.  Value will only be meaningful for
   asyputchar and asygetchar and they are only meaningful if
   NEED_PUT_GETCHAR is defined.
*/
#define INITIAL_BAUD_RATE	(BAUD_BASE/9600)

/* Initial modem control register.  This should probably not have to
   be touched.  It is here because some terminals used as the console
   require one or more of the modem signals set. It is only meaningful
   for asyputchar and asygetchar and they are only meaningful if
   NEED_PUT_GETCHAR is defined.
*/
#define INITIAL_MDM_CONTROL	0

/****************************************************/
/* Nothing past this line should have to be changed */
/****************************************************/

#define NUM_INT_VECTORS	32	/* number of possible int vectors, but
				   only the first eight are normally used
				*/

#define MAX_UNITS	16	/* we will only use that many units */

/* Miscellaneous Constants */

#define BAUD_BASE	(1843200 / 16)	/* 115200 bps */
#define HANGUP_DELAY	500		/* in milli-seconds */
#define HANGUP_TIME	1000		/* in milli-seconds */
#define BREAK_TIME	250		/* in milli-seconds */
#define EVENT_TIME	80		/* in milli-seconds */
#define	RECV_BUFF_SIZE	2048		/* receiver mblk size (MAX) */
#define RECV_SBUF_SIZE	2		/* receiver service control size */
#define NUM_RECV_BLKS	4		/* # of receive buffer to allocate */
#define MAX_VPIX_FILL	64		/* read buffer max VP/ix fill level */
#define MIN_READ_CHUNK	32		/* must be <= MAX_????_FILL/2 */
#define READ_PORT	0x0100		/* read command for sas_init_seq */
#define SOFT_INIT	0		/* init registers if cflag changed */
#define HARD_INIT	1		/* init registers w/o checking cflag */
#define B57600SAS	B0		/* high speed baud rate */
#define SPLWRK		spl6		/* SPL for character processing */
#define SPLINT		spltty		/* SPL to disable SAS interrupts */

#if ((EVENT_TIME) * (HZ) / 1000) == 0
#undef EVENT_TIME
#define EVENT_TIME	(1000 / (HZ) + 1)
#endif

#if (MAX_UNIX_FILL) > (TTYHOG)
#undef MAX_UNIX_FILL
#define MAX_UNIX_FILL	(TTYHOG)
#endif

#if (MAX_VPIX_FILL) > (TTYHOG)
#undef MAX_VPIX_FILL
#define MAX_VPIX_FILL	(TTYHOG)
#endif

#if (MIN_READ_CHUNK) > ((MAX_UNIX_FILL) / 2)
#undef MIN_READ_CHUNK
#define MIN_READ_CHUNK	((MAX_UNIX_FILL) / 2)
#endif

#if (MIN_READ_CHUNK) > ((MAX_VPIX_FILL) / 2)
#undef MIN_READ_CHUNK
#define MIN_READ_CHUNK	((MAX_VPIX_FILL) / 2)
#endif

#if (FAST_TTHIWAT) > ((TTYHOG) * 8 / 10)
#undef FAST_TTHIWAT
#undef FAST_TTLOWAT
#define FAST_TTHIWAT	((TTYHOG) * 8 / 10)	/* 80% */
#define FAST_TTLOWAT	((TTYHOG) * 4 / 10)	/* 40% */
#endif

#define MAX_INPUT_FIFO_SIZE	INPUT_NS_FIFO_SIZE
#define MAX_OUTPUT_FIFO_SIZE	OUTPUT_NS_FIFO_SIZE


/* Here are the modem control flags for the sas_modem array in space.c.
   They are arranged in three 8-bit masks which are combined to a 32-bit
   word. Each of these 32-bit words represents one entry in the sas_modem
   array.

   The lowest byte is used as a mask to manipulate the modem control
   register for modem disable. Use the MC_* macros to build the mask.

   The second lowest byte is used as a mask to manipulate the modem control
   register for modem enable during dialout. Use the MC_* macros to build
   the mask and shift them 8 bits to the left.

   The second highest byte is used as a mask to manipulate the modem control
   register for modem enable during dialin. Use the MC_* macros to build
   the mask and shift them 16 bits to the left.

   The highest byte is used to mask signals from the modem status
   register that will be used as the carrier detect signal. Use the MS_*
   macros to build the mask and shift them 24 bits to the left. If you use
   more than one signal, carrier is considered on only when all signals
   are on.

   Here are some useful macros for the space.c file. You may create your
   own macros if you have some special requirements not met by the
   predefined ones.
*/

/* modem disable (choose one) */
#define DI_RTS			((ulong) MC_SET_RTS)
#define DI_DTR			((ulong) MC_SET_DTR)
#define DI_RTS_AND_DTR		((ulong) (MC_SET_RTS | MC_SET_DTR))

/* modem enable for dialout (choose one) */
#define EO_RTS			((ulong) MC_SET_RTS << 8)
#define EO_DTR			((ulong) MC_SET_DTR << 8)
#define EO_RTS_AND_DTR		((ulong) (MC_SET_RTS | MC_SET_DTR) << 8)

/* modem enable for dialin (choose one) */
#define EI_RTS			((ulong) MC_SET_RTS << 16)
#define EI_DTR			((ulong) MC_SET_DTR << 16)
#define EI_RTS_AND_DTR		((ulong) (MC_SET_RTS | MC_SET_DTR) << 16)

/* carrier detect signal (choose one) */
#define CA_DCD			((ulong) MS_DCD_PRESENT << 24)
#define CA_CTS			((ulong) MS_CTS_PRESENT << 24)
#define CA_DSR			((ulong) MS_DSR_PRESENT << 24)


/* Here are the hardware handshake flags for the sas_flow array in space.c.
   They are arranged in three 8-bit masks which are combined to a 32-bit
   word. Each of these 32-bit words represents one entry in the sas_flow
   array.

   The lowest byte is used as a mask to manipulate the modem control
   register for input flow control. Use the MC_* macros to build the mask.

   The second lowest byte is used to mask signals from the modem status
   register that will be used for output flow control. Use the MS_* macros
   to build the mask and shift them 8 bits to the left. If you use more
   than one signal, output is allowed only when all signals are on.

   The second highest byte is used to mask signals from the modem status
   register that will be used to enable the output flow control selected
   by the second lowest byte. Use the MS_* macros to build the mask and
   shift them 16 bits to the left. If you use more than one signal, output
   flow control is enabled only when all signals are on.

   The highest byte is used as a mask to manipulate the modem control
   register for output half duplex flow control. Use the MC_* macros to
   build the mask and shift them 24 bits to the left.

   Here are some useful macros for the space.c file. You may create your
   own macros if you have some special requirements not met by the
   predefined ones.
*/

/* input flow control (choose one) */
#define HI_RTS			((ulong) MC_SET_RTS)
#define HI_DTR			((ulong) MC_SET_DTR)
#define HI_RTS_AND_DTR		((ulong) (MC_SET_RTS | MC_SET_DTR))

/* output flow control (choose one) */
#define HO_CTS			((ulong) MS_CTS_PRESENT << 8)
#define HO_DSR			((ulong) MS_DSR_PRESENT << 8)
#define HO_CTS_AND_DSR		((ulong) (MS_CTS_PRESENT | MS_DSR_PRESENT) \
					<< 8)
#define HO_CTS_ON_DSR		(((ulong) MS_CTS_PRESENT << 8) \
				| ((ulong) MS_DSR_PRESENT << 16))
#define HO_CTS_ON_DSR_AND_DCD	(((ulong) MS_CTS_PRESENT << 8) \
				| ((ulong) (MS_DSR_PRESENT | MS_DCD_PRESENT) \
					<< 16)) 

/* output hdx flow control (choose one) */
#define HX_RTS			((ulong) MC_SET_RTS << 24)
#define HX_DTR			((ulong) MC_SET_DTR << 24)
#define HX_RTS_AND_DTR		((ulong) (MC_SET_RTS | MC_SET_DTR) << 24)


/* define the local open flags */

#define OS_DEVICE_CLOSED	0x0000
#define OS_OPEN_FOR_DIALOUT	0x0001
#define OS_OPEN_FOR_GETTY	0x0002
#define OS_WAIT_OPEN		0x0004
#define OS_NO_DIALOUT		0x0008
#define OS_FAKE_CARR_ON		0x0010
#define OS_CLOCAL		0x0020
#define OS_HWO_HANDSHAKE	0x0040
#define OS_HWI_HANDSHAKE	0x0080
#define OS_HDX_HANDSHAKE	0x0100
#define OS_EXCLUSIVE_OPEN_1	0x0200
#define OS_EXCLUSIVE_OPEN_2	0x0400	/* SYSV 3.2 Xenix compatibility */

#define OS_OPEN_STATES		(OS_OPEN_FOR_DIALOUT | OS_OPEN_FOR_GETTY)
#define OS_TEST_MASK		(OS_OPEN_FOR_DIALOUT | OS_NO_DIALOUT \
				| OS_FAKE_CARR_ON | OS_CLOCAL \
				| OS_HWO_HANDSHAKE | OS_HWI_HANDSHAKE \
				| OS_HDX_HANDSHAKE | OS_EXCLUSIVE_OPEN_1 \
				| OS_EXCLUSIVE_OPEN_2)
#define OS_SU_TEST_MASK		(OS_OPEN_FOR_DIALOUT | OS_NO_DIALOUT \
				| OS_FAKE_CARR_ON | OS_CLOCAL \
				| OS_HWO_HANDSHAKE | OS_HWI_HANDSHAKE \
				| OS_HDX_HANDSHAKE | OS_EXCLUSIVE_OPEN_1)

/* define the device status flags */

#define DF_DEVICE_CONFIGURED	0x0001	/* device is configured */
#define DF_CTL_FIRST		0x0002	/* write ctl port at first access */
#define DF_CTL_EVERY		0x0004	/* write ctl port at every access */
#define DF_DEVICE_OPEN		0x0008	/* physical device is open */
#define DF_DEVICE_LOCKED	0x0010	/* physical device locked */
#define DF_MODEM_ENABLED	0x0020	/* modem enabled */
#define DF_XMIT_BUSY		0x0040	/* transmitter busy */
#define DF_XMIT_BREAK		0x0080	/* transmitter sends break */
#define DF_XMIT_LOCKED		0x0100	/* transmitter locked against output */
#define DF_DO_HANGUP		0x0200	/* delayed hangup request */
#define DF_DO_BREAK		0x0400	/* delayed break request */
#define DF_GUARD_TIMEOUT	0x0800	/* protect last char from corruption */
#define DF_FIFO_DROP_MODE	0x1000	/* receiver trigger level is dropped */
#define DF_MSI_ENABLED		0x2000	/* modem status interrupts enabled */
#define DF_HIGH_SPEED		0x4000	/* use 57600 bps instead of 38400 bps */

/* define the flow control status flags */

#define FF_HWO_HANDSHAKE	0x0001	/* output hw handshake enabled */
#define FF_HWI_HANDSHAKE	0x0002	/* input hw handshake enabled */
#define FF_HDX_HANDSHAKE	0x0004	/* output hdx hw handshake enabled */
#define	FF_HWO_STOPPED		0x0008	/* output stopped by hw handshake */
#define FF_HWI_STOPPED		0x0010	/* input stopped by hw handshake */
#define FF_HDX_STARTED		0x0020	/* output buffer contains characters */
#define FF_SWO_STOPPED		0x0040	/* output stopped by sw flow control */
#define FF_SWI_STOPPED		0x0080	/* input stopped by sw flow control */
#define FF_SW_FC_REQ		0x0100	/* sw input flow control request */
#define FF_RXFER_STOPPED	0x0200	/* rxfer function stopped */

/* define the scheduled events flags */

#define EF_DO_RXFER		0x0001	/* rxfer function request */
#define EF_DO_XXFER		0x0002	/* xxfer function request */
#define EF_DO_BRKINT		0x0004	/* break int request */
#define EF_DO_MPROC		0x0008	/* mproc function request */
#define EF_SIGNAL_VPIX		0x0010	/* send pseudorupt to VP/ix */
#define EF_RESET_DELTA_BITS	0x0020	/* reset accumulated msr delta bits */

/* define the device types */

#define NUMBER_OF_TYPES		3	/* adjust this to the highest */
					/* device type + 1 */
#define TYPE_NS16450		0
#define TYPE_I82510		1
#define TYPE_NS16550A		2

/* modifier flags that can be "ored" into the base port address in
   sas_port [] (`space.c')
*/

#define NO_FIFO		0x10000		/* force FIFOs off */
#define NO_TEST		0x20000		/* don't test the UART */
#define HIGH_SPEED	0x40000		/* use 57600 bps instead of 38400 bps */

/* define an easy way to reference the port structures */

#define RCV_DATA_PORT		(fip->port_0)
#define XMT_DATA_PORT		(fip->port_0)
#define INT_ENABLE_PORT		(fip->port_1)
#define INT_ID_PORT		(fip->port_2)
#define NS_FIFO_CTL_PORT	(fip->port_2)
#define I_BANK_PORT		(fip->port_2)
#define LINE_CTL_PORT		(fip->port_3)
#define MDM_CTL_PORT		(fip->port_4)
#define I_IDM_PORT		(fip->port_4)
#define LINE_STATUS_PORT	(fip->port_5)
#define I_RCM_PORT		(fip->port_5)
#define MDM_STATUS_PORT		(fip->port_6)
#define I_TCM_PORT		(fip->port_6)
#define DIVISOR_LSB_PORT	(fip->port_0)
#define DIVISOR_MSB_PORT	(fip->port_1)
#define CTL_PORT		(fip->ctl_port)

/* modem control port */

#define MC_SET_DTR		0x01
#define MC_SET_RTS		0x02
#define MC_SET_OUT1		0x04
#define MC_SET_OUT2		0x08	/* tristates int line when false */
#define MC_SET_LOOPBACK		0x10

#define MC_ANY_CONTROL	(MC_SET_DTR | MC_SET_RTS)

/* modem status port */

#define MS_CTS_DELTA		0x01
#define MS_DSR_DELTA		0x02
#define MS_RING_TEDGE		0x04
#define MS_DCD_DELTA		0x08
#define MS_CTS_PRESENT		0x10
#define MS_DSR_PRESENT		0x20
#define MS_RING_PRESENT		0x40
#define MS_DCD_PRESENT		0x80

#define MS_ANY_DELTA	(MS_CTS_DELTA | MS_DSR_DELTA | MS_RING_TEDGE \
				| MS_DCD_DELTA)
#define MS_ANY_PRESENT	(MS_CTS_PRESENT | MS_DSR_PRESENT | MS_RING_PRESENT \
				| MS_DCD_PRESENT)

/* interrupt enable port */

#define IE_NONE				0x00
#define	IE_RECV_DATA_AVAILABLE		0x01
#define	IE_XMIT_HOLDING_BUFFER_EMPTY	0x02
#define IE_LINE_STATUS			0x04
#define IE_MODEM_STATUS			0x08

#define IE_INIT_MODE	(IE_RECV_DATA_AVAILABLE | IE_XMIT_HOLDING_BUFFER_EMPTY \
			| IE_LINE_STATUS)

/* interrupt id port */

#define II_NO_INTS_PENDING	0x01
#define II_CODE_MASK		0x07
#define II_MODEM_STATE		0x00
#define II_XMTD_CHAR		0x02
#define II_RCVD_CHAR		0x04
#define II_RCV_ERROR		0x06
#define II_NS_FIFO_TIMEOUT	0x08
#define II_NS_FIFO_ENABLED	0xC0

/* line control port */

#define	LC_WORDLEN_MASK		0x03
#define	LC_WORDLEN_5		0x00
#define	LC_WORDLEN_6		0x01
#define	LC_WORDLEN_7		0x02
#define	LC_WORDLEN_8		0x03
#define LC_STOPBITS_LONG	0x04
#define LC_ENABLE_PARITY	0x08
#define LC_EVEN_PARITY		0x10
#define LC_STICK_PARITY		0x20
#define LC_SET_BREAK_LEVEL	0x40
#define LC_ENABLE_DIVISOR	0x80

/* line status port */

#define LS_RCV_AVAIL		0x01
#define LS_OVERRUN		0x02
#define LS_PARITY_ERROR		0x04
#define LS_FRAMING_ERROR	0x08
#define LS_BREAK_DETECTED	0x10
#define LS_XMIT_AVAIL		0x20
#define LS_XMIT_COMPLETE	0x40
#define LS_ERROR_IN_NS_FIFO	0x80	/* NS16550A only */
 
#define LS_RCV_INT	(LS_RCV_AVAIL | LS_OVERRUN | LS_PARITY_ERROR \
			| LS_FRAMING_ERROR | LS_BREAK_DETECTED)

/* fifo control port (NS16550A only) */

#define	NS_FIFO_ENABLE		0x01
#define	NS_FIFO_CLR_RECV	0x02
#define	NS_FIFO_CLR_XMIT	0x04
#define	NS_FIFO_START_DMA	0x08
#define NS_FIFO_SIZE_1		0x00
#define NS_FIFO_SIZE_4		0x40
#define NS_FIFO_SIZE_8		0x80
#define NS_FIFO_SIZE_14		0xC0
#define NS_FIFO_SIZE_MASK	0xC0

#define NS_FIFO_CLEAR_CMD	0
#define NS_FIFO_DROP_CMD	(NS_FIFO_SIZE_1 | NS_FIFO_ENABLE)
#define NS_FIFO_SETUP_CMD	(NS_FIFO_SIZE_8 | NS_FIFO_ENABLE)
#define NS_FIFO_INIT_CMD	(NS_FIFO_SETUP_CMD | NS_FIFO_CLR_RECV \
				| NS_FIFO_CLR_XMIT)

#define INPUT_NS_FIFO_SIZE	16
#define OUTPUT_NS_FIFO_SIZE	16

/* fifo control ports (i82510 only) */

#define I_BANK_0		0x00
#define I_BANK_1		0x20
#define I_BANK_2		0x40
#define I_BANK_3		0x60
#define I_FIFO_ENABLE		0x08
#define I_FIFO_CLR_RECV		0x30
#define I_FIFO_CLR_XMIT		0x0c

#define I_FIFO_CLEAR_CMD	0
#define I_FIFO_SETUP_CMD	I_FIFO_ENABLE

#define INPUT_I_FIFO_SIZE	4
#define OUTPUT_I_FIFO_SIZE	4

/* defines for ioctl calls (VP/ix) */

#define AIOC			('A'<<8)
#define AIOCINTTYPE		(AIOC|60)	/* set interrupt type */
#define AIOCDOSMODE		(AIOC|61)	/* set DOS mode */
#define AIOCNONDOSMODE		(AIOC|62)	/* reset DOS mode */
#define AIOCSERIALOUT		(AIOC|63)	/* serial device data write */
#define AIOCSERIALIN		(AIOC|64)	/* serial device data read */
#define AIOCSETSS		(AIOC|65)	/* set start/stop chars */
#define AIOCINFO		(AIOC|66)	/* tell us what device we are */

/* ioctl alternate names used by VP/ix */

#define VPC_SERIAL_DOS		AIOCDOSMODE
#define VPC_SERIAL_NONDOS	AIOCNONDOSMODE
#define VPC_SERIAL_INFO		AIOCINFO
#define VPC_SERIAL_OUT		AIOCSERIALOUT
#define VPC_SERIAL_IN		AIOCSERIALIN

/* serial in/out requests */

#define SO_DIVLLSB		1
#define SO_DIVLMSB		2
#define SO_LCR			3
#define SO_MCR			4
#define SI_MSR			1
#define SIO_MASK(x)		(1<<((x)-1))


/* This structure contains everything one would like to know about
   an open device.  There is one of it for each physical unit.

   We use several unions to eliminate most integer type conversions
   at run-time. The standard UNIX V 3.X/386 C compiler forces all
   operands in expressions and all function parameters to type int.
   To save some time, with the means of unions we deliver type int
   at the proper locations while dealing with the original type
   wherever int would be slower.

   This is highly compiler implementation specific. But for the sake
   of speed the end justifies the means.

   Take care that the size of the area that contains the various
   structure fields (up to, but excluding the ring buffers)
   is <= 128 bytes. Otherwise a 4-byte offset is used to access
   some of the structure fields. For the first 128 bytes a 1-byte
   offset is used, which is faster.
*/

struct fptr {
	dev_t dev;
	unsigned int sas_state;
	struct sas_info *fip;
	queue_t *rq;
};

struct	sas_info
{
	struct	sas_info *prev_int_user;/* link to previous sas_info struct */
	struct	sas_info *next_int_user;/* link to next sas_info struct */
	int	timeout_idx;	/* timeout index for untimeout () */
	uint	iflag;		/* current terminal input flags */
	uint	scflag;		/* current termios cflag */
	uint	cflag;		/* current terminal hardware control flags */
	uint	oflag;
	uint	lflag;
	union {			/* flags about the device state */
		ushort	s;
		uint	i;
	} device_flags;
	union {			/* flags about the flow control state */
		ushort	s;
		uint	i;
	} flow_flags;
	union {			/* flags about the scheduled events */
		ushort	s;
		uint	i;
	} event_flags;
	uint	o_state;	/* current open state */
	uint	po_state;	/* previous open state */
	union {			/* modem control masks */
		struct {
			unchar	di;	/* mask for modem disable */
			unchar	eo;	/* mask for modem enable (dialout) */
			unchar	ei;	/* mask for modem enable (dialin) */
			unchar	ca;	/* mask for carrier detect */
		} m;
		ulong	l;
	} modem;
	union {			/* hardware flow control masks */
		struct {
			unchar	ic;	/* control mask for inp. flow ctrl */
			unchar	oc;	/* control mask for outp. flow ctrl */
			unchar	oe;	/* enable mask for outp. flow ctrl */
			unchar	hc;	/* control mask for hdx flow ctrl */
		} m;
		ulong	l;
	} flow;
	unchar	msr;		/* modem status register value */
	unchar	new_msr;	/* new modem status register value */
	unchar	mcr;		/* modem control register value */
	unchar	lcr;		/* line control register value */
	unchar	ier;		/* interrupt enable register value */
	unchar	vec;		/* interrupt vector for this struct */
	unchar	device_type;	/* device type determined by sasinit() */
#if defined (HAVE_VPIX)
	unchar	v86_intmask;	/* VP/ix pseudorupt mask */
	v86_t	*v86_proc;	/* VP/ix v86proc pointer for pseudorupts */
	struct termss	v86_ss;	/* VP/ix start/stop characters */
#endif
	uint	ctl_port;	/* muliplexer control port */
	union {			/* uart port addresses and control values */
		uint	addr;
		struct {
			ushort	addr;
			unchar	ctl;
		} p;
	} port_0, port_1, port_2, port_3, port_4, port_5, port_6;
	uint	xmit_fifo_size;		/* transmitter fifo size */
	uint	recv_cnt;		/* characters in this mblk */
	uint	alloc_size;		/* size for receiver allocations */
	unsigned int *sas_curstate;	/* current state pointer */
	unsigned int sas_timerid;	/* timeout id */
	unsigned int unit;		/* unit number for this device */
	struct fptr sas_dev[2];		/* allow for getty opens */
	queue_t *rq;			/* Read queue for this device */
	mblk_t	*holdbuf;		/* Transmitter holding buffer */
	mblk_t	*free;			/* Free mblks for reader */
	int	nfree;			/* Number of free mblks for reader */
	mblk_t	*discards;		/* mblks not suitable for reader */
	mblk_t	*recv;			/* Receiver holding */
	mblk_t	*filled;		/* Filled receiver mblks */
	cc_t	cc[NCCS];		/* copy our special chars */
};

#define SAS_GETQ 0x4567

struct qval {
	int qsize;
	int holdsize;
};
