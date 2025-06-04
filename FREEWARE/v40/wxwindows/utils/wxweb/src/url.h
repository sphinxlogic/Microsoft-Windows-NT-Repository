#ifndef URL_H
#define URL_H

extern int ParseURL(const char* src, string& scheme, string& h, string& p, string& f, string& s);
extern string AbsoluteURL(const string& base, const string& url);
extern int GetAttributeValuePair(const char*& src, string& p1, string& p2);
	
#endif
