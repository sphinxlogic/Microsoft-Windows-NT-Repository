/* $XFree86: mit/server/os/xdmcp.c,v 2.1 1993/09/27 12:29:46 dawes Exp $ */
/* $XConsortium: xdmcp.c,v 1.22 92/05/19 17:22:10 keith Exp $ */
/*
 * Copyright 1989 Network Computing Devices, Inc., Mountain View, California.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of N.C.D. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  N.C.D. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 */

#ifdef XDMCP

#include "Xos.h"
#ifndef _MINIX
#include <sys/param.h>
#ifndef AMOEBA
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#else
#include <server/ip/gen/netdb.h>
#endif
#endif /* _MINIX */
#if _MINIX
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/netlib.h>
#include <net/gen/netdb.h>
#include <net/gen/socket.h>
#include <net/gen/udp.h>
#include <net/gen/udp_hdr.h>
#include <net/gen/udp_io.h>
#endif
#include <stdio.h>
#include "X.h"
#include "Xmd.h"
#include "misc.h"
#include "osdep.h"
#include "input.h"
#include "dixstruct.h"
#include "opaque.h"

#undef REQUEST
#include "Xdmcp.h"

extern int argcGlobal;
extern char **argvGlobal;
extern char *display;
extern long EnabledDevices[];
extern long AllClients[];
extern char *defaultDisplayClass;

static int		    xdmcpSocket, sessionSocket;
static xdmcp_states	    state;
static struct sockaddr_in   req_sockaddr;
static int		    req_socklen;
static CARD32		    SessionID;
static long		    timeOutTime;
static int		    timeOutRtx;
static long		    defaultKeepaliveDormancy = XDM_DEF_DORMANCY;
static long		    keepaliveDormancy = XDM_DEF_DORMANCY;
static CARD16		    DisplayNumber;
static xdmcp_states	    XDM_INIT_STATE = XDM_OFF;
#ifdef HASXDMAUTH
static char		    *xdmAuthCookie;
#endif

static XdmcpBuffer	    buffer;

static struct sockaddr_in   ManagerAddress;

static get_manager_by_name(), get_xdmcp_sock();

#ifndef _MINIX
static	receive_packet(), send_packet();
#else 
static	MNX_receive_packet(), send_packet();
#endif
static	timeout(), restart();

static	recv_willing_msg();
static	recv_accept_msg(),	recv_decline_msg();
static	recv_refuse_msg(),	recv_failed_msg();
static	recv_alive_msg();

static	send_query_msg();
static	send_request_msg();
static	send_manage_msg();
static	send_keepalive_msg();

static XdmcpFatal(), XdmcpWarning();
#ifndef _MINIX
static void XdmcpBlockHandler(), XdmcpWakeupHandler();
#else /* _MINIX */
static void MNX_XdmcpBlockHandler(), MNX_XdmcpWakeupHandler();
#endif /* !_MINIX */

static short	xdm_udp_port = XDM_UDP_PORT;
static Bool	OneSession = FALSE;

XdmcpUseMsg ()
{
    ErrorF("-query host-name       contact named host for XDMCP\n");
    ErrorF("-broadcast             broadcast for XDMCP\n");
    ErrorF("-indirect host-name    contact named host for indirect XDMCP\n");
    ErrorF("-port port-num         UDP port number to send messages to\n");
    ErrorF("-once                  Terminate server after one session\n");
    ErrorF("-class display-class   specify display class to send in manage\n");
#ifdef HASXDMAUTH
    ErrorF("-cookie xdm-auth-bits  specify the magic cookie for XDMCP\n");
#endif
    ErrorF("-displayID display-id  manufacturer display ID for request\n");
}

int 
XdmcpOptions(argc, argv, i)
    int	    argc, i;
    char    **argv;
{
    if (strcmp(argv[i], "-query") == 0) {
	get_manager_by_name(argc, argv, ++i);
	XDM_INIT_STATE = XDM_QUERY;
	AccessUsingXdmcp ();
	return (i + 1);
    }
    if (strcmp(argv[i], "-broadcast") == 0) {
	XDM_INIT_STATE = XDM_BROADCAST;
	AccessUsingXdmcp ();
	return (i + 1);
    }
    if (strcmp(argv[i], "-indirect") == 0) {
	get_manager_by_name(argc, argv, ++i);
	XDM_INIT_STATE = XDM_INDIRECT;
	AccessUsingXdmcp ();
	return (i + 1);
    }
    if (strcmp(argv[i], "-port") == 0) {
	++i;
	xdm_udp_port = atoi(argv[i]);
	return (i + 1);
    }
    if (strcmp(argv[i], "-once") == 0) {
	OneSession = TRUE;
	return (i + 1);
    }
    if (strcmp(argv[i], "-class") == 0) {
	++i;
	defaultDisplayClass = argv[i];
	return (i + 1);
    }
#ifdef HASXDMAUTH
    if (strcmp(argv[i], "-cookie") == 0) {
	++i;
	xdmAuthCookie = argv[i];
	return (i + 1);
    }
#endif
    if (strcmp(argv[i], "-displayID") == 0) {
	++i;
	XdmcpRegisterManufacturerDisplayID (argv[i], strlen (argv[i]));
	return (i + 1);
    }
    return (i);
}

/*
 * This section is a collection of routines for
 * registering server-specific data with the XDMCP
 * state machine.
 */


/*
 * Save all broadcast addresses away so BroadcastQuery
 * packets get sent everywhere
 */

#define MAX_BROADCAST	10

static struct sockaddr_in   BroadcastAddresses[MAX_BROADCAST];
static int		    NumBroadcastAddresses;

