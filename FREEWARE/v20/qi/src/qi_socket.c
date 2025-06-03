/* TCP/IP routines stolen from the VMS GOPHERD */

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <socket.h>
#include <in.h>
#include <netdb.h>
#include <inet.h>
#include <unixio.h>

#include <errno.h>

#if defined(MULTINET)
extern int socket_close();
extern int socket_read();
extern int socket_write();
extern int multinet_getpeername();
#define getpeername		multinet_getpeername
#endif 

#if defined(VMS) && (defined(WOLLONGONG) || defined(MULTINET) || defined(UCX))
/* Multinet and Wollongong (non UCX-emulation) use channel numbers */
/* for sockets, which are small multiples of 16.  The first 5 */
/* channels can be assumed to be already used, so we assume that */
/* sockets start at 64, and that only 64 VAXC fds are simultaneously */
/* open in the program.  Actually, the first socket is likely to be */
/* more like 176! */
#define IS_SOCKET(s) ((s)>=64)
 
/* Close a socket.  
 * Note that in old Wollongong and Multinet implementations close()
 * works only on fds, not sockets.
 * For UCX and Unix, closenet() is #defined to be close()
 */
int closenet(s)
int s;
{
    if (IS_SOCKET(s)) {
#ifdef MULTINET
	return (socket_close(s));
#endif
#ifdef WOLLONGONG
	return (netclose(s));
#endif
#ifdef UCX
	return (close(s));
#endif
    }
    else
	close(s); /* non-channel UCX socket */
}
#endif /* WOLLONGONG or MULTINET or UCX */


/* Read "n" bytes from a descriptor.
 * Use in place of read() when fd is a stream socket
 *
 * Returns the number of total bytes read.
 */

int readn(fd, ptr, nbytes)
  int fd;
  char *ptr;
  int nbytes;
{
     int nleft, nread;
     
     nleft = nbytes;
     while (nleft > 0) {
#if defined(VMS) && defined(WOLLONGONG)
	  nread = IS_SOCKET(fd) ? netread(fd, ptr, nleft) : read(fd, ptr, nleft);
#else
#if defined(VMS) && defined(MULTINET)
	  nread = IS_SOCKET(fd) ? socket_read(fd, ptr, nleft) : read(fd, ptr, nleft);
#else
	  nread = read(fd, ptr, nleft);
#endif
#endif
#if defined(VMS) && defined(UCX)
          if ((nread < 0) && (errno == EPIPE))
	       break;
#endif
	  if (nread < 0)
	       return(nread);	/* error, return <0 */
	  else if (nread == 0)	/* EOF */
	       break;
	  
	  nleft 	-= nread;
	  ptr 	+= nread;
     }
     return(nbytes - nleft);	/* return >= 0) */
}



/*
 * Write "n" bytes to a descriptor.
 * Use in place of write() when fd is a stream socket
 *
 * We return the number of bytes written
 */

int 
writen(fd, ptr, nbytes)
  int	fd;
  char	*ptr;
  int	nbytes;
{
     int nleft, nwritten;
     
     nleft = nbytes;
     while(nleft > 0) {
#if defined(VMS) && defined(WOLLONGONG)
	  nwritten = IS_SOCKET(fd) ? netwrite(fd, ptr, nleft) : write(fd, ptr, nleft);
#else
#if defined(VMS) && defined(MULTINET)
	  nwritten = IS_SOCKET(fd) ? socket_write(fd, ptr, nleft) : write(fd, ptr, nleft);
#else
	  nwritten = write(fd, ptr, nleft);
#endif
#endif
	  if (nwritten <= 0)
	       return(nwritten);	/* error */
	  
	  nleft	-= nwritten;
	  ptr	+= nwritten;
     }
     return(nbytes - nleft);
}


/*
 * Writestring uses the writen and strlen calls to write a
 * string to the file descriptor fd.  If the write fails
 * a -1 is returned. Otherwise zero is returned.
 */

int writestring(fd, stringptr)
  int	fd;
  char	*stringptr;
{
     int length;

     if (stringptr == NULL)
	  return(0);

     length = strlen(stringptr);
     if (writen(fd, stringptr, length) != length) {
	  return(-1);
     }
     else
	  return(0);
}

