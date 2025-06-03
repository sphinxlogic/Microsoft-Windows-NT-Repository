/* layer1c_txx.c
   This incorporates Don Taylor's solution to the 
   Buvos Kocka (Hungarian Magic Cube). 

   First Layer Corner Cube top/front/right
----------------------------------------------------------
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

void first_layer_corner_tfr(w,wdata,call_data) /* These parameters needed for macros */
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Top/Front/Right
   * The src cube in bottom layer is directly below
   * the destination cube in the top layer.
   * Operation depends where the TOP face is in the src cube.
   */
  
  if(Front(8) == TOP){
    F(1);D(1);F(-1);
    return;
  }
  else if(Right(6) == TOP){
    R(-1);D(-1);R(1);
    return;
  }
  else if(Bottom(2) == TOP){
    R(-1);D(1);R(1);D(2);
    R(-1);D(-1);R(1);
    return;
  }
}

void first_layer_corner_trb(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Top/Right/Back
   * The src cube in bottom layer is directly below
   * the destination cube in the top layer.
   * Operation depends where the TOP face is in the src cube.
   */
  
  if(Right(8) == TOP){
    R(1);D(1);R(-1);
    return;
  }
  else if(Back(8) == TOP){
    B(-1);D(-1);B(1);
    return;
  }
  else if(Bottom(8) == TOP){
    B(-1);D(1);B(1);D(2);
    B(-1);D(-1);B(1);
    return;
  }
}

void first_layer_corner_tbl(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Top/Back/Left
   * The src cube in bottom layer is directly below
   * the destination cube in the top layer.
   * Operation depends where the TOP face is in the src cube.
   */
  
  if(Back(6) == TOP){
    B(1);D(1);B(-1);
    return;
  }
  else if(Left(8) == TOP){
    L(-1);D(-1);L(1);
    return;
  }
  else if(Bottom(6) == TOP){
    L(-1);D(1);L(1);D(2);
    L(-1);D(-1);L(1);
    return;
  }
}

void first_layer_corner_tlf(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Top/Left/Front
   * The src cube in bottom layer is directly below
   * the destination cube in the top layer.
   * Operation depends where the TOP face is in the src cube.
   */
  
  if(Left(6) == TOP){
    L(1);D(1);L(-1);
    return;
  }
  else if(Front(6) == TOP){
    F(-1);D(-1);F(1);
    return;
  }
  else if(Bottom(0) == TOP){
    F(-1);D(1);F(1);D(2);
    F(-1);D(-1);F(1);
    return;
  }
}




