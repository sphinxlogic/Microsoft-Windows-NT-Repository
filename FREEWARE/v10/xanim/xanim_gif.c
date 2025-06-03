
/*
 * xanim_gif.c
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
#include <sys/types.h>
#include "xanim.h"
#include "xanim_gif.h"

void GIF_Read_File();
void GIF_Decompress();
void GIF_Get_Next_Entry();
void GIF_Add_To_Table();
void GIF_Send_Data();
void GIF_Init_Table();
void GIF_Clear_Table();
ULONG GIF_Get_Code();
void GIF_Screen_Header();
void GIF_Image_Header();
LONG GIF_Get_Short();
LONG Is_GIF_File();

#define MAXVAL  4100            /* maxval of lzw coding size */
#define MAXVALP 4200

ULONG GIF_Get_Code();

#define GIF_CMAP_SIZE 256
static GIF_Screen_Hdr gifscrn;
static GIF_Image_Hdr gifimage;
static ColorReg gifcolor[GIF_CMAP_SIZE];
static GIF_Table table[MAXVALP];

static ULONG root_code_size,code_size,CLEAR,EOI,INCSIZE;
static ULONG nextab;
static ULONG gif_mask[16] = {1,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,0,0};
static ULONG gif_ptwo[16] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,0,0};
static UBYTE gif_buff[MAXVALP];
static ULONG gif_block_size;
static LONG bits,num_bits;
static lace_flag;

static LONG gif_max_imagec,gif_max_imagex,gif_max_imagey;

static LONG pic_i,pic_size;

/* GIF89a variables */
static LONG gif_anim_type;  /* from f9 extension */
static LONG gif_anim_time;  /* from f9 extension */
static LONG gif_anim_mask;  /* from f9 extension */


/*
 *
 */
void GIF_Read_File(fname)
char *fname;
{
 FILE *fp;
 LONG i,exit_flag;
 ACTION *act;

 if ( (fp=fopen(fname,"r"))==0)
 { 
  fprintf(stderr,"Can't open %s for reading.\n",fname); 
  TheEnd();
 }

 gif_anim_type = 0;
 gif_anim_time = (jiffy_flag) ? jiffy_flag : MS_PER_60HZ;
 gif_anim_mask = 0;

 GIF_Screen_Header(fp);

 /*** read until  ,  separator */
 do
 {
  i=fgetc(fp);
  if ( (i<0) && feof(fp))
  {
   fclose(fp);
   TheEnd1("GIF_Read_Header: Unexpected End of File\n");
  }
  if (i == '!') GIF_Read_Extension(fp);  /* extension */
 } while(i != ',');

 exit_flag = 0;
 while(!exit_flag)
 {
  UBYTE *pic_ptr;
  
  /* Read Image Header
   */
  GIF_Image_Header(fp);

  /*** Setup ACTION for IMAGE or CLIP */
  ACT_Get_Next_Action(&act);

  pic_size = imagex * imagey;
  pic_ptr = (UBYTE *) malloc(pic_size * x11_bytes_pixel);
  if (pic_ptr==0) TheEnd1("GIF_Read_File: could malloc of image\n");

  pic_i=0;
  GIF_Decompress(fp,pic_ptr);

  if (gif_anim_type == 0x01)
  {
   UBYTE *clip_ptr;
   ULONG t_x;

   /* 8 pixels per byte for clip mask */
   t_x = imagex/8;
   /* make room for pad on non byte sizes */
   if (imagex%8) t_x++;

   clip_ptr = (UBYTE *) malloc( imagey * t_x );
   if (clip_ptr==0) TheEnd1("GIF_Read_File: could malloc of image\n");

   UTIL_Create_Clip(clip_ptr,pic_ptr,gifcolor[gif_anim_mask].map,imagex,imagey);

   ACT_Setup_CLIP(act,
                  gif_anim_time,
                  gifimage.left,gifimage.top,imagex,imagey,pic_ptr,clip_ptr);
  }
  else
  {
   ACT_Setup_IMAGE1(act,
                    gif_anim_time,
                    gifimage.left,gifimage.top,imagex,imagey,pic_ptr);
  }

 /*** read until "," ";" or feof */
  do
  {
   i=fgetc(fp);
   if ( (i<0) || (i == ';')) exit_flag = 1;
   else if (i == '!') GIF_Read_Extension(fp);  /* extension */
  } while( (i != ',') && (!exit_flag) );

 } /*** end of while images */

 imagex = gif_max_imagex;
 imagey = gif_max_imagey;
 imagec = gif_max_imagec;
 fclose(fp);
}

