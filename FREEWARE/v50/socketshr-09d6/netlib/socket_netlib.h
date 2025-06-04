/*	socket_netlib.h		*/

/*
** 02-Dec-1998	J. Malmberg	Misc. Fixes to allow GCC to compile clean
**		    *		Fix ITEM_LIST structure to match use
*/

#ifndef __NETLIB__
#define __NETLIB__
#define NETLIB


/*
 * min and max buffer sizes for send and receive.  Note that the min buffer
 * size will be set at run time to smaller of `DEF' below or the SYSGEN
 * parameter MAXBUF.
 *
#define TCP_SO_RCVBUF_DEF 8192
#define UDP_SO_RCVBUF_DEF 9216
 *
 * actually, look like the maximum buffer is hardcoded somewhere down in the
 * INETACP.  Through trial and error these are the maximum defaults allowed
 * on my system.
 */
#define TCP_SO_RCVBUF_DEF 3072
#define UDP_SO_RCVBUF_DEF 3072
#define TCP_SO_RCVBUF_MAX 58254
#define UDP_SO_RCVBUF_MAX 58254

/*
 * Structures
 */

/*
 * Attached to a socket that is listening for connections.
 */
struct backlogEntry {
	struct	backlogEntry *flink;
	int	sock;
};

typedef struct {
  unsigned short iosb_status;
  unsigned short iosb_byte_count;
  unsigned long iosb_reserved;
} IO_Status_Block;

/*
 * Internal structure definition of individual socket file descriptors.
 */
struct FD_ENTRY {
  int			domain;		/* domain of socket AF_INET	      */
  int			type;		/* type of socket stream or datagram  */
  int			protocol;	/* protocol of socket		      */
  int			sock_opts;	/* socket options		      */
  int			ioctl_opts;	/* ioctl/fcntl/file options	      */
  int			flags;		/* socket state flags		      */
#define SD_BIND		1		/* is bound			      */
#define SD_CONNECTED	2		/* is connected			      */
#define SD_LISTENING	4		/* active listen		      */

  int			mylen;		/* local socket address name	      */
  struct sockaddr	my;
  int	 		fromlen;	/* from socket address name	      */
  struct sockaddr	from;
  int			tolen;		/* to socket address name	      */
  struct sockaddr	to;

  unsigned long		ef;		/* socket priviate event flag	      */
  unsigned short int	chan;		/* channel assigned to this socket    */
  void			*ctx;		/* NETLIB context pointer	      */
  void			*accept_ctx;	/* NETLIB context pointer	      */
  unsigned short int	accept_chan;	/* channel for accept socket	      */
  IO_Status_Block	read_iosb;	/* qio completion status block	      */
  IO_Status_Block	write_iosb;	/* qio completion status block	      */

  int			listen_socket;	/* socket that listen was from	      */
  int			backlog;	/* max number of connection to accept */
  int			backlogSize;	/* number of connections active	      */
  struct backlogEntry	*backlogQueue;	/* the backlog queue		      */

  int			rcvbufsize;	/* receive buffer size		      */
  int			rcvbufoffset;	/* offset for partial receive	      */
  char			*rcvbuf;	/* receive buffer		      */
  struct sockaddr	rcvfrom;	/* received address name (UDP)	      */
  struct dsc$descriptor read_dsc;	/* TCP read buffer descriptor	      */
  struct dsc$descriptor write_dsc;	/* TCP write buffer descriptor	      */
  FILE			*fptr;		/* file pointer for stream I/O	      */
};

struct ITEM_LIST {
  unsigned short    itm$w_length;
  unsigned short    itm$w_itmcode;
  void *            itm$a_bufaddr;
  unsigned short *  itm$a_retlen;
};

#endif /* NETLIB */
