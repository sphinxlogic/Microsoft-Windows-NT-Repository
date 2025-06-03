
/*
 * xanim_iff.c
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
#include "xanim_iff.h"

void IFF_Read_File();
void IFF_Adjust_For_EHB();
void IFF_Adjust_For_HAM();
void IFF_Read_BODY();
LONG IFF_Read_Garb();
void IFF_Print_ID();
void IFF_Deltal();
void IFF_Delta3();
void IFF_Delta5();
LONG IFF_DeltaJ();
void IFF_Byte_Mod();
void IFF_Short_Mod();
void IFF_HAM_To_Bufferable();
LONG Is_IFF_File();
void IFF_Buffer_Action();
ULONG UTIL_Get_MSB_Long();
ULONG UTIL_Get_MSB_Short();
void IFF_Setup_HMAP();
void IFF_Setup_CMAP();

void ACT_Get_Next_Action();
void ACT_Setup_MAPPED();
void UTIL_Sub_Image();


#define HMAP_SIZE 16
#define IFF_SPEED_DEFAULT 3

ColorReg iff_cmap[256];

static LONG mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

/*
 *
 */
void IFF_Read_File(fname,anim_hdr)
BYTE *fname;
ANIM_HDR *anim_hdr;
{
  FILE *fin;
  LONG j;
  LONG camg_flag,cmap_flag,ret,compression;
  LONG crng_flag,cmap_action,formtype;
  LONG ansq_flag,ansq_cnt,dlta_cnt;
  LONG body_1,dlta_2;
  LONG bmhd_flag,file_size,file_read;
  Bit_Map_Header bmhd;
  Chunk_Header  chunk;
  Anim_Header   anhd;
  LONG *ansq_dnum,*ansq_tim,*dlta_act;
  LONG prop_flag;
  LONG exit_flag;
  LONG iff_imagex,iff_imagey;
  ACTION *cmap_act;
  Face_Header face;

 
  dlta_2 = -2;
  body_1 = -1;
  compression=0;
  anim_flags &= (~ANIM_HAM);
  file_size  = 0;
  file_read  = 0;
  iff_imagex = 0;
  iff_imagey = 0;
  prop_flag  = 0;
  bmhd_flag  = 0;
  crng_flag  = 0;
  camg_flag  = 0;
  cmap_flag  = 0;
  ansq_flag  = 0;
  ansq_cnt   = 0;
  ansq_dnum  = 0;
  ansq_tim   = 0;
  dlta_act   = 0;
  dlta_cnt   = 0;

  cmap_action = action_cnt;
  ACT_Get_Next_Action(&cmap_act);

  if ( (fin=fopen(fname,"r")) == 0)
  { 
    fprintf(stderr,"can't open %s\n",fname); 
    TheEnd();
  }

  exit_flag = 0; 
  while( !feof(fin) && !exit_flag)
  {
    /* read Chunk_Header 
    */
    chunk.id   = UTIL_Get_MSB_Long(fin);
    chunk.size = UTIL_Get_MSB_Long(fin);

    if ( feof(fin) ) break;    
    if (chunk.size == -1) ret = -1;
    else ret = 0;

    if (debug >= 1) 
    {
      fprintf(stderr,"Chunk.ID = ");
      IFF_Print_ID(stderr,chunk.id);
      fprintf(stderr,"  chunksize=%lx\n",chunk.size);
    }
    if (chunk.size & 1) chunk.size+=1; /* halfword pad it */
    if (ret==0)
    {
      switch(chunk.id)
      {
        case PROP: 
                prop_flag=1;
        case LIST: 
        case FORM: 
                formtype = UTIL_Get_MSB_Long(fin);
		file_size = chunk.size;
		file_read = -1;
                if (debug >= 2) 
                {
                  fprintf(stderr,"  IFF ",formtype);
                  IFF_Print_ID(stderr,chunk.id);
                  fprintf(stderr," = ");
                  IFF_Print_ID(stderr,formtype);
                  fprintf(stderr,"\n");
                }
                break;
        case BMHD:
                /* read Bit_Map_Header into bmhd */
                /* read so as to avoid endian problems */
                bmhd.width              = UTIL_Get_MSB_Short(fin);
                bmhd.height             = UTIL_Get_MSB_Short(fin);
                bmhd.x                  = UTIL_Get_MSB_Short(fin);
                bmhd.y                  = UTIL_Get_MSB_Short(fin);
                bmhd.depth              = fgetc(fin);
                bmhd.masking            = fgetc(fin);
                bmhd.compression        = fgetc(fin);
                bmhd.pad1               = fgetc(fin);
                bmhd.transparentColor   = UTIL_Get_MSB_Short(fin);
                bmhd.xAspect            = fgetc(fin);
                bmhd.yAspect            = fgetc(fin);
                bmhd.pageWidth          = UTIL_Get_MSB_Short(fin);
                bmhd.pageHeight         = UTIL_Get_MSB_Short(fin);
                if (verbose)
                {
                 fprintf(stderr,"     Size %ldx%ldx%ld comp=%ld masking=%ld\n",
                                bmhd.width,bmhd.height,bmhd.depth,
                                bmhd.compression,bmhd.masking);
                }
                imagex = bmhd.width;
                imagey = bmhd.height;
                imaged = bmhd.depth;
                if (imagex > iff_imagex) iff_imagex = imagex;
                if (imagey > iff_imagey) iff_imagey = imagey;
                bmhd_flag=1;
                break;
        case FACE:
                /* read Face_Header into face */
                /* read so as to avoid endian problems */
                face.width              = UTIL_Get_MSB_Short(fin);
                face.height             = UTIL_Get_MSB_Short(fin);
                face.x                  = UTIL_Get_MSB_Short(fin);
                face.y                  = UTIL_Get_MSB_Short(fin);
                face.xoff               = UTIL_Get_MSB_Short(fin);
                face.yoff               = UTIL_Get_MSB_Short(fin);
                if (verbose)
                {
                  fprintf(stderr,"     Size %ldx%ld %ldx%ld %ldx%ld\n",
                        face.width,face.height,
                        face.x,face.y,face.xoff,face.yoff );
                }
                imagex = face.width;
                imagey = face.height;
                if (imagex > iff_imagex) iff_imagex = imagex;
                if (imagey > iff_imagey) iff_imagey = imagey;
                switch(imagec)
                {
                  case  2: imaged = 1; break;
                  case  4: imaged = 2; break;
                  case  8: imaged = 3; break;
                  case 16: imaged = 4; break;
                  case 32: imaged = 5; break;
                  case 64: imaged = 6; break;
                  default: 
                        fprintf(stderr,"IFF Face error imagec=%ld \n",imagec);
                        fprintf(stderr,"         camg_flag=%ld \n",camg_flag);
                        fprintf(stderr,"         cmap_flag=%ld \n",cmap_flag);
                        TheEnd();
                }
                bmhd.width       = imagex;
                bmhd.height      = imagey;
                bmhd.depth       = imaged;
                bmhd.compression = BMHD_COMP_BYTERUN;
                bmhd.masking     = BMHD_MSK_NONE;

                action[action_cnt].data =
                                (UBYTE *) malloc( sizeof(SIZE_HDR) );
                if (action[action_cnt].data == 0)
                                TheEnd1("IFF_Read_FACE: malloc failed\n");
                action[action_cnt].type = ACT_SIZE;
                action[action_cnt].time = 0;
                { 
                  SIZE_HDR *size_hdr;
                  size_hdr = (SIZE_HDR *)action[action_cnt].data;
                  size_hdr->imagex = imagex;
                  size_hdr->imagey = imagey;
                  size_hdr->xoff = face.x + face.xoff;
                  size_hdr->yoff = face.y + face.yoff;
                } 
                action_cnt++;
                break;

        case CAMG:
                {
                  DEBUG_LEVEL2 fprintf(stderr,"IFF CAMG\n");
                  if (chunk.size != 4) 
                  {
                    ret = IFF_Read_Garb(fin,chunk.size);
                    break;
                  }

                  camg_flag = UTIL_Get_MSB_Long(fin);

                  if ((camg_flag & 0x80) && (cmap_flag))  
                  {
                    IFF_Adjust_For_EHB( iff_cmap );
                    IFF_Setup_CMAP(cmap_act,iff_cmap,imagec);
                    break;
                  }

                  if ((camg_flag & 0x800) && (cmap_flag)) 
                  {
                    ACTION *act;

                    IFF_Adjust_For_HAM( iff_cmap );
                    IFF_Setup_CMAP(cmap_act,iff_cmap,imagec);
                    anim_flags |= ANIM_HAM;

                        /* CREATE ACT_IFF_HMAP chunk */
                    ACT_Get_Next_Action(&act);
                    IFF_Setup_HMAP(act,ham_map);
                    break;
                  }
                }
                break;

        case CMAP:
                {
                  DEBUG_LEVEL2 fprintf(stderr,"IFF CMAP\n");

                  if (chunk.size == 0x40) imagec = chunk.size/2; /* R+GB */
                  else imagec = chunk.size / 3;  /* R+G+B 1 byte each */

                  if (chunk.size == 0x40)
                  {
                    LONG i,d;
                    for(i=0; i<imagec; i++)
                    {
                      d = fgetc(fin);
                      iff_cmap[i].red   = ((d<<4) | 0x0f) & 0xff;
                      d = fgetc(fin);
                      iff_cmap[i].green = ( d     | 0x0f) & 0xff;
                      iff_cmap[i].blue  = ((d<<4) | 0x0f) & 0xff;
                    }
                  }
                  else 
                  {
                    LONG i;
                    for(i=0;i<imagec;i++)
                    {
                      iff_cmap[i].red   = (fgetc(fin) | 0x0f) & 0xff;
                      iff_cmap[i].green = (fgetc(fin) | 0x0f) & 0xff;
                      iff_cmap[i].blue  = (fgetc(fin) | 0x0f) & 0xff;
                    }
                  }

                  /* Typically imaged matches imagec but not always 
                   */
                  if (bmhd_flag)
                  {
                    switch(imaged)
                    {
                      case  1: imagec =   2; break;
                      case  2: imagec =   4; break;
                      case  3: imagec =   8; break;
                      case  4: imagec =  16; break;
                      case  5: imagec =  32; break;
                      case  6: imagec =  64; break;
                      case  7: imagec = 128; break;
                      case  8: imagec = 256; break;
                    }
                  }

                  if (camg_flag & 0x80)  IFF_Adjust_For_EHB(iff_cmap);
                  else if (camg_flag & 0x800) 
                  {
                    ACTION *act;
                    IFF_Adjust_For_HAM( iff_cmap );
                    anim_flags |= ANIM_HAM;
                    ACT_Get_Next_Action(&act);
                    IFF_Setup_HMAP(act,ham_map);
                  }

                  if (cmap_flag)
                  {
                    ACTION *act;
                    ACT_Get_Next_Action(&act);
                    IFF_Setup_CMAP(act,iff_cmap,imagec);
                  }
                  else 
                    IFF_Setup_CMAP(cmap_act,iff_cmap,imagec);
                  cmap_flag = 1;
                }
                break;

        case BODY:
                DEBUG_LEVEL2 fprintf(stderr,"IFF BODY\n");
                dlta_cnt++;
                if (body_1 == -1) body_1 = action_cnt;
                action[action_cnt].data = 
                                (UBYTE *) malloc( imagex * imagey );
                if (action[action_cnt].data == 0) 
                                TheEnd1("IFF_Read_BODY: malloc failed\n");
                action[action_cnt].type = ACT_IFF_BODY;
                if (jiffy_flag) action[action_cnt].time = jiffy_flag;
                else action[action_cnt].time = 
                        IFF_SPEED_DEFAULT * MS_PER_60HZ;

                IFF_Read_BODY(fin,action[action_cnt].data,chunk.size,
                                (int)(bmhd.compression),(int)(bmhd.masking));
                action_cnt++;
                break;

        case ANHD:
                {
                 int i;
                 DEBUG_LEVEL2 fprintf(stderr,"IFF ANHD\n");

                 if (chunk.size >= Anim_Header_SIZE)
                 {
                  anhd.op               = fgetc(fin);
                  anhd.mask             = fgetc(fin);
                  anhd.w                = UTIL_Get_MSB_Short(fin);
                  anhd.h                = UTIL_Get_MSB_Short(fin);
                  anhd.x                = UTIL_Get_MSB_Short(fin);
                  anhd.y                = UTIL_Get_MSB_Short(fin);
                  anhd.abstime          = UTIL_Get_MSB_Long(fin);
                  anhd.reltime          = UTIL_Get_MSB_Long(fin);
                  anhd.interleave       = fgetc(fin);
                  anhd.pad0             = fgetc(fin);
                  anhd.bits             = UTIL_Get_MSB_Long(fin);
                  fread((BYTE *)(anhd.pad),1,16,fin); /* read pad */
                  i = Anim_Header_SIZE;
                  while(i < chunk.size) {fgetc(fin); i++;}

                  compression = anhd.op;
                 }
                 else 
                 {
                  IFF_Read_Garb(fin,chunk.size); 
                  compression = 0xffffffff;
                  fprintf(stderr,"ANHD chunksize mismatch %ld\n",chunk.size);
                 }
                }
                break;

        case DLTA:
                {
                 DEBUG_LEVEL2 fprintf(stderr,"IFF DLTA: ");
                 dlta_cnt++;
                 if (dlta_2 == -1) dlta_2 = action_cnt; /* 2nd DLTA */
                 if (dlta_2 == -2) dlta_2 = -1;  /* 1st DLTA */
                 action[action_cnt].data = (UBYTE *) malloc(chunk.size);
                 if (action[action_cnt].data == 0) 
                        TheEnd1("IFF_Read_DLTA: malloc failed");
                 ret=fread(action[action_cnt].data,chunk.size,1,fin);

                 switch(compression)
                 {
                  case 5:
                        DEBUG_LEVEL2 fprintf(stderr,"type 5\n");
                        action[action_cnt].type = ACT_IFF_DLTA5;
                        break;
                  case   3:
                        DEBUG_LEVEL2 fprintf(stderr,"type 3\n");
                        action[action_cnt].type = ACT_IFF_DLTA3;
                        break;
                  case 'J':
                        DEBUG_LEVEL2 fprintf(stderr,"type J\n");
                        action[action_cnt].type = ACT_IFF_DLTAJ;
                        break;
                  case  108:
                        DEBUG_LEVEL2 fprintf(stderr,"type l\n");
                        action[action_cnt].type = ACT_IFF_DLTAl;
                        break;
                  case   0:
                  case   1:
                  case   2:
                  case   4:
                  default:  
                        action[action_cnt].type = ACT_NOP;
                        fprintf(stderr,"Unimplemented Delta %ld\n",compression);
                        break;
                 }
                 if (jiffy_flag) action[action_cnt].time = jiffy_flag;
                 else action[action_cnt].time = 
                        IFF_SPEED_DEFAULT * MS_PER_60HZ;
                 action_cnt++;
                }
                break;

        case ANSQ:
               {
                ULONG i;
                UBYTE *p;  /* data is actually big endian USHORT */
                BYTE *garb;

                DEBUG_LEVEL2 fprintf(stderr,"IFF ANSQ\n");
                ansq_flag = 1;  /* we found an ansq chunk */
                ansq_cnt = chunk.size / 4;
                ansq_cnt = ansq_cnt + 1; /* adding dlta 0 up front */

                DEBUG_LEVEL2 fprintf(stderr,
			"    ansq_cnt=%ld dlta_cnt=%ld\n",ansq_cnt,dlta_cnt);

                /* allocate space for ansq variables 
                 */
                ansq_dnum = (LONG *)malloc( ansq_cnt * sizeof(LONG));
                ansq_tim  = (LONG *)malloc( ansq_cnt * sizeof(LONG));
                if ( (ansq_dnum==NULL) || (ansq_tim==NULL)) 
                    TheEnd1("ansq can't malloc dnum and tim arrays\0");
                if (verbose) fprintf(stderr,
                        "     frames=%ld dlts=%d comp=%ld\n",
                                ansq_cnt,dlta_cnt,compression);

                garb = (BYTE *)malloc(chunk.size);
                if (garb==0)
                      {fprintf(stderr,"ansq malloc not enough\n"); TheEnd();}
                fread(garb,chunk.size,1,fin);
                p = (UBYTE *)(garb);

                /* first delta is only used once and doesn't appear in 
                 * the ANSQ 
                 */
                ansq_dnum[0] = 0; 
                ansq_tim[0]  = 1;
                for(i=1;i<ansq_cnt;i++)
                {
                  /* this is delta to apply */
                  ansq_dnum[i] = (*p++)<<8; ansq_dnum[i] |= (*p++);

                  /* this is jiffy count or if 0xffff then a goto */
                  ansq_tim[i] = (*p++)<<8;  ansq_tim[i] |= (*p++);

                  DEBUG_LEVEL2 
                       fprintf(stderr,"<%ld %ld> ",ansq_dnum[i], ansq_tim[i]);
                }
                free(garb);
               }
               break;

        case CRNG: 
                DEBUG_LEVEL2 fprintf(stderr,"IFF CRNG\n");
                {
                 CRNG_HDR *crng_ptr;

                 /* is the chunk the correct size ?
                  */
                 if (chunk.size == CRNG_HDR_SIZE)
                 {
                   /* allocate space and read CRNG chunk 
                    */
                   crng_ptr = (CRNG_HDR *)malloc( sizeof(CRNG_HDR) );
                   if (crng_ptr == 0) 
                                TheEnd1("IFF_Read_CRNG: malloc failed");
                   action[action_cnt].data = (UBYTE *) crng_ptr;
                   crng_ptr->pad1   = UTIL_Get_MSB_Short(fin);
                   crng_ptr->rate   = UTIL_Get_MSB_Short(fin);
                   crng_ptr->active = UTIL_Get_MSB_Short(fin);
                   crng_ptr->low    = fgetc(fin);
                   crng_ptr->high   = fgetc(fin);

                   /* make it an action only if its valid 
                    */
                   if (   (crng_ptr->active & CRNG_ACTIVE)
                       && (crng_ptr->low < crng_ptr->high)
                       && (crng_ptr->rate != 0)
                      )
                   {
                     crng_flag++;
                     action[action_cnt].type = ACT_IFF_CRNG;
                     action[action_cnt].time = 0;
                     action_cnt++;
                   }
                   else 
                   {
                     free(crng_ptr);
                     DEBUG_LEVEL2 fprintf(stderr,"CRNG not used\n");
                   }
                 }
                 else
                 {
                    IFF_Read_Garb(fin,chunk.size); 
                    fprintf(stderr,"CRNG chunksize mismatch %ld\n",chunk.size);
                 }
                }
                break;

        case DPI : /* ignore */
        case IMRT: /* ignore */
        case GRAB: /* ignore */
        case DPPS: /* ignore */
        case DPPV: /* ignore */
        case DPAN: /* ignore */
        case DRNG: /* ignore */
        case VHDR: /* sound ignore should kill next body until bmhd*/
        case ANNO: /* sound ignore */
        case CHAN: /* sound ignore */
                ret = IFF_Read_Garb(fin,chunk.size);
                break;
    default:
	if ( feof(fin) ) exit_flag = 1;   /* end of file */
	else
	{
	  fprintf(stderr,"unknown IFF type="); IFF_Print_ID(stderr,chunk.id);
	  if (   (file_read < file_size)	/* there should be more  */
	      && (chunk.size < (file_size - file_read) ) /*  size n 2 big */
	     )
	  {
	    fprintf(stderr,"  Will Continue.\n");
	    ret = IFF_Read_Garb(fin,chunk.size);
	  }
	  else
	  {
	    fprintf(stderr,"  Will Abort.\n");
	    exit_flag = 1;
	  }
	}
	break;
   } /* end chunk switch */
   /*
    * keep track of number of bytes read. This allows us to distinguish
    * valid unknown chunks from garbage tacked to the end of a file.
    */
   if (!exit_flag)
   {
     if (file_read == -1) file_read = 4; /* assuming FORM chunk or similar */
     else file_read += chunk.size + 8; /* add ID and SIZE of other chunks */
   }

  } /* end if ret==0 */
 } /* end of while not eof or exit_flag */
 DEBUG_LEVEL2 fprintf(stderr,"Bytes Read = %lx\n",file_read);
 fclose(fin);

 if (ansq_flag)
 {
   LONG i;

   /* 
    * Set up a map of delta's to their action numbers.
    */
   dlta_act = (LONG *)malloc( sizeof(int) * dlta_cnt );
   j=0;
   for(i=action_start; i < action_cnt; i++)
   {
    switch(action[i].type)
    {
     case ACT_IFF_BODY:
     case ACT_IFF_DLTA1:
     case ACT_IFF_DLTA2:
     case ACT_IFF_DLTA3:
     case ACT_IFF_DLTA4:
     case ACT_IFF_DLTA5:
     case ACT_IFF_DLTAJ:
                           dlta_act[j] = i; j++;
                           break;
    }
   }
   DEBUG_LEVEL1 fprintf(stderr,"%ld dltas found\n",j);
   /* NOTE: since we're treating the BODY as a delta, the ansq's will be
    *   one off 
    */

   /* +2 is for cmap up front and frame termination at end */
   anim_hdr->frame_lst = (LONG *)malloc(sizeof(int) * (ansq_cnt+2));
   if (anim_hdr->frame_lst == NULL) 
             TheEnd1("IFF ANSQ: couldn't malloc for frame_lst\0");

       /* loop frame is the 2nd delta */
                anim_hdr->loop_frame = body_1 - action_start;
   anim_hdr->loop_frame = dlta_2 - action_start;  
   anim_hdr->frame_lst[0] = cmap_action;  /* cmap action */
   j=1;
   for(i=action_start; i<= body_1; i++)
   {
    anim_hdr->frame_lst[j] = i;  /* take care of frame up to body_1 */
    j++;
   }
    
   for(i=0;i<ansq_cnt;i++)
   {
     /* Goto */
     if (ansq_tim[i] == 0xffff)
     {
      anim_hdr->loop_frame = j + ansq_dnum[i]; /* cmap, body */
/* NOTE:
      anim_hdr->frame_lst[i+j] = -1;
*/
     }
     else /* delta to use */
     {
      anim_hdr->frame_lst[i+j] = dlta_act[ ansq_dnum[i]+1 ];
      /* the +1 below is because the ansq chunk ignores the body */
      if (jiffy_flag) action[ dlta_act[ ansq_dnum[i]+1 ] ].time = jiffy_flag;
      else action[ dlta_act[ ansq_dnum[i]+1 ] ].time = 
                        ansq_tim[i] * MS_PER_60HZ;
     } 
   } /* end of for ansq_cnt */
   anim_hdr->frame_lst[ansq_cnt+j] = -1;
   anim_hdr->last_frame = ansq_cnt + j - 1;
  }
   /* no ansq chunk */
  else
  {
    LONG frame_num,ii;

    frame_num = action_cnt - action_start;
    anim_hdr->frame_lst = (LONG *)malloc(sizeof(int) * (frame_num+1));
    if (anim_hdr->frame_lst == NULL)
             TheEnd1("IFF: couldn't malloc for frame_lst\0");
    for(ii=0; ii < frame_num; ii++)
           anim_hdr->frame_lst[ii]=action_start+ii;

    /* loop frame is the 2nd delta or 1st body if not two deltas 
     * or if ANIM_NOLOP set
     */
    if ( (dlta_2 < 0) || (anim_flags & ANIM_NOLOP) )
                anim_hdr->loop_frame = body_1 - action_start;
    else 
                anim_hdr->loop_frame = dlta_2 - action_start;

    anim_hdr->frame_lst[frame_num] = -1;
    anim_hdr->last_frame = frame_num - 1;
    if (verbose) fprintf(stderr,"     dlta_cnt=%ld comp=%ld\n",
                                                dlta_cnt,compression);
  }

  if (ansq_dnum) free(ansq_dnum); 
  if (ansq_tim ) free(ansq_tim ); 
  if (dlta_act ) free(dlta_act ); 
  imagex = iff_imagex;
  imagey = iff_imagey;

  if ( (dlta_cnt == 1) && (crng_flag) ) anim_flags |= ANIM_CYCLE;
  else anim_flags &= (~ANIM_CYCLE);
}


