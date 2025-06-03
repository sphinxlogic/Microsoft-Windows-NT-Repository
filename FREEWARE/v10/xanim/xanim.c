
/*
 * xanim.c
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

#define DA_REV 2.29

/*
 *
 */

#include "xanim.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/signal.h>

#ifndef VMS
#include <sys/param.h>
#include <sys/times.h>
#include <memory.h>

#else
#include "xtimes.h"
#define times xtimes		/* Use My Version */
#endif

#include <ctype.h>
#include "xanim_x11.h"

void TheEnd();
void TheEnd1();
void Usage();
ULONG GetWord();
ULONG GetHalfWord();
void ShowAnimation();
void ShowAction();
int Determine_Anim_Type();
void Cycle_It();

/*
 * Global X11 display configuation variables
 *
 * These are set by X11_Pre_Setup();
 *
 */

int x11_depth;
int x11_class;
int x11_bytes_pixel;
int x11_cmap_flag;
int x11_cmap_size;
int x11_display_type;

/*
 *  To retrofit image scaling, I have decided to simply catch the XPutImage
 *  calls, and if scaling is not enabled, just do the XPutImage inline.  If
 *  a scale factor is set, then we will call the XAnimPutScaledImage routine
 *  which will scale the image by pixel replication into a scaled image
 *  buffer and output from there.
 *
 *  The 8 to 24 bit conversion is also moved into this macro to simplify things,
 *  and to allow the scaling routine to combine the conversion and the scale.
 *
 *  In a more perfect world, this might be done as part of the logic that
 *  creates the image.  But I don't want to learn this code that well.
 *
 *  Because images look a bit tiny on a workstation display, use a default of
 *  2X so that it looks good, and still runs OK.  The user can always change
 *  it to 1.
 *
 */
int x11_scale = 2;

#define XAnimPutImage(dsp,win,gc,ip,sx,sy,dx,dy,iw,ih) {\
if (x11_scale <= 1) {\
 if (x11_display_type == TRUE_COLOR_24BIT)\
  {Image8_to_Image24(ip,cmap,sx,sy,iw,ih);\
   XPutImage(dsp,win,gc,x11_work_image,0,0,dx,dy,iw,ih);}\
  else XPutImage(dsp,win,gc,ip,sx,sy,dx,dy,iw,ih);}\
 else XAnimPutScaledImage(dsp,win,gc,ip,sx,sy,dx,dy,iw,ih,cmap);\
}

/*
 * Each animation is broken up into a series of individual actions.
 * For example, a gif image would become two actions, 1 to setup the
 * colormap and 1 to display the image.
 *
 * ACTION is defined in xanim.h.
 *
 * action_cnt is a global variable that points to the next unused action.
 * Currently, individual routines access this. This will change.
 *
 * action_start is a variable passed to the Read_Animation routines, It
 * keeps track of the 1st action available to routine.
 *
 */
ACTION action[3000];
int action_cnt;
int action_start;

/*
 * This structure is set up for each animation file. It keeps track of
 * special flags and which animation type.
 *
 * anim_cnt indexes the next available anim structure.
 *
 * anim_type is one of IFF_,FLI_,GIF_,TXT_,FADE_ANIM.
 *
 * merged_anim_flags is the or of all anims read in. FLI's anims need
 * only 1 buffer. IFF anims need two. This allows software to allocate
 * for the worst case.
 *
 */
ANIM_HDR anim[50];
int anim_cnt;
int anim_type;
int merged_anim_flags;


/*
 * cmap keeps track of the current colors to the screen.
 *
 * hmap is IFF HAM specific and is needed to decode HAM images into 332
 * and into 24bits when supported.
 *
 */
ColorReg *cmap = 0;
ColorReg ham_map[HMAP_SIZE];

/*
 * Cmap Color Quantization Code
 */
ULONG db_cmap_op;
ULONG db_cmap_half;
ACTION *db_cmap_first,*db_cmap_prev,*db_cmap_current;

/*
 * Global variable to keep track of Anim type
 */
int filetype;

/*
 * Global variables to keep track of current width, height, num of colors and
 * number of bit planes respectively.
 *
 * the max_ variable are used for worst case allocation. Are useful for Anims
 * that have multiple image sizes.
 *
 * image_size and max_image_size are imagex * imagey, etc.
 */
int imagex, imagey, max_imagex, max_imagey;
int disp_y, max_disp_y;
int imagec, imaged;
int image_size;
int max_image_size;

/*
 *  No default window position, can be overridden by command.
 *
 */
int win_x_position = -1;
int win_y_position = -1;

/*
 * These variable keep track of where we are in the animation.
 * cur_file  keeps track of which file we're displaying. (ie anim[ cur_file] )
 * cur_floop keeps track of how many times we've looped a file.
 * cur_frame keeps track of which frame(action) we're on.
 *
 * cycle_wait and cycle_cnt are used for color cycling.
 *
 * start_file indicates whether this is the 1st time we've looped a file
 * or not. Is used to initialize variables and resize window if necessary.
 *
 */
int cur_file,cur_floop,cur_frame;
int cycle_wait,cycle_cnt;
int start_file;
/*
 * Image buffers.
 * im_buff1 is used for double buffered anims(IFF).
 * hbuff is need as temporary space for storing the 332 images with HAM(IFF).
 *
 * pic is a pointer to im_buff0 or im_buff1 during double buffering.
 *
 */
char *im_buff0,*im_buff1,*pic,*hbuff,*im_buffX;

/*
 * Global flags that are set on command line.
 */
int buff_flag;
int loopeach_flag;
int anim_flags;
int cycle_on_flag;
int debug;
int verbose;
int jiffy_flag;
int fade_flag,fade_time;
int noresize_flag;
int anim_holdoff;
int anim_status;
int update_flag;
int optimize_flag;

/*
 * old_dlta is used for optimizing screen updates for IFF and FLI animations.
 *
 * act is a global pointer to the current action.
 *
 */
ACT_IFF_DLTA_HDR old_dlta[2];
ACTION *act;

/*
 * This function (hopefully) provides a clean exit from our code.
 */
void TheEnd()
{
 if (action_cnt > 0 )
 {
  int i;
  for(i=0;i<action_cnt;i++)
   if (action[i].data != 0) free(action[i].data);
 }
 if (im_buff0) free(im_buff0);
 if (im_buff1) free(im_buff1);
 if (im_buffX) free(im_buffX);
 if (hbuff) free(hbuff);
 if (cmap) free(cmap);
 /*if (theDisp) XtCloseDisplay(theDisp); */
 exit(0);
}

/*
 * just prints a message before calling TheEnd()
 */
void TheEnd1(err_mess)
char *err_mess;
{
 fprintf(stderr,"%s\n",err_mess);
 TheEnd();
}

/*
 * This function attempts to expain XAnim's usage if the command line
 * wasn't correct.
 */
