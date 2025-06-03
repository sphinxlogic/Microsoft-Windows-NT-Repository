/* libXaw: ChkShlibRev.c
 *
 * This code is linked to the clients and is called by their .init sections.
 * It checks the SVR3 target shared library revision.
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/ChkShlibRev.c,v 1.2 1993/03/27 09:09:42 dawes Exp $
 */

#include <stdio.h>

#ifdef SVR3SHLIB
#ifdef __STDC__
#define Const const
#else
#define Const /**/
#endif

extern Const char *_shared_libXaw_version;

void __check_libXaw_rev_()
{

  /* check revision
   */

  if(strcmp(_shared_libXaw_version, SOXAWREV))
	fprintf(stderr, "Warning: %s has other revision (%s) than expected"
			" %s\n", TARGET, _shared_libXaw_version, SOXAWREV);

}
#endif

