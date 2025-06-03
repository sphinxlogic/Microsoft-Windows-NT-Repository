

/*
 * xanim_act.c
 *
 * Copyright (C) 1990,1991,1992 by Mark Podlipec. 
 * All rights reserved.
 *
 * This software may be freely copied, modified and redistributed
 * without fee provided that this copyright notice is preserved 
 * intact on all copies and modified copies.
 * 
 * There is no warranty or other guarantee of fitness of this software.
 * It is provided solely "as is". The author(s) disclaim(s) all
 * responsibility and liability with respect to this software's usage
 * or its effect upon hardware or computer systems.
 *
 */

#include "xanim.h"
#include <stdio.h>
#include "xanim_act.h"

ACTION *act_first_cmap = 0;
ACTION *act_cur_cmap = 0;
LONG cur_index;
 
ACT_Setup_CMAP(new_cmap,csize,offset,cmap_function)
ColorReg new_cmap[];
ULONG csize,*offset,cmap_function;
{
  ACTION *act;
  ColorReg *act_cmap;
  CMAP_HDR *cmap_hdr;
  LONG i,off,new_cmap_flag;


  if ( (cmap_function & 0x01) && (act_first_cmap != 0) )
  {
    LONG j,match;

    if (debug >= DEBUG_LEVEL1) 
         fprintf(stderr,"CMAP: <%ld> remapping %ld\n",cur_index,csize);
    new_cmap_flag = FALSE;

    cmap_hdr = (CMAP_HDR *) act_cur_cmap->data;
    act_cmap = (ColorReg *) cmap_hdr->data;

    for(i=0; i<csize; i++)
    {
      match = -1;
      j = 0;
      while( (match < 0) && (j < cur_index) && (j < x11_cmap_size) )
      {
        if (    (new_cmap[i].red   == act_cmap[j].red)
             && (new_cmap[i].green == act_cmap[j].green)
             && (new_cmap[i].blue  == act_cmap[j].blue)
            ) match = new_cmap[i].map = act_cmap[j].map;
        j++;
      }
      if (match < 0)
      {
        if (cur_index < x11_cmap_size) 
        {
          act_cmap[cur_index].red   = new_cmap[i].red; 
          act_cmap[cur_index].green = new_cmap[i].green; 
          act_cmap[cur_index].blue  = new_cmap[i].blue; 
          if (cmap_function & 0x02)
            act_cmap[cur_index].map  = new_cmap[i].map = cur_index;
          else 
            act_cmap[cur_index].map   = 
                     new_cmap[i].map = x11_cmap_size - 1 - cur_index;
          cmap_hdr->cmap_size++;
          cur_index++;
        }
        else new_cmap_flag = TRUE;
      }
    } /* end of for i */
   if (debug >= DEBUG_LEVEL1) fprintf(stderr,"<%ld>\n",cur_index);
  }
  else new_cmap_flag = TRUE;


  if (new_cmap_flag == TRUE)
  {

    if (debug >= DEBUG_LEVEL1) fprintf(stderr,"CMAP: new %ld\n",csize);
    act = &action[action_cnt];
    action_cnt++;

    act_cur_cmap = act;
    if (act_first_cmap == 0) act_first_cmap = act_cur_cmap;

    act->type = ACT_CMAP;
    act->time = 0;
    cmap_hdr = (CMAP_HDR *)
                 malloc( x11_cmap_size * sizeof(ColorReg) + sizeof(CMAP_HDR));
    if (cmap_hdr == 0) TheEnd1("ACT_Setup_CMAP: malloc failed\n");
    act->data = (UBYTE *)cmap_hdr;
    act_cmap = (ColorReg *)cmap_hdr->data;
    cmap_hdr->cmap_size = csize;
    cur_index = 0;

    for(i=0; i<csize; i++)
    {
      cmap[i].red   = act_cmap[cur_index].red   = new_cmap[i].red;
      cmap[i].green = act_cmap[cur_index].green = new_cmap[i].green;
      cmap[i].blue  = act_cmap[cur_index].blue  = new_cmap[i].blue;
      if (cmap_function & 0x02)
        cmap[i].map = act_cmap[cur_index].map = new_cmap[i].map = cur_index;
      else 
        act_cmap[cur_index].map = 
              cmap[i].map = new_cmap[i].map = x11_cmap_size - 1 - cur_index;
      cur_index++;
    }
  } /* end of new action */
}

ACT_Get_Next_Action(act)
ACTION **act;
{
 *act = &action[action_cnt];
 action_cnt++;
 (*act)->data = 0;
}


ACT_Setup_IMAGE1(act,time,xpos,ypos,xsize,ysize,image_ptr)
ACTION *act;
LONG time;
LONG xpos,ypos;
LONG xsize,ysize;
UBYTE *image_ptr;
{
 ACT_IMAGE1_HDR *act_im_hdr; 

 act_im_hdr = (ACT_IMAGE1_HDR *) malloc( sizeof(ACT_IMAGE1_HDR) );
 if (act_im_hdr == 0) TheEnd1("X11_Get_Image: can't allocate memory.\n");

 act->type = ACT_IMAGE1;
 act->time = time;
 act->data = (UBYTE *) act_im_hdr;

 act_im_hdr->xpos = xpos;
 act_im_hdr->ypos = ypos;
 act_im_hdr->xsize = xsize;
 act_im_hdr->ysize = ysize;
 act_im_hdr->image = XCreateImage(theDisp,
                               DefaultVisual(theDisp,DefaultScreen(theDisp)),
                               x11_depth,ZPixmap,0,0,xsize,ysize,
                               x11_bytes_pixel,(x11_bytes_pixel * xsize));
 if (act_im_hdr->image == 0) TheEnd1("X11_Get_Image: can't create Image.\n");
 act_im_hdr->image->data = (char *)image_ptr;

}

ACT_Setup_CLIP(act,time,xpos,ypos,xsize,ysize,image_ptr,clip_ptr)
ACTION *act;
LONG time;
LONG xpos,ypos;
LONG xsize,ysize;
UBYTE *image_ptr;
UBYTE *clip_ptr;
{
 ACT_CLIP_HDR *act_cp_hdr; 

 act_cp_hdr = (ACT_CLIP_HDR *) malloc( sizeof(ACT_CLIP_HDR) );
 if (act_cp_hdr == 0) TheEnd1("X11_Get_Image: can't allocate memory.\n");

 act->type = ACT_CLIP;
 act->time = time;
 act->data = (UBYTE *) act_cp_hdr;

 act_cp_hdr->xpos = xpos;
 act_cp_hdr->ypos = ypos;
 act_cp_hdr->xsize = xsize;
 act_cp_hdr->ysize = ysize;
 act_cp_hdr->image = XCreateImage(theDisp,
                               DefaultVisual(theDisp,DefaultScreen(theDisp)),
                               x11_depth,ZPixmap,0,image_ptr,xsize,ysize,
                               x11_bytes_pixel,(x11_bytes_pixel * xsize));
 if (act_cp_hdr->image == 0) 
	TheEnd1("X11_Get_Image: can't create Image.\n");

 act_cp_hdr->clip_ptr = clip_ptr;
}