void Usage()
{
 fprintf(stderr,"Usage:\n\n");
 fprintf(stderr,"XAnim [ [+|-]opts ...] animfile [ [ [-opts] animfile] ... ]\n");
 fprintf(stderr,"\n");
 fprintf(stderr,"A + turns the following options on and a - turns them off.\n");
 fprintf(stderr,"Option f,j and l ignore leading + and -'s.\n");
 fprintf(stderr,"\n");
 fprintf(stderr,"Options:\n");
 fprintf(stderr,"\n");
 fprintf(stderr,"       b    buffers images ahead of time. default is off.\n");
 fprintf(stderr,"       c    disable looping for nonlooping iff anims.\n");
 fprintf(stderr,"               default is off.\n");
 fprintf(stderr,"       f#   fade to black in 16 frames. frame delay=#ms\n");
 fprintf(stderr,"               default is off.\n");
 fprintf(stderr,"       g # # window location at x,y \n");
 fprintf(stderr,"       i    Interlace flag. Reduce image height by two\n");
 fprintf(stderr,"               default is off.\n");
 fprintf(stderr,"       j#   # is number of milliseconds between frames\n");
 fprintf(stderr,"               (default is 17 unless specified inside the\n");
 fprintf(stderr,"                the animation itself).\n");
 fprintf(stderr,"       l#   # is number of loops(default = 1) through each\n"); fprintf(stderr,"               file before moving next file.\n");
 fprintf(stderr,"       r    turn color cycling off. default is on\n");
 fprintf(stderr,"       o    turns on certain optimizations. Turning this\n");
 fprintf(stderr,"               off would only be useful if you are\n");
 fprintf(stderr,"               buffering the anim and wish to run it\n");
 fprintf(stderr,"               backwards. default is on.\n");
 fprintf(stderr,"       s    prevents window from resizing to match image\n");
 fprintf(stderr,"               size. Window is the size of largest image.\n");
 fprintf(stderr,"               default is on.\n");
 fprintf(stderr,"       u    when single stepping, anim is played until\n");
 fprintf(stderr,"               image is updated/changed.(a new cmap is\n");
 fprintf(stderr,"               not considered an update) default is on.\n");
 fprintf(stderr,"       v    verbose mode. default is off.\n");
 fprintf(stderr,"       x#   scale image by # (default = 2, max = 5\n");
 fprintf(stderr,"\n");
 fprintf(stderr,"Window commands when anim is NOT running\n");
 fprintf(stderr,"\n");
 fprintf(stderr,"        q    quit\n");
 fprintf(stderr,"        g    For cycling images, move to next file\n");
 fprintf(stderr,"        s    Stop cycling\n");
 fprintf(stderr,"        r    Restore Colors after stopping cycling\n");
 fprintf(stderr,"      space  toggle. starts/stops animation\n");
 fprintf(stderr,"        ,    go back one frame within file\n");
 fprintf(stderr,"        .    go forward one frame within file\n");
 fprintf(stderr,"        <    go back to start of previous file\n");
 fprintf(stderr,"        >    go forward to start of next file\n");
 fprintf(stderr,"\n");

 exit(0);
}