/*
 *
 */
void IFF_Adjust_For_EHB(colormap)
ColorReg colormap[];
{
  LONG i;

  DEBUG_LEVEL1 
	fprintf(stderr,"Adjusting CMAP for Amiga Extra Half-Brite Mode\n");
  for(i=0;i<32;i++)
  {
    colormap[i+32].red   = colormap[i].red   >> 2;
    colormap[i+32].green = colormap[i].green >> 2;
    colormap[i+32].blue  = colormap[i].blue  >> 2;
  }
  imagec = 64;
}


/*
 *
 */
void IFF_Adjust_For_HAM(colormap)
ColorReg colormap[];
{
  LONG i;
  DEBUG_LEVEL1 
    fprintf(stderr,"Adjusting CMAP for Amiga Hold And Modify Mode\n");

  /* save original color registers - will be need later */
  for(i=0;i<HMAP_SIZE;i++)
  {
    ham_map[i].red   = colormap[i].red;
    ham_map[i].green = colormap[i].green;
    ham_map[i].blue  = colormap[i].blue;
  }

  /* generate kludged color map */
  /* basically 3 bits red, 3 bits green and 2 bits blue */
  for(i=0;i<256;i++)
  {
    colormap[i].red   = ( ((i>>5) & 0x07) << 5 ) | 0x1f;
    colormap[i].green = ( ((i>>2) & 0x07) << 5 ) | 0x1f;
    colormap[i].blue  = ( (i & 0x03)      << 6 ) | 0x3f;
  }
  imagec = 256;
}


