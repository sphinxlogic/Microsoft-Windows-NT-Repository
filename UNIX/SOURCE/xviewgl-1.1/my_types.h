#ifndef MY_TYPES_INCLUDED
#define MY_TYPES_INCLUDED
#include "patchlevel.h"

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

#ifndef PATHNAME_MAX
#define PATHNAME_MAX 1024
#endif

#ifdef FILENAME_MAX
#if (PATHNAME_MAX > FILENAME_MAX)
#undef FILENAME_MAX
#define FILENAME_MAX PATHNAME_MAX
#endif
#else
#define FILENAME_MAX PATHNAME_MAX
#endif

#ifndef VOID_POINTERS
#define void int
#endif

#ifndef STDLIB
long	atol();
void	*calloc();
void	free();
void	*malloc();
void	*realloc();
#endif

#ifndef FGETPOS
typedef long fpos_t;
#define fgetpos(fp,pos) ((((*(pos))=ftell(fp))==-1)?1:0)
#define fsetpos(fp,pos) (fseek(fp,*(pos),0))
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef HAS_ERRNO
extern int errno;
#endif

#ifndef HAS_STRERROR
extern char *sys_errlist[];
#define strerror(n) sys_errlist[n]
#endif

#ifdef VMS
#define fork vfork
#endif

typedef int BOOLEAN;
typedef unsigned char byte;
typedef char *MyXPointer;	/* Because XPointer isn't available yet. */
#endif
