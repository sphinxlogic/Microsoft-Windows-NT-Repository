///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  A POP3 mail client
//
// Edit History
//
//	  Started June 22th 1996 by Andrew Davison.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef POPMAIL_H
#define POPMAIL_H

#ifndef SIMSOCK_H
#include "simsock.h"
#endif

class PopClient : private SockClient
{
public:

	PopClient();
	~PopClient();

	int Connect(const char* host, const char* service = "pop3");
	int Disconnect();
	int Login(const char* userid, const char* passwd);
	const char* GetStatus() const;
	int GetCount();
	long GetLength() const { return length; };
	int Retrieve(int n);
	const char* GetSubject();
	const char* GetBody();
	const char* GetReplyTo();
	int Delete(int n);
	const char* GetRaw() const { return msgbuf; };
	void SetRaw(char* text);

private:

	int OK();
	int Quit();

	char* msgbuf;
	char buf[1024], hdr[1024];
	int count;
	int length;
};


#endif

