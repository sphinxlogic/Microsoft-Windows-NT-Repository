/*
 * XIMQ - display NASA/PDS "IMQ" files on a X11 workstation
 *
 * Copyright © 1991, 1994 Digital Equipment Corporation
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION.
 *
 * DIGITAL EQUIPMENT CORPORATION SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND DIGITAL EQUIPMENT CORPORATION HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Module:
 *
 *	XIMQ_IMAGE_FINDER.C
 *
 * Author:
 *
 *    Frederick G. Kleinsorge, November 1991
 *
 * Environment:
 *
 *    OpenVMS, VAX-C syntax
 *
 * Abstract:
 *
 *	Image finder logic for XIMQ.  Displays a menu window to allow the selection
 *	of the image.
 *
 */ 
#include "ximq.h"


/*
 *  Module local
 *
 */
static char default_fs_window_buffer[] = "NASA Image File Selection", /* FS Title */
       default_fs_icon_buffer[] = "NASA Image\nFile Selection";        /* FS  Title */
static char *fs_window_name = &default_fs_window_buffer;  /* Point to the default banner */
static char *fs_icon_name = &default_fs_icon_buffer;      /* And the icon banner */

static int
    fs_xs = 20,                                     /* FS display offset x */
    fs_ys = 40,                                     /*                   y */
    fs_th = 0,
    fs_tw = 0;

/*
 *  X11 stuff
 *
 */
static Window
	fs_text = 0,
	fs_scroll = 0,
	fs_ta = 0,
	fs_ba = 0,
	file_select_window = 0,
	mw8 = 0,
	fs_match = 0,
	fs_selected = 0;

static XFontStruct *font_info;

static GC fs_gc, fsR_gc, fsX_gc, fsw_gc, fsm_gc;

static Font file_font;

static long int
         f_use_P0[2] = {0x200, 0x200},              /* P0 address */
         volume_base[2],                            /* Section address return */
         index_base[2];                             /* Section address return */

static struct FAB index_fab;                        /* FAB for RMS open */
static struct FAB volume_fab;                       /* FAB for RMS open */


typedef struct _match_info {
  Window window_id;
  int max_size;
  int current_length;
  int insertion_point;
  unsigned char buffer[32];
  unsigned char name[64];
} MATCH_INFO;

#define SPACECRAFT   0
#define IMAGE_NO     1
#define IMAGE_ID     2
#define SHUTTER      3
#define INSTRUMENT   4
#define FILTER       5
#define TARGET       6
#define MW_MAX       7

MATCH_INFO match_data[] = {
  {0,  9, 0, 0, {0}, "Spacecraft:"},
  {0,  8, 0, 0, {0}, "Image #:"},
  {0, 10, 0, 0, {0}, "Image ID:"},
  {0,  7, 0, 0, {0}, "Shutter:"},
  {0, 19, 0, 0, {0}, "Instrument:"},
  {0,  7, 0, 0, {0}, "Filter:"},
  {0,  8, 0, 0, {0}, "Target:"},
  {0,  0, 0, 0, {0}, "END N/A"}
};

static unsigned char *input_filename;
static int input_filename_length;
static unsigned char *calib_filename;
static int calib_filename_length;

/*
 *  The index table needs to be scanned and filtered to find
 *  only the images that we are interested in and that are
 *  available... this array will point to the index records that
 *  are valid for the file
 *
 */
static char *index_records[4000];
static int  index_total    = 0;
static int  index_selected = 0;

static char *calib_records[2000];
static int  calib_total    = 0;
static int  calib_selected = 0;
static int  select_calib   = 0;

static int  index_displayed = 0;
static int  fs_index = 0;
static int  fs_index_count = 36;
static int  fs_y_height    = 0;
static int  fs_x_width     = 0;
static int  fs_ascent      = 0;
static int  fs_descent     = 0;
static int  selecting_file = 0;
static int  insert_mode    = 1;
static int  sb_siz = 0;
static int  sb_pos = 0;
static float sb_unit = 0;
static int scroll_y = 0;
static int scroll_in_progress = 0;
static int cursor_window = 0;
static int current_selection = -1;
static int selection_time = 0;

static int current_type;
static int current_file_pos[] = {0, 0, 0, 0, 0};

static char *current_label[] = {
	" ",
	"Image",
	"" };

static char *startup_message[] = {
	"  ",
	"cXIMQ - Display NASA Planetary Data System CD Images",
	"c ",
	"c©1991 Digital Equipment Corporation",
	"cAll Rights Reserved.",
	"  ",
	"cScanning the index table for all available images on the current volume.",
        "  ",
        "cThis may take a few seconds...",
	"  "};

static int startup_count = 10;
static char *startup_title = "XIMQ Startup Message";
static char *startup_icon  = "XIMQ Startup Window";
static char *startup_dismiss = " Dismiss ";

int
nocase_strcmp(unsigned char *str1, unsigned char *str2, int len)

{
  int tmp1, tmp2, i;

  for (i = 0; i < len; i += 1)
    {
      tmp1 = *str1++;
      if ((tmp1 > 64) || (tmp1 < 127)) tmp1 &= ~0x20;
      tmp2 = *str2++;
      if ((tmp2 > 64) || (tmp2 < 127)) tmp2 &= ~0x20;

      if (tmp1 != tmp2) return (len-i);

    }

  return (0);
}

void
clear_string(unsigned char *dest, int length)
{
  int i;

  for (i = length; i > 0; i -= 1)
    {
      *dest++ = 32;
    }
}

void
copy_string(unsigned char *source, unsigned char *dest, int length)
{
  int i;
  for (i = length;  i > 0; i -= 1)
    {
      if (*source == 0) return;
      *dest++ = *source++;
    }
}

/*
 *  process_volume_info()
 *
 *  description:
 *
 *	Reads the volume header file to find out the volume ID and other
 *	bits of data about the CD.
 *
 *  inputs:
 *
 *	The volume file must be open and the base address of the file
 *	loaded into volume_base[0] and end address in [1]
 *
 *  outputs:
 *
 *	A number of string variables will have the data from the volume
 *	file copied into their buffers.
 *
 */