main(argc, argv)
int argc;
char *argv[];
{
 char *filename,*in;
 int i,j;

/*
 * Initialize global variables.
 */
 theDisp = NULL;

 im_buff0 = 0;
 im_buff1 = 0;
 im_buffX = 0;
 hbuff = 0;
 max_imagex = 0;
 max_imagey = 0;
 max_disp_y = 0;

 anim_cnt = 0;
 action_cnt = 0;

 anim_holdoff  = TRUE;
 anim_status   = XA_UNSTARTED;
 buff_flag     = FALSE;
 cycle_on_flag = TRUE;
 noresize_flag = FALSE;
 verbose       = FALSE;
 debug         = 0;
 update_flag   = TRUE;
 optimize_flag = TRUE;

 db_cmap_op      = DB_CMAP_NONE;
 db_cmap_half    = DB_CMAP_LO;
 db_cmap_first   = 0;
 db_cmap_prev    = 0;
 db_cmap_current = 0;

 anim_flags        = ANIM_CYCON;
 merged_anim_flags = 0;
 loopeach_flag     = 1;
 jiffy_flag        = 0;

/* quick command line check.
 */
 if (argc<2) Usage();

/* setup for dying time.
 */
 signal(SIGINT,TheEnd);

 /* PreSet of X11 Display to find out what we're dealing with
  */
 X11_Pre_Setup();

/* Parse command line.
 */
 for(i=1;i<argc;i++)
 {
   in = argv[i];
   if ( (in[0]=='-') || (in[0]=='+') )
   {
     int len,opt_on;

     if (in[0]=='-') opt_on = FALSE;
     else opt_on = TRUE;

     len = strlen(argv[i]);
     j = 1;
     while( (j<len) && (in[j]!=0) )
     {
       switch(in[j])
       {
        case 'b':
		buff_flag = opt_on;
		j++;
  		break;
        case 'c':
                if (opt_on==TRUE)   anim_flags |= ANIM_NOLOP;
                else            anim_flags &= (~ANIM_NOLOP);
		j++;
  		break;
        case 'd':
		debug = atoi(&in[2]);
		if (debug <= 0) debug = 0;
                j+=len;
  		break;
        case 'f':
		fade_time = atoi(&in[2]);
		if (fade_time <= 0) fade_flag = 0;
		else fade_flag = 1;
		j+=len;
		break;
        case 'l':
		loopeach_flag = atoi(&in[2]);
		if (loopeach_flag<=0) loopeach_flag = 1;
		j+=len;
  		break;
        case 'g':
		if (i >= argc) Usage();
		i+= 1;
		j+=len;
		len = strlen(argv[i]);
	        j = 1;
		in = argv[i];
		win_x_position = atoi(&in[0]);
		if (win_x_position <= 0) win_x_position = 0;
		if (i >= argc) Usage();
		i+= 1;
		j+=len;
		len = strlen(argv[i]);
	        j = 1;
		in = argv[i];
		win_y_position = atoi(&in[0]);
		if (win_y_position <= 0) win_y_position = 0;
		j+=len;
		break;
        case 'i':
		if (opt_on==TRUE) anim_flags |= ANIM_LACE;
                else anim_flags &= (~ANIM_LACE);
                j++;
  		break;
        case 'j':
		jiffy_flag = atoi(&in[2]);
		if (jiffy_flag<=0) jiffy_flag = 1;
		j+=len;
  		break;
        case 'm':
		db_cmap_op = DB_CMAP_POP;
                j++;
  		break;
        case 'o':
		optimize_flag = opt_on;
                j++;
  		break;
        case 'r':
		cycle_on_flag = opt_on;
                if (opt_on==TRUE)   anim_flags |= ANIM_CYCON;
                else            anim_flags &= (~ANIM_CYCON);
		j++;
  		break;
        case 's':
		noresize_flag = opt_on;
                j++;
  		break;
        case 'u':
		update_flag = opt_on;
                j++;
  		break;
        case 'v':
		verbose = opt_on;
		j++;
  		break;
        case 'x':
		x11_scale = atoi(&in[2]);
		if (x11_scale <= 0) x11_scale = 1;
		else if (x11_scale >= 5) x11_scale = 5;
		j+=len;
		break;
        case '-':
		opt_on = FALSE;
		j++;
  		break;
        case '+':
		opt_on = TRUE;
		j++;
  		break;
        default:
		Usage();
       } /* end of option switch */
     } /* end of loop through options */
   } /* end of if - */
   else
   /* If no hyphen in front of argument, assume it's a file.
    */
   {
     action_start = action_cnt;
     filename = argv[i];

	/* default is FLI  */
     anim_type = Determine_Anim_Type(filename);
     anim[anim_cnt].anim_type = anim_type;
     switch(anim_type)
     {
        int action_number,ii;
        case IFF_ANIM:
		if (verbose) fprintf(stderr,"Reading IFF File %s\n",filename);
		merged_anim_flags |= ANIM_IFF;
	 	IFF_Read_File(filename);
	 	if (buff_flag) IFF_Buffer_Action(action_start);
		break;
        case GIF_ANIM:
		if (verbose) fprintf(stderr,"Reading GIF File %s\n",filename);
		GIF_Read_File(filename,1);

		action_number = action_cnt - action_start;
		anim[anim_cnt].frame_lst =
                      (int *)malloc(sizeof(int) * (action_number+1));
		if (anim[anim_cnt].frame_lst == NULL)
                      TheEnd1("GIF_ANIM: couldn't malloc for frame_lst\0");
		for(ii=0; ii < action_number; ii++)
                            anim[anim_cnt].frame_lst[ii]=action_start+ii;
                anim[anim_cnt].frame_lst[action_number] = -1;
                anim[anim_cnt].loop_frame = 0;
                anim[anim_cnt].last_frame = action_number-1;
		break;
        case TXT_ANIM:
		if (verbose) fprintf(stderr,"Reading TXT File %s\n",filename);
		TXT_Read_File(filename);
		break;
        case FLI_ANIM:
		Read_Fli_File(filename);

/* moved inside xanim_fli.c
		if (buff_flag) Fli_Buffer_Action(action_start);
		action_number = action_cnt - action_start;
		anim[anim_cnt].frame_lst =
                      (int *)malloc(sizeof(int) * (action_number+1));
		if (anim[anim_cnt].frame_lst == NULL)
                      TheEnd1("FLI_ANIM: couldn't malloc for frame_lst\0");
		for(ii=0; ii < action_number; ii++)
                            anim[anim_cnt].frame_lst[ii]=action_start+ii;
                anim[anim_cnt].frame_lst[action_number] = -1;
                anim[anim_cnt].loop_frame = 0;
                anim[anim_cnt].last_frame = action_number-1;
*/

		break;
        default:
		fprintf(stderr,"Unknown or unsupported animation type\n");
		break;
      }
      /*
       * Setup up anim header.
       */
      anim[anim_cnt].imagex = imagex;
      anim[anim_cnt].imagey = imagey;
      anim[anim_cnt].imagec = imagec;
      anim[anim_cnt].imaged = imaged;
      anim[anim_cnt].anim_flags = anim_flags;
      anim[anim_cnt].loop_num = loopeach_flag;
      merged_anim_flags |= anim_flags;

      anim_cnt++;

      if (imagex > max_imagex) max_imagex = imagex;
      if (imagey > max_imagey) max_imagey = imagey;
      if (anim_flags & ANIM_LACE)
      {
        if ((imagey/2) > max_disp_y) max_disp_y = imagey/2;
      }
      else
      {
        if ( imagey     > max_disp_y) max_disp_y = imagey;
      }

      /*
       * If fade flag is on, basically insert a fade anim.
       */
      if (fade_flag && (x11_cmap_flag == TRUE) )
      {
        int ii;
        anim[anim_cnt].anim_type = FADE_ANIM;

        /* Fade to Black action */
        action_start = action_cnt;
        action[action_cnt].type = ACT_FADE;
        action[action_cnt].time = fade_time;
        action[action_cnt].data = 0;
        action_cnt++;
        /* Blank screen so next CMAP doesn't show */
        action[action_cnt].type = ACT_FLI_BLACK;
        action[action_cnt].time = fade_time;
        action[action_cnt].data = 0;
        action_cnt++;


        /*
         * Setup frame list to call ACT_FADE sixteen times and FLI_BLACK once.
         */
        anim[anim_cnt].frame_lst = (int *)malloc(sizeof(int) * (18));
        if (anim[anim_cnt].frame_lst == NULL)
                  TheEnd1("FADE_ANIM: couldn't malloc for frame_lst\0");
        for(ii=0;ii<16;ii++) anim[anim_cnt].frame_lst[ii] = action_start;
        anim[anim_cnt].frame_lst[16] = action_start+1;
        /* Last frame is always -1
         */
        anim[anim_cnt].frame_lst[17] = -1;

        anim[anim_cnt].imagex = imagex;
        anim[anim_cnt].imagey = imagey;
        anim[anim_cnt].imagec = imagec;
        anim[anim_cnt].imaged = imaged;
        anim[anim_cnt].anim_flags = anim_flags;
        anim[anim_cnt].loop_num = 1;
        anim[anim_cnt].loop_frame = 0;
        anim[anim_cnt].last_frame = 16;
        anim_cnt++;
      } /* end of fade anim insertion */

    } /* end of read in file */
 } /* end of loopin through arguments */

 /* No anims listed
  */
 if (anim_cnt == 0) Usage();

/* What rev are we running
 */
 if (verbose)
 fprintf(stderr,"XAnim Rev %2.2f.1 Beta by Mark Podlipec (c) 1990,1991,1992\n",DA_REV);

 /* Set up X11 Display
  */

 if (noresize_flag==TRUE)
  X11_Setup(max_imagex,max_disp_y,
	    max_imagex, max_disp_y, win_x_position , win_y_position );
 else
  X11_Setup(max_imagex,max_disp_y,anim[0].imagex,
     (anim[0].anim_flags & ANIM_LACE)?(anim[0].imagey/2):(anim[0].imagey),
     win_x_position, win_y_position  );

 /* Start off Animation.
  */
 ShowAnimation();
 /* Wait for user input.
  */
 xanim_events();

 /* Self-Explanatory
  */
 TheEnd();
}

/* Routine to read a long word.
 * yeah, I know macro's are faster.
 */
ULONG GetWord(fp)
FILE *fp;
{
 ULONG ret;

 ret =  fgetc(fp);
 ret |= fgetc(fp) << 8;
 ret |= fgetc(fp) << 16;
 ret |= fgetc(fp) << 24;
 return ret;
}

