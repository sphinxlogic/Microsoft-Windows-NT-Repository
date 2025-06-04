////////////////////////////////////////////////////////////////////////
//
// Description
//
//	 Simple socket class.
//
// Note
//
//	Framing should be used when connecting to a record-oriented server
//	such as a POP-Mail handler (select LF as end-of-message character).
//
// Edit history
//
//	April 26, 1995	Written by Andrew Davison.
//
//  November 1, 1995 Modified by AD to remove framing character placed in
//  buffer on write (it is now up to the application to include it).
//
//  November 2, 1995 Modified to perform streams IO on a socket.
//
//  December 1995 Modified to be signal aware for Unix.
//
//  September 1996 Mod to make DLE escaping optional and non-default.
//
//  November 1996 Mod to remove DLE escaping and WriteMsg() method. Now
//  always need to include eom character in Write().
//
////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined _Windows || defined wx_msw
#include <winsock.h>
#else
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#ifdef _sgi
#include <bstring.h>
#endif
#ifdef sparc
#include <sys/filio.h>
#endif

#include "simsock.h"
#include "listof.h"
#pragma hdrstop

#ifdef _Windows
#define close closesocket
#define ioctl ioctlsocket
#define errno WSAGetLastError()
#ifdef EWOULDBLOCK
#undef EWOULDBLOCK
#endif
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINTR WSAEINTR
#endif

const DLE = 0x10;

#ifdef _Windows
// This is an MS TCP/IP routine and is not needed here. Some WinSock
// implementations (such as PC-NFS) will require you to include this
// or a similar routine (see appendix in WinSock doc or help file).

#if defined( NEED_WSAFDIsSet ) || defined( _MSC_VER )
int PASCAL FAR __WSAFDIsSet(SOCKET fd, fd_set FAR *set)
{
	int i = set->fd_count;

	while (i--)
	{
		if (set->fd_array[i] == fd)
			return 1;
	}

	return 0;
}
#endif
#endif

static int servername(const char* service)
{
	struct servent* servent;

	if (!service || !strlen(service))
	{
		//printf("No service\n");
		return -1;
	}

	if (!isdigit(*service))
	{
		if ((servent = getservbyname((char*)service, "tcp")) == 0)
		{
			//printf("no serverbyname(%s) err %d\n", service, errno);
			return -1;
		}
	}
	else
	{
		if ((servent = getservbyport(atoi(service), "tcp")) == 0)
		{
			return htons(atoi(service));
		}
	}

	return servent->s_port;
}

static long hostname(const char* host)
{
	struct hostent* hostent;
	struct in_addr* addr;

	if (!host || !strlen(host))
	{
		//printf("no host name\n");
		return -1;
	}

	if (!isdigit(*host))
	{
		if ((hostent = gethostbyname((char*)host)) == 0)
		{
			//printf("no hostbyname(%s) err %d\n", host, errno);
			return -1;
		}
	}
	else
	{
		unsigned long addr = inet_addr((char*)host);

		// There seems to be some kind of problem with some WIN95
		// machines, so bypass this tuff for now...

		//if ((hostent = gethostbyaddr((char*)&addr, 4, PF_INET)) == 0)
		{
			//printf("no hostbyaddr(%s) err %d\n", host, errno);
			//return -1;
			return addr;
		}
	}

	addr = (struct in_addr *) *(hostent->h_addr_list);

	return addr[0].s_addr;
}

int SimSock::cnt = 0;

SimSock::SimSock(unsigned _flags, char _eom, SockType _type) :
	flags(_flags), type(_type), connected(0), fd(-1), eom(_eom)
{
#ifdef _Windows
	WSADATA wsaData;

	if (!cnt)
	{
		if (WSAStartup((1 << 8) | 1, &wsaData))
		{
			//perror("WSAStartup");
			//printf("errno = %d\n", errno);
		}
	}
#else
	if (!cnt)
	{
		struct sigaction act;

#ifdef __sgi
		sigset_t mask = {0};	// Ignore compiler warning!
		act.sa_mask = mask;
#else
#ifdef _POSIX90
         sigemptyset(&(act.sa_mask));
#else
	sigset_t mask = {0};	// Ignore compiler warning!
         act.sa_mask = mask;
#endif
#endif

		act.sa_handler = (void (*)(... )) SIG_IGN;
		act.sa_flags = 0;
		sigaction(SIGPIPE, &act, 0);
	}
#endif

	cnt++;
}

SimSock::~SimSock()
{
	Close();
	cnt--;

#ifdef _Windows
	if (!cnt)
		WSACleanup();
#endif
}

int SimSock::Close()
{
	if (fd != -1)
	{
		shutdown(fd, 2);
		close(fd);
		fd = -1;
		connected = 0;
	}

	return 1;
}

int SimSock::Read(char* buffer, int nbytes)
{
	int len;

loop:

	if ((len = recv(fd, buffer, nbytes, 0)) < 0)
	{
		if (errno == EINTR)
			goto loop;
		else if (errno == EWOULDBLOCK)
			return 0;
		else
			return -errno;
	}

	return len;
}