/*
 *
 */
void IFF_Read_BODY(fin,image_out,bodysize,compression,masking)
FILE *fin;
UBYTE *image_out;
LONG bodysize,compression,masking;
{
 LONG i,ret,x,y,d,dmask,tmp,rowsize;
 LONG imagex_pad;
 BYTE *inbuff,*rowbuff,*sptr;
 BYTE *sbuff,*dbuff;

 if (   (compression != BMHD_COMP_NONE) 
     && (compression != BMHD_COMP_BYTERUN) 
    ) TheEnd1("IFF_Read_Body: unsupported compression");

 if (   (masking != BMHD_MSK_NONE)
     && (masking != BMHD_MSK_HAS)
     && (masking != BMHD_MSK_TRANS)
    ) TheEnd1("IFF_Read_Body: unsupported masking");

 inbuff = (BYTE *)malloc(bodysize);
 if (inbuff == 0) TheEnd1("IFF_Read_Body: malloc failed");
 ret=fread(inbuff,bodysize,1,fin);
 if (ret!=1) TheEnd1("IFF_Read_Body: read of BODY chunk failed");
 sbuff = inbuff;

 rowbuff = (BYTE *)malloc( imagex );
 if (rowbuff == 0) TheEnd1("IFF_Read_Body: malloc failed");

 if (x11_display_type == PSEUDO_COLOR)
 {
   i =  (1 << imaged) - 1;
   dmask = ~i;
   memset(image_out,dmask & 0xff,(imagex * imagey));
   dmask = 1;
 }
 else memset(image_out,0x00,(imagex * imagey));

 if (compression==BMHD_COMP_NONE) sptr = inbuff;

 /* width is rounded to multiples of 16 in the BODY form */
 /* extra bits are ignored upon reading */
 imagex_pad = imagex % 16;
 if (imagex_pad) imagex_pad = imagex + 16 - imagex_pad;
 else imagex_pad = imagex;

 for(y=0; y<imagey; y++)
 {
  tmp = y * imagex;
  dmask=1;
  for(d=0; d<imaged; d++)
  {

   if (compression == BMHD_COMP_BYTERUN)
   {
/* Question */
/* imagex_pad OR imagex. Need compressed file the is not a multipleof 16 */
    rowsize = imagex_pad / 8; 
 
    dbuff = rowbuff;
    ret=UnPackRow(&sbuff,&dbuff,&bodysize,&rowsize);
    if (ret) { fprintf(stderr,"error %ld in unpack\n",ret); TheEnd();}
    sptr = rowbuff;
   }

   for(x=0; x<imagex; x+=8)
   {
    for(i=0; i<8; i++) if (mask[i] & (*sptr)) image_out[tmp+x+i] |= dmask;
    sptr++;
   }
   if (imagex_pad >= (imagex+8)) sptr++;

   dmask <<= 1;
  } /* end of depth loop */

  if (masking == BMHD_MSK_HAS)
  {
   /* read the mask row and then throw out for now */
   if (compression == BMHD_COMP_BYTERUN)
   {
    rowsize = imagex_pad / 8;
    dbuff = rowbuff;
    ret=UnPackRow(&sbuff,&dbuff,&bodysize,&rowsize);
    if (ret) { fprintf(stderr,"error %ld in unpack\n",ret); TheEnd();}
   }
   else sptr += imagex/8;
  }
 } /* end of y loop */
 free(inbuff);
 free(rowbuff);
}


