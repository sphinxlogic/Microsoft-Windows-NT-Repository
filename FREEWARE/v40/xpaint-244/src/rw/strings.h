 /*
 * External function definitions for string routines
 */
#ifndef _STRING_H
#define _STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h> /* for size_t */

char	*strcat();
char	*strncat();
int	strcmp();
int	strncmp();
char	*strcpy();
char	*strncpy();
size_t	strlen();
char	*index();
char	*rindex();
char    *strerror ();


#ifdef __cplusplus
}
#endif
#endif _STRING_H