/* Routine to read a half word.
 * yeah, I know macro's are faster.
 */
ULONG GetHalfWord(fp)
FILE *fp;
{
 ULONG ret;

 ret =  fgetc(fp);
 ret |= fgetc(fp) << 8;
 return ret;
}


/*
 * ShowAnimation allocates and sets up required image buffers and
 * initializes animation variables.
 * It then kicks off the animation.
 */
void ShowAnimation()
{
 max_image_size = max_imagex * max_imagey;
 im_buff0 = (char *) malloc( max_image_size * x11_bytes_pixel);
 if (im_buff0 == 0) TheEnd1("ShowAnimation: im_buff0 malloc failed");
 im_buffX = (char *) malloc( max_image_size * x11_bytes_pixel);
 if (im_buffX == 0) TheEnd1("ShowAnimation: im_buffX malloc failed");
 if (merged_anim_flags & ANIM_IFF)
 {
  im_buff1 = (char *) malloc( max_image_size * x11_bytes_pixel);
  if (im_buff1 == 0) TheEnd1("ShowAnimation: im_buff1 malloc failed");

  if ( (merged_anim_flags & ANIM_HAM) || (merged_anim_flags & ANIM_LACE) )
  {
   hbuff = (char *) malloc( max_image_size * x11_bytes_pixel);
   if (hbuff == 0) TheEnd1("ShowAnimation: hbuff malloc failed");
  }
  old_dlta[0].minx = max_imagex; old_dlta[0].miny = max_imagey;
  old_dlta[0].maxx = 0;          old_dlta[0].maxy = 0;
  old_dlta[1].minx = max_imagex; old_dlta[1].miny = max_imagey;
  old_dlta[1].maxx = 0;          old_dlta[1].maxy = 0;
 }
 pic = im_buff0;

 cur_file = 0;
 cur_floop = 0;
 cur_frame = -1;  /* since we step first */
 start_file = 0;
 cycle_wait = 0;
 cycle_cnt = 0;

 XtAppAddTimeOut(theContext, 1, ShowAction, NULL);
}

/*
 * This is the heart of this program. It does each action and then calls
 * itself with a timeout via the XtAppAddTimeOut call.
 */
