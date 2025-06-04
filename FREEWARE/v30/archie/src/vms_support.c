/* Emulation of 4.2 UNIX socket interface routines includes drivers for
   Wollongong, CMU-TEK, UCX tcp/ip interface and also emulates the SUN
   version of X.25 sockets.  The TWG will also work for MultiNet.  */

/* This is from unixlib, by P.Kay@massey.ac.nz; wonderful implementation.
   You can get the real thing on 130.123.1.4 as unixlib_tar.z.  */

#include <stdio.h>
#include <errno.h>
#include <ssdef.h>
#include <dvidef.h>
#include <signal.h>
#include <sys$library:msgdef.h>
#include <iodef.h>
#include <ctype.h>
#include <vms.h>
#include "[.vms]network.h"

#define QIO_FAILED (st != SS$_NORMAL || p[s].iosb[0] != SS$_NORMAL)
#define QIO_ST_FAILED (st != SS$_NORMAL)

/* Socket routine.  */
int
VMSsocket (domain, type, protocol)
     int domain, type, protocol;
{
  struct descriptor inetdesc, x25desc, mbxdesc;
  int i, st, s, p_initialise ();
  long ucx_sock_def;
  char *getenv ();

  if (!tcp_make)
    set_tcp_make ();

  if (p_initialised == 0)
    {
      for (i = 0; i < 32; i++)
	p_initialise (i);

      p_initialised = 1;
    }

  /* First of all, get a file descriptor and file ptr we can associate with
     the socket, allocate a buffer, and remember the socket details.  */
  s = dup (0);
  if (s > 31)
    {
      errno = EMFILE;
      close (s);
      return -1;
    }

  p[s].fptr = fdopen (s, "r");
  p[s].fd_buff = (unsigned char *) malloc (BUF_SIZE);
  p[s].domain = domain;
  p[s].type = type;
  p[s].protocol = protocol;

  /* Handle the case of INET and X.25 separately.  */
  if (domain == AF_INET)
    {
      if (tcp_make == NONE)
	{
	  printf ("Trying to obtain a TCP socket when we don't have TCP!\n");
	  exit (1);
	}
      if (tcp_make == CMU)
	{
	  /* For CMU we need only assign a channel.  */
	  inetdesc.size = 3;
	  inetdesc.ptr = "IP:";
	  if (sys$assign (&inetdesc, &p[s].channel, 0, 0) != SS$_NORMAL)
	    return -1;
	}
      else if (tcp_make == UCX)
	{
	  /* For UCX assign channel and associate a socket with it.  */
	  inetdesc.size = 3;
	  inetdesc.ptr = "BG:";
	  if (sys$assign (&inetdesc, &p[s].channel, 0, 0) != SS$_NORMAL)
	    return -1;

	  ucx_sock_def = (domain << 24) + (type << 16) + protocol;
	  st = sys$qiow (0, p[s].channel, IO$_SETMODE, p[s].iosb, 0, 0,
			 &ucx_sock_def, 0, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;
	}
      else
	{
	  /* For TWG we assign the channel and associate a socket with it.  */
	  inetdesc.size = 7;
	  inetdesc.ptr = "_INET0:";

	  if (sys$assign (&inetdesc, &p[s].channel, 0, 0) != SS$_NORMAL)
	    return -1;

	  st = sys$qiow (0, p[s].channel, IO$_SOCKET, p[s].iosb, 0, 0,
			 domain, type, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domains yet.  */
    return -1;

  /* For each case if we are successful we return the descriptor.  */
  return s;
}

/* Bind routine.  */
VMSbind (s, name, namelen)
     int s;
     union socket_addr *name;
     int namelen;
{
  char infobuff[1024], lhost[32];
  int st;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].domain == AF_INET)
    {
      /* One main problem with bind is that if we're given a port number
	 of 0, then we're expected to return a unique port number.  Since
	 we don't KNOW, we return 1050+s and look to Lady Luck.  */
      if (tcp_make == CMU)
	{
	  if (name->in.sin_port == 0 && p[s].type != SOCK_DGRAM)
	    name->in.sin_port = 1050 + s;
	  p[s].namelen = namelen;
	  bcopy (name, &(p[s].name), namelen);

	  if (p[s].type == SOCK_DGRAM)
	    {
	      /* Another problem is that CMU still needs an OPEN request
		 even if it's a datagram socket.  */
	      st = sys$qiow (0, p[s].channel, TCP$OPEN, p[s].iosb,
			     0, 0, 0, 0, ntohs (p[s].name.in.sin_port),
			     0, 1, 0);
	      if (QIO_ST_FAILED)
		return -1;

	      p[s].cmu_open = 1;
	      sys$qiow (0, p[s].channel, TCP$INFO, p[s].iosb,
			0, 0, &infobuff, 1024, 0, 0, 0, 0);
	      bcopy (infobuff + 264, &(p[s].name.in.sin_port), 2);
	      p[s].name.in.sin_port = htons (p[s].name.in.sin_port);

	      /* So get it another way.  */
	      bcopy (infobuff + 136, lhost, infobuff[1]);
	      lhost[infobuff[1]] = '\0';
	      sys$qiow (0, p[s].channel, GTHST, p[s].iosb,
			0, 0, &infobuff, 1024, 1, lhost, 0, 0);
	      bcopy (infobuff + 4, &(p[s].name.in.sin_addr), 4);

	      /* Be prepared to receive a message.  */
	      hang_a_read (s);
	    }
	}
      else if (tcp_make == UCX)
	{
	  /* UCX will select a prot for you.  If the port's number is 0,
	     translate "name" into an item_2 list.  */
	  struct itemlist lhost;
	  lhost.length = namelen;
	  lhost.code = 0;
	  lhost.dataptr = (char *) name;

	  st = sys$qiow (0, p[s].channel, IO$_SETMODE, p[s].iosb, 0, 0,
			 0, 0, &lhost, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  if (p[s].type == SOCK_DGRAM)
	    hang_a_read (s);

	}
      else
	{
	  /* WG is more straightforward */
	  st = sys$qiow (0, p[s].channel, IO$_BIND, p[s].iosb,
			 0, 0, name, namelen, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  /* If it's a datagram, get ready for the message.  */
	  if (p[s].type == SOCK_DGRAM)
	    hang_a_read (s);
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;

  return 0;
}

/* Connect routine.  */
VMSconnect (s, name, namelen)
     int s;
     union socket_addr *name;
     int namelen;
{
  int pr, fl, st;
  char *inet_ntoa ();
  static struct
  {
    int len;
    char name[128];
  } gethostbuf;
  extern int connect_ast ();

  if (!tcp_make)
    set_tcp_make ();

  /* For datagrams we need to remember who the name was so we can send all
     messages to that address without having to specify it all the time.  */
  if (p[s].connected)
    {
      if (p[s].connected == 1)
	errno = EISCONN;
      else
	{
	  errno = ECONNREFUSED;
	  p[s].connected = 0;
	}
      return -1;
    }

  if (p[s].connect_pending)
    {
      errno = EALREADY;
      return -1;
    }

  p[s].passive = 0;
  p[s].tolen = namelen;
  bcopy (name, &(p[s].to), namelen);

  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{

	  /* Get the info about the remote host  and open up a connection.  */
	  st = sys$qiow (0, p[s].channel, GTHST, p[s].iosb, 0, 0, &gethostbuf,
			 132, 2, name->in.sin_addr.s_addr, 0, 0);
	  if (QIO_FAILED)
	    {
	      strcpy (gethostbuf.name, inet_ntoa (name->in.sin_addr.s_addr));
	      gethostbuf.len = strlen (gethostbuf.name);
	    }
	  gethostbuf.name[gethostbuf.len] = 0;

	  /* TCP */
	  pr = 0;
	  /* Active */
	  fl = 1;

	  /* Nothing else for datagrams.  */
	  if (p[s].type == SOCK_DGRAM)
	    return (0);
	  st = sys$qio (s, p[s].channel, TCP$OPEN, p[s].iosb, connect_ast,
			&p[s], &(gethostbuf.name), ntohs (name->in.sin_port),
			ntohs (p[s].name.in.sin_port), fl, pr, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else if (tcp_make == UCX)
	{
	  /* Both UDP and TCP can use a connect - IO$_ACCESS */
	  p[s].rhost.length = namelen;
	  p[s].rhost.code = 0;
	  p[s].rhost.dataptr = (char *) name;

	  st = sys$qio (s, p[s].channel, IO$_ACCESS, p[s].iosb, connect_ast,
			&p[s], 0, 0, &p[s].rhost, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else
	{
	  /* TWG */
	  if (p[s].type == SOCK_DGRAM)
	    return (0);
	  st = sys$qio (s, p[s].channel, IO$_CONNECT, p[s].iosb, connect_ast,
			&p[s], name, namelen, 0, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;

  if (p[s].non_blocking)
    {
      if (p[s].connected)
	{
	  if (p[s].connected == 1)
	    return 0;
	  else
	    {
	      p[s].connected = 0;
	      errno = ECONNREFUSED;
	      return -1;
	    }
	}
      else
	{
	  p[s].connect_pending = 1;
	  errno = EINPROGRESS;
	  return -1;
	}
    }
  else
    {
      /* wait for the connection to occur */
      if (p[s].connected)
	{
	  if (p[s].connected == 1)
	    return 0;
	  else
	    {
	      p[s].connected = 0;
	      errno = ECONNREFUSED;
	      return -1;
	    }
	}

      /* Timed out? */
      if (wait_efn (s) == -1)
	return -1;

      if (p[s].connected != SS$_NORMAL)
	{
	  errno = ECONNREFUSED;
	  return -1;
	}

      return 0;
    }
}

/* Listen routine.  */
VMSlisten (s, backlog)
     int s;
     int backlog;
{
  int st;

  if (!tcp_make)
    set_tcp_make ();

  p[s].passive = 1;
  p[s].backlog = backlog;
  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  /* For the CMU sockets we can't do the open call in listen;
	     we have to do it in hang_an_accept, because when we close
	     off the connection we have to be ready to accept another
	     one.  accept() also calls hang_an_accept on the old
	     descriptor.  */

	  /* Nothing */
	}
      else if (tcp_make == UCX)
	{

	  /* Doc Verbage sez backlog is descriptor of byte.  Doc examples
	     and common sense say backlog is value.  Value doesn't work,
	     so let's try descriptor of byte after all.  */
	  struct descriptor bl;
	  unsigned char ucx_backlog;

	  ucx_backlog = (unsigned char) backlog;
	  bl.size = sizeof (ucx_backlog);
	  bl.ptr = (char *) &ucx_backlog;

	  st = sys$qiow (0, p[s].channel, IO$_SETMODE, p[s].iosb, 0, 0,
			 0, 0, 0, &bl, 0, 0);
	  if (QIO_FAILED)
	    return -1;
	}
      else
	{
	  /* TWG */
	  st = sys$qiow (0, p[s].channel, IO$_LISTEN, p[s].iosb, 0, 0,
			 backlog, 0, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;

  p[s].status = LISTENING;
  hang_an_accept (s);
  return 0;
}

/* Accept routine.  */
int
VMSaccept (s, addr, addrlen)
     int s;
     union socket_addr *addr;
     int *addrlen;
{
  int news, st;
  struct descriptor inetdesc;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].non_blocking && !p[s].accept_pending)
    {
      errno = EWOULDBLOCK;
      return -1;
    }

  /* hang_an_accept set up an incoming connection request so we have first
     to hang around until one appears or we time out.  */
  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  char infobuff[1024];

	  /* Timed out?  */
	  if (wait_efn (s) == -1)
	    return -1;

	  /* Ok, get a new descriptor ...  */
	  news = dup (0);
	  if (news > 31)
	    {
	      errno = EMFILE;
	      close (news);
	      return -1;
	    }

	  /* ... and copy all of our data across.  */
	  bcopy (&p[s], &p[news], sizeof (p[0]));

	  /* But not this field, of course! */
	  p[news].s = news;

	  sys$qiow (0, p[news].channel, TCP$INFO, p[news].iosb,
		    0, 0, &infobuff, 1024, 0, 0, 0, 0);

	  /* Copy across the connection info if necessary.  */
	  if (addr != 0)
	    {
	      *addrlen = sizeof (struct sockaddr_in);
	      bcopy (infobuff + 132, &(addr->in.sin_port), 2);
	      addr->in.sin_port = htons (addr->in.sin_port);
	      addr->in.sin_family = AF_INET;
	      bcopy (infobuff + 272, &(addr->in.sin_addr), 4);
	      p[news].fromlen = *addrlen;
	      bcopy (addr, &(p[news].from), *addrlen);
	    }
	  p[news].status = PASSIVE_CONNECTION;

	  /* Get a new file ptr for the socket.  */
	  p[news].fptr = fdopen (news, "r");

	  /* Reset this field.  */
	  p[news].accept_pending = 0;

	  /* Allocate a buffer.  */
	  p[news].fd_buff = (unsigned char *) malloc (BUF_SIZE);
	  p[news].fd_leftover = 0;

	  /* Be prepared to get msgs.  */
	  hang_a_read (news);

	  /* Now fix up our previous socket so it's again listening
	     for connections.  */
	  inetdesc.size = 3;
	  inetdesc.ptr = "IP:";
	  if (sys$assign (&inetdesc, &p[s].channel, 0, 0) != SS$_NORMAL)
	    return -1;
	  p[s].status = LISTENING;
	  hang_an_accept (s);

	  /* Return the new socket descriptor.  */
	  return news;
	}
      else if (tcp_make == UCX)
	{
	  /* UCX does the actual accept from hang_an_accept.  The accept info
	    is put into the data structure for the "listening" socket.
	    These just need to be copied into a newly allocated socket for
	    the connect and the listening socket re-started.  */

	  /* Wait for event flag from accept being received inside
	     of hang_an_accept().  */

	  if (wait_efn (s) == -1)
	    /* Timed out.  */
	    return -1;

	  /* Ok, get a new descriptor ...  */
	  news = dup (0);
	  if (news > 31)
	    {
	      errno = EMFILE;
	      close (news);
	      return -1;
	    }
	  /* ... and copy all of our data across.  */
	  bcopy (&p[s], &p[news], sizeof (p[0]));
	  p[news].s = news;	/* but not this field */
	  p[news].channel = p[s].ucx_accept_chan;

	  /* Initialize the remote host address item_list_3 struct.  */
	  p[news].rhost.length = sizeof (struct sockaddr_in);
	  p[news].rhost.code = 0;
	  p[news].rhost.dataptr = (char *) &p[news].from;
	  p[news].rhost.retlenptr = &p[news].fromdummy;

	  if (addr != 0)
	    {
	      /* Return the caller's info, if requested.  */
	      *addrlen = p[news].fromdummy;
	      bcopy (&p[news].from, addr, p[news].fromdummy);
	    }

	  /* Finish fleshing out the new structure.  */
	  p[news].status = PASSIVE_CONNECTION;

	  /* Get a new file pointer for the socket.  */
	  p[news].fptr = fdopen (news, "r");

	  /* Reset this field.  */
	  p[news].accept_pending = 0;

	  /* Allocate a buffer.  */
	  p[news].fd_buff = (unsigned char *) malloc (BUF_SIZE);
	  p[news].fd_leftover = 0;

	  /* Get it started reading.  */
	  hang_a_read (news);

	  p[s].status = LISTENING;
	  hang_an_accept (s);

	  return news;
	}
      else
	{
	  /* TWG */
	  struct descriptor inetdesc;
	  int size;

	  /* Time out?  */
	  if (wait_efn (s) == -1)
	    return -1;

	  /* Ok, get a new descriptor ...  */
	  news = dup (0);
	  if (news > 31)
	    {
	      errno = EMFILE;
	      close (news);
	      return -1;
	    }

	  /* Assign a new channel.  */
	  inetdesc.size = 7;
	  inetdesc.ptr = "_INET0:";
	  st = sys$assign (&inetdesc, &p[news].channel, 0, 0);
	  if (QIO_ST_FAILED)
	    {
	      p[s].accept_pending = 0;
	      sys$clref (s);
	      return -1;
	    }

	  /* From info needs an int length field! */
	  size = sizeof (p[s].from) + 4;
	  st = sys$qiow (0, p[news].channel, IO$_ACCEPT, p[news].iosb, 0, 0,
			 &p[s].fromdummy, size, p[s].channel, 0, 0, 0);

	  if (QIO_ST_FAILED || p[news].iosb[0] != SS$_NORMAL)
	    {
	      p[s].accept_pending = 0;
	      sys$clref (s);
	      return -1;
	    }

	  if (addr != 0)
	    {
	      /* Return the caller's info if requested.  */
	      *addrlen = p[s].fromdummy;
	      bcopy (&p[s].from, addr, *addrlen);
	    }

	  /* Fix up our new data structure.  */
	  p[news].status = PASSIVE_CONNECTION;
	  p[news].domain = AF_INET;
	  p[news].passive = 1;
	  p[news].fptr = fdopen (news, "r");
	  /* Allocate a buffer.  */
	  p[news].fd_buff = (unsigned char *) malloc (BUF_SIZE);

	  /* Be prepared to accept msgs.  */
	  hang_a_read (news);

	  /* Get the old descriptor back onto accepting.  */
	  hang_an_accept (s);
	  return news;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* Recv routine.  */
int
VMSrecv (s, buf, len, flags)
     int s;
     char *buf;
     int len, flags;
{
  return recvfrom (s, buf, len, flags, 0, 0);
}

/* Revfrom routine.  */
int
VMSrecvfrom (s, buf, len, flags, from, fromlen)
     int s;
     char *buf;
     int len, flags;
     union socket_addr *from;
     int *fromlen;
{
  int number;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].domain != AF_INET && p[s].domain != AF_X25)
    return -1;

  /* If we're not onto datagrams, then it's possible that a previous
     call to recvfrom didn't read all the data, and left some behind.
     So first of all, look in our data buffer for any leftovers that
     will satisfy this read.  */

  /* We couldn't satisfy the request from previous calls so we must now
     wait for a message to come through.  */
  if (wait_efn (s) == -1)
    /* Timed out.  */
    return -1;

  if (p[s].closed_by_remote == 1)
    {
      /* This could have happened! */
      errno = ECONNRESET;
      return -1;
    }

  if (from != NULL)
    {
      if (tcp_make == CMU)
	{
	  if (p[s].type == SOCK_DGRAM)
	    {
	      /* Not documented but we get the from data from the beginning of
		 the data buffer.  */
	      *fromlen = sizeof (p[s].from.in);
	      from->in.sin_family = AF_INET;
	      bcopy (&p[s].fd_buff[8], &(from->in.sin_port), 2);
	      from->in.sin_port = htons (from->in.sin_port);
	      bcopy (&p[s].fd_buff[0], &(from->in.sin_addr), 4);

	      /* Remove the address data from front of data buffer.  */
	      bcopy (p[s].fd_buff + 12, p[s].fd_buff, p[s].fd_buff_size);
	    }
	  else
	    {
	      *fromlen = p[s].fromlen;
	      bcopy (&p[s].from, from, p[s].fromlen);
	    }
	}
      else if (tcp_make == UCX)
	{
	  *fromlen = p[s].fromdummy;
	  bcopy (&p[s].from, from, p[s].fromdummy);
	}
      else
	{
	  *fromlen = p[s].fromlen;
	  bcopy (&p[s].from, from, p[s].fromlen);
	}
    }

  /* We may've received too much.  */
  number = p[s].fd_buff_size;
  if (number <= len)
    {
      /* If we haven't give back all the data available.  */
      bcopy (p[s].fd_buff, buf, number);
      p[s].fd_leftover = 0;
      hang_a_read (s);
      return (number);
    }
  else
    {
      /* If we have too much data then split it up.  */
      p[s].fd_leftover = p[s].fd_buff;
      bcopy (p[s].fd_leftover, buf, len);
      /* And change the pointers.  */
      p[s].fd_leftover += len;
      p[s].fd_buff_size -= len;
      return (len);
    }
}

/* Send routine.  */
int
VMSsend (s, msg, len, flags)
     int s;
     char *msg;
     int len, flags;
{
  return sendto (s, msg, len, flags, 0, 0);
}

/* Sendto routine.  */
int
VMSsendto (s, msg, len, flags, to, tolen)
     int s;
     unsigned char *msg;
     int len, flags;
     union socket_addr *to;
     int tolen;
{
  int i, j, st, size;
  unsigned char udpbuf[BUF_SIZE + 12];
  char infobuff[1024], lhost[32];
  unsigned short int temp;

  if (!tcp_make)
    set_tcp_make ();

  /* First remember who we sent it to and set the value of size.  */
  if (to != 0)
    {
      p[s].tolen = tolen;
      bcopy (to, &(p[s].to), tolen);
      size = tolen;
    }
  else
    size = 0;

  if (p[s].domain == AF_INET)
    {
      /* We might never have started a read for udp (socket/sendto) so
	 put one here.  */
      if (p[s].type == SOCK_DGRAM)
	hang_a_read (s);

      if (tcp_make == CMU)
	{
	  if (p[s].type == SOCK_DGRAM)
	    {
	      /* We might never have opened up a udp connection yet,
		 so check.  */
	      if (p[s].cmu_open != 1)
		{
		  st = sys$qiow (0, p[s].channel, TCP$OPEN, p[s].iosb, 0, 0,
				 0, 0, 0, 0, 1, 0);
		  if (QIO_ST_FAILED)
		    return -1;

		  p[s].cmu_open = 1;
		  sys$qiow (0, p[s].channel, TCP$INFO, p[s].iosb,
			    0, 0, &infobuff, 1024, 0, 0, 0, 0);
		  bcopy (infobuff + 264, &(p[s].name.in.sin_port), 2);
		  p[s].name.in.sin_port = htons (p[s].name.in.sin_port);
		  bcopy (infobuff + 136, lhost, infobuff[1]);
		  lhost[infobuff[1]] = '\0';
		  sys$qiow (0, p[s].channel, GTHST, p[s].iosb,
			    0, 0, &infobuff, 1024, 1, lhost, 0, 0);
		  bcopy (infobuff + 4, &(p[s].name.in.sin_addr), 4);
		}

	      /* This isn't well documented.  To send to a UDP socket, we
		 need to put the address info at the beginning of the
		 buffer.  */
	      bcopy (msg, udpbuf + 12, len);
	      bcopy (&p[s].to.in.sin_addr, udpbuf + 4, 4);
	      temp = ntohs (p[s].to.in.sin_port);
	      bcopy (&temp, udpbuf + 10, 2);
	      bcopy (&p[s].name.in.sin_addr, udpbuf, 4);
	      temp = ntohs (p[s].name.in.sin_port);
	      bcopy (&temp, udpbuf + 8, 2);
	      temp = len + 12;
	      st = sys$qiow (0, p[s].channel, TCP$SEND, p[s].iosb, 0, 0,
			     udpbuf, temp, 0, 0, 0, 0);
	      if (QIO_FAILED)
		return -1;
	    }
	  else
	    {
	      /* TCP (! UDP)  */
	      st = sys$qiow (0, p[s].channel, TCP$SEND, p[s].iosb, 0, 0,
			     msg, len, 0, 0, 0, 0);
	      if (QIO_FAILED)
		return -1;
	    }
	  return len;
	}
      else if (tcp_make == UCX)
	{
	  struct itemlist rhost;
	  rhost.length = sizeof (struct sockaddr_in);
	  rhost.code = 0;
	  rhost.dataptr = (char *) &p[s].to;

	  st = sys$qiow (0, p[s].channel, IO$_WRITEVBLK, p[s].iosb, 0, 0,
			 msg, len, &rhost, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return len;
	}
      else
	{
	  /* TWG */
	  st = sys$qiow (0, p[s].channel, IO$_WRITEVBLK, p[s].iosb,
			 0, 0, msg, len, 0, &p[s].to, size, 0);
	  if (QIO_FAILED)
	    return -1;

	  return len;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* Getsockname routine.  */
int
VMSgetsockname (s, name, namelen)
     int s;
     union socket_addr *name;
     int *namelen;
{
  int st;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  /* For CMU we just return values held in our data structure.  */
	  *namelen = p[s].namelen;
	  bcopy (&(p[s].name), name, *namelen);
	  return (0);
	}
      else if (tcp_make == UCX)
	{
	  /* An item_list_3 descriptor.  */
	  struct itemlist lhost;

	  lhost.length = *namelen;
	  lhost.code = 0;
	  lhost.dataptr = (char *) name;

	  /* Fill in namelen with actual ret len value.  */
	  lhost.retlenptr = (short int *) namelen;

	  st = sys$qiow (0, p[s].channel, IO$_SENSEMODE, p[s].iosb, 0, 0,
			 0, 0, &lhost, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return 0;
	}
      else
	{
	  /* TWG gives us the information. */
	  st = sys$qiow (0, p[s].channel, IO$_GETSOCKNAME, p[s].iosb,
			 0, 0, name, namelen, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return 0;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* Select routine.  */
int
VMSselect (nfds, readfds, writefds, exceptfds, timeout)
     int nfds;
     fd_set *readfds, *writefds, *exceptfds;
     struct timeval *timeout;
{
  int timer, fd, alarm_set, total, end;
  long mask, cluster;
  struct descriptor termdesc;
  static fd_set new_readfds, new_writefds, new_exceptfds;

  FD_ZERO (&new_readfds);
  FD_ZERO (&new_writefds);
  FD_ZERO (&new_exceptfds);
  total = 0;

  /* Assign a terminal channel if we haven't already.  */
  if (terminal.chan == -1)
    {
      termdesc.size = 10;
      termdesc.ptr = "SYS$INPUT:";
      sys$assign (&termdesc, &terminal.chan, 0, 0);
    }
  alarm_set = 0;
  if (timeout != NULL)
    {
      /* If a timeout is given then set the alarm.  */
      end = timeout->tv_sec;
      if (timer != 0)
	{
	  /* We need to reset the alarm if it didn't fire, but we set it.  */
	  alarm_set = 1;
	  si_alarm (end);
	}
    }
  else
    end = 1;

  do
    {
      if (exceptfds)
	{
	   /* Nothing */ ;
	}

      if (writefds)
	{
	  for (fd = 0; fd < nfds; fd++)
	    if (FD_ISSET (fd, writefds))
	      {
		if (p[fd].connect_pending)
		   /* Nothing */ ;
		else if ((p[fd].status == ACTIVE_CONNECTION)
			 || (p[fd].status == PASSIVE_CONNECTION))
		  {
		    FD_SET (fd, &new_writefds);
		    total++;
		  }
	      }
	}

      if (readfds)
	{
	  /* True if data pending or an accept.  */
	  for (fd = 3; fd < nfds; fd++)
	    if (FD_ISSET (fd, readfds) &&
		((p[fd].fd_buff_size != -1) || (p[fd].accept_pending == 1)))
	      {
		FD_SET (fd, &new_readfds);
		total++;
	      }
	}

      if (total || (end == 0))
	break;

      /* Otherwise, wait on an event flag.  It's possible that the wait can
	 be stopped by a spurious event flag being set -- i.e. one that's
	 got a status not normal.  So we've got to be prepared to loop
	 around the wait until a valid reason happens.  */

      /* Set up the wait mask.  */
      cluster = 0;
      mask = 0;
      for (fd = 3; fd < nfds; fd++)
	{
	  sys$clref (fd);
	  if (readfds)
	    if FD_ISSET
	      (fd, readfds) mask |= (1 << fd);
	  if (writefds)
	    if FD_ISSET
	      (fd, writefds) mask |= (1 << fd);
	  if (exceptfds)
	    if FD_ISSET
	      (fd, exceptfds) mask |= (1 << fd);
	}

      mask |= (1 << TIMER_EFN);

      /* Clear it off just in case.  */
      sys$clref (TIMER_EFN);

      /* Wait around.  */
      sys$wflor (cluster, mask);

      mask = 0;
      if (read_efn (TIMER_EFN))
	{
	  errno = EINTR;
	  break;
	}
  } while (1);
  /*NOTREACHED*/

  /* Unset the alarm if we set it.  */
  if (alarm_set == 1)
    alarm (0);

  if (readfds)
    *readfds = new_readfds;

  if (writefds)
    *writefds = new_writefds;

  if (exceptfds)
    *exceptfds = new_exceptfds;

  return total;
}

/* Shutdown routine.  */
VMSshutdown (s, how)
     int s, how;
{
  int st;
  int ucx_how;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  /* For CMU we just close off.  */
	  si_close (s);
	  return 0;
	}
      else if (tcp_make == UCX)
	{
	  st = sys$qiow (0, p[s].channel, IO$_DEACCESS | IO$M_SHUTDOWN,
			 p[s].iosb, 0, 0, 0, 0, 0, how, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return 0;
	}
      else
	{
	  /* TWG lets us do it.  */
	  st = sys$qiow (0, p[s].channel, IO$_SHUTDOWN, p[s].iosb, 0, 0, how,
			 0, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return 0;
	}
    }
  else				/* it wasn't a socket */
    return -1;
}

/*  */

/* The following routines are used by the above socket calls.  */

/* hang_a_read sets up a read to be finished at some later time.  */
hang_a_read (s)
     int s;
{
  extern int read_ast ();
  int size, st;

  /* Don't bother if we already did it.  */
  if (p[s].read_outstanding == 1)
    return;

  /* Have a read outstanding.  */
  p[s].read_outstanding = 1;
  size = sizeof (p[s].from) + 4;
  sys$clref (s);

  /* Clear off the event flag just in case, and reset the buf size.  */
  p[s].fd_buff_size = -1;
  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  st = sys$qio (s, p[s].channel, TCP$RECEIVE, p[s].iosb, read_ast,
			&p[s], p[s].fd_buff, BUF_SIZE, 0, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else if (tcp_make == UCX)
	{

	  p[s].rhost.length = sizeof (struct sockaddr_in);
	  p[s].rhost.code = 0;
	  p[s].rhost.dataptr = (char *) &p[s].from;
	  p[s].rhost.retlenptr = &p[s].fromdummy;

	  st = sys$qio (s, p[s].channel, IO$_READVBLK, p[s].iosb, read_ast,
			&p[s], p[s].fd_buff, BUF_SIZE, &p[s].rhost, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else
	{
	  /* TWG */
	  st = sys$qio (s, p[s].channel, IO$_READVBLK, p[s].iosb, read_ast,
			&p[s], p[s].fd_buff, BUF_SIZE, 0, &p[s].fromlen,
			size, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* hang_an_accept waits for a connection request to come in.  */
hang_an_accept (s)
     int s;
{
  extern int accept_ast ();
  int st;

  /* Clear the event flag just in case.  */
  sys$clref (s);

  /* Reset our flag & buf size.  */
  p[s].accept_pending = 0;
  p[s].fd_buff_size = -1;
  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  st = sys$qio (s, p[s].channel, TCP$OPEN, p[s].iosb, accept_ast,
			&p[s], 0, 0, ntohs (p[s].name.in.sin_port), 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else if (tcp_make == UCX)
	{
	  struct descriptor inetdesc;

	  /* Assign channel for actual connection off listener.  */
	  inetdesc.size = 3;
	  inetdesc.ptr = "BG:";
	  if (sys$assign (&inetdesc, &p[s].ucx_accept_chan, 0,
			  0) != SS$_NORMAL)
	    return -1;

	  /* UCX's accept returns remote host info and the channel for a new
	     socket to perform reads/writes on, so a sys$assign isn't
	     really necessary.  */
	  p[s].rhost.length = sizeof (struct sockaddr_in);
	  p[s].rhost.dataptr = (char *) &p[s].from;
	  p[s].fromdummy = 0;
	  p[s].rhost.retlenptr = &p[s].fromdummy;

	  st = sys$qio (s, p[s].channel, IO$_ACCESS | IO$M_ACCEPT, p[s].iosb,
			accept_ast, &p[s], 0, 0, &p[s].rhost,
			&p[s].ucx_accept_chan, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else
	{
	  st = sys$qio (s, p[s].channel, IO$_ACCEPT_WAIT, p[s].iosb,
			accept_ast, &p[s], 0, 0, 0, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* wait_efn just sets up a wait on either an event or the timer.  */
wait_efn (s)
     int s;
{
  long mask, cluster;

  cluster = 0;
  sys$clref (TIMER_EFN);
  mask = (1 << s) | (1 << TIMER_EFN);
  sys$wflor (cluster, mask);

  if (read_efn (TIMER_EFN))
    {
      errno = EINTR;
      return -1;
    }

  return 0;
}

/* read_ast is called by the system whenever a read is done.  */
read_ast (p)
     struct fd_entry *p;
{
  int i, j;
  unsigned char *v, *w;

  /* Reset the outstanding flag.  */
  p->read_outstanding = 0;
  if (p->iosb[0] == SS$_NORMAL)
    {
      /* Check no errors.  */
      p->fd_buff_size = p->iosb[1];
      if (tcp_make == CMU)
	{
	  /* fiddle for DGRMs */
	  if (p->type == SOCK_DGRAM)
	    p->fd_buff_size -= 12;
	}
      if (p->sig_req == 1)
	gsignal (SIGIO);
    }
  else if (p->iosb[0] == SS$_CLEARED)
    p->closed_by_remote = 1;
  else if (tcp_make == UCX)
    {
      if (p->iosb[0] == SS$_LINKDISCON)
	p->closed_by_remote = 1;
    }
}

/* accept_ast is called whenever an incoming call is detected.  */
accept_ast (p)
     struct fd_entry *p;
{
  if (p->iosb[0] == SS$_NORMAL)
    p->accept_pending = 1;
  else
    /* If it failed set up another listen.  */
    listen (p->s, p[p->s].backlog);
}

/* connect_ast is called whenever an async connect is made.  */
connect_ast (p)
     struct fd_entry *p;
{
  p->connect_pending = 0;
  if ((p->connected = p->iosb[0]) == SS$_NORMAL)
    {
      /* We made the connection.  */
      p->status = ACTIVE_CONNECTION;

      /* Be prepared to accept a msg.  */
      hang_a_read (p->s);
    }
}

/*  */
/* These routines handle stream I/O.  */

/* si_close -- must close off any connection in progress.  */
si_close (s)
     int s;
{
  if (!tcp_make)
    set_tcp_make ();

  if ((s < 0) || (s > 31))
    return -1;

  if (p[s].channel != 0)
    {
      /* Was it one of our descriptors? */
      if (p[s].domain == AF_INET)
	{
	  if (tcp_make == CMU)
	    sys$qiow (0, p[s].channel, TCP$CLOSE, p[s].iosb,
		      0, 0, 0, 0, 0, 0, 0, 0);
	  if (p[s].status != HANDED_OFF)
	    sys$dassgn (p[s].channel);
	  close (s);
	  free (p[s].fd_buff);
	  p_initialise (s);
	}
      return 0;
    }
  else
    {
      /* Re-initialise data structure just in case.  */
      p[s].fd_buff_size = -1;
      p[s].accept_pending = 0;
      p[s].status = INITIALISED;
      return close (s);
    }
}

/* si_alarm -- insert a call to our own alarm function.  */
si_alarm (i)
     int i;
{
  extern int pre_alarm ();

  /* Make the call to pre_alarm instead of what the user wants;
     pre_alarm will call his routine when it finishes.  */
  /* VAX needs this call each time! */
  signal (SIGALRM, pre_alarm);
  alarm (i);
}

/* pre_alarm -- gets called first on an alarm signal.  */
pre_alarm ()
{
  /* Come here first so we can set our timer event flag.  */
  sys$setef (TIMER_EFN);
  (*alarm_function) ();
}

/* p_initialise - initialise our data array.  */
p_initialise (s)
     int s;
{
  int j;
  for (j = 0; j < 4; j++)
    p[s].iosb[j] = 0;
  p[s].channel = 0;
  p[s].fd_buff_size = -1;
  p[s].accept_pending = 0;
  p[s].connect_pending = 0;
  p[s].connected = 0;
  p[s].fd_buff = NULL;
  p[s].fd_leftover = NULL;
  p[s].fptr = NULL;
  p[s].s = s;
  p[s].name.in.sin_port = 0;
  p[s].masklen = 4;
  for (j = 0; j < 16; j++)
    p[s].mask[j] = 0xff;
  p[s].need_header = 0;
  p[s].status = INITIALISED;
  p[s].read_outstanding = 0;
  p[s].cmu_open = 0;
  p[s].x25_listener = 0;
  p[s].mother = s;
  p[s].child = 0;
  p[s].no_more_accepts = 0;
  p[s].closed_by_remote = 0;
  p[s].non_blocking = 0;
  p[s].sig_req = 0;
  sys$clref (s);
}

/* read_efn -- see whether an event flag is set.  */
read_efn (i)
     int i;
{
  int j;
  sys$readef (i, &j);
  j &= (1 << i);

  return j;
}

static
set_tcp_make ()
{
  struct descriptor inetdesc;
  int channel;
  /* first try CMU */
  inetdesc.size = 3;
  inetdesc.ptr = "IP:";
  if (sys$assign (&inetdesc, &channel, 0, 0) == SS$_NORMAL)
    {
      sys$dassgn (channel);
      tcp_make = CMU;
      return;
    }

  /* next try TWG */
  inetdesc.size = 7;
  inetdesc.ptr = "_INET0:";
  if (sys$assign (&inetdesc, &channel, 0, 0) == SS$_NORMAL)
    {
      sys$dassgn (channel);
      tcp_make = WG;
      return;
    }

  /* next try UCX */
  inetdesc.size = 4;
  inetdesc.ptr = "BG0:";
  if (sys$assign (&inetdesc, &channel, 0, 0) == SS$_NORMAL)
    {
      sys$dassgn (channel);
      tcp_make = UCX;
      return;
    }

  /* nothing there oh dear!*/
  tcp_make = NONE;
  return;
}

static char *
getdevicename (channel)
     unsigned short int channel;
{
  int st;
  struct
  {
    struct itemlist id;
    int eol;
  } itmlst;
  static char name[64];
  short int lgth;

  name[0] = '\0';
  itmlst.id.code = DVI$_DEVNAM;
  itmlst.id.length = 64;
  itmlst.id.dataptr = name;
  itmlst.id.retlenptr = &lgth;
  itmlst.eol = 0;
  st = sys$getdvi (0, channel, 0, &itmlst, 0, 0, 0, 0);
  if (QIO_ST_FAILED)
    fprintf (stderr, "error getting device name %d\n", st);

  return (name);
}
