#ifndef vms
#ifndef NoDIRENT
#include <dirent.h>
#else
#include <sys/dir.h>
#ifndef dirent
#define dirent direct
#endif
#endif
#else
#include "vms.h"
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

/*
  Utility define declarations.
*/
#ifndef vms
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL) || \
   (strchr(text,'[') != (char *) NULL) || \
   (strchr(text,']') != (char *) NULL))
#define BasenameSeparator  "/"
#define DirectorySeparator  "/"
#define SystemCommand(command)  system(command)
#define TemporaryDirectory  "/tmp"
#define TemporaryTemplate  "%s/magickXXXXXX"
#else
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL))
#define BasenameSeparator  "]"
#define DirectorySeparator  ""
#define SystemCommand(command)  (!system(command))
#define TemporaryDirectory  "SYS$Disk:[]"
#define TemporaryTemplate  "%smagickXXXXXX."
#endif

/*
  Utilities routines.
*/
extern char
  **StringToList _Declare((char *)),
  **ListColors _Declare((char *,int *)),
  **ListFiles _Declare((char *,char *,int *));

extern int
  GlobExpression _Declare((char *,char *)),
  ReadDataBlock _Declare((char *,FILE *));

extern unsigned int
  ReadData _Declare((char *,int,int,FILE *));

extern unsigned long
  LSBFirstReadLong _Declare((FILE *)),
  MSBFirstReadLong _Declare((FILE *));

extern unsigned short
  LSBFirstReadShort _Declare((FILE *)),
  MSBFirstReadShort _Declare((FILE *));

extern void
  LSBFirstWriteLong _Declare((unsigned long,FILE *)),
  LSBFirstWriteShort _Declare((unsigned int,FILE *)),
  MSBFirstOrderLong _Declare((char *,unsigned int)),
  MSBFirstOrderShort _Declare((char *,unsigned int)),
  MSBFirstWriteLong _Declare((unsigned long,FILE *)),
  MSBFirstWriteShort _Declare((unsigned int,FILE *)),
  TemporaryFilename _Declare((char *));