#ifndef _MINIX
XdmcpRegisterBroadcastAddress (addr)
    struct sockaddr_in	*addr;
{
    struct sockaddr_in	*bcast;
    if (NumBroadcastAddresses >= MAX_BROADCAST)
	return;
    bcast = &BroadcastAddresses[NumBroadcastAddresses++];
    bzero (bcast, sizeof (struct sockaddr_in));
    bcast->sin_family = addr->sin_family;
    bcast->sin_port = htons (xdm_udp_port);
    bcast->sin_addr = addr->sin_addr;
}
#else /* _MINIX */
MNX_XdmcpRegisterBroadcastAddress (addr)
    ipaddr_t	addr;
{
    struct sockaddr_in	*bcast;
    if (NumBroadcastAddresses >= MAX_BROADCAST)
	return;
    bcast = &BroadcastAddresses[NumBroadcastAddresses++];
    bzero (bcast, sizeof (struct sockaddr_in));
    bcast->sin_family = AF_INET;
    bcast->sin_port = htons (xdm_udp_port);
    bcast->sin_addr = addr;
}
#endif /* !_MINIX */

/*
 * Each authentication type is registered here; Validator
 * will be called to check all access attempts using
 * the specified authentication type
 */

static ARRAYofARRAY8	AuthenticationNames, AuthenticationDatas;
typedef struct _AuthenticationFuncs {
    Bool    (*Validator)();
    Bool    (*Generator)();
    Bool    (*AddAuth)();
} AuthenticationFuncsRec, *AuthenticationFuncsPtr;

static AuthenticationFuncsPtr	AuthenticationFuncsList;

XdmcpRegisterAuthentication (name, namelen, data, datalen, Validator, Generator, AddAuth)
    char    *name;
    int	    namelen;
    char    *data;
    int	    datalen;
    Bool    (*Validator)();
    Bool    (*Generator)();
    Bool    (*AddAuth)();
{
    int	    i;
    ARRAY8  AuthenticationName, AuthenticationData;
    static AuthenticationFuncsPtr	newFuncs;

    if (!XdmcpAllocARRAY8 (&AuthenticationName, namelen))
	return;
    if (!XdmcpAllocARRAY8 (&AuthenticationData, datalen))
    {
	XdmcpDisposeARRAY8 (&AuthenticationName);
	return;
    }
    for (i = 0; i < namelen; i++)
	AuthenticationName.data[i] = name[i];
    for (i = 0; i < datalen; i++)
	AuthenticationData.data[i] = data[i];
    if (!(XdmcpReallocARRAYofARRAY8 (&AuthenticationNames,
				     AuthenticationNames.length + 1) &&
	  XdmcpReallocARRAYofARRAY8 (&AuthenticationDatas,
				     AuthenticationDatas.length + 1) &&
	  (newFuncs = (AuthenticationFuncsPtr) xalloc (
			(AuthenticationNames.length + 1) * sizeof (AuthenticationFuncsRec)))))
    {
	XdmcpDisposeARRAY8 (&AuthenticationName);
	XdmcpDisposeARRAY8 (&AuthenticationData);
	return;
    }
    for (i = 0; i < AuthenticationNames.length - 1; i++)
	newFuncs[i] = AuthenticationFuncsList[i];
    newFuncs[AuthenticationNames.length-1].Validator = Validator;
    newFuncs[AuthenticationNames.length-1].Generator = Generator;
    newFuncs[AuthenticationNames.length-1].AddAuth = AddAuth;
    xfree (AuthenticationFuncsList);
    AuthenticationFuncsList = newFuncs;
    AuthenticationNames.data[AuthenticationNames.length-1] = AuthenticationName;
    AuthenticationDatas.data[AuthenticationDatas.length-1] = AuthenticationData;
}

/*
 * Select the authentication type to be used; this is
 * set by the manager of the host to be connected to.
 */

ARRAY8		noAuthenticationName = {(CARD16) 0, (CARD8Ptr) 0};
ARRAY8		noAuthenticationData = {(CARD16) 0, (CARD8Ptr) 0};
ARRAY8Ptr	AuthenticationName = &noAuthenticationName;
ARRAY8Ptr	AuthenticationData = &noAuthenticationData;
AuthenticationFuncsPtr	AuthenticationFuncs;

XdmcpSetAuthentication (name)
    ARRAY8Ptr	name;
{
    int	i;

    for (i = 0; i < AuthenticationNames.length; i++)
	if (XdmcpARRAY8Equal (&AuthenticationNames.data[i], name))
	{
	    AuthenticationName = &AuthenticationNames.data[i];
	    AuthenticationData = &AuthenticationDatas.data[i];
	    AuthenticationFuncs = &AuthenticationFuncsList[i];
	    break;
	}
}

/*
 * Register the host address for the display
 */

static ARRAY16		ConnectionTypes;
static ARRAYofARRAY8	ConnectionAddresses;
static long		xdmcpGeneration;

