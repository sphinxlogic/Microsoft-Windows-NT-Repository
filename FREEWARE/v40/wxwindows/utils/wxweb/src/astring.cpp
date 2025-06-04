///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  A string class.
//
// Edit History
//
//  Written October 1993 by Andrew Davison.
//
//  Made a bit safer, October 1995 by AD.
//  Use DLE escaping for ' ' & '\n' chararcters, November 1995 by AD.
//  Bring more in line with ANSI 'string' class, April 1996 by AD.
//
///////////////////////////////////////////////////////////////////////////////

#include "astring.h"

#if !ANSI_STRING

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#pragma hdrstop

const DLE = 0x10;

string string::nil("");
const size_t string::npos = (size_t)-1;

string::string(char s)
{
	sref = new stringRef;
	sref->refcnt = 1;
	sref->buf = new char[1+1];
	sref->nbytes = 1;
	sref->buf[0] = s;
	sref->buf[1] = 0;
}

string::string(const char* _s)
{
	const char* s = (_s ? _s : nil.data());

	size_t len = (size_t)strlen(s);
	sref = new stringRef;
	sref->refcnt = 1;
	sref->buf = new char[len+1];
	sref->nbytes = len;
	memcpy(sref->buf, s, len+1);	// +1 gets around memcpy bug
	sref->buf[len] = 0;
}

string::string(const char* _s, size_t len)
{
	const char* s = (_s ? _s : nil.data());

	size_t newlen = (size_t)strlen(s);

	if (newlen > len)
		newlen = len;

	sref = new stringRef;
	sref->refcnt = 1;
	sref->buf = new char[newlen+1];
	sref->nbytes = newlen;
	memcpy(sref->buf, s, newlen);
	sref->buf[newlen] = 0;
}

string::string(const string& s, size_t offset, size_t len)
{
	size_t newlen = s.length();

	if (offset > newlen)
		offset = newlen;

	newlen -= offset;

	if (newlen > len)
		newlen = len;

	sref = new stringRef;
	sref->refcnt = 1;
	sref->buf = new char[newlen+1];
	sref->nbytes = newlen;
	if (s.sref) memcpy(sref->buf, s.sref->buf+offset, newlen);
	sref->buf[newlen] = 0;
}

string::~string()
{
	if (sref)
	{
		if (--sref->refcnt == 0)
		{
			delete [] sref->buf;
			delete sref;
		}

		sref = 0;
	}
}

string& string::operator=(char s)
{
	stringRef* old_sref = sref;

	sref = new stringRef;
	sref->buf = new char[1+1];
	sref->refcnt = 1;
	sref->nbytes = 1;

	sref->buf[0] = s;
	sref->buf[1] = 0;

	if (old_sref)
	{
		if (--old_sref->refcnt == 0)
		{
			delete [] old_sref->buf;
			delete old_sref;
		}
	}

	return *this;
}

string& string::operator=(const char* _s)
{
	const char *s = (_s ? _s : nil.data());

	stringRef* old_sref = sref;

	size_t len = (size_t)strlen(s);
	sref = new stringRef;
	sref->buf = new char[len+1];
	sref->refcnt = 1;
	sref->nbytes = len;
	memcpy(sref->buf, s, len+1);
	sref->buf[len] = 0;

	if (old_sref)
	{
		if (--old_sref->refcnt == 0)
		{
			delete [] old_sref->buf;
			delete old_sref;
		}
	}

	return *this;
}

string& string::operator=(const string& s)
{
	if (sref == s.sref)
		return *this;

	stringRef* old_sref = sref;

	sref = s.sref;
	if (sref) sref->refcnt++;

	if (old_sref)
	{
		if (--old_sref->refcnt == 0)
		{
			delete [] old_sref->buf;
			delete old_sref;
		}
	}

	return *this;
}

