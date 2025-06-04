
#include <string.h>


bcopy( void *src, void *dest, int n)
{

#ifdef NEVER
    /* memcpy doesn't handle overlays; so use memmove */
    memcpy( dest, src, n ); /* note params in diff order to bcopy */
#endif
    memmove( dest, src, n );
}


bzero( void *p, int n )
{
    /* zero memory by setting it's value to 0 (2nd param) */
    memset( p, 0, n );
}

