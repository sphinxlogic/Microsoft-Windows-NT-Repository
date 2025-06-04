////////////////////////////////////////////////////////////////////////
// Description
//
//  Allow iostreams to run over a socket in a portable manner, as
//  under Windows a SOCKET is not the same as a file descriptor and
//  cannot be used interchangeably.
//
// Edit History
//
//  Written November 1995 by A. Davison.
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
////////////////////////////////////////////////////////////////////////

#ifndef STRSOCK_H
#define STRSOCK_H

#include <stdlib.h>
#include <string.h>
#include <strstream.h>
#include "simsock.h"

class  SockStreambuf : public strstreambuf
{
public:

	SockStreambuf(SimSock* s) :
		strstreambuf(buffer, sizeof(buffer), 0),
			sock(s)
	{
		setp(pbase(), pbase()+blen());
		setg(base(), base(), base());
	};

	int sync()
	{
		int len = out_waiting();
		//cout << "SYNC: " << len << ", sock=" << sock << endl;
		if (len) len = sock->Write(pbase(), len);
		setp(pbase(), pbase()+blen());
		return strstreambuf::sync();;
	 };

	int overflow(int ch)
	{
		int len = out_waiting();
		//cout << "OVERFLOW: " << len << ", sock=" << sock << endl;
		if (len) len = sock->Write(pbase(), len);
		setp(pbase(), pbase()+blen());
		sputc(ch);
		return (len < 0 ? EOF : 0);
	};

	int underflow()
	{
		//cout << "UNDERFLOW sock=" << sock << endl;
		sock->Wait(-1);
		int len = sock->Read(base(), blen());
		//cout << "READ: " << len << endl;
		if (len <= 0) return EOF;
		setg(base(), base(), base()+len);
		return 0;
	};

private:

	SimSock* sock;
	char buffer[1024];

	friend class SockStreambase;
};

class SockStreambase : public virtual ios
{
public:

	SockStreambase(SimSock* s) :
		buf(s)
	{
		ios::init(&buf);
	};

	void SetSock(SimSock* s) { buf.sock = s; };
	SimSock* GetSock() const { return buf.sock; };

protected:

	SockStreambuf buf;
};

class OSockStream : public SockStreambase, public ostream
{
public:

	OSockStream(SimSock* s = 0) :
		SockStreambase(s),
			ostream(&buf)
	{
		buf.seekoff(0, ios::beg, ios::out);
	};
};

class ISockStream : public SockStreambase, public istream
{
public:

	ISockStream(SimSock* s = 0) :
		SockStreambase(s),
			istream(&buf)
	{
		buf.seekoff(0, ios::end, ios::in);
	};

	void reset()
	{
		buf.seekoff(0, ios::end, ios::in);
	}
};

class SockStream : public ISockStream, public OSockStream
{
public:

	SockStream(SimSock* s = 0) :
		ISockStream(s),
			OSockStream(s)
	{
	};

	void SetSock(SimSock* s)
	{
		ISockStream::SetSock(s);
		OSockStream::SetSock(s);
	};
};

#endif