void ShowAction(nothing,id)
char *nothing;
XtIntervalId *id;
{
 int screen_updated;
 u_int t_frame_start,t_frame_end,t_frame_int;
 struct tms tms3;

 /* Some actions don't change screen. Useful to know
  */
 screen_updated = FALSE;

 /* If color cycling, wait for user go ahead.
  */
 if (cycle_wait && cycle_cnt)
 {
  XtAppAddTimeOut(theContext, 50, ShowAction, NULL);
  return;
 }

  switch (anim_status)
  {
    case XA_RUN_NEXT:
			Step_Action_Next();
			break;
    case XA_RUN_PREV:
			Step_Frame_Prev();
			break;
    case XA_STEP_NEXT:
			Step_Frame_Next();
			break;
    case XA_STEP_PREV:
			Step_Frame_Prev();
			break;
    case XA_FILE_NEXT:
			Step_File_Next();
			anim_status = XA_STEP_NEXT;
			break;
    case XA_FILE_PREV:
			Step_File_Prev();
			anim_status = XA_STEP_PREV;
			break;
    case XA_UNSTARTED:
    case XA_STOP_NEXT:
    case XA_STOP_PREV:
  			anim_holdoff = FALSE;
			return;
			break;
    default:
			anim_holdoff = FALSE;
			return;
			break;
  }

 /* 1st throught this particular file.
  * Resize if necessary and init required variables.
  */
 if (start_file == 0)
 {
  start_file=1;
  cycle_wait=0;
  anim_flags = anim[cur_file].anim_flags;

  if (anim_flags & ANIM_CYCON) cycle_on_flag = 1;
  else cycle_on_flag = 0;

  /* if interlaced flag then compare half-height images to current settings.
   */
  if (noresize_flag == FALSE)
  {
   if (  anim[cur_file].anim_flags & ANIM_LACE )
   {
    if (   (imagex != anim[cur_file].imagex)
        || (disp_y != (anim[cur_file].imagey/2)) )
     XResizeWindow(theDisp,mainW,anim[cur_file].imagex * x11_scale,
		                 (anim[cur_file].imagey/2) * x11_scale);
   }
   else
   {
    if (   (imagex != anim[cur_file].imagex)
       || (disp_y != anim[cur_file].imagey) )
     XResizeWindow(theDisp,mainW,anim[cur_file].imagex * x11_scale,
                                 anim[cur_file].imagey * x11_scale);
   }
  }
  else
  {
   if (imagex > anim[cur_file].imagex)
     XClearArea(theDisp,mainW,(anim[cur_file].imagex * x11_scale),0, 0,0,FALSE);
   if (  anim[cur_file].anim_flags & ANIM_LACE )
   {
    if (disp_y > (anim[cur_file].imagey/2) )
      XClearArea(theDisp,mainW,0,(anim[cur_file].imagey/2)*x11_scale, 0,0,FALSE);
   }
   else
   {
    if (disp_y > anim[cur_file].imagey )
      XClearArea(theDisp,mainW,0,anim[cur_file].imagey*x11_scale, 0,0,FALSE);
   }
  }
  /* Initialize variables
   */
  imagex = anim[cur_file].imagex;
  imagey = anim[cur_file].imagey;
  imaged = anim[cur_file].imaged;
  imagec = anim[cur_file].imagec;
  image_size = imagex * imagey;

  /*
   * What's this!? Direct access to X11 structures. tsch tsch.
   *
   * With half-height images, I fool X11 by telling it the image is
   * twice as wide and half as high, then only display left half of image.
   *
   */
  if (anim_flags & ANIM_LACE)
  {
   theImage->width = 2*imagex;
   theImage->height = imagey/2;
   theImage->bytes_per_line = 2*imagex;
   disp_y = imagey/2;
  }
  else
  {
   theImage->width = imagex;
   theImage->height = imagey;
   theImage->bytes_per_line = imagex;
   disp_y = imagey;
  }
 }

 /* OK. A quick sanity check and then we act.
  */
 if (   (anim[cur_file].frame_lst[cur_frame] >= 0)
     && (anim[cur_file].frame_lst[cur_frame] <  action_cnt)  )
 {
    /* initialize act and get the current time.
     */
    act = &action[ anim[cur_file].frame_lst[cur_frame] ];
    t_frame_start = times(tms3);
    t_frame_int = act->time;


    /* lesdoit */
    switch(act->type)
    {
       /*
	* NOP and DELAY don't change anything but can have timing info
	* that might prove useful. ie dramatic pauses :^)
	*/
     case ACT_NOP:
			if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_NOP:\n");
			break;
     case ACT_DELAY:
			if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_DELAY:\n");
			break;
       /*
	* Change Color Map.
	*/
     case ACT_CMAP:
			{
			 int j;
			 ColorReg *cptr;
			 CMAP_HDR *cmap_hdr;

			 cmap_hdr = (CMAP_HDR *)act->data;
			 imagec = cmap_hdr->cmap_size;
			 cptr = (ColorReg *)cmap_hdr->data;

			if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"CMAP: size=%ld\n",imagec);
 			 for(j=0;j<imagec;j++)
			 {
			  ULONG map;
			  map = cptr[j].map;
			  defs[j].pixel = map;
			  defs[j].red   = cptr[j].red   << 8;
			  defs[j].green = cptr[j].green << 8;
			  defs[j].blue  = cptr[j].blue  << 8;
			  defs[j].flags = DoRed | DoGreen | DoBlue;
			  cmap[map].red   = cptr[j].red;
			  cmap[map].green = cptr[j].green;
			  cmap[map].blue  = cptr[j].blue;
			  cmap[map].map   = map;
			  if (debug >= DEBUG_LEVEL3)
				fprintf(stderr,"     <%lx %lx %lx> %ld\n",
				cptr[j].red,cptr[j].green,cptr[j].blue,map);
			 }
			 if (x11_cmap_flag == TRUE)
			 {
			   XStoreColors(theDisp,theCmap,defs,imagec);
 			   XFlush(theDisp);
			 }
			}
			break;
       /*
	* Display an Image. Assumed to by imagex by imagey in size.
	*/
     case ACT_IMAGE:

			if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_IMAGE:\n");

			theImage->data = (char *)act->data;
			XAnimPutImage(theDisp,mainW,theGC,theImage,0,0,0,0,
						       imagex,disp_y);
			XFlush(theDisp);
			cycle_wait = 1;
			screen_updated = TRUE;
			break;
       /*
	* Resize Window
	*/
     case ACT_SIZE:
                        {
                         SIZE_HDR *size_hdr;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_SIZE:\n");
                         size_hdr = (SIZE_HDR *)act->data;
                         imagex = size_hdr->imagex;
                         imagey = size_hdr->imagey;
			 if (anim_flags & ANIM_LACE)
                            disp_y = size_hdr->imagey/2;
			 else
                            disp_y = size_hdr->imagey;
			 theImage->height = disp_y;
			 theImage->width = imagex;
			 theImage->bytes_per_line = imagex;
                        }
                        break;
       /*
	* Lower Color Intensity by 1/16.
	*/
     case ACT_FADE:
			{
			 int j;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_FADE:\n");
 			 for(j=0;j<imagec;j++)
			 {
			  if (cmap[j].red   <= 16) cmap[j].red   = 0;
			  else cmap[j].red   -= 16;
			  if (cmap[j].green <= 16) cmap[j].green = 0;
			  else cmap[j].green -= 16;
			  if (cmap[j].blue  <= 16) cmap[j].blue  = 0;
			  else cmap[j].blue  -= 16;

			  defs[j].pixel = cmap[j].map;
			  defs[j].red   = cmap[j].red   << 8;
			  defs[j].green = cmap[j].green << 8;
			  defs[j].blue  = cmap[j].blue  << 8;
			  defs[j].flags = DoRed | DoGreen | DoBlue;
			 }
			 XStoreColors(theDisp,theCmap,defs,imagec);
 			 XFlush(theDisp);
			}
			screen_updated = TRUE;
			break;
       /*
	* A REGION display only part of an IMAGE.
	*/
     case ACT_REGION:
			{
			 ACT_REGION_HDR *act_reg_hdr;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_REGION:\n");
			 act_reg_hdr = (ACT_REGION_HDR *)(act->data);

			 theImage->data = (char *)(act_reg_hdr->data);

			 if (anim_flags & ANIM_LACE)
			 {
   			  XAnimPutImage(theDisp,mainW,theGC,theImage,
			         act_reg_hdr->xpos,  act_reg_hdr->ypos/2,
			         act_reg_hdr->xpos,  act_reg_hdr->ypos/2,
			         act_reg_hdr->xsize, act_reg_hdr->ysize/2  );
                         }
   			 else
			 {
			  XAnimPutImage(theDisp,mainW,theGC,theImage,
			         act_reg_hdr->xpos,  act_reg_hdr->ypos,
			         act_reg_hdr->xpos,  act_reg_hdr->ypos,
			         act_reg_hdr->xsize, act_reg_hdr->ysize  );
			 }
			 XFlush(theDisp);
			 cycle_wait = 1;
			}
			screen_updated = TRUE;
			break;
       /*
	* A IMAGE1 display only part of an IMAGE.
	*/
     case ACT_IMAGE1:
			{
			 ACT_IMAGE1_HDR *act_im_hdr;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_IMAGE1:\n");
			 act_im_hdr = (ACT_IMAGE1_HDR *)(act->data);

			 if (anim_flags & ANIM_LACE)
			 {
   			  XAnimPutImage(theDisp,mainW,theGC,act_im_hdr->image,
			         0, 0,
			         act_im_hdr->xpos,  act_im_hdr->ypos/2,
			         act_im_hdr->xsize, act_im_hdr->ysize/2  );
                         }
   			 else
			 {
			  XAnimPutImage(theDisp,mainW,theGC,act_im_hdr->image,
			         0, 0,
			         act_im_hdr->xpos,  act_im_hdr->ypos,
			         act_im_hdr->xsize, act_im_hdr->ysize  );
			 }
			 XFlush(theDisp);
			 cycle_wait = 1;
			}
			screen_updated = TRUE;
			break;
       /*
	* A CLIP displays only part of an IMAGE with a clip_mask.
	*/
     case ACT_CLIP:
			{
			 Pixmap pix_map;
			 ACT_CLIP_HDR *act_cp_hdr;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_CLIP:\n");
			 act_cp_hdr = (ACT_CLIP_HDR *)(act->data);


	pix_map = XCreatePixmapFromBitmapData(theDisp,mainW,
		act_cp_hdr->clip_ptr,
	        act_cp_hdr->xsize,act_cp_hdr->ysize,
		0x01,0x00,1);
        XSetClipMask(theDisp,theGC,pix_map);
        XSetClipOrigin(theDisp,theGC,act_cp_hdr->xpos,  act_cp_hdr->ypos);
	XAnimPutImage(theDisp,mainW,theGC,act_cp_hdr->image,
			         0, 0,
			         act_cp_hdr->xpos,  act_cp_hdr->ypos,
			         act_cp_hdr->xsize, act_cp_hdr->ysize  );
        XSetClipMask(theDisp,theGC,None);
        XFreePixmap(theDisp,pix_map);

			 XFlush(theDisp);
			 cycle_wait = 1;
			}
			screen_updated = TRUE;
			break;
       /*
	* Act upon IFF Color Cycling chunk.
	*   currently we don't color cycling during animations. will be fixed.
	*/
     case ACT_IFF_CRNG:
                        {
                         CRNG_HDR *crng;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_CRNG:\n");
                         crng = (CRNG_HDR *)act->data;
                         if (   (crng->active & CRNG_ACTIVE)
                             && (crng->low < crng->high)
                             && (crng->rate != 0)
                             && (anim_flags & ANIM_CYCLE)
                             && (cycle_on_flag)
                            )
                          {
			   cycle_cnt++;
                           XtAppAddTimeOut(theContext,
			      (int)(CRNG_INTERVAL/(crng->rate)),Cycle_It, crng);
                          }

                        }
                        break;
       /*
	*  FLI BRUN chunk.
	*/
     case ACT_FLI_BRUN:
			if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_FLI_BRUN:\n");
			Decode_Fli_BRUN(act->data,pic);
			theImage->data = pic;
   			XAnimPutImage(theDisp,mainW,theGC,theImage,0,0,0,0,
							imagex,disp_y);
			XFlush(theDisp);
			screen_updated = TRUE;
			break;
       /*
	*  FLI LC chunk
	*/
     case ACT_FLI_LC:
			{
			 ULONG xpos,ypos,xsize,ysize;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_FLI_LC:\n");
			 Decode_Fli_LC( act->data, pic,
				        &xpos,&ypos,&xsize,&ysize);
			 theImage->data = pic;
   			 XAnimPutImage(theDisp,mainW,theGC,theImage,
			           xpos,ypos,xpos,ypos,xsize,ysize );
			 XFlush(theDisp);
			}
			screen_updated = TRUE;
			break;
       /*
	*  FLI LC7 chunk
	*/
     case ACT_FLI_LC7:
			{
			 ULONG xpos,ypos,xsize,ysize;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_FLI_LC7:\n");
			 Decode_Fli_LC7( act->data, pic,
				         &xpos,&ypos,&xsize,&ysize);
			 theImage->data = pic;
   			 XAnimPutImage(theDisp,mainW,theGC,theImage,
			           xpos,ypos,xpos,ypos,xsize,ysize );
			 XFlush(theDisp);
			}
			screen_updated = TRUE;
			break;
       /*
	*  FLI_COLOR not supported. Actually it become a CMAP when read in.
	*/
     case ACT_FLI_COLOR:
			fprintf(stderr,"fli_color not supported\n");
			break;
       /*
	*  FLI COPY chunk
	*/
     case ACT_FLI_COPY:
			if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_FLI_COPY:\n");
			theImage->data = (char *)act->data;
   			XAnimPutImage(theDisp,mainW,theGC,theImage,0,0,0,0,
							imagex,disp_y);
			memcpy(pic,(char *)act->data,image_size);
			XFlush(theDisp);
			screen_updated = TRUE;
			break;
       /*
	*  Display Image of color 0.
	*/
     case ACT_FLI_BLACK:
			if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_FLI_BLACK:\n");
                        memset(im_buff0,0x00,image_size);
			theImage->data = im_buff0;

   			XAnimPutImage(theDisp,mainW,theGC,theImage,0,0,0,0,
							imagex,disp_y);
			XFlush(theDisp);
			screen_updated = TRUE;
			break;
       /*
	*  Display IFF body. Note that both buffers are set to this body.
	*  this is necessary since next delta works on 2nd buffer.
	*/
     case ACT_IFF_BODY:

			if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_IFF_BODY:\n");
			memcpy(im_buff0,(char *)act->data,image_size);
			memcpy(im_buff1,(char *)act->data,image_size);
			if (anim_flags & ANIM_HAM)
			{
   			 IFF_HAM_To_332(act->data,hbuff);
			 theImage->data = hbuff;
			}
			else
			{
			 theImage->data = (char *)act->data;
			}

   			XAnimPutImage(theDisp,mainW,theGC,theImage,0,0,
			 	           0,0,imagex,disp_y);
			XFlush(theDisp);
			cycle_wait = 1;
			old_dlta[0].minx = old_dlta[1].minx = 0;
			old_dlta[0].miny = old_dlta[1].miny = 0;
			old_dlta[0].maxx = old_dlta[1].maxx = imagex;
			old_dlta[0].maxy = old_dlta[1].maxy = imagey;
			screen_updated = TRUE;
			break;
       /*
	*  Handle IFF anim op 5.
	*/
     case ACT_IFF_DLTA5:
			{
			 ACT_IFF_DLTA_HDR *iff_dlta_hdr;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_IFF_DLTA5:\n");
			 iff_dlta_hdr = (ACT_IFF_DLTA_HDR *)(act->data);
			 IFF_Delta5(im_buff0,iff_dlta_hdr->data,iff_dlta_hdr);

		/* This mess keeps track of the largest rectangle needed to
		 * display all changes. Since things are double buffered, the
		 * min/maxes of the corners of the current and previous two
 		 * images are taken. If the animation is in single step mode
		 * it's best to display the entire image.
		 */
			if (   (anim_status != XA_RUN_NEXT)
			    && (anim_status != XA_STEP_NEXT) )
			{
			 old_dlta[0].minx = old_dlta[1].minx = 0;
			 old_dlta[0].miny = old_dlta[1].miny = 0;
			 old_dlta[0].maxx = old_dlta[1].maxx = imagex;
			 old_dlta[0].maxy = old_dlta[1].maxy = imagey;
			}
			else
			{
			 old_dlta[0].minx =
				MIN(old_dlta[0].minx,iff_dlta_hdr->minx);
			 old_dlta[0].miny =
				MIN(old_dlta[0].miny,iff_dlta_hdr->miny);
			 old_dlta[0].maxx =
				MAX(old_dlta[0].maxx,iff_dlta_hdr->maxx);
			 old_dlta[0].maxy =
				MAX(old_dlta[0].maxy,iff_dlta_hdr->maxy);
			 old_dlta[1].minx =
				MIN(old_dlta[1].minx,old_dlta[0].minx);
			 old_dlta[1].miny =
				MIN(old_dlta[1].miny,old_dlta[0].miny);
			 old_dlta[1].maxx =
				MAX(old_dlta[1].maxx,old_dlta[0].maxx);
			 old_dlta[1].maxy =
				MAX(old_dlta[1].maxy,old_dlta[0].maxy);
			}

			 if (anim_flags & ANIM_HAM)
			 {
   			  IFF_HAM_To_332(pic,hbuff);
			  theImage->data = hbuff;
			 }
			 else
			 {
			  theImage->data = im_buff0;
			 }

			 if (anim_flags & ANIM_LACE)
			 {
   			  XAnimPutImage(theDisp,mainW,theGC,theImage,
			     old_dlta[1].minx,  old_dlta[1].miny/2,
			     old_dlta[1].minx,  old_dlta[1].miny/2,
			     old_dlta[1].maxx - old_dlta[1].minx,
			    (old_dlta[1].maxy - old_dlta[1].miny)/2  );
                         }
   			 else XAnimPutImage(theDisp,mainW,theGC,theImage,
			     old_dlta[1].minx,  old_dlta[1].miny,
			     old_dlta[1].minx,  old_dlta[1].miny,
			     old_dlta[1].maxx - old_dlta[1].minx,
			     old_dlta[1].maxy - old_dlta[1].miny  );
			 XFlush(theDisp);
			 pic = im_buff0; im_buff0 = im_buff1; im_buff1 = pic;

		 /* Throw out oldest rectangle and store current
		  * you get the idea.
		  */
			 old_dlta[1].minx = old_dlta[0].minx;
			 old_dlta[1].miny = old_dlta[0].miny;
			 old_dlta[1].maxx = old_dlta[0].maxx;
			 old_dlta[1].maxy = old_dlta[0].maxy;
			 old_dlta[0].minx = iff_dlta_hdr->minx;
			 old_dlta[0].miny = iff_dlta_hdr->miny;
			 old_dlta[0].maxx = iff_dlta_hdr->maxx;
			 old_dlta[0].maxy = iff_dlta_hdr->maxy;
			}
			screen_updated = TRUE;
			break;
       /*
	*  Handle IFF anim op 3 compressed actions.
 	*  NOTE: eventually put in largest rectangle optimization.
	*/
     case ACT_IFF_DLTA3:
			{
                         ACT_IFF_DLTA_HDR *iff_dlta_hdr;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_IFF_DLTA3:\n");
                         iff_dlta_hdr = (ACT_IFF_DLTA_HDR *)(act->data);
			 IFF_Delta3(im_buff0, iff_dlta_hdr->data);
			 if (anim_flags & ANIM_HAM)
			 {
   			  IFF_HAM_To_332(pic,hbuff);
			  theImage->data = hbuff;
			 }
			 else
			 {
			  theImage->data = im_buff0;
			 }

   			 XAnimPutImage(theDisp,mainW,theGC,theImage,0,0,0,0,
			 				imagex,disp_y);
			 XFlush(theDisp);
			 pic = im_buff0; im_buff0 = im_buff1; im_buff1 = pic;
			}
			screen_updated = TRUE;
			break;
       /*
	* Handle IFF anim opt J compressed actions.
	*/
     case ACT_IFF_DLTAJ:
			{
			 ACT_IFF_DLTA_HDR *iff_dlta_hdr;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_IFF_DLTAJ:\n");
                         iff_dlta_hdr = (ACT_IFF_DLTA_HDR *)(act->data);
			 IFF_DeltaJ(im_buff0,iff_dlta_hdr->data,iff_dlta_hdr);

		/* This mess keeps track of the largest rectangle needed to
		 * display all changes. Since things are double buffered, the
		 * min/maxes of the corners of the current and previous two
 		 * images are taken.
		 */
			if (   (anim_status != XA_RUN_NEXT)
			    && (anim_status != XA_STEP_NEXT) )
			{
			 old_dlta[0].minx = old_dlta[1].minx = 0;
			 old_dlta[0].miny = old_dlta[1].miny = 0;
			 old_dlta[0].maxx = old_dlta[1].maxx = imagex;
			 old_dlta[0].maxy = old_dlta[1].maxy = imagey;
			}
			else
			{
			 old_dlta[0].minx =
				MIN(old_dlta[0].minx,iff_dlta_hdr->minx);
			 old_dlta[0].miny =
				MIN(old_dlta[0].miny,iff_dlta_hdr->miny);
			 old_dlta[0].maxx =
				MAX(old_dlta[0].maxx,iff_dlta_hdr->maxx);
			 old_dlta[0].maxy =
				MAX(old_dlta[0].maxy,iff_dlta_hdr->maxy);
			 old_dlta[1].minx =
				MIN(old_dlta[1].minx,old_dlta[0].minx);
			 old_dlta[1].miny =
				MIN(old_dlta[1].miny,old_dlta[0].miny);
			 old_dlta[1].maxx =
				MAX(old_dlta[1].maxx,old_dlta[0].maxx);
			 old_dlta[1].maxy =
				MAX(old_dlta[1].maxy,old_dlta[0].maxy);
			}

			 if (anim_flags & ANIM_HAM)
			 {
   			  IFF_HAM_To_332(pic,hbuff);
			  theImage->data = hbuff;
			 }
			 else
			 {
			  theImage->data = im_buff0;
			 }

 			 if (anim_flags & ANIM_LACE)
 			 {
   			  XAnimPutImage(theDisp,mainW,theGC,theImage,
			     old_dlta[1].minx,  old_dlta[1].miny/2,
			     old_dlta[1].minx,  old_dlta[1].miny/2,
			     old_dlta[1].maxx - old_dlta[1].minx,
			    (old_dlta[1].maxy - old_dlta[1].miny)/2  );
                         }
   			 else XAnimPutImage(theDisp,mainW,theGC,theImage,
			     old_dlta[1].minx,  old_dlta[1].miny,
			     old_dlta[1].minx,  old_dlta[1].miny,
			     old_dlta[1].maxx - old_dlta[1].minx,
			     old_dlta[1].maxy - old_dlta[1].miny  );
			 XFlush(theDisp);
			 pic = im_buff0; im_buff0 = im_buff1; im_buff1 = pic;

		 /* Throw out oldest rectangle and store current
		  * you get the idea.
		  */
			 old_dlta[1].minx = old_dlta[0].minx;
			 old_dlta[1].miny = old_dlta[0].miny;
			 old_dlta[1].maxx = old_dlta[0].maxx;
			 old_dlta[1].maxy = old_dlta[0].maxy;
			 old_dlta[0].minx = iff_dlta_hdr->minx;
			 old_dlta[0].miny = iff_dlta_hdr->miny;
			 old_dlta[0].maxx = iff_dlta_hdr->maxx;
			 old_dlta[0].maxy = iff_dlta_hdr->maxy;
			}
			screen_updated = TRUE;
                        break;
       /*
        *  Handle IFF anim op l(small L not one 1) compressed actions.
        *  NOTE: eventually put in largest rectangle optimization.
        */
     case ACT_IFF_DLTAl:
                        {
                         ACT_IFF_DLTA_HDR *iff_dlta_hdr;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_IFF_DLTAl:\n");
                         iff_dlta_hdr = (ACT_IFF_DLTA_HDR *)(act->data);
                         IFF_Deltal(im_buff0, (short *)iff_dlta_hdr->data,
                                                1,iff_dlta_hdr);
                         if (anim_flags & ANIM_HAM)
                         {
                          IFF_HAM_To_332(pic,hbuff);
                          theImage->data = hbuff;
                         }
                         else
                         {
                          theImage->data = im_buff0;
                         }

                         XAnimPutImage(theDisp,mainW,theGC,theImage,0,0,0,0,
                                                        imagex,disp_y);
                         XFlush(theDisp);
                         pic = im_buff0; im_buff0 = im_buff1; im_buff1 = pic;
                        }
			screen_updated = TRUE;
                        break;

       /*
	*  Update ham_map if it changes during an animation. ham_map is
 	*  needed in order to convert HAM images into 332.
	*/
     case ACT_IFF_HMAP:
			{
			 ColorReg *hptr;
			 int i;

			 if (debug >= DEBUG_LEVEL2)
				fprintf(stderr,"ACT_IFF_HMAP:\n");
			 hptr = (ColorReg *)act->data;
			 for(i=0;i<HMAP_SIZE;i++)
			 {
			  ham_map[i].red   = hptr[i].red;
			  ham_map[i].green = hptr[i].green;
			  ham_map[i].blue  = hptr[i].blue;
			 }
			}
			break;
     default:
			fprintf(stderr,"Unknown not supported %lx\n",act->type);
    } /* end of switch of action type */
 } /* end of action valid */

