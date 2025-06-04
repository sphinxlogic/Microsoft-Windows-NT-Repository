/*
 * Facility:	LIBCMU
 *
 * Abstract:	Network to/from host short and long conversions
 *
 * Modifications:
 *	05-Nov-2000 T. Dickey
 *		change unsigned-short values to __in_addr_t
 *	16-SEP-1993 mlo 1.0.0
 *		original
 */
#ifdef VAXC
#module NTOH_HTON "v1.0.0"
#endif

#include "[-]socketprv.h"

__in_addr_t ntohl(__in_addr_t x)
{
    return(( ((x) >> 24)& 0x000000ff ) | ( ((x) >> 8) & 0x0000ff00 ) |
	   ( ((x) << 8) & 0x00ff0000 ) | ( ((x) << 24)& 0xff000000 ));
}

__in_port_t ntohs(__in_port_t x)
{
    return((((x) >> 8) | ((x) << 8)) & 0xffff);
}

__in_addr_t htonl(__in_addr_t x)
{
    return(( ((x) >> 24)& 0x000000ff ) | ( ((x) >> 8) & 0x0000ff00 ) |
	   ( ((x) << 8) & 0x00ff0000 ) | ( ((x) << 24)& 0xff000000 ));
}

__in_port_t htons(__in_port_t x)
{
    return((((x) >> 8) | ((x) << 8)) & 0xffff);
}