int
process_volume_info()
{
  int length,remain,offset = 0,i,j;
  unsigned char *position;
  remain = length = volume_base[1] - volume_base[0];

  if (state.debugging)
    printf("process_volume_info\n");

  while (remain > 0)
    {

      offset   = position_stream(volume_base[0], offset, remain);

     /*
      *  Watch for EOF on positioning...
      *
      */
      if (offset == -1) return;

      remain   = length - offset;
      position = volume_base[0] + offset;

      if (strncmp(position,"VOLUME_SET_NAME",15) == 0)
	  parse_volume_string(position, remain, &txt.volume_set_name, 1);
      if (strncmp(position,"VOLUME_SET_ID",13) == 0)
	  parse_volume_string(position, remain, &txt.volume_set_id, 1);
      if (strncmp(position,"VOLUME_NAME",11) == 0)
	  parse_volume_string(position, remain, &txt.volume_name, 1);
      if (strncmp(position,"VOLUME_ID",9) == 0)
	  parse_volume_string(position, remain, &txt.volume_id, 1);
      if (strncmp(position,"VOLUME_NOTE",11) == 0)
	  parse_volume_string(position, remain, &txt.volume_note, 8);
      if (strncmp(position,"MISSION_NAME",12) == 0)
	  parse_volume_string(position, remain, &txt.mission_name, 1);
      if (strncmp(position,"MISSION_PHASE_NAME",18) == 0)
	  parse_volume_string(position, remain, &txt.mission_phase_name, 1);
      if (strncmp(position," DATA_SET_DESCRIPTION",20) == 0)
	  parse_volume_string(position, remain, &txt.data_set_description, 40);

     }

  return TRUE;
}

/*
 *  parse_volume_string()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
parse_volume_string(unsigned char *input_data, int max_length, unsigned char **result_array, int max_lines)
{
  unsigned char *result_data;

  union {
	  unsigned char  *c;
	  short *w;
	} scanit;

  int i = 0, dblq = 0, scanning = 1, looking = 1, j = 0, lines = 0;

 /*
  *  Make sure they are all null terminated...
  */
  for (i = 0; i < lines; i += 1)
    {
      result_data = *result_array++;
      *result_data = 0;
    }

 /*
  *  Get the first output line
  *
  */
  result_data = *result_array++;

 /*
  *  Point at input data.  The records are CR control.  However, we will
  *  span records if the CR is inside of a quote.
  *
  */
  scanit.c = input_data;

 /*
  *  Main loop, continue until the end of the string is reached...
  *
  */
  while (i < max_length)
    {
     /*
      *  We are parsing a string that looks like:
      *
      *  FOO                = BAR
      *
      *  and we want to find BAR, so we scan until we hit the "=" in the
      *  string
      *
      */
      if (scanning)
        {
          if (*scanit.c++ == 61)
	    scanning = 0;
	}
     /*
      *  If we are past the "="...
      *
      */
      else
	{
         /*
          *  We are looking for the start of the string.  We expect to see
          *  spaces - which we will ignore, double quotes - which start a
          *  string that we will not touch, a single quote, which starts a
          *  literal, or even a number...
          *
          */
	  if (looking)
	    {
	     /*
	      *  Ignore spaces
	      *
	      */
	      if (*scanit.c == 32)
		{
		  *scanit.c++;
		}
	     /*
	      *  If it's a double quote " then start the string...
	      *
	      */
	      else if (*scanit.c == '\"')
		{
		  dblq = 1;
		  looking = 0;
		  *scanit.c++;
		}
	     /*
	      *  If it's a single quote then ignore it
	      *
	      */
	      else if (*scanit.c == '\'')
		{
		  *scanit.c++;
		  looking = 0;
		}
	     /*
	      *  If it's a CR then end the string NOW with NO lines returned!
	      *
	      */
	      else if (*scanit.c == '\r')
		{
		  *result_data++ = 0;
		  return(FALSE);
		}
	     /*
	      *  If it's a LF ignore it and continue
	      *
	      */
	      else if (*scanit.c == '\n')
		{
		  *scanit.c++;
		}
	     /*
	      *  If it's any other character, back up a character, change
	      *  the mode and start the string.
	      *
	      */
	      else
		{
		  looking = 0;
		  i -= 1;
		}
            }
         /*
          *  We have started the string and are now checking for the start
          *  of a comment "/*", double or single quotes to terminate the
          *  string.  Otherwise we will simply copy the character into the
          *  output buffer.
          *
          */
	  else
	    {
	     /*
	      *  We have a word pointer to look at two characters to see if
	      *  a comment is starting.  The comment terminates the string
	      *  unless the string is quoted.
	      *
	      */
	      if ((*scanit.w == '/*') && (!dblq))
		{
		  *result_data++ = 0;
		  if (j) lines += 1;
		  return (lines);
		}
	     /*
	      *  A double quote character also terminates the string.
	      *
	      */
	      else if (*scanit.c == '\"')
		{
		  *result_data++ = 0;
		  if (j) lines += 1;
		  return (lines);
		}
	     /*
	      *  A single quote terminates the string unless it's inside
	      *  a quoted string.
	      *
	      */
	      else if ((*scanit.c == '\'') && (!dblq))
		{
		  *result_data++ = 0;
		  if (j) lines += 1;
		  return (lines);
		}
	     /*
	      *  If it's a LF ignore it
	      *
	      */
	      else if (*scanit.c == '\n')
		{
		  *scanit.c++;
		}
	     /*
	      *  If it's a CR, end the string unless we are in a dblq
	      *
	      */
	      else if (*scanit.c == '\r')
		{
		 /*
		  *  Inside a quoted string, use CR to cause a newline.
		  *
		  */
		  if (dblq)
		    {

		      *result_data++ = 0;

		      lines += 1;

		      if (lines < max_lines)
			{
			  j = 0;
			  result_data = *result_array++;
			  *scanit.c++;
			}
		      else return (lines);
		    }
		  else
		    {
		      *result_data++ = 0;
		      if (j) lines += 1;
		      return (lines);
		    }
		}
	     /*
	      *  If it's any other character, just copy it.
	      *
	      */
	      else
		{
	          j += 1;
		  *result_data++ = *scanit.c++;
		}
	    }
	}

      if (j >= TEXT_MAX)
	{
	  *result_data++ = 0;

	  lines += 1;

	  if (lines < max_lines)
            {
	      j = 0;
	      result_data = *result_array++;
            }
	  else return (lines);

	}
      i += 1;
    }

 /*
  *  Exhuasted maximum input data length...
  *
  */

  *result_data++ = 0;

  if (j) lines += 1;
  return (lines);
}