XdmcpRegisterConnection (type, address, addrlen)
    int	    type;
    char    *address;
    int	    addrlen;
{
    int	    i;
    CARD8   *newAddress;

    if (xdmcpGeneration != serverGeneration)
    {
	XdmcpDisposeARRAY16 (&ConnectionTypes);
	XdmcpDisposeARRAYofARRAY8 (&ConnectionAddresses);
	xdmcpGeneration = serverGeneration;
    }
    newAddress = (CARD8 *) xalloc (addrlen * sizeof (CARD8));
    if (!newAddress)
	return;
    if (!XdmcpReallocARRAY16 (&ConnectionTypes, ConnectionTypes.length + 1))
    {
	xfree (newAddress);
	return;
    }
    if (!XdmcpReallocARRAYofARRAY8 (&ConnectionAddresses,
				    ConnectionAddresses.length +  1))
    {
	xfree (newAddress);
	return;
    }
    ConnectionTypes.data[ConnectionTypes.length - 1] = (CARD16) type;
    for (i = 0; i < addrlen; i++)
	newAddress[i] = address[i];
    ConnectionAddresses.data[ConnectionAddresses.length-1].data = newAddress;
    ConnectionAddresses.data[ConnectionAddresses.length-1].length = addrlen;
}

/*
 * Register an Authorization Name.  XDMCP advertises this list
 * to the manager.
 */

static ARRAYofARRAY8	AuthorizationNames;

XdmcpRegisterAuthorizations ()
{
    XdmcpDisposeARRAYofARRAY8 (&AuthorizationNames);
    RegisterAuthorizations ();
}

XdmcpRegisterAuthorization (name, namelen)
    char    *name;
    int	    namelen;
{
    ARRAY8  authName;
    int	    i;

    authName.data = (CARD8 *) xalloc (namelen * sizeof (CARD8));
    if (!authName.data)
	return;
    if (!XdmcpReallocARRAYofARRAY8 (&AuthorizationNames, AuthorizationNames.length +1))
    {
	xfree (authName.data);
	return;
    }
    for (i = 0; i < namelen; i++)
	authName.data[i] = (CARD8) name[i];
    authName.length = namelen;
    AuthorizationNames.data[AuthorizationNames.length-1] = authName;
}

/*
 * Register the DisplayClass string
 */

ARRAY8	DisplayClass;

XdmcpRegisterDisplayClass (name, length)
    char    *name;
    int	    length;
{
    int	    i;

    XdmcpDisposeARRAY8 (&DisplayClass);
    if (!XdmcpAllocARRAY8 (&DisplayClass, length))
	return;
    for (i = 0; i < length; i++)
	DisplayClass.data[i] = (CARD8) name[i];
}

/*
 * Register the Manufacturer display ID
 */

ARRAY8 ManufacturerDisplayID;

XdmcpRegisterManufacturerDisplayID (name, length)
    char    *name;
    int	    length;
{
    int	    i;

    XdmcpDisposeARRAY8 (&ManufacturerDisplayID);
    if (!XdmcpAllocARRAY8 (&ManufacturerDisplayID, length))
	return;
    for (i = 0; i < length; i++)
	ManufacturerDisplayID.data[i] = (CARD8) name[i];
}

/* 
 * initialize XDMCP; create the socket, compute the display
 * number, set up the state machine
 */

void 
XdmcpInit()
{
    state = XDM_INIT_STATE;
#ifdef HASXDMAUTH
    if (xdmAuthCookie)
	XdmAuthenticationInit (xdmAuthCookie, strlen (xdmAuthCookie));
#endif
    if (state != XDM_OFF)
    {
	XdmcpRegisterAuthorizations();
	XdmcpRegisterDisplayClass (defaultDisplayClass, strlen (defaultDisplayClass));
	AccessUsingXdmcp();
#ifndef _MINIX
	RegisterBlockAndWakeupHandlers (XdmcpBlockHandler, XdmcpWakeupHandler, 
			(pointer) 0);
#else /* _MINIX */
	RegisterBlockAndWakeupHandlers (MNX_XdmcpBlockHandler, 
					MNX_XdmcpWakeupHandler, (pointer) 0);
#endif /* !_MINIX */
    	timeOutRtx = 0;
    	DisplayNumber = (CARD16) atoi(display);
    	get_xdmcp_sock();
    	send_packet();
    }
}

void
XdmcpReset ()
{
    state = XDM_INIT_STATE;
    if (state != XDM_OFF)
    {
#ifndef _MINIX
	RegisterBlockAndWakeupHandlers (XdmcpBlockHandler, XdmcpWakeupHandler, 
			(pointer) 0);
#else /* _MINIX */
	RegisterBlockAndWakeupHandlers (MNX_XdmcpBlockHandler, 
					MNX_XdmcpWakeupHandler, (pointer) 0);
#endif /* !_MINIX */
    	timeOutRtx = 0;
    	send_packet();
    }
}

/*
 * Called whenever a new connection is created; notices the
 * first connection and saves it to terminate the session
 * when it is closed
 */

void
XdmcpOpenDisplay(sock)
    int	sock;
{
    if (state != XDM_AWAIT_MANAGE_RESPONSE)
	return;
    state = XDM_RUN_SESSION;
    sessionSocket = sock;
}

void 
XdmcpCloseDisplay(sock)
    int	sock;
{
    if ((state != XDM_RUN_SESSION && state != XDM_AWAIT_ALIVE_RESPONSE)
	|| sessionSocket != sock)
	    return;
    state = XDM_INIT_STATE;
    if (OneSession)
	dispatchException |= DE_TERMINATE;
    else
	dispatchException |= DE_RESET;
    isItTimeToYield = TRUE;
}

/*
 * called before going to sleep, this routine
 * may modify the timeout value about to be sent
 * to select; in this way XDMCP can do appropriate things
 * dynamically while starting up
 */

