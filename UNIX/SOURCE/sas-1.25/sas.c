#ifndef lint
static char *rcsid = "$Header: /home/roman/src/fas/sas/RCS/sas.c,v 1.25 1992/03/14 03:00:02 roman Exp $";
#endif


/*
 * $Log: sas.c,v $
 * Revision 1.25  1992/03/14  03:00:02  roman
 * Major changes to streams/asy interface
 * addition of write / read service routines
 * all buffers are now mblk_t
 *
 * Revision 1.21  1992/03/05  00:12:21  roman
 * added code for termiox and TIOCM???
 * added 115200 Baud as 1800 with HIGH_SPEED defined
 * lost SCO compatibility (temporarily) unless we decide VR4 only
 *
 * Revision 1.2  1992/02/24  04:18:54  roman
 * Added input flow control code to rxfer
 * added SCO unix support
 *
 * Revision 1.1  1992/02/17  17:05:00  roman
 * Initial revision
 *
 *
 */

/* SAS Streams Async Solution driver for 386/486 versions of system V UNIX */

/* SAS was developed by
Roe Peterson		roe@rapfast.Unibase.SK.CA
    and
Roman Dumych 		roman@nyxis.Unibase.SK.CA

vast amounts of code from and many thanks to
Uwe Doering             INET : gemini@geminix.in-berlin.de
Billstedter Pfad 17 b   UUCP : ...!unido!fub!geminix.in-berlin.de!gemini
1000 Berlin 20
Germany
*/

#if !defined (M_I286)
#ident	"@(#)sas.c	1.20"
#endif

/* Note: This source code was quite heavily optimized for speed. You
         may wonder that register variables aren't used everywhere.
         This is because there is an overhead in memory accesses
         when using register variables. As you may know data accesses
         usually need much more wait states on the memory bus than
         code accesses (because of page or cache misses). Therefore,
         saving some data accesses has higher priority than saving
         code accesses.

         You may also note some not very elegant constructions that
         may be intentional because they are faster. If you want to
         make style improvements you should check the assembler output
         whether this wouldn't slow things down.

         Decisions for speed optimization were based on assembler
         listings produced by the standard UNIX V [34].X/386 C compiler.
*/

#include <sys/sas.h>
#if 0 /* !defined (NO_ASM) */
#include <sys/inline.h>
#endif

#if defined (SCO) 
#define asyputchar sioputchar
#define asygetchar siogetchar
#define WIOC	OASLP
#endif

#if defined (NO_ASM)
#define intr_disable()	old_level = SPLINT ()
#define intr_restore()	(void) splx (old_level)
#define REGVAR
#else
/* This is a terrible ugly kludge to speed up the `inb' and `outb'
   functions. I.e., originally, the `outb' inline function had an
   overhead of four data memory accesses for parameter passing. This
   parameter passing actually consumed more clock cycles than the
   assembler `outb' command itself. Although this solution can't
   prevent unnessessary register moves it limits them at least to
   register to register moves that are much faster. You need a
   line like the following in the declaration part of every
   function that uses `inb' or `outb' calls:

	REGVAR;

   This hack should work with every compiler that knows about the
   UNIX V [34].X/386 standard compiler's inline assembler directives.
*/

/* Mods by Greg Lehey, LEMIS, 22 January 1994
 * In fact, gcc has different syntax for this, so we need to make a decision:
 */

#ifdef __GNUC__

static inline void
outb (short port, char val)
{
  __asm__ volatile ("out%B0 (%1)"::"a" (val), "d" (port));
}

static inline unsigned char
inb (short port)
{
  unsigned int ret;
  __asm__ volatile ("in%B0 (%1)":"=a" (ret):"d" (port));
  return ret;
}

/* Create a dummy optim () function to keep the compiler happy */
inline void optim ()
{
  }

#else							    /* not __gnuc__ */
asm	void loadal (val)
{
%reg	val;
	movl	val,%eax
%mem	val;
	movb	val,%al
}

asm	void loaddx (val)
{
%reg	val;
	movl	val,%edx
%mem	val;
	movw	val,%dx
}

asm	int outbyte ()
{
	outb	(%dx)
}

asm	int inbyte ()
{
	xorl	%eax,%eax
	inb	(%dx)
}
/* The port parameter of the `outb' macro must be one of the predefined
   port macros from `sas.h' or a simple uint variable (no indirection
   is allowed). Additionally, `fip' must be a register variable in the
   functions where `outb' is used. This prevents the destruction of the
   `eax' CPU register while loading the `edx' register with the port
   address. This is highly compiler implementation specific.
*/
#define outb(port,val) (regvar = (port), loaddx (regvar), regvar = (val), loadal (regvar), outbyte ())

#define inb(port) (regvar = (port), loaddx (regvar), inbyte ())

/* This function inserts the address optimization assembler pseudo-op
   wherever called.
*/

asm	void optim ()
{
	.optim
}

#endif							    /* __gnuc__ */

#define REGVAR register uint	regvar

#if defined (OPTIM)	/* Define for uPort, ISC doesn't know about */
static void		/* `.optim', but has turned on optimization by */
dummy ()		/* default, so we don't need it there anyway. */
{
	optim ();
}
#endif
#endif	/* NO_ASM */

/* functions provided by this driver */
void		sasinit ();
int		sasopen ();
int		sasclose ();
int		sasread ();
int		saswrite ();
int		sasioctl ();
int		sasintr ();
#if defined (NEED_PUT_GETCHAR)
int		asyputchar ();
int		asygetchar ();
#endif
#if defined (NEED_INIT8250)
int		init8250 ();
#endif
static int	sas_proc ();
static void	sas_param ();
static void	sas_fproc ();
static void	sas_mproc ();
static uint	sas_rproc ();
static void	sas_xproc ();
static void	sas_event ();
static void	sas_link ();
#if defined (HAVE_VPIX)
static int	sas_vpix_sr ();
#endif
static void	sas_msi_disable ();
static void	sas_msi_enable ();
int	sas_rxfer ();
static void	sas_hdx_check ();
static void	sas_hangup ();
static void	sas_timeout ();
static void	sas_cmd ();
static void	sas_open_device ();
static void	sas_close_device ();
static uint	sas_make_ctl_val ();
static int	sas_tcset();
static int	sas_tcsets();
static int	sas_test_device ();
static int	sas_delay();
static int	sas_qenable();

/* external functions used by this driver */
extern int	SPLINT ();
extern int	SPLWRK ();
extern int	splx ();
extern int	sleep ();
extern int	wakeup ();
extern void	longjmp ();
extern int	timeout ();
extern int	untimeout ();
#if defined (HAVE_VPIX)
extern int	fubyte ();
extern int	subyte ();
extern int	v86setint ();
#endif
#if defined (NO_ASM)
extern int	inb ();
extern int	outb ();
#endif

/* the following stuff is defined in space.c */
extern uint	sas_physical_units;
extern ulong	sas_port [];
extern uint	sas_vec [];
extern uint	sas_init_seq [];
extern uint	sas_mcb [];
extern ulong	sas_modem [];
extern ulong	sas_flow [];
extern uint	sas_ctl_port [];
extern uint	sas_ctl_val [];
extern uint	sas_int_ack_port [];
extern uint	sas_int_ack [];
extern struct sas_info	sas_info [];
extern struct sas_info	*sas_info_ptr [];
/* end of space.c references */

/* Forward definitions of static functions */
static int sas_wsrv (queue_t*q);
static sas_intioctl (struct sas_info *fip, mblk_t *bp);
static int sas_buffers (struct sas_info *fip, int flag);

/* sas_is_initted
   Flag to indicate that we have been thru init.
   This is realy only necessary for systems that use asyputchar
   and asygetchar but it doesn't hurt to have it anyway.
*/
static int	sas_is_initted = FALSE;

/* event_scheduled
   Flag to indicate that the event handler has been scheduled
   via the timeout() function.
*/
static int	event_scheduled = FALSE;

/* array of pointers to the first sas_info structure for each
   interrupt vector
*/
static struct sas_info	*sas_first_int_user [NUM_INT_VECTORS];

/* counters for receiver overruns, each UART type has its own counter
   indexed by the device type
*/
static uint	sas_overrun [NUMBER_OF_TYPES];

/* counter for temporarily disabled modem status interrupts due
   to noise on the modem status lines
*/
static uint	sas_msi_noise;

/* UNIX to SAS mapping of baud rates (normal speed) */
static uint	sas_baud [CBAUD + 1] =
{	B38400,		B50,
	B75,		B110,
	B134,		B150,
	B200,		B300,
	B600,		B1200,
	B1800,		B2400,
	B4800,		B9600,
	B19200,		B38400
};

/* UNIX to SAS mapping of baud rates (high speed) */
static uint	sas_hbaud [CBAUD + 1] =
{	B0,		B50,
	B75,		B110,
	B134,		B150,
	B200,		B300,
	B600,		B1200,
	B1800,		B2400,
	B4800,		B9600,
	B19200,		B57600SAS
};

/* Note: The baud rate in sas_hbaud that is overridden by B57600SAS
         has to be entered at sas_baud [0] !

   Note also: If you want to use a speed different from B38400 for
              B57600SAS, you have to make sure that the tthiwat [] and
              ttlowat [] values for that baud rate are high enough
              to allow the CLIST function to keep the SAS transmit
              buffer filled. Look at sasinit () on how to do that.
*/

/* the divisor values for the various baud rates */
static uint	sas_speeds [CBAUD + 1] =
{	BAUD_BASE/57600,	BAUD_BASE/50,
	BAUD_BASE/75,		BAUD_BASE/110,
	(2*BAUD_BASE+134)/269,	BAUD_BASE/150,
	BAUD_BASE/200,		BAUD_BASE/300,
	BAUD_BASE/600,		BAUD_BASE/1200,
	BAUD_BASE/1800,		BAUD_BASE/2400,
	BAUD_BASE/4800,		BAUD_BASE/9600,
	BAUD_BASE/19200,	BAUD_BASE/38400
};

/* time for one character to completely leave the transmitter shift register */
static uint	sas_ctimes [CBAUD + 1] =
{	HZ*15/57600+2,	HZ*15/50+2,
	HZ*15/75+2,	HZ*15/110+2,
	HZ*30/269+2,	HZ*15/150+2,
	HZ*15/200+2,	HZ*15/300+2,
	HZ*15/600+2,	HZ*15/1200+2,
	HZ*15/1800+2,	HZ*15/2400+2,
	HZ*15/4800+2,	HZ*15/9600+2,
	HZ*15/19200+2,	HZ*15/38400+2
};
/* buffer sizes for allocation of streams resources */
static uint	sas_bsizes [CBAUD + 1] =
{
	400,	64,
	64,	64,
	64,	64,
	64,	64,
	64,	64,
	64,	64,
	64,	64,
	128,	256
};
/* lookup table for minor device number -> open mode flags translation */
static uint	sas_open_modes [16] =
{
	OS_OPEN_FOR_DIALOUT | OS_FAKE_CARR_ON | OS_CLOCAL,
	OS_OPEN_FOR_DIALOUT | OS_FAKE_CARR_ON | OS_CLOCAL | OS_HWO_HANDSHAKE
							| OS_HWI_HANDSHAKE,
	OS_OPEN_FOR_DIALOUT | OS_FAKE_CARR_ON | OS_CLOCAL | OS_HWO_HANDSHAKE,
	OS_OPEN_FOR_DIALOUT | OS_FAKE_CARR_ON | OS_CLOCAL | OS_HWO_HANDSHAKE
							| OS_HDX_HANDSHAKE,
	OS_OPEN_FOR_DIALOUT | OS_FAKE_CARR_ON,
	OS_OPEN_FOR_DIALOUT | OS_FAKE_CARR_ON | OS_HWO_HANDSHAKE
						| OS_HWI_HANDSHAKE,
	OS_OPEN_FOR_DIALOUT | OS_FAKE_CARR_ON | OS_HWO_HANDSHAKE,
	OS_OPEN_FOR_DIALOUT | OS_FAKE_CARR_ON | OS_HWO_HANDSHAKE
						| OS_HDX_HANDSHAKE,
	OS_OPEN_FOR_GETTY | OS_WAIT_OPEN | OS_NO_DIALOUT,
	OS_OPEN_FOR_GETTY | OS_WAIT_OPEN | OS_NO_DIALOUT | OS_HWO_HANDSHAKE
							| OS_HWI_HANDSHAKE,
	OS_OPEN_FOR_GETTY | OS_WAIT_OPEN | OS_NO_DIALOUT | OS_HWO_HANDSHAKE,
	OS_OPEN_FOR_GETTY | OS_WAIT_OPEN | OS_NO_DIALOUT | OS_HWO_HANDSHAKE
							| OS_HDX_HANDSHAKE,
	OS_OPEN_FOR_GETTY | OS_WAIT_OPEN,
	OS_OPEN_FOR_GETTY | OS_WAIT_OPEN | OS_HWO_HANDSHAKE
						| OS_HWI_HANDSHAKE,
	OS_OPEN_FOR_GETTY | OS_WAIT_OPEN | OS_HWO_HANDSHAKE,
	OS_OPEN_FOR_GETTY | OS_WAIT_OPEN | OS_HWO_HANDSHAKE
						| OS_HDX_HANDSHAKE
};

/* The following defines are used to access multiplexed ports. */
#define GET_PORT(port,num) \
	((fip->device_flags.i & DF_CTL_EVERY)\
			? (port)\
			: (port) + (num))

#define sas_first_ctl(fip,port) \
	((void) (((fip)->device_flags.i & DF_CTL_FIRST)\
			? outb (CTL_PORT, (port).p.ctl)\
			: 0))

#define sas_ctl(fip,port) \
	((void) (((fip)->device_flags.i & (DF_CTL_FIRST | DF_CTL_EVERY))\
			? outb (CTL_PORT, (port).p.ctl)\
			: 0))

#define sas_first_outb(fip,port,val) \
	((void) (((fip)->device_flags.i & (DF_CTL_FIRST | DF_CTL_EVERY))\
			? outb (CTL_PORT, (port).p.ctl)\
			: 0),\
		(void) outb ((port).addr, (val)))

#define sas_outb(fip,port,val) \
	((void) (((fip)->device_flags.i & DF_CTL_EVERY)\
			? outb (CTL_PORT, (port).p.ctl)\
			: 0),\
		(void) outb ((port).addr, (val)))

#define sas_first_inb(fip,port) \
	((void) (((fip)->device_flags.i & (DF_CTL_FIRST | DF_CTL_EVERY))\
			? outb (CTL_PORT, (port).p.ctl)\
			: 0),\
		inb ((port).addr))

#define sas_inb(fip,port) \
	((void) (((fip)->device_flags.i & DF_CTL_EVERY)\
			? outb (CTL_PORT, (port).p.ctl)\
			: 0),\
		inb ((port).addr))

/* The following defines are used to take apart the minor device numbers. */
#define GET_UNIT(dev)		((dev) & 0x0f)
#define GET_OPEN_MODE(dev)	(sas_open_modes [((dev) >> 4) & 0x0f])

/* lock device against concurrent use */
#define get_device_lock(fip,prio) \
{\
	/* sleep while device is used by an other process */\
	while ((fip)->device_flags.i & DF_DEVICE_LOCKED)\
		(void) sleep ((caddr_t) &(fip)->device_flags.i, (prio));\
	(fip)->device_flags.s |= DF_DEVICE_LOCKED;\
}