void GIF_Decompress(fp,pic,gif_pix_offset)
FILE *fp;
char *pic;
LONG gif_pix_offset;
{
 register ULONG code,old;

 bits = 0;
 num_bits=0;
 gif_block_size=0;
     /* starting code size of LZW */
 root_code_size=(fgetc(fp) & 0xff); 
if (debug >= DEBUG_LEVEL2) 
		fprintf(stderr,"  root code size = %ld\n",root_code_size);
 GIF_Clear_Table();                /* clear decoding symbol table */

 code=GIF_Get_Code(fp);

 if (code==CLEAR) 
 {
  GIF_Clear_Table(); 
  code=GIF_Get_Code(fp);
 }
 /* write code(or what it currently stands for) to file */
 GIF_Send_Data(code,pic,gif_pix_offset);   
 old=code;
 code=GIF_Get_Code(fp);
 do
 {
  if (table[code].valid==1)    /* if known code */
  {
       /* send it's associated string to file */
    GIF_Send_Data(code,pic,gif_pix_offset);
    GIF_Get_Next_Entry(fp);       /* get next table entry (nextab) */
    GIF_Add_To_Table(old,code,nextab);  /* add old+code to table */
    old=code;
  }
  else      /* code doesn't exist */
  {
    GIF_Add_To_Table(old,old,code);   /* add old+old to table */
    GIF_Send_Data(code,pic,gif_pix_offset);
    old=code;
  }
  code=GIF_Get_Code(fp);
  if (code==CLEAR)
  { 
   GIF_Clear_Table();
   code=GIF_Get_Code(fp);
   GIF_Send_Data(code,pic,gif_pix_offset);
   old=code;
   code=GIF_Get_Code(fp);
  }
 } while(code!=EOI);
}

void GIF_Get_Next_Entry(fp)
FILE *fp;
{
   /* table walk to empty spot */
 while(  (table[nextab].valid==1)
       &&(nextab<MAXVAL)
      ) nextab++;
 /* 
  * Ran out of space??!?  Something's roached 
  */
 if (nextab>=MAXVAL)    
 { 
  fprintf(stderr,"Error: GetNext nextab=%ld\n",nextab);
  fclose(fp);
  TheEnd();
 }
 if (nextab==INCSIZE)   /* go to next table size (and LZW code size ) */
 {
   /* fprintf(stderr,"GetNext INCSIZE was %ld ",nextab); */
   code_size++; INCSIZE=(INCSIZE*2)+1;
   if (code_size>=12) code_size=12;
/*   fprintf(stderr,"<%ld>",INCSIZE); */
 }

}
/*  body is associated string
    next is code to add to that string to form associated string for
    index
*/     

void GIF_Add_To_Table(body,next,index)
register ULONG body,next,index;
{
 if (index>MAXVAL)
 { 
  fprintf(stderr,"Error index=%ld\n",index);
 }
 else
 {
  table[index].valid=1;
  table[index].data=table[next].first;
  table[index].first=table[body].first;
  table[index].last=body;
 }
}

