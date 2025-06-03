#ifndef __NETLIB_TCPWARE_H__
#define __NETLIB_TCPWARE_H__
/*
**  NETLIB_TCPWARE.H
**
**  TCPware-specific definitions for NETLIB_SRI_SUPPORT.C.
**
**  25-OCT-1994
*/
    static char *socket_library[] = {"TCPWARE_SOCKLIB_SHR", "TCPIP_SOCKLIB"};
    static char *nameserver_lognam[] = {"TCPWARE_NAMESERVERS", "TCPIP_NAMESERVERS"};
    static char *gethostname_ent[] = {"TCPWARE_GETHOSTNAME", "GETHOSTNAME"};
    static char *_gethostbyname_ent[] = {"TCPWARE_GETHOSTBYNAME", "GETHOSTBYADDR"};
    static char *_gethostbyaddr_ent[] = {"TCPWARE_GETHOSTBYADDR", "GETHOSTBYNAME"};
    static char *domain_lognam[] = {"TCPWARE_LOCALDOMAIN", "TCPIP_LOCALDOMAIN"};

#endif /* __NETLIB_TCPWARE_H__ */
