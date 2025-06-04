/*
    Structure for storing VMS specific information for an EMACS process
*/

/*
    Same as MAXDESC in process.c
*/

/* IO status block for mailbox operations.  */
struct mbx_iosb
{
  short status;
  short size;
  int   pid;
};


/* define the structure of the buffers used for communicating with the pty
   driver.  each buffer is exactly one page long. */

#define PTYBUF_SIZE (PAGESIZE - 2*sizeof(short))
#define PTY_BUFFERS 2
#define PTY_READBUF 0

struct ptybuf
{
  short stat;
  short len;
  char buf[PTYBUF_SIZE];
};

typedef  struct {
  unsigned long chan;		/* UCX, Multinet and NETLIB (keep it 32 bit) */
  short	busy : 1;		/* true when in use */
  short is_pty : 1;		/* true when this is a pty */
  short is_net : 1;		/* true when this is a network connection */
  short	is_kbd : 1;		/* true when this is the keyboard connection */
  int	eventFlag;
  /* i/o buffers and iosb's */
  union
  {
    struct
    {
      int pty_lastlen[PTY_BUFFERS];
      struct ptybuf *pty_buffers;
    } pty;

    struct
    {
      char *mbx_buffer;
      struct mbx_iosb iosb;
    } mbx;

    struct
    {
      struct dsc$descriptor net_buffer;
      /* char *net_buffer; */
      struct mbx_iosb iosb;
    } net;
  } a;
} VMS_CHAN_STUFF;

#define	MAX_VMS_CHAN_STUFF 32

typedef struct
{
  int		active;
  Lisp_Object	process;
  int		statusCode;
  int		translate_p;
} VMS_PROC_STUFF;

#define	MAX_VMS_PROC_STUFF 32

#define MSGSIZE 1024            /* Maximum size for mailbox operations */
#define NETBUFSIZ 1024		/* Maximum size for network operations */
