///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//   Implement URL parsing etc.
//
// Edit History
//
//	  Started November 25th 1995 by Andrew Davison.
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "astring.h"
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

///////////////////////////////////////////////////////////////////////////////
// URL format... scheme://host:port/file#section
///////////////////////////////////////////////////////////////////////////////

int ParseURL(const char* src, string& scheme, string& hostname, string& portname, string& filename, string& section)
{
	const char* ptr = src;

	if (!*src)
	{
		scheme = hostname = portname = filename = section = "";
		return 0;
	}

	while (*src && (*src != ':') && (*src != '/') &&
				(*src != '#') && (*src != '?'))
		src++;

	// Get the scheme...

	if (src[0] == ':')
	{
		scheme = string(ptr, (size_t)(src-ptr));
		src++;
	}
	else
	{
		src = ptr;
		scheme = "";
	}

	// Get the host...

	if ((src[0] == '/') && (src[1] == '/'))
	{
		src++; src++;

		const char* tmp;

		if ((tmp = strchr(src, ':')) != 0)
		{
			if (tmp != src)
				hostname = string(src, (size_t)(tmp-src));
			else
				hostname = "";

			src = tmp;
			src++;

			if ((tmp = strchr(src, '/')) != 0)
			{
				portname = string(src, (size_t)(tmp-src));
				src = tmp;
				src++;
			}
			else
			{
				portname = src;
				filename = section = "";
				return 1;
			}
		}
		else if ((tmp = strchr(src, '/')) != 0)
		{
			if (tmp != src)
				hostname = string(src, (size_t)(tmp-src));
			else
				hostname = "";

			portname = "";
			src = tmp;
			src++;
		}
		else
		{
			hostname = src;
			portname = filename = section = "";
			return 1;
		}
	}
	else
	{
		hostname = portname = "";
	}

	if ((ptr = strchr(src, '#')) != 0)
	{
		filename = string(src, (size_t)(ptr-src));
		src = ++ptr;
		section = src;
	}
	else
	{
		filename = src;
		section = "";
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

// Return just the directory, or NULL if no directory
static char* PathOnly (const char *path)
{
  if (path && *path)
    {
      static char buf[256];

      // Local copy
      strcpy(buf, path);

      int l = strlen(path);
      int done = 0;

      int i = l - 1;

      // Search backward for a backward or forward slash
      while (!done && i > -1)
      {
        if (path[i] == '/' || path[i] == '\\')
        {
          done = 1;
          buf[i] = 0;
          return buf;
        }
        else i--;
      }

#ifdef _Windows
      // Try Drive specifier
      if (isalpha (buf[0]) && buf[1] == ':')
	{
	  // A:junk --> A:. (since A:.\junk Not A:\junk)
	  buf[2] = '.';
	  buf[3] = '\0';
	  return buf;
	}
#endif
    }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

string AbsoluteURL(const string& base, const string& url)
{
	string scheme, hostname, portname, filename, section;
	string tmpname;

#ifdef wx_msw
	string u = url.change('\\', '/');
	//Dos2UnixFilename((char*)u.data());
#else
	string u = url;
#endif

	if (!ParseURL(u.data(), scheme, hostname, portname, filename, section))
	{
		//OnSetStatusText("Invalid URL specified");
		return u;
	}

	if (scheme.length())
		return u;

	string t1, t2;

	if (!ParseURL(base.data(), scheme, hostname, portname, t1, t2))
		return u;

	// Is it using relative addressing?

	if (string(u)[0] == '/')
	{
		filename = string(filename, 1, filename.length()-1);
	}
	else
	{
		if ((string(u)[0] != '~') && (1||hostname.length()))
		{
			char* tmp_path = PathOnly(t1.data());

			if (tmp_path)
			{
				char buf[1024];
				strcpy(buf, tmp_path);

				loop:

				if (string(filename, 0, 3) == "../")
				{
					char* ptr;

					if ((ptr = strrchr(buf, '/')) == 0)
						ptr = buf;

					*ptr = 0;

					filename = string(filename, 3, filename.length()-3);
					goto loop;
				}

				strcat(buf, "/");
				strcat(buf, (filename.length()&&(filename[0]=='/')?(filename.data())+1:filename.data()));
				filename = buf;
			}
		}
	}

	char tmp_url[1024];

	sprintf(tmp_url, "%s:%s%s%s%s/%s%s%s",
					scheme.data(),
					(strcmpi(scheme.data(), "mailto") ? "//" : ""),
					hostname.data(),
					(portname.length() ? ":" : ""),
					portname.data(),
					(filename[0] == '/'?filename.data()+1:filename.data()),
					(section.length() ? "#" : ""),
					section.data()
					);

	return string(tmp_url);
}

///////////////////////////////////////////////////////////////////////////////
// attr1=value1 attr2=value2 ...
///////////////////////////////////////////////////////////////////////////////

int GetAttributeValuePair(const char*& src, string& p1, string& p2)
{
	int len1, len2;

	// Eat whitespace
	while (*src == ' ')
		src++;

	if (!*src) { p1 = p2 = ""; return 0; };

	len1 = 0;
	const char* ptr1 = src;

	// Attribute value pairs can be space or comma delimited...

	while (*src && (*src != '=') && (*src != ' ') && (*src != ','))
	{
		len1++; src++;
	}

	if (len1)
		p1 = string(ptr1, len1);
	else
		p1 = "";

	if (!*src) { p2 = ""; return 1; };

	while ( *src && ((*src == ' ') || (*src == ',')))
		src++;

	if (!*src || (*src != '=')) return 1;

	while ( *src && ((*src == ' ') || (*src == '=')) )
		src++;

	if (!*src) { p2 = ""; return 1; };

	int quoted = 0;

	if (*src == '\"')
	{
		quoted = 1;
		src++;						// skip the leading quote
	}

	len2 = 0;
	const char* ptr2 = src;

	while ( *src && (*src != (quoted?'\"':' ')) )
	{
		len2++; src++;
	}

	if (*src == '\"')
		src++;			// skip the trailing quote

	// Eat whitespace
	while ((*src == ' ') || (*src == ','))
		src++;

	if (len2)
		p2 = string(ptr2, len2);
	else
		p2 = "";

	return 1;
}