/*
 *  position_stream()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
position_stream(unsigned char *base, int offset, int remain)
{

  base += offset;

  while (remain > 0)
    {
      if (*base++ == '\n')
        {
	  return (offset+1);
        }
      offset   += 1;
      remain   -= 1;
    }

  return (-1);
}

/*
 *  create_file_select_window()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
create_file_select_window(int map_it)

{
  int status, i, j;
  float xform;
  XSetWindowAttributes xswa;
  XGCValues xgcv;
  XSizeHints xhint;
  XCharStruct overall;

  char title_buf[100];
  char icon_buf[100];

  char em[] = "N";
  int temp, wwidth, whight;

  if (state.debugging)
    printf("create_file_select_window\n");

  if (!state.use_file_select_window) return (FALSE);

 /*
  *  Make sure that the workstation is open
  *
  */
  if (!open_workstation()) return FALSE;

  if (!file_select_window)
    {
      font_info = XLoadQueryFont(state.display, FONT4);
      file_font = font_info->fid;

      XTextExtents(font_info, &em[0], 1,
		&temp, &fs_ascent, &fs_descent, &overall);

      if (state.debugging)
	{
	  printf("ascent %d, descent %d\n", fs_ascent, fs_descent);
	  printf("lbearing %d, rbearing %d\n",
		overall.lbearing, overall.rbearing);
	}

      fs_y_height = fs_ascent + fs_descent;
      fs_x_width  = overall.lbearing + overall.rbearing;

      fs_tw = fs_x_width  * 162;
      fs_th = fs_y_height * (fs_index_count);

      wwidth = (fs_x_width  * 162) + 16 + 3;
      whight = (fs_y_height * (fs_index_count + 10)) + 4;

      xswa.background_pixel = state.default_pixel;
      xswa.border_pixel     = state.white_pixel;

      xswa.event_mask = ExposureMask | ButtonMotionMask | ButtonPressMask |
			ButtonReleaseMask | KeyReleaseMask | KeyPressMask |
			PointerMotionMask |
			LeaveWindowMask | FocusChangeMask | PropertyChangeMask;

      FindBestWindowPosition( state.display, 0,
                              wwidth, whight, 0, 0,
                              &fs_xs, &fs_ys, 0, 0);

     if (state.debugging)
       printf("best position for file selection window is X = %d, Y = %d\n", fs_xs, fs_ys);

      file_select_window = Add_Window(state.display, state.root,
				fs_xs, fs_ys,
				wwidth, whight,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_file_select_window);

      fs_text = Add_Window(state.display, file_select_window,
				0, fs_y_height,
				fs_tw, fs_th,
				1, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_fs_text_window);

      fs_scroll = Add_Window(state.display, file_select_window,
				fs_tw, fs_y_height,
				16, fs_th,
				1, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_fs_scroll_window);

      fs_ta = Add_Window(state.display, fs_scroll,
				0, 0,
				16, 16,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_fs_ta_window);

      fs_ba = Add_Window(state.display, fs_scroll,
				0, fs_th-16,
				16, 16,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_fs_ba_window);

      fs_match = Add_Window(state.display, file_select_window,
				0, fs_y_height * (fs_index_count + 3),
				wwidth, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_fs_match_window);

     /*
      *  Spacecraft
      *
      */
      match_data[SPACECRAFT].window_id =
                     Add_Window(state.display, fs_match,
				0, 0,
				21*fs_x_width, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_mw_window);

     /*
      *  Target
      *
      */
      match_data[TARGET].window_id =
                     Add_Window(state.display, fs_match,
				21*fs_x_width, 0,
				20*fs_x_width, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_mw_window);

     /*
      *  Image num
      *
      */
      match_data[IMAGE_NO].window_id =
                     Add_Window(state.display, fs_match,
				(21+20)*fs_x_width, 0,
				20*fs_x_width, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_mw_window);
     /*
      *  Image ID
      *
      */
      match_data[IMAGE_ID].window_id =
                     Add_Window(state.display, fs_match,
				(21+20+20)*fs_x_width, 0,
				20*fs_x_width, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_mw_window);

     /*
      *  Instrument
      *
      */
      match_data[INSTRUMENT].window_id =
                     Add_Window(state.display, fs_match,
				(21+20+20+20)*fs_x_width, 0,
				32*fs_x_width, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_mw_window);

     /*
      *  Shutter
      *
      */
      match_data[SHUTTER].window_id =
                     Add_Window(state.display, fs_match,
				(21+20+20+20+32)*fs_x_width, 0,
				20*fs_x_width, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_mw_window);

     /*
      *  Filter
      *
      */
      match_data[FILTER].window_id =
                     Add_Window(state.display, fs_match,
				(21+20+20+20+32+20)*fs_x_width, 0,
				20*fs_x_width, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_mw_window);
     /*
      *  Match button
      *
      */
      mw8 = Add_Window(state.display, fs_match,
				(21+20+20+20+32+20+20)*fs_x_width, 0,
				9*fs_x_width, fs_y_height,
				0, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_mw8_window);

      fs_selected = Add_Window(state.display, file_select_window,
				0, fs_y_height * (fs_index_count + 4),
				wwidth-2, fs_y_height*3,
				1, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_fs_selected_window);

      if (state.volume_file_open)
        {
          sprintf(&title_buf[0],"Image Select - %s: %s",
		txt.mission_name, txt.mission_phase_name);

          fs_window_name = &title_buf;

          sprintf(&icon_buf[0],"Image Select\n%s",txt.mission_name);
          fs_icon_name = &icon_buf[0];
        }

     /*
      *  Set the window manager stuff up.
      *
      */
      xhint.flags  = 0;
      xhint.x      = fs_xs;
      xhint.y      = fs_ys;
      xhint.width  = wwidth;
      xhint.height = whight;
      XSetStandardProperties(state.display, file_select_window,
				fs_window_name, fs_icon_name,
				None, 0, 0, &xhint);

      xgcv.foreground = state.foreground_pixel;
      xgcv.background = state.default_pixel;
      xgcv.function   = GXcopy;

      fs_gc = XCreateGC(state.display, fs_text, 
                    GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, fs_gc, file_font);

      fsw_gc = XCreateGC(state.display, file_select_window,
                    GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, fsw_gc, file_font);

      fsm_gc = XCreateGC(state.display, fs_match,
                    GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, fsm_gc, file_font);

      xgcv.background = state.foreground_pixel;
      xgcv.foreground = state.default_pixel;
      fsR_gc = XCreateGC(state.display, mw8, 
                    GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, fsR_gc, file_font);

      xgcv.background = state.default_pixel;
      xgcv.foreground = state.foreground_pixel ^ state.default_pixel;
      xgcv.function   = GXxor;
      fsX_gc = XCreateGC(state.display, fs_text,
                    GCFunction | GCForeground | GCBackground, &xgcv);
    }

  if (map_it)
    {
      XMapWindow(state.display, file_select_window);
    }

  state.file_select_window_mapped = TRUE;

 /*
  *  Install the colormap
  *
  */
  if (state.need_colormap)
    XInstallColormap(state.display, state.colormap);

  XFlush(state.display);
  XSync(state.display, 0);

  return(TRUE);
}