/* release device */
#define release_device_lock(fip) \
{\
	(fip)->device_flags.s &= ~DF_DEVICE_LOCKED;\
	/* wakeup the process that may wait for this device */\
	(void) wakeup ((caddr_t) &(fip)->device_flags.i);\
}

/* schedule event */
#define event_sched(fip,event) \
{\
	(fip)->event_flags.s |= (event);\
	if (!event_scheduled)\
	{\
		event_scheduled = TRUE;\
		(void) timeout (sas_event, (void *) NULL,\
				(EVENT_TIME) * (HZ) / 1000);\
	}\
}

#define STERMIO(bp) ((struct termio*)(bp)->b_cont->b_rptr)
#define STERMIOS(bp) ((struct termios *)(bp)->b_cont->b_rptr)
#define STERMIOX(bp) ((struct termiox *)(bp)->b_cont->b_rptr)
#define DEFFLAGS (CS8|SSPEED|HUPCL|CREAD)

#ifdef DEVFLAGAVAIL
# define FAILOPEN(arg)	return(arg)
#else
# define FAILOPEN(arg)	{ u.u_error = (arg); return(OPENFAIL); }
#endif

/*
 * Streams configuration stuff:
 */

int	sas_open(),
	sas_close(),
	sas_put(),
	sas_rsrv(),
	sas_wsrv();

static struct module_info sass_info = {
	STRID_SAS,
	"SAS",
	0,
	4096,
	100,
	50,
};


static struct qinit sas_rinit = {
	NULL, sas_rsrv, sas_open, sas_close, NULL, &sass_info, NULL
};

static struct qinit sas_winit = {
	sas_put, sas_wsrv, NULL, NULL, NULL, &sass_info, NULL
};

struct streamtab sasinfo = { &sas_rinit, &sas_winit, NULL, NULL };


/* sasinit
   This routine checks for the presense of the devices in the sas_port
   array and if the device is present tests and initializes it.
   During the initialization the device type is automatically determined
   and the UART is handled according to its requirements.
*/

void
sasinit ()
{
	REGVAR;
	register struct sas_info	*fip;
	register uint	unit;
	uint	logical_units, port, *seq_ptr;
	char	port_stat [MAX_UNITS + 1];

	if (sas_is_initted)
		return;

	sas_is_initted = TRUE;

	/* execute the init sequence for the serial card */
	for (seq_ptr = sas_init_seq; *seq_ptr; seq_ptr++)
	{
		port = *seq_ptr;
		seq_ptr++;
		if (*seq_ptr & READ_PORT)
			(void) inb (port);
		else
			(void) outb (port, *seq_ptr);
	}

	/* setup and initialize all serial ports */
	for (unit = 0; unit < sas_physical_units; unit++)
	{
		sas_info_ptr [unit] = fip = &sas_info [unit];
		fip->sas_dev[0].fip = fip->sas_dev[1].fip = fip;
		fip->sas_dev[0].rq = fip->sas_dev[1].rq = NULL;
		fip->sas_dev[0].sas_state = fip->sas_dev[1].sas_state = 0;
		fip->device_flags.i = 0;
		port_stat [unit] = '-';
		if (port = (uint) ((ushort) (sas_port [unit])))
		{
			/* check the int vector */
			if (sas_vec [unit] >= NUM_INT_VECTORS)
			{
				port_stat [unit] = '>';
				continue;
			}

			/* init all of its ports */
			if (sas_ctl_port [unit])
			{
				fip->ctl_port = sas_ctl_port [unit];

				if (sas_ctl_val [unit] & 0xff00)
					fip->device_flags.s |= DF_CTL_EVERY;
				else
					fip->device_flags.s |= DF_CTL_FIRST;
			}

			fip->holdbuf = NULL;
			fip->free = NULL;
			fip->discards = NULL;
			fip->recv = NULL;
			fip->filled = NULL;
			fip->port_0.p.addr = GET_PORT (port, 0);
			fip->port_1.p.addr = GET_PORT (port, 1);
			fip->port_2.p.addr = GET_PORT (port, 2);
			fip->port_3.p.addr = GET_PORT (port, 3);
			fip->port_4.p.addr = GET_PORT (port, 4);
			fip->port_5.p.addr = GET_PORT (port, 5);
			fip->port_6.p.addr = GET_PORT (port, 6);
			fip->port_0.p.ctl = sas_make_ctl_val (fip, unit, 0);
			fip->port_1.p.ctl = sas_make_ctl_val (fip, unit, 1);
			fip->port_2.p.ctl = sas_make_ctl_val (fip, unit, 2);
			fip->port_3.p.ctl = sas_make_ctl_val (fip, unit, 3);
			fip->port_4.p.ctl = sas_make_ctl_val (fip, unit, 4);
			fip->port_5.p.ctl = sas_make_ctl_val (fip, unit, 5);
			fip->port_6.p.ctl = sas_make_ctl_val (fip, unit, 6);
			fip->vec = sas_vec [unit];
			fip->unit = unit;

			fip->ier = IE_NONE;	/* disable all ints */
			sas_first_outb (fip, INT_ENABLE_PORT, fip->ier);

			/* is there a serial chip ? */
			if (sas_inb (fip, INT_ENABLE_PORT) != fip->ier)
			{
				port_stat [unit] = '?';
				continue;	/* a hardware error */
			}

			/* test the chip thoroughly */
			if (!(sas_port [unit] & NO_TEST)
				&& (port_stat [unit]
						= (sas_test_device (fip) + '0'))
					!= '0')
			{
				continue;	/* a hardware error */
			}

			fip->lcr = 0;
			sas_outb (fip, LINE_CTL_PORT, fip->lcr);
			fip->mcr = sas_mcb [unit] | fip->modem.m.di;
			sas_outb (fip, MDM_CTL_PORT, fip->mcr);

			fip->device_type = TYPE_NS16450;
			fip->xmit_fifo_size = 1;
			port_stat [unit] = '*';

			/* let's see if it's an NS16550A */
			sas_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_INIT_CMD);
			if (!(~sas_inb (fip, INT_ID_PORT) & II_NS_FIFO_ENABLED))
			{
				fip->device_type = TYPE_NS16550A;
				fip->xmit_fifo_size = OUTPUT_NS_FIFO_SIZE;
				port_stat [unit] = 'F';
				sas_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_CLEAR_CMD);
			}
			else
			{
				sas_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_CLEAR_CMD);
				/* or is it an i82510 ? */
				sas_outb (fip, I_BANK_PORT, I_BANK_2);
				if (!(~sas_inb (fip, I_BANK_PORT) & I_BANK_2))
				{
					fip->device_type = TYPE_I82510;
					fip->xmit_fifo_size = OUTPUT_I_FIFO_SIZE;
					port_stat [unit] = 'f';
					sas_outb (fip, I_BANK_PORT, I_BANK_1);
					sas_outb (fip, I_TCM_PORT, I_FIFO_CLR_XMIT);
					sas_outb (fip, I_RCM_PORT, I_FIFO_CLR_RECV);
				}
				sas_outb (fip, I_BANK_PORT, I_BANK_0);
			}

			/* disable FIFOs if requested in space.c */
			if ((sas_port [unit] & NO_FIFO)
					&& (fip->device_type != TYPE_NS16450))
			{
				fip->device_type = TYPE_NS16450;
				fip->xmit_fifo_size = 1;
				port_stat [unit] = '+';
			}

			/* clear potential interrupts */
			(void) sas_inb (fip, MDM_STATUS_PORT);
			(void) sas_inb (fip, RCV_DATA_PORT);
			(void) sas_inb (fip, RCV_DATA_PORT);
			(void) sas_inb (fip, LINE_STATUS_PORT);
			(void) sas_inb (fip, INT_ID_PORT);
			if (port = sas_int_ack_port [fip->vec])
				(void) outb (port, sas_int_ack [fip->vec]);

			/* do we want to use 57600 bps ? */
			if (sas_port [unit] & HIGH_SPEED) {
				fip->device_flags.s |= DF_HIGH_SPEED;
				sas_speeds[B1800] = 1;
				sas_ctimes[B1800] = 1;
				sas_bsizes[B1800] = 512;
			}

			/* show that it is present and configured */
			fip->device_flags.s |= DF_DEVICE_CONFIGURED;
		}
	}

#if defined (NEED_PUT_GETCHAR)
	fip = &sas_info [0];
	fip->mcr &= ~fip->modem.m.di;
	fip->mcr |= INITIAL_MDM_CONTROL;
	sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);

	fip->lcr = INITIAL_LINE_CONTROL;
	sas_outb (fip, LINE_CTL_PORT, fip->lcr | LC_ENABLE_DIVISOR);
	sas_outb (fip, DIVISOR_LSB_PORT, INITIAL_BAUD_RATE);
	sas_outb (fip, DIVISOR_MSB_PORT, (INITIAL_BAUD_RATE) >> 8);
	sas_outb (fip, LINE_CTL_PORT, fip->lcr);
#endif

#if defined (SCO) 
	for (unit = 0; unit < sas_physical_units; unit++)
		(void) printcfg ("sas", (uint) ((ushort) (sas_port [unit])), 7,
					sas_vec [unit], -1,
					"unit=%d type=%c release=1.25",
					unit, port_stat [unit]);
#else
	port_stat [unit] = '\0';
	(void) printf ("\nStreams Async Solution 1.25: Unit 0-%d init state is [%s]\n\n",
			unit - 1,
			port_stat);
#endif
	return;
}

/* Open a tty line. This function is called for every open, as opposed
   to the sasclose function which is called only with the last close.
*/
int
sas_open (rq,dev,flag,sflag)
queue_t *rq;
int	dev;
int	flag,sflag;
{
	register struct sas_info	*fip;
	unsigned int *ttyp;
	register uint	open_mode;
	uint	physical_unit;
	int	have_lock;
	int	old_level;

	if(sflag) FAILOPEN(EINVAL);			/* no clone open */

	physical_unit = GET_UNIT (dev);

	/* check for valid port number */
	if (physical_unit >= sas_physical_units) FAILOPEN(ENXIO);

	fip = sas_info_ptr [physical_unit];

	/* was the port present at init time ? */
	if (!(fip->device_flags.i & DF_DEVICE_CONFIGURED)) FAILOPEN(ENXIO);

	open_mode = GET_OPEN_MODE (dev);
	have_lock = FALSE;
	old_level = SPLINT ();

	/* loop until we've got the device lock and, owning the lock, we've
	   checked whether the current open mode permits us to open the
	   device
	*/
	for (;;)
	{
		/* If this is a getty open, the FNDELAY flag is not set,
		   and the device is already open for dialout or it is
		   open and there is no carrier, wait until device
		   is closed.
		*/
		while ((open_mode & OS_OPEN_FOR_GETTY) && !(flag & FNDELAY)
				&& ((fip->o_state & OS_OPEN_FOR_DIALOUT)
					|| ((fip->o_state & OS_OPEN_FOR_GETTY)
						&& !(*fip->sas_curstate
								& CARR_ON))))
		{
			if (have_lock)
			{
				release_device_lock (fip);
				have_lock = FALSE;
			}
			(void) sleep ((caddr_t) &fip->o_state, TTIPRI);
		}
		
		/* If the device is already open and another open uses a
		   different open mode or if a getty open waits for carrier
		   and doesn't allow parallel dialout opens, return with
		   EBUSY error.
		*/
		if ((fip->o_state & ((open_mode & OS_OPEN_FOR_GETTY)
					? (OS_OPEN_STATES | OS_WAIT_OPEN)
					: (OS_OPEN_STATES | OS_NO_DIALOUT)))
			&& ((flag & FEXCL)
				|| ((open_mode ^ fip->o_state) & (u.u_uid
							? OS_TEST_MASK
							: OS_SU_TEST_MASK))
				|| ((flag & FNDELAY) && (*fip->sas_curstate
								& WOPEN))))
		{
			if (have_lock)
				release_device_lock (fip);
			(void) splx (old_level);
			FAILOPEN(EBUSY);
		}

		/* If device is already open and the FTRUNC flag is set,
		   flush input and output buffers. This may be used to
		   release processes that got stuck in sasclose() during
		   an exit() call.
		*/
		if ((fip->o_state & OS_OPEN_STATES) && (flag & FTRUNC))
		{
			flag &= ~FTRUNC;	/* flush only once */
			(void) SPLWRK ();
			(void) flushq(rq);
			(void) flushq(WR(rq));
			(void) SPLINT ();
		}

		/* if we don't have the device lock, yet, try to get it */
		if (!have_lock)
		{
			if (fip->device_flags.i & DF_DEVICE_LOCKED)
			{
				get_device_lock (fip, TTIPRI);
				have_lock = TRUE;
				/* we had to sleep for some time to get the
				   lock, therefore, re-check whether the
				   current open mode still permits us to
				   open the device
				*/
				continue;
			}
			else
				get_device_lock (fip, TTIPRI);
		}

		break;
	}

	/* disable subsequent opens */
	if (flag & FEXCL)
		open_mode |= OS_EXCLUSIVE_OPEN_1;

	/* things to do on first open only */

	if (!(fip->o_state & ((open_mode & OS_OPEN_FOR_GETTY)
				? (OS_OPEN_STATES | OS_WAIT_OPEN)
				: OS_OPEN_STATES)))
	{
		if(open_mode & OS_OPEN_FOR_GETTY) {
			ttyp = &fip->sas_dev[0].sas_state;
			fip->sas_dev[0].rq = rq;
			fip->sas_dev[0].dev = dev;
			rq->q_ptr = (char *)&fip->sas_dev[0];
		} else {
			ttyp = &fip->sas_dev[1].sas_state;
			fip->sas_dev[1].rq = rq;
			fip->sas_dev[1].dev = dev;
			rq->q_ptr = (char *)&fip->sas_dev[1];
		}
		WR(rq)->q_ptr = rq->q_ptr;

		fip->recv = NULL;
		fip->recv_cnt = 0;
		/* init data structures */
		fip->sas_curstate = ttyp;
		fip->rq = rq;
		fip->po_state = fip->o_state;
		fip->o_state = open_mode & ~OS_OPEN_STATES;
#if defined (HAVE_VPIX)
		/* initialize VP/ix related variables */
		fip->v86_proc = (v86_t *) NULL;
		fip->v86_intmask = 0;
		fip->v86_ss.ss_start = CSTART;
		fip->v86_ss.ss_stop = CSTOP;
#endif
		sas_open_device (fip);		/* open physical device */
		sas_param (fip, HARD_INIT, DEFFLAGS);	/* set up port registers */

		/* allow pending tty interrupts */
		(void) SPLWRK ();
		(void) SPLINT ();
	}

	/* If getty open and the FNDELAY flag is not set,
	   block and wait for carrier if device not yet open.
	*/
	if ((open_mode & OS_OPEN_FOR_GETTY) && !(flag & FNDELAY))
	{
		/* sleep while open for dialout or no carrier */
		while ((fip->o_state & OS_OPEN_FOR_DIALOUT)
			|| !(*fip->sas_curstate & CARR_ON))
		{
			*fip->sas_curstate |= WOPEN;
			release_device_lock (fip);
			(void) sleep ((caddr_t) fip->sas_curstate, TTIPRI);
			get_device_lock (fip, TTIPRI);
		}
		*fip->sas_curstate &= ~WOPEN;
	}

	/* we need to flush the receiver with the first open */
	if (!(fip->o_state & OS_OPEN_STATES)) sas_cmd (fip, T_RFLUSH);

	/* set open type flags */
	fip->o_state = open_mode;
	*fip->sas_curstate  |= ISOPEN;

	release_device_lock (fip);
	(void) splx (old_level);
	return (0);
}

