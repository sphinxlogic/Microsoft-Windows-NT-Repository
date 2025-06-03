/* layer1edge_tr.c
   This incorporates Don Taylor's solution to the 
   Buvos Kocka (Hungarian Magic Cube). 

   First Layer Edge Cube top/front


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

void first_layer_edge_cube_tr(w,wdata,call_data) /* These parameters needed for macros */
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Top/Right */
  /* Search for the Top/Right edge cube.
     There are 12 edge cubes in all,
     with 2 possible configurations: Top/Right
     or Right/Top.
     */
  
  /* Top edge cubes */
  if(Top(1) == RIGHT && Front(1) == TOP){
    F(1);R(1);
  }
  
  else if(Top(5) == RIGHT && Right(1) == TOP){
    R(2);D(1);B(1);R(-1);B(-1);
  }
  
  else if(Top(7) == RIGHT && Back(1) == TOP){
    B(-1);R(-1);
  } 
  
  else if(Top(3) == RIGHT && Left(1) == TOP){
    L(2);D(-1);B(1);R(-1);B(-1);
  }
  

  
  else if(Top(1) == TOP && Front(1) == RIGHT){
    F(2);D(1);R(2);
  }
  
  else if(Top(5) == TOP && Right(1) == RIGHT){
    /* the correct one */ 
    ;
  }
  
  else if(Top(7) == TOP && Back(1) == RIGHT){
    B(2);D(-1);R(2);
  } 
  
  else if(Top(3) == TOP && Left(1) == RIGHT){
    L(2);D(2);R(2);
  }

  /* middle edge cubes */ 
  else if(Front(5) == RIGHT && Right(3) == TOP){
    F(1);D(1);F(-1);R(2);
  }
  
  else if(Right(5) == RIGHT && Back(5) == TOP){
    R(-1);
  }
  
  else if(Back(3) == RIGHT && Left(5) == TOP){
    L(-1);D(-1);L(1);B(1);R(-1);B(-1);
  } 

  else if(Left(3) == RIGHT && Front(3) == TOP){
    L(1);D(2);L(-1);R(2);
  }



  else if(Front(5) == TOP && Right(3) == RIGHT){
    R(1);
  }

  else if(Right(5) == TOP && Back(5) == RIGHT){
    R(1);D(1);B(1);R(-1);B(-1);
  }

  else if(Back(3) == TOP && Left(5) == RIGHT){
    B(2);R(-1);B(-2);
  } 

  else if(Left(3) == TOP && Front(3) == RIGHT){
    L(1);D(-1);L(-1);B(1);R(-1);B(-1);
  }


  /* Bottom edge cubes */
  else if(Bottom(1) == RIGHT && Front(7) == TOP){
    F(-1);R(1);F(1);
  }
  
  else if(Bottom(5) == RIGHT && Right(7) == TOP){
    D(1);B(1);R(-1);B(-1);
  }

  else if(Bottom(7) == RIGHT && Back(7) == TOP){
    B(1);R(-1);B(-1);
  } 

  else if(Bottom(3) == RIGHT && Left(7) == TOP){
    D(-1);B(1);R(-1);B(-1);
  }


  else if(Bottom(1) == TOP && Front(7) == RIGHT){
    D(1);R(2);
  }

  else if(Bottom(5) == TOP && Right(7) == RIGHT){
    R(2);
  }

  else if(Bottom(7) == TOP && Back(7) == RIGHT){
    D(-1);R(2);
  } 

  else if(Bottom(3) == TOP && Left(7) == RIGHT){
    D(2);R(2);
  }
}




