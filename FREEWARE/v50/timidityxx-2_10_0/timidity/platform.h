/*
 *  Multimedia services -- platform-specific definitions.
 *
 *  This header file contains any platform-specific definitions common
 *  to many modules.
 *
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/* #define __SOCKET_TYPEDEFS 1	*/

#ifdef __VMS

typedef unsigned long   	Uint64; 
typedef unsigned long 		CARD64;
typedef unsigned long 		POINTER; 
typedef unsigned long		BITS64;

typedef unsigned int	        Uint32;     
typedef unsigned int 		CARD32;
typedef unsigned int		BITS32;
typedef Uint32      	    	DWORD;
typedef Uint32        	    	UINT;

typedef unsigned short		Uint16;
typedef unsigned short 		CARD16;
typedef unsigned short		BITS16;
typedef unsigned short 		Uchar16;

typedef unsigned char 	   	BOOL;
typedef unsigned char 		Uint8;
typedef unsigned char 		Uchar8;
typedef unsigned char  		CARD8;
typedef unsigned char		BYTE;

typedef char 			INT8;
typedef char 			Int8;
typedef char 			Char8;
typedef Char8 		    	CHAR;

typedef short 			Int16;
typedef short 			Char16;
typedef short          		INT16;
typedef Int16 		    	SHORT;
typedef Uint16      	    	WORD;

typedef int  			Int32;
typedef int  			INT32;
typedef Int32               	INT;

typedef long			Int64;
typedef Int64               	LONG;

typedef float               	FLOAT;


#ifdef dont_use
typedef UINT                WPARAM;
typedef LONG                LPARAM;
typedef UINT                MMRESULT;
#endif

/*
 *  These two are defined in mme/cmmfuncs.h, slightly
 *  differently than here.  We'll use the one in cmmfuncs.h
#define bcopy(src,dst,len) memcpy((dst),(src),(len))
#define bzero(dst,len) memset((dst),0,(len))
*/

#define USE_STDIO 1

/*
 *  The following is required to make sure we get 6 4-bit longs if
 *  we're on OpenVMS AXP.  The OpenVMS version of limits.h does not define
 *  LONG_BIT, which various things depend on.
 */
#ifdef __ALPHA
#define LONG_BIT 64
#endif /* __ALPHA */

#endif  /* __VMS */

#endif  /* _PLATFORM_H_ */
