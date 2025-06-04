#include <errno.h>
#include <descrip.h>
#include <rmsdef.h>

/*
  Typedef declarations.
*/
struct dirent
{
  char
     d_name[255];

  int 
    d_namlen;
};

typedef struct _dirdesc
{
  long
    context;

  char
    *pattern;

  struct dirent
    entry;

  struct dsc$descriptor_s
    pat;
} DIR;

/*
  VMS utilities routines.
*/
extern void
  closedir();

extern DIR
  *opendir();

extern struct dirent
  *readdir();
