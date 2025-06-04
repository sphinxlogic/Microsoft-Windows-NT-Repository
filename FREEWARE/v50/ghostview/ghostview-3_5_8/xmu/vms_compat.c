#include <string.h>

void
vms_bcopy(char *from,char *to,long length)
{
   memcpy(to,from,(size_t)length);
}


void
vms_bzero(char *data,long length)
{
   memset(data,0,(size_t)length);
} 

