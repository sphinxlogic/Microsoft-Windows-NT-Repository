/*
 * internal interfaces of socketshr
 */

#include "[-]si_socket.h"	/* redefines ntoh_hton.c interface */

#include <sys/types.h>
#include <netdb.h>		/* struct protoent, etc. */
#include <netinet/in.h>

/* inet.c */
extern char *inet_ntoa(struct in_addr in);
extern unsigned long inet_addr(char *str);

/* ntoh_hton.c */
#ifndef __INET_LOADED		/* <inet.h> */
#ifndef ____IN_PORT_T
#   define ____IN_PORT_T 1
    typedef unsigned short __in_port_t;
#endif

#ifndef ____IN_ADDR_T
#   define ____IN_ADDR_T 1
    typedef unsigned int __in_addr_t;
#endif

extern __in_addr_t htonl(__in_addr_t x);
extern __in_addr_t ntohl(__in_addr_t x);
extern __in_port_t htons(__in_port_t x);
extern __in_port_t ntohs(__in_port_t x);
#endif /* __INET_LOADED */

/* readdb.c */
extern int socket_endprotoent(void);
extern int socket_endservent(void);
extern int socket_setprotoent(int stayopen);
extern int socket_setservent(int stayopen);
extern struct protoent *socket_getprotobyname(char *name);
extern struct protoent *socket_getprotobynumber(int proto);
extern struct protoent *socket_getprotoent(void);
extern struct servent *socket_getservbyname(char *name, char *proto);
extern struct servent *socket_getservbyport(int port, char *proto);
extern struct servent *socket_getservent(void);

/* trnlnm.c */
extern int __trnlnm(char *table, char *name, char *mode, char *buff, int len);