void GIF_Send_Data(index,pic,gif_pix_offset)
register LONG index;
char *pic;
LONG gif_pix_offset;
{
 register LONG i,j;
 i=0;
 do         /* table walk to retrieve string associated with index */
 { 
  gif_buff[i]=table[index].data; 
  i++;
  index=table[index].last;
  if (i>MAXVAL)
  { 
   fprintf(stderr,"Error: Sending i=%ld index=%ld\n",i,index);
   TheEnd();
  }
 } while(index>=0);

 /* now invert that string since we retreived it backwards */
 i--;
 for(j=i;j>=0;j--)
 {
  if (x11_display_type == PSEUDO_COLOR_8BIT) 
		pic[pic_i] = gifcolor[ gif_buff[j] ].map;
  else
  {
   if (x11_display_type == TRUE_COLOR_24BIT)
   {
    ((ULONG *)(pic))[pic_i] = gifcolor[ (gif_buff[j]) ].map;
   }
  }
  pic_i++;

  if (lace_flag)
  {
   if ((pic_i % imagex) == 0 )
   { 
    switch(lace_flag)
    {
     case 1: pic_i += imagex * 7; break; /* fill every 8th row */
     case 2: pic_i += imagex * 7; break; /* fill every 8th row */
     case 3: pic_i += imagex * 3; break; /* fill every 4th row */
     case 4: pic_i += imagex    ; break; /* fill every other row */
    }
   }
   if (pic_i > pic_size)
   {
    lace_flag++;
    switch(lace_flag)
    {
     case 2: pic_i = imagex << 2; break;  /* start at 4th row */
     case 3: pic_i = imagex << 1; break;  /* start at 2nd row */
     case 4: pic_i = imagex;      break;  /* start at 1st row */
     default: fprintf(stderr,"unlacing error\n");
              lace_flag = 0;
              pic_i = 0;
              break;
    }
   }
  } /*** end of if lace_flag */
 } /*** end of code expansion */
}


/* 
 * initialize string table 
 */
void GIF_Init_Table()       
{
 register LONG maxi,i;

if (debug >= DEBUG_LEVEL2) fprintf(stderr,"  Initing Table...");
 maxi=gif_ptwo[root_code_size];
 for(i=0; i<maxi; i++)
 {
  table[i].data=i;   
  table[i].first=i;
  table[i].valid=1;  
  table[i].last = -1;
 }
 CLEAR=maxi; 
 EOI=maxi+1; 
 nextab=maxi+2;
 INCSIZE = (2*maxi)-1;
 code_size=root_code_size+1;
if (debug >= DEBUG_LEVEL2) fprintf(stderr,"done\n");
}


/* 
 * clear table 
 */
void GIF_Clear_Table()   
{
 register LONG i;
if (debug >= DEBUG_LEVEL2) fprintf(stderr,"  Clearing Table...\n");
 for(i=0;i<MAXVAL;i++) table[i].valid=0;
 GIF_Init_Table();
}


/*CODE*/
ULONG GIF_Get_Code(fp) /* get code depending of current LZW code size */
FILE *fp;
{
 ULONG code;
 register LONG tmp;

 while(num_bits < code_size)
 {
  /**** if at end of a block, start new block */
  if (gif_block_size==0)
  {
   tmp = fgetc(fp);
   if (tmp >= 0 ) gif_block_size=(ULONG)(tmp);
   else TheEnd1("EOF in data stream\n");
   if (debug >= DEBUG_LEVEL2) 
		fprintf(stderr,"New Block size=%ld\n",gif_block_size);
  }

  tmp = fgetc(fp);   gif_block_size--;
  if (tmp >= 0)
  {
/*POD added ULONG on 0xff */
   bits |= ( ((ULONG)(tmp) & (ULONG)(0xff)) << num_bits );
   if (debug >= DEBUG_LEVEL2) 
	fprintf(stderr,"tmp=%lx bits=%lx num_bits=%ld\n",tmp,bits,num_bits);

   num_bits+=8;
  }
  else TheEnd1("EOF in data stream\n");
 }

 code = bits & gif_mask[code_size];
 bits >>= code_size;  
 num_bits -= code_size;
 if (debug >= DEBUG_LEVEL2) fprintf(stderr,"code=%lx \n",code);


 if (code>MAXVAL)
 {
  fprintf(stderr,"\nError! in stream=%lx \n",code);
  fprintf(stderr,"CLEAR=%lx INCSIZE=%lx EOI=%lx code_size=%lx \n",
                                           CLEAR,INCSIZE,EOI,code_size);
  code=EOI;
 }

 if (code==INCSIZE)
 {
  if (debug >= DEBUG_LEVEL2) fprintf(stderr,"  code=INCSIZE(%ld)\n",INCSIZE);
  if (code_size<12)
  {
   code_size++; INCSIZE=(INCSIZE*2)+1;
  }
  else if (debug >= DEBUG_LEVEL2) fprintf(stderr,"  <13?>\n");
  if (debug >= DEBUG_LEVEL2) fprintf(stderr,"  new size = %ld\n",code_size);
 }

 return(code);
}


