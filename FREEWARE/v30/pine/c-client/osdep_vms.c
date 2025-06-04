/* OSDEP_VMS.C - Extracted from C-CLIENT/OSDEP.H */
#include "../pine/headers.h"
#include "../c-client/osdep.h"


/* Standard string descriptor */
struct DESC {
	short	length, type;	/* Length of string and descriptor type */
	char	*address;	/* Buffer's address */
	};

/* Item list for passing parameters to the mail routines */
struct ITEM_LIST {
	short	length,		/* Buffer length */
		code;		/* Item/action code */
	char	*buffer;	/* Input buffer address */
	int	*Olength;	/* Where to place length of result (if requested) */
	};



/* Write current time in RFC 822 format
 * Accepts: destination string
 */

void rfc822_date (date)
	char *date;
{
  int zone;
  char *zonename;
  struct tm *t;
  long	clock;
	char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


  time(&clock);
  t = localtime (&clock);	/* convert to individual items */
  zone = 120;		/*~~~~~~*/
  zonename = "IST";	/*~~~~~*/
				/* and output it */
  sprintf (date,"%s, %d %s %d %02d:%02d:%02d %+03d%02d (%s)",
	   days[t->tm_wday],t->tm_mday,months[t->tm_mon],t->tm_year+1900,
	   t->tm_hour,t->tm_min,t->tm_sec,zone/60,abs (zone) % 60,zonename);
}

/* Get a block of free storage
 * Accepts: size of desired block
 * Returns: free storage block
 */

void *fs_get (size)
	size_t size;
{
  void *block = malloc (size);
  if (!block) fatal ("Out of free storage");
  return (block);
}


/* Resize a block of free storage
 * Accepts: ** pointer to current block
 *	    new size
 */

void fs_resize (block,size)
	void **block;
	size_t size;
{
  if (!(*block = realloc (*block,size))) fatal ("Can't resize free storage");
}


/* Return a block of free storage
 * Accepts: ** pointer to free storage block
 */

void fs_give (block)
	void **block;
{
  free (*block);
  *block = NIL;
}


/* Report a fatal error
 * Accepts: string to output
 */

fatal (string)
	char *string;
{
  mm_fatal (string);		/* output the string */
  printf ("IMAP C-Client crash: %s",string);
  abort ();			/* die horribly */
}

/* Copy string with CRLF newlines
 * Accepts: destination string
 *	    pointer to size of destination string
 *	    source string
 *	    length of source string
 * Returns: length of copied string
 */

unsigned long strcrlfcpy (dst,dstl,src,srcl)
	char **dst;
	unsigned long *dstl;
	char *src;
	unsigned long srcl;
{
  long i,j;
  char *d = src;
				/* count number of LF's in source string(s) */
  for (i = srcl,j = 0; j < srcl; j++) if (*d++ == '\012') i++;
  if (i > *dstl) {		/* resize if not enough space */
    fs_give ((void **) dst);	/* fs_resize does an unnecessary copy */
    *dst = (char *) fs_get ((*dstl = i) + 1);
  }
  d = *dst;			/* destination string */
				/* copy strings, inserting CR's before LF's */
  while (srcl--) switch (*src) {
  case '\015':			/* unlikely carriage return */
    *d++ = *src++;		/* copy it and any succeeding linefeed */
    if (srcl && *src == '\012') {
      *d++ = *src++;
      srcl--;
    }
    break;
  case '\012':			/* line feed? */
    *d++ ='\015';		/* yes, prepend a CR, drop into default case */
  default:			/* ordinary chararacter */
    *d++ = *src++;		/* just copy character */
    break;
  }
  *d = '\0';			/* tie off destination */
  return d - *dst;		/* return length */
}


/* Length of string after strcrlfcpy applied
 * Accepts: source string
 * Returns: length of string
 */

