
/*
 * xanim_fli.c
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
#include <stdio.h>
#include "xanim.h"
#include "xanim_fli.h" 

/*POD */
extern buff_flag;
extern optimize_flag;

void Read_Fli_Header();
void Read_Fli_Frame_Header();
void Read_Fli_File();
void Decode_Fli_BRUN();
void Decode_Fli_LC();
void Decode_Fli_LC7();
void Read_Fli_COLOR();
void Fli_Buffer_Action();

ULONG Fli_Get_Word();
ULONG Fli_Get_HalfWord();

static ColorReg fli_cmap[FLI_MAX_COLORS];
static Fli_Header fli_hdr;
static Fli_Frame_Header frame_hdr;

void Read_Fli_Header(fp,fli_hdr)
FILE *fp;
Fli_Header *fli_hdr;
{
  int i;

  fli_hdr->size   = Fli_Get_Word(fp);
  fli_hdr->magic  = Fli_Get_HalfWord(fp);
  fli_hdr->frames = Fli_Get_HalfWord(fp);
  fli_hdr->width  = Fli_Get_HalfWord(fp);
  fli_hdr->height = Fli_Get_HalfWord(fp);
  fli_hdr->res1   = Fli_Get_HalfWord(fp);
  fli_hdr->flags  = Fli_Get_HalfWord(fp);
  fli_hdr->speed  = Fli_Get_HalfWord(fp);
  fli_hdr->next   = Fli_Get_Word(fp);
  fli_hdr->frit   = Fli_Get_Word(fp);
  for(i=0;i<102;i++) fgetc(fp);   /* ignore unused part of Fli_Header */

  imagex=fli_hdr->width;
  imagey=fli_hdr->height;
  if ( (fli_hdr->magic != 0xaf11) && (fli_hdr->magic != 0xaf12) )
  {
   fprintf(stderr,"imagex=%lx imagey=%lx\n",imagex,imagey);
   fprintf(stderr,"Fli Header Error magic %lx not = 0xaf11\n",fli_hdr->magic);
   TheEnd();
  }
}

void Read_Fli_Frame_Header(fp,frame_hdr)
FILE *fp;
Fli_Frame_Header *frame_hdr;
{
  int i;

  frame_hdr->size = Fli_Get_Word(fp);
  frame_hdr->magic = Fli_Get_HalfWord(fp);
  frame_hdr->chunks = Fli_Get_HalfWord(fp);
  for(i=0;i<8;i++) fgetc(fp);	/* ignore unused part of Fli_Frame_Header */

  if (frame_hdr->magic != 0xf1fa)
  {
    fprintf(stderr,"Frame_Header Error magic %lx not = 0xf1fa\n",
							frame_hdr->magic);
    TheEnd();
  }
}