/* 
 * read GIF header 
 */
void GIF_Screen_Header(fp)
FILE *fp;
{
 LONG temp,i;

 for(i=0;i<6;i++) fgetc(fp);	/* read and toss GIF87a or GIF89? */

 gifscrn.width  = GIF_Get_Short(fp);
 gifscrn.height = GIF_Get_Short(fp);
 imagex = gifscrn.width;
 imagey = gifscrn.height;
 gif_max_imagex = gifscrn.width;
 gif_max_imagey = gifscrn.height;

 temp=fgetc(fp);
 gifscrn.m       =  temp & 0x80;
 gifscrn.cres    = (temp & 0x70) >> 4;
 gifscrn.pixbits =  temp & 0x07;
 gifscrn.bc  = fgetc(fp);
 temp=fgetc(fp);
 imagec=gif_ptwo[(1+gifscrn.pixbits)];
 gif_max_imagec = imagec;

 if (verbose == TRUE)
  fprintf(stderr,"Screen: %ldx%ldx%ld m=%ld cres=%ld bkgnd=%ld pix=%ld\n",
    gifscrn.width,gifscrn.height,imagec,gifscrn.m,gifscrn.cres,
    gifscrn.bc,gifscrn.pixbits);

 if ( (imagec > x11_cmap_size) && (x11_cmap_flag == TRUE) )
 {
  fprintf(stderr,"ERROR: Image has %ld colors, display can handle %ld\n",
					imagec,x11_cmap_size);
  TheEnd();
 }

  if (gifscrn.m)
  {
    if (x11_display_type == PSEUDO_COLOR_8BIT)
    {
     ACTION *act;
     ULONG off;

     for(i=0; i < imagec; i++)
     {
       gifcolor[i].red   = fgetc(fp);
       gifcolor[i].green = fgetc(fp);
       gifcolor[i].blue  = fgetc(fp);
     } /* end of for i */

     /* Setup ACTION for CMAP */
     ACT_Get_Next_Action(&act);
     ACT_Setup_CMAP(gifcolor,imagec,&off,1);
   } /* end of Pseudo Color */
   else 
     if (x11_display_type == TRUE_COLOR_24BIT)
     {
       for(i=0; i < imagec; i++)
       {
         gifcolor[i].map  = (fgetc(fp) & 0xff);
         gifcolor[i].map |= (fgetc(fp) & 0xff) <<  8;
         gifcolor[i].map |= (fgetc(fp) & 0xff) << 16;
       } /* end of for i */
     } /* end of True Color */
  } /* end of image color map */

} /* end of function */