unsigned long strcrlflen (s)
	STRING *s;
{
  unsigned long pos = GETPOS (s);
  unsigned long i = SIZE (s);
  unsigned long j = i;
  while (j--) switch (SNX (s)) {/* search for newlines */
  case '\015':			/* unlikely carriage return */
    if (j && (CHR (s) == '\012')) {
      SNX (s);			/* eat the line feed */
      j--;
    }
    break;
  case '\012':			/* line feed? */
    i++;
  default:			/* ordinary chararacter */
    break;
  }
  SETPOS (s,pos);		/* restore old position */
  return i;
}

/* Server log in
 * Accepts: user name string
 *	    password string
 *	    optional place to return home directory
 * Returns: T if password validated, NIL otherwise
 */
#include <uaidef.h>
#include <rmsdef.h>
long server_login (username,password,home,argc,argv)
	char *username;
	char *password;
	char **home;
	int argc;
	char *argv[];
{
    int	sys$getuai(), sys$hash_password();
    int status, uai_flags;
    unsigned long uai_pwd[2], putative_pw[2];
    unsigned short uai_salt;
    unsigned char uai_encrypt;
    struct DESC	usernameD, passwordD;
    char	*p,
	Username[256], Password[256];

	struct	ITEM_LIST itmlst[] = {
         {sizeof(uai_flags),           UAI$_FLAGS,     &uai_flags,     NULL},
         {sizeof(uai_encrypt),          UAI$_ENCRYPT,   &uai_encrypt,   NULL},
         {sizeof(uai_salt),            UAI$_SALT,      &uai_salt,      NULL},
         {8,                           UAI$_PWD,       uai_pwd,        NULL},
         {0, 0, NULL, NULL }
    };

/* If the password or the username are empty - return error (HUJI) */
    if((*username == '\0') || (*password == '\0'))
	return NIL;	/* No such user */

/* Do not ruin the username - copy it elsewhere */
	strcpy(Username, username);
	strcpy(Password, password);
    ucase(Username);
    ucase(Password);

    usernameD.address = Username;
    usernameD.length = strlen(username);
    usernameD.type = 0;
    status = sys$getuai(0, 0, &usernameD, itmlst, 0, 0, 0);
    if (status == RMS$_RNF || (UAI$M_DISACNT & uai_flags))
    {
        /* A record was not found for this username, or the guy's disuser'd */
        return NIL;
    }
    if((status & 0x1) == 0)
        return NIL;                 /* unexpected error of some kind */

    /* Now hash the putative password to see if it matches the one on file. */

    passwordD.address = Password;
    passwordD.length = strlen(password);
    passwordD.type = 0;
    status = sys$hash_password(&passwordD, uai_encrypt, uai_salt, &usernameD,
                               putative_pw);
    if((status & 0x1) == 0)
        return NIL;

    if ((putative_pw[0] != uai_pwd[0]) || putative_pw[1] != uai_pwd[1])
    {
        return(NIL);
    }

/* All ok. Now try getting the mail directory from the mail file. If not found,
   then try to get the VMS login directory. */
    vms_mail_get_user_directory(Username, home);
    return T;
}

/* Return my user name
 * Returns: my user name
 */

char *uname = NIL;

char *myusername ()
{
  return uname ? uname : (uname = cpystr (cuserid(NULL)));
}


/* Return my home directory name
 * Returns: my home directory name
 */

char *hdname = NIL;

char *myhomedir ()
{
  return hdname ? hdname : (hdname = cpystr (getenv("HOME")));
}


/* Build status lock file name
 * Accepts: scratch buffer
 *	    file name
 * Returns: name of file to lock
 */

char *lockname (tmp,fname)
	char *tmp;
	char *fname;
{
  int i;
  sprintf (tmp,"SYS$SCRATCH:.%s",fname);
  for (i = 6; i < strlen (tmp); ++i) if (tmp[i] == '/') tmp[i] = '\\';
  return tmp;			/* note name for later */
}

#ifdef MULTINET
#include <sys/types.h>	/* To define Multinet;s structures */
#include <netinet/in.h>
/* TCP/IP open
 * Accepts: host name
 *	    contact port number
 * Returns: TCP/IP stream if success else NIL
 */

