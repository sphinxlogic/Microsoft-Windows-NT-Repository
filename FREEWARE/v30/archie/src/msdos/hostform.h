/* hfile.inc - placed into all .h files to set up for PVCS 
   $Header:   E:/pcdirs/vcs/hostform.h_v   1.0   15 Jan 1990 19:30:22   bkc  $
   Revision History ----------------------------------------------------
   $Log:   E:/pcdirs/vcs/hostform.h_v  $
 * 
 *    Rev 1.0   15 Jan 1990 19:30:22   bkc
*/


/*
*  Host and local machine configuration information.
*
****************************************************************************
*                                                                          *
*      NCSA Telnet for the PC                                              *
*      by Tim Krauskopf, VT100 by Gaige Paulsen, Tek by Aaron Contorer     *
*                                                                          *
*      National Center for Supercomputing Applications                     *
*      152 Computing Applications Building                                 *
*      605 E. Springfield Ave.                                             *
*      Champaign, IL  61820                                                *
*                                                                          *
*      This program is in the public domain.                               *
*                                                                          *
****************************************************************************
*/

/*
*  Linked list of structures which describe other machines.
*  Arranged one per session with unique session names.
*/

struct machinfo {
	unsigned char 
		*sname,					/* pointer to name of session */
		*hname,                 /* pointer to name of that machine */
		*font,					/* font name, if we can do it */
		*keymap,				/* pointer to keymap file name */
		hostip[4],				/* IP number of this machine */
		gateway,				/* gateway preference, start with 1 */
		nameserv,				/* nameserver preference, start with 1 */
		bksp,					/* backspace value */
		halfdup,				/* half duplex required */
		crmap,					/* Strange Berkeley 4.3 CR mode needed */
		vtwrap,					/* flag on when need wrap mode */
		vtmargin;				/* col to ring bell at */
	int
		clearsave,				/* whether to save cleared lines */
		fsize,					/* font size in points */
		nfcolor[3],				/* normal foreground */
		nbcolor[3],				/* normal background */
		bfcolor[3],				/* blink             */
		bbcolor[3],
		ufcolor[3],             /* underline */
		ubcolor[3],
		mno,					/* machine number for reference */
		mstat,					/* status of this machine entry */
		bkscroll,				/* how many lines to save */
		retrans,				/* initial retrans timeout */
		conto,					/* time out in seconds to wait for connect */
		window,					/* window, will be checked against buffers */
		maxseg,					/* maximum receivable segment size */
		mtu,					/* maximum transfer unit MTU (out) */
                domainsremaining,                       /* how many domain search list entries remain */
                destport,                               /* yepper, you can telnet to a different port than 23 */
		flags;					/* general flags holder */		
#define	MFLAGS_SCRIPT	0x80
	struct machinfo *next;		/* surprise, its a linked list! */
};

struct machinfo *Sgethost(),*Shostlook(),*Slooknum(),*Slookip(),*Smadd();

/*
*  status fields for mstat, what do we know about that machine?
*/
#define NOIP 1					/* we don't have IP number */
#define UDPDOM 3				/* there is a UDP request pending on it */
/*  The next 20 numbers are reserved for UDPDOM */
#define HAVEIP 50				/* at least we have the # */
#define HFILE 70				/* we have IP number from host file */
#define DOM 71					/* we have an IP number from DOMAIN */
#define FROMKIP 72				/* have IP# from KIP server */



/*
*   Configuration information which 
*   the calling program may want to obtain from the hosts file.
*   The calling program should include hostform.h and call
*   Sgetconfig(cp)
*     struct config *cp;
*   which will copy the information to the user's data structure.
*/
struct config {
	unsigned char
		netmask[4],				/* subnetting mask being used */
		havemask,				/* do we have a netmask? */
		irqnum,					/* which hardware interrupt */
		myipnum[4],				/* what is my IP #? */
		me[32],					/* my name description (first 30 chars) */
		color[3],				/* default colors to use */
		hw[10],					/* hardware type for network */
		video[10],				/* video graphics hardware available */
		bios,					/* flag, do we want to use BIOS for screen access */
		tek,					/* flag, enable tektronix graphics */
		ftp,					/* flag, enable ftp server */
		rcp,					/* flag, enable rcp server */
		comkeys,				/* flag, commandkeys=yes */
		*termtype,				/* terminal type specification */
		*zone,					/* AppleTalk zone for KIP NBP */
		*defdom,				/* default domain */
		*capture,				/* pointer to where the capture file name is */
		*pass,					/* pointer to where the password file name is */
		*hpfile,				/* HP file name */
		*psfile,				/* PS file name */
		*tekfile;				/* tek file name */
int
		nstype,					/* nameserver = 1-domain 2-IEN116  */
		domto,					/* time out for UDP domain request */
		ndom,					/* number of retries for domain requests */
		timesl,					/* time slice */
		address,				/* segment address */
		ioaddr;					/* I/O address */
char            *domainpath;                               /* domain name search path */
char            *map3270;                               /* path to 3270 map */
char            tnmode;                                 /* how we decide to connect to 3270 streams */
long		sys_flags;
#define	SYS_FLAGS_VISUAL_BELL	0x1		/* use visual bell */

};


#define TNMODE_FLAGGED  0
#define TNMODE_AUTO     1
#define TNMODE_NOTFLAGGED 2
#define TNMODE_OFF      3
