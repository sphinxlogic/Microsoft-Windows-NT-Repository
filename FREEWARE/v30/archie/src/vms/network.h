/* Miscellaneous things for the networking library.  */

/* Actually an itemlist_3, but can be used for itemlist_2's.  */
struct itemlist
{
  short length;
  short code;
  char *dataptr;
  short *retlenptr;
};

union socket_addr
{
  struct sockaddr_in in;
};

#define   TCP$SEND        (IO$_WRITEVBLK)
#define   TCP$RECEIVE     (IO$_READVBLK)
#define   TCP$OPEN        (IO$_CREATE)
#define   TCP$CLOSE       (IO$_DELETE)
#define   TCP$ABORT       (IO$_DEACCESS)
#define   TCP$STATUS      (IO$_ACPCONTROL)
#define   TCP$INFO        (IO$_MODIFY)
#define   GTHST           (IO$_SKIPFILE)

#define   IO$_SEND        (IO$_WRITEVBLK)
#define   IO$_RECEIVE     (IO$_READVBLK)
#ifndef IO$S_FCODE
#define IO$S_FCODE 0x0006
#endif
#define   IO$_SOCKET      (IO$_ACCESS | (0 << IO$S_FCODE))
#define   IO$_BIND        (IO$_ACCESS | (1 << IO$S_FCODE))
#define   IO$_LISTEN      (IO$_ACCESS | (2 << IO$S_FCODE))
#define   IO$_ACCEPT      (IO$_ACCESS | (3 << IO$S_FCODE))
#define   IO$_CONNECT     (IO$_ACCESS | (4 << IO$S_FCODE))
#define   IO$_SETSOCKOPT  (IO$_ACCESS | (5 << IO$S_FCODE))
#define   IO$_GETSOCKOPT  (IO$_ACCESS | (6 << IO$S_FCODE))
#define   IO$_IOCTL       (IO$_ACCESS | (8 << IO$S_FCODE))
#define   IO$_ACCEPT_WAIT (IO$_ACCESS | (10 << IO$S_FCODE))
#define   IO$_NETWORK_PTY (IO$_ACCESS | (11 << IO$S_FCODE))
#define   IO$_SHUTDOWN    (IO$_ACCESS | (12 << IO$S_FCODE))
#define   IO$_GETSOCKNAME (IO$_ACCESS | (13 << IO$S_FCODE))
#define	  SETCHAR_HANDOFF (1<<2)

#define   NFB$C_DECLNAME   0x15

#define TIMER_EFN 1
#define TERM_EFN  2
#define BUF_SIZE 2000

#define INITIALISED 0
#define ACTIVE_CONNECTION 1
#define PASSIVE_CONNECTION 2
#define LISTENING 3
#define HANDED_OFF 4

static struct fd_entry
{
  unsigned short int channel;	/* vms channel assigned to this socket */
  unsigned short int iosb[4];	/* returned status block */
  int fd_buff_size;		/* number of chrs in buffer still to be read */
  int accept_pending;		/* a call is waiting to be accepted */
  int connect_pending;		/* a connect is outstanding*/
  int connected;		/* this descriptor is connected */
  unsigned char *fd_buff;	/* pointer to buffer dyn assigned */
  unsigned char *fd_leftover;	/* pointer to any chrs still to be read */
  FILE *fptr;			/* we need to assgn a file ptr for stream io */
  int s;			/* socket number - needed in the ast's */
  int namelen;			/* our socket address name */
  union socket_addr name;
  short int fromdummy;		/* wg - accept wants an int - recvfrom wants a short!!*/
  short int fromlen;		/* the from socket address name */
  union socket_addr from;
  int tolen;			/* wg - sendto wants an int*/
  union socket_addr to;		/* the to socket address name */
  int passive;			/* still needed because of x25 close ambig */
  int backlog;			/* backlog - not handled well! */
  int domain;			/* domain of socket AF_INET or AF_X25 */
  int type;			/* type of socket stream or datagram */
  int protocol;			/* protocol of socket - ignored */
  int mbx_channel;		/* mailbox channel - needed for x25 */
  unsigned char mbx_buff[255];	/* mailbox buffer */
  unsigned short int miosb[4];	/* mailbox status block */
  int ncb_size;			/* x25 connection information */
  unsigned char ncb[128];
  unsigned char masklen;	/* x25 user data mask */
  unsigned char mask[16];
  int need_header;		/* x25 header field gives data status if req*/
  int send_type;		/* x25 data packet type eg more bit set etc */
  int status;			/* status of socket */
  int closed_by_remote;		/* flag for remote dropouts */
  int read_outstanding;		/* flag so we don't hang two reads */
  int cmu_open;			/* flag to say whether a cmu open was hung */
  int x25_listener;		/* flag to say we are an x25 listener */
  int oob_type;			/* handles interrupt messages */
  int mother;			/* mother socket for X25 accepts */
  int child;			/* child socket for X25 accepts */
  int no_more_accepts;		/* don't accept anymore calls */
  char int_data;		/* interrupt data - only 1 char supported */
  int non_blocking;		/* don't block on a read if no data */
  int sig_req;			/* generate SIGIO on data ready */
  struct itemlist rhost;	/* descriptor pointing to "p[].from" info for UCX */
  unsigned short ucx_accept_chan;	/* Channel returned by a UCX accept via hang_an_accept */
} p[32];

/* So we can handle select on terminal input.  */
static struct term_entry
{
  int chan;
  short int iosb[4];
  short int char_available;
  short int read_outstanding;
  char c[1];
} terminal =

{
  -1, 0, 0, 0, 0, 0, 0
};

#define CMU 1
#define WG  2
#define NONE 3
#define TGV 4
#define UCX 5
static int tcp_make = 0;

struct descriptor
{
  int size;
  char *ptr;
};

/* Initialize certain things 1st time thru.  */
static int p_initialised = 0;

/* A routine to point SIGALRM and SIGURG at.  */
static int 
si_dummy ()
{
}
static int (*alarm_function) () = si_dummy;
static int (*sigurg_function) () = si_dummy;

FILE *fdopen ();
static set_tcp_make ();
static char *getdevicename ();
