///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//   Implement HttpClient.
//
// Edit History
//
//	  Started November 25th 1995 by Andrew Davison.
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <ctype.h>
#include <sys/stat.h>

#if USE_SSL
#include "sslsock.h"
#else
#include "simsock.h"
#endif

#include "listof.h"
#include "http.h"
#include "url.h"

#ifdef unix 
#define strcmpi(s1, s2) strcasecmp(s1, s2)
#define strncmpi(s1, s2, n) strncasecmp(s1, s2, n)
#ifdef BSD
extern "C" int strcasecmp(const char*s1, const char* s2);
extern "C" int strncasecmp(const char*s1, const char* s2, int n);
#endif
#elif !defined (__BORLANDC__)
#define strcmpi _stricmp
#define strncmpi _strnicmp
#endif

#ifdef BSD
#define SPRINTF(exp) strlen(sprintf exp)
#else
#define SPRINTF(exp) sprintf exp
#endif

string HttpClient::proxy_host, HttpClient::proxy_port, HttpClient::proxy_exclude, HttpClient::cache_path;
int HttpClient::proxies = 0, HttpClient::nocache = 1;
ListOf HttpClient::unreachables;

#ifdef wx_msw
#define SEP '\\'
#else
#define SEP '/'
#endif

#define MinOf(a, b) ((a) < (b) ? (a) : (b))

struct AuthCookie
{
	string url, authentication_scheme, realm, cookie;
};
static ListOf auth_cookies;

struct NetscapeCookie
{
	string host, name, value, path, domain, expires;
	int secure;
};
static ListOf netscape_cookies;

const time_t CONNECTION_TIMEOUT = 60;
static ListOf connections;
static SockClient dummy_sock;
static unsigned instance_count = 0;
const char* version = "2.17";
const BUFFER_SIZE = 4096;

ofstream log;

//////////////////////////////////////////////////////////////
// Timestamp io-manipulator...
//////////////////////////////////////////////////////////////

static ostream& ts(ostream& ostr)
{
	time_t now = time(0);
	char *ptr;

	ptr = asctime(gmtime(&now));
	*strchr(ptr, '\n') = 0;
	return ostr << " " << ptr << " GMT ";
}

///////////////////////////////////////////////////////////////////////////////
// A connection describes a long-lived (keep-alive) socket...
///////////////////////////////////////////////////////////////////////////////

struct HttpConnection
{
	HttpConnection(SockClient* s) { sock = s; time(&last); busy = 0; };
	~HttpConnection() { delete sock; };

	SockClient* GetSock() const { return sock; };

	string hostname, portname;
	time_t last;
	time_t keep_alive_period;
	int max_conns;
	float http_ver;
	int busy;
	unsigned long nbr;

private:

	SockClient* sock;
};

///////////////////////////////////////////////////////////////////////////////
// Simple but effective enuff random number generator...
///////////////////////////////////////////////////////////////////////////////

static long longrnd()
{
	static long seed = clock();
	return seed = ((seed * 2743) + 5923) & -1L;
}

///////////////////////////////////////////////////////////////////////////////
// Note: path should NOT include the trailing separator e.g. "/usr/tmp"
///////////////////////////////////////////////////////////////////////////////

static string TempFileNameWithExtension(const char* path, const char* extension)
{
	struct stat st;
	char buf[1024];
	char ext[256];

	strcpy(ext, extension);

#if defined (wx_msw) && !defined (WIN32)
	if (strlen(ext) > 4)
		ext[4] = 0;
#endif

	// Loop untill we get a unique filename...

	do
	{
		sprintf(buf, "%s%0.8lX%s", (path?path:""), longrnd(), (ext?ext:".tmp"));
	}
	  while (stat(buf, &st) == 0);		// File exists

	return string(buf);
}

///////////////////////////////////////////////////////////////////////////////

