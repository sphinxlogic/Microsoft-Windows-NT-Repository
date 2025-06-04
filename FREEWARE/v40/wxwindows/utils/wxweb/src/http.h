///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTTP handler.
//
// Edit History
//
//	  Started November 25th 1995 by Andrew Davison.
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef HTTP_H
#define HTTP_H

#include <time.h>

#ifndef ASTRING_H
#include "astring.h"
#endif

class ListOf;

class HttpClient
{
public:

	HttpClient();
	virtual ~HttpClient();

	void SetProxyServer(const string& host, const string& p, int enabled=1, const string& exclude="");
	void GetProxyServer(string& _host, string& _port, int& enabled, string& _exclude) const;

	int GetURL(const string& url, string& localfilename);
	string GetURL(const string& url);

	int GetHttpCode() const { return http_code; };
	unsigned long GetHttpContentLength() const { return http_length; };
	const string& GetHttpContentType() const { return http_type; };
	const string& GetHttpLastModified() const { return http_last_modified; };
	const string& GetHttpExpires() const { return http_expires; };

	const string& _GetCurrentURL() const { return url; };
	const string& _GetBaseURL() const { return base; };

	enum scheme {PROTO_UNKNOWN=0, PROTO_HTTP=1, PROTO_FTP=2, PROTO_HTTPS=3};

	int Get(const string& host, const string& portname, const string& filename, string& localname, int inline_object=0, int post=0, int head=0, enum scheme=PROTO_HTTP, const string& data="");
	int Put(const string& host, const string& portname, const string& filename, enum scheme=PROTO_HTTP, const string& data="");
	void SetCookies(int state=1) { use_cookies = state; };
	int GetCookies() const { return use_cookies; };
	void Cancel() { cancel = 1; };

protected:

	static string proxy_host, proxy_port, proxy_exclude, cache_path;
	static int proxies, nocache;
	int was_secure, http_code;
	string location, current_server, url, base;

	struct Unreachable
	{
		string hostname, portname;
		time_t when;
	};
	static ListOf unreachables;

	virtual string SaveAs(const string& /*u*/) { return ""; };
	virtual string Authenticate(const string& /*realm*/) { return ""; };
	virtual void OnSetStatusText(const char*, int /*number*/=0) {};
//	virtual void AddCache(const string& /*u*/, const string& /*localname*/="", const string& /*last_modified*/=0, const string& /*expires*/=0, int /*not_found*/=0, const string& /*mime_type*/="", int /*head*/=0) {};
        virtual void AddCache(const string& u, const string& localname="", 
            const string& last_modified=(char)0, const string& expires=(char)0, 
            int not_found=0, const string& mime_type="", int head=0) {};

	virtual int SearchCache(const string& /*u*/, string& /*localname*/, string& /*last_modified*/, string& /*expires*/, int& /*not_found*/, string& /*mime_type*/, int /*head*/) { return 0; };
	virtual int RemoveCache(const string& /*u*/) { return 1; };
	virtual void DoYield() const {};

private:

	string http_last_modified, http_expires, http_type;
	unsigned long http_length;
	int use_cookies, cancel;

	int Do(const string& host, const string& portname, const string& filename, string& localname, int inline_object, int post, int head, enum scheme, int put, const string& data);
};

#endif