int
display_file_text(int from, int min, int max)
{
  GC lcl_gc;
  unsigned char *irec;
  int i, j, y, y2;

  static unsigned char output_line[512];

  if (state.debugging)
    printf("display_file_text\n");

  y = (from * fs_y_height) + fs_ascent;

  for (i = min; i < (min+max); i += 1)
    {

      if (current_selection == i)
        {
	  lcl_gc = fsR_gc;
        }
      else
        {
	  lcl_gc = fs_gc;
        }

      irec = index_records[i];

      clear_string(&output_line[0], 200);

      copy_string(irec+1,   &output_line[0],   9);
      copy_string(irec+33,  &output_line[11],  8);
      copy_string(irec+56,  &output_line[21],  8);
      copy_string(irec+44,  &output_line[31], 10);
      copy_string(irec+112, &output_line[43], 19);
      copy_string(irec+144, &output_line[64],  7);
      copy_string(irec+174, &output_line[73],  7);
      copy_string(irec+197, &output_line[82], 80);

      XDrawImageString(state.display, fs_text, lcl_gc,
		1, y, &output_line[0], 162);

      y += fs_y_height;

    }

  XFlush(state.display);

}

int
display_file_selected()
{
  unsigned char *irec;
  int y;

  static unsigned char output_line[512];

  if (state.debugging)
    printf("display_file_selected\n");

  y = 3*fs_ascent;

  XDrawImageString(state.display, fs_selected, fs_gc,
	1, fs_ascent, "Current Selection:  ", 19);

  irec = current_file_pos[0];

  if (irec != 0)
    {
      clear_string(&output_line[0], 200);

      copy_string(irec+1,   &output_line[0],   9);
      copy_string(irec+33,  &output_line[11],  8);
      copy_string(irec+56,  &output_line[21],  8);
      copy_string(irec+44,  &output_line[31], 10);
      copy_string(irec+112, &output_line[43], 19);
      copy_string(irec+144, &output_line[64],  7);
      copy_string(irec+174, &output_line[73],  7);
      copy_string(irec+197, &output_line[82], 80);

      XDrawImageString(state.display, fs_selected, fsR_gc,
		1, y, &output_line[0], 162);

    }
  else
    {
      XDrawImageString(state.display, fs_selected, fsR_gc,
		1, y, "*** NONE ***    ", 12);
    }

  XFlush(state.display);

}

int
display_match_window(int match_id, GC lcl_gc)
{
  unsigned char workb[80];
  int i, y, ll, temp;
  XCharStruct overall;

  if (state.debugging)
    printf("display_match_window\n");

  y = fs_ascent;

  sprintf(workb, "%s %-20.*s",
    match_data[match_id].name, match_data[match_id].current_length, match_data[match_id].buffer);
  ll = strlen(workb);
  XDrawImageString(state.display, match_data[match_id].window_id, lcl_gc, 1, y, workb, ll);

  XFlush(state.display);
}

/*
 *  display_select_file_window()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
display_select_file_window()
{
  static unsigned char workb[256];
  unsigned char *irec;
  int i, y;

  if (state.debugging)
    printf("display_select_file_window\n");

  XClearWindow(state.display, file_select_window);

  clear_string(&workb[0], 200);
  copy_string("Spacecraft", &workb[0],  10);
  copy_string("Target",     &workb[11],  6);
  copy_string("Image #",    &workb[21],  7);
  copy_string("Image ID",   &workb[31],  8);
  copy_string("Instrument", &workb[43], 10);
  copy_string("Shutter",    &workb[64],  7);
  copy_string("Filter",     &workb[73],  6);
  copy_string("Image Note", &workb[82], 10);

  XDrawImageString(state.display, file_select_window, fs_gc,
		1, fs_ascent, &workb[0], 153);

  y = ((fs_y_height * fs_index_count+2)+1)+fs_ascent;

  sprintf(workb, "Current Filter Criteria:");
  XDrawImageString(state.display, file_select_window, fs_gc,
		1, y, workb, strlen(workb));

  y = (fs_y_height * (fs_index_count + 8))+fs_ascent;

  sprintf(workb, "Volume: %1.8s", txt.volume_id);
  XDrawImageString(state.display, file_select_window, fs_gc,
		1, y, workb, strlen(workb));

  sprintf(workb, "Total Files in Index: %d", index_total);
  XDrawImageString(state.display, file_select_window, fs_gc,
		(21*fs_x_width), y, workb, strlen(workb));

  sprintf(workb, "Total Files Selected: %d", index_selected);
  XDrawImageString(state.display, file_select_window, fs_gc,
		(80*fs_x_width), y, workb, strlen(workb));

  XFlush(state.display);
}

int
display_top_arrow()
{
  XPoint points[3];

  points[0].x = 7;
  points[0].y = 1;
  points[1].x = 1;
  points[1].y = 14;
  points[2].x = 14;
  points[2].y = 14;

  XFillPolygon(state.display, fs_ta, fsw_gc, &points, 3, Nonconvex, CoordModeOrigin);
  XFlush(state.display);

}

int
display_bottom_arrow()
{

  XPoint points[3];

  points[0].x = 7;
  points[0].y = 14;
  points[1].x = 1;
  points[1].y = 1;
  points[2].x = 14;
  points[2].y = 1;

  XFillPolygon(state.display, fs_ba, fsw_gc, &points, 3, Nonconvex, CoordModeOrigin);
  XFlush(state.display);

}

int
display_scroll_window()
{
  int j;

  XClearWindow(state.display, fs_scroll);

  if ((index_selected > 0) && (index_displayed > 0))
    {

      if (index_selected == index_displayed)
        {
          sb_unit = (fs_th - (2*16));
	  sb_siz  = (fs_th - (2*16));
	  sb_pos  = 16;
	}
      else
	{
	  sb_unit  = (fs_th - (2*16)) / (index_selected * 1.0);
	  sb_siz   = (index_displayed * sb_unit) + 0.5;
	  sb_pos   = (fs_index * sb_unit) + 0.5;
	  sb_pos  += 16;
	}

      if (state.debugging)
	printf("Images per pixel in scroll area %f, height of thingy %d\n",
		sb_unit, sb_siz);

      if (sb_siz < 1) sb_siz = 1;

      XFillRectangle(state.display, fs_scroll, fsw_gc, 1, sb_pos, 14, sb_siz);

    }

  XFlush(state.display);

}

int
scroll_text_window(int dest)
{
  int x1, y1, x2, y2, sh, sw, c = 0, j, k, l, i;

  if (!index_selected) return (0);
  if (index_selected < fs_index_count) return (0);

  if (dest < 0)
    dest = 0;
  else if (dest > (index_selected-fs_index_count))
    dest = index_selected - fs_index_count;

  if (fs_index == dest) return (0);

  if (fs_index > dest)
    {
     /*
      *  The current top line is greater than the
      *  new top line, we scroll DOWN and backwards
      *  in the array...
      *
      */

     /*
      *  Number of lines requested...
      *
      */
      j = fs_index - dest;

      fs_index -= j;
      display_file_text(0, fs_index, index_displayed);
      display_scroll_window();
    }
  else
    {
     /*
      *  The current top line is less than the new top
      *  line, we scroll UP and FORWARD in the array...
      *
      */

     /*
      *  Number of lines requested...
      *
      */
      j = dest - fs_index;

      fs_index += j;
      display_file_text(0, fs_index, index_displayed);
      display_scroll_window();
    }  
}

