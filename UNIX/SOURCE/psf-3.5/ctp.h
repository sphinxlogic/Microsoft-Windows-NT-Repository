#include <ctype.h>
#ifndef tolower
#define tolower(X) ((X)-'A'+'a') /* XXX ASCII */
#endif
#ifndef toupper
#define toupper(X) ((X)-'a'+'A') /* XXX ASCII */
#endif
#define UCCHAR(X) ((isascii(X) && islower(X))?toupper(X):(X))
#define LCCHAR(X) ((isascii(X) && isupper(X))?tolower(X):(X))
