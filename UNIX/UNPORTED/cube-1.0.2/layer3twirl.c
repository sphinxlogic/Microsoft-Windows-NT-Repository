/* layer3twirl.c
   This incorporates Don Taylor's solution to the 
   Buvos Kocka (Hungarian Magic Cube). 

   Last Layer Cubes

   Twirl corner cubes.


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

Boolean twirl(w,wdata,call_data) /* These parameters needed for macros */
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Test if ok */
  if(Front(6) == FRONT &&
     Right(6) == RIGHT &&
     Back(8)  == BACK  &&
     Left(8)  == LEFT)
    return(True);

  /* Here if not ok */
  /* Twirl corners */
  /* 3 possibilities:
   * 1: one corner needs twirl clockwise and another needs twirl anti-clockwise
   * 2: two corners need twirl clockwise and other two need twirl anti-clockwise
   * 3: three corners need to be twirled the same way
   * 
   * The twirl algorithm twirls one corner clockwise, and another anti-clockwise.
   * This is repeated until all 4 corners are correct.
   */

  /* Find a corner that needs a clockwise twirl.
   * (Note: all corners are in correct position.
   * A corner needs a clockwise twirl if the front matches the top with
   * said cube oriented in a top right-hand position.
   * A corner needs an anti-clockwise twirl if the right-hand face matches the top.)
   */

  if(Front(6) == BOTTOM){
    L(-1);U(1);L(1);F(1);U(1);F(-1);
    /* Now search for cube that needs an anti-clockwise turn. */
    if(Back(6) == BOTTOM){
      D(1);
      F(1);U(-1);F(-1);L(-1);U(-1);L(1);
      D(-1);
    }
    else if(Right(8) == BOTTOM){
      D(2);
      F(1);U(-1);F(-1);L(-1);U(-1);L(1);
      D(2);
    }
    else if(Front(8) == BOTTOM){
      D(-1);
      F(1);U(-1);F(-1);L(-1);U(-1);L(1);
      D(1);
    }
  }
  else if(Right(6) == BOTTOM){
    F(-1);U(1);F(1);R(1);U(1);R(-1);
    if(Left(6) == BOTTOM){
      D(1);
      R(1);U(-1);R(-1);F(-1);U(-1);F(1);
      D(-1);
    }
    else if(Back(6) == BOTTOM){
      D(2);
      R(1);U(-1);R(-1);F(-1);U(-1);F(1);
      D(2);
    }
    else if(Right(8) == BOTTOM){
      D(-1);
      R(1);U(-1);R(-1);F(-1);U(-1);F(1);
      D(1);
    }
  }
  else if(Back(8) == BOTTOM){
    R(-1);U(1);R(1);B(1);U(1);B(-1);
    if(Front(8) == BOTTOM){
      D(1);
      B(1);U(-1);B(-1);R(-1);U(-1);R(1);
      D(-1);
    }
    else if(Left(6) == BOTTOM){
      D(2);
      B(1);U(-1);B(-1);R(-1);U(-1);R(1);
      D(2);
    }
    else if(Back(6) == BOTTOM){
      D(-1);
      B(1);U(-1);B(-1);R(-1);U(-1);R(1);
      D(1);
    }
  }
  else if(Left(8) == BOTTOM){
    B(-1);U(1);B(1);L(1);U(1);L(-1);
    if(Right(8) == BOTTOM){
      D(1);
      L(1);U(-1);L(-1);B(-1);U(-1);B(1);
      D(-1);
    }
    else if(Front(8) == BOTTOM){
      D(2);
      L(1);U(-1);L(-1);B(-1);U(-1);B(1);
      D(2);
    }
    else if(Left(6) == BOTTOM){
      D(-1);
      L(1);U(-1);L(-1);B(-1);U(-1);B(1);
      D(1);
    }
  }
  else{
    /* Here if not ok and no corners need clockwise twirl.
     * So this is case 3 with three corners needing an
     * anti-clockwise twirl.
     * 
     * So we perform operations to create atleast 1 corner
     * needing a clockwise twirl, then return false.
     * Operation is from "Front(6) == BOTTOM/Back(6) == BOTTOM" case.
     */
    L(-1);U(1);L(1);F(1);U(1);F(-1);
    D(1);
    F(1);U(-1);F(-1);L(-1);U(-1);L(1);
    D(-1);
  }
  return(False);
}
