///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An SMPTP mail client
//
//
// Error status
//
//  < 0    - socket error
//  = 0    - SMTP protocol error
//  > 0    - success
//
// Edit History
//
//	  Started August 30th 1996 by Andrew Davison.
//
///////////////////////////////////////////////////////////////////////////////

#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
//#include <math.h>

#include "smtpmail.h"
#include "base64.h"

///////////////////////////////////////////////////////////////////////////////

static char *Map8Bit(const char* src, int& len)
{
	int nbytes = (strlen(src) * 2) + 1024;
	char *p, *pp;
	char* buf = p = new char[nbytes];

	while (len > 0)
	{
		if ( (*src == '\n') && (src[1] == '.') && (src[2] == '\r') )
			*p++ = '.';

		if ( (((unsigned char)*src) > 127) || (*src == '=') )
		{
			char mybuf[16];

			sprintf(mybuf,"=%02X", (unsigned int)((unsigned char)*src));
			pp = mybuf;

			while (*pp)
				*p++ = *pp++;

			src++;
			len--;
			continue;
		}

		*p++ = *src++;
		len--;
	}

	*p = 0;

	len = p - buf;
	return buf;
}

///////////////////////////////////////////////////////////////////////////////

SmtpClient::SmtpClient(int use_mime, int use_8bit) :
	SockClient()
{
	connected = 0;
	mime = use_mime;
	map_8bit = use_8bit;
	buf[0] = 0;
}

///////////////////////////////////////////////////////////////////////////////

SmtpClient::~SmtpClient()
{
	if (connected) Disconnect();
}

///////////////////////////////////////////////////////////////////////////////

int SmtpClient::Connect(const char* host, const char* service)
{
	int status;

	if ((status = SockClient::Connect(host, service)) < 0)
		return status;

	return OK();
}

///////////////////////////////////////////////////////////////////////////////

int SmtpClient::Disconnect()
{
	Quit();

	connected = 0;

	ClearAttachments();

	return Close();
}

///////////////////////////////////////////////////////////////////////////////

void SmtpClient::ClearAttachments()
{
	void* t;
	while (attachments.Pop(t))
		free(t);
}

///////////////////////////////////////////////////////////////////////////////

const char* SmtpClient::GetStatus() const
{
	return buf;
}

///////////////////////////////////////////////////////////////////////////////

int SmtpClient::OK()
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

	if (buf[0] > '3')
		return 0;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

int SmtpClient::Post(const char* from, const char* reply_to, const char* to, const char* subject, const char* data, int data_len, int is_html, const char* base)
{
	if (!from || !strlen(from)) return 0;
	if (!to || !strlen(to)) return 0;

	int status;

	sprintf(buf, "HELO %s\r\n", LocalHost());
	Write(buf, strlen(buf));

	if ((status = OK()) <= 0)
		return status;

	sprintf(buf, "MAIL FROM: <%s>\r\n", from);
	Write(buf, strlen(buf));

	if ((status = OK()) <= 0)
		return status;

	char _to[1024];
	strcpy(_to, to);

	if (!strchr(to, '@'))
		strcat(_to, "@localhost");

	sprintf(buf, "RCPT TO: <%s>\r\n", _to);
	Write(buf, strlen(buf));

	if ((status = OK()) <= 0)
		return status;

	sprintf(buf, "DATA\r\n");
	Write(buf, strlen(buf));

	if ((status = OK()) <= 0)
		return status;

	//sprintf(buf, "X-Sender: %s\r\n", from);
	//Write(buf, strlen(buf));

	sprintf(buf, "X-Mailer: WXWEB\r\n");
	Write(buf, strlen(buf));

	sprintf(buf, "To: %s\r\n", to);
	Write(buf, strlen(buf));

	sprintf(buf, "From: <%s>\r\n", from);
	Write(buf, strlen(buf));

	if (reply_to && strlen(reply_to) && strcmp(from, reply_to))
	{
		sprintf(buf, "Reply-To: <%s>\r\n", from);
		Write(buf, strlen(buf));
	}

	if (subject && strlen(subject))
	{
		sprintf(buf, "Subject: %s\r\n", subject);
		Write(buf, strlen(buf));
	}

	if (mime)
	{
		sprintf(buf, "MIME-Version: 1.0\r\n");
		Write(buf, strlen(buf));
	}

	if (mime && attachments.Count())
	{
		const char* filename = tmpnam(0);

		MultiPart(filename, data, data_len, is_html);

#ifdef _Windows
		ifstream istr(filename, ios::in|ios::binary);
#else
		ifstream istr(filename, ios::in);
#endif

		while (!istr.read(buf, sizeof(buf)).eof())
			Write(buf, istr.gcount());

		if (istr.gcount())
			Write(buf, istr.gcount());

		istr.close();
		remove(filename);

		sprintf(buf, "\r\n.\r\n");
		Write(buf, strlen(buf));

		return OK();
	}

	if (mime)
	{
		if (is_html && base && base[0])
		{
			sprintf(buf, "Content-Base: %s\r\n", base);
			Write(buf, strlen(buf));
		}

		if (is_html && map_8bit)
			sprintf(buf,"Content-Type: text/html; charset=iso-8859-1\r\nContent-Transfer-Encoding: quoted-printable\r\n");
		else if (is_html)
			sprintf(buf,"Content-Type: text/html; charset=us-ascii\r\nContent-Transfer-Encoding: 7bit\r\n");
		else if (map_8bit)
			sprintf(buf,"Content-Type: text/plain; charset=iso-8859-1\r\nContent-Transfer-Encoding: quoted-printable\r\n");
		else
			sprintf(buf,"Content-Type: text/plain; charset=us-ascii\r\nContent-Transfer-Encoding: 7-bit\r\n");
	}
	else
	{
		sprintf(buf,"Content-Type: text/%s; charset=us-ascii\r\nContent-Transfer-Encoding: 7bit\r\n", is_html?"html":"plain");
	}

	Write(buf, strlen(buf));

	// Send data here

	if (mime && map_8bit && data && data_len)
	{
		char* tmp = Map8Bit(data, data_len);

		sprintf(buf, "Content-Length: %d\r\n", data_len+5);
		Write(buf, strlen(buf));

		sprintf(buf, "\r\n");
		Write(buf, strlen(buf));

		Write(tmp, data_len);
		delete [] tmp;
	}
	else if (mime && data && data_len)
	{
		sprintf(buf, "Content-Length: %d\r\n", data_len+5);
		Write(buf, strlen(buf));

		sprintf(buf, "\r\n");
		Write(buf, strlen(buf));

		Write(data, data_len);
	}
	else if (data && data_len)
	{
		sprintf(buf, "\r\n");
		Write(buf, strlen(buf));

		Write(data, data_len);
	}


	sprintf(buf, "\r\n.\r\n");
	Write(buf, strlen(buf));

	// Done

	return OK();
}