#ifndef _MINIX
/*ARGSUSED*/
static void
XdmcpBlockHandler(data, wt, LastSelectMask)
    pointer	    data;   /* unused */
    struct timeval  **wt;
    long	    *LastSelectMask;
{
    long millisToGo, wtMillis;
    static struct timeval waittime;

    if (state == XDM_OFF)
	return;
    *LastSelectMask |= (1 << xdmcpSocket);
    if (timeOutTime == 0)
	return;
    millisToGo = timeOutTime - GetTimeInMillis() + 1;
    if (millisToGo < 0)
	millisToGo = 0;
    if (*wt == NULL)
    {
	waittime.tv_sec = (millisToGo) / 1000;
	waittime.tv_usec = 1000 * (millisToGo % 1000);
	*wt = &waittime;
    }
    else
    {
	wtMillis = (*wt)->tv_sec * 1000 + (*wt)->tv_usec / 1000;
	if (millisToGo < wtMillis)
 	{
	    (*wt)->tv_sec = (millisToGo) / 1000;
	    (*wt)->tv_usec = 1000 * (millisToGo % 1000);
	}
    }
}
#else /* _MINIX */

static int read_inprogress= FALSE;
static int read_completed= FALSE;
static int read_result;
static char read_buffer[XDM_MAX_MSGLEN+sizeof(udp_io_hdr_t)];
static size_t read_buffer_size= sizeof(read_buffer);
/*ARGSUSED*/
static void
MNX_XdmcpBlockHandler(data, wt, fdset)
    pointer	    data;   /* unused */
    struct timeval  wt[2];
    struct asio_fd_set *fdset;
{
    long millisToGo, wtMillis;
    struct timeval waittime;
    int r;

    if (state == XDM_OFF)
	return;
    if (!read_inprogress)
    {
    	r= read(xdmcpSocket, read_buffer, read_buffer_size);
    	if (r == -1 && errno == EINPROGRESS)
    	{
    		read_inprogress= TRUE;
    		read_completed= FALSE;
    		ASIO_FD_SET(xdmcpSocket, ASIO_READ,  fdset);
    	}
    	else if (r > 0)
    	{
    		read_completed= TRUE;
    		read_result= r;
    		wt[0]= wt[1];	/* Don't wait */
    		return;
    	}
    	else
    	{
    		ErrorF("(warning got read error (%d, %s)\n", r, 
		    strerror(errno));
    		read_completed= FALSE;
    	}
    }
    else
    	ASIO_FD_SET(xdmcpSocket, ASIO_READ,  fdset);
    millisToGo = timeOutTime - GetTimeInMillis() + 1;
    if (millisToGo < 0)
	millisToGo = 0;
    waittime= wt[1];
    waittime.tv_sec += millisToGo / 1000;
    waittime.tv_usec += (millisToGo % 1000) * 1000;
    if (waittime.tv_usec >= 1000000)
    {
    	waittime.tv_usec -= 1000000;
    	waittime.tv_sec++;
    }
    if (waittime.tv_sec < wt[0].tv_sec || 
    	(waittime.tv_sec == wt[0].tv_sec && waittime.tv_usec < wt[0].tv_usec))
    {
    	wt[0]= waittime;
    }
}
#endif /* !_MINIX */

/*
 * called after select returns; this routine will
 * recognise when XDMCP packets await and
 * process them appropriately
 */

#ifndef _MINIX
/*ARGSUSED*/
static void
XdmcpWakeupHandler(data, i, LastSelectMask)
    pointer data;   /* unused */
    int	    i;
    long    *LastSelectMask;
{
    long    devicesReadable[mskcnt];

    if (state == XDM_OFF)
	return;
    if (i > 0)
    {
	if (GETBIT(LastSelectMask, xdmcpSocket))
	{
	    receive_packet();
	    BITCLEAR(LastSelectMask, xdmcpSocket);
	} 
	MASKANDSETBITS(devicesReadable, LastSelectMask, EnabledDevices);
	if (ANYSET(devicesReadable))
	{
	    if (state == XDM_AWAIT_USER_INPUT)
		restart();
	    else if (state == XDM_RUN_SESSION)
		keepaliveDormancy = defaultKeepaliveDormancy;
	}
	if (ANYSET(AllClients) && state == XDM_RUN_SESSION)
	    timeOutTime = GetTimeInMillis() +  keepaliveDormancy * 1000;
    }
    else if (timeOutTime && GetTimeInMillis() >= timeOutTime)
    {
    	if (state == XDM_RUN_SESSION)
    	{
	    state = XDM_KEEPALIVE;
	    send_packet();
    	}
    	else
	    timeout();
    }
}
#else /* _MINIX */
static void
MNX_XdmcpWakeupHandler(data, i, fw)
    pointer data;   /* unused */
    int	    i;
    struct fwait    *fw;
{
    if (state == XDM_OFF)
	return;
    if (fw && fw->fw_fd == xdmcpSocket)
    {
    	assert(fw->fw_operation == ASIO_READ);
    	fw->fw_fd= -1;
    	assert(read_inprogress);
    	read_inprogress= FALSE;
    	assert(!read_completed);
    	if (fw->fw_result <= 0)
    	{
    		ErrorF("(warning got read error (%d, %s)\n", fw->fw_fd, 
		    strerror(fw->fw_errno));
    	}
    	else
    	{
    		read_result= fw->fw_result;
    		read_completed= TRUE;
    	}
    }
    if (read_completed)
    {
    	read_completed= FALSE;
    	MNX_receive_packet(read_buffer, read_result);
    }
    if (fw != NULL)
    {
        extern asio_fd_set_t ClientFdSet;	/* HACK XXX */
	if (state == XDM_AWAIT_USER_INPUT)
	    restart();
	else if (state == XDM_RUN_SESSION)
	    keepaliveDormancy = defaultKeepaliveDormancy;
	if (memchr((char *)&ClientFdSet, '\0', sizeof(ClientFdSet)) != NULL && 
		state == XDM_RUN_SESSION)
	{
	    timeOutTime = GetTimeInMillis() +  keepaliveDormancy * 1000;
	}
    }
    else if (timeOutTime && GetTimeInMillis() >= timeOutTime)
    {
    	if (state == XDM_RUN_SESSION)
    	{
	    state = XDM_KEEPALIVE;
	    send_packet();
    	}
    	else
	    timeout();
    }
}
#endif /* !_MINIX */