/* Close a tty line. This is only called if there is no other
   concurrent open left. A blocked getty open is not counted as
   a concurrent open because in this state it isn't really open.
*/
int
sas_close(rq,flag)
queue_t *rq;
int flag;
{
	register struct sas_info	*fip;
	register struct fptr *fp;
	register unsigned int *ttyp;
	uint	open_mode;
	int	old_level;
	int	dev;

	if (rq == NULL)
		return;
	fp = (struct fptr *)rq->q_ptr;
	if (fp == NULL)
		return;
	fip = (struct sas_info *) fp->fip;

	if (fip->sas_timerid) {
		untimeout(fip->sas_timerid);
		fip->sas_timerid = 0;
	}
	open_mode = GET_OPEN_MODE (fp->dev);

	/* set up pointer to tty structure */
	ttyp = &fp->sas_state;
	
	old_level = SPLINT ();
	get_device_lock (fip, TTIPRI);

	/* wait for output buffer drain only if device was open */
	if (*ttyp & ISOPEN)
	{
		/* wait for buffer drain and catch interrupts */
		while (qsize(WR(rq)) || (*ttyp & (BUSY | TIMEOUT)))
		{
			*ttyp |= TTIOW;
			if (sleep ((caddr_t) ttyp, TTOPRI | PCATCH))
			{
				/* caught signal */
				*ttyp &= ~TTIOW;
				break;
			}
		}
		/* block transmitter and wait until it is
		   empty
		*/
		fip->device_flags.s |= DF_XMIT_LOCKED;
		while (fip->device_flags.i & (DF_XMIT_BUSY
					| DF_XMIT_BREAK
					| DF_GUARD_TIMEOUT))
			(void) sleep ((caddr_t) &fip->device_flags.i,
							PZERO - 1);
	}

signaled:
	/* allow pending tty interrupts */
	(void) SPLWRK ();
	(void) SPLINT ();

	fp->rq = NULL;

	if (open_mode & OS_OPEN_FOR_GETTY)
	{
		/* not waiting any more */
		*ttyp &= ~WOPEN;
		if (!(fip->o_state & OS_OPEN_FOR_DIALOUT))
		{
			sas_close_device (fip);
			fip->o_state = OS_DEVICE_CLOSED;
		}
		else
			fip->po_state = OS_DEVICE_CLOSED;
	}
	else
	{
		sas_close_device (fip);
		fip->o_state = OS_DEVICE_CLOSED;
		/* If there is a waiting getty open on
		   this port, reopen the physical device.
		*/
		if (fip->po_state & OS_WAIT_OPEN)
		{
			/* get the getty version of the
			   state
			*/
			if((&fp->sas_state) == &(fip->sas_dev[0].sas_state)) {
				fip->sas_curstate = &(fip->sas_dev[1].sas_state);
				fip->rq = fip->sas_dev[1].rq;
			} else {
				fip->sas_curstate = &(fip->sas_dev[0].sas_state);
				fip->rq = fip->sas_dev[0].rq;
			}
			fip->o_state = fip->po_state;
			fip->po_state = OS_DEVICE_CLOSED;
#if defined (HAVE_VPIX)
			/* initialize VP/ix related variables */
			fip->v86_proc = (v86_t *) NULL;
			fip->v86_intmask = 0;
			fip->v86_ss.ss_start = CSTART;
			fip->v86_ss.ss_stop = CSTOP;
#endif
			if (!(fip->device_flags.i & DF_DO_HANGUP))
			{
				sas_open_device (fip);
				/* set up port registers */
				sas_param (fip, HARD_INIT, DEFFLAGS);
			}
		}
	}

	if (fip->o_state == OS_DEVICE_CLOSED) {
		if (fip->discards) {
			freemsg(fip->discards);
			fip->discards = NULL;
		}
		if (fip->free) {
			freemsg(fip->free);
			fip->free = NULL;
			fip->nfree = 0;
		}
		if (fip->recv) {
			freemsg(fip->recv);
			fip->recv = NULL;
			fip->recv_cnt = 0;
		}
		if (fip->filled) {
			freemsg(fip->filled);
			fip->filled = NULL;
		}
		if (fip->holdbuf) {
			freemsg(fip->holdbuf);
			fip->holdbuf = NULL;
		}
	}
	*fip->sas_curstate &= ~ISOPEN;

	(void) wakeup ((caddr_t) &fip->o_state);

	if (!(fip->device_flags.i & DF_DO_HANGUP))
		release_device_lock (fip);

	(void) splx (old_level);
	return (0);
}


sas_put(wq,bp)
queue_t	*wq;
register mblk_t	*bp;
{
	REGVAR;
	register struct fptr *fp = (struct fptr *)wq->q_ptr;
	register struct sas_info *fip;
	register struct iocblk *ioss;
	register int s,x;
	struct copyreq *cpt;
	struct copyresp *ppt;
	struct termios *arg;
	struct termio *tpt;
	struct termiox  *xpt;
	int	xpar = 0;
	int	*ipt;
	int	old_spl;
	mblk_t  *mp;
	struct qval *qt;

	if (!fp) {
		sas_error(wq,bp);	/* quit now if not open */
		return;
	}
	fip = fp->fip;

	s = spltty();

	switch (bp->b_datap->db_type) {

	case M_FLUSH:
		if (*bp->b_rptr & FLUSHW) {
			sas_cmd(fip,T_WFLUSH);
			sas_cmd(fip,T_RESUME);
		}
		if (*bp->b_rptr & FLUSHR) {
			sas_cmd(fip,T_RFLUSH);
		}
		sas_flush(wq,bp);

		break;

	case M_DATA:
#ifdef NOT
		if(mpc->mpc_flags & MPC_SLIPLINE) {
			if(!(mpc->mpc_flags & MPC_CD)) {
				freemsg(bp);
				break;
			}
		}
#endif
		if (!qsize(wq)) {
			old_spl = splhi();
			if (fip->holdbuf && (msgdsize(fip->holdbuf) >
				(fip->alloc_size * 10))) {
				(void)splx(old_spl);
				putq(wq, bp);
				break;
			} else {
				sas_link(&fip->holdbuf, bp);
			}
			(void) splx(old_spl);
			*fip->sas_curstate |= BUSY;
			sas_xproc(fip);
		} else {
			putq(wq,bp);
		}
		break;

	case M_IOCDATA:
		ppt = (struct copyresp *)bp->b_rptr;
		ioss = (struct iocblk *)bp->b_rptr;
		if (ppt->cp_cmd == TCGETX || ppt->cp_cmd == TIOCMGET) {
			bp->b_datap->db_type = M_IOCACK;
			bp->b_wptr = bp->b_rptr + sizeof(struct iocblk);
			ioss->ioc_error = 0;
			ioss->ioc_count = 0;
			ioss->ioc_rval = 0;
			qreply(wq, bp);
			break;
		}
		putq(wq, bp);
		break;

	case M_DELAY:
		putq(wq,bp);
		break;

	case M_IOCTL:
		ioss = (struct iocblk *)bp->b_rptr;

		switch (ioss->ioc_cmd) {

		case TCFLSH:
			if(ioss->ioc_count != sizeof(int)) {
				bp->b_datap->db_type = M_IOCNAK;
			} else {
				switch(*((int *)(bp)->b_cont->b_rptr)) {
					case 0:
					sas_cmd(fip,T_RFLUSH);
					flushq(RD(wq), FLUSHDATA);
					break;

					case 1:
					sas_cmd(fip,T_WFLUSH);
					sas_cmd(fip,T_RESUME);
					flushq(wq, FLUSHDATA);
					break;

					case 2:
					sas_cmd(fip,T_RFLUSH);
					sas_cmd(fip,T_WFLUSH);
					sas_cmd(fip,T_RESUME);
					flushq(RD(wq), FLUSHDATA);
					flushq(wq, FLUSHDATA);
					break;
				}
				bp->b_datap->db_type = M_IOCACK;
			}
			qreply(wq,bp);
			break;

		case TCSBRK:
			putq(wq,bp);
			break;

#ifndef SCO
		case TIOCMBIC:
		case TIOCMBIS:
		case TIOCMSET:
		case TCSETX:
		case TCSETXW:
		case TCSETXF:
			if (ioss->ioc_count != TRANSPARENT) {
				if (bp->b_cont) {
					freemsg(bp->b_cont);
					bp->b_cont = NULL;
				}
				bp->b_datap->db_type = M_IOCNAK;
				qreply(wq,bp);
				break;
			}
			cpt = (struct copyreq *)bp->b_rptr;
			cpt->cq_addr = (caddr_t) *(long*)bp->b_cont->b_rptr;
			freemsg(bp->b_cont);
			bp->b_cont = NULL;
			if (((ioss->ioc_cmd >> 8)&0xff) == 'X') {
				cpt->cq_private = (mblk_t *)'X';
				cpt->cq_size = sizeof(struct termiox);
			} else {
				cpt->cq_private = (mblk_t *) 't';
				cpt->cq_size = sizeof(int);
			}
			cpt->cq_flag = 0;
			bp->b_datap->db_type = M_COPYIN;
			bp->b_wptr = bp->b_rptr + sizeof(struct copyreq);
			qreply(wq, bp);
			break;
			
		case TCSETS:
			if (ioss->ioc_count != sizeof(struct termios)) {
				bp->b_datap->db_type = M_IOCNAK;
			} else {
				if (!sas_tcsets(fip,bp))
					bp->b_datap->db_type = M_IOCNAK;
			}
			qreply(wq,bp);
			break;

		case TCSETSW:
		case TCSETSF:
			if (ioss->ioc_count != sizeof(struct termios)) {
				bp->b_datap->db_type = M_IOCNAK;
				qreply(wq,bp);
			} else {
				putq(wq,bp);
			}
			break;
#endif
		case TCSETA:
			if (ioss->ioc_count != sizeof(struct termio)) {
				bp->b_datap->db_type = M_IOCNAK;
			} else {
				if (!sas_tcset(fip,bp))
					bp->b_datap->db_type = M_IOCNAK;
			}
			qreply(wq,bp);
			break;

		case TCSETAW:
		case TCSETAF:
			if (ioss->ioc_count != sizeof(struct termio)) {
				bp->b_datap->db_type = M_IOCNAK;
				qreply(wq,bp);
			} else {
				putq(wq,bp);
			}
			break;

#ifndef SCO
		case TIOCMGET:
		case TCGETX:
			if (ioss->ioc_count == TRANSPARENT) {
				cpt = (struct copyreq *)bp->b_rptr;
				if (ioss->ioc_cmd == TCGETX) {
					cpt->cq_size = sizeof (struct termiox);
				} else {
					cpt->cq_size = sizeof(int);
				}
				cpt->cq_addr = (caddr_t) *(long*)bp->b_cont->b_rptr;
				cpt->cq_flag = 0;
				xpar = 1;
			}
			if (bp->b_cont)
				freemsg(bp->b_cont);
			if ((bp->b_cont = allocb(cpt->cq_size,
							BPRI_MED)) == NULL) {
				bp->b_datap->db_type = M_IOCNAK;
				ioss->ioc_error = EAGAIN;
				qreply(wq, bp);
				break;
			}
			if (ioss->ioc_cmd == TCGETX) {
				xpt = (struct termiox *)bp->b_cont->b_rptr;
				xpt->x_sflag = 0;
				xpt->x_cflag = 0;
				for (x=0; x<NFF; x++) {
					xpt->x_rflag[x] = 0;
				}
				xpt->x_hflag = 0;
				if (fip->flow_flags.s & FF_HWI_HANDSHAKE) {
					if (fip->flow.m.ic == MC_SET_RTS) {
						xpt->x_hflag |= RTSXOFF;
					} else {
						xpt->x_hflag |= DTRXOFF;
					}
				}
				if (fip->flow_flags.s & FF_HWO_HANDSHAKE) {
					if (fip->flow.m.oc == MS_CTS_PRESENT) {
						xpt->x_hflag |= CTSXON;
					} else {
						xpt->x_hflag |= CDXON;
					}
				}
				bp->b_cont->b_wptr = bp->b_cont->b_rptr +
					sizeof (struct termiox);
			} else {
				ipt = (int *)bp->b_cont->b_rptr;
				*ipt = 0;
				if (fip->device_flags.s & DF_MODEM_ENABLED) {
					*ipt |= TIOCM_LE;
				}
				if (fip->mcr & MC_SET_DTR) {
					*ipt |= TIOCM_DTR;
				}
				if (fip->mcr & MC_SET_RTS) {
					*ipt |= TIOCM_RTS;
				}
				if (fip->msr & MS_CTS_PRESENT) {
					*ipt |= TIOCM_CTS;
				}
				if (fip->msr & MS_DCD_PRESENT) {
					*ipt |= TIOCM_CAR;
				}
				if (fip->msr & MS_RING_PRESENT) {
					*ipt |= TIOCM_RNG;
				}
				if (fip->msr & MS_DSR_PRESENT) {
					*ipt |= TIOCM_DSR;
				}
				bp->b_cont->b_wptr = bp->b_cont->b_rptr +
					sizeof (int);
			}
			if (xpar) {
				bp->b_datap->db_type = M_COPYOUT;
				bp->b_wptr = bp->b_rptr+sizeof(struct copyreq);
			} else {
				bp->b_datap->db_type = M_IOCACK;
				ioss->ioc_count = sizeof(struct termiox);
			}
			qreply(wq,bp);
			break;
		case TCGETS:
			if(bp->b_cont) freemsg(bp->b_cont);
			if((bp->b_cont=allocb(sizeof(struct termios),BPRI_MED)) == NULL) {
				bp->b_datap->db_type = M_IOCNAK;
				ioss->ioc_error = EAGAIN;
				qreply(wq,bp);
				break;
			}
			arg = (struct termios *)bp->b_cont->b_rptr;
			arg->c_cflag = fip->scflag;
			arg->c_lflag = fip->lflag;
			arg->c_iflag = fip->iflag;
			arg->c_oflag = fip->oflag;
			for(x=0;x<NCCS;x++) arg->c_cc[x] = fip->cc[x];
			bp->b_datap->db_type = M_IOCACK;
			ioss->ioc_count = sizeof(struct termios);
			bp->b_cont->b_wptr = bp->b_cont->b_rptr+sizeof(struct termios);

			qreply(wq,bp);
			break;
#endif

		case TCGETA:
			if(bp->b_cont) freemsg(bp->b_cont);
			if((bp->b_cont=allocb(sizeof(struct termio),BPRI_MED)) == NULL) {
				bp->b_datap->db_type = M_IOCNAK;
				ioss->ioc_error = EAGAIN;
				qreply(wq,bp);
				break;
			}
			tpt = (struct termio *)bp->b_cont->b_rptr;
			tpt->c_cflag = fip->scflag;
			tpt->c_lflag = fip->lflag;
			tpt->c_oflag = fip->oflag;
			tpt->c_iflag = fip->iflag;
			for(x=0;x<NCC;x++) tpt->c_cc[x] = fip->cc[x];
			bp->b_datap->db_type = M_IOCACK;
			bp->b_cont->b_wptr = bp->b_cont->b_rptr + sizeof(struct termio);
			ioss->ioc_count = sizeof(struct termio);

			qreply(wq,bp);
			break;

		case TCXONC:
			if (ioss->ioc_count != sizeof(int)) {
				printf("SAS: IOCNAK at TCXONC!\n");
				bp->b_datap->db_type = M_IOCNAK;
				qreply(wq,bp);
				break;
			};
			switch (*(int*)(bp->b_cont->b_rptr)) {
			case 0:		/* stop output */
				sas_cmd(fip,T_SUSPEND);
				break;
			case 1:		/* resume output */
				sas_cmd(fip,T_RESUME);
				break;
			default:
				ioss->ioc_error = EINVAL;
				printf("SAS: EINVAL 1!\n");
				break;
			}
			bp->b_datap->db_type = M_IOCACK;
			ioss->ioc_count = 0;
			qreply(wq, bp);
			break;


		case TIOCSWINSZ:
		case TIOCGWINSZ:
		case STGET:
		case JWINSIZE:
			ioss->ioc_error = ENOTTY;
			ioss->ioc_count = 0;
			bp->b_datap->db_type = M_IOCNAK;
			qreply(wq,bp);
			break;

#ifndef SCO
		case EUC_MSAVE:
		case EUC_MREST:
			bp->b_datap->db_type = M_IOCACK;
			ioss->ioc_count = 0;
			qreply(wq,bp);
			break;
#endif

		case SAS_GETQ:
			if(bp->b_cont) freemsg(bp->b_cont);
			if((bp->b_cont=allocb(sizeof(struct qval),BPRI_MED)) == NULL) {
				bp->b_datap->db_type = M_IOCNAK;
				ioss->ioc_error = EAGAIN;
				qreply(wq,bp);
				break;
			}
			qt = (struct qval *)bp->b_cont->b_rptr;
			qt->qsize = wq->q_count;
			if(fip->holdbuf) qt->holdsize = msgdsize(fip->holdbuf);
				else qt->holdsize = 0;
			bp->b_datap->db_type = M_IOCACK;
			ioss->ioc_count = sizeof(struct qval);
			bp->b_cont->b_wptr = bp->b_cont->b_rptr+sizeof(struct qval);
			qreply(wq,bp);
			break;

		default:
			printf("SAS: bad ioctl 0x%x\n",ioss->ioc_cmd);
			ioss->ioc_error = ENOTTY;
			ioss->ioc_count = 0;
			bp->b_datap->db_type = M_IOCNAK;
			qreply(wq,bp);
			break;
		}
		break;

#ifndef SCO
	case M_CTL:
		if(bp->b_wptr - bp->b_rptr == sizeof(struct iocblk)) {
			ioss = (struct iocblk *)bp->b_rptr;
			if(bp->b_cont) freemsg(bp->b_cont);
			if((bp->b_cont=allocb(sizeof(struct termios),BPRI_MED)) == NULL) {
				bp->b_datap->db_type = M_IOCNAK;
				ioss->ioc_error = EAGAIN;
				qreply(wq,bp);
				break;
			}
			ioss->ioc_cmd = MC_PART_CANON;
			arg = (struct termios *) bp->b_cont->b_rptr;
			arg->c_iflag = IXON|IXOFF|IXANY;
			arg->c_oflag = 0;
			arg->c_lflag = 0;
			bp->b_cont->b_wptr = bp->b_cont->b_rptr +
				sizeof(struct termios);

			qreply(wq,bp);
		} else {
			printf("SAS: This is not iocblk!\n");
			sas_error(wq,bp);
		}
		break;

	case M_STOPI:
		if (fip->flow_flags.i & FF_HWI_HANDSHAKE) {
			fip->mcr &= ~fip->flow.m.ic;
			sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
			fip->flow_flags.s |= FF_HWI_STOPPED;
		}
		if(fip->iflag & IXOFF) {
			fip->flow_flags.s |= (FF_SWI_STOPPED | FF_SW_FC_REQ);
			*fip->sas_curstate |= (TTXOFF | BUSY);
			sas_hdx_check (fip);
			sas_xproc (fip);
		}
		freemsg(bp);
		break;

	case M_STARTI:
		if(fip->flow_flags.s & FF_HWI_STOPPED) {
			fip->flow_flags.s &= ~FF_HWI_STOPPED;
			fip->mcr |= fip->flow.m.ic;
			sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
		}
		if(fip->flow_flags.s & FF_SWI_STOPPED) {
			fip->flow_flags.s &= ~FF_SWI_STOPPED;
			fip->flow_flags.s |= FF_SW_FC_REQ;
			*fip->sas_curstate |= (TTXON | BUSY);
			sas_hdx_check (fip);
			sas_xproc (fip);
		}
		freemsg(bp);
		break;
#endif
	case M_STOP:
		sas_cmd(fip,T_SUSPEND);
		freemsg(bp);
		break;

	case M_START:
		sas_cmd(fip,T_RESUME);
		freemsg(bp);
		break;

	case M_READ:
		freemsg(bp);
		break;

	default:
		printf("SAS: Unknown message!!! TYPE 0x%x\n",bp->b_datap->db_type);
		sas_error(wq,bp);
	}
	splx(s);
}