string& string::operator+=(const string& s)
{
	stringRef* old_sref = sref;
	size_t len = (s.sref ? s.sref->nbytes : (size_t)0);

	sref = new stringRef;
	sref->refcnt = 1;

	if (old_sref)
	{
		sref->buf = new char[old_sref->nbytes+len+1];
		sref->nbytes = old_sref->nbytes;
		memcpy(sref->buf, old_sref->buf, old_sref->nbytes);

		if (--old_sref->refcnt == 0)
		{
			delete [] old_sref->buf;
			delete old_sref;
		}
	}
	else
	{
		sref->buf = new char[len+1];
		sref->nbytes = 0;
	}

	if (s.sref) memcpy(sref->buf+sref->nbytes, s.sref->buf, len);

	sref->nbytes += len;
	sref->buf[sref->nbytes] = 0;

	return *this;
}

string& string::operator+=(char s)
{
	// Use temp for G++
	string tmp(s);
	return *this += tmp;
}

string& string::operator+=(const char* _s)
{
	const char *s = (_s ? _s : nil.data());

	// Use temp for G++
	string tmp(s);
	return *this += tmp;
}

string string::operator+(char s) const
{
	// Use temp for G++
	string tmp(*this);
	return tmp += s;
}

string string::operator+(const char* s) const
{
	// Use temp for G++
	string tmp(*this);
	return tmp += s;
}

string string::operator+(const string& s) const
{
	// Use temp for G++
	string tmp(*this);
	return tmp += s;
}

string& string::_pad(size_t len, char ch)
{
	if (sref && (sref->nbytes >= len))
		return *this;

	stringRef* old_sref = sref;

	sref = new stringRef;
	sref->buf = new char[len+1];
	sref->refcnt = 1;

	if (old_sref)
	{
		sref->nbytes = old_sref->nbytes;
		memcpy(sref->buf, old_sref->buf, old_sref->nbytes);

		if (--old_sref->refcnt == 0)
		{
			delete [] old_sref->buf;
			delete old_sref;
		}
	}
	else
		sref->nbytes = 0;

	while (sref->nbytes < len)
	{
		sref->buf[sref->nbytes++] = ch;
	}

	sref->buf[sref->nbytes] = 0;

	return *this;
}

string& string::_trim(char ch)
{
	stringRef* old_sref = sref;
	size_t len = sref->nbytes;

	while (len > 0)
	{
		if (sref->buf[len-1] != ch)
			break;

		len--;
	}

	sref = new stringRef;
	sref->buf = new char[len+1];
	sref->refcnt = 1;
	strncpy(sref->buf, old_sref->buf, len);
	sref->buf[len] = 0;
	sref->nbytes = len;

	if (--old_sref->refcnt == 0)
	{
		delete [] old_sref->buf;
		delete old_sref;
	}

	return *this;
}

string& string::_upper()
{
	if (sref->refcnt > 1)
    {
		stringRef* old_sref = sref;

		sref = new stringRef;
		sref->buf = new char[old_sref->nbytes+1];
		sref->refcnt = 1;
		sref->nbytes = old_sref->nbytes;
		memcpy(sref->buf, old_sref->buf, old_sref->nbytes);
		sref->buf[old_sref->nbytes] = 0;

		if (--old_sref->refcnt == 0)
		{
			delete [] old_sref->buf;
			delete old_sref;
		}
	}

	char *tmp = sref->buf;

	while (*tmp)
	{
		*tmp = (char)toupper(*tmp);
		tmp++;
	}

	return *this;
}

string& string::_lower()
{
	if (sref->refcnt > 1)
    {
		stringRef* old_sref = sref;

		sref = new stringRef;
		sref->buf = new char[old_sref->nbytes+1];
		sref->refcnt = 1;
		sref->nbytes = old_sref->nbytes;
		memcpy(sref->buf, old_sref->buf, old_sref->nbytes);
		sref->buf[old_sref->nbytes] = 0;

		if (--old_sref->refcnt == 0)
		{
			delete [] old_sref->buf;
			delete old_sref;
		}
	}

	char* tmp = sref->buf;

	while (*tmp)
	{
		*tmp = (char)tolower(*tmp);
		tmp++;
	}

	return *this;
}