/*
 * This routine should be called from the routine that drives the
 * user's host menu when the user selects a host
 */

XdmcpSelectHost(host_sockaddr, host_len, AuthenticationName)
    struct sockaddr_in	*host_sockaddr;
    int			host_len;
    ARRAY8Ptr		AuthenticationName;
{
    state = XDM_START_CONNECTION;
    bcopy(host_sockaddr, &req_sockaddr, host_len);
    req_socklen = host_len;
    XdmcpSetAuthentication (AuthenticationName);
    send_packet();
}

/*
 * !!! this routine should be replaced by a routine that adds
 * the host to the user's host menu. the current version just
 * selects the first host to respond with willing message.
 */

/*ARGSUSED*/
XdmcpAddHost(from, fromlen, AuthenticationName, hostname, status)
    struct sockaddr_in  *from;
    ARRAY8Ptr		AuthenticationName, hostname, status;
{
    XdmcpSelectHost(from, fromlen, AuthenticationName);
}

/*
 * A message is queued on the socket; read it and
 * do the appropriate thing
 */

ARRAY8	UnwillingMessage = { (CARD8) 14, (CARD8 *) "Host unwilling" };

#ifndef _MINIX
static
receive_packet()
{
    struct sockaddr_in from;
    int fromlen = sizeof(struct sockaddr_in);
    XdmcpHeader	header;

    /* read message off socket */
    if (!XdmcpFill (xdmcpSocket, &buffer, (struct sockaddr *) &from, &fromlen))
	return;

    /* reset retransmission backoff */
    timeOutRtx = 0;

    if (!XdmcpReadHeader (&buffer, &header))
	return;

    if (header.version != XDM_PROTOCOL_VERSION)
	return;

    switch (header.opcode) {
    case WILLING:
	recv_willing_msg(&from, fromlen, header.length);
	break;
    case UNWILLING:
	XdmcpFatal("Manager unwilling", &UnwillingMessage);
	break;
    case ACCEPT:
	recv_accept_msg(header.length);
	break;
    case DECLINE:
	recv_decline_msg(header.length);
	break;
    case REFUSE:
	recv_refuse_msg(header.length);
	break;
    case FAILED:
	recv_failed_msg(header.length);
	break;
    case ALIVE:
	recv_alive_msg(header.length);
	break;
    }
}
#else /* _MINIX */
static
MNX_receive_packet(buf, size)
char *buf;
int size;
{
    struct sockaddr_in from;
    int fromlen = sizeof(struct sockaddr_in);
    XdmcpHeader	header;

    /* read message off socket */
    if (!MNX_XdmcpFill (xdmcpSocket, &buffer, (struct sockaddr *) &from, 
    	&fromlen, buf, size))
	return;

    /* reset retransmission backoff */
    timeOutRtx = 0;

    if (!XdmcpReadHeader (&buffer, &header))
	return;

    if (header.version != XDM_PROTOCOL_VERSION)
	return;

    switch (header.opcode) {
    case WILLING:
	recv_willing_msg(&from, fromlen, header.length);
	break;
    case UNWILLING:
	XdmcpFatal("Manager unwilling", &UnwillingMessage);
	break;
    case ACCEPT:
	recv_accept_msg(header.length);
	break;
    case DECLINE:
	recv_decline_msg(header.length);
	break;
    case REFUSE:
	recv_refuse_msg(header.length);
	break;
    case FAILED:
	recv_failed_msg(header.length);
	break;
    case ALIVE:
	recv_alive_msg(header.length);
	break;
    }
}
#endif /* !_MINIX */

/*
 * send the appropriate message given the current state
 */

static
send_packet()
{
    int rtx;

    switch (state) {
    case XDM_QUERY:
    case XDM_BROADCAST:
    case XDM_INDIRECT:
	send_query_msg();
	break;
    case XDM_START_CONNECTION:
	send_request_msg();
	break;
    case XDM_MANAGE:
	send_manage_msg();
	break;
    case XDM_KEEPALIVE:
	send_keepalive_msg();
	break;
    }
    rtx = (XDM_MIN_RTX << timeOutRtx);
    if (rtx > XDM_MAX_RTX)
	rtx = XDM_MAX_RTX;
    timeOutTime = GetTimeInMillis() + rtx * 1000;
}

/*
 * The session is declared dead for some reason; too many
 * timeouts, or Keepalive failure.
 */

XdmcpDeadSession (reason)
    char *reason;
{
    ErrorF ("XDM: %s, declaring session dead\n", reason);
    state = XDM_INIT_STATE;
    isItTimeToYield = TRUE;
    dispatchException |= DE_RESET;
    timeOutTime = 0;
    timeOutRtx = 0;
    send_packet();
}

/*
 * Timeout waiting for an XDMCP response.
 */