void Read_Fli_File(fname)
char *fname;
{
  FILE *fin;
  int i,j,ret;
  ACTION *act;
  UBYTE *pic;
  ULONG pic_size;
  ULONG second_frame;


  if ( (fin=fopen(fname,"r")) == 0)
  { 
    fprintf(stderr,"can't open Fli File %s for reading\n",fname); 
    TheEnd();
  }
 
  Read_Fli_Header(fin,&fli_hdr);

  if (verbose) 
  {
   fprintf(stderr,"Reading FLI File %s\n",fname);
   fprintf(stderr,"   Size  %ldx%ld Frames = %ld\n",fli_hdr.width,
            fli_hdr.height,fli_hdr.frames);
  }

  /* Start off with global colormap
   */
  imagec = FLI_MAX_COLORS;
  for(i = 0; i < FLI_MAX_COLORS; i++) 
  {
    if (i < x11_cmap_size)
    {
      fli_cmap[i].red   = cmap[i].red;
      fli_cmap[i].green = cmap[i].green;
      fli_cmap[i].blue  = cmap[i].blue;
      fli_cmap[i].map   = i;
    }
    else /* for displays with cmaps larger than 256 colors */
    {
      fli_cmap[i].red   = 0;
      fli_cmap[i].green = 0;
      fli_cmap[i].blue  = 0;
      fli_cmap[i].map   = i;
    }
  }

  /* Allocate image buffer so deltas may be applied if buffering
   */
  pic_size = imagex * imagey;
  if (buff_flag == TRUE)
  {
   pic = (UBYTE *) malloc( pic_size );
   if (pic == 0) TheEnd1("BufferAction: malloc failed");
  }

  /* in case there isn't a 2nd frame */
  second_frame = action_cnt;
  for(i = 0; i < fli_hdr.frames; i++)
  {
    ULONG fli_time;
    UBYTE *t_pic;

    /* in order to loop back to 2nd frame */
    if (i == 1) second_frame = action_cnt;

    Read_Fli_Frame_Header(fin, &frame_hdr);

    if (frame_hdr.chunks==0)  /* this frame is for timing purposes */
    {
      if (debug >= DEBUG_LEVEL1) 
		fprintf(stderr," FLI DELAY %ld\n",fli_hdr.speed);

      ACT_Get_Next_Action(&act);
      act->type = ACT_DELAY;
      if (jiffy_flag) act->time = jiffy_flag;
      else act->time = fli_hdr.speed * MS_PER_60HZ;
      act->data = 0;
    }
    else /* this frame has real data in it */
    {
      /* Loop through chunks in the frame
       */
      for(j=0;j<frame_hdr.chunks;j++)
      {
        ULONG chunk_size,chunk_type;
   
        chunk_size = Fli_Get_Word(fin);
        chunk_type = Fli_Get_HalfWord(fin);
        switch(chunk_type)
        {
          case CHUNK_4:
                 if (debug >= DEBUG_LEVEL1) 
			fprintf(stderr," FLI COLOR(4)\n");
		 Read_Fli_COLOR(fin,0);
		 break;
 
          case FLI_COLOR:
                 if (debug >= DEBUG_LEVEL1) 
			fprintf(stderr," FLI COLOR(11)\n");
		 Read_Fli_COLOR(fin,2);
		 break;

          case FLI_LC7:
		{
		 ULONG xpos,ypos,xsize,ysize;
		 UBYTE *lc_data;

		 if (debug >= DEBUG_LEVEL1) 
			fprintf(stderr," FLI LC(7)\n");

	         ACT_Get_Next_Action(&act);
		 fli_time = (jiffy_flag)?(jiffy_flag)
				    :(fli_hdr.speed * MS_PER_60HZ);

 		 lc_data = (UBYTE *) malloc( chunk_size-6 );
 		 if (lc_data == 0) TheEnd1("Fli LC7: malloc failed");
 		 ret = fread( lc_data, (chunk_size-6), 1, fin);
 		 if (ret != 1) TheEnd1("Fli LC7: read failed");

                 if (buff_flag == TRUE)
		 {
		   ULONG xpos,ypos,xsize,ysize,new_size;
		   Decode_Fli_LC7( lc_data, pic, &xpos,&ypos,&xsize,&ysize);
		   new_size = xsize * ysize;
                   t_pic = (UBYTE *) malloc( new_size );
                   if (t_pic == 0) TheEnd1("Buff FLI LC7: malloc failed");
		   UTIL_Sub_Image(t_pic,pic,xsize,ysize,xpos,ypos,imagex);
		   ACT_Setup_IMAGE1(act,fli_time,xpos,ypos,xsize,ysize,t_pic);
		   free(lc_data);
		 }
		 else
		 {
		   act->type = ACT_FLI_LC7;
 		   act->time = fli_time;
 		   act->data = lc_data;
		 }
		}
		break;
 
 
          case FLI_LC:
		{
		 ULONG xpos,ypos,xsize,ysize;
		 UBYTE *lc_data;

		 if (debug >= DEBUG_LEVEL1) 
			fprintf(stderr," FLI LC(12)\n");

	         ACT_Get_Next_Action(&act);
		 fli_time = (jiffy_flag)?(jiffy_flag)
				    :(fli_hdr.speed * MS_PER_60HZ);

 		 lc_data = (UBYTE *) malloc( chunk_size-6 );
 		 if (lc_data == 0) TheEnd1("Fli LC: malloc failed");
 		 ret = fread( lc_data, (chunk_size-6), 1, fin);
 		 if (ret != 1) TheEnd1("Fli LC: read failed");

                 if (buff_flag == TRUE)
		 {
		   ULONG xpos,ypos,xsize,ysize,new_size;
		   Decode_Fli_LC( lc_data, pic, &xpos,&ypos,&xsize,&ysize);
		   new_size = xsize * ysize;
                   t_pic = (UBYTE *) malloc( new_size );
                   if (t_pic == 0) TheEnd1("Buff FLI LC: malloc failed");
		   UTIL_Sub_Image(t_pic,pic,xsize,ysize,xpos,ypos,imagex);
		   ACT_Setup_IMAGE1(act,fli_time,xpos,ypos,xsize,ysize,t_pic);
		   free(lc_data);
		 }
		 else
		 {
		   act->type = ACT_FLI_LC;
 		   act->time = fli_time;
 		   act->data = lc_data;
		 }
		}
		break;
 
          case FLI_BLACK:
		{
                 if (debug >= DEBUG_LEVEL1) 
			fprintf(stderr," FLI BLACK(13)\n");
		 if (chunk_size > 6) 
                       TheEnd1("Read_Fli_BLACK different than expected\n");

	         ACT_Get_Next_Action(&act);
		 fli_time = (jiffy_flag)?(jiffy_flag)
				    :(fli_hdr.speed * MS_PER_60HZ);

                 if (buff_flag == TRUE)
		 {
                   t_pic = (UBYTE *) malloc( pic_size );
                   if (t_pic == 0) TheEnd1("Buff FLI BLACK: malloc failed");
                   memset(t_pic,0x00,pic_size); /* create black image */
                   memset(pic,0x00,pic_size);   /* clear pic buffer */
		   ACT_Setup_IMAGE1(act,fli_time,0,0,imagex,imagey,t_pic);
		 }
		 else
		 {
		   act->type = ACT_FLI_BLACK;
 		   act->time = fli_time;
 		   act->data = 0;
		 }
		}
		break;
 
          case FLI_BRUN:
		{
		 UBYTE *brun_data;

                 if (debug >= DEBUG_LEVEL1) 
			fprintf(stderr," FLI BRUN(15)\n");
	         ACT_Get_Next_Action(&act);
		 fli_time = (jiffy_flag)?(jiffy_flag)
				    :(fli_hdr.speed * MS_PER_60HZ);
	         /* Read BRUN Data 
		  */
 		 brun_data = (UBYTE *) malloc( chunk_size-6 );
 		 if (brun_data == 0) TheEnd1("FLI BRUN: malloc failed");
 		 ret = fread( brun_data, (chunk_size-6), 1, fin);
 		 if (ret != 1) TheEnd1("FLI BRUN: read failed");

                 if (buff_flag == TRUE)
		 {
		   Decode_Fli_BRUN(brun_data,pic);
                   t_pic = (UBYTE *) malloc( pic_size );
                   if (t_pic == 0) TheEnd1("Buff FLI BRUN: malloc failed");
		   memcpy((char *)t_pic,(char *)pic,pic_size);
		   ACT_Setup_IMAGE1(act,fli_time,0,0,imagex,imagey,t_pic);
		   free(brun_data);
		 }
		 else
		 {
		   act->type = ACT_FLI_BRUN;
 		   act->time = fli_time;
 		   act->data = brun_data;
		 }
		}
		break;
 
          case FLI_COPY:
		{
		 ULONG kludge;
		 UBYTE *copy_data;

		 if (imagex == 320) kludge = 4;
                 else kludge = 6;

                 if (debug >= DEBUG_LEVEL1) 
			fprintf(stderr," FLI COPY(16)\n");
		 if ( (chunk_size-kludge) != pic_size)
			TheEnd1("FLI COPY: not same size as image\n");
	         ACT_Get_Next_Action(&act);
		 fli_time = (jiffy_flag)?(jiffy_flag)
				    :(fli_hdr.speed * MS_PER_60HZ);
 		 copy_data = (UBYTE *) malloc( chunk_size-kludge );
 		 if (copy_data == 0) TheEnd1("FLI COPY: malloc failed");
 		 ret = fread( copy_data, (chunk_size-kludge), 1, fin);
 		 if (ret != 1) TheEnd1("FLI COPY: read failed");

		 if (buff_flag == TRUE) 
		 {
		      /* for next images */
		   memcpy((char *)pic,(char *)copy_data,pic_size); 
		   ACT_Setup_IMAGE1(act,fli_time,0,0,imagex,imagey,copy_data);
		 }
		 else
		 {
		   act->type = ACT_FLI_COPY;
 		   act->time = fli_time;
 		   act->data = copy_data;
		 }
		}
		break;

           default: 
             {
               int i;
	       fprintf(stderr,"FLI Unsupported Chunk: type = %lx size=%lx\n",
					chunk_type,chunk_size);
               for(i=0;i<(chunk_size-6);i++) fgetc(fin);
               if (chunk_size & 0x01) fgetc(fin); 
             }
	         
		 break;
        } /* end of switch */

        if (chunk_size & 0x01)
        {
/*
          fprintf(stderr,"chunk_size=%lx ODD\n",chunk_size);
*/
        }
      } /* end of chunks is frame */
    } /* end of not Timing Frame */
  } /* end of frames in file */
  if ( (buff_flag) && (pic != 0) ) free(pic);        
  fclose(fin);

  {
    ULONG action_number;

    action_number = action_cnt - action_start;
    anim[anim_cnt].frame_lst =
             (int *)malloc(sizeof(int) * (action_number+1));
    if (anim[anim_cnt].frame_lst == NULL)
        TheEnd1("FLI_ANIM: couldn't malloc for frame_lst\0");
    for(i=0; i < action_number; i++)
        anim[anim_cnt].frame_lst[i]=action_start+i;
    anim[anim_cnt].frame_lst[action_number] = -1;
    anim[anim_cnt].loop_frame = 0;
/* second_frame - action_start; */
    anim[anim_cnt].last_frame = action_number-1;
  }

}


