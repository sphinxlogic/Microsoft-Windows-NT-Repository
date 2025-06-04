
/**********************************************************************/
/*-->getlogin*/
#include <unixlib.h>
char*
getlogin()
{
    return ((char *)getenv("USER")); /* use equivalent VMS routine */
}
/*********************************************************************/