/*
 *
 */
LONG IFF_Read_Garb(fp,size)
FILE *fp;
LONG size;
{
 BYTE *garb;

 garb = (BYTE *)malloc(size);
 if (garb==0)
 { fprintf(stderr,"readgarb malloc err size=%ld",size); return(-1);}
 fread(garb,size,1,fp);
 free(garb);
 return(0);
}

void IFF_Print_ID(fout,id)
FILE *fout;
LONG id;
{
 fprintf(fout,"%c",     ((id >> 24) & 0xff)   );
 fprintf(fout,"%c",     ((id >> 16) & 0xff)   );
 fprintf(fout,"%c",     ((id >>  8) & 0xff)   );
 fprintf(fout,"%c(%lx)", (id        & 0xff),id);
}


/* 
 *
 */
void IFF_Delta5(fptr,deltaptr,xs,ys,xe,ye)
UBYTE *fptr;
UBYTE *deltaptr;
ULONG *xs,*ys,*xe,*ye;
{
 register LONG i,col,depth,dmask;
 register LONG rowsize,width;
 ULONG poff;
 register UBYTE *imageptr;
 register UBYTE *dptr,opcnt,op,cnt,data;
 LONG miny,minx,maxy,maxx;

 /* set to opposites for min/max testing */
 *xs = imagex;
 *ys = imagey;
 *xe = 0;
 *ye = 0;

 imageptr = fptr;
 width = imagex;
 rowsize = width / 8;
 dmask = 1;
 for(depth=0; depth<imaged; depth++)
 {
  minx = -1;
  maxx = -1;
  
  imageptr = fptr;
  /* offset into delt chunk */
  poff  = (deltaptr[ 4 * depth    ]) << 24;
  poff |= (deltaptr[ 4 * depth + 1]) << 16;
  poff |= (deltaptr[ 4 * depth + 2]) <<  8;
  poff |= (deltaptr[ 4 * depth + 3]);

  if (poff)
  {
   dptr = (UBYTE *)((ULONG)(deltaptr) + poff);
   for(col=0;col<rowsize;col++)
   {
    imageptr = (UBYTE *)((ULONG)(fptr) + col * 8); /* start at top of column */
    opcnt = *dptr++;  /* get number of ops for this column */
    
    miny = -1;
    maxy = -1;

    while(opcnt)    /* execute ops */
    {
       /* keep track of min and max columns */
       if (minx == -1) minx = col * 8;
       maxx = (col * 8) + 7;  

       op = *dptr++;   /* get op */
     
       if (op & 0x80)    /* if type uniqe */
       {
          if (miny == -1) miny=( (ULONG)imageptr - (ULONG)fptr ) / width;
          cnt = op & 0x7f;         /* get cnt */
      
          for(i=0;i<cnt;i++)       /* loop through data */
          {
             data = *dptr++;
             IFF_Byte_Mod(imageptr,data,dmask,0);
             imageptr += width;
          }
        } /* end unique */
        else
        {
           if (op == 0)   /* type same */
           {
              if (miny == -1) miny=( (ULONG)imageptr - (ULONG)fptr ) / width;
              cnt = *dptr++;
              data = *dptr++;

              for(i=0;i<cnt;i++)       /* loop through data */
              { 
                 IFF_Byte_Mod(imageptr,data,dmask,0);
                 imageptr += width;
              }
            } /* end same */
            else
            {
               imageptr += (width * op);  /* type skip */
            }
         } /* end of hi bit clear */
       opcnt--;
     } /* end of while opcnt */
     maxy = ( (ULONG)imageptr -(ULONG)fptr ) / width;
     if ( (miny>=0) && (miny < *ys)) *ys = miny;
     if ( (maxy>=0) && (maxy > *ye)) *ye = maxy;
    } /* end of column loop */
   } /* end of valid pointer for this plane */
   dmask <<= 1;
   if ( (minx>=0) && (minx < *xs)) *xs = minx;
   if ( (maxx>=0) && (maxx > *xe)) *xe = maxx;
  } /* end of for depth */

  if (optimize_flag == TRUE)
  {
    if (*xs >= imagex) *xs = 0;
    if (*ys >= imagey) *ys = 0;
    if (*xe <= 0)      *xe = imagex;
    if (*ye <= 0)      *ye = imagey;
  }
  else
  {
    *xs = 0;      *ys = 0;
    *xe = imagex; *ye = imagey;
  }
 
} /* end of routine */

