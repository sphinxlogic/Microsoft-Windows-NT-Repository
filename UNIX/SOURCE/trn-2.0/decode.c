/* $Id: decode.c,v 1.2 92/01/11 16:04:12 usenet Exp $
 *
 * $Log:	decode.c,v $
 * Revision 1.2  92/01/11  16:04:12  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4.3.1  1991/11/22  04:12:25  davison
 * Trn Release 2.0
 *
 */

#include "EXTERN.h"
#include "common.h"
#include "INTERN.h"
#include "decode.h"

void
decode_init()
{
    unship_init();
}

void
decode_end()
{
    if (decode_fp != Nullfp) {
	fclose(decode_fp);
	decode_fp = Nullfp;
	printf("\n%s INCOMPLETE -- removed.\n", decode_dest) FLUSH;
	unlink(decode_dest);
    }
}
