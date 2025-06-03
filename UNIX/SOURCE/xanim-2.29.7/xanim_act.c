

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
#include "xanim_act.h"

int ColorComp();
void ACT_Setup_CMAP();
void ACT_Setup_Mapped();
void ACT_Setup_Packed();
void ACT_Get_Next_Action();
void qsort();

ACTION *act_first_cmap = 0;
ACTION *act_cur_cmap = 0;
static LONG cur_index;

/*
 *
 */
int ColorComp(c1,c2)
ColorReg *c1,*c2;
{
 long val1,val2;

  val1 = (3 * c1->red) + (4 * c1->green) + (2 * c1->blue);
  val2 = (3 * c2->red) + (4 * c2->green) + (2 * c2->blue);
  if (val1 != val2) return( val2 - val1 );
  else if (c1->green != c2->green) return( c2->green - c1->green );
  else if (c1->red   != c2->red  ) return( c2->red   - c1->red   );
  else return( c2->blue - c1->blue );
}

/*
 *
 */
int CMAP_Find_Closest(r,g,b,t_cmap,csize)
ULONG r,g,b;
ColorReg *t_cmap;
ULONG csize;
{
  register ULONG i,min_diff,diff,tmp;
  register int cmap_entry;

  diff = min_diff = 32 * 256 * 256 * 2;
  i = csize;
  while( (i > 0) && (diff != 0) )
  {
    i--;
    tmp = r - (ULONG)(t_cmap[i].red);   tmp *= tmp;  diff  = 11 * tmp;
    tmp = g - (ULONG)(t_cmap[i].green); tmp *= tmp;  diff += 16 * tmp;
    tmp = b - (ULONG)(t_cmap[i].blue);  tmp *= tmp;  diff +=  5 * tmp;

    if (diff < min_diff) {min_diff = diff; cmap_entry = i;}
  }
  return(cmap_entry);
}

/*
 *
 */
int CMAP_Find_Exact(r,g,b,cmap,csize)
ULONG r,g,b;
ColorReg *cmap;
ULONG csize;
{
  register int i,match;

  match = -1;
  i = csize;
  while( (i > 0) && (match < 0) )
  {
    i--;
    if (   (r == (ULONG)(cmap[i].red))
        && (g == (ULONG)(cmap[i].green))
        && (b == (ULONG)(cmap[i].blue))  ) match = i;
  }
  return(match);
}
 
/*
 *
 */