/*
 * Routine to Decode a Fli BRUN chunk
 */
void Decode_Fli_BRUN(data,image_out)
unsigned char *data,*image_out;
{
  ULONG i,j,k,packets,size,x,offset;

  for(i=0; i<imagey; i++)
  {
    offset = i * imagex;
    packets = *data++;

    x=0;
    for(j= 0; j < packets; j++)
    {
      size = *data++;
      if (size & 0x80)         /* size < 0 so there is -size unique bytes */
      {
        size = 256-size; 
        for(k= x; k < (x+size); k++) 
              image_out[k+offset] = *data++;
        x += size;   
      }
      else                     /* size is pos repeat next byte size times */
      {
        ULONG d;
        d = *data++;
        for(k= x; k < (x+size); k++) image_out[k+offset] = d;
        x+=size;   
      }
    } /* end of packets per line */
  } /* end of line */
}

/*
 * Routine to Decode an Fli LC chunk
 */
void Decode_Fli_LC(data,image_out,xpos,ypos,xsize,ysize)
unsigned char *data,*image_out;
ULONG *xpos,*ypos,*xsize,*ysize;
{
  ULONG i,j,k,packets,size,x,offset;
  ULONG start,lines,skip,minx,maxx;

  start = *data++; start |= *data++ << 8;  /* lines to skip */
  lines = *data++; lines |= *data++ << 8;  /* number of lines */

  minx = imagex; 
  maxx = 0;
 
  for(i=start;i<(start+lines);i++)
  {
    offset = i * imagex;
    packets = *data++;

    x=0;
    for(j=0;j<packets;j++)
    {
      skip = *data++;   /* this is the skip count */
      size = *data++;

      if (j==0) if (skip < minx) minx = skip;
      x+=skip;
      if (size==0) 
      {
/*
        size = *data++;
        fprintf(stderr,"size==0 what to we do? next is %ld\n",size);
        TheEnd();
*/
      }
      else if (size & 0x80) /* next byte repeated -size times */
      {
        ULONG d;
        size = 256-size; 
        d = *data++;
        for(k=x;k<(x+size);k++) image_out[k+offset] = d;
        x+=size;   
      }
      else /* size is pos */
      {
        for(k=x;k<(x+size);k++) image_out[k+offset] = *data++;
        x+=size;   
      }
    } /* end of packets per line */
    if (x > maxx) maxx = x;
  } /* end of line */

  if (optimize_flag == TRUE)
  {
   *ypos = start;
   *ysize = lines;
   *xpos = minx;
   if (maxx > imagex) maxx=imagex;
   if (maxx > minx) *xsize = maxx - minx;
   else *xsize = imagex - minx;
  }
  else
  {
   *ypos = 0;
   *ysize = imagey;
   *xpos = 0;
   *xsize = imagex;
  }

  if (debug >= DEBUG_LEVEL1)
        fprintf(stderr,"      LC: xypos=<%ld %ld> xysize=<%ld %ld>\n",
                        *xpos,*ypos,*xsize,*ysize);

}