TCPSTREAM *tcp_open (host,port)
	char *host;
	long port;
{
  TCPSTREAM *stream = NIL;
  int sock;
  char *s;
  struct sockaddr_in sin;
  struct hostent *host_name;
  char hostname[MAILTMPLEN];
  char tmp[MAILTMPLEN];
  /* The domain literal form is used (rather than simply the dotted decimal
     as with other Unix programs) because it has to be a valid "host name"
     in mailsystem terminology. */
				/* look like domain literal? */
  if (host[0] == '[' && host[(strlen (host))-1] == ']') {
    strcpy (hostname,host+1);	/* yes, copy number part */
    hostname[(strlen (hostname))-1] = '\0';
    if ((sin.sin_addr.s_addr = inet_addr (hostname)) != -1) {
      sin.sin_family = AF_INET;	/* family is always Internet */
      strcpy (hostname,host);	/* hostname is user's argument */
    }
    else {
      sprintf (tmp,"Bad format domain-literal: %.80s",host);
      mm_log (tmp,ERROR);
      return NIL;
    }
  }

  else {			/* lookup host name, note that brain-dead Unix
				   requires lowercase! */
    strcpy (hostname,host);	/* in case host is in write-protected memory */
    if ((host_name = gethostbyname (lcase (hostname)))) {
				/* copy address type */
      sin.sin_family = host_name->h_addrtype;
				/* copy host name */
      strcpy (hostname,host_name->h_name);
				/* copy host addresses */
      memcpy (&sin.sin_addr,host_name->h_addr,host_name->h_length);
    }
    else {
      sprintf (tmp,"No such host as %.80s",host);
      mm_log (tmp,ERROR);
      return NIL;
    }
  }

				/* copy port number in network format */
  if (!(sin.sin_port = htons (port))) fatal ("Bad port argument to tcp_open");
				/* get a TCP stream */
  if ((sock = socket (sin.sin_family,SOCK_STREAM,0)) < 0) {
    sprintf (tmp,"Unable to create TCP socket: %s", strerror (errno));
    mm_log (tmp,ERROR);
    return NIL;
  }
				/* open connection */
  if (connect (sock,(struct sockaddr *)&sin,sizeof (sin)) < 0) {
    sprintf (tmp,"Can't connect to %.80s,%d: %s",hostname,port,
	     strerror (errno));
    mm_log (tmp,ERROR);
    return NIL;
  }
				/* create TCP/IP stream */
  stream = (TCPSTREAM *) fs_get (sizeof (TCPSTREAM));
				/* copy official host name */
  stream->host = cpystr (hostname);
				/* get local name */
  gethostname (tmp,MAILTMPLEN-1);
  stream->localhost = cpystr ((host_name = gethostbyname (tmp)) ?
			      host_name->h_name : tmp);
				/* init sockets */
  stream->tcpsi = stream->tcpso = sock;
  stream->ictr = 0;		/* init input counter */
  return stream;		/* return success */
}

TCPSTREAM *tcp_aopen (host,service)
{
	return NIL;		/* Not supported here */
}


/* TCP/IP receive line
 * Accepts: TCP/IP stream
 * Returns: text line string or NIL if failure
 */

