///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An SMTP mail client
//
// Edit History
//
//	  Started August 30th 1996 by Andrew Davison.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SMTPMAIL_H
#define SMTPMAIL_H

#ifndef SIMSOCK_H
#include "simsock.h"
#endif

#ifndef LISTOF_H
#include "listof.h"
#endif

class SmtpClient : private SockClient
{
public:

	SmtpClient(int use_mime = 1, int map_8bit = 1);
	~SmtpClient();

	int Connect(const char* host, const char* service = "smtp");
	int Disconnect();
	int Post(const char* from, const char* reply_to, const char* to, const char* subject, const char* data, int len, int is_html=0, const char* base=0);
	const char* GetStatus() const;
	void Attach(const char* filename);
	void ClearAttachments();

private:

	int MultiPart(const char* filename, const char* data, int len, int is_html);
	int OK();
	int Quit();

	char buf[1024];
	int count;
	int mime, map_8bit, connected;
	ListOf attachments;
};


#endif