/*
 * Routine to read an Fli COLOR chunk
 */
void Read_Fli_COLOR(fin,color_shift)
FILE *fin;
ULONG color_shift;
{
 ULONG i,k,l,c_index,packets,skip,colors,cnt;

 packets = Fli_Get_HalfWord(fin);
 c_index = 0;

 if (debug >= DEBUG_LEVEL1) fprintf(stderr,"   packets = %ld\n",packets);
 cnt=2;
 for(k=0;k<packets;k++)
 {
  skip = fgetc(fin);
  colors=fgetc(fin);
  if (debug >= DEBUG_LEVEL1) 
		fprintf(stderr,"      skip %ld colors %ld\n",skip,colors);
  cnt+=2;
  if (colors==0) colors=FLI_MAX_COLORS;
  if (colors > x11_cmap_size)
  {
   fprintf(stderr,"FLI has %ld colors and the display only supports %ld.\n",
         colors,x11_cmap_size);
   TheEnd();
  }
  c_index += skip;
  for(l = 0; l < colors; l++)
  {
   fli_cmap[c_index].red   = fgetc(fin) << color_shift;
   fli_cmap[c_index].green = fgetc(fin) << color_shift;
   fli_cmap[c_index].blue  = fgetc(fin) << color_shift;
   if (debug >= DEBUG_LEVEL5) fprintf(stderr,"         %ld)  <%lx %lx %lx>\n", 
		l,fli_cmap[l].red, fli_cmap[l].green,fli_cmap[l].blue);
   c_index++;
  } /* end of colors */
  cnt+= 3 * colors;
 } /* end of packets */
 if (cnt&0x01) fgetc(fin);  /* read pad byte if needed */

 ACT_Setup_CMAP(fli_cmap,FLI_MAX_COLORS,&k,2);

}


