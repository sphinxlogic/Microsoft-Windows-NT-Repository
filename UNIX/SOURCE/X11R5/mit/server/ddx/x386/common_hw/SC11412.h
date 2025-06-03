/* $XFree86: mit/server/ddx/x386/common_hw/SC11412.h,v 2.0 1994/02/25 15:02:15 dawes Exp $ */

/* Norbert Distler ndistler@physik.tu-muenchen.de */

typedef int Bool;
#define TRUE 1
#define FALSE 0
#define QUARZFREQ	        14318
#define MIN_SC11412_FREQ        45000
#define MAX_SC11412_FREQ       100000

Bool SC11412SetClock( 
#if NeedFunctionPrototypes
   long
#endif
);     