int InList(const string& list, const string& s)
{
	string attr, value;
	const char* src = list.data();

	while (GetAttributeValuePair(src, attr, value))
	{
		const char* wild = strchr(attr.data(), '*');

		if (!wild)
		{
			if (!strcmpi(attr.data(), s.data()))
				return 1;
		}
		else
		{
			size_t len = wild - attr.data();

			if (!strncmpi(attr.data(), s.data(), len))
				return 1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

HttpClient::HttpClient()
{
	if (!instance_count++)
	{
		cache_path = ".";
	}

	use_cookies = 1;
	was_secure = 0;
	cancel = 0;
}

///////////////////////////////////////////////////////////////////////////////

HttpClient::~HttpClient()
{
	if (!--instance_count)
	{
		// Kill any outstanding connections for this session...

		void* a;

		while (connections.Pop(a))
			delete (HttpConnection*)a;
	}
}

///////////////////////////////////////////////////////////////////////////////

void HttpClient::SetProxyServer(const string& h, const string& p, int enabled, const string& x)
{
	proxies = enabled;
	proxy_host = h;
	proxy_port = p;
	proxy_exclude = x;
}

///////////////////////////////////////////////////////////////////////////////

void HttpClient::GetProxyServer(string& _host, string& _port, int& enabled, string& _exclude) const
{
	_host = proxy_host;
	_port = proxy_port;
	_exclude = proxy_exclude;
	enabled = proxies;
}

///////////////////////////////////////////////////////////////////////////////

string HttpClient::GetURL(const string& u)
{
	string localname;

	if (!GetURL(u, localname))
		return "";

	ifstream istr(localname.data());
	char buf[1024];
	string s = "";

	while (!istr.read(buf, sizeof(buf)).eof())
		s += string(buf, sizeof(buf));

	s += string(buf, istr.gcount());

	istr.close();

	RemoveCache(u);
	remove(localname.data());

	return s;
}

///////////////////////////////////////////////////////////////////////////////

int HttpClient::GetURL(const string& u, string& localname)
{
	string scheme, hostname, portname, filename, section;
	enum scheme proto;

	if (!ParseURL(u.data(), scheme, hostname, portname, filename, section))
		return 0;

	if (!strcmpi(scheme.data(), "http"))
		proto = PROTO_HTTP;
	else if (!strcmpi(scheme.data(), "https"))
		proto = PROTO_HTTPS;
	else if (!strcmpi(scheme.data(), "ftp"))
		proto = PROTO_FTP;
	else
		return 0;

	return Do(hostname, portname, filename, localname, 0, 0, 0, proto, 0, "");
}

///////////////////////////////////////////////////////////////////////////////

int HttpClient::Get(const string& hostname, const string& portname, const string& filename, string& localname, int inline_object, int post, int head, enum scheme scheme, const string& data)
{
	return Do(hostname, portname, filename, localname, inline_object, post, head, scheme, 0, data);

}

///////////////////////////////////////////////////////////////////////////////

int HttpClient::Put(const string& hostname, const string& portname, const string& filename, enum scheme scheme, const string& data)
{
	string localname = "";
	return Do(hostname, portname, filename, localname, 0, 0, 0, scheme, 1, data);
}

///////////////////////////////////////////////////////////////////////////////

int HttpClient::Do(const string& hostname, const string& _portname, const string& filename, string& localname, int inline_object, int post, int head, enum scheme scheme, int put, const string& data)
{
	static char* tmp = new char[BUFFER_SIZE];

	int ftp = (scheme == PROTO_FTP);

#if USE_SSL
	int https = (scheme == PROTO_HTTPS);
#else
	int https = 0;
#endif

	cancel = 0;

	// Poll connections...

	SockClient* sock = 0;
	time_t now = time(0);

	ListOfIterator ilist = connections;
	void* a;

	for (int ok2 = ilist.First(a); ok2; ok2 = ilist.Next(a))
	{
		HttpConnection* connection = (HttpConnection*)a;

		if ((connection->http_ver < 1.1F) &&
			( ((now - connection->last) > connection->keep_alive_period) ||
			  (connection->max_conns <= 0) ) )
		{
			if (!connection->max_conns)
				log << connection->nbr << ts << "Kill limited connection" << endl;
			else
				log << connection->nbr << ts << "Kill timed connection" << endl;

			connection->GetSock()->Close();
		}

		if (connection->GetSock()->WaitForError(0) ||
				!connection->GetSock()->IsConnected())
		{
			log << connection->nbr << ts << "Close connection" << endl;
			delete connection;
			ilist.Delete();
		}
	}

	// OK, carry on...

	string authentication_scheme, realm, auth_cookie;
	location = "";
	http_code = 0;

	if (!inline_object) current_server = "";

loop:

	string portname = _portname, mime_type = "", file_extension = "";
	int authentication_needed = 0;
	int not_found = 0;

	int keep_alive = 0, new_sock = 0, max_conns = 0, timeout = 0;

	if (portname == "80")
		portname = "";

	sprintf(tmp, "%s%s%s%s%s%s",
		(hostname.length() ? (ftp?"ftp://":(https?"https://":"http://")) : ""),
			(hostname.length() ? (char*)hostname.data() : ""),
			(portname.length() ? ":" : ""),
			(portname.length() ? (char*)portname.data() : ""),
			(hostname.length() ? "/" : ""),
			filename.data());

	string new_url = tmp;

	if (!portname.length())
		portname = "80";

	// Is the URL in the cache?

	if (!nocache && !post && !put &&
		SearchCache(new_url, localname, http_last_modified, http_expires, not_found, http_type, head))
	{
		if (https)
		{
			if (!was_secure && !inline_object)
				was_secure = 1;
		}
		else
		{
			if (was_secure && !inline_object)
				was_secure = 0;
		}

		//log << ts << "LOADING: " << filename << " <= " << localname << endl;

		if (!inline_object) current_server = "Cached";

		if (!not_found && !inline_object)
		{
			base = url = new_url;
		}

		return (not_found ? 0 : 1);
	}

	// Is the URL in the auth cookie-cache?

	ListOfIterator cilist = auth_cookies;

	for (int ok = cilist.First(a); ok; ok = cilist.Next(a))
	{
		AuthCookie* citem = (AuthCookie*)a;

		if (citem->url == new_url)
		{
			authentication_scheme = citem->authentication_scheme;
			realm = citem->realm;
			auth_cookie = citem->cookie;
			break;
		}
	}

	// Is it a local file?

/*
	if (!hostname.length())
	{
		const char* path = wxPathOnly((char*)url.data());

		if (IsAbsolutePath(filename.data()) || !path)
			sprintf(tmp, "%s", filename.data());
		else
			sprintf(tmp, "%s/%s", path, filename.data());

		log << ts << "LOADING: " << filename << endl;
		localname = tmp;

		return 1;
	}
*/

	// Using a proxy?

	int using_proxies = (https ? 0 : proxies);

	string which_host = (using_proxies ? proxy_host : hostname);
	string which_port = (using_proxies ? proxy_port : portname);

	// No proxies for these...

	if ((hostname == "localhost") || (hostname == "127.0.0.1") ||
		(hostname == SimSock::LocalHost()) || (hostname == proxy_host) || 
		InList(proxy_exclude, hostname))
	{
		which_host = hostname;
		which_port = portname;
		using_proxies = 0;
	}

	if (https) which_port = "443";		// Hardcode for now

	// Search for a cached connection...

	HttpConnection* connection = 0;
	static unsigned long conn_nbr = 0;
	unsigned long nbr;
	float http_ver = 1.0F;

	ilist = connections;
	for (int ok3 = ilist.First(a); ok3; ok3 = ilist.Next(a))
	{
		connection = (HttpConnection*)a;

		// Already connected?

		if ((connection->hostname == which_host) &&
				(connection->portname == which_port) &&
					!connection->busy)
		{
			connection->busy = 1;
			connection->max_conns--;
			time(&connection->last);
			sock = connection->GetSock();
			nbr = connection->nbr;
			http_ver = connection->http_ver;
			break;
		}
	}

	if (!sock)
	{
		// First search the list of unreachable hosts so we don't wait
		// around unnecessarily. Unreachable hosts can be retried after
		// 'n' minutes...

		ilist = unreachables;
		for (int ok4 = ilist.First(a); ok4; ok4 = ilist.Next(a))
		{
			Unreachable* item = (Unreachable*)a;

			if ( (item->hostname == hostname) &&
					(item->portname == portname) &&
						((time(0) - item->when) < 2) )
			{
				return 0;
			}
		}

		nbr = ++conn_nbr;

		sprintf(tmp, "Connecting %s... %s:%s %s%s",
			(using_proxies ? "(via proxy ":""),
			which_host.data(), which_port.data(),
			(using_proxies ? ") to ":""),
			(using_proxies ? proxy_host.data():"")
			);

		OnSetStatusText(tmp);
		log << nbr << ts << "CONNECT: " << which_host << ", Port: " << which_port << endl;

#if USE_SSL
		if (https)
		{
			if (!was_secure && !inline_object)
				was_secure = 1;

			sock = new SSL_SockClient(SimSock::NONE, '\n');
		}
		else
#endif
		{
			if (was_secure && !inline_object)
				was_secure = 0;

			sock = new SockClient(SimSock::NONE, '\n');
		}

		if (sock->Connect(which_host.data(), which_port.data()) < 0)
		{
			Unreachable* item = new Unreachable();
			item->hostname = which_host;
			item->portname = which_port;
			item->when = time(0);
			unreachables.Append(item);

			string s = "Unreachable: ";
			s += hostname;
			OnSetStatusText(s.data());
			log << ts << s << endl;
			delete sock;
			return 0;
		}

		DoYield();
		connection = 0;
		new_sock = 1;
	}

	sprintf(tmp, "Requesting: %s", new_url.data());
	OnSetStatusText(tmp);
	log << nbr << ts << tmp << endl;

	char* dst = tmp;

	const char* method = (post ? "POST" : (head ? "HEAD" : (put ? "PUT" : "GET")));

	if (using_proxies)
		dst += SPRINTF((dst, "%s %s HTTP/1.1\n", method, new_url.data()));
	else if (filename[0] == '/')
		dst += SPRINTF((dst, "%s %s HTTP/1.1\n", method, filename.data()));
	else
		dst += SPRINTF((dst, "%s /%s HTTP/1.1\n", method, filename.data()));

	dst += SPRINTF((dst, "Host: %s\n", hostname.data()));

	//if (url.length())
	//	dst += SPRINTF((dst, "Referer: %s\n", url.data()));

	if (using_proxies && (http_ver >= 1.1F))
	{
		dst += SPRINTF((dst, "Connection: close\n"));
	}
	else if (http_ver == 1.0F)
	{
		dst += SPRINTF((dst, "Connection: keep-alive, persist\n"));
		dst += SPRINTF((dst, "Persist: %s\n", hostname.data()));
		dst += SPRINTF((dst, "Keep-Alive:\n"));
	}

	dst += SPRINTF((dst, "User-Agent: %s (", (string("wxWeb/")+version).data()));

#ifdef wx_msw
	dst += SPRINTF((dst, "Windows; "));
#elif defined wx_xview
	dst += SPRINTF((dst, "Unix; "));
#elif defined wx_motif
	dst += SPRINTF((dst, "Unix; "));
#else
	dst += SPRINTF((dst, "other; "));
#endif

#ifdef wx_msw
#ifdef WIN32
	dst += SPRINTF((dst, "WIN32)"));
#else
	dst += SPRINTF((dst, "WIN16)"));
#endif
#elif defined wx_xview
	dst += SPRINTF((dst, "XView)"));
#elif defined wx_motif
	dst += SPRINTF((dst, "Motif)"));
#else
	dst += SPRINTF((dst, "other)"));
#endif

	dst += SPRINTF((dst, "\n"));

	if (authentication_scheme.length())
	{
		dst += SPRINTF((dst, "Authorization: %s %s\n",
					authentication_scheme.data(), auth_cookie.data()));
	}

	// Is the URL in the Netscape cookie-cache?

	cilist = netscape_cookies;
	int cookies = 0;

	for (int ok4 = cilist.First(a); ok4; ok4 = cilist.Next(a))
	{
		NetscapeCookie* citem = (NetscapeCookie*)a;

		if (citem->host == hostname)
		{
			cookies++;
		}
	}

	if (cookies)
	{
		dst += SPRINTF((dst, "Cookie: "));
	}

	for (int ok5 = cilist.First(a); cookies && ok5; ok5 = cilist.Next(a))
	{
		NetscapeCookie* citem = (NetscapeCookie*)a;

		if (citem->host == hostname)
		{
			dst += SPRINTF((dst, "%s=%s\n", citem->name.data(), citem->value.data()));

			if (--cookies)
			{
				*dst++ = ';';
				*dst++ = ' ';
			}
		}
	}

	if (cookies)
		*dst++ = '\n';

	dst += SPRINTF((dst, "Accept: text/html, text/plain, image/gif, image/x-ms-bmp\n"));

#ifdef wx_msw
	dst += SPRINTF((dst, "Accept: image/jpeg, image/pjpeg, image/png, image/xpm, audio/x-wav, application/x-msvideo\n"));
#else
	dst += SPRINTF((dst, "Accept: image/pbm, image/pcx, image/xbm\n"));
#endif

#if ALLOW_SITE_MODE
	dst += SPRINTF((dst, "Accept: text/wsml\n"));
#endif

	dst += SPRINTF((dst, "Accept: application/*\n"));

	if (post)
	{
		dst += SPRINTF((dst, "Content-Type: application/x-www-form-urlencoded\n"));
		dst += SPRINTF((dst, "Content-Length: %ld\n", (long)data.length()));
	}

	if (put)
	{
		dst += SPRINTF((dst, "Content-Type: text/html\n"));
		dst += SPRINTF((dst, "Content-Length: %ld\n", (long)data.length()));
	}

	if (post)
		log << nbr << ts << "POST: " << data << endl;

	sprintf(dst, "\n");		// Ends request...

	if (sock->Write(tmp, strlen(tmp)) < 0)
	{
		if (connection)
			connection->busy = 0;

		if (!keep_alive && new_sock)
			delete sock;
		else
			sock->Close();

		return 0;
	}

	// Post any form data...

	if (post || put)
	{
		long nbytes = data.length();
		const char* src = data.data();

		while (nbytes > 0)
		{
			int len = (int)MinOf(1024L, nbytes);
			if (sock->Write(src, len) < 0)
			{
				if (!keep_alive && new_sock)
					delete sock;
				else
					sock->Close();

				return 0;
			}

			src += len;
			nbytes -= len;
		}
	}

	DoYield();

//loop2:

	unsigned long length = ULONG_MAX;
	http_last_modified = http_expires = "none";
	http_type = "text/html";
	int no_cache_this_one = (put ? 1 : 0);
	int accept_ranges = 0;
	int length_specified = 0;
	string date = "";
	time_t timer = time(0);
	int status;

	while (!cancel)
	{
		status = sock->Wait(https?CONNECTION_TIMEOUT:1);

		if ((time(0) - timer) > CONNECTION_TIMEOUT)
			break;

		if (!status)
		{
			DoYield();
			continue;
		}

		timer = time(0);

		int len = sock->ReadMsg(tmp, BUFFER_SIZE-1);

		if (len <= 2)
			break;

		if (tmp[len-1] == '\n') len--;
		if (tmp[len-1] == '\r') len--;
		tmp[len] = 0;

		log << nbr << ts << "GOT: " << tmp << endl;

		static char status[1024], status2[1024];
		status[0] = status2[0] = 0;

		if (sscanf(tmp, "HTTP/%f %d %[^\r\n]", &http_ver, &http_code, status) >= 2)
		{
			// Authorization required?

			if (http_code == 401)
			{
				authentication_needed = 1;
			}

			// Forbidden?

			else if (http_code == 403)
			{
				authentication_scheme = realm = auth_cookie = "";
			}

			// OK?

			else if (/*inline_object &&*/ (http_code >= 200) && (http_code <= 299))
			{
				if (http_code == 204)
				{
					no_cache_this_one = 1;
					length = 0;
				}
			}

			// Proxy error (this is not technically correct)...

			else if ((http_code >= 500) && (http_code <= 599))
			{
				Unreachable* item = new Unreachable();
				item->hostname = hostname;
				item->portname = portname;
				item->when = time(0);
				unreachables.Append(item);
			}
			else
			{
				not_found = 1;
			}

			// In HTTP/1.1 connections are assumed to be persistent
			// unless otherwise notified, this is the oppostie of
			// HTTP/1.0 style keep-alives...

			if (http_ver >= 1.1F)
				keep_alive = 1;
			else
				keep_alive = 0;
		}
		else if (use_cookies && !strncmpi(tmp, "Set-Cookie:", 11))
		{
			static char rest[1024];
			rest[0] = 0;

			if (sscanf(tmp, "%*[^:]: %[^=]=%[^;\r\n]; %[^\r\n]", status, status2, rest) >= 2)
			{
				NetscapeCookie* citem = new NetscapeCookie();
				citem->host = hostname;
				citem->name = status;
				citem->value = status2;
				citem->secure = 0;

				string a, v;
				char* ptr = rest;

				while (GetAttributeValuePair(ptr, a, v))
				{
					if (!strcmpi(a.data(), "EXPIRES"))
					{
						citem->expires = v;
					}
					else if (!strcmpi(a.data(), "DOMAIN"))
					{
						citem->domain = v;
					}
					else if (!strcmpi(a.data(), "PATH"))
					{
						citem->path = v;
					}
					else if (!strcmpi(a.data(), "SECURE"))
					{
						citem->secure = 1;
					}
				}

				netscape_cookies.Push(citem);
			}
		}
		else if (!strncmpi(tmp, "WWW-Authenticate:", 17))
		{
			if (sscanf(tmp, "%*[^:]: %s %[^\r\n]", status, status2) == 2)
			{
				authentication_scheme = status;
				realm = status2;
			}
		}
		else if ((http_ver == 1.0F) && !strncmpi(tmp, "Keep-Alive:", 11))
		{
			if (sscanf(tmp, "%*[^:]: timeout=%d, max=%d", &timeout, &max_conns) == 2)
				keep_alive = 1;
			else if (sscanf(tmp, "%*[^:]: max=%d, timeout=%d", &max_conns, &timeout) == 2)
				keep_alive = 1;
		}
		else if (!strncmpi(tmp, "Connection:", 11))
		{
			sscanf(tmp, "%*[^:]: %[^,\r\n],%[^,\r\n]", status, status2);

			if (!strcmpi(status, "keep-alive") || !strcmpi(status, "persist"))
				keep_alive = 1;
			
			if (!strcmpi(status2, "keep-alive") || !strcmpi(status2, "persist"))
				keep_alive = 1;
			
			if (!strcmpi(status, "close") || !strcmpi(status2, "close"))
				keep_alive = 0;
		}
		else if (!strncmpi(tmp, "Server:", 6))
		{
			sscanf(tmp, "%*[^:]: %[^\r\n]", status);
			if (!inline_object) current_server = status;
		}
		else if (!strncmpi(tmp, "Location:", 9))
		{
			sscanf(tmp, "%*[^:]: %[^\r\n]", status);
			location = status;
		}
		else if (!strncmpi(tmp, "Last-Modified:", 14))
		{
			sscanf(tmp, "%*[^:]: %[^\r\n]", status);
			http_last_modified = status;
		}
		else if (!strncmpi(tmp, "Date:", 5))
		{
			sscanf(tmp, "%*[^:]: %[^\r\n]", status);
			date = status;
		}
		else if (!strncmpi(tmp, "Expires:", 8))
		{
			sscanf(tmp, "%*[^:]: %[^\r\n]", status);
			http_expires = status;
		}
		else if (!strncmpi(tmp, "Content-Type:", 13))
		{
			sscanf(tmp, "%*[^:]: %[^;\r\n]", status);
			char ext[256];
			sscanf(status, "%*[^/]/%[^;]", ext);
			file_extension = string(".") + ext;

#if defined(wx_msw) && !defined(WIN32)
			if (file_extension.length() > 4)
				file_extension = string(file_extension, 0, 4);
#endif

			http_type = mime_type = status;

#ifdef wx_msw
			if (mime_type == "audio/x-wav")
				file_extension = ".wav";
			else if (mime_type == "image/x-ms-bmp")
				file_extension = ".bmp";
			else if (mime_type == "application/x-msvideo")
				file_extension = ".avi";
#endif
		}
		else if (!strncmpi(tmp, "Content-Length:", 15))
		{
			length_specified = 1;

			//if (http_ver == 1.0F)
			//	keep_alive = 1;			// ??????

			char* src = tmp+15;

			while (*src == ' ')
				src++;

			length = atol(src);
		}
		else if (!strncmpi(tmp, "Pragma: no-cache", 16))
		{
			no_cache_this_one = 1;
		}
		else if ((http_ver > 1.0F) && !strncmpi(tmp, "Cache-Control: no-cache", 23))
		{
			no_cache_this_one = 1;
		}
		else if ((http_ver > 1.0F) && !strncmpi(tmp, "Accept-Ranges:", 14))
		{
			sscanf(tmp, "%*[^:]: %s", status);
			if (!strcmpi(status, "bytes"))
				accept_ranges = 1;
		}
	}

	DoYield();

	if (!length_specified) keep_alive = 0;

	if (keep_alive && !max_conns && (http_ver < 1.1F))
	{
		timeout = CONNECTION_TIMEOUT;
		max_conns = 256;
	}

	if (cancel)
	{
		if (connection)
			connection->busy = 0;

		if (!keep_alive && new_sock)
			delete sock;
		else if (connection)
		{
			delete connection;
			ilist.Delete();
		}

		cancel = 0;
		return 0;
	}

	// Local filename?

	if ((mime_type != "text/plain") &&
		(mime_type != "text/html") &&
#if ALLOW_SITE_MODE
		(mime_type != "text/wsml") &&
#endif
		(mime_type != "text/css") &&
		(mime_type != "image/gif") &&
		(mime_type != "image/x-ms-bmp") &&
#ifdef wx_msw
		(mime_type != "image/xpm") &&
		(mime_type != "image/png") &&
		(mime_type != "image/jpeg") &&
		(mime_type != "image/pjpeg") &&
		(mime_type != "application/x-msvideo") &&
		(mime_type != "audio/x-wav") &&
#else
		(mime_type != "image/pbm") &&
		(mime_type != "image/pcx") &&
		(mime_type != "image/xbm") &&
#endif
		!inline_object)
	{
		localname = SaveAs(filename);

		if (localname.empty())
		{
			return 0;
		}
	}

	// Get a temporary local file...

	if (!localname.length())
	{
		localname = TempFileNameWithExtension
							(
								(cache_path + SEP).data(),
								file_extension.data()
							);
	}

#ifdef wx_msw
	ofstream str(localname.data(), ios::out|ios::binary);
#else
	ofstream str(localname.data(), ios::out);
#endif

	if (str.bad())
	{
		if (connection)
			connection->busy = 0;

		if (!keep_alive && new_sock)
			delete sock;
		else
			sock->Close();

		return 0;
	}

	// Read and copy...

	unsigned long save_length = length;
	unsigned long nbytes = 0;
	time_t started = time(0);
	time_t last = started;
	time_t last_bit = started;

	OnSetStatusText((string("Receiving: ")+filename).data());

	timer = time(0);

	while (!cancel && length)
	{
		status = sock->Wait(https?CONNECTION_TIMEOUT:1);

		if ((time(0) - timer) > CONNECTION_TIMEOUT)
			break;

		if (!status)
		{
			DoYield();
			continue;
		}

		time_t now = timer = time(0);

		// SSLeay sockets have a bug...

		last_bit = now;

		int len = sock->Read(tmp, (int)MinOf(BUFFER_SIZE, length));

		if (len <= 0)
			break;

		if (connection) time(&connection->last);

		length -= len;

		str.write(tmp, len);
		nbytes += len;

		if (now != last)
		{
			time_t elapsed = now - started;

			sprintf(tmp, "%s: %ldK bytes of %ldK (%d%% at %.1fK bps)",
				(filename.length() ? filename.data() : "/"),
				nbytes/1024,
				save_length/1024,
				(int)((nbytes*100)/save_length),
				((8.0*nbytes)/(elapsed?elapsed:1))/1000);

			OnSetStatusText(tmp);
			DoYield();
		}

		last = now;
	}

	str.flush();
	str.close();

	DoYield();

	http_length = nbytes;

	if (connection)
		connection->busy = 0;

	sprintf(tmp, "Received: %.1fK bytes", (float)nbytes/1024);
	OnSetStatusText(tmp);

	log << nbr << ts << "RECEIVED: " << nbytes << " bytes in " << (time(0)-started) << " secs" << endl;

	if (cancel || (length_specified && length))
	{
		if (!cancel)
		{
			sprintf(tmp, "ERROR: left-over bytes = %ld", length);
			OnSetStatusText(tmp);
			log << nbr << ts << tmp << endl << endl;
		}

		remove(localname.data());
		localname = "";

		if (!keep_alive && new_sock)
			delete sock;
		else if (connection)
		{
			delete connection;
			ilist.Delete();
		}

		cancel = 0;
		return 0;
	}

	// Cache the connection?

	if (keep_alive && new_sock)
	{
		connection = new HttpConnection(sock);
		connection->hostname = which_host;
		connection->portname = which_port;
		connection->keep_alive_period = timeout;
		connection->max_conns = --max_conns;
		connection->http_ver = http_ver;
		connection->nbr = nbr;
		connections.Append(connection);
	}

	if (!keep_alive && new_sock)
	{
		log << "-" << ts << "Drop new connection" << endl;
		delete sock;
		sock = 0;
	}

	if (!keep_alive && connection)
	{
		log << connection->nbr << ts << "Drop old connection, why?" << endl;
		delete connection;
		connection = 0;
		ilist.Delete();
	}

	// Do we need authentication?

	if (authentication_needed)
	{
		remove(localname.data());
		localname = "";

		auth_cookie = Authenticate(realm);

		if (auth_cookie.empty())
			return 0;

		goto loop;
	}

	// Redirected?

	//if ((http_code >= 301) && (http_code <= 302))
	if (http_code == 302)
	{
		remove(localname.data());
		localname = "";

		string s, h, p, f;

		if (!ParseURL(location.data(), s, h, p, f, s))
			return 0;

		if (s == "http")
			scheme = PROTO_HTTP;
		else if (s == "https")
			scheme = PROTO_HTTPS;
		else if (s == "ftp")
			scheme = PROTO_FTP;

		return Do(h, p, f, localname, inline_object, post, head, scheme, put, data);
	}

	// Cache the item...

	if (!nocache && !no_cache_this_one && !head)
	{
		AddCache(new_url, localname, http_last_modified, http_expires, not_found, mime_type, head);
	}

	if (!not_found && !inline_object)
	{
		base = url = new_url;
	}

	// Cache any auth_cookie-data...

	if (auth_cookie.length())
	{
		AuthCookie* citem = new AuthCookie;
		citem->url = new_url;
		citem->authentication_scheme = authentication_scheme;
		citem->realm = realm;
		citem->cookie = auth_cookie;
		auth_cookies.Push(citem);
	}

	// Multi-part response?

#if 0
	if (sock && sock->WaitForRead(0))
	{
		wxMessageBox("Multi-part response!");
		goto loop2;
	}
#endif

	return (not_found ? 0 : 1);
}