char *tcp_getline (stream)
	TCPSTREAM *stream;
{
  int n,m;
  char *st,*ret,*stp;
  char c = '\0';
  char d;
				/* make sure have data */
  if (!tcp_getdata (stream)) return NIL;
  st = stream->iptr;		/* save start of string */
  n = 0;			/* init string count */
  while (stream->ictr--) {	/* look for end of line */
    d = *stream->iptr++;	/* slurp another character */
    if ((c == '\015') && (d == '\012')) {
      ret = (char *) fs_get (n--);
      memcpy (ret,st,n);	/* copy into a free storage string */
      ret[n] = '\0';		/* tie off string with null */
      return ret;
    }
    n++;			/* count another character searched */
    c = d;			/* remember previous character */
  }
				/* copy partial string from buffer */
  memcpy ((ret = stp = (char *) fs_get (n)),st,n);
				/* get more data from the net */
  if (!tcp_getdata (stream)) return NIL;
				/* special case of newline broken by buffer */
  if ((c == '\015') && (*stream->iptr == '\012')) {
    stream->iptr++;		/* eat the line feed */
    stream->ictr--;
    ret[n - 1] = '\0';		/* tie off string with null */
  }
				/* else recurse to get remainder */
  else if (st = tcp_getline (stream)) {
    ret = (char *) fs_get (n + 1 + (m = strlen (st)));
    memcpy (ret,stp,n);		/* copy first part */
    memcpy (ret + n,st,m);	/* and second part */
    fs_give ((void **) &stp);	/* flush first part */
    fs_give ((void **) &st);	/* flush second part */
    ret[n + m] = '\0';		/* tie off string with null */
  }

  return ret;
}

/* TCP/IP receive buffer
 * Accepts: TCP/IP stream
 *	    size in bytes
 *	    buffer to read into
 * Returns: T if success, NIL otherwise
 */

long tcp_getbuffer (stream,size,buffer)
	TCPSTREAM *stream;
	unsigned long size;
	char *buffer;
{
  unsigned long n;
  char *bufptr = buffer;
  while (size > 0) {		/* until request satisfied */
    if (!tcp_getdata (stream)) return NIL;
    n = min (size,stream->ictr);/* number of bytes to transfer */
				/* do the copy */
    memcpy (bufptr,stream->iptr,n);
    bufptr += n;		/* update pointer */
    stream->iptr +=n;
    size -= n;			/* update # of bytes to do */
    stream->ictr -=n;
  }
  bufptr[0] = '\0';		/* tie off string */
  return T;
}



long tcp_abort ();

/* TCP/IP receive data
 * Accepts: TCP/IP stream
 * Returns: T if success, NIL otherwise
 */

long tcp_getdata (stream)
	TCPSTREAM *stream;
{
  int i;
  fd_set fds;

  FD_ZERO (&fds);		/* initialize selection vector */
  if (stream->tcpsi < 0) return NIL;
  if(stream->ictr < 1) {	/* If the buffer is empty */
	FD_SET (stream->tcpsi,&fds);/* set bit in selection vector */
	if((i = select (getdtablesize(),&fds,0,0,0)) < 0) return tcp_abort(stream);
	i = socket_read (stream->tcpsi,stream->ibuf,BUFLEN);
	if (i < 1) return tcp_abort (stream);
	stream->ictr = i;		/* set new byte count */
	stream->iptr = stream->ibuf;/* point at TCP buffer */
   }
  return T;
}

/* TCP/IP send string as record
 * Accepts: TCP/IP stream
 *	    string pointer
 * Returns: T if success else NIL
 */

long tcp_soutr (stream,string)
	TCPSTREAM *stream;
	char *string;
{
  return tcp_sout (stream,string,(unsigned long) strlen (string));
}


/* TCP/IP send string
 * Accepts: TCP/IP stream
 *	    string pointer
 *	    byte count
 * Returns: T if success else NIL
 */

long tcp_sout (stream,string,size)
	TCPSTREAM *stream;
	char *string;
	unsigned long size;
{
  int i;
  fd_set fds;

  FD_ZERO (&fds);		/* initialize selection vector */
  if (stream->tcpso < 0) return NIL;
  while (size > 0) {		/* until request satisfied */
    FD_SET (stream->tcpso,&fds);/* set bit in selection vector */
    errno = NIL;		/* block and wrtie */
    while (((i = select (getdtablesize(),0,&fds,0,0)) < 0) &&
	   (errno == EINTR));
    if (i < 0) return tcp_abort (stream);
    while (((i = socket_write (stream->tcpso,string,size)) < 0) &&
	   (errno == EINTR));
    if (i < 0) return tcp_abort (stream);
    size -= i;			/* how much we sent */
    string += i;
  }
  return T;			/* all done */
}

