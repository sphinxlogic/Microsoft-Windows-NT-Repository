/**********************************************************************************
 * init_cubik.h: declarations for the cube program's init_cubik.c
 *The following references were used:
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
#include <Xm/Xm.h>
#include <X11/Shell.h>
#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/BulletinB.h>
#include <Xm/RowColumn.h>
#include "libXs.h"

/* message in messageBox of popup window */
char *popup_str[] = {
  "   To init cube pick color from palette, pick a side,     ",
  "   then click on cube.   ",
  "   YOU CAN'T CHANGE CENTER REFERENCE SQUARE.",
  "   -----------------------------------------",
  "    \\top  /                   \\top  /",
  "     -----                      -----",
  "left|front|right           back|left |front",
  "     -----                      -----",
  "    /bottom\\                  /bottom\\",
  " ",
  "    \\ top  /                  \\ top  /",
  "      -----                      ----",
  "front|right|back           right|back|left",
  "      -----                      ----",
  "     /bottom\\                  /bottom\\",
  " ",
  "    \\front/                   \\front/",
  "      ---                       ------",
  "right|top|left             left|bottom|right",
  "      ---                       ------",
  "     /back\\                   /back \\",
  "",""};

char *frontref_str[] = {
  "",""};

/* forward declarations */
void xcic_done_callback();
void xcic_refresh();

extern void set_fill_pattern();
extern void set_cube_side();
extern void align_subfaces();

extern *seed_color[];
extern *side_names[];

extern Boolean Cube_Side_Chosen;
extern Boolean Fill_Pattern_Chosen;