int SimSock::Peek(char* buffer, int nbytes)
{
	int len;

loop:

	if ((len = recv(fd, buffer, nbytes, MSG_PEEK)) < 0)
	{
		if (errno == EINTR)
			goto loop;
		else if (errno == EWOULDBLOCK)
			return 0;
		else
			return -errno;
	}

	return len;
}

int SimSock::Write(const char* buffer, int nbytes)
{
	const char* ptr = buffer;

	int len;
	int bytes_written = 0;

	do_write:

	if ((len = send(fd, ptr, nbytes, 0)) < 0)
	{
		if ((errno != EWOULDBLOCK) && (errno != EINTR))
			return -errno;
	}

	bytes_written += len;

	if (len != nbytes)
	{
		ptr += len;
		nbytes -= len;
		WaitForWrite(-1);		// Wait for socket to be writable
		goto do_write;
	}

	return bytes_written;
}

int SimSock::ReadMsg(char* buffer, int nbytes)
{
	char* dst = buffer;

	while (nbytes-- > 0)
	{
		int len;
		char ch;

		if ((len = recv(fd, &ch, 1, 0)) < 0)
		{
			if (errno == EINTR)
			{
				nbytes++;
				continue;
			}
			else if (errno == EWOULDBLOCK)
			{
				if (WaitForRead() < 0)
					return -errno;

				nbytes++;
				continue;
			}

			return -errno;
		}

		if (!len)
			return 0;

		*dst++ = ch;

		if (ch == eom)
			break;
	}

	return (int)(dst - buffer);
}

int SimSock::_Wait(long seconds, long microseconds, int type)
{
	if (!connected) return 0;

	struct timeval timeval = {seconds, microseconds};
	fd_set read_fds;
	fd_set error_fds;
	fd_set write_fds;

	FD_ZERO(&read_fds);
	FD_SET(fd, &read_fds);

	FD_ZERO(&error_fds);
	FD_SET(fd, &error_fds);

	FD_ZERO(&write_fds);
	FD_SET(fd, &write_fds);

	return select(FD_SETSIZE,
				(type&1?&read_fds:0),
					(type&2?&write_fds:0),
						(type&4?&error_fds:0),
							(seconds < 0 ? 0 : &timeval));
}

int SimSock::Wait(long seconds, long microseconds)
{
	return _Wait(seconds, microseconds, 5);
}

int SimSock::WaitForRead(long seconds, long microseconds)
{
	return _Wait(seconds, microseconds, 1);
}

int SimSock::WaitForWrite(long seconds, long microseconds)
{
	return _Wait(seconds, microseconds, 2);
}

int SimSock::WaitForError(long seconds, long microseconds)
{
	return _Wait(seconds, microseconds, 4);
}

const char* SimSock::LocalHost()
{
	static char buf[256];
	if (gethostname(buf, sizeof(buf)) < 0)
		return "localhost";
	 else
		return buf;
}

SockServer::SockServer(const char* service, unsigned flags, char eom) :
	SimSock(flags, eom, SOCK_SERVER)
{
	fd = socket(PF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		//perror("socket");
		//printf("errno = %d\n", errno);
		return;
	}

	int flag = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));

	struct sockaddr_in myaddr;

	myaddr.sin_family = PF_INET;
	myaddr.sin_port = port = servername(service);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
	{
		//perror("bind");
		//printf("errno = %d\n", errno);
		return;
	}

	if (listen(fd, 5) < 0)
	{
		//perror("listen");
		//printf("errno = %d\n", errno);
                fd = -1;
		return;
	}

#if defined BSD || defined linux
	signal(SIGCHLD, SIG_IGN);			// BSD
#elif !defined _Windows
	sigset(SIGCLD, SIG_IGN);			// SVR4
#endif
}

