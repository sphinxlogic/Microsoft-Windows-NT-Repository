/*
 * Cheap imitation of BSD dup2()
 */

#include <fcntl.h>

#if _SYSV
int
dup2( oldd, newd )
int	oldd, newd;
{
	int	fd;

	if (fcntl( oldd, F_GETFL, 0 ) < 0)
		return( -1 );

	(void) close( newd );
	fd = fcntl( oldd, F_DUPFD, newd );

	return( (fd > newd) ? -1 : fd );
}
#endif