void
ACT_Setup_CMAP(new_cmap,csize,offset,cmap_function)
ColorReg new_cmap[];
ULONG csize,*offset,cmap_function;
{
  ACTION *act;
  ColorReg *act_cmap;
  CMAP_HDR *cmap_hdr;
  LONG i,off,new_cmap_flag;

  if (   (x11_display_type == TRUE_COLOR)
      || (x11_cmap_flag == FALSE) )
  {
    act = &action[action_cnt];
    action_cnt++;
    act->type = ACT_CMAP;
    act->time = 0;
    cmap_hdr = (CMAP_HDR *)
                 malloc( csize * sizeof(ColorReg) + sizeof(CMAP_HDR));
    if (cmap_hdr == 0) TheEnd1("ACT_Setup_CMAP: malloc failed\n");
    act->data = (UBYTE *)cmap_hdr;
    act_cmap = (ColorReg *)cmap_hdr->data;
    cmap_hdr->cmap_size = csize;

    for(i=0; i<csize; i++)
    {
      act_cmap[i].red   = new_cmap[i].red;
      act_cmap[i].green = new_cmap[i].green;
      act_cmap[i].blue  = new_cmap[i].blue;
      if ( (x11_display_type == TRUE_COLOR) && (anm_map_flag == TRUE) )
         act_cmap[i].map = new_cmap[i].map = X11_Get_True_Color(
		 new_cmap[i].red, new_cmap[i].green, new_cmap[i].blue, 8);
      else act_cmap[i].map = new_cmap[i].map = i;
    }
    return;
  }

  new_cmap_flag = TRUE;
  if (   (cmap_function & CMAP_ALLOW_REMAP) 
      && (cmap_try_to_1st == TRUE) 
      && (act_first_cmap != 0) )
  {
    LONG j,match;

    DEBUG_LEVEL1 fprintf(stderr,"CMAP: <%ld> remapping %ld\n",cur_index,csize);
    new_cmap_flag = FALSE;

    cmap_hdr = (CMAP_HDR *) act_cur_cmap->data;
    act_cmap = (ColorReg *) cmap_hdr->data;

    for(i=0; i<csize; i++)
    {
      match = -i;
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

          act_cmap[cur_index].map = new_cmap[i].map = 
				x11_cmap_size - 1 - cur_index;
          cmap_hdr->cmap_size++;
          cur_index++;
        }
        else new_cmap_flag = TRUE;
      }
    } /* end of for i */
    DEBUG_LEVEL1 fprintf(stderr,"<%ld>\n",cur_index);
    if (new_cmap_flag == FALSE) return;
  }

  /* If above try_to_fit routine failed or wasn't called then
   * forcibly remap new cmap into old one if appropriate.
   */
  if (   (cmap_function & CMAP_ALLOW_REMAP) 
      && (cmap_map_to_1st == TRUE) 
      && (act_first_cmap != 0) )
  {
    LONG match;

    cmap_hdr = (CMAP_HDR *) act_cur_cmap->data;
    act_cmap = (ColorReg *) cmap_hdr->data;

    for(i=0; i<csize; i++)
    {
      match = CMAP_Find_Closest( (ULONG)(new_cmap[i].red), 
		(ULONG)(new_cmap[i].green),
      		(ULONG)(new_cmap[i].blue),act_cmap,cmap_hdr->cmap_size);
      new_cmap[i].map = act_cmap[match].map;
    }
    return;
  }

  /* Create a new cmap and CMAP action
   */
  if (   (new_cmap_flag == TRUE)
      || (act_first_cmap == 0) )
  {
   int *remap;

    DEBUG_LEVEL1 fprintf(stderr,"CMAP: new %ld\n",csize);
    act = &action[action_cnt]; action_cnt++;

    remap = 0;
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

    remap = (int *) malloc( csize * sizeof( int ) );
    if (remap == 0) TheEnd1("CMAP_ACT: malloc error\n");
    for(i=0; i<csize; i++) remap[i] = i;
    for(i=0; i<csize; i++) new_cmap[i].map = i;

    if (   (cmap_function & CMAP_ALLOW_REMAP)  /* allow remapping */ 
	&& (cmap_luma_sort == TRUE) )    /* qsort on */
    {
      qsort( (char *)new_cmap, csize, sizeof(ColorReg), ColorComp);
      for(i=0; i<csize; i++) remap[ new_cmap[i].map ] = i;
    }

   /* NOTE: NEED TO DO BETTER TO PRESERVE ORIGINAL CMAP */
    for(cur_index = 0; cur_index < csize; cur_index++)
    {
      ULONG k;
	/* need to put RGB back in original order */
      k = new_cmap[cur_index].map;
      cmap[k].red   = act_cmap[k].red   = new_cmap[cur_index].red;
      cmap[k].green = act_cmap[k].green = new_cmap[cur_index].green;
      cmap[k].blue  = act_cmap[k].blue  = new_cmap[cur_index].blue;
    }

    for(cur_index = 0; cur_index < csize; cur_index++)
    {
      new_cmap[cur_index].red   = act_cmap[cur_index].red;
      new_cmap[cur_index].green = act_cmap[cur_index].green;
      new_cmap[cur_index].blue  = act_cmap[cur_index].blue;
      if (cmap_function & CMAP_ALLOW_REMAP)
      {
         act_cmap[cur_index].map = cmap[cur_index].map =
	 new_cmap[cur_index].map =
         	x11_cmap_size - 1 - remap[cur_index];
      } 
      else
         cmap[cur_index].map = act_cmap[cur_index].map 
			       = new_cmap[cur_index].map = cur_index;
    }
    if (remap != 0) free(remap);
    cur_index = csize;
  } /* end of new action */
}