/*
 *  get_image_file()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
get_image_file(unsigned char *filename, int maxlength,
               int *file_pos, int *type,
               int calib, unsigned char *calibname)
{
  if (state.debugging)
    printf("get_image_file\n");

  input_filename = filename;
  input_filename_length = maxlength;
  *input_filename = 0;

  current_type  = *type;

  calib_filename = calibname;
  calib_filename_length = maxlength;
  *calib_filename = 0;

  select_calib = calib;
  
 /*
  *  Create the file selection window
  *
  */
  create_file_select_window(TRUE);

 /*
  *  Now just spin until the filename is selected
  *
  */
  selecting_file = 1;

  while (selecting_file)
    {
      XNextEvent(state.display, state.event);
      handle_events(1, FALSE);
    }

  *file_pos = current_file_pos[current_type];

  return (TRUE);
}

/*
 *  hide_file_select_file()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
hide_file_select_window()
{

  if (state.debugging)
    printf("hide_file_select_window\n");

  if (state.file_select_window_mapped)
    XUnmapWindow(state.display, file_select_window);

  state.file_select_window_mapped = FALSE;

  return(TRUE);

}

int
draw_cursor(int mw)
{
  int x, y1 = 0, y2, temp;
  XCharStruct overall;

  if (state.debugging)
    printf("draw_cursor\n");

  y2 = fs_ascent + fs_descent;

  XTextExtents(font_info, match_data[mw].name, strlen(match_data[mw].name),
		&temp, &temp, &temp, &overall);

  x = overall.width;

  if (match_data[mw].insertion_point > 0)
    {
      XTextExtents(font_info, match_data[mw].buffer,
		   match_data[mw].insertion_point+1,
		&temp, &temp, &temp, &overall);

      x += overall.width;
    }

  XDrawLine(state.display, match_data[mw].window_id, fs_gc, x, y1, x, y2);

  XFlush(state.display);
}

int
insert_character(XEvent *event, int match_id)
{
  static KeySym sym = 0;
  static XComposeStatus compose;

  unsigned char *from, *to;
  int i, j, k;

  int length;
  unsigned char data[40];

  length = XLookupString(event, &data[0], 40, &sym, &compose);

  if (state.debugging)
    printf("insert_character, window %d\n", match_data[match_id].window_id);

  if (length == 0)
    {
      switch (sym)
        {
          case XK_Right:

            if (match_data[match_id].insertion_point <
                match_data[match_id].current_length)
              match_data[match_id].insertion_point += 1;

            break;

          case XK_Left:

            if (match_data[match_id].insertion_point > 0)
              match_data[match_id].insertion_point -= 1;

            break;
        }
    }

  if (insert_mode)
    {
      if (state.debugging)
	{
          printf("insert mode: old string, insert @%d, length %d, <%-*.*s>\n",
		match_data[match_id].insertion_point, match_data[match_id].current_length,
                match_data[match_id].current_length, match_data[match_id].max_size, match_data[match_id].buffer);
          printf("input data string <%-*.*s> 0x%X\n", length, length,
		  &data[0], sym);
	}

      for (j = 0; j < length; j += 1)
	{
	  if ((data[j] > 32) && (data[j] < 126))
	    {
              if ((match_data[match_id].insertion_point >= match_data[match_id].max_size) ||
                  (match_data[match_id].current_length  >= match_data[match_id].max_size))
		{
	          XBell(state.display, 0);
		} /* End at EOF */
	      else
		{
		  if (match_data[match_id].current_length >
                      match_data[match_id].insertion_point)
		    {
	
                      if (state.debugging)
                        printf("Need to insert string\n");

		      from = match_data[match_id].buffer +
                             (match_data[match_id].current_length-1);

		      to   = match_data[match_id].buffer +
                             (match_data[match_id].current_length);

		      for (k = 0;
                           k < (match_data[match_id].current_length -
                                match_data[match_id].insertion_point);
                           k += 1)
                        {
                          *to-- = *from--;
                        }
		    }
		  to   = match_data[match_id].buffer + match_data[match_id].insertion_point;
		  *to++ = data[j];
		  match_data[match_id].insertion_point += length;
                  match_data[match_id].current_length  += length;
		} /* End character insertion */
	    } /* End non-control character */
	  else
	    {
	     /*
	      * Handle controls: 
	      *  
	      *  ^A   (1)	= Toggle insert and overstrike
	      *  CR   (13)	= Carrage return, cause scan to occur
	      *  ^Z   (26)	= Same as CR
	      *  DEL  (127)	= Delete the previous character
	      *  ^H   (8)	= Backspace
	      *  
	      */
	      switch (data[j])
		{
		  case 1:
		  case 8:

		    break;

		  case 13:
		  case 26:

		    scan_and_display();
		    break;

		  case 127:

		    if (match_data[match_id].insertion_point > 0)
		      {
			i    = match_data[match_id].insertion_point;
			from = match_data[match_id].buffer + match_data[match_id].insertion_point;
			match_data[match_id].insertion_point -= 1;
			to   = match_data[match_id].buffer + match_data[match_id].insertion_point;

			for (k = 0; k < match_data[match_id].current_length - i; k += 1)
			  *to++ = *from++;

			*to++ = 0;

			match_data[match_id].current_length -= 1;
		      }
		    break;

		  default:
		    if (state.debugging)
		      printf("Control %d\n", data[j]);

		} /* End case */
	    } /* End control character */
	} /* End loop for each character */
    } /* End insert mode */
  else
    {
	/* Overstrike mode */
    }

  if (length == 0)
    {
     /*
      *  Check the keysym
      *
      */
      switch (sym)
	{

	  default:
	    break;

	}
    }

  if (state.debugging)
    printf("new string, insert @%d, length %d, <%-*.*s>\n",
		match_data[match_id].insertion_point, match_data[match_id].current_length,
                match_data[match_id].current_length, match_data[match_id].max_size, match_data[match_id].buffer);

  display_match_window(match_id, fs_gc);

}

