/* layer3.c
   This incorporates Don Taylor's solution to the 
   Buvos Kocka (Hungarian Magic Cube). 

   Last Layer Cubes

   a) Down(n) until one corner has correct position
   b) Move corners into correct position (Section 11; a, b, c, d solutions)
   c) Move edge cubes into correct position
       


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

void last_layer(w,wdata,call_data) /* These parameters needed for macros */
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /**** Move corners into correct position ****/
  /* Place correct corner at Back(8)/Right(8)/Bottom(8).
   * There are only 4 possible locations.
   */
  
  
  if((Back(6) == BACK   || Back(6) == BOTTOM   || Back(6) == RIGHT) &&
     (Bottom(6) == BACK || Bottom(6) == BOTTOM || Bottom(6) == RIGHT) &&
     (Left(8) == BACK   || Left(8) == BOTTOM   || Left(8) == RIGHT)){
    D(-1);
  }
  else if((Front(6) == BACK  || Front(6) == BOTTOM  || Front(6) == RIGHT) &&
	  (Bottom(0) == BACK || Bottom(0) == BOTTOM || Bottom(0) == RIGHT) &&
	  (Left(6) == BACK   || Left(6) == BOTTOM   || Left(6) == RIGHT)){
    D(2);
  }
  else if((Front(8) == BACK  || Front(8) == BOTTOM  || Front(8) == RIGHT) &&
	  (Bottom(2) == BACK || Bottom(2) == BOTTOM || Bottom(2) == RIGHT) &&
	  (Right(6) == BACK  || Right(6) == BOTTOM  || Right(6) == RIGHT)){
    D(1);
  }
  
  /* Now Back(8)/Right(8)/Bottom(8) is correct and is reference  point.
   * Place other corner cubes in correct position.
   * 
   * Proceed to crack for a, b, c, or d solution.
   * Using just bottom(n=0,2,6,8) == true to test if cube is in right 
   * position. The following if/then/else tree is used to crack for
   * a, b, c,  or d solution.
   *
   *  Given that bottom(8) = true now:
   *          front 
   *   -------------------
   *   |     |     |     |
   *   |     |     |     |
   *   |  0  |     |  2  |
   *   -------------------
   * l |     |     |     |  right 
   * e |     |     |     |
   * f |     |     |     |
   * t  -------------------
   *   |     |     |     |
   *   |     |     | ok  |
   *   |  6  |     |  8  |
   *   -------------------
   *          back
   *
   *   if(bottom(2) == true){
   *       if(bottom(0) == true)  
   *         "then all four courners must be in correct postion"
   *       else
   *         "then bottom(6)=false and C solution(swap0-6)"
   *   }
   *   else{
   *       if(bottom(0) == true)
   *         "then bottom(6)=false and D solution(swap2-6)"
   *       else{
   *         if(bottom(6) == true)
   *          "then C solution(swap0-2)"
   *	     else{
   *	       if(bottom(6) belongs in bottom(0))
   *	          "A solution(2<--0;0<--6;6<--2)"
   *	       else
   *	          "B solution(2<--6;0<--2;6<--0)"
   *         }
   *       }
   *   }
   */

  if(/* bottom(2) */
     (Front(8) == FRONT  || Front(8) == BOTTOM  || Front(8) == RIGHT) &&
     (Bottom(2) == FRONT || Bottom(2) == BOTTOM || Bottom(2) == RIGHT) &&
     (Right(6) == FRONT  || Right(6) == BOTTOM  || Right(6) == RIGHT)){
    /* Then this adjacent cube is correct and sub-solution is `c' 
     * or all corners in correct position.
     */
    if(/* bottom(0) */
       (Front(6) == FRONT  || Front(6) == BOTTOM  || Front(6) == LEFT) &&
       (Bottom(0) == FRONT || Bottom(0) == BOTTOM || Bottom(0) == LEFT) &&
       (Left(6) == FRONT   || Left(6) == BOTTOM   || Left(6) == LEFT)){
      /* all corners must be ok */
      ;
    }
    else{
      /*          front 
       *   -------------------
       *   |  0  |     |  2  |
       *   | not |     | ok  |
       *   | ok  |     |     |
       *   -------------------
       * l |     |     |     |  right 
       * e |     |     |     |
       * f |     |     |     |
       * t  -------------------
       *   | not |     |     |
       *   | ok  |     | ok  |
       *   |  6  |     |  8  |
       *   -------------------
       *          back
       */
      /* c solution needed */
      R(1);D(-1);L(-1);D(1);R(-1);D(-1);L(1);D(2);
    }
  }
  else{
    if(/* Bottom(0) */
       (Front(6) == FRONT  || Front(6) == BOTTOM  || Front(6) == LEFT) &&
       (Bottom(0) == FRONT || Bottom(0) == BOTTOM || Bottom(0) == LEFT) &&
       (Left(6) == FRONT   || Left(6) == BOTTOM   || Left(6) == LEFT)){
      /*          front 
       *   -------------------
       *   |     |     | not |
       *   |  ok |     |  ok |
       *   |  0  |     |  2  |
       *   -------------------
       * l |     |     |     |  right 
       * e |     |     |     |
       * f |     |     |     |
       * t -------------------
       *   | not |     |     |
       *   | ok  |     | ok  | 
       *   |  6  |     |  8  |
       *   -------------------
       *          back
       */
      /* d solution needed */
      D(1);B(1);D(1);R(1);D(-1);R(-1);B(-1);
    }
    else{
      if(/*bottom(6)*/
	 (Back(6) == BACK   || Back(6) == BOTTOM   || Back(6) == LEFT) &&
	 (Bottom(6) == BACK || Bottom(6) == BOTTOM || Bottom(6) == LEFT) &&
	 (Left(8) == BACK   || Left(8) == BOTTOM   || Left(8) == LEFT)){
	/*          front 
	 *   -------------------
	 *   | not |     | not |
	 *   |  ok |     |  ok |
	 *   |  0  |     |  2  |
	 *   -------------------
	 * l |     |     |     |  right 
	 * e |     |     |     |
	 * f |     |     |     |
	 * t -------------------
	 *   |     |     |     |
	 *   | ok  |     | ok  | Must be OK
	 *   |  6  |     |  8  |
	 *   -------------------
	 *          back
	 */ 
	/* c solution needed */
	B(1);D(-1);F(-1);D(1);B(-1);D(-1);F(1);D(2);
      }
      else{ 
	if(/*bottom(6)*/
	 (Back(6) == FRONT   || Back(6) == BOTTOM   || Back(6) == LEFT) &&
	 (Bottom(6) == FRONT || Bottom(6) == BOTTOM || Bottom(6) == LEFT) &&
	 (Left(8) == FRONT   || Left(8) == BOTTOM   || Left(8) == LEFT)){
	  /* bottom(6) belongs in bottom(0) */
	  /* A solution needed (2<--0;0<--6;6<--2) */
	  L(-1);D(1);R(1);D(-1);L(1);D(1);R(-1);D(-1);
	}
	else{
	  /* bottom(6) belongs in bottom(2) */
	  /* B solution needed (2<--6;0<--2;6<--0) */
	  D(1);R(1);D(-1);L(-1);D(1);R(-1);D(-1);L(1);
	}
      }
    }
  }

  /**** Move edges into correct position ****/
  
  /* Proceed to crack for a, b, c, or d solution.
   * Using just bottom(n=1,3,5,7) == true to test if cube is in right 
   * position. The following if/then/else tree is used to crack for
   * a, b, c,  or d solution.
   *
   *          front 
   *   -------------------
   *   |     |     |     |
   *   |     |     |     |
   *   |     |  1  |     |
   *   -------------------
   * l |     |     |     |  right 
   * e |     |     |     |
   * f |  3  |     |  5  |
   * t -------------------
   *   |     |     |     |
   *   |     |     |     |
   *   |     |  7  |     |
   *   -------------------
   *          back
   *
   *   if(bottom(1) == true){
   *      if(bottom(3) belongs in bottom(5))
   *        "A solution(3 to 5 to 7 to 3)"
   *      else if(bottom(5) belongs in bottom(3))
   *        "B solution(5 to 3 to 7 to 5)"
   *      else
   *        "No solution needed, all edges  are ok"
   *   }else
   *   if(bottom(3) == true){
   *      if(bottom(7) belongs in bottom(1))
   *        "A solution(7 to 1 to 5 to 7)"
   *      else if(bottom(1) belongs in bottom(7))
   *        "B solution(1 to 7 to 5 to 1)"
   *      else
   *        "No solution needed, all edges  are ok"
   *   }else
   *   if(bottom(5) == true){
   *      if(bottom(1) belongs in bottom(7))
   *        "A solution(1 to 7 to 3 to 1)"
   *      else if(bottom(7) belongs in bottom(1))
   *        "B solution(7 to 1 to 3 to 7)"
   *      else
   *        "No solution needed, all edges  are ok"
   *   }else
   *   if(bottom(7) == true){
   *      if(bottom(5) belongs in bottom(3))
   *        "A solution(5 to 3 to 1 to 5)"
   *      else if(bottom(3) belongs in bottom(5))
   *        "B solution(3 to 5 to 1 to 3)"
   *      else
   *        "No solution needed, all edges  are ok"
   *   }else{
   *    "No edges are in the correct position."
   *    "Find out where bottom(1) belongs and this determines
   *     the correct solution"
   *     if(bottom(1) belongs in bottom(3))
   *       "D solution(1 to 3; 3 to 1; 5 to 7; 7 to 5)"
   *     else if(bottom(1) belongs in bottom(7))
   *       "C solution(1 to 7; 7 to 1; 3 to 5; 5 to 3)"
   *     else if(bottom(1) belongs in bottom(5))
   *       "D solution(1 to 5; 5 to 1; 3 to 7; 7 to 3)"
   *   }
   */ 

  if(Front(7) == FRONT || Bottom(1) == FRONT){
    /* bottom(1) ok */
    if(Left(7) == RIGHT || Bottom(3) == RIGHT){
      /* bottom(3) belongs in bottom(5) */
      /* A solution(3 to 5 to 7 to 3) */
      B(2);D(1);R(-1);L(1);B(2);R(1);L(-1);D(1);B(2);
    }
    else if(Right(7) == LEFT || Bottom(5) == LEFT){
      /* bottom(5) belongs in bottom(3) */
      /* B solution(5 to 3 to 7 to 5) */
      B(2);D(-1);R(-1);L(1);B(2);R(1);L(-1);D(-1);B(2);
    }
    else{ ; } /* all edges in proper place */
  }

  else if(Left(7) == LEFT || Bottom(3) == LEFT){
    /* bottom(3) ok */
    if(Back(7) == FRONT || Bottom(7) == FRONT){
      /* bottom(7) belongs in bottom(1) */
      /* A solution(7 to 1 to 5 to 7) */
      L(2);D(1);F(-1);B(1);L(2);F(1);B(-1);D(1);L(2);
    }
    else if(Front(7) == BACK || Bottom(1) == BACK){
      /* bottom(1) belongs in bottom(7) */
      /* B solution(1 to 7 to 5 to 1) */
      L(2);D(-1);F(-1);B(1);L(2);F(1);B(-1);D(-1);L(2);
    }
    else{ ; } /* all edges in proper place */
  }

  else if(Right(7) == RIGHT || Bottom(3) == RIGHT){
    /* bottom(5) ok */
    if(Front(7) == BACK || Bottom(1) == BACK){
      /* bottom(1) belongs in bottom(7) */
      /* A solution(1 to 7 to 3 to 1) */
      L(2);D(1);B(-1);F(1);L(2);B(1);F(-1);D(1);L(2);
    }
    else if(Back(7) == FRONT || Bottom(7) == FRONT){
      /* bottom(7) belongs in bottom(1) */
      /* B solution(7 to 1 to 3 to 7) */
      L(2);D(-1);B(-1);F(1);L(2);B(1);F(-1);D(-1);L(2);
    }
    else{ ; } /* all edges in proper place */
  }
  
  else if(Back(7) == BACK || Bottom(3) == BACK){
    /* bottom(7) ok */
    if(Right(7) == LEFT || Bottom(5) == LEFT){
      /* bottom(5) belongs in bottom(3) */
      /* A solution(5 to 3 to 1 to 5) */
      F(2);D(1);L(-1);R(1);F(2);L(1);R(-1);D(1);F(2);
    }
    else if(Left(7) == RIGHT || Bottom(3) == RIGHT){
      /* bottom(3) belongs in bottom(5) */
      /* B solution(3 to 5 to 1 to 3) */
      F(2);D(-1);L(-1);R(1);F(2);L(1);R(-1);D(-1);F(2);
    }
    else{ ; } /* all edges in proper place */
  }

  else{
    /* There are no edges in their proper place. */
    if(Front(7) == LEFT || Bottom(1) == LEFT){
      /* bottom(1) belongs in bottom(3) */
      /* D solution(1 to 3; 3 to 1; 5 to 7; 7 to 5) */
      F(1);L(1);D(1);L(-1);D(-1);F(2);R(-1);D(-1);R(1);D(1);F(1);
    }
    else if(Front(7) == BACK || Bottom(7) == BACK){
      /* bottom(1) belongs in bottom(7) */
      /* C solution(1 to 7; 7 to 1; 3 to 5; 5 to 3) */
      R(2);L(2);U(1);R(2);L(2);D(2);R(2);L(2);U(1);R(2);L(2);
    }
    else if(Front(7) == RIGHT || Bottom(5) == RIGHT){
      /* bottom(1) belongs in bottom(5) */
      /* D solution(1 to 5; 5 to 1; 3 to 7; 7 to 3) */
      R(1);F(1);D(1);F(-1);D(-1);R(2);B(-1);D(-1);B(1);D(1);R(1);
    }
    else{
      printf("ERROR: layer3 error...");
    }
  }

}