/*
 * 
 */
void IFF_Delta3(fptr,deltaptr)
UBYTE *fptr;
UBYTE *deltaptr;
{
 register LONG i,depth,dmask;
 ULONG poff;
 register SHORT  offset;
 register USHORT s,data;
 register UBYTE  *imageptr,*dptr;

 imageptr = fptr;
 dmask = 1;
 for(depth=0;depth<imaged;depth++)
 {
  imageptr = fptr;

  /*poff = planeoff[depth];*/ /* offset into delt chunk */

  poff  = (deltaptr[ 4 * depth    ]) << 24;
  poff |= (deltaptr[ 4 * depth + 1]) << 16;
  poff |= (deltaptr[ 4 * depth + 2]) <<  8;
  poff |= (deltaptr[ 4 * depth + 3]);

  if (poff)
  {
   dptr = (UBYTE *)( (ULONG)(deltaptr) + poff);
   while( (dptr[0] != 0xff) || (dptr[1] != 0xff) )
   {
     offset = (*dptr++)<<8; offset |= (*dptr++);
     if (offset >= 0)
     {
      data = (*dptr++)<<8; data |= (*dptr++);
      imageptr += 16 * (ULONG)(offset);
      IFF_Short_Mod(imageptr,data,dmask,0);
     } /* end of pos */
     else
     {
      imageptr += 16 * (ULONG)(-(offset+2));
      s = (*dptr++)<<8; s |= (*dptr++); /* size of next */
      for(i=0; i < (ULONG)s; i++)
      {
       data = (*dptr++)<<8; data |= (*dptr++);
       imageptr += 16;
       IFF_Short_Mod(imageptr,data,dmask,0);
      }
    }  /* end of neg */
   } /* end of delta for this plane */
  } /* plane has changed data */
  dmask <<= 1;
 } /* end of d */
}

/* 
 *  NOTE: optimize
 */
void IFF_Byte_Mod(iptr,data,dmask,xorflag)
register BYTE *iptr;
register UBYTE data;
ULONG dmask;
register ULONG xorflag;
{
 register LONG i,rmask;
 register UBYTE dmskoff,dmskon;

 dmskon = dmask;
 dmskoff = ~dmask;
 rmask = 0x80;
 if (xorflag)
 {
    /* if set then invert bit */

  for(i=0;i<8;i++)
  {
   if (rmask & data) *iptr++ ^= dmskon; /* invert bit */
   else iptr++;   /* do nothing but still have to inc ptr */
   rmask >>= 1;
  }
 }
 else
 {
    /* if set then set bit otherwise clear */
  for(i=0;i<8;i++)
  {
   if (rmask & data) *iptr++ |= dmskon;
   else *iptr++ &= dmskoff;
   rmask >>= 1;
  }
 }
}


/*  NOTE: optimize
 *
 */
void IFF_Short_Mod(iptr,data,dmask,xorflag)
UBYTE *iptr;
USHORT data;
ULONG dmask;
LONG xorflag;
{
 register LONG i;
 register UBYTE bdata,dmskoff,dmskon;

 dmskon = dmask;
 dmskoff = ~dmask;
 if (xorflag)
 {
  bdata = (UBYTE)(data >> 8);
  for(i=0;i<8;i++)
  {
   if (mask[i] & bdata) *iptr++ ^= dmskon;
   else iptr++;
  }
  bdata = (UBYTE)(data & 0xff);
  for(i=0;i<8;i++)
  {
   if (mask[i] & bdata) *iptr++ ^= dmskon;
   else iptr++;
  }
 }
 else
 {
  bdata = (UBYTE)(data >> 8);
  for(i=0;i<8;i++)
  {
   if (mask[i] & bdata) *iptr++ |= dmskon;
   else *iptr++ &= dmskoff;
  }
  bdata = (UBYTE)(data & 0xff);
  for(i=0;i<8;i++)
  {
   if (mask[i] & bdata) *iptr++ |= dmskon;
   else *iptr++ &= dmskoff;
  }
 }
}


/* 
 *
 */
void IFF_HAM_To_Bufferable(out,in,h_cmap,xosize,yosize,xip,yip,xisize)
UBYTE *out,*in;
ColorReg *h_cmap;
ULONG xosize,yosize,xip,yip,xisize;
{
  register ULONG x,y;
  register ULONG pred,pgreen,pblue,data,xend;

  xend = xip + xosize;
  for (y=0; y < (yip + yosize); y++)
  {
   /* NOTE: this might should be moved before y loop */
    pred=pgreen=pblue=0;
    for (x=0; x<xisize; x++)
    {
      data = (ULONG )(*in++);
      switch( (data & 0x30) )
      {
        case 0x00: /* use color register given by low */
          {
            register ULONG low;
            low = data & 0x0f;
            pred   = h_cmap[low].red;
            pgreen = h_cmap[low].green;
            pblue  = h_cmap[low].blue;
          }
          break;
        case 0x10: /* hold red and green   but change blue to low*/
          pblue = ((data << 4) | 0x0f) & 0xff;
          break;
        case 0x20: /* hold green and blue  but change red to low */
          pred = ((data << 4) | 0x0f) & 0xff;
          break;
        case 0x30: /* hold red and blue    but change green to low */
          pgreen = ((data << 4) | 0x0f) & 0xff;
          break;
      } /* end of switch */
      if ( (y >= yip) && (x >= xip) && (x < xend) )
      {
        register ULONG t_color;
        if (x11_display_type == TRUE_COLOR)
        {
           t_color = X11_Get_True_Color(pred,pgreen,pblue,8);
           if (x11_bytes_pixel == 4)
		{ ULONG *ulp = (ULONG *)out;  *ulp = (ULONG)(t_color); }
           else if (x11_bytes_pixel == 2)
		{ USHORT *usp = (USHORT *)out;  *usp = (USHORT)(t_color); }
           else *out = (UBYTE)(t_color);
	   out += x11_bytes_pixel;
        }
        else *out++ =     (pred   & 0xe0) | ((pgreen & 0xe0) >> 3)
                       | ((pblue  & 0xc0) >> 6);
      } /* end of output */
    } /* end of x */
  } /* end of y */
}