/*
 *
 */
void
ACT_Get_Next_Action(act)
ACTION **act;
{
 *act = &action[action_cnt];
 action_cnt++;
 (*act)->data = 0;
}

/*
 *
 */
void
ACT_Setup_Packed(act,time,xpos,ypos,xsize,ysize,dsize,image_ptr)
ACTION *act;
LONG time;
LONG xpos,ypos;
LONG xsize,ysize,dsize;
UBYTE *image_ptr;
{
  ACT_PACKED_HDR *act_pk_hdr; 
  ULONG pk_size,pk_image_size;
  UBYTE *t_pic;

  if (dsize == 3) pk_size = 4;
  else pk_size = dsize;
  pk_image_size = xsize * ysize;
  if (pk_image_size % pk_size) pk_image_size = (pk_image_size/pk_size) + 1;
  else pk_image_size /= pk_size;

  act_pk_hdr = (ACT_PACKED_HDR *) malloc( sizeof(ACT_PACKED_HDR) );
  if (act_pk_hdr == 0) TheEnd1("ACT Setup PACKED: can't allocate memory.\n");

  UTIL_Pack_Image(act_pk_hdr->data,image_ptr,xsize,ysize,dsize);
  free(image_ptr);

  act->type = ACT_PACKED;
  act->time = time;
  act->data = (UBYTE *) act_pk_hdr;
  act_pk_hdr->xpos = xpos;
  act_pk_hdr->ypos = ypos;
  act_pk_hdr->xsize = xsize;
  act_pk_hdr->ysize = ysize;
}

/*
 *
 */
void
ACT_Setup_Mapped(act,time,image_ptr,map,map_size,xpos,ypos,xsize,ysize,
		already_mapped,clip_ptr)
