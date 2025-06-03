/*
 * Emulation of misc. ANSI C stdio functions
 */

/* $Header */

#include <stdio.h>

#if 1
int
remove(name)
	Const char *name;
{
	return unlink(name);
}
#endif

#if _V7
int
rename(oname, nname)
	Const char *oname, *nname;
{
	return link(oname, nname) == 0 && unlink(oname) == 0 ? 0 : -1;
}
#endif