/* 
 *
 */
LONG Is_IFF_File(filename)
BYTE *filename;
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

 if (firstword == FORM) return(TRUE);
 if (firstword == LIST) return(TRUE);
 if (firstword == PROP) return(TRUE);
 return(FALSE);
}


/* 
 *
 */
void IFF_Buffer_Action(action_start)
LONG action_start;
{
 LONG i,image_size;
 UBYTE *buff0,*buff1,*tmp;
 ACTION *act;
 ULONG buff_csize;
 ColorReg *image_map;

 image_size = imagex * imagey;
 buff0 = (UBYTE *) malloc( image_size );
 if (buff0 == 0) TheEnd1("IFF Buffer Action: malloc failed 0");

 buff1 = (UBYTE *) malloc( image_size );
 if (buff1 == 0) 
 {
  free(buff0);
  TheEnd1("IFF Buffer Action: malloc failed 1");
 }

 buff_csize = imagec;  /* for IFF files with different image types */
 i=action_start;
 while(i<action_cnt)
 {
  act = &action[i];
  switch(act->type)
  {
   case ACT_CMAP:
        {
          CMAP_HDR *cmap_hdr;

          cmap_hdr = (CMAP_HDR *)act->data;
          image_map = (ColorReg *)cmap_hdr->data;
	  buff_csize = cmap_hdr->cmap_size;
        }
        break;
 
   case ACT_SIZE:       
        { 
          SIZE_HDR *size_hdr;
          size_hdr = (SIZE_HDR *)act->data;
          imagex = size_hdr->imagex;
          imagey = size_hdr->imagey;
        } 
        break;

   case ACT_IFF_DLTA5:
   case ACT_IFF_DLTAJ:
        {
          int minx,miny,maxx,maxy; 
          int pic_size,pic_x,pic_y;
          UBYTE *t_pic,*w_pic;

          switch(act->type)
          {
            case ACT_IFF_DLTA5:
                IFF_Delta5(buff0, act->data, &minx, &miny, &maxx, &maxy);
                break;
            case ACT_IFF_DLTAJ:
                IFF_DeltaJ(buff0, act->data, &minx, &miny, &maxx, &maxy);
                break;
          }
          free(act->data);      /* free delta chunk */
          IFF_Update_DLTA_HDR(&minx,&miny,&maxx,&maxy);

          pic_x = maxx - minx; pic_y = maxy - miny;
          pic_size = pic_x * pic_y;
          t_pic = (UBYTE *) malloc( pic_size * x11_bytes_pixel);
          if (t_pic == 0) TheEnd1("Buff IFF DLTA5: malloc failed");

          IFF_Image_To_Bufferable(&w_pic, t_pic, buff0,
                image_map, ham_map, buff_csize, pic_x, pic_y,
                minx, miny, imagex, imagey, anim_flags);
          if (w_pic == buff0) memcpy((char *)t_pic,(char *)buff0,pic_size);

	  ACT_Setup_Mapped(act, act->time, t_pic, image_map, buff_csize,
		minx, miny, pic_x, pic_y, TRUE, 0);


          tmp = buff0; buff0 = buff1; buff1 = tmp;
        }
        break;

   case ACT_IFF_BODY:
   case ACT_IFF_DLTA3:
   case ACT_IFF_DLTAl:
        {
          UBYTE *t_pic,*w_pic;
          switch(act->type)
          {
            case ACT_IFF_BODY:
                memcpy((char *)buff0, (char *)act->data, image_size);
                memcpy((char *)buff1, (char *)act->data, image_size);
                IFF_Init_DLTA_HDR(imagex,imagey);
                break;

            case ACT_IFF_DLTA3:
                IFF_Delta3(buff0, act->data);
                break;
            case ACT_IFF_DLTAl:
                IFF_Deltal(buff0, act->data, 1);
                break;
          }

          free(act->data);         /* free delta chunk */
          t_pic = (UBYTE *) malloc( image_size * x11_bytes_pixel);

          IFF_Image_To_Bufferable(&w_pic, t_pic, buff0,
                image_map, ham_map, buff_csize, imagex, imagey, 0, 0,
		imagex, imagey, anim_flags);
          if (w_pic == buff0) memcpy((char *)t_pic,(char *)buff0,image_size);

	  ACT_Setup_Mapped(act, act->time, t_pic, image_map, buff_csize,
		0, 0, imagex, imagey, TRUE, 0);

          tmp = buff0; buff0 = buff1; buff1 = tmp;
        }
        break;

   case ACT_IFF_HMAP:   
        {
          ColorReg *hptr;
          LONG i;

          hptr = (ColorReg *)act->data;
          for(i=0;i<HMAP_SIZE;i++)
          {
            ham_map[i].red   = hptr[i].red;
            ham_map[i].green = hptr[i].green;
            ham_map[i].blue  = hptr[i].blue;
          }
          act->type = ACT_NOP;
          free(act->data);
          act->data = 0;
        }
        break;
  }
  i++;
 }
 if (buff0) free(buff0);
 if (buff1) free(buff1);
}

