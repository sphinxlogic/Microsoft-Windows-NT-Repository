#include <syidef.h>
#include <stdlib.h>
#include <starlet.h>
#include "utsname.h"

int uname(struct utsname *u)
{       static int len1, len2, len3, len4, len5, status;
        long iosb[2];
        struct 
        {   short length, code; 
            char *buffer; 
            int *ret_len; 
        } item[] =
        { {NOSWLEN+1, SYI$_NODE_SWTYPE, NULL, &len1},
          {NONALEN+1, SYI$_NODENAME, NULL, &len2},
          {RELLEN+1, SYI$_VERSION, NULL, &len3},
          {HWNLEN+1, SYI$_HW_NAME, NULL, &len4},
          {SIDLEN+1, SYI$_SID, NULL, &len5},
          {0,0,0,0}
        };

           item[0].buffer = u->sysname;
           item[1].buffer = u->nodename;
	   item[2].buffer = u->release;
	   item[3].buffer = u->machine;
	   item[4].buffer = u->__idnumber;
           status = sys$getsyiw ( 0, 0, 0, &item, iosb, 0, 0 );   
           u->sysname[len1]='\0';
           u->nodename[len2]='\0';
           u->release[len3]='\0';
           u->machine[len4]='\0';
           u->__idnumber[len5]='\0';
           u->version[0]='\0';
           return(0);
}

#ifdef TEST
int main()
{ struct utsname *u;
  u=(struct utsname *)malloc(sizeof(struct utsname));    
  uname(u);
}
#endif