/*
 *  open_volume_descriptor()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
open_volume_descriptor()
{

  int status;

  if (state.debugging)
    printf("open_volume_descriptor\n");

 /*
  *  This routine opens and reads into memory the volume information
  *  file that contains (among other things) the volume ID.
  *
  */

  volume_fab = cc$rms_fab;
  volume_fab.fab$l_fop = FAB$M_UFO;
  volume_fab.fab$b_fac = FAB$M_GET;
  volume_fab.fab$b_rtv = 0xFF;
  volume_fab.fab$l_fna = state.volume_filename;
  volume_fab.fab$b_fns = strlen(state.volume_filename);
  volume_fab.fab$l_dna = VOLUME_DEFAULT;
  volume_fab.fab$b_dns = sizeof (VOLUME_DEFAULT - 1);

  status = sys$open( &volume_fab);

  if (status != RMS$_NORMAL)
    {
      printf("The volume information file %s could not be opened %d\n", state.volume_filename, status);
      return (FALSE);
    }

  state.volume_file_open = 1;

  if (state.debugging)
    {
      printf("Volume Information File is open\n");
    }


 /*
  *  Map the input file as a private section in P0 space, let VMS figure out
  *  where.
  *
  */
  status = sys$crmpsc( &f_use_P0,                   /* inadr */
                       &volume_base,                /* retadr */
                       0,                           /* acmode */
                       SEC$M_EXPREG | SEC$M_CRF,    /* flags */
                       0,                           /* gsdnam */
                       0,                           /* ident */
                       0,                           /* relpag */
                       volume_fab.fab$l_stv,        /* chan */
                       0,                           /* pagcnt */
                       0,                           /* vbn */
                       0xEEEE,                      /* prot */
                       0);                          /* pfc */

  if (!(status & 1))
    {
      printf("Could not map the volume info, error %d\n",status);
      sys$close( &volume_fab);
      state.volume_file_open = 0;
      return (FALSE);
    }

  if (state.debugging)
    {
      printf("Volume Information mapped, start %d, end %d\n", volume_base[0], volume_base[1]);
    }

  process_volume_info();

  return (TRUE);
}

/*
 *  open_index_file()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
open_index_file()
{

  int status;

  if (state.debugging)
    printf("open_index_file\n");


 /*
  *  This routine opens and reads into memory the index table for the
  *  CD.  This table is used to find files to present to the user...
  *
  *
  *                IMAGE INDEX FLAT TABLE CONTENTS
  *
  * Field  Terse Name  Full Name            Type        Start  Width   Dec
  * -----  ----------- -------------------  ----------  -----  -----   ---
  *     1  SCNAME      SPACECRAFT_NAME      Character       2      9
  *     2  MSNPHSNM    MISSION_PHASE_NAME   Character      14     17
  *     3  TARGETNAME  TARGET_NAME          Character      34      8
  *     4  IMAGEID     IMAGE_ID             Character      45     10
  *     5  IMAGENUM    IMAGE_NUMBER         Numeric        57      8     2
  *     6  IMAGETIME   IMAGE_TIME           Character      67     20
  *     7  EARTHRCDTM  EARTH_RECEIVED_TIME  Character      90     20
  *     8  INSTRNAME   INSTRUMENT_NAME      Character     113     19
  *     9  SCANMODEID  SCAN_MODE_ID         Character     135      7
  *    10  SHUTMODEID  SHUTTER_MODE_ID      Character     145      7
  *    11  GAINMODEID  GAIN_MODE_ID         Character     155      7
  *    12  EDITMODEID  EDIT_MODE_ID         Character     165      7
  *    13  FILTERNAME  FILTER_NAME          Character     175      7
  *    14  FILTERNUM   FILTER_NUMBER        Numeric       184      4
  *    15  EXPOSUREDU  EXPOSURE_DURATION    Numeric       189      7     4
  *    16  NOTE        NOTE                 Character     198     80
  *    17  SMPLBITMSK  SAMPLE_BIT_MASK      Character     281      8
  *    18  DATAANMTYP  DATA_ANOMALY_TYPE    Character     292      6
  *    19  IMAGEVOLID  IMAGE_VOLUME_ID      Character     301      8
  *    20  IMAGEFILNM  IMAGE_FILE_NAME      Character     312     31
  *    21  BROWSVOLID  BROWSE_VOLUME_ID     Character     346      8
  *    22  BROWSFILNM  BROWSE_FILE_NAME     Character     357     38
  *
  *  Now, open the index file...
  *
  */

  index_fab = cc$rms_fab;
  index_fab.fab$l_fop = FAB$M_UFO;
  index_fab.fab$b_fac = FAB$M_GET;
  index_fab.fab$b_rtv = 0xFF;
  index_fab.fab$l_fna = state.index_filename;
  index_fab.fab$b_fns = strlen(state.index_filename);
  index_fab.fab$l_dna = INDEX_DEFAULT;
  index_fab.fab$b_dns = sizeof (INDEX_DEFAULT - 1);

  status = sys$open( &index_fab);

  if (status != RMS$_NORMAL)
    {
      printf("The image index file %s could not be opened %d\n", state.index_filename, status);
      return (FALSE);
    }

  state.index_file_open = 1;

  if (state.debugging)
    {
      printf("Index File is open\n");
    }


 /*
  *  Map the input file as a private section in P0 space, let VMS figure out
  *  where.
  *
  */
  status = sys$crmpsc( &f_use_P0,                   /* inadr */
                       &index_base,                 /* retadr */
                       0,                           /* acmode */
                       SEC$M_EXPREG | SEC$M_CRF,    /* flags */
                       0,                           /* gsdnam */
                       0,                           /* ident */
                       0,                           /* relpag */
                       index_fab.fab$l_stv,         /* chan */
                       0,                           /* pagcnt */
                       0,                           /* vbn */
                       0xEEEE,                      /* prot */
                       0);                          /* pfc */

  if (!(status & 1))
    {
      printf("Could not map the index, error %d\n",status);
      sys$close( &index_fab);
      state.index_file_open = 0;
      return (FALSE);
    }

  if (state.debugging)
    {
      printf("Index mapped, start %d, end %d\n", index_base[0],index_base[1]);
    }

  display_message_window(&startup_message[0], startup_count,
			startup_title, startup_icon, startup_dismiss);

  scan_index();
  scan_calib();

  hide_message_window();

  return (TRUE);
}