#if defined (HAVE_VPIX)
/* Send port status register to VP/ix */
static int
sas_vpix_sr (fip, token, status)
register struct sas_info	*fip;
uint	token;
uint	status;
{
	if ((fip->recv_ring_cnt <= RECV_BUFF_SIZE - 3)
		&& ((fip->tty->t_state & (ISOPEN | CARR_ON)) ==
						(ISOPEN | CARR_ON)))
	{
		/* sent the character sequence 0xff, <token>, <status>
		   to VP/ix
		*/
		fip->recv_ring_cnt += 3;

		*fip->recv_ring_put_ptr = 0xff;
		if (++fip->recv_ring_put_ptr
				== &fip->recv_buffer [RECV_BUFF_SIZE])
			fip->recv_ring_put_ptr
				= &fip->recv_buffer [0];
		*fip->recv_ring_put_ptr = token;
		if (++fip->recv_ring_put_ptr
				== &fip->recv_buffer [RECV_BUFF_SIZE])
			fip->recv_ring_put_ptr
				= &fip->recv_buffer [0];
		*fip->recv_ring_put_ptr = status;
		if (++fip->recv_ring_put_ptr
				== &fip->recv_buffer [RECV_BUFF_SIZE])
			fip->recv_ring_put_ptr
				= &fip->recv_buffer [0];
		return (TRUE);
	}
	return (FALSE);
}
#endif

#if defined (NEED_PUT_GETCHAR)

int
asyputchar (arg1)
unchar	arg1;
{
	register struct	sas_info	*fip;
	REGVAR;

	if (!sas_is_initted)
		(void) sasinit ();

	fip = &sas_info [0];
	if (fip->device_flags.i & DF_DEVICE_CONFIGURED)
	{
		sas_ctl (fip, LINE_STATUS_PORT);
		while (!(inb (LINE_STATUS_PORT.addr) & LS_XMIT_AVAIL))
			;
		sas_outb (fip, XMT_DATA_PORT, arg1);
		if (arg1 == 10)
			(void) asyputchar (13);
	}
	return (0);
}

int
asygetchar ()
{
	register struct	sas_info	*fip;
	REGVAR;

	if (!sas_is_initted)
		(void) sasinit ();

	fip = &sas_info [0];
	if ((fip->device_flags.i & DF_DEVICE_CONFIGURED)
		&& (sas_first_inb (fip, LINE_STATUS_PORT) & LS_RCV_AVAIL))
		return (sas_inb (fip, RCV_DATA_PORT));
	else
		return (-1);
}
#endif

#if defined (NEED_INIT8250)

/* reset the requested port to be used directly by a DOS process */
int
init8250 (port, ier)
ushort	port, ier;	/* ier not used in this stub */
{
	REGVAR;
	register struct sas_info	*fip;
	register uint	physical_unit;
	int	old_level;

	/* See if the port address matches a port that is used by
	   the sas driver.
	*/
	for (physical_unit = 0; physical_unit < sas_physical_units;
			physical_unit++)
		if (port == (ushort) (sas_port [physical_unit]))
			break;

	if (physical_unit >= sas_physical_units)
		return (-1);	/* port didn't match */

	fip = sas_info_ptr [physical_unit];

	old_level = SPLINT ();

	fip->ier = IE_NONE;
	sas_first_outb (fip, INT_ENABLE_PORT, fip->ier);

	fip->mcr &= ~(fip->flow.m.ic | fip->flow.m.hc);
	sas_outb (fip, MDM_CTL_PORT, fip->mcr);

	if (fip->device_type == TYPE_NS16550A)
		sas_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_CLEAR_CMD);
	else if (fip->device_type == TYPE_I82510)
	{
		sas_outb (fip, I_BANK_PORT, I_BANK_1);
		sas_outb (fip, I_TCM_PORT, I_FIFO_CLR_XMIT);
		sas_outb (fip, I_RCM_PORT, I_FIFO_CLR_RECV);
		sas_outb (fip, I_BANK_PORT, I_BANK_2);
		sas_outb (fip, I_IDM_PORT, I_FIFO_CLEAR_CMD);
		sas_outb (fip, I_BANK_PORT, I_BANK_0);
	}

	(void) sas_inb (fip, MDM_STATUS_PORT);
	(void) sas_inb (fip, RCV_DATA_PORT);
	(void) sas_inb (fip, RCV_DATA_PORT);
	(void) sas_inb (fip, LINE_STATUS_PORT);
	(void) sas_inb (fip, INT_ID_PORT);
	(void) splx (old_level);
	return (0);
}
#endif

sas_flush(wq,bp)
register queue_t *wq;
register mblk_t *bp;
{
	if (*bp->b_rptr & FLUSHW)
		flushq(wq, FLUSHDATA);

	if (*bp->b_rptr & FLUSHR) {
		flushq(RD(wq), FLUSHDATA);
		*bp->b_rptr &= ~FLUSHW;
		qreply(wq, bp);
	} else {
		freemsg(bp);
	}
}
static int
sas_wsrv(q)
queue_t	*q;
{
	register struct sas_info *fip;
	register mblk_t	*wbp;
	register unsigned char	*p;		/* pointer to info */
	register int	nc;		/* number of char to xfer */
	int	old_spl;
	struct copyresp *cpt;
	struct iocblk *ipt;
	unchar	c;

	fip = ((struct fptr *) q->q_ptr)->fip;
	sas_buffers(fip, FALSE);
	old_spl = splhi();
	if (fip->holdbuf && fip->holdbuf->b_cont &&
		(msgdsize(fip->holdbuf) > (10*fip->alloc_size))) {
		(void) splx(old_spl);
		return;
	}
	(void) splx(old_spl);
	
	while (wbp = getq(WR(fip->rq))) {

		switch (wbp->b_datap->db_type) {
		case M_DATA:
			if (!msgdsize(wbp)) {
				freemsg(wbp);
				continue;
			}
			break;
	
		case M_DELAY:
			if(!fip->holdbuf) {
				*fip->sas_curstate &= ~WIOC;
			} else {
				*fip->sas_curstate |= WIOC;
				putbq(WR(fip->rq),wbp);
				return;
			}
			*fip->sas_curstate |= RTO;
			fip->sas_timerid = timeout(sas_delay,
				(caddr_t)fip,*(unsigned char *)wbp->b_rptr);
			freemsg(wbp);
			continue;
	
		case M_IOCDATA:
			cpt = (struct copyresp *)wbp->b_rptr;
			ipt = (struct iocblk *)wbp->b_rptr;
			if (cpt->cp_rval) {
				freemsg(wbp);
				continue;
			}
			ipt->ioc_error = 0;
			ipt->ioc_count = 0;
			ipt->ioc_rval = 0;
			wbp->b_wptr = wbp->b_rptr + sizeof(struct iocblk);
			if(sas_intioctl(fip, wbp) < 0) {
				putbq(WR(fip->rq),wbp);
				return;
			}
			freemsg(wbp->b_cont);
			wbp->b_cont = NULL;
			continue;
			
		case M_IOCTL:
			if(sas_intioctl(fip, wbp) < 0) {
				putbq(WR(fip->rq),wbp);
				return;
			}
			continue;
	
		default:
			cmn_err (CE_PANIC, "mega: bad stream message");
			freemsg(wbp);
			continue;
		}
	
		(void) splhi();
		sas_link(&fip->holdbuf, wbp);
		if (fip->holdbuf &&
			msgdsize(fip->holdbuf) > (10*fip->alloc_size)) {
			splx(old_spl);
			break;
		}
		splx(old_spl);
		if (!(*fip->sas_curstate & BUSY)) {
			*fip->sas_curstate |= BUSY;
			sas_xproc(fip);
		}
	}
	
}

sas_error(wq,bp)
register queue_t *wq;
register mblk_t *bp;
{
	bp->b_datap->db_type = M_ERROR;
	bp->b_rptr = bp->b_datap->db_base;
	bp->b_wptr = bp->b_rptr + 1;
	*bp->b_rptr = EIO;

	qreply(wq, bp);
}

/* This is used for in-band ioctls: */

static
sas_intioctl(fip,bp)
register struct sas_info *fip;
register mblk_t *bp;
{
	REGVAR;
	register struct iocblk *ioss;
	unsigned int stat, *ipt;
	unsigned char	new_mcr;

	ioss = (struct iocblk*)bp->b_rptr;

	switch (ioss->ioc_cmd) {
#ifndef SCO
	case TIOCMBIS:
	case TIOCMBIC:
	case TIOCMSET:
		ipt = (unsigned int *) bp->b_cont->b_rptr;
		if (*ipt & ~(TIOCM_DTR|TIOCM_RTS)) {
			bp->b_datap->db_type = M_IOCNAK;
			break;
		}
		stat = 0;
		if (*ipt & TIOCM_DTR) {
			stat |= MC_SET_DTR;
		}
		if (*ipt & TIOCM_RTS) {
			stat |= MC_SET_DTR;
		}
		new_mcr = fip->mcr;
		switch (ioss->ioc_cmd) {
		case TIOCMBIS:
			new_mcr |= stat;
			break;
		case TIOCMSET:
			new_mcr &= ~MC_ANY_CONTROL;
			new_mcr |= stat;
			break;
		case TIOCMBIC:
			new_mcr &= ~stat;
			break;
		}
		if (!fip->flow.m.oe || (fip->msr & fip->flow.m.oe))
		{
			if (fip->flow_flags.s & FF_HWI_HANDSHAKE) {
				if (new_mcr & fip->flow.m.ic) {
					fip->flow_flags.s &= ~FF_HWI_STOPPED;
				} else {
					fip->flow_flags.s |= FF_HWI_STOPPED;
				}
			}
		}
		fip->mcr = new_mcr;
		sas_outb (fip, MDM_CTL_PORT, fip->mcr);
 
		bp->b_datap->db_type = M_IOCACK;
		break;

	case TCSETXF:
		if(sas_tcsetx(fip,bp))
			(void)putctl1(fip->rq->q_next, M_FLUSH, FLUSHR);
		else
			bp->b_datap->db_type = M_IOCNAK;
		break;

	case TCSETXW:
		if(!fip->holdbuf) {
			*fip->sas_curstate &= ~WIOC;
		} else {
			*fip->sas_curstate |= WIOC;
			return(-1);
		}
	case TCSETX:
		if (!sas_tcsetx(fip,bp))
			bp->b_datap->db_type = M_IOCNAK;
		break;
	case TCSETSF:
		if(sas_tcsets(fip,bp))
			(void)putctl1(fip->rq->q_next, M_FLUSH, FLUSHR);
		break;

	case TCSETSW:
		if(!fip->holdbuf) {
			*fip->sas_curstate &= ~WIOC;
		} else {
			*fip->sas_curstate |= WIOC;
			return(-1);
		}
	case TCSETS:
		(void)sas_tcsets(fip,bp);
		break;
#endif

	case TCSETAF:
		if(sas_tcset(fip,bp)) {
			(void)putctl1(fip->rq->q_next, M_FLUSH, FLUSHR);
		}
		break;

	case TCSETAW:
		if(!fip->holdbuf) {
			*fip->sas_curstate &= ~WIOC;
		} else {
			*fip->sas_curstate |= WIOC;
			return(-1);
		}
	case TCSETA:
		(void)sas_tcset(fip,bp);
		break;

	case TCSBRK:
		if(*(int *)bp->b_cont->b_rptr == 0) sas_cmd(fip,T_BREAK);
		ioss->ioc_count = 0;
		bp->b_datap->db_type = M_IOCACK;
		break;

	default:
		ioss->ioc_error = EINVAL;
		printf("SAS: EINVAL 4!\n");
		ioss->ioc_count = 0;
		bp->b_datap->db_type = M_IOCNAK;
		break;		
	}

	putnext(fip->rq, bp);
	return(0);
}