/* Move on if we're running. If we're in single step mode and the update_flag
 * is set, we move on until the screen has been updated.
 */

 if ( (anim_status == XA_STEP_NEXT) || (anim_status == XA_STEP_PREV) )
 {
   if ( (update_flag == FALSE) || (screen_updated == TRUE) )
   {
     if (anim_status == XA_STEP_NEXT)
     {
       anim_status = XA_STOP_NEXT;
       anim_holdoff = FALSE;
       return;
     }
     else if (anim_status == XA_STEP_PREV)
          {
            anim_status = XA_STOP_PREV;
	    anim_holdoff = FALSE;
            return;
          }
   }
 }


  /* Harry, what time is it?
   */
 t_frame_end = times(tms3) - t_frame_start;
 t_frame_end *= 1000/HZ; /*  convert to milliseconds */

  /* how much time left
   */
 t_frame_end = t_frame_int - t_frame_end;
  /* default to 1 ms, other wise 100 clk_tcks gives 10ms per tick.
   * NOTE: change all timing to milliseconds, not ticks.
   */
 if (t_frame_end <=0 ) t_frame_end = 1;
 if ( (anim_status != XA_STOP_NEXT) || (anim_status != XA_STOP_PREV) )
   XtAppAddTimeOut(theContext, t_frame_end, ShowAction, NULL);
 else anim_holdoff = FALSE;
}

