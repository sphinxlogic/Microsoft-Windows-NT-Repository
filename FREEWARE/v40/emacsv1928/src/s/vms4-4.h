#include "vms.h"
#define VMS4_4

/* VMS does not implement soft links yet...  */
#ifdef S_IFLNK
#undef S_IFLNK
#endif
#ifdef S_ISLNK
#undef S_ISLNK
#endif