static 
timeout()
{
    timeOutRtx++;
    if (state == XDM_AWAIT_ALIVE_RESPONSE && timeOutRtx >= XDM_KA_RTX_LIMIT )
    {
	XdmcpDeadSession ("too many keepalive retransmissions");
	return;
    }
    else if (timeOutRtx >= XDM_RTX_LIMIT)
    {
	ErrorF("XDM: too many retransmissions\n");
	state = XDM_AWAIT_USER_INPUT;
	timeOutTime = 0;
	timeOutRtx = 0;
	return;
    }

    switch (state) {
    case XDM_COLLECT_QUERY:
	state = XDM_QUERY;
	break;
    case XDM_COLLECT_BROADCAST_QUERY:
	state = XDM_BROADCAST;
	break;
    case XDM_COLLECT_INDIRECT_QUERY:
	state = XDM_INDIRECT;
	break;
    case XDM_AWAIT_REQUEST_RESPONSE:
	state = XDM_START_CONNECTION;
	break;
    case XDM_AWAIT_MANAGE_RESPONSE:
	state = XDM_MANAGE;
	break;
    case XDM_AWAIT_ALIVE_RESPONSE:
	state = XDM_KEEPALIVE;
	break;
    }
    send_packet();
}

static
restart()
{
    state = XDM_INIT_STATE;
    timeOutRtx = 0;
    send_packet();
}

XdmcpCheckAuthentication (Name, Data, packet_type)
    ARRAY8Ptr	Name, Data;
    int	packet_type;
{
    return (XdmcpARRAY8Equal (Name, AuthenticationName) &&
	    (AuthenticationName->length == 0 ||
	     (*AuthenticationFuncs->Validator) (AuthenticationData, Data, packet_type)));
}

XdmcpAddAuthorization (name, data)
    ARRAY8Ptr	name, data;
{
    Bool    (*AddAuth)(), AddAuthorization();

    if (AuthenticationFuncs && AuthenticationFuncs->AddAuth)
	AddAuth = AuthenticationFuncs->AddAuth;
    else
	AddAuth = AddAuthorization;
    return (*AddAuth) ((unsigned short)name->length,
		       (char *)name->data,
		       (unsigned short)data->length,
		       (char *)data->data);
}

/*
 * from here to the end of this file are routines private
 * to the state machine.
 */

