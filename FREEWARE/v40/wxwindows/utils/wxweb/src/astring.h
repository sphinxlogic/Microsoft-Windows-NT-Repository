///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  A simple string class. Strings can be of any (i.e. unlimited) length up
//  to the limit of 'size_t' on a machine.
//
// Edit History
//
//      Written October 1993 by Andrew Davison.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ASTRING_H
#define ASTRING_H

#ifndef ANSI_STRING
#define ANSI_STRING 0
#endif

#if ANSI_STRING

#include <string>

#else

#include <string.h>
#include <iostream.h>
#include <limits.h>

class string
{
private:

	static string nil;

	struct stringRef
	{
		size_t refcnt;
		size_t nbytes;
		char* buf;
	}
	  *sref;

public:

	string() { sref = 0; };
	string(char);
	string(const char*);
	string(const char*, size_t len);
	string(const string&, size_t offset, size_t len);
	string(const string& s);
	~string();

	size_t length() const { return (size_t)(sref ? sref->nbytes : 0); };
	int empty() const { return length() == 0; };

	static size_t max_size() { return UINT_MAX-1; };
	static char eos() { return 0; };
	static const size_t npos;


	// Assignment...

	string& operator=(char);
	string& operator=(const char*);
	string& operator=(const string&);

	// Concatenation...

	string& operator+=(char);
	string& operator+=(const char*);
	string& operator+=(const string&);

	// Addition...

	string operator+(char) const;
	string operator+(const char*) const;
	string operator+(const string&) const;

	// Equality...

	int operator==(char* s) const;
	int operator==(const char*) const;
	int operator==(const string&) const;

	int operator!=(char* s) const;
	int operator!=(const char*) const;
	int operator!=(const string&) const;

	// Maintenance (note: these operate on the string itself)...

	string& _pad(size_t len, char ch = ' ');
	string& _trim(char ch = ' ');
	string& _upper();
	string& _lower();

	// Misc...

	string _dq() const;
	string _pack() const;
	string _unpack() const;

	// Sub-string operations...

	string change(char from, char to) const;
	string replace(const char* from, const char* to) const;

	size_t find(const string& s, size_t offset, size_t len) const
		{ return find(s.data(), offset, len); };
	size_t find(const string& s, size_t offset = 0) const
		{ return find(s.data(), offset); };

	size_t find(const char* str, size_t offset, size_t len) const;
	size_t find(const char* str, size_t offset = 0) const;

	// Cast...

	const char* data() const { return (sref ? sref->buf : ""); };

	// Access...

	char operator [] (int n) const
		{ return (sref ? ((size_t)n < sref->nbytes ? sref->buf[n] : eos()) : eos()); };

	char operator [] (size_t n) const
		{ return (sref ? (n < sref->nbytes ? sref->buf[n] : eos()) : eos()); };

	char& operator [] (int n)
		{ return sref->buf[n]; };

	char& operator [] (size_t n)
		{ return sref->buf[n]; };


	// Output...

	friend ostream& operator<<(ostream&, const string&);
	friend istream& operator>>(istream&, string&);

	// Concatenation...

	friend string operator+(char, const string&);
	friend string operator+(const char*, const string&);

#ifdef WXSTRING_COMPATIBLE

	// WXWINDOWS compatability...

	string& Assign(const string& s) { return *this = s; };
	const char* Chars() const { return data(); };
	size_t Length() const { return length(); };
	string At(size_t offset, size_t len) const { return mid(offset, len); };
	char Lastchar() const { return lastchar(); };
	int Contains(const char* str) const { return find(str); };

#endif
};

inline string::string(const string& s)
{
	sref = s.sref;
	if (sref) sref->refcnt++;
}

inline int string::operator==(const char* s) const
{
	return sref && !strcmp(sref->buf, s);
}

inline int string::operator==(char* s) const
{
	return *this == (const char*)s;
}

inline int string::operator==(const string& s) const
{
	if (sref == s.sref)
		return 1;
	else if (!sref || !s.sref)
		return 0;
	else if (sref->nbytes != s.sref->nbytes)
		return 0;
	else
		return !strcmp(sref->buf, s.sref->buf);
}

inline int string::operator!=(const char* s) const
{
	return !sref || strcmp(sref->buf, s);
}

inline int string::operator!=(char* s) const
{
	return *this != (const char*)s;
}

inline int string::operator!=(const string& s) const
{
	if (sref == s.sref)
		return 0;
	else if (!sref || !s.sref)
		return 1;
	else if (sref->nbytes != s.sref->nbytes)
		return 1;
	else
		return strcmp(sref->buf, s.sref->buf);
}

#endif

#endif
