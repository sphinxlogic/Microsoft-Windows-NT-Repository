///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  A POP3 mail client
//
//
// Error status
//
//  < 0    - socket error
//  = 0    - POP3 protocol error
//  > 0    - success
//
// Edit History
//
//	  Started June 22th 1996 by Andrew Davison.
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <fstream.h>

#include "popmail.h"

//static ofstream log("x.log", ios::out|ios::binary|ios::trunc);

///////////////////////////////////////////////////////////////////////////////

PopClient::PopClient() :
	SockClient()
{
	count = 0;
	buf[0] = 0;
	msgbuf = 0;
}

///////////////////////////////////////////////////////////////////////////////

PopClient::~PopClient()
{
	if (msgbuf)
		delete [] msgbuf;
}

///////////////////////////////////////////////////////////////////////////////

int PopClient::Connect(const char* host, const char* service)
{
	int status;

	if ((status = SockClient::Connect(host, service)) < 0)
		return status;

	return OK();
}

///////////////////////////////////////////////////////////////////////////////

int PopClient::Disconnect()
{
	Quit();

	return Close();
}

///////////////////////////////////////////////////////////////////////////////

const char* PopClient::GetStatus() const
{
	return buf;
}

///////////////////////////////////////////////////////////////////////////////

int PopClient::OK()
{
	int secs = 30;

	while (--secs && !Wait(1))
		;

	if (!secs || !IsConnected())
		return -1;

	int len = ReadMsg(buf, sizeof(buf)-1);

	if (len < 0)
		return len;

	buf[len] = 0;
	//log << buf << flush;

	if (buf[0] != '+')
		return 0;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

int PopClient::Login(const char* userid, const char*passwd)
{
	sprintf(buf, "USER %s\r\n", userid);
	Write(buf, strlen(buf));
	//log << buf << endl;

	int status = OK();
	if (status <= 0)
		return status;

	sprintf(buf, "PASS %s\r\n", passwd);
	Write(buf, strlen(buf));
	//log << buf << endl;

	return OK();
}

///////////////////////////////////////////////////////////////////////////////

int PopClient::Quit()
{
	sprintf(buf, "QUIT\r\n");
	Write(buf, strlen(buf));
	//log << buf << endl;

	return OK();
}

///////////////////////////////////////////////////////////////////////////////

int PopClient::GetCount()
{
	sprintf(buf, "STAT\r\n");
	Write(buf, strlen(buf));
	//log << buf << endl;

	int status = OK();
	if (status <= 0)
		return status;

	count = 0;
	sscanf(buf, "%*s %d %*d", &count);

	return count;
}

///////////////////////////////////////////////////////////////////////////////

int PopClient::Delete(int n)
{
	sprintf(buf, "DELE %d\r\n", n);
	Write(buf, strlen(buf));
	//log << buf << endl;

	return OK();
}

///////////////////////////////////////////////////////////////////////////////

int PopClient::Retrieve(int n)
{
	sprintf(buf, "RETR %d\r\n", n);
	Write(buf, strlen(buf));
	//log << buf << endl;

	int status = OK();
	if (status <= 0)
		return status;

	long nbytes = 0;
	sscanf(buf, "%*s %ld", &nbytes);

	if (msgbuf)
		delete [] msgbuf;

	msgbuf = new char[nbytes*2];
	char* dst = msgbuf;
	int len;
	long cnt = 0;

	while ((len = ReadMsg(dst, 512)) > 0)
	{
		cnt += len;

		if ((dst[0] == '.') && (len == 3))
			break;

		dst += len;
	}

	*dst = 0;

	length = dst - msgbuf;

	//log << "Read: " << cnt << " chars" << endl;

	return status;
}

///////////////////////////////////////////////////////////////////////////////

const char* PopClient::GetSubject()
{
	const char* src = strstr(msgbuf, "Subject:");

	if (!src) return ("none");

	while (*src != ':')
		src++;

	src++;

	while (*src == ' ')
		src++;

	char* dst = hdr;

	while (*src && (*src != '\r') && (*src != '\n'))
		*dst++ = *src++;

	*dst = 0;

	return hdr;
}

///////////////////////////////////////////////////////////////////////////////

const char* PopClient::GetReplyTo()
{
	const char* src = strstr(msgbuf, "Reply-To:");

	if (!src) src = strstr(msgbuf, "From:");

	if (!src) return ("[unknown]");

	while (*src != ':')
		src++;

	src++;

	while (*src != '<')
		src++;

	src++;

	char* dst = hdr;

	while (*src && (*src != '>') && (*src != '\r') && (*src != '\n'))
		*dst++ = *src++;

	*dst = 0;

	return hdr;
}

///////////////////////////////////////////////////////////////////////////////

const char* PopClient::GetBody()
{
	const char* ptr = msgbuf;
	const char* src = msgbuf;

	do
	{
		while (*src && (*src != '\r'))
			src++;

		if (src == ptr)
			break;

		src++; src++;
		ptr = src;
	}
	  while (*src);

	src++; src++;

	return src;
}

///////////////////////////////////////////////////////////////////////////////

void PopClient::SetRaw(char* text)
{
	if (msgbuf)
		delete [] msgbuf;

	msgbuf = text;
}