LONG IFF_DeltaJ(fptr,deltaptr,tminx,tminy,tmaxx,tmaxy)
UBYTE *fptr;
register UBYTE *deltaptr;
LONG *tminx,*tminy,*tmaxx,*tmaxy;
{
 register LONG rowsize,width;
 register UBYTE *imageptr;
 register LONG exitflag;
 register ULONG  type,r_flag,b_cnt,g_cnt,r_cnt; 
 register ULONG b,g,r;
 register ULONG offset,dmask,depth;
 register UBYTE data;
 LONG changed;
 LONG tmp,minx,miny,maxx,maxy;
 LONG kludge_j;
 /* this kludge is because xanimations with width less than 320 get
  * centered in the middle of a 320 screen. Does this happen with
  * animations greater than lores overscan(374) and less than hi-res(640)????
  */

 if (imagex >= 320) kludge_j = 0;
 else kludge_j = (320-imagex)/2;

 maxx=0;
 maxy=0;
 minx=imagex;
 miny=imagey;

 changed=0;
 width = imagex;
 rowsize = width / 8;
 exitflag=0;
 while(!exitflag)
 {
  /* read compression type and reversible_flag(xor data not just set) 
   */
  type   = (*deltaptr++) << 8; type   |= (*deltaptr++);
  r_flag = (*deltaptr++) << 8; r_flag |= (*deltaptr++);
  DEBUG_LEVEL2 
        fprintf(stderr,"J type=%lx rev_flag=%lx imaged=%lx\n",
                                                type,r_flag,imaged);

  /* switch on compression type */
  switch(type)
  {
   case 0: exitflag = 1; break; /* end of list */
   case 1:
      /* Get byte count and group count 
       */
      b_cnt = (*deltaptr++) << 8; b_cnt |= (*deltaptr++);
      g_cnt = (*deltaptr++) << 8; g_cnt |= (*deltaptr++);
      DEBUG_LEVEL2 
                fprintf(stderr,"byte group cnts %lx %lx\n",b_cnt,g_cnt);
      
      /* Loop thru groups
       */
      for(g=0; g<g_cnt; g++)
      {
        offset = (*deltaptr++) << 8; offset |= (*deltaptr++);
        DEBUG_LEVEL2 
                fprintf(stderr,"  g%ld) offset=%lx\n",g,offset);

        offset <<= 3; /* counts bytes */
        if (kludge_j) 
             offset = ((offset/320) * imagex) + (offset%320) - kludge_j;

        imageptr = (UBYTE *)((ULONG)fptr + offset);

        tmp = offset%imagex; if (tmp<minx) minx=tmp;
        tmp += 8;            if (tmp>maxx) maxx=tmp;
        tmp = offset/imagex; if (tmp<miny) miny=tmp;
        tmp += b_cnt;        if (tmp>maxy) maxy=tmp;
       
        /* Loop thru byte count 
         */
        for(b=0; b < b_cnt; b++)
        {
          dmask = 1;
          for(depth=0;depth<imaged;depth++) /* loop thru planes */
          {
            data = *deltaptr++;
            DEBUG_LEVEL3 fprintf(stderr,"<%lx>",data);
            changed |= data;          /* CHECKFORZERO change */
            IFF_Byte_Mod(imageptr,data,dmask,r_flag);
            dmask <<= 1;
          } /* end of depth loop */
          DEBUG_LEVEL3 fprintf(stderr,"\n");
          imageptr += width; /* direction is vertical */
        } /* end of byte loop */
      } /* end of group loop */
      break;
   case 2:
      /* Read row count, byte count and group count 
       */
      r_cnt = (*deltaptr++) << 8; r_cnt |= (*deltaptr++);
      b_cnt = (*deltaptr++) << 8; b_cnt |= (*deltaptr++);
      g_cnt = (*deltaptr++) << 8; g_cnt |= (*deltaptr++);
      DEBUG_LEVEL2 fprintf(stderr,"row byte group cnts %lx %lx %lx\n",
                           r_cnt,b_cnt,g_cnt);
      
      /* Loop thru groups
       */
      for(g=0; g < g_cnt; g++)
      {
        offset = (*deltaptr++) << 8; offset |= (*deltaptr++);
        DEBUG_LEVEL2 fprintf(stderr,"  g%ld) offset=%lx\n",g,offset);
        offset <<= 3; /* counts bytes */
        if (kludge_j) 
             offset = ((offset/320) * imagex) + (offset%320) - kludge_j;

        tmp = offset%imagex;     if (tmp<minx) minx=tmp;
        tmp += b_cnt * 8;        if (tmp>maxx) maxx=tmp;
        tmp = offset/imagex;     if (tmp<miny) miny=tmp;
        tmp += r_cnt;            if (tmp>maxy) maxy=tmp;
       
        /* Loop thru rows of group
         */
        for(r=0; r < r_cnt; r++)
        {
          DEBUG_LEVEL2 fprintf(stderr,"     ROW%lx ++%ld,%ld++",r,
              (offset%width),(offset/width + r));
          dmask = 1;
          for(depth=0;depth<imaged;depth++) /* loop thru planes */
          {
            imageptr = (UBYTE *)(fptr + offset + (r * width));
            for(b=0; b < b_cnt; b++) /* loop thru byte count */
            {
              data = *deltaptr++;
              DEBUG_LEVEL3 fprintf(stderr,"<%lx>",data);
              changed |= data;        /* CHECKFORZERO */
           
              IFF_Byte_Mod(imageptr,data,dmask,r_flag);
              imageptr += 8;        /* data is horizontal */
            } /* end of byte loop */
            dmask <<= 1;
            DEBUG_LEVEL3 fprintf(stderr,"((%lx))\n",depth);
          } /* end of depth loop */
        } /* end of row loop */
      } /* end of group loop */
      break;
   default: /* don't know this one yet */
            fprintf(stderr,"Unknown J-type %x\n",type);
            type = 0;      /* force an exit */
            exitflag = 1;
            break;
  } /* end of type switch */
 } /* end of while loop */

 if (optimize_flag == TRUE)
 {
   *tminx = minx; *tminy = miny;
   *tmaxx = maxx; *tmaxy = maxy;

   if (*tminx >= imagex) *tminx = 0;
   if (*tminy >= imagey) *tminy = 0;
   if (*tmaxx <= 0)      *tmaxx = imagex;
   if (*tmaxy <= 0)      *tmaxy = imagey;
   DEBUG_LEVEL2 fprintf(stderr,"xypos=<%ld,%ld> xysize=<%ld %ld>\n",
		*tminx,*tminy,*tmaxx,*tmaxy );
 }
 else
 {
   *tminx = 0; *tminy = 0;
   *tmaxx = imagex; *tmaxy = imagey;
 }

 /* if changed is zero then this Delta didn't change the image at all */ 
 return(changed);
} /* end of routine */

/* 
 *  Decode IFF type l anims
 */
void IFF_Deltal(fptr,deltaptr,vertflag)
UBYTE *fptr;
UBYTE *deltaptr;
LONG vertflag;
{
 register LONG i,depth,dmask,width;
 ULONG poff0,poff1;
 register UBYTE *imageptr;
 register UBYTE *optr,*dptr;
 register SHORT cnt;
 register USHORT offset,data;

 imageptr = fptr;
 if (vertflag) width = imagex;
 else width = 16;
 dmask = 1;
 for(depth = 0; depth<imaged; depth++)
 {
   imageptr = fptr;
   /*poff = planeoff[depth];*/ /* offset into delt chunk */
   poff0  = (deltaptr[ 4 * depth    ]) << 24;
   poff0 |= (deltaptr[ 4 * depth + 1]) << 16;
   poff0 |= (deltaptr[ 4 * depth + 2]) <<  8;
   poff0 |= (deltaptr[ 4 * depth + 3]);

   if (poff0)
   {
     poff1  = (deltaptr[ 4 * (depth+8)    ]) << 24;
     poff1 |= (deltaptr[ 4 * (depth+8) + 1]) << 16;
     poff1 |= (deltaptr[ 4 * (depth+8) + 2]) <<  8;
     poff1 |= (deltaptr[ 4 * (depth+8) + 3]);

     dptr = (UBYTE *)( (ULONG)(deltaptr) + 2 * poff0); 
     optr = (UBYTE *)( (ULONG)(deltaptr) + 2 * poff1); 

     /* while short *optr != -1 */
     while( (optr[0] != 0xff) || (optr[1] != 0xff) )
     {
       offset = (*optr++) << 8; offset |= (*optr++);
       cnt    = (*optr++) << 8; cnt    |= (*optr++);
 
       if (cnt < 0)  /* cnt negative */
       {
         imageptr = fptr + 16 * (ULONG)(offset);
         cnt = -cnt;
         data = (*dptr++) << 8; data |= (*dptr++);
         for(i=0; i < (ULONG)cnt; i++)
         {
           IFF_Short_Mod(imageptr,data,dmask,0);
           imageptr += width;
         }
       }  /* end of neg */
       else/* cnt pos then */
       {
         imageptr = fptr + 16 * (ULONG)(offset);
         for(i=0; i < (ULONG)cnt; i++)
         {
           data = (*dptr++) << 8; data |= (*dptr++);
           IFF_Short_Mod(imageptr,data,dmask,0);
           imageptr += width;
         }
       } /* end of pos */
     } /* end of delta for this plane */
   } /* plane has changed data */
   dmask <<= 1;
 } /* end of d */
}

