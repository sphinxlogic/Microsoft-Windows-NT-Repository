/* taylor.c
   This incorporates Don Taylor's solution to the 
   Buvos Kocka (Hungarian Magic Cube). 

   Steps to Solution:
    1) First Layer Edge Cubes (top)
       top/front
       top/right
       top/back
       top/left
    2) First layer Corner Cubes (top)
       top/front/right
       top/right/back
       top/back/left
       top/left/front
    3) Middle layer (edge cubes)
    4) Last Layer
       Getting corner cubes in correct position
       Getting edge cubes in correct position
       Edge Flips/Corner Cubes
      
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

Widget         saved_w;
widget_data    *saved_wdata;
caddr_t        saved_call_data;
int            number_moves;

void F(); /*  front  */
void L(); /*  left   */
void R(); /*  right  */
void B(); /*  back   */
void U(); /*  up     */
void D(); /*  down   */

extern void first_layer_edge_cube_tf(); /* layer1edge_tf */
extern void first_layer_edge_cube_tr(); /* layer1edge_tr */
extern void first_layer_edge_cube_tb(); /* layer1edge_tb */
extern void first_layer_edge_cube_tl(); /* layer1edge_tl */

extern Boolean first_layer_corner();    /* layer1corner */

extern Boolean middle_layer();          /* layer2 */ 

extern void last_layer();               /* layer3 */

extern Boolean twirl();                 /* layer3twirl */
extern Boolean flip();                  /* layer3flip */

void taylor(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Save 'em in a global so we don't have to pass 'em 
   * around all the time.
   */
  saved_w = w;
  saved_wdata = wdata;
  saved_call_data = call_data;
  number_moves = 1;
  printf("\nSolution:\n");

  /* major parts of solution */
  first_layer_edge_cube_tf(w,wdata,call_data); /* top/front */
  first_layer_edge_cube_tr(w,wdata,call_data); /* top/right */
  first_layer_edge_cube_tb(w,wdata,call_data); /* top/back  */
  first_layer_edge_cube_tl(w,wdata,call_data); /* top/left  */

  /* This is repeated until top layer is completed */
  while(!first_layer_corner(w,wdata,call_data));
  printf("TOP LAYER COMPLETED\n");

  /* This is repeated until middle layer is completed */
  while(!middle_layer(w,wdata,call_data));
  printf("MIDDLE LAYER COMPLETED\n");

  /* put last layer's corners and edges in correct position */
  last_layer(w,wdata,call_data);

  /* twirl last layer's corners */
  while(!twirl(w,wdata,call_data));
  printf("CORNERS TWIRLED\n");

  /* flip last layer's edges */
  while(!flip(w,wdata,call_data));

  printf("End Of Solution\n");

}


void F(nturns)
     int nturns;
{
  int i, normturns;

  printf("%d. Front(%d)\n", number_moves++,nturns);

  /* get rid of extra turns...just in case */
  normturns = nturns % 4;

  /* our cubik_front goes the wrong way */
  /* normturns ranges from -3 to +3, 
     change this range to a 3, 2, 1, -3, -2, -1
   */
  if(normturns > 0)
    normturns -= 4;
  for(i=normturns; i<0; i++)
    cubik_front(saved_w,saved_wdata,saved_call_data);
}
  
void L(nturns)
     int nturns;
{
  int i, normturns;

  printf("%d. Left(%d)\n", number_moves++,nturns);

  /* get rid of extra turns...just in case */
  normturns = nturns % 4;

  /* our cubik_left goes the wrong way */
  if(normturns > 0)
    normturns -= 4;
  for(i=normturns; i<0; i++)
    cubik_left(saved_w,saved_wdata,saved_call_data);
}
  
void R(nturns)
     int nturns;
{
  int i, normturns;

  printf("%d. Right(%d)\n", number_moves++,nturns);

  /* get rid of extra turns...just in case */
  normturns = nturns % 4;

  /* our cubik_right goes the correct way */
  if(normturns < 0)
    normturns += 4;
  for(i=normturns; i>0; i--)
    cubik_right(saved_w,saved_wdata,saved_call_data);
}
  
void B(nturns)
     int nturns;
{
  int i, normturns;

  printf("%d. Back(%d)\n", number_moves++,nturns);

  /* get rid of extra turns...just in case */
  normturns = nturns % 4;

  /* our cubik_back goes the right way */
  if(normturns < 0)
    normturns += 4;
  for(i=normturns; i>0; i--)
    cubik_back(saved_w,saved_wdata,saved_call_data);
}
  
void U(nturns)
     int nturns;
{
  int i, normturns;

  printf("%d. Up(%d)\n", number_moves++,nturns);

  /* get rid of extra turns...just in case */
  normturns = nturns % 4;

  /* our cubik_top goes the right way */
  if(normturns < 0)
    normturns += 4;
  for(i=normturns; i>0; i--)
    cubik_top(saved_w,saved_wdata,saved_call_data);
}
  
void D(nturns)
     int nturns;
{
  int i, normturns;

  printf("%d. Down(%d)\n", number_moves++,nturns);

  /* get rid of extra turns...just in case */
  normturns = nturns % 4;

  /* our cubik_bottom goes the wrong way */
  if(normturns > 0)
    normturns -= 4;
  for(i=normturns; i<0; i++)
    cubik_bottom(saved_w,saved_wdata,saved_call_data);
}
  

