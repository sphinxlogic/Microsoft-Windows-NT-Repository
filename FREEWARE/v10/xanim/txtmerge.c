
/*
 * txtmerge.c
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
 /*
  * Description:
  *
  * This program reads a txt91(see XAnim docs) file and merges the listed
  * gif files into one gif file. 
  *
  * Eventually, I'd like to have this program compare the current image
  * with the previous image and check to see if only a small section of
  * the screen changed from the previous image. Worth a shot.
  */

 /*
  * Rev 1.00	23Jul91	Mark Podlipec
  *	creation
  * Rev 1.01	08Jan92	Mark Podlipec
  *     use all colormaps, not just 1st.
  * 
  */
#define DA_REV 1.01

#include <stdio.h>
#include <sys/types.h>
#include "xanim.h"
#include "xanim_gif.h"

#define MAXVAL  4100            /* maxval of lzw coding size */
#define MAXVALP 4200

int debug_flag = 0;  /* make these options */
int verbose = 0;  /* make these options */
int imagex = 0;
int imagey = 0;
int imagec = 0;

GIF_Color gif_cmap[256];

ULONG GIF_Get_Code();
void GIF_Decompress();
void GIF_Get_Next_Entry();
void GIF_Add_To_Table();
void GIF_Send_Data();
void GIF_Clear_Table();
void GIF_Screen_Header();
void GIF_Image_Header();

GIF_Screen_Hdr gifscrn;
GIF_Image_Hdr gifimage;
GIF_Table table[MAXVALP];

ULONG root_code_size,code_size,CLEAR,EOI,INCSIZE;
ULONG nextab;
ULONG gif_mask[16] = {1,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,0,0};
ULONG gif_ptwo[16] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,0,0};
UBYTE gif_buff[MAXVALP];
ULONG gif_block_size;
int num_bits,bits;

int pic_i;
char gif_file_name[256];
int screen_was_last;

void TheEnd()
{
 exit(0);
}

void TheEnd1(p)
char *p;
{
 fprintf(stderr,"%s",p);
 TheEnd();
}

Usage()
{
  fprintf(stderr,"\nUsage: txtmerge anim.txt91 > anim.gifanim\n\n");
  exit(0);
}

main(argc,argv)
int argc;
char *argv[];
{
 FILE *fp;
 int ret,i;
 int num_of_files,num_of_frames;

 fprintf(stderr,"txtmerge Rev %2.2f (C) 1991,1992 by Mark Podlipec\n",DA_REV);
 
 if (argc < 2) Usage();
 
 screen_was_last = FALSE;
 i = 1;
 while( i < argc)
 {
  char *p;
  p = argv[i];
  if ( (p[0] == '-') || (p[0] == '+') )
  { 
   switch(p[1])
   {
    case 'v':
	        verbose = 1;
	        i++;
		break;
    default: 
		Usage();
		exit(0);
		break;
   }
  }
  else break;
 }

 if ( (fp=fopen(argv[i],"r"))==0)
 { 
  fprintf(stderr,"Can't open %s for reading.\n",argv[i]); 
  TheEnd();
 }

 /* throw away txt91 header 
  */
 fscanf(fp,"%*s",gif_file_name); 

 /* read number of files
  */
 fscanf(fp,"%ld",&num_of_files);
 if (num_of_files<=0)
 {
  fprintf(stderr,"num_of_file is wierd=%ld\n",num_of_files);
  fclose(fp);
  TheEnd();
 }

 /* Read files, save colormap of 1st one only
  */
 for(i=0;i<num_of_files;i++)
 {
  fscanf(fp,"%s",gif_file_name);
  fprintf(stderr,"Reading %s\n",gif_file_name);
  if (i==0) GIF_Read_File(gif_file_name,1);
  else      GIF_Read_File(gif_file_name,0);
 }

 /* Read number of frames if present
  */
 ret=fscanf(fp,"%ld",&num_of_frames);
 if ( (ret==1) && (num_of_frames>=0))
 {
  int action_number;
  int frame;

  /* For now throw away frame information.
   * Maybe later incorporation as GIF Chunk(?)
   */
  for(i=0;i<num_of_frames;i++)
  {
   ret = fscanf(fp,"%ld",&frame);
  }
 }
 fputc(';',stdout); /* image separator */
 fclose(fp);
}


/*
 * Read a GIF file, outputting to fname as we go.
 * It would be faster to read and write the individual blocks,
 * but eventually we'd like to optimize based on changes from
 * previous images(ie only a small section of the image changed.
 */
