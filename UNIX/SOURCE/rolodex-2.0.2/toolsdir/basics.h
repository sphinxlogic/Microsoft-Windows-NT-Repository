#define T 1
#define F 0

#define True 1
#define False 0

#define SUCCESS 1
#define FAILURE 0
#define ERROR -1

#define Bool int

#ifdef VMS
#ifndef MAXINT
#define MAXINT 2147483647
#endif

#ifndef MAXINTSTR
#define MAXINTSTR "2147483647"
#endif
#endif


#ifdef UNIX
#ifndef MAXINT
#define MAXINT 2147483647
#endif

#ifndef MAXINTSTR
#define MAXINTSTR "2147483647"
#endif
#endif

#ifdef  MSDOS
#ifndef MAXINTSTR
extern char *itoa();
extern char maxintstr[];

#define MAXINT (~(1 << ((sizeof(int) * 8 - 1))))
#define MAXINTSTR ((*maxintstr=='\0')?itoa(MAXINT,maxintstr,10):maxintstr)
#endif
#endif