int
scan_and_display()
{
  fs_index = 0;
  scan_index();
  if (select_calib) scan_calib();
  display_select_file_window();
  XClearWindow(state.display, fs_text);
  display_file_text(0, fs_index, index_displayed);
  display_scroll_window();
}

/*
 *  scan_index()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
scan_index()
{
  int i, j = 0, k = 0, m, rec = 1;
  unsigned char *index_file_data = index_base[0];

  if (state.debugging)
    printf("scan_index\n");

  for (i = 0; i < 10000; i += 1)
    {

      for (m = 7; m > 0; m -= 1)
	{
	  if (*(index_file_data+300+m) != 32) break;
	}

      if (nocase_strcmp(index_file_data+300,txt.volume_id,m+1) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+1,
		match_data[SPACECRAFT].buffer, match_data[SPACECRAFT].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+33,
		match_data[TARGET].buffer, match_data[TARGET].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+56,
		match_data[IMAGE_NO].buffer, match_data[IMAGE_NO].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+44,
		match_data[IMAGE_ID].buffer, match_data[IMAGE_ID].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+112,
		match_data[INSTRUMENT].buffer, match_data[INSTRUMENT].current_length)  != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+144,
		match_data[SHUTTER].buffer, match_data[SHUTTER].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+174,
		match_data[FILTER].buffer, match_data[FILTER].current_length) != 0) rec = 0;

      if (rec)
	{
	  index_records[j] = index_file_data;
	  j  += 1;
	}
      else rec = 1;

     /*
      *  Increment total record count and check to see if we have maxed
      *  out.
      *
      */
      k += 1;

      if (j > 4000)
	{
	  printf("Record max reached in index selection!  Truncating...\n");
	  break;
	}

     /*
      *  Next record
      *
      */
      index_file_data += 512;

     /*
      *  Check for EOF
      *
      */
      if (index_file_data >= index_base[1])
	break;

     /*
      * Every 8th time or so through the loop, check for an event...
      * The initial scan can take a while when reading from a CD.
      *
      */
      if (j & 8)
	{
	  int num_ev;
	  num_ev = XPending(state.display);
          if (num_ev)
	    {
	      XNextEvent(state.display, state.event);
              handle_events(1, FALSE);
	    }
        }
    }

  if (state.debugging)
    printf("Index scanned, %d out of %d records selected\n", j, k);

  index_total    = k;
  index_selected = j;

  if ((index_selected - fs_index) < fs_index_count)
    {
      index_displayed = index_selected - fs_index;
    }
  else 
    {
      index_displayed = fs_index_count;
    }

  current_selection = -1;

  return (j);
}

/*
 *  scan_calib()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
scan_calib()
{
  int i, j = 0, k = 0, m, rec = 1;
  unsigned char *index_file_data = index_base[0];

  if (state.debugging)
    printf("scan_calib\n");

  for (i = 0; i < 10000; i += 1)
    {

     /*
      *  Figure out how many non-space characters in the volume ID
      *
      */
      for (m = 7; m > 0; m -= 1)
	{
	  if (*(index_file_data+300+m) != 32) break;
	}

     /*
      *  Now, find any excuse not to add the record as a potential calibration
      *  image.  It must be a DARK current image.  And it must match all the
      *  other settings for the selected images.
      */
      if (nocase_strcmp(index_file_data+300,txt.volume_id,m+1) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+1,
		match_data[SPACECRAFT].buffer, match_data[SPACECRAFT].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+33,
		"DARK",4) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+56,
		match_data[IMAGE_NO].buffer, match_data[IMAGE_NO].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+44,
		match_data[IMAGE_ID].buffer, match_data[IMAGE_ID].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+112,
		match_data[INSTRUMENT].buffer, match_data[INSTRUMENT].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+144,
		match_data[SHUTTER].buffer, match_data[SHUTTER].current_length) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+174,
		match_data[FILTER].buffer, match_data[FILTER].current_length) != 0) rec = 0;

      if (rec)
	{
	  calib_records[j] = index_file_data;
	  j  += 1;
	}
      else rec = 1;

     /*
      *  Increment total record count and check to see if we have maxed
      *  out.
      *
      */
      k += 1;

      if (j > 2000)
	{
	  printf("Record max reached in calibration selection!  Truncating...\n");
	  break;
	}

     /*
      *  Next record
      *
      */
      index_file_data += 512;

     /*
      *  Check for EOF
      *
      */
      if (index_file_data >= index_base[1])
	break;

     /*
      * Every 8th time or so through the loop, check for an event...
      * The initial scan can take a while when reading from a CD.
      *
      */
      if (j & 8)
	{
	  int num_ev;
	  num_ev = XPending(state.display);
          if (num_ev)
	    {
	      XNextEvent(state.display, state.event);
              handle_events(1, FALSE);
	    }
        }
    }

  if (state.debugging)
    printf("Index scanned for calibration, %d out of %d records selected\n", j, k);

  calib_total    = k;
  calib_selected = j;

  return (j);
}

/*
 *  find_calibration_image()
 *
 *  description:
 *
 *	xxxxx
 *
 *  inputs:
 *
 *	xxxxx
 *
 *  outputs:
 *
 *	xxxxx
 *
 */