/* Routine to read a little endian long word.
 * Note
 */
ULONG Fli_Get_Word(fp)
FILE *fp;
{
 ULONG ret;

 ret =  fgetc(fp);
 ret |= fgetc(fp) << 8;
 ret |= fgetc(fp) << 16;
 ret |= fgetc(fp) << 24;
 return ret;
}

/* Routine to read a little endian half word.
 */
ULONG Fli_Get_HalfWord(fp)
FILE *fp;
{
 ULONG ret;

 ret =  fgetc(fp);
 ret |= fgetc(fp) << 8;
 return ret;
}


/*
 * Routine to Decode an Fli LC chunk
 */
void Decode_Fli_LC7(data,image_out,xpos,ypos,xsize,ysize)
unsigned char *data,*image_out;
ULONG *xpos,*ypos,*xsize,*ysize;
{
  ULONG i,j,x,y;
  ULONG packets,blocks,xoff,cnt,tmp_data0,tmp_data1;
  ULONG minx,maxx,miny;
  UBYTE *ptr;
  

  minx = imagex;
  maxx = 0;
  miny = imagey;

  ptr = image_out;
  y = 0;
  x = 0;
  packets = *data++; packets |= *data++ << 8;  /* # of packets */

  if (debug >= DEBUG_LEVEL5) fprintf(stderr,"pkts=%ld\n",packets);

  for(i=0; i < packets; i++)
  {
   
    blocks = *data++; blocks |= *data++ << 8; 

    if (debug >= DEBUG_LEVEL5) fprintf(stderr,"     %ld) ",i);

    if (blocks & 0x8000)  /* neg lines to jump */
    {
      blocks = 0x10000 - blocks;
      y += blocks;
      if (debug >= DEBUG_LEVEL5) fprintf(stderr,"     yskip %ld",blocks);
      blocks = *data++; blocks |= *data++ << 8;
    }
    if (debug >= DEBUG_LEVEL5) fprintf(stderr,"     blocks = %ld\n",blocks);

    if (optimize_flag == TRUE) if (y < miny) miny = y;

    ptr  = (UBYTE *)( (ULONG)(image_out) + y*imagex);
    x = 0;

    for(j=0; j < blocks; j++)
    {
      xoff = *data++;  /* x offset */
      cnt  = *data++;  /* halfword count */
      ptr += xoff;
      x += xoff;

      if (debug >= DEBUG_LEVEL5) 
        fprintf(stderr,"          %ld) xoff %lx  cnt = %lx",j,xoff,cnt);

      if (cnt & 0x80)
      {
        cnt = 256 - cnt;
        if (debug >= DEBUG_LEVEL5) 
            fprintf(stderr,"               NEG %ld\n",cnt);
        tmp_data0 = *data++;
        tmp_data1 = *data++;
        while(cnt--)
        {
          *ptr++ = (UBYTE)tmp_data0;
          *ptr++ = (UBYTE)tmp_data1;
        }

        if (optimize_flag == TRUE)
        {  
          if (x < minx) minx = x;
          x += (cnt<<1);
          if (x > maxx) maxx = x;
        }
      }
      else
      {   /* cnt is number of unique pairs of bytes */
        if (debug >= DEBUG_LEVEL5) 
           fprintf(stderr,"               POS %ld\n",cnt);
        if (optimize_flag == TRUE)
        {
          if (cnt == 1)  /* potential NOPs just to move x */
          {
            if ( (*ptr != *data) || (ptr[1] != data[1]) )  
            {
              if (x < minx) minx = x; 
              x += (cnt<<1);
              if (x > maxx) maxx = x;
            }
          }
	  else
          {
            if (x < minx) minx = x; 
            x += (cnt<<1);
            if (x > maxx) maxx = x;
          }
        }
           
        while(cnt--) 
        {
          *ptr++ = *data++;
          *ptr++ = *data++;
        }
      } 

    } /* (j) end of blocks */
    y++;
  } /* (i) end of packets */

  if (optimize_flag == TRUE)
  {
    if (minx >= imagex) minx = 0; 
    if (miny >= imagey) miny = 0; 

    *xpos = minx;
    *ypos = miny;

    if (maxx > minx) *xsize = maxx - minx;
    else             *xsize = imagex - minx;

    if (y > miny) *ysize = y - miny;
    else          *ysize = imagey - miny;
  }
  else
  {
    *xpos = 0;
    *ypos = 0;
    *xsize = imagex;
    *ysize = imagey;
  }

}