void
IFF_Setup_HMAP(act,hmap)
ACTION *act;
ColorReg *hmap;
{
  ColorReg *hptr;
  ULONG i;

  act->data = (UBYTE *) malloc(HMAP_SIZE * sizeof(ColorReg) );
  if (act->data == 0) TheEnd1("IFF_Setup_HMAP: malloc failed\n");
  act->type = ACT_IFF_HMAP;
  act->time = 0;
  hptr = (ColorReg *) act->data;
  for(i=0; i < HMAP_SIZE; i++) 
  {
    hptr[i].red   = hmap[i].red;
    hptr[i].green = hmap[i].green;
    hptr[i].blue  = hmap[i].blue;
  }
}

void
IFF_Setup_CMAP(act,cmap_ptr,sizec)
ACTION *act;
ColorReg *cmap_ptr;
ULONG sizec;
{
  ColorReg *act_cmap;
  CMAP_HDR *cmap_hdr; 
  ULONG i;
  LONG tmp;

  cmap_hdr = (CMAP_HDR *)
        malloc( sizec * sizeof(ColorReg) + sizeof(CMAP_HDR));
  if (cmap_hdr == 0) TheEnd1("IFF_Setup_CMAP: malloc failed\n");

  act->type = ACT_CMAP;
  act->time = 0;
  act->data = (UBYTE *)cmap_hdr;

  cmap_hdr->cmap_size = sizec;

  act_cmap = (ColorReg *)cmap_hdr->data;
  
  tmp = x11_cmap_size - sizec;
  if ( (tmp < 0) && (x11_cmap_flag == TRUE) )
  {
    fprintf(stderr,"IFF image has larger colormap than display.\n");
    fprintf(stderr,"This is not yet supported.\n");
    TheEnd();
  }
  else
  for(i = 0; i < sizec; i++)
  {
    act_cmap[i].red   = cmap_ptr[i].red;
    act_cmap[i].green = cmap_ptr[i].green;
    act_cmap[i].blue  = cmap_ptr[i].blue;
    if (x11_display_type == TRUE_COLOR)
      act_cmap[i].map = X11_Get_True_Color(
          cmap_ptr[i].red, cmap_ptr[i].green, cmap_ptr[i].blue, 8);
    else if (x11_display_type == MONOCHROME) act_cmap[i].map = i;
    else act_cmap[i].map = i + tmp;
  }
}
 
IFF_DLTA_HDR iff_dlta[2];

IFF_Init_DLTA_HDR(max_x,max_y)
ULONG max_x,max_y;
{
  iff_dlta[0].minx = iff_dlta[1].minx = 0;
  iff_dlta[0].miny = iff_dlta[1].miny = 0;
  iff_dlta[0].maxx = iff_dlta[1].maxx = imagex;
  iff_dlta[0].maxy = iff_dlta[1].maxy = imagey;
}

IFF_Update_DLTA_HDR(min_x,min_y,max_x,max_y)
LONG *min_x,*min_y,*max_x,*max_y;
{
  LONG tmin_x,tmin_y,tmax_x,tmax_y;

 /* This mess keeps track of the largest rectangle needed to
  * display all changes. Since things are double buffered, the
  * min/maxes of the corners of the current and previous two
  * images are taken. If the animation is in single step mode
  * it's best to display the entire image.
  */

  tmin_x = *min_x;
  tmin_y = *min_y;
  tmax_x = *max_x;
  tmax_y = *max_y;
  iff_dlta[0].minx = MIN(iff_dlta[0].minx, tmin_x);
  iff_dlta[0].miny = MIN(iff_dlta[0].miny, tmin_y);
  iff_dlta[0].maxx = MAX(iff_dlta[0].maxx, tmax_x);
  iff_dlta[0].maxy = MAX(iff_dlta[0].maxy, tmax_y);
  *min_x = iff_dlta[1].minx = MIN(iff_dlta[1].minx, iff_dlta[0].minx);
  *min_y = iff_dlta[1].miny = MIN(iff_dlta[1].miny, iff_dlta[0].miny);
  *max_x = iff_dlta[1].maxx = MAX(iff_dlta[1].maxx, iff_dlta[0].maxx);
  *max_y = iff_dlta[1].maxy = MAX(iff_dlta[1].maxy, iff_dlta[0].maxy);

/* Throw out oldest rectangle and store current.
 */
  iff_dlta[1].minx = iff_dlta[0].minx;
  iff_dlta[1].miny = iff_dlta[0].miny;
  iff_dlta[1].maxx = iff_dlta[0].maxx;
  iff_dlta[1].maxy = iff_dlta[0].maxy;
  iff_dlta[0].minx = tmin_x;
  iff_dlta[0].miny = tmin_y;
  iff_dlta[0].maxx = tmax_x;
  iff_dlta[0].maxy = tmax_y;
}

  
IFF_Image_To_Bufferable(i_ptr,out,in,map,h_cmap, map_size,
                        xosize,yosize,xip,yip,xisize,yisize,flags)
UBYTE **i_ptr,*out,*in;
ColorReg *map,*h_cmap;
ULONG map_size;
ULONG xosize,yosize,xip,yip,xisize,yisize,flags;
{

  if (x11_display_type == MONOCHROME)
  {
    register ULONG line_size;

    *i_ptr = out;
    line_size = X11_Get_Line_Size(xisize);

    /* NOTE: this code currently assumes no optimization under MONOCHROME */
    if (flags & ANIM_HAM)
    {
      IFF_HAM_To_Bufferable(out,in,h_cmap,xisize,yisize,0,0,xisize);
      UTIL_Mapped_To_Bitmap(out,out,map,map_size,xisize,yisize,line_size);
    }
    else UTIL_Mapped_To_Bitmap(out,in,map,map_size,xisize,yisize,line_size);
    return;
  }

  if (flags & ANIM_HAM)
  {
    *i_ptr = out;
    IFF_HAM_To_Bufferable(out,in,h_cmap,xosize,yosize,xip,yip,xisize);
    return;
  }

  if (x11_display_type == TRUE_COLOR)
  {
    *i_ptr = out;
    UTIL_Sub_Mapped_To_True(out, in, map, xosize, yosize,
                                xip, yip, xisize);
  }
  else
  {
    /* Optimize if xip==yip==0 and xosize==imagex and yosize==imagey */
    *i_ptr = out;
    UTIL_Sub_Image(out, in, xosize, yosize, xip, yip, xisize, 1);
  }
}

IFF_Image_To_Displayable(i_ptr,out,in,map,h_cmap,map_size,xop,yop,x_im,y_im,
                         xosize,yosize,xip,yip,xisize,yisize,flags)
UBYTE **i_ptr,*out,*in;
ColorReg *map,*h_cmap;
ULONG map_size;
ULONG *xop,*yop,*x_im,*y_im;
ULONG xosize,yosize,xip,yip,xisize,yisize,flags;
{

  *xop = 0; *yop = 0; *x_im = xosize; *y_im = yosize;

  if (x11_display_type == MONOCHROME)
  {
    register ULONG line_size;

    *i_ptr = out;
    line_size = X11_Get_Line_Size(xisize);

    if (flags & ANIM_HAM)
    {
      IFF_HAM_To_Bufferable(out,in,h_cmap,xisize,yisize,0,0,xisize);
      UTIL_Mapped_To_Bitmap(out,out,map,map_size,xisize,yisize,line_size);
    }
    else UTIL_Mapped_To_Bitmap(out,in,map,map_size,xisize,yisize,line_size);
    *x_im = xisize; *y_im = yisize;
    return;
  }

  if (flags & ANIM_HAM)
  {
    *i_ptr = out;
    IFF_HAM_To_Bufferable(out,in,h_cmap,xosize,yosize,xip,yip,xisize);
    return;
  }

  if (x11_display_type == TRUE_COLOR)
  {
    *i_ptr = out;
    UTIL_Sub_Mapped_To_True(out, in, map, xosize, yosize, xip, yip, xisize);
    return;
  }
  else /* PSEUDO COLOR display */
  {
    *i_ptr = in;
    *xop = xip; *yop = yip; *x_im = xisize; *y_im = yisize;
  }
}
