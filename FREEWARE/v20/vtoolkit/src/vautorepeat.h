/* vautorepeat.h - interface module for vautorepeat.c

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

MODIFICATION HISTORY:

31-Aug-1993 (sjk/nfc) Initial entry
*/

#ifndef _VAutoRepeat_h
#define _VAutoRepeat_h

void VAutoRepeatEnable(Widget, int, int, int, int);
void VAutoRepeatInitializeForMRM(void);

#define VAutoRepeatInitializeForDRM	VAutoRepeatInitializeForMRM

#endif  /* _VAutoRepeat_h */
