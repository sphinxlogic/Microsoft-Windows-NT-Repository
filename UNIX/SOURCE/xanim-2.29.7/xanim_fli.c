
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
#include "xanim.h"
#include "xanim_fli.h" 

void Read_Fli_Header();
void Read_Fli_Frame_Header();
void Fli_Read_File();
void Decode_Fli_BRUN();
void Decode_Fli_LC();
void Decode_Fli_LC7();
void Read_Fli_COLOR();
ULONG UTIL_Get_LSB_Long();
ULONG UTIL_Get_LSB_Short();

void ACT_Setup_CMAP();
void ACT_Get_Next_Action();
void UTIL_Sub_Image();

static ColorReg fli_cmap[FLI_MAX_COLORS];
static Fli_Header fli_hdr;
static Fli_Frame_Header frame_hdr;

/*
 *
 */
int Is_FLI_File(filename)
char *filename;
{
  FILE *fin;
  ULONG data;

  if ( (fin=fopen(filename,"r")) == 0)
  {
   fprintf(stderr,"can't open %s\n",filename);
   TheEnd();
  }

  data = UTIL_Get_LSB_Long(fin);  /* read past size */
  data = UTIL_Get_LSB_Short(fin); /* read magic */
  fclose(fin);
  if ( (data == 0xaf11) || (data == 0xaf12) ) return(TRUE);
  return(FALSE);
}


void Read_Fli_Header(fp,fli_hdr)
FILE *fp;
Fli_Header *fli_hdr;
{
  int i;

  fli_hdr->size   = UTIL_Get_LSB_Long(fp);
  fli_hdr->magic  = UTIL_Get_LSB_Short(fp);
  fli_hdr->frames = UTIL_Get_LSB_Short(fp);
  fli_hdr->width  = UTIL_Get_LSB_Short(fp);
  fli_hdr->height = UTIL_Get_LSB_Short(fp);
  fli_hdr->res1   = UTIL_Get_LSB_Short(fp);
  fli_hdr->flags  = UTIL_Get_LSB_Short(fp);
  fli_hdr->speed  = UTIL_Get_LSB_Short(fp);
  fli_hdr->next   = UTIL_Get_LSB_Long(fp);
  fli_hdr->frit   = UTIL_Get_LSB_Long(fp);
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

  frame_hdr->size = UTIL_Get_LSB_Long(fp);
  frame_hdr->magic = UTIL_Get_LSB_Short(fp);
  frame_hdr->chunks = UTIL_Get_LSB_Short(fp);
  for(i=0;i<8;i++) fgetc(fp);	/* ignore unused part of Fli_Frame_Header */

  DEBUG_LEVEL1 
	fprintf(stderr,"Frame Header size %lx  magic %lx  chunks %ld\n",
		frame_hdr->size,frame_hdr->magic,frame_hdr->chunks);

  if (   (frame_hdr->magic != 0xf1fa)
      && (frame_hdr->magic != 0xf100) )
  {
    fprintf(stderr,"Frame_Header Error magic %lx not = 0xf1fa\n",
							frame_hdr->magic);
    TheEnd();
  }
}

