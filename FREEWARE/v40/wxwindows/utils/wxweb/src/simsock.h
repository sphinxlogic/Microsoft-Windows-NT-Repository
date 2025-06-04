////////////////////////////////////////////////////////////////////////
//
// Description
//
//	 Simple socket class.
//
////////////////////////////////////////////////////////////////////////

#ifndef SIMSOCK_H
#define SIMSOCK_H

////////////////////////////////////////////////////////////////////////

class SimSock
{
public:

	enum SockFlags { NONE=0, NOWAIT=1, FORK=2 };

	virtual ~SimSock();
	virtual int Close();
	virtual int Peek(char* buffer, int nbytes);
	virtual int Read(char* buffer, int nbytes);
	virtual int Write(const char* buffer, int nbytes);
	virtual int ReadMsg(char* buffer, int nbytes);

	int OK() { return (fd < 0 ? 0 : 1); };


	int IsConnected() const { return connected; };
	int IsDisconnected() const { return !IsConnected(); };
	int IsNoWait() const { return flags & NOWAIT; };
	int IsFork() const { return flags & FORK; };
	int GetPort() const { return port; };
	int GetFD() const { return fd; };
	
	// seconds = -1 means indefinite wait
	// seconds = 0 means no wait
	// seconds > 0 means specified wait

	virtual int Wait(long seconds = -1, long microseconds = 0);
	virtual int WaitForRead(long seconds = -1, long microseconds = 0);
	virtual int WaitForWrite(long seconds = -1, long microseconds = 0);
	virtual int WaitForError(long seconds = -1, long microseconds = 0);

	static const char* LocalHost();

protected:

	enum SockType { SOCK_CLIENT, SOCK_SERVER, SOCK_INTERNAL };

	SimSock(unsigned flags, char eom, SockType type);

	unsigned flags;
	SockType type;
	int connected, fd, port;
	char eom;

	int _Wait(long seconds, long microseconds, int type);

private:

	static int cnt;			// Keep track of WinSock initialisations

	friend class SockHandler;
	friend class SockServer;
};

////////////////////////////////////////////////////////////////////////

class SockServer : public SimSock
{
public:

	// 'service' can be a name or a port-number

	SockServer(const char* service, unsigned flags = SimSock::NONE, char eom = '\n');

	SimSock* Accept();
};

////////////////////////////////////////////////////////////////////////

class SockClient : public SimSock
{
public:

	SockClient(unsigned flags = SimSock::NONE, char eom = '\n');
	~SockClient();

	// 'host' can be a name or internet-address-number
	// 'service' can be a name or a port-number

	virtual int Connect(const char* host, const char* service);

	int WaitOnConnect(long seconds = -1);
	const char* GetHostName() const { return host_name; };
	const char* GetPortName() const { return port_name; };

private:

	char* host_name;
	char* port_name;
};

////////////////////////////////////////////////////////////////////////

struct fd_set;
class ListOf;
class ListOfIterator;

class SockHandler
{
public:

	SockHandler();
	~SockHandler();

	void Register(SimSock* sock);
	void DeRegister(SimSock* sock);

	unsigned long Count() const;

	// seconds = -1 means indefinite wait
	// seconds = 0 means no wait
	// seconds > 0 means specified wait

	int Wait(long seconds = -1, long microseconds = 0);

	enum SockStatus { SOCK_NONE, SOCK_DATA, SOCK_CONNECT, SOCK_DISCONNECT, SOCK_ERROR };

	SimSock* Notify(SockStatus&);

private:

	ListOfIterator* isocks;
	ListOf* socks;
	fd_set* read_fds;
	fd_set* write_fds;
	fd_set* except_fds;
};

#endif