/* Several functions for flow control, character output and special event
   requests and handling.
*/
static void
sas_cmd (fip, arg2)
register struct sas_info	*fip;
int	arg2;
{
	REGVAR;
	register unsigned int *ttyp = fip->sas_curstate;
	int	old_spl;

	switch (arg2)
	{
	case T_TIME:	/* timeout */
		*ttyp &= ~TIMEOUT;
start_output:
		event_sched (fip, EF_DO_XXFER);
		break;

	case T_SUSPEND:	/* suspend character output */
		fip->flow_flags.s |= FF_SWO_STOPPED;
		*ttyp |= TTSTOP;
		break;

	case T_RESUME:	/* restart character output */
		fip->flow_flags.s &= ~FF_SWO_STOPPED;
		*ttyp &= ~TTSTOP;
		sas_xproc (fip);
		break;

	case T_BLOCK:	/* stop character input, request XOFF */
		*ttyp |= TBLOCK;
		break;	/* note: we do our own XON/XOFF */

	case T_UNBLOCK:	/* restart character input, request XON */
		*ttyp &= ~TBLOCK;
		break;	/* note: we do our own XON/XOFF */

	case T_RFLUSH:	/* flush input buffers and restart input */
		if (fip->device_type == TYPE_NS16550A)
			sas_first_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_SETUP_CMD
						| NS_FIFO_CLR_RECV);
		else if (fip->device_type == TYPE_I82510)
		{
			sas_first_outb (fip, I_BANK_PORT, I_BANK_1);
			sas_outb (fip, I_RCM_PORT, I_FIFO_CLR_RECV);
			sas_outb (fip, I_BANK_PORT, I_BANK_0);
		}

		old_spl = splhi();
		if (fip->recv) {
			fip->recv->b_wptr = fip->recv->b_rptr;
			fip->recv_cnt = 0;
		}
		(void) splx(old_spl);
		*ttyp &= ~TBLOCK;

		break;

	case T_WFLUSH:	/* flush output buffer and restart output */
		if (fip->device_type == TYPE_NS16550A)
			sas_first_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_SETUP_CMD
						| NS_FIFO_CLR_XMIT);
		else if (fip->device_type == TYPE_I82510)
		{
			sas_first_outb (fip, I_BANK_PORT, I_BANK_1);
			sas_outb (fip, I_TCM_PORT, I_FIFO_CLR_XMIT);
			sas_outb (fip, I_BANK_PORT, I_BANK_0);
		}


		fip->flow_flags.s &= ~FF_SWO_STOPPED;
		*ttyp &= ~TTSTOP;

		flushq(WR(fip->rq));

		if (!(fip->device_flags.i & (DF_XMIT_BUSY | DF_GUARD_TIMEOUT
						| DF_XMIT_BREAK)))
		{
			*ttyp &= ~BUSY;
			sas_hdx_check (fip);
			goto start_output;
		}
		break;

	case T_BREAK:	/* do a break on the transmitter line */
		fip->device_flags.s |= DF_XMIT_BREAK;
		*ttyp |= BUSY;
		sas_hdx_check (fip);
		if (fip->device_flags.i & (DF_XMIT_BUSY | DF_GUARD_TIMEOUT))
		{
			fip->device_flags.s |= DF_DO_BREAK;
		}
		else
		{
			/* set up break request flags */
			fip->lcr |= LC_SET_BREAK_LEVEL;
			sas_first_outb (fip, LINE_CTL_PORT, fip->lcr);
			(void) timeout (sas_timeout, fip, (BREAK_TIME) * (HZ)
								/ 1000);
		}
		break;

	}
}

/* open device physically */
static void
sas_open_device (fip)
register struct sas_info	*fip;
{
	REGVAR;

	/* if already open, set up the mcr register only */
	if (fip->device_flags.i & DF_DEVICE_OPEN)
		goto setmcr;

	/* init some variables */
	fip->device_flags.s &= DF_DEVICE_CONFIGURED | DF_DEVICE_LOCKED
				| DF_CTL_FIRST | DF_CTL_EVERY | DF_HIGH_SPEED;
	fip->flow_flags.s = 0;
	fip->event_flags.s = 0;
	fip->cflag = 0;
	fip->iflag = 0;

	/* hook into the interrupt users chain */
	fip->next_int_user = sas_first_int_user [fip->vec];
	if (fip->next_int_user)
		fip->next_int_user->prev_int_user = fip;
	sas_first_int_user [fip->vec] = fip;
	fip->prev_int_user = (struct sas_info *) NULL;

	fip->lcr = 0;
	sas_first_outb (fip, LINE_CTL_PORT, fip->lcr);

	/* clear and disable FIFOs */
	if (fip->device_type == TYPE_NS16550A)
		sas_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_CLEAR_CMD);
	else if (fip->device_type == TYPE_I82510)
	{
		sas_outb (fip, I_BANK_PORT, I_BANK_1);
		sas_outb (fip, I_TCM_PORT, I_FIFO_CLR_XMIT);
		sas_outb (fip, I_RCM_PORT, I_FIFO_CLR_RECV);
		sas_outb (fip, I_BANK_PORT, I_BANK_2);
		sas_outb (fip, I_IDM_PORT, I_FIFO_CLEAR_CMD);
		sas_outb (fip, I_BANK_PORT, I_BANK_0);
	}

	/* clear interrupts */
	(void) sas_inb (fip, MDM_STATUS_PORT);
	(void) sas_inb (fip, RCV_DATA_PORT);
	(void) sas_inb (fip, RCV_DATA_PORT);
	(void) sas_inb (fip, LINE_STATUS_PORT);
	(void) sas_inb (fip, INT_ID_PORT);

	/* enable FIFOs */
	if (fip->device_type == TYPE_NS16550A)
		sas_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_SETUP_CMD);
	else if (fip->device_type == TYPE_I82510)
	{
		sas_outb (fip, I_BANK_PORT, I_BANK_2);
		sas_outb (fip, I_IDM_PORT, I_FIFO_SETUP_CMD);
		sas_outb (fip, I_BANK_PORT, I_BANK_0);
	}

	fip->msr = fip->new_msr = sas_inb (fip, MDM_STATUS_PORT)
					& (MS_CTS_PRESENT
						| MS_DSR_PRESENT
						| MS_DCD_PRESENT);

	fip->ier = IE_INIT_MODE;	/* enable UART interrupts */
	sas_outb (fip, INT_ENABLE_PORT, fip->ier);
	/* Select hardware handshake depending on the minor device
	   number and the CTSFLOW and RTSFLOW flags (if they are
	   available).
	*/
	fip->flow_flags.s &= ~(FF_HWO_HANDSHAKE
				| FF_HWI_HANDSHAKE
				| FF_HDX_HANDSHAKE);
	if (fip->o_state & (OS_HWO_HANDSHAKE | OS_HWI_HANDSHAKE
					| OS_HDX_HANDSHAKE))
	{
		if (fip->o_state & OS_HWO_HANDSHAKE)
			fip->flow_flags.s |= FF_HWO_HANDSHAKE;
		if (fip->o_state & OS_HWI_HANDSHAKE)
			fip->flow_flags.s |= FF_HWI_HANDSHAKE;
		if (fip->o_state & OS_HDX_HANDSHAKE)
			fip->flow_flags.s |= FF_HDX_HANDSHAKE;
	}
	fip->modem.l = sas_modem [fip->unit];
	fip->flow.l = sas_flow [fip->unit];

	/* mask off invalid bits */
	fip->modem.m.di &= MC_ANY_CONTROL;
	fip->modem.m.eo &= MC_ANY_CONTROL;
	fip->modem.m.ei &= MC_ANY_CONTROL;
	fip->modem.m.ca &= MS_ANY_PRESENT;
	fip->flow.m.ic &= MC_ANY_CONTROL;
	fip->flow.m.oc &= MS_ANY_PRESENT;
	fip->flow.m.oe &= MS_ANY_PRESENT;
	fip->flow.m.hc &= MC_ANY_CONTROL;

setmcr:
	/* set up modem and flow control lines */
	fip->mcr &= ~(fip->modem.m.di
			| fip->modem.m.ei
			| fip->modem.m.eo
			| fip->flow.m.ic
			| fip->flow.m.hc);

	fip->mcr |= (fip->o_state & OS_WAIT_OPEN)
			? fip->modem.m.ei
			: fip->modem.m.eo;

	if (fip->o_state & OS_HWI_HANDSHAKE)
		fip->mcr |= fip->flow.m.ic;
	else if (!(fip->o_state & OS_HDX_HANDSHAKE))
	{
		fip->flow_flags.s |= FF_HDX_STARTED;
		fip->mcr |= fip->flow.m.hc;
	}

	sas_outb (fip, MDM_CTL_PORT, fip->mcr);

	fip->device_flags.s |= DF_DEVICE_OPEN | DF_MODEM_ENABLED;
}

/* close device physically */
static void
sas_close_device (fip)
register struct sas_info	*fip;
{
	REGVAR;

	fip->ier = IE_NONE;	/* disable UART interrupts */
	sas_first_outb (fip, INT_ENABLE_PORT, fip->ier);

	/* drop flow control lines */
	fip->mcr &= (fip->o_state & OS_HWI_HANDSHAKE)
		? ~fip->flow.m.ic
		: ~fip->flow.m.hc;
	sas_outb (fip, MDM_CTL_PORT, fip->mcr);

	/* clear and disable FIFOs */
	if (fip->device_type == TYPE_NS16550A)
		sas_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_CLEAR_CMD);
	else if (fip->device_type == TYPE_I82510)
	{
		sas_outb (fip, I_BANK_PORT, I_BANK_1);
		sas_outb (fip, I_TCM_PORT, I_FIFO_CLR_XMIT);
		sas_outb (fip, I_RCM_PORT, I_FIFO_CLR_RECV);
		sas_outb (fip, I_BANK_PORT, I_BANK_2);
		sas_outb (fip, I_IDM_PORT, I_FIFO_CLEAR_CMD);
		sas_outb (fip, I_BANK_PORT, I_BANK_0);
	}

	/* reset break level */
	fip->lcr &= ~LC_SET_BREAK_LEVEL;
	sas_outb (fip, LINE_CTL_PORT, fip->lcr);

	/* clear some variables */
	fip->device_flags.s &= ~DF_DEVICE_OPEN;
	fip->event_flags.s = 0;

	/* unhook from interrupt users chain */
	if (fip->prev_int_user)
		fip->prev_int_user->next_int_user = fip->next_int_user;
	else
		sas_first_int_user [fip->vec] = fip->next_int_user;
	if (fip->next_int_user)
		fip->next_int_user->prev_int_user = fip->prev_int_user;

	if (fip->cflag & HUPCL)
	{
		/* request hangup */
		fip->device_flags.s |= DF_DO_HANGUP;
		(void) timeout (sas_hangup, fip, (HANGUP_DELAY) * (HZ) / 1000);
	}
}

/* compute the port access control value */
static uint
sas_make_ctl_val (fip, unit, num)
register struct sas_info	*fip;
uint	unit;
uint	num;
{
	register uint	mask, val;
	uint	i;

	if (fip->device_flags.i & DF_CTL_FIRST)
		return (sas_ctl_val [unit]);

	if (fip->device_flags.i & DF_CTL_EVERY)
	{
		for (i = 0, mask = sas_ctl_val [unit],
				val = sas_ctl_val [unit] << 8; i < 8; i++)
		{
			if (mask & 0x100)
			{
				if (num & 0x01)
					val ^= 0x100;
				num >>= 1;
			}
			mask >>= 1;
			val >>= 1;
		}
		return (val);
	}
	return (0);
}

