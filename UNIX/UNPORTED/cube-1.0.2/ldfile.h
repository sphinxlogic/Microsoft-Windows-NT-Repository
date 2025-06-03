/* ldfile.h
   Header file for ldfile.c 
   Buvos Kocka (Hungarian Magic Cube). 


The following references were used:
   "The X Window System Programming And Applications with Xt
   OSF/MOTIF EDITION"
   by Douglas A Young 
   Prentice-Hall, 1990.
   ISBN 0-13-642786-3

   "Mastering Rubik's Cube"
   by Don Taylor
   An Owl Book; Holt, Rinehart and Winston, New York, 1980
   ISBN 0-03-059941-5

-------------------------------------------------------------------
Copyright (C) 1993 by Pierre A. Fleurant
Permission is granted to copy and distribute this program
without charge, provided this copyright notice is included
in the copy.
This Software is distributed on an as-is basis. There will be
ABSOLUTELY NO WARRANTY for any part of this software to work
correct. In no case will the author be liable to you for damages
caused by the usage of this software.
-------------------------------------------------------------------
*/   

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h> 
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/FileSB.h> 

#include "common.h"
#include "libXs.h"
#include <stdio.h>

#define DIALOG_SIZE			325
#define DIALOG_X			325
#define DIALOG_Y			325
#define MSG_SIZ				256