void GIF_Read_File(fname,first_image)
char *fname;
int first_image;
{
 FILE *fp;
 int ret,i,exit_flag;

 if ( (fp=fopen(fname,"r"))==0)
 { 
  fprintf(stderr,"Can't open %s for reading.\n",fname); 
  TheEnd();
 }

 GIF_Screen_Header(fp,stdout,first_image);

 /*** read until  ,  separator */
 do
 {
  i=fgetc(fp);
  if ( (i<0) && feof(fp))
  {
   fclose(fp);
   TheEnd1("GIF_Read_Header: Unexpected End of File\n");
  }
 } while(i != ',');

 fputc(',',stdout); /* image separator */

 GIF_Image_Header(fp,stdout,first_image);

 /*** Setup ACTION for IMAGE */

 GIF_Decompress(fp,stdout,0);
 fputc(0,stdout);  /* block count of zero */

 fclose(fp);
}

void GIF_Decompress(fp,fout)
FILE *fp,*fout;
{
 register ULONG code,old;

 pic_i = 0;
 bits=0;
 num_bits=0;
 gif_block_size=0;
    /* starting code size of LZW */
 root_code_size=(fgetc(fp) & 0xff); fputc(root_code_size,fout);
 GIF_Clear_Table();                /* clear decoding symbol table */

 code=GIF_Get_Code(fp,fout);

 if (code==CLEAR) 
 {
  GIF_Clear_Table(); 
  code=GIF_Get_Code(fp,fout);
 }
 /* write code(or what it currently stands for) to file */
 GIF_Send_Data(code);   
 old=code;
 code=GIF_Get_Code(fp,fout);
 do
 {
  if (table[code].valid==1)    /* if known code */
  {
       /* send it's associated string to file */
    GIF_Send_Data(code);
    GIF_Get_Next_Entry(fp);       /* get next table entry (nextab) */
    GIF_Add_To_Table(old,code,nextab);  /* add old+code to table */
    old=code;
  }
  else      /* code doesn't exist */
  {
    GIF_Add_To_Table(old,old,code);   /* add old+old to table */
    GIF_Send_Data(code);
    old=code;
  }
  code=GIF_Get_Code(fp,fout);
  if (code==CLEAR)
  { 
   GIF_Clear_Table();
   code=GIF_Get_Code(fp,fout);
   GIF_Send_Data(code);
   old=code;
   code=GIF_Get_Code(fp,fout);
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

void GIF_Send_Data(index)
register int index;
{
 register int i,j;
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
  /*pic[pic_i] = gif_buff[j] | gif_pix_offset;*/
  pic_i++;
 }
}


/* 
 * initialize string table 
 */
void GIF_Init_Table()       
{
 register int maxi,i;

if (debug_flag) fprintf(stderr,"Initing Table...");
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
}


/* 
 * clear table 
 */
void GIF_Clear_Table()   
{
 register int i;
if (debug_flag) fprintf(stderr,"Clearing Table...\n");
 for(i=0;i<MAXVAL;i++) table[i].valid=0;
 GIF_Init_Table();
}

/*CODE*/
ULONG GIF_Get_Code(fp,fout) /* get code depending of current LZW code size */
FILE *fp,*fout;
{
 ULONG code;
 register int tmp;

 while(num_bits < code_size)
 {
  /**** if at end of a block, start new block */
  if (gif_block_size==0) 
  {
   tmp = fgetc(fp);
   if (tmp >= 0 )
   {
    fputc(tmp,fout);
    gif_block_size=(ULONG)(tmp);
   }
   else TheEnd1("EOF in data stream\n");
  }

  tmp = fgetc(fp);   gif_block_size--;
  if (tmp >= 0)
  {
   fputc(tmp,fout);
   bits |= ( ((ULONG)(tmp) & 0xff) << num_bits );
   num_bits+=8;
  }
  else TheEnd1("EOF in data stream\n");
 }
  
 code = bits & gif_mask[code_size];
 bits >>= code_size;
 num_bits -= code_size; 


 if (code>MAXVAL)
 { 
  fprintf(stderr,"\nError! in stream=%lx \n",code); 
  fprintf(stderr,"CLEAR=%lx INCSIZE=%lx EOI=%lx code_size=%lx \n",
                                           CLEAR,INCSIZE,EOI,code_size); 
  code=EOI;
 }

 if (code==INCSIZE)
 {
  if (code_size<12)
  {
   code_size++; INCSIZE=(INCSIZE*2)+1;
  }
  else if (debug_flag) fprintf(stderr,"<13?>"); 
 }

 return(code);
}


/* 
 * read GIF header 
 */
void GIF_Screen_Header(fp,fout,first_time)
FILE *fp,*fout;
int first_time;
{
 int temp,i;

 for(i=0;i<6;i++) 
 {
  temp = fgetc(fp);
  if (first_time==TRUE) fputc(temp,fout);
 }

 gifscrn.width  = GIF_Get_Short(fp,fout,first_time);
 gifscrn.height = GIF_Get_Short(fp,fout,first_time);
 temp=fgetc(fp);		 if (first_time==TRUE) fputc(temp,fout);
 gifscrn.m       =  temp & 0x80;
 gifscrn.cres    = (temp & 0x70) >> 4;
 gifscrn.pixbits =  temp & 0x07;
 gifscrn.bc  = fgetc(fp);	 if (first_time==TRUE) fputc(gifscrn.bc,fout);
 temp=fgetc(fp);		 if (first_time==TRUE) fputc(temp,fout);
 imagec=gif_ptwo[(1+gifscrn.pixbits)];

 if (verbose)
  fprintf(stderr,"Screen: %ldx%ldx%ld m=%ld cres=%ld bkgnd=%ld pix=%ld\n",
    gifscrn.width,gifscrn.height,imagec,gifscrn.m,gifscrn.cres,
    gifscrn.bc,gifscrn.pixbits);

 if (gifscrn.m)
 {
  for(i=0;i<imagec;i++)
  {
   gif_cmap[i].cmap.red   = temp = fgetc(fp); 
           if (first_time==TRUE) fputc(temp,fout);
   gif_cmap[i].cmap.green = temp = fgetc(fp); 
           if (first_time==TRUE) fputc(temp,fout);
   gif_cmap[i].cmap.blue  = temp = fgetc(fp); 
           if (first_time==TRUE) fputc(temp,fout);
  }
 }
 screen_was_last = TRUE;
}

void GIF_Image_Header(fp,fout,first_time)
FILE *fp,*fout;
int first_time;
{
 int temp,tnum,i;

 gifimage.left   = GIF_Get_Short(fp,fout,1);
 gifimage.top    = GIF_Get_Short(fp,fout,1);
 gifimage.width  = GIF_Get_Short(fp,fout,1);
 gifimage.height = GIF_Get_Short(fp,fout,1);
 temp=fgetc(fp); 

 gifimage.m        = temp & 0x80;
 gifimage.i        = temp & 0x40;
 gifimage.pixbits  = temp & 0x07;

 if (screen_was_last && (first_time==FALSE)) temp |= 0x80;
 temp &= 0xf8;
 temp |= gifscrn.pixbits;
 fputc(temp,fout);

 imagex=gifimage.width;
 imagey=gifimage.height;
 tnum=gif_ptwo[(1+gifimage.pixbits)];
 if (verbose)
  fprintf(stderr,"Image: %ldx%ldx%ld m=%ld i=%ld pix=%ld \n",
    imagex,imagey,tnum,gifimage.m,gifimage.i,gifimage.pixbits);

 /* if there is an image cmap, then use it */
 if (gifimage.m)
 {
  for(i=0;i<tnum;i++)
  {
   gif_cmap[i].cmap.red   = temp = fgetc(fp); fputc(temp,fout);
   gif_cmap[i].cmap.green = temp = fgetc(fp); fputc(temp,fout);
   gif_cmap[i].cmap.blue  = temp = fgetc(fp); fputc(temp,fout);
  }
 }  /* else if screen was last not 1st time */
 else if (screen_was_last && (first_time==FALSE))
 {
  for(i=0;i<imagec;i++)
  {
   fputc(gif_cmap[i].cmap.red  ,fout);
   fputc(gif_cmap[i].cmap.green,fout);
   fputc(gif_cmap[i].cmap.blue ,fout);
  }
 }
 screen_was_last = FALSE; 
}


/*
 *
 */
int GIF_Get_Short(fp,fout,first_time)
FILE *fp,*fout;
int first_time;
{
 register int temp,tmp1;
 temp=fgetc(fp);	 if (first_time==TRUE) fputc(temp,fout);
 tmp1=fgetc(fp);	 if (first_time==TRUE) fputc(tmp1,fout);
 return(temp|( (tmp1) << 8 ));
}