/* test device thoroughly */
static int
sas_test_device (fip)
register struct sas_info	*fip;
{
	REGVAR;
	register unchar	*cptr;
	int	done;
	uint	delay_count, rep_count, i, lsr;
	static uint	lcrval [3] =
	{
		LC_WORDLEN_8,
		LC_WORDLEN_8 | LC_ENABLE_PARITY,
		LC_WORDLEN_8 | LC_ENABLE_PARITY | LC_EVEN_PARITY
	};

	/* make sure FIFO is off */
	sas_first_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_CLEAR_CMD);
	sas_outb (fip, I_BANK_PORT, I_BANK_2);
	sas_outb (fip, I_IDM_PORT, I_FIFO_CLEAR_CMD);
	sas_outb (fip, I_BANK_PORT, I_BANK_0);

	/* set counter divisor */
	sas_outb (fip, LINE_CTL_PORT, LC_ENABLE_DIVISOR);
	sas_outb (fip, DIVISOR_LSB_PORT, sas_speeds [B57600SAS]);
	sas_outb (fip, DIVISOR_MSB_PORT, sas_speeds [B57600SAS] >> 8);
	sas_outb (fip, LINE_CTL_PORT, 0);

	/* switch to local loopback */
	sas_outb (fip, MDM_CTL_PORT, MC_SET_LOOPBACK);

	done = 0;

	/* wait until the transmitter register is empty */
	for (delay_count = 20000;
		delay_count && (~sas_inb (fip, LINE_STATUS_PORT)
				& (LS_XMIT_AVAIL | LS_XMIT_COMPLETE));
		delay_count--)
		;

	if (!delay_count)
		done = 1;

	if (!done)
	{
	    /* clear flags */
	    (void) sas_inb (fip, RCV_DATA_PORT);
	    (void) sas_inb (fip, RCV_DATA_PORT);
	    (void) sas_inb (fip, LINE_STATUS_PORT);

	    /* make sure there are no more characters in the
	       receiver register
	    */
	    for (delay_count = 20000;
		delay_count && !(sas_inb (fip, LINE_STATUS_PORT)
						& LS_RCV_AVAIL);
		delay_count--)
		;

	    if (delay_count)
		(void) sas_inb (fip, RCV_DATA_PORT);

	    for (rep_count = 100; rep_count; rep_count--)
	    {
		for (i = 0; i < 3; i++)
		{
			/* test pattern */
			cptr = (unchar *) "\377\125\252\045\244\0\0";

			/* check LSR flags */
			if (sas_inb (fip, LINE_STATUS_PORT)
					!= (LS_XMIT_AVAIL | LS_XMIT_COMPLETE))
			{
				done = 2;
				break;
			}

			/* test transmitter and receiver
			   with different line settings
			*/
			sas_outb (fip, LINE_CTL_PORT, lcrval [i]);

			/* send first test pattern */
			sas_outb (fip, XMT_DATA_PORT, *cptr);

			/* wait until the transmitter holding register
			   is empty
			*/
			for (delay_count = 20000;
					delay_count
					  && !((lsr = sas_inb (fip,
						    LINE_STATUS_PORT))
							& LS_XMIT_AVAIL);
				delay_count--)
				;

			if (!delay_count)
			{
				done = 3;
				break;
			}

			/* check LSR flags */
			if (lsr != LS_XMIT_AVAIL)
			{
				done = 2;
				break;
			}

			do
			{
				if (*cptr)
				{
					/* send next test pattern */
					sas_outb (fip, XMT_DATA_PORT, *(cptr + 1));

					/* check LSR flags */
					if (sas_inb (fip, LINE_STATUS_PORT))
					{
						done = 2;
						break;
					}
				}

				/* wait until the test pattern is received */
				for (delay_count = 20000;
					delay_count
					  && !((lsr = sas_inb (fip,
							LINE_STATUS_PORT))
								& LS_RCV_AVAIL);
					delay_count--)
					;

				if (!delay_count)
				{
					done = 4;
					break;
				}

				/* check LSR flags */
				if ((lsr & LS_RCV_INT) != LS_RCV_AVAIL)
				{
					done = 5;
					break;
				}

				/* check test pattern */
				if (sas_inb (fip, RCV_DATA_PORT) != *cptr)
				{
					done = 6;
					break;
				}

				/* check LSR flags */
				if (sas_inb (fip, LINE_STATUS_PORT)
							& LS_RCV_INT)
				{
					done = 5;
					break;
				}

				/* wait until the transmitter register
				   is empty
				*/
				if (*cptr)
				{
				    for (delay_count = 20000;
					delay_count
					  && !((lsr = sas_inb (fip,
						    LINE_STATUS_PORT))
							& LS_XMIT_AVAIL);
					delay_count--)
					;

				    if (!delay_count)
				    {
					done = 7;
					break;
				    }

				    /* check LSR flags */
				    if (lsr != LS_XMIT_AVAIL)
				    {
					done = 8;
					break;
				    }
				}
				else
				{
				    for (delay_count = 20000;
					delay_count
					  && !((lsr = sas_inb (fip,
						    LINE_STATUS_PORT))
							& LS_XMIT_COMPLETE);
					delay_count--)
					;

				    if (!delay_count)
				    {
					done = 7;
					break;
				    }

				    /* check LSR flags */
				    if (lsr != (LS_XMIT_AVAIL
							| LS_XMIT_COMPLETE))
				    {
					done = 8;
					break;
				    }
				}
			} while (*((ushort *) (cptr++)));

			if (done)
				break;
		}

		if (done)
			break;
	    }
	}

	if (!done)
	{
	    /* clear delta bits */
	    (void) sas_inb (fip, MDM_STATUS_PORT);

	    for (rep_count = 1000; rep_count; rep_count--)
	    {
		/* test pattern */
		cptr = (unchar *) "\005\142\012\237\006\130\011\257\017\361\0\017\0\0";

		for (; *((ushort *) cptr); cptr += 2)
		{
			/* test modem control and status lines */
			sas_outb (fip, MDM_CTL_PORT, *cptr | MC_SET_LOOPBACK);
			if (sas_inb (fip, MDM_STATUS_PORT) != *(cptr + 1))
			{
				done = 9;
				break;
			}
			if (sas_inb (fip, MDM_STATUS_PORT)
					!= (*(cptr + 1) & ~MS_ANY_DELTA))
			{
				done = 9;
				break;
			}
		}

		if (done)
			break;
	    }
	}

	/* switch back to normal operation */
	sas_outb (fip, MDM_CTL_PORT, 0);

	return (done);
}


/* Handle hangup after last close (called via timeout() at SPLHI) */
static void
sas_hangup (fip)
register struct sas_info	*fip;
{
	int	old_level;
	REGVAR;

	if (fip->device_flags.i & DF_DO_HANGUP)
	{
		/* do the hangup */
		fip->mcr &= ~(fip->modem.m.ei
				| fip->modem.m.eo);
		fip->mcr |= fip->modem.m.di;
		sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
		fip->device_flags.s &= ~(DF_MODEM_ENABLED | DF_DO_HANGUP);
		(void) timeout (sas_hangup, fip, (HANGUP_TIME) * (HZ) / 1000);
	}
	else
	{
		/* If there was a waiting getty open on this
		   port, reopen the physical device.
		*/
		if (fip->o_state & OS_WAIT_OPEN)
		{
			/* allow pending tty interrupts */
			old_level = SPLWRK ();
			(void) splx (old_level);

			sas_open_device (fip);
			sas_param (fip, HARD_INIT,DEFFLAGS);	/* set up port regs */

			/* allow pending tty interrupts */
			old_level = SPLWRK ();
			(void) splx (old_level);
		}
		release_device_lock (fip);
	}
}

/* main timeout function (called via timeout() at SPLHI) */
static void
sas_timeout (fip)
register struct sas_info	*fip;
{
	REGVAR;

	/* handle break request */
	if (fip->device_flags.i & DF_DO_BREAK)
	{
		/* set up break request flags */
		fip->lcr |= LC_SET_BREAK_LEVEL;
		sas_first_outb (fip, LINE_CTL_PORT, fip->lcr);
		fip->device_flags.s &= ~(DF_DO_BREAK | DF_GUARD_TIMEOUT);
		(void) timeout (sas_timeout, fip, (BREAK_TIME) * (HZ) / 1000);
		return;
	}

	/* reset break state */
	if ((fip->device_flags.i & DF_XMIT_BREAK)
		&& (fip->lcr & LC_SET_BREAK_LEVEL))
	{
		fip->lcr &= ~LC_SET_BREAK_LEVEL;
		sas_first_outb (fip, LINE_CTL_PORT, fip->lcr);
		fip->device_flags.s |= DF_GUARD_TIMEOUT;
		fip->timeout_idx = timeout (sas_timeout, fip,
					sas_ctimes [fip->cflag & CBAUD]);
		return;
	}

	fip->device_flags.s &= ~(DF_GUARD_TIMEOUT | DF_XMIT_BREAK);

	if (!qsize(WR(fip->rq)) && !(fip->device_flags.i & DF_XMIT_BUSY))
	{
		*fip->sas_curstate &= ~BUSY;
		sas_hdx_check (fip);
	}

	event_sched (fip, EF_DO_XXFER);

	(void) wakeup ((caddr_t) &(fip)->device_flags.i);
}

/* Half-duplex hardware flow control check. */
static void
sas_hdx_check (fip)
register struct sas_info	*fip;
{
	REGVAR;

	/* don't interfere with hardware input handshake */
	if (fip->flow_flags.i & FF_HWI_HANDSHAKE)
		return;

#if defined (HAVE_VPIX)
	/* don't touch the mcr if we are in dos mode and hdx hardware
	   handshake is disabled (dos handles the handshake line(s)
	   on its own in this mode)
	*/
	if ((fip->iflag & DOSMODE) && !(fip->flow_flags.i & FF_HDX_HANDSHAKE))
		return;
#endif
	if (fip->flow_flags.i & FF_HDX_STARTED)
	{
		/* If output buffer is empty signal the connected
		   device that all output is done.
		*/
		if ((fip->flow_flags.i & FF_HDX_HANDSHAKE)
			&& !(*fip->sas_curstate & BUSY))
		{
			fip->mcr &= ~fip->flow.m.hc;
			sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
			fip->flow_flags.s &= ~FF_HDX_STARTED;
		}
	}
	else
	{
		/* If the output ring buffer contains characters
		   and was previously empty signal the connected
		   device that output is resumed.
		*/
		if (!(fip->flow_flags.i & FF_HDX_HANDSHAKE)
			|| (*fip->sas_curstate & BUSY))
		{
			fip->mcr |= fip->flow.m.hc;
			sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
			fip->flow_flags.s |= FF_HDX_STARTED;
		}
	}
}

/* Output characters to the transmitter register. */
static void
sas_xproc (fip)
register struct sas_info	*fip;
{
	REGVAR;
	register mblk_t *mp = fip->holdbuf;
	register uint	num_to_output,nbytes;

	/* proceed only if transmitter is available */
	if ((fip->device_flags.i & (DF_XMIT_BUSY | DF_XMIT_BREAK
						| DF_XMIT_LOCKED))
		|| (fip->flow_flags.i & FF_HWO_STOPPED)) return;

	num_to_output = fip->xmit_fifo_size;

	/* handle XON/XOFF input flow control requests */
	if (fip->flow_flags.i & FF_SW_FC_REQ)
	{
#if defined (HAVE_VPIX)
		sas_first_outb (fip, XMT_DATA_PORT, (fip->flow_flags.i & FF_SWI_STOPPED)
					? fip->v86_ss.ss_stop
					: fip->v86_ss.ss_start);
#else
		sas_first_outb (fip, XMT_DATA_PORT, (fip->flow_flags.i & FF_SWI_STOPPED)
					? fip->cc[VSTOP]
					: fip->cc[VSTART]);
#endif
		*fip->sas_curstate &= ~(TTXON | TTXOFF);
		fip->device_flags.s |= DF_XMIT_BUSY;
		fip->flow_flags.s &= ~FF_SW_FC_REQ;
		/* disable guard timeout */
		if (fip->device_flags.i & DF_GUARD_TIMEOUT)
		{
			fip->device_flags.s &= ~DF_GUARD_TIMEOUT;
			(void) untimeout (fip->timeout_idx);
		}
		num_to_output--;
	}

	/* bail out if output is suspended by XOFF */
	if (fip->flow_flags.i & FF_SWO_STOPPED) return;

	/* Determine how many chars to put into the transmitter
	   register.
	*/
	while (mp && !(nbytes=mp->b_wptr - mp->b_rptr)) {
		fip->holdbuf = mp->b_cont;
		mp->b_cont = NULL;
		sas_link(&fip->discards, mp);
	/* no characters available ? */
		if (!(mp = fip->holdbuf)) {
			break;
		} else {
			if (mp->b_cont == NULL)
				timeout(sas_qenable, WR(fip->rq), 1);
		}
	}

	if(!mp) {
		if (*fip->sas_curstate & TTIOW) {
			*fip->sas_curstate &= ~(TTIOW|BUSY);
			wakeup((caddr_t) fip->sas_curstate);
		}
		*fip->sas_curstate &= ~BUSY;
		event_sched (fip, EF_DO_XXFER);
		return;
	}

	if (nbytes < num_to_output)
		num_to_output = nbytes;

	sysinfo.outch += num_to_output;

	sas_ctl (fip, XMT_DATA_PORT);

	if (num_to_output == 1) {
		(void) outb (XMT_DATA_PORT.addr, *mp->b_rptr++);
	} else {
		(void) repoutsb(XMT_DATA_PORT.addr, mp->b_rptr, num_to_output);
		mp->b_rptr += num_to_output;
	}

	/* signal that transmitter is busy now */
	fip->device_flags.s |= DF_XMIT_BUSY;
	/* disable guard timeout */
	if (fip->device_flags.i & DF_GUARD_TIMEOUT)
	{
		fip->device_flags.s &= ~DF_GUARD_TIMEOUT;
		(void) untimeout (fip->timeout_idx);
	}
}

/* Main sas interrupt handler. Actual character processing is splitted
   into sub-functions.
*/
int
sasintr (vect)
int	vect;
{
	REGVAR;
	register struct sas_info	*fip;
	register uint	status;
	struct sas_info	*old_fip;
	int	done, drop_mode;
	uint	port, old_recv_count;

	drop_mode = FALSE;

	/* The 8259 interrupt controller is set up for edge trigger.
	   Therefore, we must loop until we make a complete pass without
	   getting any UARTs that are interrupting.
	*/
	do
	{
		done = TRUE;

		fip = sas_first_int_user[vect];
			
		
		/* loop through all users of this interrupt vector */
		for (fip = sas_first_int_user[vect] ; fip ;
			fip = fip->next_int_user)
		{
			/* process only ports that we expect ints from
			   and that actually need to be serviced
			*/
fastloop:
			if (sas_first_inb (fip, INT_ID_PORT)
					& II_NO_INTS_PENDING)
			{
				/* restore the normal receiver trigger level */
				if (fip->device_flags.i & DF_FIFO_DROP_MODE)
				{
					fip->device_flags.s &=
							~DF_FIFO_DROP_MODE;
					sas_outb (fip, NS_FIFO_CTL_PORT,
							NS_FIFO_SETUP_CMD);
				}
				/* speed beats beauty */
				fip = fip->next_int_user;
				if (fip)
					goto fastloop;
				break;
			}

goit:
			/* restore the normal receiver trigger level */
			if (fip->device_flags.i & DF_FIFO_DROP_MODE)
			{
				fip->device_flags.s &= ~DF_FIFO_DROP_MODE;
				sas_outb (fip, NS_FIFO_CTL_PORT,
							NS_FIFO_SETUP_CMD);
			}

			done = FALSE;	/* not done if we got an int */
			old_recv_count = fip->recv_cnt;

			do
			{
				/* read in all the characters from the FIFO */
				if ((status = sas_inb (fip, LINE_STATUS_PORT))
							& LS_RCV_INT)
				{
				    if (!drop_mode && (fip->device_type
							== TYPE_NS16550A))
				    {
					/* Drop receiver trigger levels to make
					   sure that we will see all received
					   characters in all NS16550A. This
					   prevents multiple interrupts if we
					   receive characters on more than one
					   unit.
					*/
					old_fip = fip;
					for (fip = sas_first_int_user [vect];
						fip; fip = fip->next_int_user)
					{
					    if ((fip->device_type
							== TYPE_NS16550A)
						&& (fip != old_fip))
					    {
						fip->device_flags.s |=
							DF_FIFO_DROP_MODE;
						sas_first_outb (fip,
							NS_FIFO_CTL_PORT,
							NS_FIFO_DROP_CMD);
					    }
					}
					fip = old_fip;
					drop_mode = TRUE;
				    }
				    status = sas_rproc (fip, status);
				    sysinfo.rcvint++;
				}

				/* Is it a transmitter empty int ? */
				if ((status & LS_XMIT_AVAIL)
					&& (fip->device_flags.i & DF_XMIT_BUSY))
				{
					fip->device_flags.s &= ~DF_XMIT_BUSY;
					sas_xproc(fip);
					if (!(fip->device_flags.i
							& DF_XMIT_BUSY))
					{
						fip->device_flags.s |=
							DF_GUARD_TIMEOUT;
						fip->timeout_idx =
							timeout (
							sas_timeout, fip,
							sas_ctimes [fip->cflag
								& CBAUD]);
					}
					sysinfo.xmtint++;
				}

				/* Has there been a polarity change on
				   some of the modem lines ?
				*/
				if ((fip->device_flags.i & DF_MSI_ENABLED)
					&& ((status = sas_inb (fip,
							MDM_STATUS_PORT))
						& MS_ANY_DELTA))
				{
					/* if the same modem status line
					   is responsible for a modem status
					   interrupt twice during two
					   event scheduler runs, we disable
					   modem status interrupts until we
					   process this interrupt in the event
					   scheduler
					*/
					if (status & fip->new_msr
							& MS_ANY_DELTA)
					{
						sas_msi_disable (fip);
						sas_msi_noise++;
					}
					/* Do special RING line handling.
					   RING generates an int only on the
					   trailing edge.
					*/
					status = (status & ~MS_RING_PRESENT)
						| (fip->new_msr
							& MS_RING_PRESENT);
					if (status & MS_RING_TEDGE)
						status |= MS_RING_PRESENT;
					if ((status ^ fip->new_msr)
							& MS_ANY_PRESENT)
					{
						/* check hw flow flags */
						sas_fproc (fip, status);
						event_sched (fip, EF_DO_MPROC);
					}
					else
						event_sched(fip,
							EF_RESET_DELTA_BITS);
					/* "or" the delta flags to prevent
					   excessive modem status interrupts
					*/
					fip->new_msr = status
							| (fip->new_msr
								& MS_ANY_DELTA);
					sysinfo.mdmint++;
				}
			} while (!(sas_inb (fip, INT_ID_PORT)
						& II_NO_INTS_PENDING));


			/* Has the input buffer level changed ? */
			if (fip->recv_cnt != old_recv_count)
			{
				/* schedule character transfer to UNIX buffer */
				if (!old_recv_count && !(fip->flow_flags.i
							& FF_RXFER_STOPPED)) {
					event_sched(fip, EF_DO_RXFER);
				}
			}
	/* Check buffer against high water mark 
		if above stop input or dump it  */
			if (!fip->nfree && fip->recv)
			{
				if (fip->flow_flags.i & FF_HWI_HANDSHAKE &&
					!(fip->flow_flags.s & FF_HWI_STOPPED)) {
					fip->mcr &= ~fip->flow.m.ic;
					sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
					fip->flow_flags.s |= FF_HWI_STOPPED;
				} 
				if(fip->iflag & IXOFF &&
					!(fip->flow_flags.s & FF_SWI_STOPPED)) {
					fip->flow_flags.s |= (FF_SWI_STOPPED | FF_SW_FC_REQ);
					*fip->sas_curstate |= (TTXOFF | BUSY);
					sas_hdx_check (fip);
					sas_xproc (fip);
				}
			}
		}
	} while (!done);

	/* clear the shared interrupt since we have scanned all
	   of the ports that share this interrupt vector
	*/	
	if (port = sas_int_ack_port [vect])
		(void) outb (port, sas_int_ack [vect]);

	return (0);
}