string string::_dq() const
{
	if ((sref->buf[0] != '\"') && (sref->buf[0] != '\''))
		return *this;

	return string(sref->buf, (size_t)1, (size_t)(length()-2));
}

string string::_pack() const
{
	string tmp = "";

	for (size_t i = 0; i < length(); i++)
	{
		if (  (sref->buf[i] == ' ') ||
				(sref->buf[i] == '\r') ||
				(sref->buf[i] == '\n') )
		{
			tmp += DLE;
			tmp += char(sref->buf[i] + 0x80);
		}
		else
			tmp += sref->buf[i];

	}

	return tmp;
}

string string::_unpack() const
{
	string tmp = "";

	for (size_t i = 0; i < length(); i++)
	{
		if (sref->buf[i] == DLE)
		{
			tmp += char(sref->buf[++i] - 0x80);
		}
		else
			tmp += sref->buf[i];
	}

	return tmp;
}

string string::change(char from, char to) const
{
	string s = data();

	char* tmp = s.sref->buf;

	while (*tmp)
	{
		if (*tmp == from) *tmp = to;
		tmp++;
	}

	return s;
}

string string::replace(const char* from, const char* to) const
{
	if (!length() || !from || !to) return *this;

	size_t from_len = strlen(from);

	if (!from_len) return *this;

	const char* tmp = data();
	string s = "";

	while (*tmp)
	{
		if (!strncmp(tmp, from, from_len))
		{
			if (*to) s += to;
			tmp += from_len;
		}
		else
			s += *tmp++;
	}

	return s;
}

size_t string::find(const char* str, size_t offset) const
{
	if (!str || !length()) return npos;
	if (offset >= length()) return npos;

	size_t len = strlen(str);

	if (!len) return npos;

	for (size_t i = offset; i < length(); i++)
	{
		if (len > (length() - i))
			break;

		if (!strncmp(sref->buf+i, str, len))
			return i;
	}

	return npos;
}

size_t string::find(const char* str, size_t offset, size_t len) const
{
	if (!str || !length()) return npos;
	if (offset >= length()) return npos;

	if (!len) return npos;

	for (size_t i = offset; i < length(); i++)
	{
		if (len > (length() - i))
			break;

		if (!strncmp(sref->buf+i, str, len))
			return i;
	}

	return npos;
}

string operator+(char c, const string& s)
{
	// Use temp for G++
	string tmp(c);
	return tmp += s;
}

string operator+(const char* str, const string& s)
{
	// Use temp for G++
	string tmp(str);
	return tmp += s;
}

ostream& operator<<(ostream& ostr, const string& s)
{
	return ostr << s.data();
}

istream& operator>>(istream& istr, string& s)
{
  if (!istr.ipfx(0) || (!(istr.flags() & ios::skipws) && !ws(istr)))
  {
	 istr.clear(ios::failbit|istr.rdstate()); // Redundant if using GNU iostreams.
	 return istr;
  }

  int ch;
  size_t i = 0;
  const BLOCK_SIZE = 256;
  char* block = (char*)malloc(BLOCK_SIZE);
  size_t nbytes = BLOCK_SIZE;

  streambuf *sb = istr.rdbuf();
  while ((ch = sb->sbumpc()) != EOF)
  {
	 if (isspace(ch))
		break;

	 if (i == nbytes)
	 {
		nbytes += (size_t)BLOCK_SIZE;
		block = (char*)realloc(block, nbytes);
	 }

	 block[i++] = (char)ch;
  }

  int new_state = istr.rdstate();
  if (i == 0) new_state |= ios::failbit;
  if (ch == EOF) new_state |= ios::eofbit;
  istr.clear(new_state);

  s = string(block, 0, i);
  free(block);

  return istr;
}

#endif