/* TCP/IP close
 * Accepts: TCP/IP stream
 */

void tcp_close (stream)
	TCPSTREAM *stream;
{
  tcp_abort (stream);		/* nuke the stream */
				/* flush host names */
  fs_give ((void **) &stream->host);
  fs_give ((void **) &stream->localhost);
  fs_give ((void **) &stream);	/* flush the stream */
}


/* TCP/IP abort stream
 * Accepts: TCP/IP stream
 * Returns: NIL always
 */

long tcp_abort (stream)
     TCPSTREAM *stream;
{
  if (stream->tcpsi >= 0) {	/* no-op if no socket */
    socket_close (stream->tcpsi);	/* nuke the socket */
    if (stream->tcpsi != stream->tcpso) close (stream->tcpso);
    stream->tcpsi = stream->tcpso = -1;
  }
  return NIL;
}

/* TCP/IP get host name
 * Accepts: TCP/IP stream
 * Returns: host name for this stream
 */

char *tcp_host (stream)
	TCPSTREAM *stream;
{
  return stream->host;		/* return host name */
}


/* TCP/IP get local host name
 * Accepts: TCP/IP stream
 * Returns: local host name
 */

char *tcp_localhost (stream)
	TCPSTREAM *stream;
{
  return stream->localhost;	/* return local host name */
}
#endif	/* MULTINET */

#ifdef NETLIB
#include <descrip.h>
/* TCP/IP open
 * Accepts: host name
 *	    contact port number
 * Returns: TCP/IP stream if success else NIL
 */

TCPSTREAM *tcp_open (host,port)
	char *host;
	long port;
{
  TCPSTREAM *stream = NIL;
  unsigned long sock;
  int status;
  char *s;
  char hostname[MAILTMPLEN];
  char tmp[MAILTMPLEN];
  struct dsc$descriptor HostDesc = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL };
		/* used for giving the hostname to connect to */
  struct dsc$descriptor LocalhostDesc = { 0, DSC$K_DTYPE_T,
	DSC$K_CLASS_D, NULL };	/* Used to receive the local hostname */

/* Assign a local socket */
  if(((status = net_assign(&sock)) & 0x1) == 0) {
	sprintf(tmp, "Unable to assign to net, status=%d", status);
	mm_log (tmp, ERROR);
	return NIL;
  }
  if(((status = net_bind(&sock, 1)) & 0x1) == 0) {
	sprintf(tmp, "Unable to create local socket, status=%d", status);
	mm_log (tmp, ERROR);
	return NIL;
  }
				/* open connection */
  HostDesc.dsc$w_length = strlen(host);
  HostDesc.dsc$a_pointer = host;
  if(((status = tcp_connect (&sock, &HostDesc, port)) & 0x1) == 0) {
    sprintf (tmp,"Can't connect to %.80s,%d: %s",host,port,
	     strerror (errno));
    mm_log (tmp,ERROR);
    return NIL;
  }
				/* create TCP/IP stream */
  stream = (TCPSTREAM *) fs_get (sizeof (TCPSTREAM));
				/* copy official host name */
  stream->host = cpystr (hostname);

				/* get local name */
  status = net_get_hostname(&LocalhostDesc);
  if((status & 0x1) == 0) {
	sprintf(tmp, "Can't get local hostname, status=%d", status);
	mm_log(tmp, ERROR);
	return NIL;
  }
  strncpy(tmp, LocalhostDesc.dsc$a_pointer, LocalhostDesc.dsc$w_length);
  tmp[LocalhostDesc.dsc$w_length] = '\0';
  str$free1_dx(&LocalhostDesc);
  stream->localhost = cpystr (tmp);

/* Save parameters */
  stream->tcpsi = stream->tcpso = sock;
  stream->ictr = 0;		/* init input counter */
  return stream;		/* return success */
}

TCPSTREAM *tcp_aopen (host,service)
{
	return NIL;		/* Not supported here */
}


/*
 | return the local hostname in Buffer whose size is Size. This is a
 | replacement to the Gethostname() which does not exist in Nelib.
 */