SimSock* SockServer::Accept()
{
	int fd2;

	if ((fd2 = accept(fd, 0, 0)) < 0)
	{
		//perror("accept");
		//printf("errno = %d\n", errno);
		return 0;
	}

#ifndef _Windows
	// Windows can't fork() and under Unix it is optional...

	if (flags & SimSock::FORK)
	{
		//	Create a child process which will return with the connection
		//	while the parent will continue to wait for more.

		if (!fork())
			close(fd);				/* Child process */
		else
		{
			close(fd2);				/* Parent process */
			return 0;
		}
	}
#endif

	struct linger linger;
	linger.l_onoff = 1;
	linger.l_linger = 1;

	setsockopt(fd2, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	int flag = 1;
	setsockopt(fd2, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, sizeof(int));

	unsigned long flag2 = (flags & SimSock::NOWAIT ? 1 : 0);
	ioctl(fd2, FIONBIO, &flag2);

	SimSock* sock = new SimSock(flags, eom, SOCK_INTERNAL);
	sock->fd = fd2;
	sock->connected = 1;
	sock->port = port;

	return sock;
}

SockClient::SockClient(unsigned flags, char eom) :
	SimSock(flags, eom, SOCK_CLIENT)
{
	host_name = port_name = 0;
}

SockClient::~SockClient()
{
	if (host_name) free(host_name);
	if (port_name) free(port_name);
}

int SockClient::Connect(const char* host, const char* service)
{
	fd = socket(PF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		//perror("socket");
		//printf("errno = %d\n", errno);
		return -1;
	}

	struct linger linger;
	linger.l_onoff = 1;
	linger.l_linger = 5;

	setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	// Stay in touch with the state of things...

	int flag = 1;
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, sizeof(int));

	// Disable the nagle algorithm, which delays sends till the
	// buffer is full (or a certain time period has passed?)...

#if defined(_Windows) || (defined(IPPROTO_TCP) && defined(TCP_NODELAY))
	flag = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
#endif

	struct sockaddr_in remote;

	remote.sin_family = PF_INET;
	remote.sin_port = port = servername(service);
	remote.sin_addr.s_addr = hostname(host);

	if (connect(fd, (struct sockaddr *)&remote, sizeof(remote)) < 0)
		return -errno;

	unsigned long flag2 = (flags & SimSock::NOWAIT ? 1 : 0);
	ioctl(fd, FIONBIO, &flag2);

	if (host_name) free(host_name);
	host_name = strdup(host);

	if (port_name) free(port_name);
	port_name = strdup(service);
	connected = 1;
	return fd;
}

int SockClient::WaitOnConnect(long seconds)
{
	int ret = WaitForWrite(seconds);

	if (ret > 0)
		connected = 1;
	else if (ret < 0)
		return -1;

	return IsConnected();
}

SockHandler::SockHandler()
{
	socks = new ListOf;
	isocks = new ListOfIterator(*socks);
	read_fds = new fd_set;
	write_fds = new fd_set;
	except_fds = new fd_set;
}

SockHandler::~SockHandler()
{
	void* t;

	while (socks->Pop(t))
	{
		SimSock* sock = (SimSock*)t;

		if (sock->type == SimSock::SOCK_INTERNAL)
        	delete sock;
	}

	delete isocks;
	delete socks;
	delete read_fds;
	delete write_fds;
	delete except_fds;
}

void SockHandler::Register(SimSock* sock)
{
	ListOfIterator ilist(*socks);
	void* t;

	for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
	{
		SimSock* s = (SimSock*)t;

		if (s == sock)
        	return;    	
	}

	if (sock)
		socks->Push(sock);
}

void SockHandler::DeRegister(SimSock* sock)
{
	ListOfIterator ilist(*socks);
	 void* t;

	for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
	{
		SimSock* s = (SimSock*)t;

		if (s == sock)
		{
			isocks->Delete();
			return;
		}
	}
}

unsigned long SockHandler::Count() const
{
	return socks->Count();
}

int SockHandler::Wait(long seconds, long microseconds)
{
	struct timeval timeval = {seconds, microseconds};

	void* t;

	FD_ZERO(read_fds);
	FD_ZERO(write_fds);
	FD_ZERO(except_fds);

	for (int ok = isocks->First(t); ok; ok = isocks->Next(t))
	{
		SimSock* sock = (SimSock*)t;
		FD_SET(sock->fd, read_fds);
		FD_SET(sock->fd, write_fds);
		FD_SET(sock->fd, except_fds);
	}

	// Start iterator...

	isocks->First();

	return select(FD_SETSIZE, read_fds, 0, except_fds, (seconds < 0 ? 0 : &timeval));
}

SimSock* SockHandler::Notify(SockStatus& status)
{
	if (!socks)
	{
		status = SOCK_NONE;
		return 0;
	}

	void* t;

	while (isocks->Current(t))
	{
		SimSock* sock = (SimSock*)t;

		// Data on a socket...

		if (FD_ISSET(sock->fd, read_fds))
		{
			if (sock->type != SimSock::SOCK_SERVER)
			{
				char buf[1];

				if (recv(sock->fd, buf, sizeof(buf), MSG_PEEK) <= 0)
				{
					status = SOCK_DISCONNECT;
					DeRegister(sock);
					sock->Close();
				}
				else
					status = SOCK_DATA;

				isocks->Next();
				return sock;
			}

			// Data on a server socket means a connection request...

			SimSock* s = ((SockServer*)sock)->Accept();

			if (!s)
			{
				status = SOCK_NONE;			// Parent process */
				isocks->Next();
				return s;
			}

			status = SOCK_CONNECT;
			Register(s);
			isocks->Next();
			return s;
		}

		// Exception on a socket...

		if (FD_ISSET(sock->fd, except_fds))
		{
			status = SOCK_ERROR;
			DeRegister(sock);
			sock->Close();
			isocks->Next();
			return sock;
		}

		isocks->Next();
	}

	status = SOCK_NONE;
	return 0;
}

