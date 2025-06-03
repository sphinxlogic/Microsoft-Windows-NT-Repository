/* xtimes.h */

#include <time.h>
typedef unsigned int    u_int;	/* Need it unsigned for large enough space */

struct tms {
	clock_t tms_utime;	/* user time */
	clock_t tms_stime;	/* system time */
	clock_t tms_cutime;	/* user time,children */
	clock_t tms_cstime;	/* system time,children */
};

u_int xtimes(struct tms buffer);