Step_Action_Next()
{
 /* Move to next frame
  */
 cur_frame++;
 if (debug) fprintf(stderr,"frame = %ld\n",cur_frame);

 /* Are we at the end of an anim?
  */
 if (   (anim[cur_file].frame_lst[cur_frame] < 0)
     || (anim[cur_file].frame_lst[cur_frame] >= action_cnt) )
 {
   cur_frame = anim[cur_file].loop_frame;

   cur_floop++;
   if (debug) fprintf(stderr,"  loop = %ld\n",cur_floop);

   /* Done looping animation. Move on to next file if present
    */
   if (cur_floop >= anim[cur_file].loop_num)
   {
     cur_floop = 0;             /* Reset Loop Count */

     /* This is a special case check.
      * If more that one file, reset start_file, otherwise
      * if we're only displaying 1 animation jump to the loop_frame
      * which has already been set up above.
      */
     if (anim_cnt > 1)
     {
       start_file = 0;
       cur_frame = 0;
     } /* end of move on to next file/anim */

     cur_file++;

     if (cur_file >= anim_cnt)
     {
       cur_file=0;
     } /* end done with last file */
     if (debug)
       fprintf(stderr,"  file = %ld  anim_cnt = %ld\n",cur_file,anim_cnt);
   } /* end done looping file */
 } /* end done with frames in file */
}