///////////////////////////////////////////////////////////////////////////////

int SmtpClient::Quit()
{
	sprintf(buf, "QUIT\r\n");
	Write(buf, strlen(buf));

	return OK();
}

///////////////////////////////////////////////////////////////////////////////

void SmtpClient::Attach(const char* filename)
{
	attachments.Append(strdup(filename));
}

///////////////////////////////////////////////////////////////////////////////

int SmtpClient::MultiPart(const char* filename, const char* data, int data_len, int is_html)
{
	char boundary[256];
	sprintf(boundary, "NEXT_PART_%ld_%ld", time(0), (long)clock());

#ifdef _Windows
	ofstream ostr(filename, ios::out|ios::binary);
#else
	ofstream ostr(filename, ios::out);
#endif

	ostr << "This is a multi-part message in MIME format.\r\n";

	// The message...

	ostr << "\r\n--" << boundary << "\r\n";

	if (is_html)
		ostr << "Content-Type: text/html; charset=us-ascii\r\nContent-Transfer-Encoding: 7bit\r\n";
	else if (map_8bit)
		ostr << "Content-Type: text/plain; charset=iso-8859-1\r\nContent-Transfer-Encoding: quoted-printable\r\n";
	else
		ostr << "Content-Type: text/plain; charset=us-ascii\r\nContent-Transfer-Encoding: 7bit\r\n";

	ostr << "\r\n";

	if ((is_html || !map_8bit) && data && data_len)
	{
		ostr.write(data, data_len);
	}
	else if (data && data_len)
	{
		char* tmp = Map8Bit(data, data_len);
		ostr.write(tmp, data_len);
		delete [] tmp;
	}

	ostr << "\r\n";

	// The attachments...

	void* t;

	while (attachments.Pop(t))
	{
		const char* attached_filename = (char*)t;

#if defined(_Windows)
		const char* f = strrchr(attached_filename, '\\');
#else
		const char* f = strrchr(attached_filename, '/');
#endif

		if (f)
			f++;
		else
			f = attached_filename;

		const char* mime_type;
		const char* ext = strrchr(f, '.');

		if (ext && !strncmp(ext, ".htm", 4))
			mime_type = "text/html";
		else if (ext && !strcmp(ext, ".txt"))
			mime_type = "text/plain";
		else if (ext && !strcmp(ext, ".gif"))
			mime_type = "image/gif";
		else if (ext && !strcmp(ext, ".jpg"))
			mime_type = "image/jpeg";
		else if (ext && !strcmp(ext, ".png"))
			mime_type = "image/png";
		else if (ext && !strcmp(ext, ".zip"))
			mime_type = "application/x-zip-compressed";
		else
			mime_type = "application/octet-stream";

		ostr <<  "\r\n--" << boundary << "\r\n";
		ostr << "Content-Type: " << mime_type << "; name=\"" << f << "\"\r\n";

		if (!strncmp(mime_type, "text/", 5))
		{
			ostr << "Content-Transfer-Encoding: 7bit\r\n";
		}
		else
		{
			ostr << "Content-Transfer-Encoding: base64\r\n";
		}

		ostr << "Content-Disposition: inline; filename=\"" << f << "\"\r\n";
		ostr << "\r\n";

#ifdef _Windows
		ifstream istr(attached_filename, ios::in|ios::nocreate|ios::binary);
#else
		ifstream istr(attached_filename, ios::in|ios::nocreate);
#endif

		if (!strncmp(mime_type, "text/", 5))
		{
			while (!istr.read(buf, sizeof(buf)).eof())
				ostr.write(buf, sizeof(buf));

			if (istr.gcount())
				ostr.write(buf, istr.gcount());
		}
		else
		{
			Base64Encoder b64(ostr);

			while (!istr.read(buf, sizeof(buf)).eof())
				b64.Put((unsigned char*)buf, sizeof(buf));

			if (istr.gcount())
				b64.Put((unsigned char*)buf, istr.gcount());

			b64.InputFinished();
		}

		free(t);
	}

	ostr << "\r\n--" << boundary << "--";
	ostr.flush();

	// The preamble...

	sprintf(buf,"Content-Type: multipart/mixed; boundary=\"%s\"\r\nContent-Length: %ld\r\n\r\n", boundary, ostr.tellp()+5);
	Write(buf, strlen(buf));

	return 1;
}


