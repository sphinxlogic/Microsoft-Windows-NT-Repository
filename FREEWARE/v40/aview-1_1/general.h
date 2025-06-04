/* General module include file */

#ifndef _GENERAL_H
#define _GENERAL_H

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef VMS
#include <termios.h>
#endif
#include <unistd.h>
#ifndef VMS
#include <sys/ioctl.h>
#else
#if (__DECC_VER >= 50200000)
#include <sys/ioctl.h>
#endif
#endif

#ifndef uint
#define uint   unsigned int
#define ulong  unsigned long
#define ushort unsigned short int
#define uchar  unsigned char
#endif

#ifndef abs
#define abs(x)	(((x)>0)? (x) : (0-(x)))
#endif

long freadblock( FILE *f, long size, void *target );
	/* reads size bytes from file f to target
	returns 1 on success, 0 on error
	*/
	
void echo( void *source, long size ); 
	/* echos size bytes taken from source to the std output this way:
	000 001 002 000 255 .......
	*/

int getkey( void );
	/* returns a character from the stdin stream, returning 0 if no 
	key was pressed
	*/

#endif