Step_Frame_Next()
{
 /* Move to next frame
  */
 cur_frame++;
 if (debug) fprintf(stderr,"frame = %ld\n",cur_frame);

 /* Are we at the end of an anim?
  */
 if (   (anim[cur_file].frame_lst[cur_frame] < 0)
     || (anim[cur_file].frame_lst[cur_frame] >= action_cnt) )
 {
   cur_frame = anim[cur_file].loop_frame;
 }
}

Step_Frame_Prev()
{
 /* Move to next frame
  */
 cur_frame--;
 if (debug) fprintf(stderr,"frame = %ld\n",cur_frame);

 /* Are we at the beginning of an anim?
  */
 if (   (anim[cur_file].frame_lst[cur_frame] < 0)
     || (anim[cur_file].frame_lst[cur_frame] >= action_cnt)
     || (cur_frame < anim[cur_file].loop_frame)
     || (cur_frame < 0)
    )
 {
   cur_frame = anim[cur_file].last_frame;
 }
}

Step_File_Next()
{
  start_file = 0;
  cur_frame = 0;
  cur_floop = 0; /* used if things start up again */

  cur_file++;
  if (cur_file >= anim_cnt)
  {
   cur_file=0;
  } /* end done with last file */

  if (debug) fprintf(stderr,"  file = %ld\n",cur_file);
}

Step_File_Prev()
{
  start_file = 0;
  cur_frame = 0;
  cur_floop = 0; /* used if things start up again */

  cur_file--;
  if (cur_file < 0)
  {
   cur_file = anim_cnt-1;
  } /* end done with last file */

  if (debug) fprintf(stderr,"  file = %ld\n",cur_file);
}


/*
 * Simple routine to find out the file type. Defaults to FLI.
 */
int Determine_Anim_Type(filename)
char *filename;
{
 if ( Is_IFF_File(filename) ) return(IFF_ANIM);
 if ( Is_GIF_File(filename) ) return(GIF_ANIM);
 if ( Is_TXT_File(filename) ) return(TXT_ANIM);
 return(FLI_ANIM);
}

void Cycle_It(crng, id)
CRNG_HDR   *crng;
XtIntervalId *id;
{
 int             i,clow,chi;
 XColor          tmp_def;

 if ((anim_flags & ANIM_CYCLE) && cycle_on_flag)
  XtAppAddTimeOut(theContext,(int)(CRNG_INTERVAL/(crng->rate)),Cycle_It, crng);
 else cycle_cnt--;

 clow = crng->low;
 chi = crng->high;

 if (crng->active & CRNG_REVERSE)
 {
  tmp_def = defs[clow];
  for (i = clow; i < chi; i++) defs[i] = defs[i + 1];
  defs[chi] = tmp_def;
 }
 else
 {
  tmp_def = defs[chi];
  for (i = chi; i > clow; i--) defs[i] = defs[i - 1];
  defs[clow] = tmp_def;
 }

/* POD this is wrong. pixel should be i + (x11_cmap_size - imagec)
   but should be worked into crng chunk up front
 */
 for (i = clow; i <= chi; i++) defs[i].pixel = x11_cmap_size - imagec + i;

 XStoreColors(theDisp,theCmap,&defs[clow],(chi - clow + 1) );
 XFlush(theDisp);
}