void GIF_Image_Header(fp)
FILE *fp;
{
 LONG temp,tnum,i;

 gifimage.left   = GIF_Get_Short(fp);
 gifimage.top    = GIF_Get_Short(fp);
 gifimage.width  = GIF_Get_Short(fp);
 gifimage.height = GIF_Get_Short(fp);
 temp=fgetc(fp);
 gifimage.m        = temp & 0x80;
 gifimage.i        = temp & 0x40;
 if (gifimage.i) lace_flag = TRUE;
 else lace_flag = FALSE;
 gifimage.pixbits  = temp & 0x07;
 imagex=gifimage.width;
 imagey=gifimage.height;
 if (imagex > gif_max_imagex) gif_max_imagex = imagex;
 if (imagey > gif_max_imagey) gif_max_imagey = imagey;
 tnum=gif_ptwo[(1+gifimage.pixbits)];
 if (tnum > gif_max_imagec) gif_max_imagec = tnum;
 imagec = tnum;
 if (verbose == TRUE)
 {
  fprintf(stderr,"Image: %ldx%ldx%ld m=%ld i=%ld pix=%ld ",
    imagex,imagey,tnum,gifimage.m,gifimage.i,gifimage.pixbits,
    gifimage.reserved );
  fprintf(stderr,"Pos: %ldx%ld \n",gifimage.left,gifimage.top);
 }

 
  if (gifimage.m)
  {
    if (x11_display_type == PSEUDO_COLOR_8BIT)
    {
      ACTION *act;
      LONG off;

      /* Read in Image CMAP if Image has one
       */
      for(i=0; i < imagec; i++)
      {
        gifcolor[i].red   = fgetc(fp);
        gifcolor[i].green = fgetc(fp);
        gifcolor[i].blue  = fgetc(fp);
      }
 
      /* Setup ACTION for CMAP 
       */
      ACT_Get_Next_Action(&act);
      ACT_Setup_CMAP(gifcolor,imagec,&off,1);
    }
    else
    if (x11_display_type == TRUE_COLOR_24BIT)
    {
      for(i= 0; i < imagec; i++)
      {
        gifcolor[i].map  = (fgetc(fp) & 0xff);
        gifcolor[i].map |= (fgetc(fp) & 0xff) <<  8;
        gifcolor[i].map |= (fgetc(fp) & 0xff) << 16;
      } /* end of i */
    } /* end of True Color */

  } /* end of Image has cmap */
}


/*
 *
 */
LONG GIF_Get_Short(fp)
FILE *fp;
{
 register LONG temp;
 temp=fgetc(fp);
 return(temp|( (fgetc(fp)) << 8 ));
}

/*
 *
 */
LONG Is_GIF_File(filename)
char *filename;
{
 FILE *fp;
 ULONG firstword;

 if ( (fp=fopen(filename,"r")) == 0)
 { 
  fprintf(stderr,"can't open %s\n",filename); 
  TheEnd();
 }
 /* by reading bytes we can ignore big/little endian problems */
 firstword  = (fgetc(fp) & 0xff) << 24;
 firstword |= (fgetc(fp) & 0xff) << 16;
 firstword |= (fgetc(fp) & 0xff) <<  8;
 firstword |= (fgetc(fp) & 0xff);

 fclose(fp);

 if (firstword == 0x47494638) return(TRUE);
 return(FALSE);
}

GIF_Read_Extension(fp)
FILE *fp;
{
 int block_size,code,tmp,i;

 code = fgetc(fp);
 if (verbose == TRUE) fprintf(stderr,"GIF Extension: Code = %lx\n",code);
 block_size = fgetc(fp);

 if ( (code == 0xf9) && (block_size == 4))
 {
  gif_anim_type = fgetc(fp);
  i = GIF_Get_Short(fp);
  if (jiffy_flag == 0) gif_anim_time = i;
  gif_anim_mask = fgetc(fp);
  block_size = fgetc(fp);
 }

 while(block_size > 0)
 {
  for(i=0; i<block_size; i++) 
  {
   tmp=fgetc(fp);
   if ( (code == 0xfe) && (verbose==TRUE) ) fprintf(stderr,"%c",tmp);
   if ( (code == 0xf9) && (verbose==TRUE) ) fprintf(stderr,"%lx ",tmp);
  }
  block_size = fgetc(fp);
  if ( (code == 0xf9) && (verbose==TRUE) ) fprintf(stderr,"\n",tmp);
 }
}

