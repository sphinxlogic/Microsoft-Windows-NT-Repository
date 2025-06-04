/*	SI_NETLIB_SOCKET.C
 *	V1.0
 *+
 * Jacket routines for all of the si_xxxx routines.
 *-
 */

/*
 * NOTE: Never include socketshr.h, since we call the original routines here!
 */

#include <stdio.h>
#include <unixio.h>
#include <socket.h>
#include <signal.h>
#include <string.h>

/* Entry points which have no jacket routines here but are used here */

#define netlib_recv	si_recv
#define netlib_recvfrom si_recvfrom
#define netlib_send	si_send
#define netlib_sendto	si_sendto

#define isSocket isNetlibSocket
#define get_fd netlib_get_fd

#define TRACE /**/
/* #define FTRACE__  stdout /**/
#include "[-]si_trace.h"

int isSocket(int s);
int get_fd(FILE *fptr);
short int netlib_get_sdc(int s);
int netlib_close(int s);
int netlib_write (int s, void *buffer, int nbytes);
int netlib_read (int s, void *buffer, int nbytes);
int netlib_writev(int s, struct iovec *iov, int iovcnt);
int netlib_ioctl(int s, long request, void *argp);
int netlib_fnctl(int s, int cmd, int arg);
int netlib_recv(int s, char *buf, int len, int flags);
int netlib_recvfrom(int s, char *buf, int len, int flags,
			struct sockaddr *from, int *fromlen);
int netlib_send(int s, char *msg, int len, int flags);
int netlib_sendto(int s, char *msg, int len, int flags, 
			struct sockaddr *to, int tolen);
FILE *netlib_fdopen (int s, char *a_mode);

#ifndef VAXC
#define noshare
#endif

noshare FILE *ftrace__ = (FILE *)NULL;
noshare int trace__ = 0;
noshare int h_errno = 0;

/*
 * ======================================
 * si_get_sdc get VMS channel
 * ======================================
 */
short int
si_get_sdc(int s)
{
	STRACE("si_get_sdc",0,s,"",0,0);
	return( netlib_get_sdc(s) );
}

/*
 * ======================================
 * si_close
 * ======================================
 */
int
si_close(int s)
{
	STRACE("si_close",0,s,"",0,0);
	return( isSocket(s) ? netlib_close(s) : close(s) );
}

/*
 * ======================================
 * si_write
 * ======================================
 */
int
si_write (int s, void *buf, int l)
{
	STRACE("si_write",0,s,"%d bytes",l,0);
	return( isSocket(s) ? netlib_write(s, buf, l) : write(s, buf, l) );
}

/*
 * ======================================
 * si_writev
 * ======================================
 */
int
si_writev (int s, struct iovec *iov, int iovcnt)
{
	STRACE("si_writev",0,s,"",0,0);
	return( isSocket(s) ? netlib_writev(s, iov, iovcnt) : -1 );
}

/*
 * ======================================
 * si_read
 * ======================================
 */
int
si_read (int s, void *buf, int l)
{
	int i;
	STRACE("si_read",0,s,"%d bytes",l,0);
/* printf("si_read: s=%d, isSocket=%d\n",s,isSocket(s)); */
/*	return( isSocket(s) ? netlib_read(s, buf, l) : read(s, buf, l) ); */
	if ( !isSocket(s) ) return(read(s, buf, l));
	i = netlib_read(s, buf, l);
	STRACE("si_read",0,s,"return - %d bytes received",i,0);
	return(i);
}

#ifdef XXXX	/* handled by SOCKET_Netlib */
/*
 * ======================================
 * si_ioctl
 * ======================================
 */
int
si_ioctl(int s, long req, void *argp)
{
	STRACE("si_ioctl",0,s,"request: %d",req,0);
	return( isSocket(s) ? netlib_ioctl(s, req, argp) : -1 );
}

/*
 * ======================================
 * si_fcntl
 * ======================================
 */
int
si_fcntl(int s, int cmd, int arg)
{
	STRACE("si_fcntl",0,s,"cmd: %d, arg: %d",cmd,arg);
	return( isSocket(s) ? netlib_fcntl(s, cmd, arg) : -1 );
}
#endif

/*
 * ======================================
 * si_fdopen
 * ======================================
 */
FILE *
si_fdopen (int s, char *a_mode)
{
	STRACE("si_fdopen",0,s,"mode: %s",a_mode,0);
	return( isSocket(s) ? netlib_fdopen(s, a_mode) : fdopen(s, a_mode) );
}

/*
 * Now the standard I/O functions
 */

/*
 * ======================================
 * si_fileno
 * ======================================
 */
int
si_fileno(FILE * fptr)
{
	int s;
	FTRACE("si_fileno",0,fptr,"",0,0);
	if ( (s=get_fd(fptr)) != -1) return(s);
	return(fileno(fptr));
}

/*
 * ======================================
 * si_fread
 * ======================================
 */
int
si_fread(s, nsize, num, stream)
char *s;
int nsize;
int num;
FILE *stream;
{
  int fd;
  int i;

	FTRACE("si_fread",0,stream,"%d units of %d bytes",num,nsize);
/*  if ((fd=get_fd(stream)) != -1) return(netlib_recvfrom(fd,s,nsize*num,0,0,0)); */
  if ((fd=get_fd(stream)) != -1) {
	i=netlib_recvfrom(fd,s,nsize*num,0,0,0);
/* printf("si_fread: status=%d\n",i); */
/* if (i>0) {s[i]='\0';printf("\n{%s}\n\n",s);} */
	return( (i>0) ? i : 0 );
  }
  return(fread(s, nsize, num, stream));
}


