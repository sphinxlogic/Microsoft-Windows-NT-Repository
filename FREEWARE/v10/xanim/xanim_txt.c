
/*
 * xanim_txt.c
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

int Is_TXT_File();
void TXT_Read_File();

/*
 * This file will open the filename passed to it and determine if
 * it is a txt91 file. If it is it returns TRUE, else FALSE. It
 * closes the file before returning.
 */
int Is_TXT_File(filename)
char *filename;
{
 FILE *fp;
 ULONG firstword;

 if ( (fp=fopen(filename,"r")) == 0)
 { fprintf(stderr,"can't open %s\n",filename); TheEnd();}
 /* by reading bytes we can ignore big/little endian problems */
 firstword  = (fgetc(fp) & 0xff) << 24;
 firstword |= (fgetc(fp) & 0xff) << 16;
 firstword |= (fgetc(fp) & 0xff) <<  8;
 firstword |= (fgetc(fp) & 0xff);
 fclose(fp);

/*                   t x t 9     so we ignore the 1 */
 if (firstword == 0x74787439) return(TRUE);
 return(FALSE);
}

static char gif_file_name[256];

/*
 * This file parse the txt animation file and converts it into actions. 
 *
 */
void TXT_Read_File(fname)
char *fname;
{
 FILE *fp;
 int ret,i;
 int num_of_files,txtframe_num;
 int *txt_act_lst;

 if ( (fp=fopen(fname,"r"))==0)
 { 
  fprintf(stderr,"Can't open %s for reading.\n",fname); 
  TheEnd();
 }

 /* read and throw away txt91 header 
  */
 fscanf(fp,"%*s",gif_file_name);

 /* Read the number of files
  */
 fscanf(fp,"%ld",&num_of_files);
 if (num_of_files<=0)
 {
  fprintf(stderr,"num_of_file is wierd=%ld\n",num_of_files);
  fclose(fp);
  TheEnd();
 }
 
 txt_act_lst = (int *) malloc( sizeof(int) * (num_of_files + 1));
 /* Read in the GIF files, use only the 1st one's colormap
  */
 for(i=0;i<num_of_files;i++)
 {
  fscanf(fp,"%s",gif_file_name);
  fprintf(stderr,"Reading %s\n",gif_file_name);
  txt_act_lst[i] = action_cnt;
  GIF_Read_File(gif_file_name);
 }
 txt_act_lst[num_of_files] = action_cnt;

 /* Check for Frame list at end of images.
  */
 ret=fscanf(fp,"%ld",&txtframe_num);
 if ( (ret==1) && (txtframe_num>=0))
 {
  int *txt_frames,tmp_txtframe,num_valid_txtframes;
  int numof_frames,j,k;

  /* read in txt frame list, keep track of actual frames since each txt_frame
   * can have several frames(cmaps and images);
   */
  txt_frames = (int *) malloc(txtframe_num * sizeof(int) );
  numof_frames = 0;
  num_valid_txtframes = 0;
  for(i=0; i<txtframe_num; i++)
  {
   ret = fscanf(fp,"%ld",&tmp_txtframe);
   if ( (ret==1) && (tmp_txtframe >= 0) && (tmp_txtframe < num_of_files) )
   {
    txt_frames[num_valid_txtframes] = tmp_txtframe;
    numof_frames += txt_act_lst[ tmp_txtframe + 1 ] 
                               - txt_act_lst[ tmp_txtframe ];
    num_valid_txtframes++;
   }
   else
   {
    fprintf(stderr,"TXT_READ: bad frame number (%ld) in frame list\n",
							tmp_txtframe);
   }
  }
  /* Allocate a frame_lst of that size.
   */
  anim[anim_cnt].frame_lst = (int *)malloc(sizeof(int) * (numof_frames + 1));
  if (anim[anim_cnt].frame_lst == NULL)
                   TheEnd1("TXT_ANIM: couldn't malloc for frame_lst\0");

  j = 0;
  for(i=0; i<num_valid_txtframes; i++)
  {
   for(k=txt_act_lst[ txt_frames[i] ]; k < txt_act_lst[ txt_frames[i]+1 ]; k++)
   {
    anim[anim_cnt].frame_lst[j] = k;
    j++;
   }
  }

  free(txt_frames);
  /* put into animfile header
   */
  anim[anim_cnt].frame_lst[numof_frames] = -1;
  anim[anim_cnt].loop_frame = 0;
  anim[anim_cnt].last_frame = numof_frames - 1;
 }
 /* If there is no frame list then create a sequential one
  */
 else
 {
  int numof_actions;

  /* the number of actions is the number of frames 
   */
  numof_actions = action_cnt - action_start;
  /* malloc frame array plus 1 for ending frame
   */
  anim[anim_cnt].frame_lst = (int *)malloc(sizeof(int) * (numof_actions+1));
  if (anim[anim_cnt].frame_lst == NULL)
                   TheEnd1("TXT_ANIM: couldn't malloc for frame_lst\0");
  for(i=0; i < numof_actions; i++) anim[anim_cnt].frame_lst[i]=action_start+i;

  /* put into animfile header
   */
  anim[anim_cnt].frame_lst[numof_actions] = -1;
  anim[anim_cnt].loop_frame = 0;
  anim[anim_cnt].last_frame = numof_actions - 1;
 }

 free(txt_act_lst);
 fclose(fp);
}