ACTION *act;
LONG time;
UBYTE *image_ptr;
ColorReg *map;
LONG map_size;
LONG xpos,ypos,xsize,ysize;
ULONG already_mapped;
UBYTE *clip_ptr;
{
  ULONG line_size;
  ULONG pixmap_type;
  XImage *image;
  UBYTE *t_pic;

/*
  if (pack_flag) 
    ACT_Setup_Packed(act,time,image_ptr,map,map_size,xpos,ypos,xsize,ysize);
*/

  line_size = X11_Get_Line_Size(xsize);
  if (x11_display_type == MONOCHROME)
  {
    pixmap_type = XYPixmap;
    t_pic = (UBYTE *) malloc(ysize * line_size);
    if (t_pic == 0) TheEnd1("X11_Get_Image: t_pic malloc failed.\n");
    if (already_mapped == FALSE)
      UTIL_Mapped_To_Bitmap(t_pic,image_ptr,map,map_size,xsize,ysize,line_size);
    else memcpy((char *)t_pic,(char *)image_ptr,(ysize*line_size) );
    free(image_ptr);
    image_ptr = t_pic;
  }
  else 
  {
    pixmap_type = ZPixmap;
    if ( (x11_display_type == TRUE_COLOR) && (already_mapped == FALSE) )
    {
      t_pic = (UBYTE *) malloc(ysize * line_size);
      if (t_pic == 0) TheEnd1("X11_Get_Image: t_pic malloc failed.\n");
      UTIL_Sub_Mapped_To_True(t_pic,image_ptr,map,xsize,ysize,0,0,xsize);
      free(image_ptr);
      image_ptr = t_pic;
    }
  }

  if (anim_flags & ANIM_LACE)
  {
    if ((ypos & 0x01) && (ysize <= 1))  /* if odd single line then NOP */
    {
      free(image_ptr);
      act->type = ACT_NOP;
      act->data = 0;
      return;
    }
    else
    {
      ULONG tmp_ysize;

      tmp_ysize = ysize>>1;
      if ( !(ypos & 0x01) && (ysize & 0x01) ) tmp_ysize++;

      if (clip_ptr != 0) /* reduce clip in half */
      {
	ULONG tmp_xsize,typos,tysize;

        typos = ypos; tysize = ysize;
	tmp_xsize = imagex/8; if (imagex % 8) tmp_xsize++;
        t_pic = (UBYTE *)malloc( tmp_ysize * tmp_xsize );
        if (t_pic == 0) TheEnd1("X11_Get_Image: lace clip malloc failed.\n");
        UTIL_Half_Image_Height(t_pic,clip_ptr,xpos,&typos,
					xsize,&tysize,tmp_xsize);
        free(clip_ptr);
        clip_ptr = t_pic;
       
      }

      t_pic = (UBYTE *)malloc( tmp_ysize * line_size );
      if (t_pic == 0) TheEnd1("X11_Get_Image: lace t_pic malloc failed.\n");
      UTIL_Half_Image_Height(t_pic,image_ptr,xpos,&ypos,
					xsize,&ysize,line_size);
      free(image_ptr);
      image_ptr = t_pic;

    }
  }

  image = XCreateImage(theDisp, DefaultVisual(theDisp,DefaultScreen(theDisp)),
	x11_depth,pixmap_type,0,NULL,xsize,ysize,x11_bytes_pixel, line_size);
  if (image == 0) TheEnd1("ACT_Setup_Mapped: can't create Image.\n");
  image->data = (char *)image_ptr;

  act->time = time;

  if (clip_ptr != 0)
  {
    ACT_CLIP_HDR *act_cp_hdr;
    act_cp_hdr = (ACT_CLIP_HDR *) malloc( sizeof(ACT_CLIP_HDR) );
    if (act_cp_hdr == 0) TheEnd1("ACT_Setup_Mapped: can't malloc for clip.\n");

    act->type = ACT_CLIP;
    act->data = (UBYTE *) act_cp_hdr;
    act_cp_hdr->xpos = xpos;
    act_cp_hdr->ypos = ypos;
    act_cp_hdr->xsize = xsize;
    act_cp_hdr->ysize = ysize;
    act_cp_hdr->image = image;
    act_cp_hdr->clip_ptr = clip_ptr;
  }
  else
  if (pixmap_flag == FALSE)
  {
    ACT_IMAGE_HDR *act_im_hdr; 
    act_im_hdr = (ACT_IMAGE_HDR *) malloc( sizeof(ACT_IMAGE_HDR) );
    if (act_im_hdr == 0) TheEnd1("ACT Setup IMAGE: can't allocate memory.\n");

    act->type = ACT_IMAGE;
    act->data = (UBYTE *) act_im_hdr;
    act_im_hdr->xpos = xpos;
    act_im_hdr->ypos = ypos;
    act_im_hdr->xsize = xsize;
    act_im_hdr->ysize = ysize;
    act_im_hdr->image = image;
  }
  else
  {
    ACT_PIXMAP_HDR *act_pm_hdr;
    act_pm_hdr = (ACT_PIXMAP_HDR *) malloc( sizeof(ACT_PIXMAP_HDR) );
    if (act_pm_hdr == 0) TheEnd1("ACT_SETUP_PIXMAP: can't allocate memory.\n");

    act->type = ACT_PIXMAP;
    act->data = (UBYTE *) act_pm_hdr;
    act_pm_hdr->xpos = xpos;
    act_pm_hdr->ypos = ypos;
    act_pm_hdr->xsize = xsize;
    act_pm_hdr->ysize = ysize;
    act_pm_hdr->pixmap = XCreatePixmap(theDisp,
	DefaultRootWindow(theDisp), xsize, ysize, x11_depth);
    /* NOTE: should make sure CreatePixmap was successful */
    DEBUG_LEVEL1 fprintf(stderr,"PIXMAP pixmap = %lx\n",act_pm_hdr->pixmap);
    XPutImage(theDisp, act_pm_hdr->pixmap, theGC, image, 0,0,0,0,xsize,ysize);
    XDestroyImage(image);
  }

}