static
get_xdmcp_sock()
{
#ifndef _MINIX
    int soopts = 1;

    if ((xdmcpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
#else
    char *udp_device;
    int r, s_errno;
    nwio_udpopt_t udpopt;

    udp_device= getenv("UDP_DEVICE");
    if (udp_device == NULL)
    	udp_device= UDP_DEVICE;
    xdmcpSocket= open(udp_device, O_RDWR);
    if (xdmcpSocket != -1)
    {
    	udpopt.nwuo_flags= NWUO_COPY | NWUO_LP_SEL | NWUO_EN_LOC | 
    		NWUO_DI_BROAD | NWUO_RP_ANY | NWUO_RA_ANY | NWUO_RWDATALL |
    		NWUO_DI_IPOPT;
    	r= ioctl(xdmcpSocket, NWIOSUDPOPT, &udpopt);
    	if (r == -1)
    	{
    		s_errno= errno;
    		close(xdmcpSocket);
    		xdmcpSocket= -1;
    		errno= s_errno;
    	}
    	ioctl(xdmcpSocket, NWIOGUDPOPT, &udpopt);
    	ErrorF("0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", 
    		udpopt.nwuo_flags,
    		udpopt.nwuo_locport,
    		udpopt.nwuo_remport,
    		udpopt.nwuo_locaddr,
    		udpopt.nwuo_remaddr);
    }
    if (xdmcpSocket != -1)
    {
	fcntl(xdmcpSocket, F_SETFD, fcntl(xdmcpSocket, F_GETFD) | 
    								FD_ASYNCHIO);
    }
    if (xdmcpSocket == -1)
#endif
	XdmcpWarning("UDP socket creation failed");
#ifdef SO_BROADCAST
    else if (setsockopt(xdmcpSocket, SOL_SOCKET, SO_BROADCAST, &soopts,
	sizeof(soopts)) < 0)
	    XdmcpWarning("UDP set broadcast socket-option failed");
#endif /* SO_BROADCAST */
}

static
send_query_msg()
{
    XdmcpHeader	header;
    Bool	broadcast = FALSE;
    int		i;

    header.version = XDM_PROTOCOL_VERSION;
    switch(state){
    case XDM_QUERY:
	header.opcode = (CARD16) QUERY; 
	state = XDM_COLLECT_QUERY;
	break;
    case XDM_BROADCAST:
	header.opcode = (CARD16) BROADCAST_QUERY;
	state = XDM_COLLECT_BROADCAST_QUERY;
	broadcast = TRUE;
	break;
    case XDM_INDIRECT:
	header.opcode = (CARD16) INDIRECT_QUERY;
	state = XDM_COLLECT_INDIRECT_QUERY;
	break;
    }
    header.length = 1;
    for (i = 0; i < AuthenticationNames.length; i++)
	header.length += 2 + AuthenticationNames.data[i].length;

    XdmcpWriteHeader (&buffer, &header);
    XdmcpWriteARRAYofARRAY8 (&buffer, &AuthenticationNames);
    if (broadcast)
    {
	int i;

	for (i = 0; i < NumBroadcastAddresses; i++)
	    XdmcpFlush (xdmcpSocket, &buffer, &BroadcastAddresses[i],
			sizeof (struct sockaddr_in));
    }
    else
    {
	XdmcpFlush (xdmcpSocket, &buffer, &ManagerAddress,
		    sizeof (ManagerAddress));
    }
}

static
recv_willing_msg(from, fromlen, length)
    struct sockaddr_in	*from;
    int			fromlen;
    unsigned		length;
{
    ARRAY8	authenticationName;
    ARRAY8	hostname;
    ARRAY8	status;

    authenticationName.data = 0;
    hostname.data = 0;
    status.data = 0;
    if (XdmcpReadARRAY8 (&buffer, &authenticationName) &&
	XdmcpReadARRAY8 (&buffer, &hostname) &&
	XdmcpReadARRAY8 (&buffer, &status))
    {
    	if (length == 6 + authenticationName.length +
		      hostname.length + status.length)
    	{
	    switch (state)
	    {
	    case XDM_COLLECT_QUERY:
	    	XdmcpSelectHost(from, fromlen, &authenticationName);
	    	break;
	    case XDM_COLLECT_BROADCAST_QUERY:
	    case XDM_COLLECT_INDIRECT_QUERY:
	    	XdmcpAddHost(from, fromlen, &authenticationName, &hostname, &status);
	    	break;
    	    }
    	}
    }
    XdmcpDisposeARRAY8 (&authenticationName);
    XdmcpDisposeARRAY8 (&hostname);
    XdmcpDisposeARRAY8 (&status);
}

static
send_request_msg()
{
    XdmcpHeader	    header;
    int		    length;
    int		    i;
    ARRAY8	    authenticationData;

    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) REQUEST;

    length = 2;					    /* display number */
    length += 1 + 2 * ConnectionTypes.length;	    /* connection types */
    length += 1;				    /* connection addresses */
    for (i = 0; i < ConnectionAddresses.length; i++)
	length += 2 + ConnectionAddresses.data[i].length;
    authenticationData.length = 0;
    authenticationData.data = 0;
    if (AuthenticationFuncs)
    {
	(*AuthenticationFuncs->Generator) (AuthenticationData,
					   &authenticationData,
 					   REQUEST);
    }
    length += 2 + AuthenticationName->length;	    /* authentication name */
    length += 2 + authenticationData.length;	    /* authentication data */
    length += 1;				    /* authorization names */
    for (i = 0; i < AuthorizationNames.length; i++)
	length += 2 + AuthorizationNames.data[i].length;
    length += 2 + ManufacturerDisplayID.length;	    /* display ID */
    header.length = length;

    if (!XdmcpWriteHeader (&buffer, &header))
    {
	XdmcpDisposeARRAY8 (&authenticationData);
	return;
    }
    XdmcpWriteCARD16 (&buffer, DisplayNumber);
    XdmcpWriteARRAY16 (&buffer, &ConnectionTypes);
    XdmcpWriteARRAYofARRAY8 (&buffer, &ConnectionAddresses);

    XdmcpWriteARRAY8 (&buffer, AuthenticationName);
    XdmcpWriteARRAY8 (&buffer, &authenticationData);
    XdmcpDisposeARRAY8 (&authenticationData);
    XdmcpWriteARRAYofARRAY8 (&buffer, &AuthorizationNames);
    XdmcpWriteARRAY8 (&buffer, &ManufacturerDisplayID);
    if (XdmcpFlush (xdmcpSocket, &buffer, &req_sockaddr, req_socklen))
	state = XDM_AWAIT_REQUEST_RESPONSE;
}

static
recv_accept_msg(length)
    unsigned		length;
{
    CARD32  AcceptSessionID;
    ARRAY8  AcceptAuthenticationName, AcceptAuthenticationData;
    ARRAY8  AcceptAuthorizationName, AcceptAuthorizationData;

    if (state != XDM_AWAIT_REQUEST_RESPONSE)
	return;
    AcceptAuthenticationName.data = 0;
    AcceptAuthenticationData.data = 0;
    AcceptAuthorizationName.data = 0;
    AcceptAuthorizationData.data = 0;
    if (XdmcpReadCARD32 (&buffer, &AcceptSessionID) &&
	XdmcpReadARRAY8 (&buffer, &AcceptAuthenticationName) &&
	XdmcpReadARRAY8 (&buffer, &AcceptAuthenticationData) &&
	XdmcpReadARRAY8 (&buffer, &AcceptAuthorizationName) &&
	XdmcpReadARRAY8 (&buffer, &AcceptAuthorizationData))
    {
    	if (length == 12 + AcceptAuthenticationName.length +
		      	   AcceptAuthenticationData.length +
		      	   AcceptAuthorizationName.length +
 		      	   AcceptAuthorizationData.length)
    	{
	    if (!XdmcpCheckAuthentication (&AcceptAuthenticationName,
				      &AcceptAuthenticationData, ACCEPT))
	    {
		XdmcpFatal ("Authentication Failure", &AcceptAuthenticationName);
	    }
	    /* permit access control manipulations from this host */
	    AugmentSelf (&req_sockaddr, req_socklen);
	    /* if the authorization specified in the packet fails
	     * to be acceptable, enable the local addresses
	     */
	    if (!XdmcpAddAuthorization (&AcceptAuthorizationName,
					&AcceptAuthorizationData))
	    {
		AddLocalHosts ();
	    }
	    SessionID = AcceptSessionID;
    	    state = XDM_MANAGE;
    	    send_packet();
    	}
    }
    XdmcpDisposeARRAY8 (&AcceptAuthenticationName);
    XdmcpDisposeARRAY8 (&AcceptAuthenticationData);
    XdmcpDisposeARRAY8 (&AcceptAuthorizationName);
    XdmcpDisposeARRAY8 (&AcceptAuthorizationData);
}

static
recv_decline_msg(length)
    unsigned		length;
{
    ARRAY8  Status, DeclineAuthenticationName, DeclineAuthenticationData;

    Status.data = 0;
    DeclineAuthenticationName.data = 0;
    DeclineAuthenticationData.data = 0;
    if (XdmcpReadARRAY8 (&buffer, &Status) &&
	XdmcpReadARRAY8 (&buffer, &DeclineAuthenticationName) &&
	XdmcpReadARRAY8 (&buffer, &DeclineAuthenticationData))
    {
    	if (length == 6 + Status.length +
		      	  DeclineAuthenticationName.length +
 		      	  DeclineAuthenticationData.length &&
	    XdmcpCheckAuthentication (&DeclineAuthenticationName,
				      &DeclineAuthenticationData, DECLINE))
    	{
	    XdmcpFatal ("Session declined", &Status);
    	}
    }
    XdmcpDisposeARRAY8 (&Status);
    XdmcpDisposeARRAY8 (&DeclineAuthenticationName);
    XdmcpDisposeARRAY8 (&DeclineAuthenticationData);
}

static
send_manage_msg()
{
    XdmcpHeader	header;

    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) MANAGE;
    header.length = 8 + DisplayClass.length;

    if (!XdmcpWriteHeader (&buffer, &header))
	return;
    XdmcpWriteCARD32 (&buffer, SessionID);
    XdmcpWriteCARD16 (&buffer, DisplayNumber);
    XdmcpWriteARRAY8 (&buffer, &DisplayClass);
    state = XDM_AWAIT_MANAGE_RESPONSE;
    XdmcpFlush (xdmcpSocket, &buffer, &req_sockaddr, req_socklen);
}

