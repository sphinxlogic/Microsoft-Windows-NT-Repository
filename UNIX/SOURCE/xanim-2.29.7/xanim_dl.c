
/*
 * xanim_dl.c
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
#include "xanim_dl.h"

static ULONG dl_version;
static ULONG dl_format;
static ULONG dl_time;
static UBYTE dl_title[21];
static UBYTE dl_author[21];
static ULONG dl_num_of_images;
static ULONG dl_num_of_frames;
static ULONG dl_frame_cnt;
static ULONG dl_loop_frame;
static ULONG *dl_frame_lst;

static ColorReg dl_cmap[DL_MAX_COLORS];

/*
 *
 */
int Is_DL_File(filename)
char *filename;
{
  FILE *fin;
  ULONG data0,data1;

  if ( (fin=fopen(filename,"r")) == 0)
  {
   fprintf(stderr,"can't open %s\n",filename);
   TheEnd();
  }
  data0 = fgetc(fin);
  data1 = fgetc(fin);
  fclose(fin);
  if (data0 == 0x01) { fclose(fin); return(TRUE); }
  if (data0 == 0x02)
  {
    if ( (data1 == 0x01) || (data1 == 0x02) ) return(TRUE);
  }
  return(FALSE);
}

/*
 *
 */
void DL_Read_File(fname,anim_hdr)
ANIM_HDR *anim_hdr;
char *fname;
{
  FILE *fin;
  LONG i,j,ret,tmp;
  ACTION *act;
  UBYTE *pic;
  ULONG pic_size;

  if ( (fin = fopen(fname,"r")) == 0)
  { 
    fprintf(stderr,"DL_Read_File: Can't open %s for reading.\n",fname); 
    TheEnd();
  }

  dl_version = fgetc(fin);  /* either 1 or 2 */

  switch(dl_version)
  {
    case DL_VERSION_1:
		dl_format = DL_FORM_MEDIUM;
		break;
    case DL_VERSION_2:
		dl_format = fgetc(fin);
		break;
    default:
		break;
  }

  switch(dl_format)
  {
    case DL_FORM_LARGE:
	imagex = DL_LARGE_XSIZE;
	imagey = DL_LARGE_YSIZE;
	break;
    case DL_FORM_MEDIUM:
	imagex = DL_MEDIUM_XSIZE;
	imagey = DL_MEDIUM_YSIZE;
	break;
    case DL_FORM_SMALL:
	imagex = DL_SMALL_XSIZE;
	imagey = DL_SMALL_YSIZE;
	break;
    default:
	fprintf(stderr,"DL_Read_File: unknown format %lx\n",dl_format);
	TheEnd();
	break;
  }

  dl_title[20] = 0;
  for(i=0; i<20; i++) 
  {
    tmp = fgetc(fin);
    dl_title[i] = (tmp)?(tmp ^ 0xff):tmp;
  }

  dl_author[0] = dl_author[20] = 0;
  if (dl_version == DL_VERSION_2)
    for(i=0; i<20; i++)
    {
      tmp = fgetc(fin);
      dl_author[i] = (tmp)?(tmp ^ 0xff):tmp;
    }

  dl_num_of_images = fgetc(fin);

  if (dl_version == DL_VERSION_1)
     dl_num_of_frames = UTIL_Get_LSB_Short(fin);
  else
     dl_num_of_frames = UTIL_Get_LSB_Long(fin);

  if (verbose == TRUE)
  {
    fprintf(stderr,"Reading DL file: version %ld  format %ld\n",
	dl_version,dl_format);
    fprintf(stderr,"                 images %ld  frames %ld\n",
	dl_num_of_images * (dl_format * 3 + 1), dl_num_of_frames );
    fprintf(stderr,"   title = %s  Author = %s\n",dl_title,dl_author);
  }

  imagec = DL_MAX_COLORS;
  for(i=0; i < DL_MAX_COLORS; i++)
  {
    dl_cmap[i].red =   ((fgetc(fin) & 0x3f) << 2);
    dl_cmap[i].green = ((fgetc(fin) & 0x3f) << 2);
    dl_cmap[i].blue =  ((fgetc(fin) & 0x3f) << 2);
  }
  ACT_Setup_CMAP(dl_cmap,DL_MAX_COLORS,&i,CMAP_DIRECT);

  
  pic_size = imagex * imagey;

  dl_time = (jiffy_flag)?(jiffy_flag):(100);

  for(j = 0; j < dl_num_of_images; j++)
  {
    switch(dl_format)
    {
      case DL_FORM_LARGE: /* large */
	pic = (UBYTE *) malloc( pic_size );
	if (pic == 0) TheEnd1("DL: malloc failed.\n");
        ret = fread(pic, pic_size, 1, fin);
	if (ret != 1) TheEnd1("DL: read of image failed format 0");
	DEBUG_LEVEL3 fprintf(stderr,"Read large format image\n");
        ACT_Get_Next_Action(&act);
	ACT_Setup_Mapped(act,dl_time,pic,dl_cmap,imagec,
			0,0,imagex,imagey,FALSE,0);
	break;

      case DL_FORM_MEDIUM: /* medium */
      case DL_FORM_SMALL: /* medium */
	{
	  ULONG x,y;
	  UBYTE *t_pic;

	  tmp = DL_LARGE_XSIZE * DL_LARGE_YSIZE;
	  pic = (UBYTE *) malloc( tmp );
	  if (pic == 0) TheEnd1("DL: malloc failed.\n");
          ret = fread(pic, tmp , 1, fin);
	  if (ret != 1) TheEnd1("DL: read of image failed format 1");
	  DEBUG_LEVEL3 fprintf(stderr,"Read small/medium image\n");

          for(y = 0; y < DL_LARGE_YSIZE; y += imagey)
            for(x = 0; x < DL_LARGE_XSIZE; x += imagex)
	    {
	      t_pic = (UBYTE *) malloc( pic_size );
	      if (t_pic == 0) TheEnd1("DL: malloc failed.\n");
	      UTIL_Sub_Image(t_pic,pic,imagex,imagey, x, y,(DL_LARGE_XSIZE),1);
              ACT_Get_Next_Action(&act);
	      ACT_Setup_Mapped(act,dl_time,t_pic,dl_cmap,imagec,
			    0,0,imagex,imagey,FALSE,0);
	    }
	}
	break;
   }
 }

 dl_frame_lst = (ULONG *)malloc( dl_num_of_frames * sizeof(ULONG) );
 if (dl_frame_lst == 0) TheEnd1("DL_Read_File: malloc failed");

 dl_loop_frame = 0;
 switch(dl_version)
 {
   case DL_VERSION_1:
	dl_frame_cnt = 0;
	for(i=0; i < dl_num_of_frames; i++)
	{
	  register ULONG tmp;
	  tmp = fgetc(fin);
	  dl_frame_lst[dl_frame_cnt] = (j % 10) - 1 + ((j / 10) - 1) * 4;
	  dl_frame_cnt++;
	}
	break;
   case DL_VERSION_2:
	DEBUG_LEVEL3 fprintf(stderr," DL reading frame lst: ");
	dl_frame_cnt = 0;
	for(i=0; i < dl_num_of_frames; i++)
	{
	  register ULONG tmp;
	  tmp  =  fgetc(fin);
	  tmp |=  ( fgetc(fin) << 8); 
	  DEBUG_LEVEL3 fprintf(stderr,"\t<%ld %lx>",i,tmp);
	  if (tmp & 0x8000)
	  {
	    if (i == (dl_num_of_frames - 1) )
		dl_loop_frame = (~tmp + 1) & 0x7fff + 1;
	    else
	      fprintf(stderr,"unknown frame ignored for now. %lx\n",tmp);
	  }
	  else
	  {
	    dl_frame_lst[dl_frame_cnt] = tmp;
	    dl_frame_cnt++;
	  }
	}
	DEBUG_LEVEL3 fprintf(stderr,"\n");
	break;
  }
  anim_hdr->frame_lst = (int *)malloc(sizeof(int) * (dl_frame_cnt+2) ); 
  if (anim_hdr->frame_lst == NULL)
      TheEnd1("DL_ANIM: couldn't malloc for frame_lst\0");

  anim_hdr->frame_lst[0] = 0;			/* cmap */
  for(i=1; i < dl_frame_cnt; i++)
      anim_hdr->frame_lst[i] = action_start + dl_frame_lst[i] + 1;
  anim_hdr->frame_lst[dl_frame_cnt + 1] = -1;
  anim_hdr->loop_frame = dl_loop_frame;
  anim_hdr->last_frame = dl_frame_cnt;
  free(dl_frame_lst);

}

