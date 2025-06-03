
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
#define DA_MINOR_REV 7

/*
 * Any problems, suggestions, solutions, etc contact:
 *
 * podlipec@dgxyris.webo.dg.com
 *
 */

#include "xanim.h"
#include "xanim_config.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <sys/signal.h>
#include <sys/param.h>
#include <sys/times.h>
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
void Step_File_Next();
void Step_File_Prev();
void Step_Frame_Next();
void Step_Frame_Prev();
void Step_Action_Next();
void Free_Actions();
ANIM_HDR *Get_Anim_Hdr();

/*
 * Global X11 display configuation variables
 *
 * These are set by X11_Pre_Setup();
 *
 */

int x11_depth;
int x11_class;
int x11_bytes_pixel;
int x11_bitmap_unit;
int x11_bit_order;
int x11_cmap_flag;
int x11_cmap_size;
int x11_display_type;
int x11_red_mask;
int x11_green_mask;
int x11_blue_mask;
int x11_red_shift;
int x11_green_shift;
int x11_blue_shift;
int x11_red_size;
int x11_green_size;
int x11_blue_size;
int x11_black;
int x11_white;
int x11_verbose_flag;

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
 * anim_type is one of IFF_,FLI_,GIF_,TXT_,FADE_ANIM. 
 *
 * merged_anim_flags is the or of all anims read in. FLI's anims need
 * only 1 buffer. IFF anims need two. This allows software to allocate
 * for the worst case.
 *
 */
ANIM_HDR anim;
int anim_type;
int merged_anim_flags;


/*
 * cmap keeps track of the current colors to the screen.
 *
 * hmap is IFF HAM specific and is needed to decode HAM images into 332
 * and into 24bits when supported.
 *
 * image_map is used for encoded(any image not in the correct format) images.
 * and is used to map image pixels to their correct color map entry.
 * also used to map  mapped images to TRUE_COLOR.
 *
 */
ColorReg *cmap = 0;
ColorReg ham_map[HMAP_SIZE];
ColorReg *image_map = 0;
int cmap_luma_sort;
int cmap_try_to_1st;
int cmap_map_to_1st;

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
 * These variable keep track of where we are in the animation.
 * cur_file  ptr to the header of the current anim file. 
 * cur_floop keeps track of how many times we've looped a file.
 * cur_frame keeps track of which frame(action) we're on.
 *
 * cycle_wait and cycle_cnt are used for color cycling.
 *
 * file_is_started indicates whether this is the 1st time we've looped a file
 * or not. Is used to initialize variables and resize window if necessary.
 * 
 */
ANIM_HDR *cur_file   = 0;
ANIM_HDR *first_file = 0;
int cur_floop,cur_frame;
int cycle_wait,cycle_cnt;
int file_is_started;
/*
 * Image buffers.
 * im_buff1 is used for double buffered anims(IFF).
 * disp_buff is needed when the display is of a different format than the
 * double buffered images. (like HAM or TRUE_COLOR).
 *
 * pic is a pointer to im_buff0 or im_buff1 during double buffering.
 *
 */
char *im_buff0,*im_buff1,*pic,*disp_buff;

/* 
 * Global flags that are set on command line.
 */
int buff_flag;
int cycle_on_flag;
int fade_flag,fade_time;
int noresize_flag;
int update_flag;
int optimize_flag;
int pixmap_flag;
int dither_flag;
int pack_flag;
int debug;
int verbose;

int loopeach_flag;
int jiffy_flag;

int anim_flags;
int anim_holdoff;
int anim_status;
int anm_map_flag;

/*
 * old_dlta is used for optimizing screen updates for IFF and FLI animations.
 *
 * act is a global pointer to the current action.
 *
 */
ACTION *act;

/*
 * This function (hopefully) provides a clean exit from our code.
 */
void TheEnd()
{
  ANIM_HDR *tmp_hdr;

  Free_Actions();
  cur_file = first_file;
  first_file->prev_file->next_file = 0;  /* last file's next ptr to 0 */
  while( cur_file != 0)
  {
    tmp_hdr = cur_file->next_file;
    free(cur_file);
    cur_file = tmp_hdr;
  }
  if (im_buff0) free(im_buff0);
  if (im_buff1) free(im_buff1);
  if (disp_buff) free(disp_buff);
  if (cmap) free(cmap);
  if (theDisp) XtCloseDisplay(theDisp);
  exit(0);
}

