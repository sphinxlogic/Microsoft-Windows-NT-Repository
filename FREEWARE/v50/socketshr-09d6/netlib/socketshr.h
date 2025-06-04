/*	socketshr.h	*/
/*	SOCKETSHR V0.9D	*/

#ifndef _SOCKET_FUNCTIONS
#define _SOCKET_FUNCTIONS

#if defined(__DECC) || defined(__DECCXX)
#pragma __nostandard  /* This file uses non-ANSI-Standard features */
#else
#pragma nostandard
#endif

#ifdef __cplusplus
    extern "C" {
#endif


/*
 * Define routines supported SOCKETSHR so that they are linked
 * to SOCKETSHR instead of the compiler's run time library.
 */

/* stdio */

#define fgets si_fgets
#define fputs si_fputs
#define fgetc si_fgetc
#define fputc si_fputc
#define fprintf si_fprintf
#define fflush si_fflush
#define fclose si_fclose
#define fread si_fread
#define fwrite si_fwrite
#define rewind si_rewind
#define fdopen si_fdopen
/* use standard fileno() now
     #define fileno si_fileno
*/

/* unixio */

#define read si_read
#define write si_write
#define close si_close

/* signal (not really used currently, reserved for future) */

#define signal  si_signal
#define sigvec  si_sigvec
#define alarm   si_alarm

/* all other routines */

#define socket si_socket
#define bind si_bind
#define connect si_connect
#define listen si_listen
#define accept si_accept
#define recv si_recv
#define recvfrom si_recvfrom
#define send si_send
#define sendto si_sendto
#define getsockname si_getsockname
#define select si_select
#define ioctl si_ioctl
#define shutdown si_shutdown
#define getsockopt si_getsockopt
#define setsockopt si_setsockopt
#define writev si_writev
#define fcntl si_fcntl
#define getservbyname si_getservbyname
#define getservbyport si_getservbyport
#define getservent si_getservent
#define setservent si_setservent
#define endservent si_endservent
#define getprotobyname si_getprotobyname
#define getprotobynumber si_getprotobynumber
#define getprotoent si_getprotoent
#define setprotoent si_setprotoent
#define endprotoent si_endprotoent
#define gethostbyname si_gethostbyname
#define gethostbyaddr si_gethostbyaddr
#define gethostent si_gethostent
#define sethostent si_sethostent
#define endhostent si_endhostent
#define gethostname si_gethostname
#define getpeername si_getpeername
#define inet_addr si_inet_addr
#define inet_ntoa si_inet_ntoa
#define ntohl si_ntohl
#define ntohs si_ntohs
#define htonl si_htonl
#define htons si_htons

#define vaxc$get_sdc si_get_sdc
#define decc$get_sdc si_get_sdc
#define get_sdc si_get_sdc

/*
 * PROTOTYPES
 *
 * If the appropriate header files (e.g. socket.h) are already loaded before,
 * we have to prototype the si_ routines here. If the header file has not
 * been loaded we leave that to the header file which would prototype the
 * routine then.
 *
 * To determine if the header file is already included we try to use a
 * definition which "is a must" in that file, e.g. AF_INET in socket.h.
 * If there is no such symbol (like in unixio.h) we have to use the
 * header file's own indicator (e.g. __UNIXIO_LOADED) which is an
 * internal symbol and may change in future versions of the header files.
 *
 * It is recommended to include SOCKETSHR.H *after* including other related
 * header files, since they may re-define something.
 *
 * To keep it simple, we do not include any other header file here. You
 * should just add #include <socketshr.h> behind the other includes like
 * stdio.h, socket.h, unixio.h, in.h, inet.h, netdb.h
 *
 * The following definitions are from the header files of DECC and from
 * some UNIX header files.
 *
 * __HIDE_FORBIDDEN_NAMES is defined only in strict ANSI mode on DECC.
 */


/* socket */
#ifdef AF_INET
int socket (int __af, int __mess_type, int __prot_type);
int accept (int __sd, struct sockaddr *__S_addr, int *__addrlen);
int bind (int __sd, struct sockaddr *__s_name, int __namelen);
int listen (int __sd, int __backlog);
int connect (int __sd, struct sockaddr *__name, int __namelen);
int send (int __sd, char *__msg, int __length, int __flags);
/* not supported
int recvmsg (int __sd, struct msghdr *__msg, int __flags);
int sendmsg (int __sd, struct msghdr *__msg, int __flags);
*/
int sendto (int __sd, char *__msg, int __length, int __flags, struct sockaddr *__to, int __tolen);
int recv (int __sd, char *__buf, int __length, int __flags);
int recvfrom (int __sd, char *__buf, int __length, int __flags, struct sockaddr *__from, int *__fromlen);
int shutdown (int __sd, int __mode);
int select (int __nfds, int *__readfds, int *__writefds, int *__exceptfds, struct timeval *__timeout);
int gethostname (char *__name, int __namelen);
/* not supported
int gethostaddr (char *__addr);
*/
int getpeername (int __sd, struct sockaddr *__name, int *__namelen);
int getsockname (int __sd, struct sockaddr *__name, int *__namelen);
int getsockopt (int __sd, int __level, int __optname, char *__optval, int *__optlen);
int setsockopt (int __sd, int __level, int __optname, char *__optval, int __optlen);
int decc$get_sdc (int __descrip_no);
#endif


/* in */
#ifdef IPPROTO_TCP
unsigned long  int htonl ( unsigned long int __n);
unsigned short int htons ( unsigned short int __n);
unsigned long  int ntohl ( unsigned long int __n);
unsigned short int ntohs ( unsigned short int __n);
#endif


/* inet */
#ifdef __INET_LOADED
int inet_addr (char *__cp);
char *inet_ntoa (struct in_addr __in); 
#endif


/* netdb */
#ifdef __NETDB_LOADED
struct hostent	*gethostbyaddr (char *__addr, int __len, int __type);
struct hostent	*gethostbyname (char *__name);
struct hostent	*gethostent ();
int sethostent(int __stayopen);
int endhostent();

struct servent	*getservbyname (char *__name, char *__proto);
struct servent	*getservbyport (int __port, char *__proto);
struct servent	*getservent ();
int setservent(int __stayopen);
int endservent();

struct protoent	*getprotobyname (char *__name);
struct protoent	*getprotobynumber (int __proto);
struct protoent	*getprotoent ();
int setprotoent(int __stayopen);
int endprotoent();

/* getnetxxx not supported
struct netent	*getnetbyaddr (long __net, int __type);
struct netent	*getnetbyname (char *__name);
struct netent	*getnetent ();
int setnetent(int __stayopen);
int endnetent();
*/
#endif


/* stdio */
#ifdef EOF
int	fgetc	(FILE *__stream);
int	fputc	(int __c, FILE *__stream);
char   *fgets	(char *__s, int __n, FILE *__stream);
int	fputs	(const char *__s, FILE *__stream);
int	fprintf	(FILE *__stream, const char *__format, ...);
int	fclose	(FILE *__stream);
int	fflush	(FILE *__stream);
size_t	fread	(void *__ptr, size_t __size, size_t __nmemb, FILE *__stream);
size_t	fwrite	(const void *__ptr, size_t __size, size_t __nmemb, FILE *__stream);
/* ANSI defines rewind() to return void, but the DEC C RTL returns an
   int status. */
#if __HIDE_FORBIDDEN_NAMES
void	rewind	(FILE *__stream);
#else
int     rewind  (FILE *__stream);
FILE   *fdopen	(int __file_desc, char *__mode);
#endif
#endif


/* unixio */
#ifdef __UNIXIO_LOADED
int	close	(int __file_desc);
int	read	(int __file_desc, void *__buffer, int __nbytes);
int	write	(int __file_desc, void *__buffer, int __nbytes);
#endif


/* signal */
#ifdef SIGINT
void	(*signal (int __sig, void (*__func)(int))) (int);
#ifndef __HIDE_FORBIDDEN_NAMES
int	alarm	(unsigned int __seconds);
int	sigvec	(int __sigveca, struct sigvec *__sv, struct sigvec *__osv);
#endif
#endif

/*
 * Prototypes for routines not defined in VAXC or DECC RTL.
 */

int	ioctl	(int __file_desc, long __request, void *__argp);
int	fcntl	(int __file_desc, int __cmd, int __arg);
/* int	writev	(int __file_desc, struct iovec *__iov, int __iovcnt); */
int	writev	(int __file_desc, void *__iov, int __iovcnt);


#ifdef __cplusplus
    }
#endif

#if defined(__DECC) || defined(__DECCXX)
#pragma __standard  /* This file uses non-ANSI-Standard features */
#else
#pragma standard
#endif

#endif	/* _SOCKET_FUNCTIONS */