static
recv_refuse_msg(length)
    unsigned		length;
{
    CARD32  RefusedSessionID;

    if (state != XDM_AWAIT_MANAGE_RESPONSE)
	return;
    if (length != 4)
	return;
    if (XdmcpReadCARD32 (&buffer, &RefusedSessionID))
    {
	if (RefusedSessionID == SessionID)
	{
    	    state = XDM_START_CONNECTION;
    	    send_packet();
	}
    }
}

static
recv_failed_msg(length)
    unsigned		length;
{
    CARD32  FailedSessionID;
    ARRAY8  Status;

    if (state != XDM_AWAIT_MANAGE_RESPONSE)
	return;
    Status.data = 0;
    if (XdmcpReadCARD32 (&buffer, &FailedSessionID) &&
	XdmcpReadARRAY8 (&buffer, &Status))
    {
    	if (length == 6 + Status.length &&
	    SessionID == FailedSessionID)
	{
	    XdmcpFatal ("Session failed", &Status);
	}
    }
    XdmcpDisposeARRAY8 (&Status);
}

static
send_keepalive_msg()
{
    XdmcpHeader	header;

    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) KEEPALIVE;
    header.length = 6;

    XdmcpWriteHeader (&buffer, &header);
    XdmcpWriteCARD16 (&buffer, DisplayNumber);
    XdmcpWriteCARD32 (&buffer, SessionID);

    state = XDM_AWAIT_ALIVE_RESPONSE;
    XdmcpFlush (xdmcpSocket, &buffer, &req_sockaddr, req_socklen);
}

static
recv_alive_msg (length)
    unsigned		length;
{
    CARD8   SessionRunning;
    CARD32  AliveSessionID;
    int	    dormancy;

    if (state != XDM_AWAIT_ALIVE_RESPONSE)
	return;
    if (length != 5)
	return;
    if (XdmcpReadCARD8 (&buffer, &SessionRunning) &&
	XdmcpReadCARD32 (&buffer, &AliveSessionID))
    {
    	if (SessionRunning && AliveSessionID == SessionID)
    	{
	    /* backoff dormancy period */
	    state = XDM_RUN_SESSION;
	    if ((GetTimeInMillis() - lastDeviceEventTime.milliseconds) >
		keepaliveDormancy * 1000)
	    {
		keepaliveDormancy <<= 1;
		if (keepaliveDormancy > XDM_MAX_DORMANCY)
		    keepaliveDormancy = XDM_MAX_DORMANCY;
	    }
	    timeOutTime = GetTimeInMillis() + keepaliveDormancy * 1000;
    	}
	else
    	{
	    XdmcpDeadSession ("Alive respose indicates session dead");
    	}
    }
}

static 
XdmcpFatal (type, status)
    char	*type;
    ARRAY8Ptr	status;
{
    extern void AbortDDX();

    ErrorF ("XDMCP fatal error: %s %*.*s\n", type,
	   status->length, status->length, status->data);
    AbortDDX ();
    exit (1);
}

static 
XdmcpWarning(str)
    char *str;
{
    ErrorF("XDMCP warning: %s\n", str);
}

static
get_manager_by_name(argc, argv, i)
    int	    argc, i;
    char    **argv;
{
    struct hostent *hep;

    if (i == argc)
    {
	ErrorF("Xserver: missing host name in command line\n");
	exit(1);
    }
    if (!(hep = gethostbyname(argv[i])))
    {
	ErrorF("Xserver: unknown host: %s\n", argv[i]);
	exit(1);
    }
#ifndef _MINIX
    if (hep->h_length == sizeof (struct in_addr))
#else
    if (hep->h_length == sizeof (ipaddr_t))
#endif
    {
	bcopy(hep->h_addr, &ManagerAddress.sin_addr, hep->h_length);
	ManagerAddress.sin_family = AF_INET;
	ManagerAddress.sin_port = htons (xdm_udp_port);
    }
    else
    {
	ErrorF ("Xserver: host on strange network %s\n", argv[i]);
	exit (1);
    }
}
#endif /* XDMCP */