/*
 * Read from the socket into a buffer.  Mucho more efficent in terms of
 * system calls..
 * 
 * returns bytes read, or <0 for an error
 */

#define RECVSIZE 4096

int readrecvbuf(sockfd, buf, len)
  int sockfd;
  char *buf;
  int len;
{
     static char recvbuf[RECVSIZE];
     static int  recvbufptr  = 0;
     static int  recvbufsize = 0;
     static int Oldsockfd = 0;
     int bytesread = 0;

     if ((recvbufptr == 0) || (Oldsockfd != sockfd)) {
#if defined(VMS) && defined(WOLLONGONG)
	  recvbufsize = IS_SOCKET(sockfd) ?
		netread(sockfd, recvbuf, RECVSIZE) : read(sockfd, recvbuf, RECVSIZE);
#else
#if defined(VMS) && defined(MULTINET)
	  recvbufsize = IS_SOCKET(sockfd) ?
		socket_read(sockfd, recvbuf, RECVSIZE) : read(sockfd, recvbuf, RECVSIZE);
#else
	  recvbufsize = read(sockfd, recvbuf, RECVSIZE);
#endif
#endif
	  Oldsockfd = sockfd;
	  recvbufptr = 0;
	  if (recvbufsize == 0)
	       return(0);
#if defined(VMS) && defined(UCX)
	  if ((recvbufsize < 0) && (errno == EPIPE))
	       return(0);
#endif

     }
     
     while (len--) {
	  *buf++ = recvbuf[recvbufptr++];
	  bytesread++;

	  if ((recvbufptr == recvbufsize) && (len != 0)) {
	       recvbufsize = readn(sockfd, recvbuf, RECVSIZE);
	       recvbufptr = 0;
	       if (recvbufsize == 0)
		    return(bytesread);
	       if (recvbufsize < 0)
		    return(recvbufsize);

	  } else if (recvbufptr >= recvbufsize)
	       recvbufptr = 0;
     }
     return(bytesread);
}     


/*
 * Read a line from a descriptor.  Read the line one byte at a time,
 * looking for the newline.  We store the newline in the buffer,
 * then follow it with a null (the same as fgets(3)).
 * We return the number of characters up to, but not including,
 * the null (the same as strlen(3))
 */

int readline(fd, ptr, maxlen)
  int	fd;
  char	*ptr;
  int 	maxlen;
{
     int n;
     int rc;
     char c;

     
     for (n=1; n < maxlen; n++) {
	  if ( (rc = readrecvbuf(fd, &c, 1)) == 1) {
	       *ptr++ = c;
	       if (c == '\n')
		    break;
	  }
	  else if (rc == 0) {
	       if (n == 1)
		    return(0);	/* EOF, no data read */
	       else
		    break;		/* EOF, some data was read */
	  }
	  else
	       return(-1);		/* error */
     }
     
     *ptr = 0; 				/* Tack a NULL on the end */

     return(n);
}




void inet_netnames(int sockfd, char *host_name, char *ipnum)
{
     struct sockaddr_in      sa;
     int                     length;
     u_long                  net_addr;
     struct hostent          *hp;

     length = sizeof(sa);
     if (getpeername(sockfd, (struct sockaddr *)&sa, &length) == -1) {
/*        qilog(-2, "getpeername() failure: %s", vms_errno_string()); */
	  strcpy(ipnum,"Unknown");
	  strcpy(host_name,"Unknown");
	  return;
     }
     strcpy(ipnum, (char *) inet_ntoa(sa.sin_addr));
     if (host_name != ipnum)
          strcpy(host_name, (char *) inet_ntoa(sa.sin_addr));

     hp = gethostbyaddr((char *) &sa.sin_addr,
			sizeof (sa.sin_addr.s_addr), AF_INET);
     
     if (hp != NULL)
	  (void) strcpy(host_name, hp->h_name);
     else {
/**	  qilog(-2, "gethostbyaddr(%s) failure", ipnum); /**Diagnostic**/
	  strcpy(host_name, ipnum);
     }
}