void Fli_Read_File(fname,anim_hdr)
char *fname;
ANIM_HDR *anim_hdr;
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
  /* NOTE: do better than this */
  imagec = FLI_MAX_COLORS;
  for(i = 0; i < FLI_MAX_COLORS; i++) 
  {
    if ( (i < x11_cmap_size) && (x11_cmap_flag == TRUE) )
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
    pic = (UBYTE *) malloc(pic_size);
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
    
    if (frame_hdr.magic == 0xf100)
    {
     int i;
     DEBUG_LEVEL1 
	fprintf(stderr,"FLI 0xf100 Frame: size = %lx\n",frame_hdr.size);
     for(i=0;i<(frame_hdr.size - 16);i++) fgetc(fin);
     if (frame_hdr.size & 0x01) fgetc(fin); 
    }
    else
    if (frame_hdr.chunks==0)  /* this frame is for timing purposes */
    {
      DEBUG_LEVEL1 fprintf(stderr," FLI DELAY %ld\n",fli_hdr.speed);

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
   
        chunk_size = UTIL_Get_LSB_Long(fin);
        chunk_type = UTIL_Get_LSB_Short(fin);
        switch(chunk_type)
        {
          case CHUNK_4:     /* FLI Color with 8 bits RGB */
                 DEBUG_LEVEL1 fprintf(stderr," FLI COLOR(4)\n");
		 Read_Fli_COLOR(fin,0);
		 break;
 
          case FLI_COLOR:     /* FLI Color with 6 bits RGB */
                 DEBUG_LEVEL1
			fprintf(stderr," FLI COLOR(11)\n");
		 Read_Fli_COLOR(fin,2);
		 break;

          case FLI_LC:
          case FLI_LC7:
          case FLI_BRUN:
          case FLI_COPY:
		{
		 UBYTE *chunk_data,kludge;
		 
		 if ( (chunk_type==FLI_COPY) && (imagex == 320)) kludge = 4;
		 else kludge = 6;
	         ACT_Get_Next_Action(&act);
		 fli_time = (jiffy_flag)?(jiffy_flag)
				    :(fli_hdr.speed * MS_PER_60HZ);
		 switch(chunk_type)
		 {
		  case FLI_LC7:
		    DEBUG_LEVEL1 fprintf(stderr," FLI LC(7)\n");
		    act->type = ACT_FLI_LC7;
		    break;
		  case FLI_LC:
		    DEBUG_LEVEL1 fprintf(stderr," FLI LC(12)\n");
		    act->type = ACT_FLI_LC;
		    break;
		  case FLI_BRUN:
		    DEBUG_LEVEL1 fprintf(stderr," FLI BRUN(15)\n");
		    act->type = ACT_FLI_BRUN;
		    break;
		  case FLI_COPY:
		    DEBUG_LEVEL1 fprintf(stderr," FLI COPY(16)\n");
		    act->type = ACT_FLI_COPY;
		    break;
		 }

		 if (chunk_size > kludge)
		 {
 		   chunk_data = (UBYTE *) malloc( chunk_size-kludge );
 		   if (chunk_data == 0) TheEnd1("FLI CHUNK: malloc failed");
 		   ret = fread( chunk_data, (chunk_size-kludge), 1, fin);
 		   if (ret != 1) TheEnd1("FLI CHUNK: read failed");
		 }
		 else TheEnd1("FLI CHUNK: invalid chunk size");

		 if (buff_flag == FALSE)
		 {
 		   act->time = fli_time;
 		   act->data = chunk_data;
		 }
                 else /* buffer it up */
		 {
		   ULONG xpos,ypos,xsize,ysize,new_size;

		   switch(chunk_type)
		   {
		     case FLI_LC7:
			Decode_Fli_LC7(fli_cmap,chunk_data, pic, 
				&xpos,&ypos,&xsize,&ysize,FALSE);
			break;
		     case FLI_LC:
			Decode_Fli_LC(fli_cmap,chunk_data, pic, 
				&xpos,&ypos,&xsize,&ysize,FALSE);
			break;
		     case FLI_BRUN:
			Decode_Fli_BRUN(fli_cmap,chunk_data,pic,FALSE);
			xpos = ypos = 0; xsize = imagex; ysize = imagey;
			break;
		     case FLI_COPY:
			if ( (chunk_size-kludge) > pic_size)
			{
			 fprintf(stderr,"chunk_size = %lx pic_size=%lx\n",
				chunk_size,pic_size);
			 TheEnd1("FLI COPY: invalid chunk_size");
			}
			memcpy((char *)pic,(char *)chunk_data,(chunk_size-6));
			xpos = ypos = 0; xsize = imagex; ysize = imagey;
			break;
		   }
		   new_size = xsize * ysize;
		   t_pic = (UBYTE *) malloc(new_size * x11_bytes_pixel);
		   if (t_pic == 0) TheEnd1 ("FLI BUFF CHUNK: malloc failed");
		   UTIL_Sub_Image(t_pic,pic,xsize,ysize,xpos,ypos,imagex,1);
		   ACT_Setup_Mapped(act,fli_time,t_pic,fli_cmap,imagec,
			xpos,ypos,xsize,ysize,FALSE,0);
		   free(chunk_data);
		 } /* end of buffer */
		}
		break;
 
          case FLI_BLACK:
		{
                 DEBUG_LEVEL1 fprintf(stderr," FLI BLACK(13)\n");
		 if (chunk_size > 6) 
                       TheEnd1("Read_Fli_BLACK different than expected\n");

	         ACT_Get_Next_Action(&act);
		 fli_time = (jiffy_flag)?(jiffy_flag)
				    :(fli_hdr.speed * MS_PER_60HZ);

                 if (buff_flag == TRUE)
		 {
                   t_pic = (UBYTE *) malloc( pic_size * x11_bytes_pixel);
                   if (t_pic == 0) TheEnd1("Buff FLI BLACK: malloc failed");
			/* create black image */
                   memset(t_pic,0x00,(pic_size * x11_bytes_pixel) ); 
			/* clear pic buffer */
                   memset(pic,0x00,pic_size);
		   ACT_Setup_Mapped(act,fli_time,t_pic,fli_cmap,imagec,
			0,0,imagex,imagey,TRUE,0);
		 }
		 else
		 {
		   act->type = ACT_FLI_BLACK;
 		   act->time = fli_time;
 		   act->data = 0;
		 }
		}
		break;
          case FLI_MINI:
		{
		  int i;
		  DEBUG_LEVEL1 fprintf(stderr," FLI MINI(18) ignored.\n");
		  for(i=0;i<(chunk_size-6);i++) fgetc(fin);
		  if (chunk_size & 0x01) fgetc(fin); 
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
      } /* end of chunks is frame */
    } /* end of not Timing Frame */
  } /* end of frames in file */
  if ( (buff_flag) && (pic != 0) ) free(pic);        
  fclose(fin);

  {
    ULONG action_number;

    action_number = action_cnt - action_start;
    anim_hdr->frame_lst =
             (int *)malloc(sizeof(int) * (action_number+1));
    if (anim_hdr->frame_lst == NULL)
        TheEnd1("FLI_ANIM: couldn't malloc for frame_lst\0");
    for(i=0; i < action_number; i++)
        anim_hdr->frame_lst[i]=action_start+i;
    anim_hdr->frame_lst[action_number] = -1;
    anim_hdr->loop_frame = 0;
    /* second_frame - action_start; */
    anim_hdr->last_frame = action_number-1;
  }

}


