/* layer3flip.c
   This incorporates Don Taylor's solution to the 
   Buvos Kocka (Hungarian Magic Cube). 

   Last Layer Cubes

   Flip edge cubes.

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

#include "taylor.h"

extern void F(); /*  front  */
extern void L(); /*  left   */
extern void R(); /*  right  */
extern void B(); /*  back   */
extern void U(); /*  up     */
extern void D(); /*  down   */

Boolean flip(w,wdata,call_data) /* These parameters needed for macros */
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Test if ok */
  if(Front(7) == FRONT &&
     Right(7) == RIGHT &&
     Back(7)  == BACK  &&
     Left(7)  == LEFT)
    return(True);

  /* Here if not ok 
   * Flip corners   
   * There are always an even number of cubes that nedd to be flipped.
   *
   * Look for an edge that needs to be flipped.
   */
  if(Front(7) == BOTTOM){
    F(1);D(1);U(-1);R(2);D(2);U(2);L(1);
    if(Left(7) == BOTTOM){
      L(-1);U(2);D(2);R(2);U(1);D(-1);F(-1);
    }
    else if(Back(7) == BOTTOM){
      D(1);
      L(-1);U(2);D(2);R(2);U(1);D(-1);F(-1);
      D(-1);
    }
    else if(Right(7) == BOTTOM){
      D(2);
      L(-1);U(2);D(2);R(2);U(1);D(-1);F(-1);
      D(2);
    }
    else if(Front(7) == BOTTOM){
      D(-1);
      L(-1);U(2);D(2);R(2);U(1);D(-1);F(-1);
      D(1);
    }
  }
  else if(Right(7) == BOTTOM){
    R(1);D(1);U(-1);B(2);D(2);U(2);F(1);
    if(Front(7) == BOTTOM){
      F(-1);U(2);D(2);B(2);U(1);D(-1);R(-1);
    }
    else if(Left(7) == BOTTOM){
      D(1);
      F(-1);U(2);D(2);B(2);U(1);D(-1);R(-1);
      D(-1);
    }
    else if(Back(7) == BOTTOM){
      D(2);
      F(-1);U(2);D(2);B(2);U(1);D(-1);R(-1);
      D(2);
    }
    else if(Right(7) == BOTTOM){
      D(-1);
      F(-1);U(2);D(2);B(2);U(1);D(-1);R(-1);
      D(1);
    }
  }
  else if(Back(7) == BOTTOM){
    B(1);D(1);U(-1);L(2);D(2);U(2);R(1);
    if(Right(7) == BOTTOM){
      R(-1);U(2);D(2);L(2);U(1);D(-1);B(-1);
    }
    else if(Front(7) == BOTTOM){
      D(1);
      R(-1);U(2);D(2);L(2);U(1);D(-1);B(-1);
      D(-1);
    }
    else if(Left(7) == BOTTOM){
      D(2);
      R(-1);U(2);D(2);L(2);U(1);D(-1);B(-1);
      D(2);
    }
    else if(Back(7) == BOTTOM){
      D(-1);
      R(-1);U(2);D(2);L(2);U(1);D(-1);B(-1);
      D(1);
    }
  }
  else if(Left(7) == BOTTOM){
    L(1);D(1);U(-1);F(2);D(2);U(2);B(1);
    if(Back(7) == BOTTOM){
      B(-1);U(2);D(2);F(2);U(1);D(-1);L(-1);
    }
    else if(Right(7) == BOTTOM){
      D(1);
      B(-1);U(2);D(2);F(2);U(1);D(-1);L(-1);
      D(-1);
    }
    else if(Front(7) == BOTTOM){
      D(2);
      B(-1);U(2);D(2);F(2);U(1);D(-1);L(-1);
      D(2);
    }
    else if(Left(7) == BOTTOM){
      D(-1);
      B(-1);U(2);D(2);F(2);U(1);D(-1);L(-1);
      D(1);
    }
  }
  return(False);
}