int
find_calibration_image()
{
  int i, j = 0, k = 0, m, rec = 1;
  unsigned char *index_file_data, *calib_file_data;

  if (state.debugging)
    printf("find_calibration_image\n");

  index_file_data = index_records[current_selection];

  for (i = 0; i < calib_selected; i += 1)
    {

      calib_file_data = calib_records[i];

     /*
      *  Match the spacecraft, instrument (camera) and the filter.
      *
      */
      if (nocase_strcmp(index_file_data+1, calib_file_data+1, 9) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+112, calib_file_data+112, 1) != 0) rec = 0;
      else if (nocase_strcmp(index_file_data+174, calib_file_data+174, 7) != 0) rec = 0;

      if (rec)
	{
	  if (calib_filename)
	    {
	      strncpy(calib_filename, calib_file_data+311, 31);
              *(calib_filename+31) = 0;
	    }
	  if (state.debugging)
	    printf("Using Calibration Image: %31.31s\n", calib_filename);

	  return (i);
	}
      else rec = 1;

     /*
      * Every 8th time or so through the loop, check for an event...
      * The initial scan can take a while when reading from a CD.
      *
      */
      if (j & 8)
	{
	  int num_ev;
	  num_ev = XPending(state.display);
          if (num_ev)
	    {
	      XNextEvent(state.display, state.event);
              handle_events(1, FALSE);
	    }
        }
    }

  return (0);
}

/*
 *   EVENT PROCESSING
 *
 */

void
event_on_fs_text_window(XEvent *event, int from_mainline)
{
  int y, i, j, k;
  unsigned char *index_file_data;

  if (event->type == Expose)
    {
      XClearWindow(state.display, fs_text);
      display_file_text(0, fs_index, index_displayed);
      return;
    }

  if (state.event->type == ButtonPress)
    {

      y = state.event->xbutton.y / fs_y_height;

      if (y < index_displayed)
        {
          i = fs_index+y;
          index_file_data = index_records[i];

          strncpy(input_filename, index_file_data+311, 31);
          *(input_filename+31) = 0;

          if (state.debugging)
            printf("index = %d, id = %10.10s, filename = %s\n",
			i, index_file_data+44, input_filename);

          if (current_selection == i)
            {
              if (state.debugging)
                printf("Timestamp on click: %d\n", state.event->xbutton.time);

              if ((state.event->xbutton.time - selection_time) < 250)
                {
                  current_file_pos[current_type] = index_file_data;
                  selecting_file = 0;

                  if (select_calib)
                    find_calibration_image(current_selection);

                }
            }
          else
            {
              k = current_selection;
              current_selection = i;

              if ((k >= 0) && (k < 4000))
                {
                  j = k - fs_index;
                  display_file_text(j, k, 1);
                }
            }

          selection_time = state.event->xbutton.time;

          display_file_text(y, i, 1);

        }
    }
}

void
event_on_file_select_window(XEvent *event, int from_mainline)
{
  if (event->type == Expose)
    {
      display_select_file_window();
      return;
    }
}

void
event_on_fs_scroll_window(XEvent *event, int from_mainline)
{
  XEvent peek_event;
  int y, i, event_count;

  if (event->type == Expose)
    {
      display_scroll_window();
      return;
    }

  switch (state.event->type)
    {

      case ButtonPress:

        if ((state.event->xbutton.y > (sb_pos-5)) &&
            (state.event->xbutton.y < (sb_pos+sb_siz+5)))
          {
            scroll_y = state.event->xbutton.y - 16;
            scroll_in_progress = 1;
          }
        break;

      case ButtonRelease:

        scroll_in_progress = 0;
        break;

      case MotionNotify:

        if (scroll_in_progress)
          {

            event_count = XEventsQueued(state.display, QueuedAfterFlush);

           /*
            *  Look at the next events, and find the last motion event, but
            *  terminate the search on any non-motion event.  This does a
            *  motion compression.
            *
            */
            while (event_count > 0)
              {
                XPeekEvent(state.display, &peek_event);

                if ((peek_event.xany.window == fs_scroll) &&
                    (peek_event.type == MotionNotify))
                  XNextEvent(state.display, state.event);
                else break;

                event_count = XEventsQueued(state.display, QueuedAfterFlush);
              }

            y = state.event->xmotion.y - 16;

            if (y < 0) break;
            if (y > (fs_th-32)) break;
            if (y == scroll_y) break;

            if (y > scroll_y)
              {
                i = ((y - scroll_y) / sb_unit);
                if (!i) break;
                scroll_text_window(fs_index+i);
                scroll_y = y;
              }
            else
              {
                i = ((scroll_y - y) / sb_unit);
                if (!i) break;
                scroll_text_window(fs_index-i);
                scroll_y = y;
              }
          }

        break;
    }
}

void
event_on_fs_ta_window(XEvent *event, int from_mainline)
{
  if (event->type == Expose)
    {
      display_top_arrow();
      return;
    }

  if (event->type == ButtonPress)
    scroll_text_window(fs_index-1);

}

void
event_on_fs_ba_window(XEvent *event, int from_mainline)
{
  if (event->type == Expose)
    {
      display_bottom_arrow();
      return;
    }

  if (event->type == ButtonPress)
    scroll_text_window(fs_index+1);

}

void
event_on_fs_match_window(XEvent *event, int from_mainline)
{
  if (event->type == Expose)
    {
      XDrawImageString(state.display, mw8, fsR_gc, 1, fs_ascent, " FILTER ", strlen(" FILTER "));
      return;
    }
}

void
event_on_mw_window(XEvent *event, int from_mainline)
{
  int mw;

  for (mw = 0; mw < MW_MAX; mw += 1)
    {
      if (match_data[mw].window_id == event->xany.window) break;      
    }

  if (mw == MW_MAX) return;

  if (event->type == Expose)
    {
      display_match_window(mw, fs_gc);
      return;
    }

  switch (event->type)
    {

      case KeyRelease:
      case KeyPress:

	if (event->type == KeyRelease)
	  {
	    if (state.debugging) printf("KeyRelease\n");
	    break;
	  }

        insert_character(event, mw);

	draw_cursor( mw);

	break;

      case ButtonPress:

	XUngrabPointer(state.display, CurrentTime);

	XSetInputFocus(state.display, event->xbutton.window,
			RevertToNone, CurrentTime);

        display_match_window(cursor_window, fs_gc);

        cursor_window = mw;

        draw_cursor( mw);

	break;

    }
}

void
event_on_mw8_window(XEvent *event, int from_mainline)
{

  if (event->type == Expose)
    {
      XDrawImageString(state.display, mw8, fsR_gc, 1, fs_ascent, " FILTER ", strlen(" FILTER "));
      return;
    }

  if (event->type == ButtonPress)
    {

      XUngrabPointer(state.display, CurrentTime);

      XSetInputFocus(state.display, event->xbutton.window,
			RevertToNone, CurrentTime);

      scan_and_display();

    }
}

void
event_on_fs_selected_window(XEvent *event, int from_mainline)
{
  if (event->type == Expose)
    {
      display_file_selected();
      return;
    }
}
