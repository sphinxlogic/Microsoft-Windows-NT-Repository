#include <stdio.h>
#include "gscdbd.h"

double version = 1.4;	/* must be writeable using %.1f */

/* 1.4  10/5	allow more free-form header format
 *      12/7	add -l option.
 * 1.3	8/16	fix fov limiting; send End message when finished; lint cleanups.
 * 1.2  8/2	mimic the cd-rom's subdirectory structure.
 * 1.1	5/31	change some defaults and improve help message.
 * 1.0	5/2/95	begin work
 *	5/3	unannounced trail release.
 */