/*
 * Routine to Decode a Fli BRUN chunk
 */
void Decode_Fli_BRUN(map,data,image_out,map_flag)
ColorReg *map;
unsigned char *data,*image_out;
ULONG map_flag;
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
        if (map_flag == TRUE)
        {
          if (x11_bytes_pixel == 4)
            for(k=x;k<(x+size);k++)
                ((ULONG *)(image_out))[k+offset] = (ULONG)(map[*data++].map);
          else if (x11_bytes_pixel == 2)
            for(k=x;k<(x+size);k++)
                ((USHORT *)(image_out))[k+offset] = (USHORT)(map[*data++].map);
          else
            for(k=x;k<(x+size);k++)
                ((UBYTE *)(image_out))[k+offset] = (UBYTE)(map[*data++].map);
        }
        else
        {
          for(k=x;k<(x+size);k++)
                ((UBYTE *)(image_out))[k+offset] = (UBYTE)(*data++);
        }

        x += size;   
      }
      else                     /* size is pos repeat next byte size times */
      {
        ULONG d;
        d = *data++;
        if (map_flag == TRUE)
        {
          if (x11_bytes_pixel == 4)
            for(k=x;k<(x+size);k++)
                ((ULONG *)(image_out))[k+offset] = (ULONG)(map[d].map);
          else if (x11_bytes_pixel == 2)
             for(k=x;k<(x+size);k++)
                ((USHORT *)(image_out))[k+offset] = (USHORT)(map[d].map);
          else
             for(k=x;k<(x+size);k++)
                ((UBYTE *)(image_out))[k+offset] = (UBYTE)(map[d].map);
        }
        else
        {
          for(k=x;k<(x+size);k++)
                ((UBYTE *)(image_out))[k+offset] = (UBYTE)(d);
        }
        x+=size;   
      }
    } /* end of packets per line */
  } /* end of line */
}

/*
 * Routine to Decode an Fli LC chunk
 */
