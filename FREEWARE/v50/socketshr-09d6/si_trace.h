#ifdef TRACE

#ifndef VAXC
#define noshare
#endif

extern noshare FILE *ftrace__;
extern noshare int trace__;
extern noshare int h_errno;

#ifndef FTRACE__
#define FTRACE__ ftrace__
#endif

#define STRACE(str,flag,s,fmt,a1,a2) _strace(str,flag,s,fmt,(char *)a1,(char *)a2)
#define FTRACE(str,flag,f,fmt,a1,a2) _ftrace(str,flag,f,fmt,(char *)a1,(char *)a2)
#define DTRACE(str,flag,fmt,a1,a2) _dtrace(str,flag,fmt,(char *)a1,(char *)a2)

#define DMPHX(flag,msg,len) \
	if ( (trace__&2) || (trace__ == -1) || flag) { \
		_dmphx(FTRACE__,msg,len); \
}

void _strace(char *, int, int, char *, char *, char *);
void _ftrace(char *, int, FILE *, char *, char *, char *);
void _dtrace(char *, int, char *, char *, char *);
void _dmphx(FILE *, char *, int);

#else

#define STRACE(str,flag,s,fmt,a1,a2)
#define FTRACE(str,flag,f,fmt,a1,a2)
#define DTRACE(str,flag,fmt,a1,a2)
#define DMPHX(flag,msg,len)

#endif
/* XTRACE is a no-op. simple way to disable one of the other. */
#define XTRACE(a,b,c,v,d) /**/

#define INTOA(a) inet_ntoa(*(struct in_addr *)a)