/*
 * ======================================
 * si_fwrite
 * ======================================
 */
int
si_fwrite(s, nsize, num, stream)
char *s;
int nsize;
int num;
FILE *stream;
{
  int fd;

	FTRACE("si_fwrite",0,stream,"%d units of %d bytes",num,nsize);
  if ((fd=get_fd(stream)) != -1) return(netlib_sendto(fd,s,nsize*num,0,0,0));
  return(fwrite(s, nsize, num, stream));
}

/*
 * ======================================
 * si_fgetc
 * ======================================
 */
char
si_fgetc(stream)
FILE *stream;
{
  int fd;
  char buf[1];

	FTRACE("si_fgetc",0,stream,"",0,0);
  if ((fd=get_fd(stream)) != -1) {
        if (netlib_read(fd,buf,1) == -1) {
	   FTRACE("si_fgetc",0,stream,"ferror: %d, feof: %d",ferror(stream),feof(stream));
           return(EOF);
        }
     return(*buf);
  }
  return(fgetc(stream));
}


/*
 * ======================================
 * si_fgets
 * ======================================
 */
char *
si_fgets(s,n,stream)
char *s;
int n;
FILE *stream;
{
  int fd, i;
  char buf[1], *t;

	FTRACE("si_fgets",0,stream,"%d bytes max",n,0);
  /* we need to read a character string from the socket which is */
  /* terminated by a nl. Do this the hard way by reading 1 char  */
  /* at a time and testing it */
  if ((fd=get_fd(stream)) != -1) {
     t = s;
     for (i=1;i<n;i++) {
        if (netlib_read(fd,buf,1) == 0) {
           return(0);
        }
        *t++ = buf[0];
        if (buf[0] == '\n') break;
     }
     *t = '\0';
     return(s);
  }
  return(fgets(s,n,stream));
}

/*
 * ======================================
 * si_fputc
 * ======================================
 */
int
si_fputc(c,stream)
char c;
FILE *stream;
{
  int fd;

	FTRACE("si_fputc",0,stream,"char: {%c} (0x%02X)",c,c);
  if ((fd=get_fd(stream)) != -1) return(netlib_sendto(fd,&c,1,0,0,0));
  return(fputc(c,stream));
}

/*
 * ======================================
 * si_fputs
 * ======================================
 */
int
si_fputs(s,stream)
char *s;
FILE *stream;
{
  int fd;

	FTRACE("si_fputs",0,stream,"%d bytes",strlen(s),0);
  if ((fd=get_fd(stream)) != -1) return(netlib_sendto(fd,s,strlen(s),0,0,0));
  return(fputs(s,stream));
}


/*
 * ======================================
 * si_fclose
 * ======================================
 */
int
si_fclose(stream)
FILE *stream;
{
  int fd;

	FTRACE("si_fclose",0,stream,"",0,0);
  if ((fd=get_fd(stream)) != -1) return(netlib_close(fd));
  return(fclose(stream));
}

/*
 * ======================================
 * si_rewind
 * ======================================
 */
int
si_rewind(stream)
FILE *stream;
{
  int fd;

	FTRACE("si_rewind",0,stream,"",0,0);
  /* don't do anything if its one of ours */
  if ((fd=get_fd(stream)) != -1) return(0);
  return(rewind(stream));
}

/*
 * ======================================
 * si_fflush
 * ======================================
 */
int
si_fflush(stream)   
FILE *stream;
{
  int fd;

	FTRACE("si_fflush",0,stream,"",0,0);
  /* don't do anything if its one of ours */
  if ((fd=get_fd(stream)) != -1) return(0);
  return(fflush(stream));
}


/*
 * ======================================
 * si_fprintf
 * ======================================
 */
int
si_fprintf(stream,format,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16)
FILE *stream;
char *format;
int *a1,*a2,*a3,*a4,*a5,*a6,*a7,*a8,*a9,*a10,*a11,*a12,*a13,*a14,*a15,*a16;
{
  char buf[1024];
  int fd;

	FTRACE("si_fprintf",0,stream,"format: {%s}",format,0);
  /* this is not pretty but how else can we handle variable number of */
  /* arguments except declare lots of them, no checking is done if we */
  /* supply to few. Of course if more than 16 are passed we get into  */
  /* awful trouble */
  if ((fd=get_fd(stream)) != -1) {
     sprintf(buf,format,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16);
     return(netlib_send(fd,buf,strlen(buf),0));
  }
  return(fprintf(stream,format,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16));
}

/*
 * Unsupported functions.
 */

/*
 * ======================================
 * si_signal
 * ======================================
 */
void
si_signal(sig,func)
int sig;
void (*func)();
{
	signal(sig,func);
}

/*
 * ======================================
 * si_sigvec
 * ======================================
 */
int
si_sigvec(sig,vec,ovec)
int sig;
struct sigvec *vec, *ovec;
{
	return(sigvec(sig,vec,ovec));
}

/*
 * ======================================
 * si_alarm
 * ======================================
 */
int
si_alarm(i)
int i;
{
	return(alarm(i));
}