get_local_host_name(buffer, size)
char	*buffer;
int	size;
{
  int	status;

  struct dsc$descriptor LocalhostDesc = { 0, DSC$K_DTYPE_T,
	DSC$K_CLASS_D, NULL };	/* Used to receive the local hostname */

  status = net_get_hostname(&LocalhostDesc);
  if((status & 0x1) == 0) {
	char	tmp[256];
	sprintf(tmp, "Can't get local hostname, status=%d", status);
	mm_log(tmp, ERROR);
	return NIL;
  }
  strncpy(buffer, LocalhostDesc.dsc$a_pointer, LocalhostDesc.dsc$w_length);
  buffer[LocalhostDesc.dsc$w_length] = '\0';
  str$free1_dx(&LocalhostDesc);
}



/* TCP/IP receive line
 * We do not use Netlib's get-line since it bombs...
 * Accepts: TCP/IP stream
 * Returns: text line string or NIL if failure
 */

char *tcp_getline (stream)
	TCPSTREAM *stream;
{
  int n,m;
  char *st,*ret,*stp;
  char c = '\0';
  char d;
				/* make sure have data */
  if (!tcp_getdata (stream)) return NIL;
  st = stream->iptr;		/* save start of string */
  n = 0;			/* init string count */
  while (stream->ictr--) {	/* look for end of line */
    d = *stream->iptr++;	/* slurp another character */
    if ((c == '\015') && (d == '\012')) {
      ret = (char *) fs_get (n--);
      memcpy (ret,st,n);	/* copy into a free storage string */
      ret[n] = '\0';		/* tie off string with null */
      return ret;
    }
    n++;			/* count another character searched */
    c = d;			/* remember previous character */
  }
				/* copy partial string from buffer */
  memcpy ((ret = stp = (char *) fs_get (n)),st,n);
				/* get more data from the net */
  if (!tcp_getdata (stream)) return NIL;
				/* special case of newline broken by buffer */
  if ((c == '\015') && (*stream->iptr == '\012')) {
    stream->iptr++;		/* eat the line feed */
    stream->ictr--;
    ret[n - 1] = '\0';		/* tie off string with null */
  }
				/* else recurse to get remainder */
  else if (st = tcp_getline (stream)) {
    ret = (char *) fs_get (n + 1 + (m = strlen (st)));
    memcpy (ret,stp,n);		/* copy first part */
    memcpy (ret + n,st,m);	/* and second part */
    fs_give ((void **) &stp);	/* flush first part */
    fs_give ((void **) &st);	/* flush second part */
    ret[n + m] = '\0';		/* tie off string with null */
  }

  return ret;
}

/* TCP/IP receive buffer
 * Accepts: TCP/IP stream
 *	    size in bytes
 *	    buffer to read into
 * Returns: T if success, NIL otherwise
 */

long tcp_getbuffer (stream,size,buffer)
	TCPSTREAM *stream;
	unsigned long size;
	char *buffer;
{
  unsigned long n;
  char *bufptr = buffer;
  while (size > 0) {		/* until request satisfied */
    if (!tcp_getdata (stream)) return NIL;
    n = min (size,stream->ictr);/* number of bytes to transfer */
				/* do the copy */
    memcpy (bufptr,stream->iptr,n);
    bufptr += n;		/* update pointer */
    stream->iptr +=n;
    size -= n;			/* update # of bytes to do */
    stream->ictr -=n;
  }
  bufptr[0] = '\0';		/* tie off string */
  return T;
}



long tcp_abort ();

/* TCP/IP receive data
 * Accepts: TCP/IP stream
 * Returns: T if success, NIL otherwise
 */