void Hard_Death()
{
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

void
Usage_Default_TF(flag,justify)
ULONG flag,justify;
{
  if (justify == 1) fprintf(stderr,"            ");
  if (flag == TRUE) fprintf(stderr," default is on.\n");
  else fprintf(stderr," default is off.\n");
}

void
Usage_Default_Num(num,justify)
ULONG num,justify;
{
  if (justify == 1) fprintf(stderr,"            ");
  fprintf(stderr," default is %ld.\n",num);
}

/*
 * This function attempts to expain XAnim's usage if the command line
 * wasn't correct.
 */
void Usage()
{
 fprintf(stderr,"Usage:\n\n");
 fprintf(stderr,"xanim [ [+|-]opts ...] animfile [ [ [+|-opts] animfile] ... ]\n");
 fprintf(stderr,"\n");
 if (DEFAULT_PLUS_IS_ON == TRUE) 
      fprintf(stderr,"A + turns an option on and a - turns it off.\n");
 else fprintf(stderr,"A - turns an option on and a + turns it off.\n");
 fprintf(stderr,"Options d,f,j and l ignore leading + and -'s.\n");
 fprintf(stderr,"Leave a space after #'s\n");
 fprintf(stderr,"\n");
 fprintf(stderr,"Options:\n");
 fprintf(stderr,"\n");
 fprintf(stderr,"       Cl   luma sort colormaps.");
 Usage_Default_TF(DEFAULT_CMAP_LUMA_SORT,0);
 fprintf(stderr,"       Ct   try to fit new cmaps to 1st cmap.");
 Usage_Default_TF(DEFAULT_CMAP_TRY_TO_1ST,0);
 fprintf(stderr,"       Cf   force/map new cmaps to 1st cmap.");
 Usage_Default_TF(DEFAULT_CMAP_MAP_TO_1ST,0);
 fprintf(stderr,"       F    Floyd-Steinberg dithering when needed.\n");
 Usage_Default_TF(DEFAULT_DITHER_FLAG,1);
 fprintf(stderr,"       b    buffers images ahead of time."); 
 Usage_Default_TF(DEFAULT_BUFF_FLAG,0);
 fprintf(stderr,"       c    disable looping for nonlooping iff anims.\n"); 
 Usage_Default_TF(DEFAULT_IFF_LOOP_OFF,1);
 fprintf(stderr,"       d#   debug. 1 to 5 for level of report.\n"); 
 Usage_Default_Num(DEFAULT_DEBUG,1);
 fprintf(stderr,"       f#   fade to black in 16 frames. frame delay=#ms\n");
 Usage_Default_TF(DEFAULT_FADE_FLAG,1);
 fprintf(stderr,"       i    Interlace flag. Reduce image height by two\n"); 
 Usage_Default_TF(FALSE,1);
 fprintf(stderr,"       j#   # is number of milliseconds between frames\n");
 fprintf(stderr,"	     if 0 then default depends on the animation\n");
 Usage_Default_Num(DEFAULT_JIFFY_FLAG,1);
 fprintf(stderr,"       l#   # is number of times each file is shown\n");
 fprintf(stderr,"             before moving on.\n");
 Usage_Default_Num(DEFAULT_LOOPEACH_FLAG,1);
 fprintf(stderr,"       o    turns on certain optimizations. See readme\n"); 
 Usage_Default_TF(DEFAULT_OPTIMIZE_FLAG,1);
 fprintf(stderr,"       p    Use Pixmap instead of Image in X11.\n"); 
 Usage_Default_TF(DEFAULT_PIXMAP_FLAG,1);
 fprintf(stderr,"       r    turn color cycling off. Currently only for\n");
 fprintf(stderr,"            single images."); 
 Usage_Default_TF(DEFAULT_CYCLE_ON_FLAG,0);
 fprintf(stderr,"       s    prevents window from resizing to match image\n"); 
 fprintf(stderr,"             size. Window is the size of largest image.\n"); 
 Usage_Default_TF(DEFAULT_NORESIZE_FLAG,1);
 fprintf(stderr,"       u    when single stepping, anim is played until\n");
 fprintf(stderr,"             image is updated/changed.(a new cmap is\n");
 fprintf(stderr,"             not considered an update).");
 Usage_Default_TF(DEFAULT_UPDATE_FLAG,0);
 fprintf(stderr,"       v    verbose mode."); 
 Usage_Default_TF(DEFAULT_VERBOSE,0);
 fprintf(stderr,"\n");
 fprintf(stderr,"Window commands.\n");
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
 disp_buff = 0;
 max_imagex = 0;
 max_imagey = 0;
 max_disp_y = 0;
 image_map = cmap;

 first_file = cur_file = 0;
 action_cnt = 0;

 anim_holdoff  = TRUE;
 anim_status   = XA_UNSTARTED;

 buff_flag     = DEFAULT_BUFF_FLAG;
 pack_flag     = DEFAULT_PACK_FLAG;
 cycle_on_flag = DEFAULT_CYCLE_ON_FLAG;
 noresize_flag = DEFAULT_NORESIZE_FLAG;
 verbose       = DEFAULT_VERBOSE;
 debug         = DEFAULT_DEBUG;
 update_flag   = DEFAULT_UPDATE_FLAG;
 optimize_flag = DEFAULT_OPTIMIZE_FLAG;
 pixmap_flag   = DEFAULT_PIXMAP_FLAG;
 dither_flag   = DEFAULT_DITHER_FLAG;
 loopeach_flag = DEFAULT_LOOPEACH_FLAG;
 jiffy_flag    = DEFAULT_JIFFY_FLAG;
 x11_verbose_flag = DEFAULT_X11_VERBOSE_FLAG;
 cmap_luma_sort   = DEFAULT_CMAP_LUMA_SORT;
 cmap_try_to_1st  = DEFAULT_CMAP_TRY_TO_1ST;
 cmap_map_to_1st  = DEFAULT_CMAP_MAP_TO_1ST;

 anim_flags        = 0;
ANIM_CYCON;
 merged_anim_flags = 0;

 if (DEFAULT_IFF_LOOP_OFF  == TRUE) anim_flags |= ANIM_NOLOP;
 if (DEFAULT_CYCLE_ON_FLAG == TRUE) anim_flags |= ANIM_CYCON;
 if (DEFAULT_LACE_FLAG     == TRUE) anim_flags |= ANIM_LACE;

/* What rev are we running
 */
 fprintf(stderr,"XAnim Rev %2.2f.%ld  by Mark Podlipec (c) 1990,1991,1992\n",DA_REV,DA_MINOR_REV);

/* quick command line check.
 */
 if (argc<2) Usage();

/* setup for dying time.
 */
 signal(SIGINT,Hard_Death);

 /* PreSet of X11 Display to find out what we're dealing with
  */
 X11_Pre_Setup(argc, argv);

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
     /* if + turns off then reverse opt_on */
     if (DEFAULT_PLUS_IS_ON == FALSE) opt_on = (opt_on==TRUE)?FALSE:TRUE;

     len = strlen(argv[i]);
     j = 1;
     while( (j<len) && (in[j]!=0) )
     {
       switch(in[j])
       {
	int tmp_len;
        case 'C':
		  j++;
		  switch(in[j])
		  {
		    case 'l': cmap_luma_sort  = opt_on; j++; break;
		    case 't': cmap_try_to_1st = opt_on; j++; break;
		    case 'f': cmap_map_to_1st = opt_on; j++; break;
		  }
                break;
        case 'F':
                dither_flag = opt_on;	j++;	break;
        case 'b':
                buff_flag = opt_on;	j++;	break;
        case 'c':
                if (opt_on==TRUE) anim_flags |= ANIM_NOLOP;
                else anim_flags &= (~ANIM_NOLOP);
                j++; break;
        case 'd':
                j++; debug = atoi(&in[j]);
                if (debug <= 0) debug = 0;
		j += strlen(&in[j]);	break;
        case 'f':
                j++; fade_time = atoi(&in[j]);
                if (fade_time <= 0) fade_flag = 0;
                else fade_flag = 1;
		j += strlen(&in[j]);	break;
        case 'l':
		j++; loopeach_flag = atoi(&in[j]);
                if (loopeach_flag<=0) loopeach_flag = 1;
		j += strlen(&in[j]);	break;
        case 'i':
                if (opt_on==TRUE) anim_flags |= ANIM_LACE;
                else anim_flags &= (~ANIM_LACE);
                j++;	break;
        case 'j':
                j++; jiffy_flag = atoi(&in[j]);
                if (jiffy_flag<=0) jiffy_flag = 1;
		j += strlen(&in[j]);
                break;
        case 'o':
                optimize_flag = opt_on;
                j++;	break;
        case 'p':
                pixmap_flag = opt_on;	j++;	break;
        case 'r':
                cycle_on_flag = opt_on;
                if (opt_on==TRUE)   anim_flags |= ANIM_CYCON;
                else            anim_flags &= (~ANIM_CYCON);
                j++;
                break;
        case 's':
                noresize_flag = opt_on;	j++;	break;
        case 'u':
                update_flag = opt_on;	j++;	break;
        case 'v':
                verbose = opt_on;	j++;	break;
        case '-':
                opt_on = (DEFAULT_PLUS_IS_ON==TRUE)?FALSE:TRUE; j++;	break;
        case '+':
                opt_on = (DEFAULT_PLUS_IS_ON==TRUE)?TRUE:FALSE; j++;	break;
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

     if ( (x11_display_type == MONOCHROME) || (pack_flag == TRUE) )
       anm_map_flag = FALSE;
     else
       anm_map_flag = TRUE;
        /* default is FLI  */
     cur_file = Get_Anim_Hdr(cur_file);
     anim_type = Determine_Anim_Type(filename);
     cur_file->anim_type = anim_type;
     if (x11_display_type == MONOCHROME) optimize_flag = FALSE;
     switch(anim_type)
     {
        int action_number,ii;


        case IFF_ANIM:
                if (verbose) fprintf(stderr,"Reading IFF File %s\n",filename);
                merged_anim_flags |= ANIM_IFF;
                IFF_Read_File(filename,cur_file);
                if (buff_flag) IFF_Buffer_Action(action_start);
                break;
        case GIF_ANIM:
                if (verbose) fprintf(stderr,"Reading GIF File %s\n",filename);
                GIF_Read_File(filename);

                action_number = action_cnt - action_start;
                cur_file->frame_lst = 
                      (int *)malloc(sizeof(int) * (action_number+1));
                if (cur_file->frame_lst == NULL)
                      TheEnd1("GIF_ANIM: couldn't malloc for frame_lst\0");
                for(ii=0; ii < action_number; ii++)
                            cur_file->frame_lst[ii]=action_start+ii;
                cur_file->frame_lst[action_number] = -1;
                cur_file->loop_frame = 0;
                cur_file->last_frame = action_number-1;
                break;
        case TXT_ANIM:
                if (verbose) fprintf(stderr,"Reading TXT File %s\n",filename);
                TXT_Read_File(filename,cur_file);
                break;
        case FLI_ANIM:
                Fli_Read_File(filename,cur_file);
                break;
        case DL_ANIM:
                DL_Read_File(filename,cur_file);
                break;
/* Temporary
        case JPG_ANIM:
                JFIF_Read_File(filename,cur_file);

                action_number = action_cnt - action_start;
                cur_file->frame_lst =
                      (int *)malloc(sizeof(int) * (action_number+1));
                if (cur_file->frame_lst == NULL)
                      TheEnd1("JPG_ANIM: couldn't malloc for frame_lst\0");
                for(ii=0; ii < action_number; ii++)
                            cur_file->frame_lst[ii]=action_start+ii;
                cur_file->frame_lst[action_number] = -1;
                cur_file->loop_frame = 0;
                cur_file->last_frame = action_number-1;
                break;
*/
        default:
                fprintf(stderr,"Unknown or unsupported animation type\n");
                break;
      } 
      /*
       * Setup up anim header.
       */
      cur_file->imagex = imagex;
      cur_file->imagey = imagey;
      cur_file->imagec = imagec;
      cur_file->imaged = imaged;
      cur_file->anim_flags = anim_flags;
      cur_file->loop_num = loopeach_flag;
      merged_anim_flags |= anim_flags;

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

        cur_file = Get_Anim_Hdr(cur_file);
        cur_file->anim_type = FADE_ANIM;
   
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
        cur_file->frame_lst = (int *)malloc(sizeof(int) * (18));
        if (cur_file->frame_lst == NULL)
                  TheEnd1("FADE_ANIM: couldn't malloc for frame_lst\0");
        for(ii=0;ii<16;ii++) cur_file->frame_lst[ii] = action_start;
        cur_file->frame_lst[16] = action_start+1;
        /* Last frame is always -1
         */
        cur_file->frame_lst[17] = -1;

        cur_file->imagex = imagex;
        cur_file->imagey = imagey;
        cur_file->imagec = imagec;
        cur_file->imaged = imaged;
        cur_file->anim_flags = anim_flags;
        cur_file->loop_num = 1;
        cur_file->loop_frame = 0;
        cur_file->last_frame = 16;
      } /* end of fade anim insertion */

    } /* end of read in file */
 } /* end of loopin through arguments */

 /* No anims listed
  */
 if (first_file == 0) Usage();

 /* Set up X11 Display
  */

 if (noresize_flag==TRUE)
  X11_Setup(max_imagex,max_disp_y,max_imagex,max_disp_y);
 else X11_Setup(max_imagex,max_disp_y,first_file->imagex,
   (first_file->anim_flags & ANIM_LACE)?(first_file->imagey/2):
					(first_file->imagey)  );

 if (x11_display_type == MONOCHROME) optimize_flag = FALSE;
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
  if (merged_anim_flags & ANIM_IFF)
  {
    im_buff1 = (char *) malloc( max_image_size * x11_bytes_pixel);
    if (im_buff1 == 0) TheEnd1("ShowAnimation: im_buff1 malloc failed");
  }

  if (   (merged_anim_flags & ANIM_HAM) 
      || (merged_anim_flags & ANIM_LACE)
      || (x11_display_type == MONOCHROME)
      || (x11_display_type == TRUE_COLOR) );
  {
     disp_buff = (char *) malloc( max_image_size * x11_bytes_pixel);
     if (disp_buff == 0) TheEnd1("ShowAnimation: disp_buff malloc failed");
  }
 pic = im_buff0;

 cur_file = first_file;
 cur_floop = 0;
 cur_frame = -1;  /* since we step first */
 file_is_started = FALSE;
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
 int t_frame_start,t_frame_end,t_frame_int;
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

  DEBUG_LEVEL1 fprintf(stderr,"frame = %ld\n",cur_frame);

 /* 1st throught this particular file.
  * Resize if necessary and init required variables.
  */
 if (file_is_started == FALSE)
 {
  file_is_started = TRUE;
  cycle_wait = 0;
  anim_flags = cur_file->anim_flags;

  if (anim_flags & ANIM_CYCON) cycle_on_flag = 1;
  else cycle_on_flag = 0;

  /* if interlaced flag then compare half-height images to current settings.
   */
  if (noresize_flag == FALSE)
  {
   if (cur_file->anim_flags & ANIM_LACE)
   {
    if (   (imagex != cur_file->imagex)
        || (disp_y != (cur_file->imagey/2)) ) 
     XResizeWindow(theDisp,mainW,cur_file->imagex,
                                 cur_file->imagey/2);
   }
   else
   {
    if (   (imagex != cur_file->imagex)
       || (disp_y != cur_file->imagey) ) 
     XResizeWindow(theDisp,mainW,cur_file->imagex,
                                 cur_file->imagey);
   }
  }
  else
  {
   if (imagex > cur_file->imagex)
     XClearArea(theDisp,mainW,cur_file->imagex,0, 0,0,FALSE);
   if (  cur_file->anim_flags & ANIM_LACE )
   {
    if (disp_y > (cur_file->imagey>>1) )
      XClearArea(theDisp,mainW,0,(cur_file->imagey/2), 0,0,FALSE);
   }
   else
   {
    if (disp_y > cur_file->imagey )
      XClearArea(theDisp,mainW,0,cur_file->imagey, 0,0,FALSE);
   }
  }
  /* Initialize variables
   */
  imagex = cur_file->imagex;
  imagey = cur_file->imagey;
  imaged = cur_file->imaged;
  imagec = cur_file->imagec;
  image_size = imagex * imagey;

  disp_y = (anim_flags & ANIM_LACE)?(imagey>>1):imagey;

  X11_Init_Image_Struct(theImage,imagex,imagey,anim_flags);

 }
 
 /* OK. A quick sanity check and then we act.
  */
 if (   (cur_file->frame_lst[cur_frame] >= 0)
     && (cur_file->frame_lst[cur_frame] <  action_cnt)  )
 {
    /* initialize act and get the current time.
     */
    act = &action[ cur_file->frame_lst[cur_frame] ];
    t_frame_start = times(&tms3);
    t_frame_int = act->time;


    /* lesdoit */
    switch(act->type)
    {
       /* 
        * NOP and DELAY don't change anything but can have timing info
        * that might prove useful. ie dramatic pauses :^)
        */
     case ACT_NOP:      
                        DEBUG_LEVEL2 fprintf(stderr,"ACT_NOP:\n");
                        break;
     case ACT_DELAY:    
                        DEBUG_LEVEL2 fprintf(stderr,"ACT_DELAY:\n");
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
	  image_map = cptr = (ColorReg *)cmap_hdr->data;
	
	  DEBUG_LEVEL2 fprintf(stderr,"CMAP: size=%ld\n",imagec);
	  if (x11_cmap_flag == TRUE)
	  {
	    for(j=0;j<imagec;j++)
	    {
	      ULONG map;
	      map = cptr[j].map;
	      defs[j].pixel = map;
	      defs[j].red   = cptr[j].red   << 8;
	      defs[j].green = cptr[j].green << 8;
	      defs[j].blue  = cptr[j].blue  << 8;
	      defs[j].flags = DoRed | DoGreen | DoBlue;
	      cmap[j].red   = cptr[j].red;
	      cmap[j].green = cptr[j].green;
	      cmap[j].blue  = cptr[j].blue;
	      cmap[j].map   = map;
	      DEBUG_LEVEL5 fprintf(stderr,"     <%lx %lx %lx> %ld\n",
		  cptr[j].red,cptr[j].green,cptr[j].blue,map);
	    }
	    XStoreColors(theDisp,theCmap,defs,imagec);
	    XFlush(theDisp);
	  }
	}
	break;
       /* 
        * Resize Window
        */
     case ACT_SIZE:
        {
          SIZE_HDR *size_hdr;

          DEBUG_LEVEL2 fprintf(stderr,"ACT_SIZE:\n");
          size_hdr = (SIZE_HDR *)act->data;
          imagex = size_hdr->imagex;
          imagey = size_hdr->imagey;
	  disp_y = (anim_flags & ANIM_LACE)?(imagey>>1):imagey;
          X11_Init_Image_Struct(theImage,imagex,imagey,anim_flags);
        }
        break;
       /* 
        * Lower Color Intensity by 1/16.
        */
     case ACT_FADE:     
                        {
                         int j;

                         DEBUG_LEVEL2 fprintf(stderr,"ACT_FADE:\n");
                         if ( (x11_display_type == PSEUDO_COLOR) &&
                              (x11_cmap_flag    == TRUE) )
                         {
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
                        }
                        screen_updated = TRUE;
                        break;
       /* 
        * A IMAGE display only part of an IMAGE.
        */
     case ACT_IMAGE:   
	{
	  ACT_IMAGE_HDR *act_im_hdr;

	  DEBUG_LEVEL2 fprintf(stderr,"ACT_IMAGE:\n");
	  act_im_hdr = (ACT_IMAGE_HDR *)(act->data);

	  XPutImage(theDisp,mainW,theGC,act_im_hdr->image, 0, 0,
		act_im_hdr->xpos,  act_im_hdr->ypos,
		act_im_hdr->xsize, act_im_hdr->ysize  );
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

	  DEBUG_LEVEL2 fprintf(stderr,"ACT_CLIP:\n");
	  act_cp_hdr = (ACT_CLIP_HDR *)(act->data);


          pix_map = XCreatePixmapFromBitmapData(theDisp,mainW,
		act_cp_hdr->clip_ptr,
		act_cp_hdr->xsize,act_cp_hdr->ysize,
		0x01,0x00,1);
          XSetClipMask(theDisp,theGC,pix_map);
          XSetClipOrigin(theDisp,theGC,act_cp_hdr->xpos,  act_cp_hdr->ypos);
          XPutImage(theDisp,mainW,theGC,act_cp_hdr->image, 0, 0,
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
        * A PIXMAP 
        */
     case ACT_PIXMAP:   
	{
	  ACT_PIXMAP_HDR *act_pm_hdr;

	  DEBUG_LEVEL2 fprintf(stderr,"ACT_PIXMAP:\n");
	  act_pm_hdr = (ACT_PIXMAP_HDR *)(act->data);

	  XCopyArea(theDisp,act_pm_hdr->pixmap,mainW,theGC, 0, 0, 
		act_pm_hdr->xsize, act_pm_hdr->ysize,
		act_pm_hdr->xpos,  act_pm_hdr->ypos   );
	  XSync(theDisp,False);
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

	  DEBUG_LEVEL2 fprintf(stderr,"ACT_CRNG:\n");
	  if (   (x11_display_type == PSEUDO_COLOR)
	      && (x11_cmap_flag    == TRUE) )
	  {
	    crng = (CRNG_HDR *)act->data;
	    if (   (crng->active & CRNG_ACTIVE)
	        && (crng->low < crng->high)
	        && (crng->rate != 0)
	        && (anim_flags & ANIM_CYCLE) 
	        && (cycle_on_flag)		)
	    {
	      cycle_cnt++;
	      XtAppAddTimeOut(theContext,(int)(CRNG_INTERVAL/(crng->rate)),
			Cycle_It, crng);
	    }
	  }

	}
	break;

       /* 
        *  FLI BRUN chunk
        *  FLI LC chunk
        *  FLI LC7 chunk
        *  FLI COPY chunk
        *  FLI BLACK chunk
        */
     case ACT_FLI_BRUN:   
     case ACT_FLI_LC:   
     case ACT_FLI_LC7:
     case ACT_FLI_COPY:
     case ACT_FLI_BLACK:        
	{
	  ULONG xpos,ypos,xsize,ysize;

	  switch(act->type)
	  {
	    case ACT_FLI_BRUN: 
		DEBUG_LEVEL2 fprintf(stderr,"ACT_FLI_BRUN:\n");
		Decode_Fli_BRUN(image_map, act->data,pic,TRUE);
		xpos = ypos = 0; xsize = imagex;  ysize = imagey;
		break;
	    case ACT_FLI_LC:
		DEBUG_LEVEL2 fprintf(stderr,"ACT_FLI_LC:\n");
		Decode_Fli_LC( image_map, act->data, pic, 
			&xpos,&ypos,&xsize,&ysize,TRUE);
		break;
	    case ACT_FLI_LC7:
		DEBUG_LEVEL2 fprintf(stderr,"ACT_FLI_LC7:\n");
		Decode_Fli_LC7( image_map, act->data, pic, 
			&xpos,&ypos,&xsize,&ysize,TRUE);
		break;
	    case ACT_FLI_COPY:
		DEBUG_LEVEL2 fprintf(stderr,"ACT_FLI_COPY:\n");
		if (x11_display_type == TRUE_COLOR)
		{
		  memcpy(disp_buff,(char *)act->data,image_size);
		  UTIL_Sub_Mapped_To_True(pic,disp_buff,image_map,
					0,0,imagex,imagey,imagex);
		}
		else memcpy(pic,(char *)act->data,image_size);
		xpos = ypos = 0; xsize = imagex;  ysize = imagey;
		break;
	    case ACT_FLI_BLACK:        
		DEBUG_LEVEL2 fprintf(stderr,"ACT_FLI_BLACK:\n");
		memset(pic,0x00,image_size * x11_bytes_pixel);
		xpos = ypos = 0; xsize = imagex;  ysize = imagey;
		break;
	  }

	  if (x11_display_type == MONOCHROME)
	  {
	    ULONG line_size;
	    line_size = X11_Get_Line_Size(imagex);
	    UTIL_Mapped_To_Bitmap(disp_buff,pic,image_map,imagec,
			imagex,imagey,line_size);
	    theImage->data = disp_buff;
	  }
	  else theImage->data = pic;

	  X11_Init_Image_Struct(theImage,imagex,imagey,anim_flags);
          if (anim_flags & ANIM_LACE) 
	  { 
	    if (!(ypos & 0x01)) ysize++;
	    ypos++; ypos >>= 1;
	    ysize >>= 1;
	  }
	  XPutImage(theDisp,mainW,theGC,theImage,
		xpos,ypos,xpos,ypos,xsize,ysize );
	  XSync(theDisp,False);
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
        *  Display IFF body. Note that both buffers are set to this body.
        *  this is necessary since next delta works on 2nd buffer.
        * 
        *  Handle IFF anim op 3 and op l(small L) compressed actions.
        */
     case ACT_IFF_BODY:
     case ACT_IFF_DLTAl:
     case ACT_IFF_DLTA3:
	{
        int src_x,src_y,im_x,im_y;
        switch(act->type)
        {
         case ACT_IFF_DLTAl:
            DEBUG_LEVEL2 fprintf(stderr,"ACT_IFF_DLTAl:\n");
            IFF_Deltal(im_buff0, (short *)act->data, 1);
            break;
         case ACT_IFF_DLTA3:
            DEBUG_LEVEL2 fprintf(stderr,"ACT_IFF_DLTA3:\n");
            IFF_Delta3(im_buff0, act->data);
            break;
         case ACT_IFF_BODY:
            DEBUG_LEVEL2 fprintf(stderr,"ACT_IFF_BODY:\n");
            memcpy(im_buff0,(char *)act->data,image_size);
            memcpy(im_buff1,(char *)act->data,image_size);
            IFF_Init_DLTA_HDR(imagex,imagey);
            break;
        }
        IFF_Image_To_Displayable( &(theImage->data), disp_buff, im_buff0,
                image_map, ham_map, imagec,
                &src_x, &src_y, &im_x, &im_y,
		imagex, imagey, 0, 0, imagex, imagey, anim_flags);
        X11_Init_Image_Struct(theImage,im_x,im_y,anim_flags);
        XPutImage(theDisp,mainW,theGC,theImage,0,0,0,0,imagex,disp_y);
        XSync(theDisp,False);
        screen_updated = TRUE;
        if (act->type == ACT_IFF_BODY) cycle_wait = 1;
        else { pic = im_buff0; im_buff0 = im_buff1; im_buff1 = pic; }
	}
        break;


       /* 
        *  Handle IFF anim op 5 and opt J compressed actions. 
        */
     case ACT_IFF_DLTAJ:
     case ACT_IFF_DLTA5:
        {
          int minx,miny,maxx,maxy,pic_x,pic_y,src_x,src_y,im_x,im_y;

          if (act->type == ACT_IFF_DLTAJ)
          {
            DEBUG_LEVEL2 fprintf(stderr,"ACT_IFF_DLTAJ:\n");
            IFF_DeltaJ(im_buff0, act->data, &minx, &miny, &maxx, &maxy);
          }
          else if (act->type == ACT_IFF_DLTA5)
          {
            DEBUG_LEVEL2 fprintf(stderr,"ACT_IFF_DLTA5:\n");
            IFF_Delta5(im_buff0, act->data, &minx, &miny, &maxx, &maxy);
          }
          else break;

          if ( (anim_status != XA_RUN_NEXT) && (anim_status != XA_STEP_NEXT) )
          {
            minx = miny = 0; maxx = imagex; maxy = imagey;
          }
          IFF_Update_DLTA_HDR(&minx,&miny,&maxx,&maxy);
          pic_x = maxx - minx; pic_y = maxy - miny;
          IFF_Image_To_Displayable( &(theImage->data), disp_buff, im_buff0,
                image_map, ham_map, imagec,
                &src_x, &src_y, &im_x, &im_y, 
                pic_x, pic_y, minx, miny, imagex, imagey, anim_flags);

          X11_Init_Image_Struct(theImage,im_x,im_y,anim_flags);
DEBUG_LEVEL1 fprintf(stderr,"src<%ld %ld> min<%ld %ld> pic<%ld %ld> im<%ld %ld>\n",
	src_x, src_y, minx,  miny, pic_x, pic_y,im_x,im_y);

          if (anim_flags & ANIM_LACE) 
	  { 
	    if ( (src_y==0) && (miny & 0x01) ) src_x += im_x;
	    else pic_y++;
	    pic_y >>= 1;
	    if (src_y != 0) src_y++;
	    src_y >>= 1;
	    miny++; miny >>= 1;
	  }

DEBUG_LEVEL1 fprintf(stderr,"src<%ld %ld> min<%ld %ld> pic<%ld %ld> im<%ld %ld>\n\n",
	src_x, src_y, minx,  miny, pic_x, pic_y,im_x,im_y);
          XPutImage(theDisp,mainW,theGC,theImage,
                        src_x, src_y, minx,  miny, pic_x, pic_y);
          XSync(theDisp,False);
          pic = im_buff0; im_buff0 = im_buff1; im_buff1 = pic;
        }
        screen_updated = TRUE;
        break;

       /* 
        *  Update ham_map if it changes during an animation. ham_map is
        *  needed in order to convert HAM images into displayable images.
        */
     case ACT_IFF_HMAP: 
        {
          ColorReg *hptr;
          int i;

          DEBUG_LEVEL2 fprintf(stderr,"ACT_IFF_HMAP:\n");
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
 t_frame_end = times(&tms3) - t_frame_start;
 t_frame_end *= 1000/HZ;  /* convert to milliseconds */

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

/*
 *
 */
void
Step_Action_Next()
{
 /* Move to next frame
  */
 cur_frame++;

 /* Are we at the end of an anim?
  */
 if (   (cur_file->frame_lst[cur_frame] < 0)
     || (cur_file->frame_lst[cur_frame] >= action_cnt) )
 {
   cur_frame = cur_file->loop_frame;

   cur_floop++;
   DEBUG_LEVEL1 fprintf(stderr,"  loop = %ld\n",cur_floop);

   /* Done looping animation. Move on to next file if present
    */
   if (cur_floop >= cur_file->loop_num)
   {
     cur_floop = 0;             /* Reset Loop Count */

     /* This is a special case check.
      * If more that one file, reset file_is_started, otherwise
      * if we're only displaying 1 animation jump to the loop_frame
      * which has already been set up above.
      */
     if (first_file->next_file != first_file )
     {
       file_is_started = FALSE;
       cur_frame = 0;
     } /* end of move on to next file/anim */

     cur_file = cur_file->next_file;

     DEBUG_LEVEL1
       fprintf(stderr,"  file = %ld\n",cur_file->file_num);
   } /* end done looping file */
 } /* end done with frames in file */
}

/*
 *
 */
void
Step_Frame_Next()
{
 /* Move to next frame
  */
 cur_frame++;

 /* Are we at the end of an anim?
  */
 if (   (cur_file->frame_lst[cur_frame] < 0)
     || (cur_file->frame_lst[cur_frame] >= action_cnt) )
 {
   cur_frame = cur_file->loop_frame;
 }
}

/*
 *
 */
void
Step_Frame_Prev()
{
 /* Move to next frame
  */
 cur_frame--;

 /* Are we at the beginning of an anim?
  */
 if (   (cur_file->frame_lst[cur_frame] < 0)
     || (cur_file->frame_lst[cur_frame] >= action_cnt) 
     || (cur_frame < cur_file->loop_frame)
     || (cur_frame < 0)
    )
 {
   cur_frame = cur_file->last_frame;
 }
}

/*
 *
 */
void
Step_File_Next()
{
  file_is_started = FALSE;
  cur_frame = 0;
  cur_floop = 0; /* used if things start up again */
  cur_file = cur_file->next_file;

  if (debug) fprintf(stderr,"  file = %ld\n",cur_file);
}

/*
 *
 */
void
Step_File_Prev()
{
  file_is_started = FALSE;
  cur_frame = 0;
  cur_floop = 0; /* used if things start up again */
  cur_file = cur_file->prev_file;

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
 if ( Is_FLI_File(filename) ) return(FLI_ANIM); 
 if ( Is_DL_File(filename)  ) return(DL_ANIM);
/* Temporary
 if ( Is_JFIF_File(filename)  ) return(JPG_ANIM);
*/
 return(UNKNOWN_ANIM);
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
   * but should be worked into crng chunk up front
   */
  for (i = clow; i <= chi; i++) defs[i].pixel = x11_cmap_size - imagec + i;

  if ( (x11_display_type == PSEUDO_COLOR) &&
       (x11_cmap_flag    == TRUE) )
  {
    XStoreColors(theDisp,theCmap,&defs[clow],(chi - clow + 1) );
    XSync(theDisp,False);
  }
}

void
Free_Actions()
{
  ACTION *act;
  ULONG i;

  if (action_cnt <= 0) return;

  for(i=0;i<action_cnt;i++)
  {
    act = &action[i];

    switch(act->type)
    {
      case ACT_IMAGE:
        {
          ACT_IMAGE_HDR *act_im_hdr;
          act_im_hdr = (ACT_IMAGE_HDR *)(act->data);
          XDestroyImage(act_im_hdr->image);
          free(act->data);
        }
        break;
      case ACT_CLIP:
        {
          ACT_CLIP_HDR *act_cp_hdr;
          act_cp_hdr = (ACT_CLIP_HDR *)(act->data);
          free(act_cp_hdr->clip_ptr);
          XDestroyImage(act_cp_hdr->image);
          free(act->data);
        }
        break;
      case ACT_PIXMAP:
        {
          ACT_PIXMAP_HDR *act_pm_hdr;
          act_pm_hdr = (ACT_PIXMAP_HDR *)(act->data);
          XFreePixmap(theDisp,act_pm_hdr->pixmap);
          free(act->data);
          DEBUG_LEVEL1 fprintf(stderr,"  freed ACT_PIXMAP\n");
        }
        break;
      default:
        if (act->data != 0) free(act->data);
        break;
    }
  }
}

ANIM_HDR *Get_Anim_Hdr(file_hdr)
ANIM_HDR *file_hdr;
{
  ANIM_HDR *temp_hdr;
  static int file_num;

  temp_hdr = (ANIM_HDR *)malloc( sizeof(ANIM_HDR) );
  if (temp_hdr == 0) TheEnd1("Get_Anim_Hdr: malloc failed\n");

  if (first_file == 0) first_file = temp_hdr;
  if (file_hdr == 0)
  {
    file_num = 0;
    temp_hdr->next_file = temp_hdr;
    temp_hdr->prev_file = temp_hdr;
  }
  else
  {
    temp_hdr->prev_file = file_hdr;
    temp_hdr->next_file = file_hdr->next_file;
    file_hdr->next_file = temp_hdr;
  }

  temp_hdr->anim_type = 0;
  temp_hdr->imagex = 0;
  temp_hdr->imagey = 0;
  temp_hdr->imagec = 0;
  temp_hdr->imaged = 0;
  temp_hdr->imaged = 0;
  temp_hdr->anim_flags = 0;
  temp_hdr->loop_num   = 0;
  temp_hdr->loop_frame = 0;
  temp_hdr->frame_lst  = 0;
  temp_hdr->file_num = file_num;
  file_num++;

  return(temp_hdr);
}
  