/* Receiver interrupt handler. Translates input characters to character
   sequences as described in TERMIO(7) man page.
*/
static uint
sas_rproc (fip, line_status)
register struct sas_info	*fip;
uint	line_status;
{
	REGVAR;
	register unsigned int *ttyp = fip->sas_curstate;
	uint	charac;
	register uint	csize;
	unchar	metta [4];

	sas_first_ctl (fip, RCV_DATA_PORT);

	/* Translate characters from FIFO according to the TERMIO(7)
	   man page.
	*/
	do
	{
		charac = (line_status & LS_RCV_AVAIL)
				? sas_inb (fip, RCV_DATA_PORT)
				: 0;	/* was line status int only */

		/* Was there a receiver overrun ? Count them in a
		   separate counter for each UART type.
		*/
		if (line_status & LS_OVERRUN)
			sas_overrun [fip->device_type]++;

		/* do we have to junk the character ? */
		if (!(fip->cflag & CREAD)
			|| ((*ttyp & (ISOPEN | CARR_ON)) != (ISOPEN | CARR_ON)))
		{
			/* if there are FIFOs we take a short cut */
			if (fip->device_type == TYPE_NS16550A)
				sas_outb (fip, NS_FIFO_CTL_PORT, NS_FIFO_SETUP_CMD
							| NS_FIFO_CLR_RECV);
			else if (fip->device_type == TYPE_I82510)
			{
				sas_outb (fip, I_BANK_PORT, I_BANK_1);
				sas_outb (fip, I_RCM_PORT, I_FIFO_CLR_RECV);
				sas_outb (fip, I_BANK_PORT, I_BANK_0);
			}
			continue;
		}

		csize = 0;

		/* ignore parity errors ? */
		if ((line_status & LS_PARITY_ERROR)
			&& !(fip->iflag & INPCK))
			line_status &= ~LS_PARITY_ERROR;

		/* do we have some kind of character error ? */
		if (line_status & (LS_PARITY_ERROR
					| LS_FRAMING_ERROR
					| LS_BREAK_DETECTED))
		{
#if defined (HAVE_VPIX)
			if (fip->iflag & DOSMODE)
			{
			    if (fip->iflag & PARMRK)
			    {
				/* send status bits to VP/ix */
				(void) sas_vpix_sr (fip, 1,
					(line_status & (LS_PARITY_ERROR
							| LS_FRAMING_ERROR
							| LS_BREAK_DETECTED))
						| LS_RCV_AVAIL
						| LS_XMIT_AVAIL
						| LS_XMIT_COMPLETE);
			    }
			    goto valid_char;
			}
			else
#endif
			/* is it a BREAK ? */
			if (line_status & LS_BREAK_DETECTED)
			{
				if (!(fip->iflag & IGNBRK))
					if (fip->iflag & BRKINT)
					{
						/* do BREAK interrupt */
						event_sched (fip, EF_DO_BRKINT);
					}
					else
					{
						metta [csize] = 0;
						csize++;
						if (fip->iflag & PARMRK)
						{
							metta [csize] = 0;
							csize++;
							metta [csize] = 0xff;
							csize++;
						}
					}
			}
			else if (!(fip->iflag & IGNPAR))
				if (fip->iflag & PARMRK)
				{
					metta [csize] = charac;
					csize++;
					metta [csize] = 0;
					csize++;
					metta [csize] = 0xff;
					csize++;
				}
				else
				{
					metta [csize] = 0;
					csize++;
				}
		}
		else
		/* is there a character to process ? */
		if (line_status & LS_RCV_AVAIL)
		{
			if (fip->iflag & IXON)
			{
				/* do output start/stop handling */
				if (fip->flow_flags.i & FF_SWO_STOPPED)
				{
#if defined (HAVE_VPIX)
					if ((charac == fip->v86_ss.ss_start)
#else
					if ((charac == fip->cc[VSTART])
#endif
						|| (fip->iflag & IXANY))
					{
						fip->flow_flags.s &=
							~FF_SWO_STOPPED;
						*ttyp &= ~TTSTOP;
						/* restart output */
						sas_xproc (fip);
					}
				}
				else
				{
#if defined (HAVE_VPIX)
					if (charac == fip->v86_ss.ss_stop)
#else
					if (charac == fip->cc[VSTOP])
#endif
					{
						fip->flow_flags.s |=
							FF_SWO_STOPPED;
						*ttyp |= TTSTOP;
					}
				}
				/* we don't put start/stop characters
				   into the receiver buffer
				*/
#if defined (HAVE_VPIX)
				if ((charac == fip->v86_ss.ss_start)
					|| (charac == fip->v86_ss.ss_stop))
#else
				if ((charac == fip->cc[VSTART])
					|| (charac == fip->cc[VSTOP]))
#endif
					continue;
			}
valid_char:
			if ((charac == 0xff) && (fip->iflag & PARMRK))
			{
				metta [csize] = 0xff;
				csize++;
				metta [csize] = 0xff;
				csize++;
			}
			else
			{
				/* we take a short-cut if only one character
				   has to be put into the receiver buffer
				*/
				if (fip->recv_cnt < RECV_BUFF_SIZE)
				{
					if (!fip->recv) {
						if ((fip->recv = fip->free) 
							== NULL) {
							continue;
						}
						fip->nfree--;
						fip->free = fip->recv->b_cont;
						fip->recv->b_cont = 0;
						fip->recv_cnt = 0;
					}
					fip->recv_cnt++;
					*fip->recv->b_wptr++ = charac;
					if (fip->recv_cnt == fip->alloc_size) {
						sas_link(&fip->filled, 
								fip->recv);
						fip->recv = NULL;
						fip->recv_cnt = 0;
					}
				}
				continue;
			}
		}
		if (!csize)
			continue;	/* nothing to put into recv buffer */
		if (fip->recv_cnt + csize > RECV_BUFF_SIZE) {
			sas_link(&fip->filled, fip->recv);
			fip->recv = NULL;
			fip->recv_cnt = 0;
			if (!fip->nfree)
				continue;
			fip->nfree--;
			fip->recv = fip->free;
			fip->free = fip->recv->b_cont;
			fip->recv->b_cont = NULL;
		}

		if (!fip->recv) {
			if ((fip->recv = fip->free) == NULL) {
				continue;
			}
			fip->nfree--;
			fip->free = fip->recv->b_cont;
			fip->recv->b_cont = NULL;
		}
		fip->recv_cnt += csize;

		/* store translation in ring buffer */
		do
		{
			*fip->recv->b_wptr++ = (metta - 1) [csize];
		} while (--csize);
	} while ((line_status = sas_inb (fip, LINE_STATUS_PORT)) & LS_RCV_INT);

	return (line_status);
}

/* Asynchronous event handler. Scheduled by functions that can't do the
   processing themselves because of execution time restrictions.
   (called via timeout() at SPLHI)
*/
static void
sas_event (dummy)
void	*dummy;
{
	register struct sas_info	*fip;
	register uint	unit;
	int	old_level;

	/* allow pending tty interrupts */
	old_level = SPLWRK ();
	(void) splx (old_level);

	/* loop through all sas_info structures */
	for (fip = &sas_info [0], unit = 0; unit < sas_physical_units;
			fip++, unit++)
	{
		/* process only structures that actually need to
		   be serviced
		*/
fastloop2:
		if (!fip->event_flags.i)
		{
			/* speed beats beauty */
			fip++;
			if (++unit < sas_physical_units)
				goto fastloop2;
			break;
		}

		/* reset the accumulated msr delta bits */
		if (fip->event_flags.i & EF_RESET_DELTA_BITS)
		{
			fip->event_flags.s &= ~EF_RESET_DELTA_BITS;
			fip->new_msr &= ~MS_ANY_DELTA;
		}

		/* check the modem signals */
		if (fip->event_flags.i & EF_DO_MPROC)
		{
			fip->event_flags.s &= ~EF_DO_MPROC;
			fip->new_msr &= ~MS_ANY_DELTA;
			sas_mproc (fip);
		}

		/* do the break interrupt */
		if (fip->event_flags.i & EF_DO_BRKINT)
		{
			fip->event_flags.s &= ~EF_DO_BRKINT;
			if (*fip->sas_curstate & ISOPEN)
				putctl(fip->rq->q_next,M_BREAK);
		}

		/* transfer characters to the UNIX input buffer */
		if (fip->event_flags.i & EF_DO_RXFER)
		{
			fip->event_flags.s &= ~EF_DO_RXFER;
			if (!(fip->flow_flags.i & FF_RXFER_STOPPED))
			{
				/* if not done, re-schedule this event */
				qenable (fip->rq);
			}
		}

		/* transfer characters to the output ring buffer */
		if (fip->event_flags.i & EF_DO_XXFER)
		{
			fip->event_flags.s &= ~EF_DO_XXFER;
			sas_hdx_check (fip);
			/* output characters */
			qenable (WR(fip->rq));
		}

#if defined (HAVE_VPIX)
		/* send pseudorupt to VP/ix */
		if (fip->event_flags.i & EF_SIGNAL_VPIX)
		{
			fip->event_flags.s &= ~EF_SIGNAL_VPIX;
			if ((fip->iflag & DOSMODE) && fip->v86_proc)
			{
				old_level = SPLWRK ();
				(void) v86setint (fip->v86_proc,
							fip->v86_intmask);
				(void) splx (old_level);
			}
		}
#endif

		if ((fip->device_flags.i & (DF_DEVICE_OPEN | DF_MSI_ENABLED))
			== DF_DEVICE_OPEN)
		{
#if defined (HAVE_VPIX)
			if (!(fip->cflag & CLOCAL)
				|| (fip->flow_flags.i & FF_HWO_HANDSHAKE)
				|| (fip->iflag & DOSMODE))
#else
			if (!(fip->cflag & CLOCAL)
				|| (fip->flow_flags.i & FF_HWO_HANDSHAKE))
#endif
				sas_msi_enable (fip);
		}

		/* allow pending tty interrupts */
		old_level = SPLWRK ();
		(void) splx (old_level);
	}

	event_scheduled = FALSE;

	/* check whether there have been new requests in the mean time */

	fip = &sas_info [0];
	unit = 0;

fastloop3:
	if (!fip->event_flags.i)
	{
		/* speed beats beauty */
		fip++;
		if (++unit < sas_physical_units)
			goto fastloop3;
	}
	else
	{
		/* there is at least one new request, so
		   schedule the next event processing
		*/
		event_scheduled = TRUE;
		(void) timeout (sas_event, (void *) NULL,
				(EVENT_TIME) * (HZ) / 1000);
	}
}

/* modem status handler */
static void
sas_mproc (fip)
register struct sas_info	*fip;
{
	register unsigned int *ttyp = fip->sas_curstate;
	register uint	mdm_status;
	uint	vpix_status;
	int	old_level;

	mdm_status = fip->new_msr;
	fip->new_msr &= ~MS_RING_PRESENT;

	/* Check the carrier detect signal and set the state flags
	   accordingly. Also, if not in clocal mode, send SIGHUP on
	   carrier loss and flush the buffers.
	*/
	if (!(fip->cflag & CLOCAL))
	{
		if (!(~mdm_status & fip->modem.m.ca))
		{
			*ttyp |= CARR_ON;
			/* Unblock getty open only if it is ready to run. */
			if ((*ttyp & WOPEN)
				&& (~fip->msr & fip->modem.m.ca))
				(void) wakeup ((caddr_t) ttyp);
		}
		else
		{
			if (!(~fip->msr & fip->modem.m.ca))
			{
				*ttyp &= ~CARR_ON;
				if(*ttyp & ISOPEN) {
					putctl1(fip->rq->q_next,M_FLUSH,FLUSHW);
					putctl1(fip->rq->q_next,M_HANGUP);
				}
			}
		}
	}

#if defined (HAVE_VPIX)
	if (((fip->iflag & (DOSMODE | PARMRK))
			== (DOSMODE | PARMRK))
		&& (fip->v86_intmask != V86VI_KBD))
	{
		/* prepare status bits for VP/ix */
		vpix_status = (((mdm_status ^ fip->msr) >> 4) & MS_ANY_DELTA)
				| (mdm_status & (MS_CTS_PRESENT
							| MS_DSR_PRESENT
							| MS_DCD_PRESENT));
		if (fip->flow_flags.i & FF_HWO_HANDSHAKE)
		{
			vpix_status &= ~((fip->flow.m.oc | fip->flow.m.oe)
							>> 4);
			vpix_status |= fip->flow.m.oc | fip->flow.m.oe;
		}
		/* send status bits to VP/ix */
		if ((vpix_status & MS_ANY_DELTA)
			&& sas_vpix_sr (fip, 2, vpix_status))
			event_sched (fip, EF_DO_RXFER);
	}
#endif
	fip->msr = mdm_status & ~MS_RING_PRESENT;
}
/* hardware flow control interrupt handler */
static void
sas_fproc (fip, mdm_status)
register struct sas_info	*fip;
register uint	mdm_status;
{
	/* Check the output flow control signals and set the state flag
	   accordingly.
	*/
	if (!(~mdm_status & fip->flow.m.oc)
		|| (~mdm_status & fip->flow.m.oe)
		|| !(fip->flow_flags.i & FF_HWO_HANDSHAKE))
	{
		if (fip->flow_flags.i & FF_HWO_STOPPED)
		{
			fip->flow_flags.s &= ~FF_HWO_STOPPED;
			sas_xproc (fip);
		}
	}
	else
		fip->flow_flags.s |= FF_HWO_STOPPED;
}

/* Disable modem status interrupts. */
static void
sas_msi_disable (fip)
register struct sas_info	*fip;
{
	REGVAR;

	fip->ier &= ~IE_MODEM_STATUS;
	sas_first_outb (fip, INT_ENABLE_PORT, fip->ier);
	fip->device_flags.s &= ~DF_MSI_ENABLED;
}

/* Re-enable modem status interrupts. */
static void
sas_msi_enable (fip)
register struct sas_info	*fip;
{
	REGVAR;

	fip->device_flags.s |= DF_MSI_ENABLED;
	fip->ier |= IE_MODEM_STATUS;
	sas_first_outb (fip, INT_ENABLE_PORT, fip->ier);
}

/* set up a port according to the given termio structure */
static void
sas_param (fip, init_type, cflag)
register struct	sas_info	*fip;
int	init_type;
uint	cflag;
{
	REGVAR;
	uint	fake_cflag;
	uint	divisor;
	int	old_spl;

#if defined (HAVE_VPIX)
	/* we don't set port registers if we are in dos mode */
	if (fip->iflag & DOSMODE)
	{
		/* This is a kludge. We don't know what baud rate
		   DOS will use. Therefore, we assume a rather low
		   one to be on the safe side.
		*/
		cflag = (cflag & ~CBAUD) | B300;
		goto setflags;
	}
#endif
	fip->scflag = cflag;
	/* Make sure that we have a valid baud rate. If we don't
	   get one, take the previous baud rate.
	*/
	fake_cflag = (cflag & ~CBAUD)
			| (((cflag & CBAUD) == B0)
				? (fip->cflag & CBAUD)
				: (fip->device_flags.i & DF_HIGH_SPEED)
					? sas_hbaud [cflag & CBAUD]
					: sas_baud [cflag & CBAUD]);

	/* if soft init mode: don't set port registers if cflag didn't change */
	if ((init_type == SOFT_INIT)
		&& !((cflag ^ ((fip->cflag & ~CBAUD)
				| ((fip->device_flags.i & DF_MODEM_ENABLED)
					? sas_baud [fip->cflag & CBAUD]
					: B0)))
			& (CBAUD | CSIZE | CSTOPB | PARENB | PARODD)))
	{
		cflag = fake_cflag;
		goto setflags;
	}

	/* hangup line if it is baud rate 0, else enable line */
	if ((cflag & CBAUD) == B0)
	{
		if (fip->device_flags.i & DF_MODEM_ENABLED)
		{
			fip->mcr &= (fip->o_state & OS_WAIT_OPEN)
					? ~fip->modem.m.ei
					: ~fip->modem.m.eo;
			sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
			fip->device_flags.s &= ~DF_MODEM_ENABLED;
		}
	}
	else
	{
		if (!(fip->device_flags.i & DF_MODEM_ENABLED))
		{
			fip->mcr |= (fip->o_state & OS_WAIT_OPEN)
					? fip->modem.m.ei
					: fip->modem.m.eo;
			sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
			fip->device_flags.s |= DF_MODEM_ENABLED;
		}
	}

	cflag = fake_cflag;

	/* don't change break flag */
	fip->lcr &= LC_SET_BREAK_LEVEL;

	/* set character size */
	switch (cflag & CSIZE)
	{
	case CS5:
		fip->lcr |= LC_WORDLEN_5;
		break;

	case CS6:
		fip->lcr |= LC_WORDLEN_6;
		break;

	case CS7:
		fip->lcr |= LC_WORDLEN_7;
		break;

	default:
		fip->lcr |= LC_WORDLEN_8;
		break;
	}

	/* set # of stop bits */
	if (cflag & CSTOPB)
		fip->lcr |= LC_STOPBITS_LONG;

	/* set parity */
	if (cflag & PARENB)
	{
		fip->lcr |= LC_ENABLE_PARITY;

		if (!(cflag & PARODD))
			fip->lcr |= LC_EVEN_PARITY;
	}

	/* get counter divisor for selected baud rate */
	divisor = sas_speeds [cflag & CBAUD];
	/* set LCR and baud rate */
	sas_first_outb (fip, LINE_CTL_PORT, fip->lcr | LC_ENABLE_DIVISOR);
	sas_outb (fip, DIVISOR_LSB_PORT, divisor);
	sas_outb (fip, DIVISOR_MSB_PORT, divisor >> 8);
	sas_outb (fip, LINE_CTL_PORT, fip->lcr);
	old_spl = splhi();
	fip->alloc_size = sas_bsizes[cflag & CBAUD];
	if (fip->nfree) {
		fip->nfree = 0;
		sas_link(&fip->discards, fip->free);
		fip->free = NULL;
	}
	(void) splx(old_spl);
	sas_buffers(fip, TRUE);

setflags:
	/* disable modem control signals if required by open mode */
	if (fip->o_state & OS_CLOCAL)
		cflag |= CLOCAL;

	/* Determine whether to enable MSI, or not.
	   Set the interrupt enable port accordingly.
	*/
#if defined (HAVE_VPIX)
	if ((cflag & CLOCAL) && !(fip->flow_flags.i & FF_HWO_HANDSHAKE)
		&& !(fip->iflag & DOSMODE))
#else
	if ((cflag & CLOCAL) && !(fip->flow_flags.i & FF_HWO_HANDSHAKE))
#endif
	{
		if (fip->device_flags.i & DF_MSI_ENABLED)
			sas_msi_disable (fip);
	}
	else
	{
		if (!(fip->device_flags.i & DF_MSI_ENABLED))
			sas_msi_enable (fip);
	}

	/* Fake the carrier detect state flag if CLOCAL mode or if
	   requested by open mode.
	*/
	if (!(~fip->msr & fip->modem.m.ca)
		|| (cflag & CLOCAL)
		|| ((fip->o_state & OS_FAKE_CARR_ON)
			&& (!(fip->o_state & OS_OPEN_STATES)
				|| (*fip->sas_curstate & CARR_ON))))
		*fip->sas_curstate |= CARR_ON;
	else
		*fip->sas_curstate &= ~CARR_ON;

#if defined (XCLUDE)	/* SYSV 3.2 Xenix compatibility */
	/* Permit exclusive use of this device. */
	if (cflag & XCLUDE)
		fip->o_state |= OS_EXCLUSIVE_OPEN_2;
	else
		fip->o_state &= ~OS_EXCLUSIVE_OPEN_2;
#endif

	fip->cflag=cflag;
	/* enable transmitter */
	fip->device_flags.s &= ~DF_XMIT_LOCKED;

	/* setup handshake flags */
	sas_hdx_check (fip);
	sas_fproc (fip, fip->new_msr);

	/* restart output */
	sas_xproc(fip);
}

/* Receiver buffer -> UNIX buffer transfer function. */
int
sas_rsrv (q)
queue_t	*q;
{
	REGVAR;
	register struct sas_info	*fip;
	register int	num_to_xfer;
	mblk_t *mp;
	int	num_save = 0;
	int	old_level;

	fip = ((struct fptr *) q->q_ptr)->fip;
	sas_buffers(fip, TRUE);
	old_level = SPLWRK ();

	/* return if there aren't any characters to transfer */
	if (!fip->recv_cnt && !fip->filled)
	{
		(void) splx (old_level);
		return;
	}

	if(!canput(fip->rq->q_next)) {
		if (fip->flow_flags.i & FF_HWI_HANDSHAKE &&
			!(fip->flow_flags.s & FF_HWI_STOPPED)) {
			fip->mcr &= ~fip->flow.m.ic;
			sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
			fip->flow_flags.s |= FF_HWI_STOPPED;
		}
		if(fip->iflag & IXOFF &&
			!(fip->flow_flags.s & FF_SWI_STOPPED)) {
			fip->flow_flags.s |= (FF_SWI_STOPPED | FF_SW_FC_REQ);
			*fip->sas_curstate |= (TTXOFF | BUSY);
			sas_hdx_check (fip);
			sas_xproc (fip);
		}
		splx(old_level);
		return;
	}
/*	DITTO with M_STARTI */
	if(fip->flow_flags.s & FF_HWI_STOPPED) {
		fip->flow_flags.s &= ~FF_HWI_STOPPED;
		fip->mcr |= fip->flow.m.ic;
		sas_first_outb (fip, MDM_CTL_PORT, fip->mcr);
	}
	if(fip->flow_flags.s & FF_SWI_STOPPED) {
		fip->flow_flags.s &= ~FF_SWI_STOPPED;
		fip->flow_flags.s |= FF_SW_FC_REQ;
		*fip->sas_curstate |= (TTXON | BUSY);
		sas_hdx_check (fip);
		sas_xproc (fip);
	}

#if defined (HAVE_VPIX)
	/* wakeup VP/ix */
	if ((fip->iflag & DOSMODE) && !ttyp->t_rawq.c_cc)
	{
		(void) splx (old_level);
		event_sched (fip, EF_SIGNAL_VPIX);
		old_level = SPLWRK ();
	}
#endif
	mp = NULL;
	(void) splhi();
	mp = fip->filled;
	fip->filled = 0;
	if (fip->recv_cnt) {
		sas_link(&mp, fip->recv);
		fip->recv = NULL;
		fip->recv_cnt = 0;
	}
	SPLWRK();
	
	if (mp) {
		sysinfo.rawch += msgdsize(mp);
		putnext(fip->rq,mp);
	}
	(void) splx(old_level);
}

sas_tcsetx(fip,bp)
register struct sas_info *fip;
register mblk_t *bp;
{
	register struct termiox *xp;
	register struct iocblk *ioss;
	
	ioss = (struct iocblk*)bp->b_rptr;
	xp = STERMIOX(bp);

	if (xp->x_cflag) {
		return(0);
	}
	if ((xp->x_hflag & (RTSXOFF|DTRXOFF)) == (RTSXOFF|DTRXOFF)) {
		return(0);
	}
	if ((xp->x_hflag & (CTSXON|CDXON)) == (CTSXON|CDXON)) {
		return(0);
	}
	if (xp->x_hflag & ISXOFF) {
		return(0);
	}
	/* AS PER TERMIOX(7) if using RTS DTR is open signal
				using DTRXOFF RTS is open signal
				using CTSXON DCD is "carrier"
				using CDXON CTS is "carrier"
							*/
	if (xp->x_hflag & (RTSXOFF|DTRXOFF)) {
		if (xp->x_hflag & RTSXOFF) {
			fip->flow.m.ic = MC_SET_RTS;
			fip->modem.m.eo = MC_SET_DTR;
			fip->modem.m.ei = MC_SET_DTR;
		}
		if (xp->x_hflag & DTRXOFF) {
			fip->flow.m.ic = MC_SET_DTR;
			fip->modem.m.eo = MC_SET_RTS;
			fip->modem.m.ei = MC_SET_RTS;
		}
		fip->flow_flags.s |= FF_HWI_HANDSHAKE;
	} else {
		fip->flow_flags.s &= ~FF_HWI_HANDSHAKE;
	}
	if (xp->x_hflag & (CTSXON|CDXON)) {
		if (xp->x_hflag & CTSXON) {
			fip->flow.m.oc = MS_CTS_PRESENT;
			fip->modem.m.ca = MS_DCD_PRESENT;
		}
		if (xp->x_hflag & CDXON) {
			fip->flow.m.oc = MS_DCD_PRESENT;
			fip->modem.m.ca = MS_CTS_PRESENT;
		}
		fip->flow_flags.s |= FF_HWO_HANDSHAKE;
	} else {
		fip->flow_flags.s &= ~FF_HWO_HANDSHAKE;
	}

	ioss->ioc_count = 0;
	bp->b_datap->db_type = M_IOCACK;
	return(1);
}

static int
sas_tcsets(fip,bp)
register struct sas_info *fip;
register mblk_t *bp;
{
	register struct iocblk *ioss;
	register struct termios *tp;
	register uint cflag;
	register int x;

	ioss = (struct iocblk*)bp->b_rptr;
	tp = STERMIOS(bp);

	cflag = tp->c_cflag;
	for(x=0;x<NCCS;x++)  fip->cc[x] = tp->c_cc[x];

	sas_param(fip,SOFT_INIT,cflag);

	tp->c_cflag = fip->scflag;	/* tell line discipline the results */

	ioss->ioc_count = 0;
	bp->b_datap->db_type = M_IOCACK;

	fip->lflag = tp->c_lflag;
	fip->oflag = tp->c_oflag;
	fip->iflag = tp->c_iflag;

	return(1);
}


static int
sas_tcset(fip,bp)
register struct sas_info *fip;
register mblk_t *bp;
{
	register struct iocblk *ioss;
	register struct termio *tp;
	register uint cflag;
	register int x;

	ioss = (struct iocblk*)bp->b_rptr;
	tp = STERMIO(bp);

	cflag = tp->c_cflag;

	for(x=0;x<NCC;x++)  fip->cc[x] = tp->c_cc[x];

	fip->cc[VSTOP] = CSTOP;
	fip->cc[VSTART] = CSTART; 

	sas_param(fip,SOFT_INIT,cflag);

	tp->c_cflag = fip->scflag;	/* tell line discipline the results */

	ioss->ioc_count = 0;
	bp->b_datap->db_type = M_IOCACK;

	fip->lflag = tp->c_lflag;
	fip->oflag = tp->c_oflag;
	fip->iflag = tp->c_iflag;

	return(1);
}

static
sas_delay(fip)
register struct sas_info *fip;
{
	register int oldpri;

	oldpri = spltty();
	*fip->sas_curstate &= ~RTO;
	fip->sas_timerid = 0;
	qenable(WR(fip->rq));
	splx(oldpri);
}
static int
sas_qenable(q)
queue_t	*q;
{
	(void) qenable(q);
}
static int
sas_buffers(fip, flag)
register struct sas_info *fip;
int	flag;
{
	register mblk_t *mp, *discard;
	int	old_spl;

	old_spl = splhi();
	if (fip->discards) {
		discard = fip->discards;
		fip->discards = NULL;
	} else {
		discard = NULL;
	}
	(void) splx(old_spl);
	if (flag) {
		while (fip->nfree < NUM_RECV_BLKS) {
			mp = NULL;
			while (discard) {
				if (discard->b_datap->db_size <
						fip->alloc_size ||
						discard->b_datap->db_ref != 1) {
					mp = discard->b_cont;
					discard->b_cont = NULL;
					freeb(discard);
					discard = mp;
					continue;
				} 
				mp = discard;
				discard = mp->b_cont;
				mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
				mp->b_cont = NULL;
				break;
			}
			if (!mp) {
				if ((mp = allocb(fip->alloc_size, BPRI_MED)) == NULL) {
					printf("SAS alloc error\n");
				}
			}
			if (mp) {
				(void) splhi();
				mp->b_cont = fip->free;
				fip->free = mp;
				fip->nfree++;
				(void) splx(old_spl);
			} else {
				break;
			}
		}
	}
	if (discard)
		freemsg(discard);
}
static void
sas_link(mp, bp)
register mblk_t	**mp, *bp;
{
	register mblk_t	*lp;
	if (!*mp) {
		*mp = bp;
	} else {
		lp = *mp;
		while (lp->b_cont)
			lp = lp->b_cont;
		lp->b_cont = bp;
	}
}

