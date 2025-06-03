/* layer2.c
   This incorporates Don Taylor's solution to the 
   Buvos Kocka (Hungarian Magic Cube). 

   Middle Layer Cubes
   
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

Boolean middle_layer(w,wdata,call_data) /* These parameters needed for macros */
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Search for middle edge cubes.
     Place in bottom layer.
     Dispatch to middle_layer_[f,r,b,l]t accordingly.
     */

  /* Find edge cubes in bottom layer that belong 
   * in  middle layer.
   */
  
  /*1*/
  if(Front(7) != BOTTOM &&
     Bottom(1) != BOTTOM){
    /* Then it is a middle layer edge */
    /* It is 1 of 8 combinations */
    if(Front(7)  == FRONT &&
       Bottom(1) == RIGHT){
      D(-1);
      F(1);D(2);L(1);D(1);L(-1);D(2);F(-1); /* 10.3 'top matches left-hand' */
    }
    
    else if(Front(7)  == RIGHT &&
	    Bottom(1) == FRONT){
      F(1);D(2);L(1);D(-1);L(-1);D(2);F(-1); /* 10.4 'top matches front-hand' */
    }
    
    
    
    else if(Front(7)  == RIGHT &&
	    Bottom(1) == BACK){
      R(1);D(2);F(1);D(1);F(-1);D(2);R(-1); /* 10.3 */
    }
    
    else if(Front(7)  == BACK &&
	    Bottom(1) == RIGHT){
      D(1);
      R(1);D(2);F(1);D(-1);F(-1);D(2);R(-1); /* 10.4 */
    }
    
    
    
    else if(Front(7)  == BACK &&
	    Bottom(1) == LEFT){
      D(1);
      B(1);D(2);R(1);D(1);R(-1);D(2);B(-1); /* 10.3 */
    }
    
    else if(Front(7)  == LEFT &&
	    Bottom(1) == BACK){
      D(2);
      B(1);D(2);R(1);D(-1);R(-1);D(2);B(-1);
    }
    
    
    
    else if(Front(7)  == LEFT &&
	    Bottom(1) == FRONT){
      D(2);
      L(1);D(2);B(1);D(1);B(-1);D(2);L(-1);
    }
    
    else if(Front(7)  == FRONT &&
	    Bottom(1) == LEFT){
      D(-1);
      L(1);D(2);B(1);D(-1);B(-1);D(2);L(-1);
    }
    return(False);
  }
  /*2*/
  else if(Right(7) != BOTTOM &&
	  Bottom(5) != BOTTOM){
    /* Then it is a middle layer edge */
    /* It is 1 of 8 combinations */
    if(Right(7)  == FRONT &&
       Bottom(5) == RIGHT){
      D(2);
      F(1);D(2);L(1);D(1);L(-1);D(2);F(-1); /* 10.3 'top matches left-hand' */
    }
    
    else if(Right(7)  == RIGHT &&
	    Bottom(5) == FRONT){
      D(-1);
      F(1);D(2);L(1);D(-1);L(-1);D(2);F(-1); /* 10.4 'top matches front-hand' */
    }
    
    
    
    else if(Right(7)  == RIGHT &&
	    Bottom(5) == BACK){
      D(-1);
      R(1);D(2);F(1);D(1);F(-1);D(2);R(-1); /* 10.3 */
    }
    
    else if(Right(7)  == BACK &&
	    Bottom(5) == RIGHT){
      R(1);D(2);F(1);D(-1);F(-1);D(2);R(-1); /* 10.4 */
    }
    
    
    
    else if(Right(7)  == BACK &&
	    Bottom(5) == LEFT){
      B(1);D(2);R(1);D(1);R(-1);D(2);B(-1); /* 10.3 */
    }
    
    else if(Right(7)  == LEFT &&
	    Bottom(5) == BACK){
      D(1);
      B(1);D(2);R(1);D(-1);R(-1);D(2);B(-1);
    }
    
    
    
    else if(Right(7)  == LEFT &&
	    Bottom(5) == FRONT){
      D(1);
      L(1);D(2);B(1);D(1);B(-1);D(2);L(-1);
    }
    
    else if(Right(7)  == FRONT &&
	    Bottom(5) == LEFT){
      D(2);
      L(1);D(2);B(1);D(-1);B(-1);D(2);L(-1);
    }
    return(False);
  }
  /*3*/
  else if(Back(7) != BOTTOM &&
	  Bottom(7) != BOTTOM){
    /* Then it is a middle layer edge */
    /* It is 1 of 8 combinations */
    if(Back(7)  == FRONT &&
       Bottom(7) == RIGHT){
      D(1);
      F(1);D(2);L(1);D(1);L(-1);D(2);F(-1); /* 10.3 'top matches left-hand' */
    }
    
    else if(Back(7)  == RIGHT &&
	    Bottom(7) == FRONT){
      D(2);
      F(1);D(2);L(1);D(-1);L(-1);D(2);F(-1); /* 10.4 'top matches front-hand' */
    }
    
    
    
    else if(Back(7)  == RIGHT &&
	    Bottom(7) == BACK){
      D(2);
      R(1);D(2);F(1);D(1);F(-1);D(2);R(-1); /* 10.3 */
    }
    
    else if(Back(7)  == BACK &&
	    Bottom(7) == RIGHT){
      D(3);
      R(1);D(2);F(1);D(-1);F(-1);D(2);R(-1); /* 10.4 */
    }
    
    
    
    else if(Back(7)  == BACK &&
	    Bottom(7) == LEFT){
      D(3);
      B(1);D(2);R(1);D(1);R(-1);D(2);B(-1); /* 10.3 */
    }
    
    else if(Back(7)  == LEFT &&
	    Bottom(7) == BACK){
      B(1);D(2);R(1);D(-1);R(-1);D(2);B(-1);
    }
    
    
    
    else if(Back(7)  == LEFT &&
	    Bottom(7) == FRONT){
      L(1);D(2);B(1);D(1);B(-1);D(2);L(-1);
    }
    
    else if(Back(7)  == FRONT &&
	    Bottom(7) == LEFT){
      D(1);
      L(1);D(2);B(1);D(-1);B(-1);D(2);L(-1);
    }
    return(False);
  }
  /*4*/
  else if(Left(7) != BOTTOM &&
	  Bottom(3) != BOTTOM){
    /* Then it is a middle layer edge */
    /* It is 1 of 8 combinations */
    if(Left(7)  == FRONT &&
       Bottom(3) == RIGHT){
      F(1);D(2);L(1);D(1);L(-1);D(2);F(-1); /* 10.3 'top matches left-hand' */
    }
    
    else if(Left(7)  == RIGHT &&
	    Bottom(3) == FRONT){
      D(1);
      F(1);D(2);L(1);D(-1);L(-1);D(2);F(-1); /* 10.4 'top matches front-hand' */
    }
    
    
    
    else if(Left(7)  == RIGHT &&
	    Bottom(3) == BACK){
      D(1);
      R(1);D(2);F(1);D(1);F(-1);D(2);R(-1); /* 10.3 */
    }
    
    else if(Left(7)  == BACK &&
	    Bottom(3) == RIGHT){
      D(2);
      R(1);D(2);F(1);D(-1);F(-1);D(2);R(-1); /* 10.4 */
    }
    
    
    
    else if(Left(7)  == BACK &&
	    Bottom(3) == LEFT){
      D(2);
      B(1);D(2);R(1);D(1);R(-1);D(2);B(-1); /* 10.3 */
    }
    
    else if(Left(7)  == LEFT &&
	    Bottom(3) == BACK){
      D(-1);
      B(1);D(2);R(1);D(-1);R(-1);D(2);B(-1);
    }
    
    
    
    else if(Left(7)  == LEFT &&
	    Bottom(3) == FRONT){
      D(-1);
      L(1);D(2);B(1);D(1);B(-1);D(2);L(-1);
    }
    
    else if(Left(7)  == FRONT &&
	    Bottom(3) == LEFT){
      L(1);D(2);B(1);D(-1);B(-1);D(2);L(-1);
    }
    return(False);
  }

  /* Check if all middle layer cubes in place */
  else if(Front(5) == FRONT &&
	  Right(3) == RIGHT &&
	  Right(5) == RIGHT &&
	  Back(5) == BACK &&
	  Back(3) == BACK &&
	  Left(5) == LEFT &&
	  Left(3) == LEFT &&
	  Front(3) == FRONT){
    return(True);
  }
  /* Else no middle layer cubes in the bottom layer.
     * So a middle layer cube must be moved out to the bottom layer.
     */
  else {
    if(Front(5) == RIGHT &&
       Right(3) == FRONT){
      F(1);D(2);L(1);D(1);L(-1);D(2);F(-1);     
    }
    else if(Right(5) == BACK &&
	    Back(5) == RIGHT){
      R(1);D(2);F(1);D(1);F(-1);D(2);R(-1); 
    }
    else if(Back(3) == LEFT &&
	    Left(5) == BACK){
      B(1);D(2);R(1);D(1);R(-1);D(2);B(-1); 
    }
    else if(Left(3) == FRONT &&
	    Front(3) == LEFT){
      L(1);D(2);B(1);D(1);B(-1);D(2);L(-1);
    }
    return(False);
  }
}