long tcp_getdata (stream)
	TCPSTREAM *stream;
{
  int i, status;
/* Note: the doc says we need here dynamic descriptor, but we need static one... */
  struct dsc$descriptor BufDesc = { BUFLEN, DSC$K_DTYPE_T, DSC$K_CLASS_S, stream->ibuf };
  static short iosb[4];

  if (stream->tcpsi < 0) return NIL;
  while (stream->ictr < 1) {	/* if nothing in the buffer */
    if(((status = tcp_receive(&(stream->tcpsi), &BufDesc, iosb)) & 0x1) == 0) {
	char tmp[512];
	sprintf(tmp, "Error reading from TcpIp/NETLIB, status=%d", status);
	mm_log(tmp, ERROR);
	return tcp_abort(stream);
    }
    if(iosb[1] > BUFLEN) 
	i = BUFLEN;
    else i = iosb[1];
    if (i < 1) return tcp_abort (stream);
    stream->ictr = i;		/* set new byte count */
    stream->iptr = stream->ibuf;/* point at TCP buffer */
  }
  return T;
}

/* TCP/IP send string as record
 * Accepts: TCP/IP stream
 *	    string pointer
 * Returns: T if success else NIL
 */

long tcp_soutr (stream,string)
	TCPSTREAM *stream;
	char *string;
{
  return tcp_sout (stream,string,(unsigned long) strlen (string));
}


/* TCP/IP send string
 * Accepts: TCP/IP stream
 *	    string pointer
 *	    byte count
 * Returns: T if success else NIL
 */

long tcp_sout (stream,string,size)
	TCPSTREAM *stream;
	char *string;
	unsigned long size;
{
  int status;

  struct dsc$descriptor_s BufDesc = { strlen(string), DSC$K_DTYPE_T,
		DSC$K_CLASS_S, string };

  status = tcp_send(&(stream->tcpso), &BufDesc, 2);	/*2 = Do not add \r\n */
  if((status & 0x1) == 0)
      return tcp_abort (stream);

  return T;			/* all done */
}

/* TCP/IP close
 * Accepts: TCP/IP stream
 */

void tcp_close (stream)
	TCPSTREAM *stream;
{
  tcp_abort (stream);		/* nuke the stream */
				/* flush host names */
  fs_give ((void **) &stream->host);
  fs_give ((void **) &stream->localhost);
  fs_give ((void **) &stream);	/* flush the stream */
}


/* TCP/IP abort stream
 * Accepts: TCP/IP stream
 * Returns: NIL always
 */

long tcp_abort (stream)
     TCPSTREAM *stream;
{
  if (stream->tcpsi >= 0) {	/* no-op if no socket */
    tcp_disconnect(&(stream->tcpsi));	/* nuke the socket */
    stream->tcpsi = stream->tcpso = -1;
  }
  return NIL;
}

/* TCP/IP get host name
 * Accepts: TCP/IP stream
 * Returns: host name for this stream
 */

char *tcp_host (stream)
	TCPSTREAM *stream;
{
  return stream->host;		/* return host name */
}


/* TCP/IP get local host name
 * Accepts: TCP/IP stream
 * Returns: local host name
 */

char *tcp_localhost (stream)
	TCPSTREAM *stream;
{
  return stream->localhost;	/* return local host name */
}
#endif	/* NETLIB */

#if !defined(NETLIB) && !defined(MULTINET)
/* Put dummy routines so user won't get error messages during link. */
TCPSTREAM *tcp_open (host,port)
{ return NIL; }

int get_local_host_name(buffer, size)
{ return NIL; }

char *tcp_getline (stream)
{ return NULL; }

long tcp_getbuffer (stream,size,buffer)
{ return NIL; }

long tcp_getdata (stream)
{ return NIL; }

long tcp_soutr (stream,string)
{ return NIL; }

long tcp_sout (stream,string,size)
{ return NIL; }

void tcp_close (stream)
{ }

char *tcp_host (stream)
{ return NULL; }

char *tcp_localhost (stream)
{ return NULL; }

#endif	/* !NETLIB || !MULTINET */

/*~~~~~~ Temporary */
gethostid()
{
	return 1;
}

char *strerror(code)
{
	static char	buffer[128];

	sprintf(buffer, "%d", code);
	return buffer;
}
