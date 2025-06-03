/* layer1corner.c
   This incorporates Don Taylor's solution to the 
   Buvos Kocka (Hungarian Magic Cube). 

   First Layer Corner Cube

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

extern void first_layer_corner_tfr();   /* layer1c_txx */
extern void first_layer_corner_trb();   /* layer1c_txx */
extern void first_layer_corner_tbl();   /* layer1c_txx */
extern void first_layer_corner_tlf();   /* layer1c_txx */

Boolean first_layer_corner(w,wdata,call_data) /* These parameters needed for macros */
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Search for the Top corner cubes,
     place in lower layer directly below
     destination cube then  
     dispatch to first_layer_corner_tXX accordingly.
     There are 8 corner cubes in all.
     */

  /* Bottom Corner Cubes */
  /* There are 12 places a TOP can be in the bottom layer.
   */
  
  /*1*/
  if(Front(8) == TOP){
    if(Right(6) == FRONT){
      if(Bottom(2) == RIGHT){
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(2) == LEFT){
	D(-1);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Right(6) == RIGHT){
      if(Bottom(2) == FRONT){
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(2) == BACK){
	D(1);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Right(6) == BACK){
      if(Bottom(2) == RIGHT){
	D(1);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Bottom(2) == LEFT){
	D(2);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Right(6) == LEFT){
      if(Bottom(2) == FRONT){
	D(-1);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Bottom(2) == BACK){
	D(2);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  else if(Right(6) == TOP){
    if(Front(8) == FRONT){
      if(Bottom(2) == RIGHT){
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(2) == LEFT){
	D(-1);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Front(8) == RIGHT){
      if(Bottom(2) == FRONT){
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(2) == BACK){
	D(1);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Front(8) == BACK){
      if(Bottom(2) == RIGHT){
	D(1);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Bottom(2) == LEFT){
	D(2);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Front(8) == LEFT){
      if(Bottom(2) == FRONT){
	D(-1);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Bottom(2) == BACK){
	D(2);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  else if(Bottom(2) == TOP){
    if(Front(8) == FRONT){
      if(Right(6) == RIGHT){
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Right(6) == LEFT){
	D(-1);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Front(8) == RIGHT){
      if(Right(6) == FRONT){
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Right(6) == BACK){
	D(1);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Front(8) == BACK){
      if(Right(6) == RIGHT){
	D(1);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Right(6) == LEFT){
	D(2);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Front(8) == LEFT){
      if(Right(6) == FRONT){
	D(-1);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Right(6) == BACK){
	D(2);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  /*2*/
  if(Right(8)  == TOP){
    if(Back(8) == FRONT){
      if(Bottom(8) == RIGHT){
	D(-1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(8) == LEFT){
	D(2);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Back(8) == RIGHT){
      if(Bottom(8) == FRONT){
	D(-1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(8) == BACK){
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Back(8) == BACK){
      if(Bottom(8) == RIGHT){
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Bottom(8) == LEFT){
	D(1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Back(8) == LEFT){
      if(Bottom(8) == FRONT){
	D(2);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Bottom(8) == BACK){
	D(1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  else if(Back(8)  == TOP){
    if(Right(8) == FRONT){
      if(Bottom(8) == RIGHT){
	D(-1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(8) == LEFT){
	D(2);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Right(8) == RIGHT){
      if(Bottom(8) == FRONT){
	D(-1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(8) == BACK){
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Right(8) == BACK){
      if(Bottom(8) == RIGHT){
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Bottom(8) == LEFT){
	D(1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Right(8) == LEFT){
      if(Bottom(8) == FRONT){
	D(2);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Bottom(8) == BACK){
	D(1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  else if(Bottom(8)  == TOP){
    if(Right(8) == FRONT){
      if(Back(8) == RIGHT){
	D(-1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Back(8) == LEFT){
	D(2);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Right(8) == RIGHT){
      if(Back(8) == FRONT){
	D(-1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Back(8) == BACK){
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Right(8) == BACK){
      if(Back(8) == RIGHT){
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Back(8) == LEFT){
	D(1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Right(8) == LEFT){
      if(Back(8) == FRONT){
	D(2);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Back(8) == BACK){
	D(1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  /*3*/
  if(Back(6)  == TOP){
    if(Left(8) == FRONT){
      if(Bottom(6) == RIGHT){
	D(2);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(6) == LEFT){
	D(1);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Left(8) == RIGHT){
      if(Bottom(6) == FRONT){
	D(2);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(6) == BACK){
	D(-1);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Left(8) == BACK){
      if(Bottom(6) == RIGHT){
	D(-1);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Bottom(6) == LEFT){
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Left(8) == LEFT){
      if(Bottom(6) == FRONT){
	D(1);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Bottom(6) == BACK){
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  else if(Left(8)  == TOP){
    if(Back(6) == FRONT){
      if(Bottom(6) == RIGHT){
	D(2);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(6) == LEFT){
	D(1);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Back(6) == RIGHT){
      if(Bottom(6) == FRONT){
	D(2);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(6) == BACK){
	D(-1);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Back(6) == BACK){
      if(Bottom(6) == RIGHT){
	D(-1);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Bottom(6) == LEFT){
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Back(6) == LEFT){
      if(Bottom(6) == FRONT){
	D(1);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Bottom(6) == BACK){
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  else if(Bottom(6)  == TOP){
    if(Back(6) == FRONT){
      if(Left(8) == RIGHT){
	D(2);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Left(8) == LEFT){
	D(1);
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Back(6) == RIGHT){
      if(Left(8) == FRONT){
	D(2);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Left(8) == BACK){
	D(-1);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Back(6) == BACK){
      if(Left(8) == RIGHT){
	D(-1);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Left(8) == LEFT){
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Back(6) == LEFT){
      if(Left(8) == FRONT){
	D(1);
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Left(8) == BACK){
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  /*4*/
  if(Left(6)  == TOP){
    if(Front(6) == FRONT){
      if(Bottom(0) == RIGHT){
	D(1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(0) == LEFT){
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Front(6) == RIGHT){
      if(Bottom(0) == FRONT){
	D(1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(0) == BACK){
	D(2);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Front(6) == BACK){
      if(Bottom(0) == RIGHT){
	D(2);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Bottom(0) == LEFT){
	D(-1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Front(6) == LEFT){
      if(Bottom(0) == FRONT){
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Bottom(0) == BACK){
	D(-1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }
  
  else if(Front(6)  == TOP){
    if(Left(6) == FRONT){
      if(Bottom(0) == RIGHT){
	D(1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(0) == LEFT){
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Left(6) == RIGHT){
      if(Bottom(0) == FRONT){
	D(1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Bottom(0) == BACK){
	D(2);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Left(6) == BACK){
      if(Bottom(0) == RIGHT){
	D(2);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Bottom(0) == LEFT){
	D(-1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Left(6) == LEFT){
      if(Bottom(0) == FRONT){
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Bottom(0) == BACK){
	D(-1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }

  else if(Bottom(0)  == TOP){
    if(Left(6) == FRONT){
      if(Front(6) == RIGHT){
	D(1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Front(6) == LEFT){
	first_layer_corner_tlf(w,wdata,call_data); 
      }
    }
    else if(Left(6) == RIGHT){
      if(Front(6) == FRONT){
	D(1);
	first_layer_corner_tfr(w,wdata,call_data); 
      }
      else if(Front(6) == BACK){
	D(2);
	first_layer_corner_trb(w,wdata,call_data); 
      }
    }
    else if(Left(6) == BACK){
      if(Front(6) == RIGHT){
	D(2);
	first_layer_corner_trb(w,wdata,call_data);
      }
      else if(Front(6) == LEFT){
	D(-1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
    else if(Left(6) == LEFT){
      if(Front(6) == FRONT){
	first_layer_corner_tlf(w,wdata,call_data);
      }
      else if(Front(6) == BACK){
	D(-1);
	first_layer_corner_tbl(w,wdata,call_data);
      }
    }
  }

  /* Top Corner Cubes */
  /* There are 12 places a TOP can be in the top layer corner cubes layer.
      If all first layer corners are ok, then True is returned.
      Else if a TOP is found in top layer corner, with the wrong orientation then
     the operation to bring it to the lower layer is performed.
      Else False is returned. 
   */

  if(Top(0) == TOP &&
     Top(2) == TOP &&
     Top(6) == TOP &&
     Top(8) == TOP &&
     Front(0) == FRONT &&
     Front(2) == FRONT &&
     Right(0) == RIGHT &&
     Right(2) == RIGHT &&
     Back(0) == BACK &&
     Back(2) == BACK &&
     Left(0) == LEFT &&
     Left(2) == LEFT)
    return(True);
  
  /* Here if more passes needed.
   * Check if any miss-oriented ones on top.
   * If so, then bring it down to bottom layer.
   */
  if((Top(2)   == TOP ||  
      Front(2) == TOP ||  
      Right(0) == TOP) &&
     (Top(2)   != TOP ||  
      Front(2) != FRONT ||  
      Right(0) != RIGHT)){
    R(-1);D(1);R(1);
    return(False);
  }
  
  if((Top(8)   == TOP ||  
      Right(2) == TOP ||  
      Back(2)  == TOP) &&
     (Top(8)   != TOP ||  
      Right(2) != RIGHT ||  
      Back(2)  != BACK)){
    B(-1);D(1);B(1);
    return(False);
  }
  
  if((Top(6)   == TOP ||  
      Back(0)  == TOP ||  
      Left(2)  == TOP) &&
     (Top(6)   != TOP ||  
      Back(0)  != BACK ||  
      Left(2)  != LEFT)){
    L(-1);D(1);L(1);
    return(False);
  }
  
  if((Top(0)   == TOP ||  
      Left(0)  == TOP ||  
      Front(0) == TOP) &&
     (Top(0)   != TOP ||  
      Left(0)  != LEFT ||  
      Front(0) != FRONT)){
    F(-1);D(1);F(1);
    return(False);
  }
  return(False);
}
