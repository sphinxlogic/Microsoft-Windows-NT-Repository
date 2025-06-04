#ifndef SSLSOCK_H
#define SSLSOCK_H

#include <stdio.h>
#include <ssl.h>
#include "simsock.h"

class SSL_SockClient : public SockClient
{
public:

	SSL_SockClient(unsigned flags = SimSock::NONE, char eom = '\n');

	int Connect(const char* host, const char* service);
	int Close();

	int Read(char* buffer, int nbytes);
	int Write(const char* buffer, int nbytes);
	int ReadMsg(char* buffer, int nbytes);
	int Wait(long seconds = -1, long microseconds = 0);

private:

	SSL* ssl_con;
};

#endif