void Decode_Fli_LC(map,data,image_out,xpos,ypos,xsize,ysize,map_flag)
ColorReg *map;
unsigned char *data,*image_out;
ULONG *xpos,*ypos,*xsize,*ysize,map_flag;
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
	if (map_flag == TRUE)
        {
	  if (x11_bytes_pixel == 4)
            for(k=x;k<(x+size);k++) 
		((ULONG *)(image_out))[k+offset] = (ULONG)(map[d].map);
	  else if (x11_bytes_pixel == 2)
             for(k=x;k<(x+size);k++) 
		((USHORT *)(image_out))[k+offset] = (USHORT)(map[d].map);
	  else
             for(k=x;k<(x+size);k++) 
		((UBYTE *)(image_out))[k+offset] = (UBYTE)(map[d].map);
        }
        else
        {
          for(k=x;k<(x+size);k++) 
		((UBYTE *)(image_out))[k+offset] = (UBYTE)(d);
        }
        x+=size;   
      }
      else /* size is pos */
      {
        if (map_flag == TRUE)
        {
          if (x11_bytes_pixel == 4)
            for(k=x;k<(x+size);k++) 
		((ULONG *)(image_out))[k+offset] = (ULONG)(map[*data++].map);
          else if (x11_bytes_pixel == 2)
            for(k=x;k<(x+size);k++) 
		((USHORT *)(image_out))[k+offset] = (USHORT)(map[*data++].map);
          else
            for(k=x;k<(x+size);k++) 
		((UBYTE *)(image_out))[k+offset] = (UBYTE)(map[*data++].map);
        }
        else
        {
          for(k=x;k<(x+size);k++) 
		((UBYTE *)(image_out))[k+offset] = (UBYTE)(*data++);
        }

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

  DEBUG_LEVEL1 fprintf(stderr,"      LC: xypos=<%ld %ld> xysize=<%ld %ld>\n",
                        *xpos,*ypos,*xsize,*ysize);

}


/*
 * Routine to read an Fli COLOR chunk
 */
void Read_Fli_COLOR(fin,color_shift)
FILE *fin;
ULONG color_shift;
{
 ULONG k,l,c_index,packets,skip,colors,cnt;

 packets = UTIL_Get_LSB_Short(fin);
 c_index = 0;

 DEBUG_LEVEL1 fprintf(stderr,"   packets = %ld\n",packets);
 cnt=2;
 for(k=0;k<packets;k++)
 {
  skip = fgetc(fin);
  colors=fgetc(fin);
  DEBUG_LEVEL1 fprintf(stderr,"      skip %ld colors %ld\n",skip,colors);
  cnt+=2;
  if (colors==0) colors=FLI_MAX_COLORS;
  if ( (colors > x11_cmap_size) &&
       (x11_display_type == PSEUDO_COLOR) )
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
   DEBUG_LEVEL5 fprintf(stderr,"         %ld)  <%lx %lx %lx>\n", 
		  l,fli_cmap[l].red, fli_cmap[l].green,fli_cmap[l].blue);
   c_index++;
  } /* end of colors */
  cnt+= 3 * colors;
 } /* end of packets */
 if (cnt&0x01) fgetc(fin);  /* read pad byte if needed */

 ACT_Setup_CMAP(fli_cmap,FLI_MAX_COLORS,&k,CMAP_DIRECT);

}

/*
 * Routine to Decode an Fli LC chunk
 */
