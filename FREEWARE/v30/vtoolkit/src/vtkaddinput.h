/* vtkaddinput.h - vtkaddinput.c interface

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains routines that support XtAddInput, turning VMS ASTs
into toolkit-compatible callback routines.

MODIFICATION HISTORY:

03-Sep-1993 (sjk/nfc) VToolkit V2.

06-Nov-1991 (sjk) Cleanup.

06-Sep-1990 (sjk) Initial entry.
*/

#ifndef _VtkAddInput_h
#define _VtkAddInput_h

#define VTK_MISC_EFN 	2		/* use for system service calls */

typedef void VtkAddInputProc();

extern void VtkCompletionAst(Opaque closure);
Opaque VtkAllocateAddInputRec(VtkAddInputProc *routineP, Opaque closure);

#endif /* _VtkAddInput_h */
