#include <descrip.h>

#define MAXARRAY 32767

typedef struct dsc$descriptor string;

#define DYNAMIC_STRING(a) string a = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0}

#define SLEN(a) (a).dsc$w_length
#define SPTR(a) (a).dsc$a_pointer
#define PLEN(a) (a)->dsc$w_length
#define PPTR(a) (a)->dsc$a_pointer

typedef struct _ascic
{
  unsigned char length;
  char text[MAXARRAY];
} ascic;