void Decode_Fli_LC7(map,data,image_out,xpos,ypos,xsize,ysize,map_flag)
ColorReg *map;
UBYTE *data,*image_out;
ULONG *xpos,*ypos,*xsize,*ysize,map_flag;
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
  packets = *data++; packets |= *data++ << 8;  /* # of packets */

  DEBUG_LEVEL5 fprintf(stderr,"pkts=%ld\n",packets);

  for(i=0; i < packets; i++)
  {
   
    blocks = *data++; blocks |= *data++ << 8; 

    DEBUG_LEVEL5 fprintf(stderr,"     %ld) ",i);

    if (blocks & 0x8000)  /* neg lines to jump */
    {
      blocks = 0x10000 - blocks;
      y += blocks;
      DEBUG_LEVEL5 fprintf(stderr,"     yskip %ld",blocks);
      blocks = *data++; blocks |= *data++ << 8;
    }
    DEBUG_LEVEL5 fprintf(stderr,"     blocks = %ld\n",blocks);

    if (optimize_flag == TRUE) if (y < miny) miny = y;

    ptr  = (UBYTE *)( (ULONG)(image_out) + y*imagex*x11_bytes_pixel);
    x = 0;

    for(j=0; j < blocks; j++)
    {
      xoff = (ULONG) *data++;  /* x offset */
      cnt  = (ULONG) *data++;  /* halfword count */
      ptr += (xoff * x11_bytes_pixel);
      x += xoff;

      DEBUG_LEVEL5 
        fprintf(stderr,"          %ld) xoff %lx  cnt = %lx",j,xoff,cnt);

      if (cnt & 0x80)
      {
        cnt = 256 - cnt;
        DEBUG_LEVEL5 fprintf(stderr,"               NEG %ld\n",cnt);
        if (optimize_flag == TRUE)
        {  
          if (x < minx) minx = x;
          x += (cnt << 1);
          if (x > maxx) maxx = x;
        }

	if (map_flag == TRUE)
        {
          tmp_data0 = map[*data++].map;
          tmp_data1 = map[*data++].map;
          if (x11_bytes_pixel == 4)
	  { 
	    ULONG *ulp = (ULONG *)ptr;
            while(cnt--) { *ulp++ = (ULONG)tmp_data0;
			   *ulp++ = (ULONG)tmp_data1; }
	    ptr = (UBYTE *)ulp;
          }
          else if (x11_bytes_pixel == 2)
	  { 
	    USHORT *usp = (USHORT *)ptr;
	    while(cnt--) { *usp++ = (USHORT)tmp_data0;
			   *usp++ = (USHORT)tmp_data1; }
	    ptr = (UBYTE *)usp;
          }
          else
            while(cnt--) { *ptr++ = (UBYTE)tmp_data0;
                           *ptr++ = (UBYTE)tmp_data1; }
	}
	else
	{
          tmp_data0 = *data++;
          tmp_data1 = *data++;
          while(cnt--) { *ptr++ = (UBYTE)tmp_data0;
                         *ptr++ = (UBYTE)tmp_data1; }
	}

      }
      else
      {   /* cnt is number of unique pairs of bytes */
        DEBUG_LEVEL5 fprintf(stderr,"               POS %ld\n",cnt);
        if (optimize_flag == TRUE)
        {
          if (cnt == 1)  /* potential NOPs just to move x */
          {
            if ( (*ptr != *data) || (ptr[1] != data[1]) )  
            {
              if (x < minx) minx = x; 
              x += (cnt << 1);
              if (x > maxx) maxx = x;
            }
          }
	  else
          {
            if (x < minx) minx = x; 
            x += (cnt << 1);
            if (x > maxx) maxx = x;
          }
        }
        if (map_flag == TRUE)
        {
          if (x11_bytes_pixel == 4)
	  { 
	    ULONG *ulp = (ULONG *)ptr;
	    while(cnt--) { *ulp++ = (ULONG)map[*data++].map;
                           *ulp++ = (ULONG)map[*data++].map; }
	    ptr = (UBYTE *)ulp;
          }
          else if (x11_bytes_pixel == 2)
	  { 
	    USHORT *usp = (USHORT *)ptr;
	    while(cnt--) { *usp++ = (USHORT)map[*data++].map;
			   *usp++ = (USHORT)map[*data++].map; }
	    ptr = (UBYTE *)usp;
          }
          else
             while(cnt--) { *ptr++ = (UBYTE)map[*data++].map;
			    *ptr++ = (UBYTE)map[*data++].map; }
        }
        else
             while(cnt--) { *ptr++ = (UBYTE) *data++;
			    *ptr++ = (UBYTE) *data++; }

      } 
    } /* (j) end of blocks */
    y++;
  } /* (i) end of packets */

DEBUG_LEVEL3
 fprintf(stderr,"      LC: minx miny (%ld %ld), maxx maxy (%ld %ld)\n",
     minx,miny,maxx,y);

  if (optimize_flag == TRUE)
  {
    if (minx >= imagex) minx = 0; 
    if (miny >= imagey) miny = 0; 

    *xpos = minx;
    *ypos = miny;

    if (maxx > minx) *xsize = maxx - minx + 2;
    else             *xsize = imagex - minx;

    if (y > miny) *ysize = y - miny + 1;
    else          *ysize = imagey - miny;
  }
  else
  {
    *xpos = 0;
    *ypos = 0;
    *xsize = imagex;
    *ysize = imagey;
  }

  DEBUG_LEVEL1 fprintf(stderr,"      LC: xypos=<%ld %ld> xysize=<%ld %ld>\n",
		 *xpos,*ypos,*xsize,*ysize);

}


