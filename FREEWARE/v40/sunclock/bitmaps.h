/*
 * bitmaps for sun clock, X11 version; header file
 */

#ifdef	DEFINE_BITS
#define	EXTERN
#else
#define	EXTERN	extern
#endif

#define icon_map_width 126
#define icon_map_height 63
EXTERN char icon_map_bits[]
#ifdef	DEFINE_BITS
#include "icon_bits.h"
#else
;
#endif

#define large_map_width 640
#define large_map_height 320
EXTERN char large_map_bits[] 
#ifdef	DEFINE_BITS
#include "large_bits.h"
#else
;
#endif
