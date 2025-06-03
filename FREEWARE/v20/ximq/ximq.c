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
 *	XIMQ.C
 *
 * Author:
 *
 *    Frederick G. Kleinsorge, November 1991
 *
 * Environment:
 *
 *    OpenVMS AXP, VAX-C syntax
 *
 * Abstract:
 *
 *    Display Compressed NASA PDS images on a X11 workstation.  Main routine
 *    logic and entry point.
 *
 *    This application reads compressed image files from a NASA image CD
 *    and displays it on a X11 workstation.  It was written using the VAX
 *    workstation and takes between 1 and 15 second to display a greyscale
 *    image that has been read from disk, decompressed, equalized and pixel
 *    mapped.
 *
 * Modifications:
 *
 */ 
#include "ximq.h"

/*
 *  DEFINE the global state structure, fill in the defaults.
 *
 */
struct window_list my_windows = {
    0,						/* Last window offset */
    {0}						/* Array of WIN structs */
};

struct statedef state = {
    FALSE,					/* Is there a display? */
    FALSE,					/* Main display open */
    FALSE,					/* Print debug statements */
    0,						/* Number of planes */
    0,						/* Screen number */
    FALSE,					/* Need colormap */ 
    0,						/* Visual class */
    FALSE,					/* manual file select */
    TRUE,					/* use file select window */
    FALSE,					/* file select window mapped */
    TRUE,					/* use message window */
    FALSE,					/* message window mapped */
    TRUE,					/* .. the menu window */
    255,					/* white pixel */
    0,						/* black pixel */
    0,						/* default pixel */
    0,						/* foreground pixel  */
    0,						/* red   color pixel */
    0,						/* blue  color pixel */
    0,						/* green color pixel */
    DEF_COLORS,					/* Number of colors to use */
    DEF_DISPLAY_INC,				/* Number out per call... */
    DEF_SCANLINES,				/* Image scanlines */
    DEF_SAMPLES_PER_LINE,			/* and number of samples */
    0,						/* Colormap ID */
    {0},					/* Revectored pixels */
    {0},					/* Final Color */
    {0},					/* Lookup Table */

    0,						/* Display ID */
    0,						/* Window and Root IDs */
    0,						/* Visual ID */

    0, 						/* Xevent */
    0,						/* image window */
    0,						/* image GC */
    0,						/* small image gc */

    {0},					/* image buffer1 */
    {0},					/* zoom buffer1 */
    {0},					/* zoom buffer2 */

    {0},					/* original image buffer1 */
    {0},					/* original image buffer2 */

    {0},					/* current image */
    {0},					/* original image */
    {0},					/* zoom buffer */
    {0},					/* image buffer */
    0,						/* main image (XImage) */
    0,						/* small image (XImage) */

    INDEX_FILE,					/* index_filename */
    FALSE,					/* index_file_open */
    TRUE,					/* use_index */

    VOLUME_FILE,				/* volume_filename */
    FALSE,					/* volume_file_open */
    TRUE					/* use_volume */
};


/*
 *  Module specific variables.  
 *
 */
static struct lcldef {

  unsigned long plane_masks[8];			/* A place for some masks */
  unsigned long pixels[MAX_VALUES];		/* And pixels */
  int default_pixel_red;			/* default pixel definition */
  int default_pixel_green;
  int default_pixel_blue;
  int foreground_pixel_red;			/* foreground pixel definition */
  int foreground_pixel_green;
  int foreground_pixel_blue;
  int highlight_pixel_red;			/* highlight pixel definition */
  int highlight_pixel_green;
  int highlight_pixel_blue;
  int status_pixel_red;				/* status pixel definition */
  int status_pixel_green;
  int status_pixel_blue;
  int no_red_blue_guns;				/* Turn off Red & Blue */
  int cmap;					/* Map ID */
  int cmap_size;				/* Size of map */
  int cmap_setup;				/* Map has been set up */
  int cmap_priv;				/* Current colormap private */
  int pixel_twiddle;				/* Use pixel twiddling */ 
  int writeable_map;				/* Window attribute */ 
  int private_map;				/* Private colormap flag */
  int defmap;					/* Default map ID */
  int show_intensity_window;			/* Display the scale window */
  int show_small_image_window;			/* .. the 200x200 window */
  int show_text_window;				/* .. the text window */
  int show_image_histogram_window;		/* .. the histogram window */
  int show_pixel_intensity;			/* .. the pixel intensity */
  int force_shared_colors;			/* Force shared allocate */
  int black_limit;				/* Black from here down */
  int white_limit;				/* White from here up */
  int auto_equal;				/* Auto Hist Equalization */
  int black_fudge;				/* Black limit fudge */
  int white_fudge;				/* White limit fudge */
  int bright_shift;				/* Shift increment */
  int brightness_level;				/* No adjustment */
  int convolve_float;				/* Floating convolve? */
  int select_inverted;				/* The selection is highlighted */
  int current_select;				/* The current selection value */
  int redraw_image;				/* Need to redraw image */
  int processing_event;				/* Currently processing an event */
  int ihw_bright;				/* */
  int ihw_bright_init;				/* */
  int ihw_button;				/* */
  int highlight_width;				/* */
  int highlight_on;				/* */
  int highlight_enabled;			/* */
  int highlight_address;			/* */
  int highlight_red;				/* */
  int highlight_green;				/* */
  int highlight_blue;				/* */
  int zoom_upper_left[2];
  int zoom_lower_right[2];
  int zoom_displayed;
  int zoom_mode;
  int zoom_style;
  int zoom_rubber_band_on;
  int zoom_square_aspect;
  int app_running;
  int enable_gamma;
  int set_sync;					/* Run in Sync (debug) */
  char *display_name;				/* Workstation name    */
  int xs;					/* FB display offset x */
  int ys;					/*                   y */
  int ws;					/*            width    */
  int hs;					/*            height   */
  double gamma;
  double gamma_shift;
  float pix_per_count;
  float equalization_threshold;
  int calibrate_image_data;
  int select_offset;
  int select_max;

  FILEINFO *base_image_file;
  FILEINFO *calib_file;
  FILEINFO *current_file;

  Window main_window;
  Window image_hist_window;
  Window text_window;
  Window select_window;
  Window small_image_window;
  Window status_window;
  Window intensity_window;

  XColor highlight_band[HIGHLIGHT_MAX*2];	/* Highlight color array */
  XColor colors[MAX_VALUES];			/* Color update array */

  Font display_font;
  Font select_font;

  GC text_gc;
  GC status_gc;
  GC statusR_gc;
  GC select_gc;
  GC select_rubber_gc;
  GC hist_gc;
  GC zoom_rubber_gc;
  GC color_gc;

  XPoint zoom_rubber_box[5];			/* Points for zoom box */

  char *window_name;
  char *icon_name;

  int  (*gamma_routine)();
  void (*pixel_map_routine)();

} lcl = {

  {0,0,0,0,0,0,0,0},				/* A place for some masks */
  {0},						/* And pixels */
  0,30,124,					/* Default pixel */
  156,156,156,					/* Foreground pixel */
  224,77,77,					/* Highlight pixel */
  77,224,0,					/* Ready pixel */
  0,						/* Turn off Red & Blue */
  0,						/* Map ID */
  0,						/* Size of map */
  FALSE,					/* Map has been set up */
  FALSE,					/* Current colormap private */
  FALSE,					/* Twiddle the colormap */
  TRUE,						/* Window attribute */ 
  FALSE,					/* Force a private colormap */
  0,						/* Default map ID */
  TRUE,						/* Display the scale window */
  TRUE,						/* .. the 200x200 window */
  TRUE,						/* .. the text window */
  TRUE,						/* .. the histogram window */
  TRUE,						/* .. the pixel intensity */
  FALSE,					/* Force shared allocate */
  BLACK_LIMIT_DEFAULT,				/* Black from here down */
  WHITE_LIMIT_DEFAULT,				/* White from here up */
  AUTO_EQUAL_DEFAULT,				/* Auto Hist Equalization */
  BLACK_FUDGE_DEFAULT,				/* Black limit fudge */
  WHITE_FUDGE_DEFAULT,				/* White limit fudge */
  BRIGHT_SHIFT_DEFAULT,				/* Shift increment */
  BRIGHTNESS_DEFAULT,				/* No adjustment */
  FALSE,					/* Floating convolve? */
  FALSE,					/* select inverted */
  FALSE,					/* current select */
  FALSE,					/* redraw image */
  FALSE,					/* processing event */
  FALSE,					/* ihw bright */
  FALSE,					/* ihw bright init */
  FALSE,					/* ihw button */
  DEF_HIGHLIGHT_WIDTH,				/* highlight width */
  FALSE,					/* highlight on */
  TRUE,						/* highlight enabled */
  0,						/* highlight address */
  TRUE,						/* highlight red */
  FALSE,					/* highlight green */
  FALSE,					/* highlight blue */
  {0,0},					/* zoom upper left */
  {0,0},					/* zoom lower right */
  FALSE,					/* zoom displayed */
  FALSE,					/* zoom mode */
  0,						/* zoom style */
  FALSE,					/* zoom rubber band on */
  TRUE,						/* zoom square aspect */
  TRUE,						/* app running */
  TRUE,						/* enable gamma */
  FALSE,					/* Run in Sync (debug) */
  0,						/* Workstation name */
  0,						/* FB display offset x */
  20,						/*                   y */
  1010,						/*            width    */
  840,						/*            height   */
  1.20,						/* gamma */
  0.10,						/* gamma shift */
  0.0,						/* pix per count */
  0.015,					/* equalization threshold */
  TRUE,						/* Calibrate Image Data */
  0,						/* select offset */
  16,						/* select max */
  0,						/* base image file */
  0,						/* calib file */
  0,						/* current file */

  0,						/* main window */
  0,						/* image hist window */
  0,						/* text window */
  0,						/* select window */
  0,						/* small image window */
  0,						/* status window */
  0,						/* intensity window */

  {0}, 						/* highlight_band */
  {0}, 						/* Color update array */

  {0},						/* display_font */
  {0},						/* select_font */

  {0},						/* text_gc */
  {0},						/* status_gc */
  {0},						/* statusR_gc */
  {0},						/* select_gc */
  {0},						/* select_rubber_gc */
  {0},						/* hist_gc */
  {0},						/* zoom_rubber_gc */
  {0},						/* color_gc */

  {0},						/* Points for zoom box */

  "NASA CD Images",				/* Window Title */
  "NASA CD Images",				/* Icon  Title */

  &setup_gamma_correction,			/* Gamma correction routine */
  &setup_pixel_map				/* Pixel map routine */

};

static int position_fixed = 0;


char txtbuf[TEXT_MAX*60];

struct txtdef txt = {

  &txtbuf[0],					/* volume set name */
  &txtbuf[TEXT_MAX*1],				/* volume set id */
  &txtbuf[TEXT_MAX*2],				/* volume name */
  &txtbuf[TEXT_MAX*3],				/* volume id */
  &txtbuf[TEXT_MAX*4],				/* mission name */
  &txtbuf[TEXT_MAX*5],				/* mission phase name */

  {  &txtbuf[TEXT_MAX*6],			/* volume note [8] */
     &txtbuf[TEXT_MAX*7],
     &txtbuf[TEXT_MAX*8],
     &txtbuf[TEXT_MAX*9],
     &txtbuf[TEXT_MAX*10],
     &txtbuf[TEXT_MAX*11],
     &txtbuf[TEXT_MAX*12],
     &txtbuf[TEXT_MAX*13] },

  {  &txtbuf[TEXT_MAX*13],			/* data set description [40] */
     &txtbuf[TEXT_MAX*14],
     &txtbuf[TEXT_MAX*15],
     &txtbuf[TEXT_MAX*16],
     &txtbuf[TEXT_MAX*17],
     &txtbuf[TEXT_MAX*18],
     &txtbuf[TEXT_MAX*19],
     &txtbuf[TEXT_MAX*20],
     &txtbuf[TEXT_MAX*21],
     &txtbuf[TEXT_MAX*22],

     &txtbuf[TEXT_MAX*23],
     &txtbuf[TEXT_MAX*24],
     &txtbuf[TEXT_MAX*25],
     &txtbuf[TEXT_MAX*26],
     &txtbuf[TEXT_MAX*27],
     &txtbuf[TEXT_MAX*28],
     &txtbuf[TEXT_MAX*29],
     &txtbuf[TEXT_MAX*30],
     &txtbuf[TEXT_MAX*31],
     &txtbuf[TEXT_MAX*32],

     &txtbuf[TEXT_MAX*33],
     &txtbuf[TEXT_MAX*34],
     &txtbuf[TEXT_MAX*35],
     &txtbuf[TEXT_MAX*36],
     &txtbuf[TEXT_MAX*37],
     &txtbuf[TEXT_MAX*38],
     &txtbuf[TEXT_MAX*39],
     &txtbuf[TEXT_MAX*40],
     &txtbuf[TEXT_MAX*41],
     &txtbuf[TEXT_MAX*42],

     &txtbuf[TEXT_MAX*43],
     &txtbuf[TEXT_MAX*44],
     &txtbuf[TEXT_MAX*45],
     &txtbuf[TEXT_MAX*46],
     &txtbuf[TEXT_MAX*47],
     &txtbuf[TEXT_MAX*48],
     &txtbuf[TEXT_MAX*49],
     &txtbuf[TEXT_MAX*50],
     &txtbuf[TEXT_MAX*51],
     &txtbuf[TEXT_MAX*52] },

     {0},					/* Filter name */
     {0},					/* Target name */
     {0},					/* image id */
     {0},					/* image number */
     {0},					/* note data */
     {0},					/* image time */
     {0},					/* Filter number */
     {0},					/* scan mode */
     {0},					/* spacecraft name */
     {0},					/* work buffer */
     "Starting...",		/* status text */
     "Ready",					/* no select */
     "Calibrating Image...",			/* calibrating text */
     "Decompressing and displaying image..."	/* uncompressing text */

};

struct menudef {

  int   selection_number;
  char  *select_command;
  char  *select_text;
  void (*select_routine)();

  } main_menu[] = {

	0,
	"Quit",
	"Exiting...",
	&quit_ximq,

	1,
	"New Image",
	"Selecting New Image...",
	&new_image,

	2,
	"Reset Image",
	"Resetting Image...",
	&reset_image_variables,

	3,
	"Zoom",
	"Zooming...",
	&start_zoom,

	4,
	"UnZoom",
	"UnZooming...",
	&unzoom,

	5,
	"Enable EQ",
	"Equalizing...",
	&equalize,

	6,
	"Disable EQ",
	"Disable Equalization...",
	&noequal,

	7,
	"Gamma Bright",
	"Gamma Brightening...",
	&gamma_lighten,

	8,
	"Gamma Darken",
	"Gamma Darkening...",
	&gamma_darken,

	9,
	"Linear Bright",
	"Linear Brightening...",
	&brighten,

	10,
	"Linear Darken",
	"Linear Darkening...",
	&darken,

	11,
	"Enhance Edges",
	"Enhancing Edges...",
	&sharpen_image,

	12,
	"Blur Image",
	"Blurring Image...",
	&smooth,

	13,
	"Noise Filter",
	"Applying Noise Filter...",
	&apply_median_filter,

	14,
	"Refresh",
	"Refreshing...",
	&refresh_display,

	15,
	"Next Menu",
	"Next Menu...",
	&next_menu,

/* Messages for menu 2 */

	0,
	"Enable Calib",
	"Enabling Calibration...",
	&calibrate,

	1,
	"Disable Calib",
	"Disabling Calibration...",
	&uncalibrate,

	2,
	"",
	"",
	&nada,

	3,
	"",
	"",
	&nada,

	4,
	"",
	"",
	&nada,

	5,
	"",
	"",
	&nada,

	6,
	"",
	"",
	&nada,

	7,
	"",
	"",
	&nada,

	8,
	"",
	"",
	&nada,

	9,
	"",
	"",
	&nada,

	10,
	"",
	"",
	&nada,

	11,
	"",
	"",
	&nada,

	12,
	"",
	"",
	&nada,

	13,
	"",
	"",
	&nada,

	14,
	"",
	"",
	&nada,

	15,
	"Prev Menu",
	"Back...",
	&prev_menu

};

/*
 *  Routine:
 *
 *	main
 *
 *  Description:
 *
 *	Application entry point.  Calls initialization routines and main loop
 *
 *  Inputs:
 *
 *	argv, argc
 *
 *  Outputs:
 *
 *	none
 *
 */
main(argc, argv)
int argc;
char *argv[];

{

  int status, imgsize = 1;

  if (state.debugging)
    printf("main\n");

  lcl.base_image_file = malloc(sizeof(FILEINFO));
  lcl.calib_file      = malloc(sizeof(FILEINFO));
  lcl.current_file    = malloc(sizeof(FILEINFO));

  lcl.base_image_file->filename_valid = 0;

 /*
  *  Process the command line if present
  *
  */
  status = parse_command_line(argc, argv);
  if (!status) return;

  state.event = malloc(sizeof(XEvent));

 /*
  *  Make sure that the workstation is open
  *
  */
  if (!open_workstation()) return;

 /*
  *  We now have a connection to the display.  The next
  *  thing we need to do is set up the colormap.  This
  *  routine will also set the routine vectors that are
  *  based on the visual type.
  *
  */
  setup_colormap();

 /*
  *  Try to open the volume information file...
  *
  */
  if (state.use_volume)
    {
      status = open_volume_descriptor();

      if (!status)
	{
	  state.use_index  = FALSE;
	  state.use_volume = FALSE;
	}
    }

 /*
  *  Try to open the index file...
  *
  */
  if (state.use_index)
    {
      status = open_index_file();

      if (!status)
	{
	  state.use_index  = FALSE;
	  state.use_volume = FALSE;
	}

    }

  lcl.base_image_file->type  = 0;
  lcl.base_image_file->index_file_pos = 0;
  lcl.base_image_file->image_valid      = 0;
  lcl.base_image_file->image_calibrated = 0;

  lcl.calib_file->type   = 1;
  lcl.calib_file->index_file_pos   = 0;
  lcl.calib_file->image_valid      = 0;
  lcl.calib_file->image_calibrated = 0;
  lcl.calib_file->filename_valid   = 0;

 /*
  *  Open the workstation and find out the
  *  type of display...
  *
  */
  if (!open_workstation()) return(FALSE);

  if (state.planes > 8) imgsize = 4;

  imgsize = imgsize * (DEF_SCANLINES * (DEF_SAMPLES_PER_LINE+1));

  state.image_buffer1 = state.current_image = malloc(imgsize);
  state.zoom_buffer1  = state.zoom_buffer   = malloc(imgsize);
  state.zoom_buffer2  = malloc(imgsize);
  state.original_image_buffer1 = malloc(imgsize);
  state.original_image_buffer2 = malloc(imgsize);

  state.original_image  = &lcl.base_image_file->image_buffer[0];

 /*
  *  Open the initial image file
  *
  */
  status = process_image(lcl.base_image_file, FALSE);

 /*
  *  This is where we will spin the rest of the time, we will just
  *  dispatch X11 events...
  *
  */
  if (status) main_loop();

 /*
  *  Close the input file on exit
  *
  */
  close_file(lcl.base_image_file);
  close_file(lcl.calib_file);
}

/*
 *  Routine:
 *
 *	reset_image
 *
 *  Description:
 *
 *	Resets variables associated with the current image and free's any
 *	memory.
 *
 *  Inputs:
 *
 *	address of a file information block
 *
 *  Outputs:
 *
 *	none
 *
 */
void
reset_image(FILEINFO *fin)

{
  int i;

  if (fin == 0) return;

 /*
  *  Delete the huffman tree from the root
  *
  */
  if (fin->tree)
    {
      delete_node(fin->tree);
      fin->tree = 0;
    }

  rewind_file(fin);
  
  txt.filter_name[0] = 0;
  txt.filter_number[0] = 0;
  txt.target_name[0] = 0;
  txt.spacecraft_name[0] = 0;
  txt.image_id[0] = 0;
  txt.image_number[0] = 0;
  txt.image_time[0] = 0;
  txt.scan_mode[0] = 0;
  txt.note_data[0] = 0;
  state.num_scanlines = 0;
  state.samples_per_line = 0;

 /*
  *  Reset the image buffer pointers to the originals...
  *
  */  
  state.current_image   = state.image_buffer1;
  state.original_image  = &fin->image_buffer[0];
  state.image_buffer = 0;

 /*
  *  Reset any and all controls to the program defaults...
  *
  */
  lcl.black_limit      = BLACK_LIMIT_DEFAULT;
  lcl.white_limit      = WHITE_LIMIT_DEFAULT;
  lcl.auto_equal       = AUTO_EQUAL_DEFAULT;
  lcl.black_fudge      = BLACK_FUDGE_DEFAULT;
  lcl.white_fudge      = WHITE_FUDGE_DEFAULT;
  lcl.brightness_level = BRIGHTNESS_DEFAULT;
  lcl.gamma            = 1.20;

 /*
  *  Turn off zoom
  *
  */
  lcl.zoom_mode = 0;
  lcl.zoom_displayed = 0;
  state.zoom_buffer = state.zoom_buffer1;

  for (i = 0; i < 512; i += 1)
    {
      fin->image_hist[i] = 0;
      fin->encoding_hist[i] = 0;
    }

  if (state.image_window) XClearWindow(state.display, state.image_window);
}

/*
 *  Routine:
 *
 *	process_image
 *
 *  Description:
 *
 *	This routine requests the file, opens and processes the data and
 *	creates an image from it.
 *
 *  Inputs:
 *
 *	address of a file information block
 *	flag indicating that the output should be drawn
 *
 *  Outputs:
 *
 *	SUCCESS != 0
 *	FAIL    == 0
 *
 */
int
process_image(FILEINFO *fin, int draw_win_immediate)

{
  int status = 0;

  if (state.debugging)
    printf("process_image\n");

  if (fin == 0)
    {
      if (state.debugging)
	printf("File pointer is NULL!\n");
      return 0;
    }

  while (fin->input_file_open == 0)
    {
      status = get_new_image_file(fin);
      if (!(status & 1))
        {
          printf("Exiting XIMQ - error opening new image\n");
          return (FALSE);
        }
    }

 /*
  *  Create the display...
  *
  */
  if (!state.main_display_open)
    if (!create_main_display()) return FALSE;

  txt.status_text = "Processing new image...";
  display_image_status();

 /*
  *  Get the image information out of the header
  *
  */
  if (!process_label(fin, TRUE)) return (FALSE);

 /*
  *  Check for a couple of things...
  *
  */
  if ((state.num_scanlines    != DEF_SCANLINES) ||
      (state.samples_per_line != DEF_SAMPLES_PER_LINE))
    {
      printf("Unexpected file data! Size must be %d x %d\n",
			DEF_SCANLINES, DEF_SAMPLES_PER_LINE);
      return(FALSE);
    }

 /*
  *  Get the image histogram and find the equalization
  *  points if enabled.
  *
  */
  if (get_image_histogram(fin) == 0) return (FALSE);

 /*
  *  Get the encoding histogram for the decompression
  *
  */
  if (get_init_encoding_histogram(fin) == 0) return (FALSE);

 /*
  *  Set up a gamma correction table... that is, provide a non-linear
  *  color ramp if requested...
  *
  */
  (*lcl.gamma_routine)();
  
 /*
  *  Now, using the histogram equalization (if enabled) build the map of
  *  input pixel (0-255 linear ramp from black to white) into the allocated
  *  pixels thay represent the input intensity.  This step is needed because
  *  X11 is unfriendly in it's color allocation mechanisms - that is unless
  *  we use a private map.  Besides, we will use pixel value manipulation to
  *  massage the image data instead of colormap manipulation to allow this
  *  to run on a wide range of systems.
  *
  */
  (*lcl.pixel_map_routine)();

 /*
  *  Display the ancillary windows
  *
  */
  if (draw_win_immediate)
   {
     display_image_histogram(fin);
     display_image_colors(fin);
     display_select_window();
     display_image_text();
     display_image_status();
   }

  txt.status_text = txt.uncompressing_text;
  display_image_status();

 /*
  *  Decompress the image and display it...
  *
  */
  if (create_uncompressed_image(fin, state.current_image, TRUE) == 0) return FALSE;

 /*
  *  Flag this as the current file...
  *
  */
  lcl.current_file = fin;

 /*
  *  If calibration data is present, and enabled - then calibrate it!
  *  This will cause a second pass of the display!
  *
  */
  if ((lcl.calib_file->input_file_mapped) && (lcl.calibrate_image_data))
    {
      txt.status_text = txt.calibrating_text;
      display_image_status();

      if (!process_label(lcl.calib_file, FALSE)) return (FALSE);
      if (get_image_histogram(lcl.calib_file) == 0) return (FALSE);
      if (get_init_encoding_histogram(lcl.calib_file) == 0) return (FALSE);
      if (create_uncompressed_image(lcl.calib_file, FALSE, FALSE) == 0) return (FALSE);

      subtract_image(	&fin->image_buffer[0],
			&lcl.calib_file->image_buffer[0],
			&fin->image_buffer[0],
			800 * 800);

      fin->image_calibrated = 1;

      recompute_output(state.num_outlines, TRUE);

    }

  create_small_image(&fin->small_image_buffer[0], state.original_image, 0, 0, TRUE);

  txt.status_text = txt.no_select;
  display_image_status();

  return TRUE;
}

/*
 *  Routine:
 *
 *	create_small_image
 *
 *  Description:
 *
 *	Creates a 200x200 image from a 800x800 image by dropping 3 scans
 *	and using 1.
 *
 *  Inputs:
 *
 *	Pointer to destination image
 *	Pointer to source image
 *	x offset
 *	y offset
 *	flag if intermediate results should be displayed
 *	
 *  Outputs:
 *
 *	Always returns TRUE 
 */
int
create_small_image(unsigned char *output_image,
                   unsigned char *image_data,
                   int x_off, int y_off, int show_immediate)

{

  union {
   unsigned char *c;
   unsigned int  *l;
   } small_imagep;

  unsigned char *sourcep;

  int i, j, x, y, tx, ty;

  if (state.debugging)
    printf("create_small_image\n");

  small_imagep.c = output_image;

  if (show_immediate)
    {
      txt.status_text = "Creating small image...";
      display_image_status();
      state.small_ximage->data = (char *) small_imagep.c;
    }

 /*
  *  Clear it
  *
  */
  for (j = 0; j < SMALL_SCANLINES*SMALL_SAMPLES_PER_LINE; j += 1)
    {
      if (state.planes > 8)
	      *small_imagep.l++ = 0;
	  else
	      *small_imagep.c++ = 0;
    }

  small_imagep.c = output_image;

  i = state.num_scanlines / SMALL_SCANLINES;
  j = state.samples_per_line / SMALL_SAMPLES_PER_LINE;

 /*
  *  Quick and dirty, throw away data to get the reduced image
  *
  */
  for (y = 0; y < SMALL_SCANLINES; y += 1)
    {
      for (x = 0; x < SMALL_SAMPLES_PER_LINE; x += 1)
        {

	  tx = x + x_off;
	  if (tx >= state.samples_per_line) tx = state.samples_per_line - 1;

	  ty = y + y_off;
	  if (ty >= state.num_scanlines) ty = state.num_scanlines - 1;

	  sourcep = image_data + (((ty * i) * state.samples_per_line) + (tx * j));

	  if (state.planes > 8)
	      *small_imagep.l++ = state.pixel_map[*sourcep];
	  else
	      *small_imagep.c++ = state.pixel_map[*sourcep];

        }
      if (show_immediate) display_small_image_scanline(y, 1);
    }
  return (TRUE);
}

/*
 *  Routine:
 *
 *	create_uncompressed_image
 *
 *  Description:
 *
 *	Decompress the image, and optionally reformat and display it.
 *
 *  Inputs:
 *
 *	address of a file information block
 *	optional address of a output buffer to reformat the data into
 *	flag indicating if the data should be displayed as it's converted
 *
 *  Outputs:
 *
 *	the input buffer in the file information block contains the
 *	uncompressed data.  If requested, the output buffer is loaded
 *	with data formatted for the display, and optionally the image
 *	is displayed.
 *
 */
int
create_uncompressed_image(FILEINFO *fin,
                          unsigned char *image_pointer,
                          int show_immediate)

{   
  int status, line = 0, length = 0, out_bytes = 800, j = 0;
  unsigned char *input_record = 0, *image_p, *original_pointer;

  if (state.debugging)
    printf("create_uncompressed_image\n");

  original_pointer = &fin->image_buffer[0];

  if ((status = seek_var(fin, fin->image_record)) != fin->image_record)
    {
      printf("The file seek to the image record %d failed - returned %d\n",
		fin->image_record, status);
    }

  image_p = image_pointer;

  do
    {
     /*
      *  Position to the first record, and get length and pointer
      *
      */
      length = set_pos_var(fin, &input_record);

     /*
      *  Exit at EOF
      *
      */
      if (length <= 0) break;

     /*
      *  Decompress the image
      *
      */
      dcmprs(input_record, original_pointer, &length, &out_bytes, fin->tree);

      if (image_pointer != 0)
	{
	  reformat_image_data(original_pointer, image_pointer);

	  j += 1;

	  if ((show_immediate) && (j == state.num_outlines))
	    {
	      display_scanline(line - j, image_p, j);
	      image_p = image_pointer;
	      j = 0;
	    }
	}

      original_pointer += state.samples_per_line;
      line += 1;

    } while (length > 0 && line < state.num_scanlines);

  if ((image_pointer != 0) && (show_immediate) && (j > 0))
    {
      display_scanline(line - j, image_p, j);
    }

  if (line) fin->image_valid = 1;

  return (line);
}

/*
 *  Routine:
 *
 *	get_image_histogram
 *
 *  Description:
 *
 *	Reads the image histogram into the histogram array and generates
 *	the pixel equalization information.
 *
 *  Inputs:
 *
 *	address of a file information block
 *
 *  Outputs:
 *
 *	The image histogram array is loaded, and the equalization variables
 *	are set up.
 *
 */
int
get_image_histogram(FILEINFO *fin)

{
  int length, i, j, max_count = 0, min_count = 10000000;
  float p;

  if (state.debugging)
    printf("get_image_histogram\n");

  seek_var(fin, fin->image_histogram_record);

 /*
  *  Read the image histogram
  *
  */
  length  = read_var(fin, (unsigned char *)fin->image_hist);
  length += read_var(fin, (unsigned char *)fin->image_hist+length);

 /*
  *  It must be 256 int32s (1024 bytes)
  *
  */
  if (length != 1024)
    {
      printf("The image histogram length was not correct <%d>\n", length);
      return (0);
    }
 /*
  *  Loop through the array and find the min/max
  *
  */
  for (i = 0; i < 256; i += 1)
    {
     /*
      *  Find min and max
      *
      */
      if (fin->image_hist[i] > max_count)
        max_count = fin->image_hist[i];
      else if (fin->image_hist[i] < min_count)
        min_count = fin->image_hist[i];
    }

 /*
  *  This value is used for displaying the histogram
  *  and is the number of pixels in the histogram per
  *  frequency count.
  *
  */
  lcl.pix_per_count = 64.0 / (float)max_count; 

 /*
  *  For automatic equalization, we can build the
  *  black and white limits by analyzing the data.
  *
  *  The analysis is pretty simple/sleazy, it just
  *  looks for 2 counts in a row that are below the
  *  threshold.  The threshold is some small
  *  percentage of the peak.  First we scan from black
  *  then we scan from white down.  The points are
  *  adjusted by a user-settable fudge factor up or
  *  down and these then are used as the black and
  *  white limits for the image and the intensity
  *  ramp is between these values.  I us 2 counts in
  *  a row because these tends to be some noise spikes
  *  in image information that will consistantly cause
  *  the routine to have a wide band.
  *
  */
  if (lcl.auto_equal)
    {
      int preload = 0;

      p = max_count * lcl.equalization_threshold;
      j = p;

      for (i = 0; i < 256; i += 1)
        {
          if (fin->image_hist[i] > j)
            {
              if (preload)
		{
                  lcl.black_limit = i - lcl.black_fudge;
		  if (lcl.black_limit < 0) lcl.black_limit = 0;
                  break;
		}
	      else
		{
		  preload = 1;
		}
            }
	  else
	    {
	      preload = 0;
	    }
        }

      preload = 0;

      for (i = 255; i >= 0; i -= 1)
        {
          if (fin->image_hist[i] > j)
            {
              if (preload)
		{
                  lcl.white_limit = i + lcl.white_fudge;
		  if (lcl.white_limit >= MAX_VALUES) lcl.white_limit = MAX_PIXEL;
                  break;
		}
	      else
		{
		  preload = 1;
		}
            }
	  else
	    {
	      preload = 0;
	    }
        }
    }

  return (TRUE);

}

/*
 *  Routine:
 *
 *	get_init_encoding_histogram
 *
 *  Description:
 *
 *	Read and inits the encoding histogram from the image file.
 *
 *  Inputs:
 *
 *	address of a file information block
 *
 *  Outputs:
 *
 *	the image histogram array is loaded and the huffman tree is built.
 *
 *	TRUE  == success
 *	FALSE == fail
 *
 */
int
get_init_encoding_histogram(FILEINFO *fin)

{
  int x, length, status;

  if (state.debugging)
    printf("get_init_encoding_histogram\n");

  if (seek_var(fin, fin->encoding_histogram_record) == 0)
    {
      printf("Error positioning to the encoding histogram records (%d), current record %d\n",
              fin->encoding_histogram_record, fin->current_record);
      return (FALSE);
    }

  length = 0;

  for (x = 0; x < fin->encoding_histogram_length; x += 1)
    {
      status = read_var(fin, (unsigned char *)fin->encoding_hist+length);

      if (!status)
        {
          printf("Error reading encoding histogram record %d, current record %d\n",
                  x+1, fin->current_record);
          return (FALSE);
        }

      length += status;
    }

  fin->tree = huff_tree(fin->encoding_hist);

  return (TRUE);
}

/*
 *  Routine:
 *
 *	get_new_image_file
 *
 *  Description:
 *
 *	Closes the file if open, requests the filename for a new file,
 *	and opens the file.  It fills in the file information block with
 *	the new information.  It also closes the calibration image.
 *
 *  Inputs:
 *
 *	address of a file information block
 *
 *  Outputs:
 *
 *	file information block is filled in and the file is open and mapped
 *
 *	TRUE  = success (file open)
 *	FALSE = failed (file is not open)
 *
 */
int
get_new_image_file(FILEINFO *fin)

{
  int status;

  if (state.debugging)
    printf("get_new_image_file\n");

 /*
  *  Close the file if it's open
  *
  */
  close_file(fin);
  close_file(lcl.calib_file);

  if (state.manual_file_select)
    {
      lcl.calibrate_image_data = 0;

      if (!lcl.base_image_file->filename_valid)
	{
	  printf("filename: ");
	}

      status = 1;

    }
  else
    {
      status = get_image_file(&fin->input_filename[0], 256,
			  &fin->index_file_pos, &fin->type,
			  lcl.calibrate_image_data,
			  &lcl.calib_file->input_filename[0]);

    }

  if (!status) return (FALSE);

 /*
  *  Open and map the input file... 
  *
  */
  status = open_image_file(fin);

  if (!(status & 1))
    {
      printf("The image file could not be opened\n");
      return (FALSE);
    }

 /*
  *  Open and map the calibration image if present and asked for...
  *
  */
  if (lcl.calibrate_image_data)
    status = open_image_file(lcl.calib_file);

  if (!(status & 1))
    {
      printf("The calibration file could not be opened\n");
      return (FALSE);
    }

  hide_file_select_window();  

  return (TRUE);
}

/*
 *  Routine:
 *
 *	open_workstation
 *
 *  Description:
 *
 *	Creates the connection to the X11 display and gets information
 *	about the display.
 *
 *  Inputs:
 *
 *	None
 *
 *  Outputs:
 *
 *	TRUE  = success
 *	FALSE = failed to open display
 *
 */
int
open_workstation()

{

  if (state.debugging)
    printf("open_workstation\n");

  if (state.workstation_open) return (TRUE);

 /*
  *  Open the display...
  *
  */
  if (!state.display)
    state.display = XOpenDisplay(&lcl.display_name[0]);

  if (state.display == 0)
    {
      printf("ERROR: Could not open display\n");
      return (FALSE);
    }

  if (lcl.set_sync)
    {
      XSynchronize(state.display,1);
    }

  state.screen = XDefaultScreen(state.display);
  state.root   = XRootWindow(state.display, state.screen);
  state.planes = XDisplayPlanes(state.display, state.screen);
  state.visual = XDefaultVisual(state.display, state.screen);
  lcl.defmap = XDefaultColormap(state.display, state.screen);
  state.black_pixel = XBlackPixel(state.display, state.screen);
  state.foreground_pixel = state.white_pixel = XWhitePixel(state.display, state.screen);

  state.workstation_open = 1;

  return (TRUE);

}

Window
Add_Window(Display *display, Window root,
           int x, int y, int w, int h, int bw, 
           int depth, int class, Visual *visual, int mask,
           XSetWindowAttributes *att, void (*rtn)())

{
  Window win;

  win = XCreateWindow( display, root, x, y, w, h,
			 bw, depth, class, visual, mask, att);

  XMapWindow(state.display, win);
  XFlush(display);

  my_windows.list[my_windows.last].window = win;
  my_windows.list[my_windows.last].event_rtn = rtn;
  my_windows.last += 1;
  
  return win;
}


/*
 *  Routine:
 *
 *	create_main_display
 *
 *  Description:
 *
 *	Creates the main display window(s) and maps it.
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	TRUE  = success
 *	FALSE = error
 *
 */
int
create_main_display()
{
  int status, i, j, border_width = 1;
  float xform;
  XSetWindowAttributes xswa;
  XGCValues xgcv;
  XSizeHints xhint;
  char title_buf[100];
  char icon_buf[100];

  if (state.debugging)
    printf("create_main_display\n");

    {

      xswa.colormap         = state.colormap;
      xswa.background_pixel = state.default_pixel;
      xswa.border_pixel     = state.white_pixel;

     /*
      *  Create a image structure
      *
      */
      state.main_ximage = XCreateImage(state.display, state.visual, state.planes,
			ZPixmap, 0, (char *) &state.current_image,
			state.samples_per_line, state.num_scanlines, 8, 0);

      state.small_ximage = XCreateImage(state.display, state.visual, state.planes,
			ZPixmap, 0, (char *) &lcl.base_image_file->small_image_buffer[0],
			200, 200, 8, 0);

      xswa.event_mask = ExposureMask;

      FindBestWindowPosition( state.display, 0,
                              lcl.ws, lcl.hs, 0, 0,
                              &lcl.xs, &lcl.ys, 0, 0);

      if (state.debugging)
        printf("Best position for main window is X = %d, Y = %d\n", lcl.xs, lcl.ys);

     /*
      *  Now (at last) create the windows...
      *  
      *  
      *  +---+--+-----+-------------------------------------------+
      *  |   |  |Menu |                                           |
      *  | H |I +-----+                                           |
      *  | i |n |     |                                           |
      *  | s |t +-----+                                           |
      *  | t |e |     |                                           |
      *  | o |n +-----+                                           |
      *  |   |  |     |                  IMAGE                    |
      *  +---+--+ ----+                                           |
      *  |            |                                           |
      *  |    Text    |                                           |
      *  |            |                                           |
      *  +------------+                                           |
      *  | +--------+ |                                           |
      *  | |        | |                                           |
      *  | |  Small | +-------------------------------------------+
      *  | |        | |                  Status                   |
      *  | +--------+ +-------------------------------------------+
      *  +--------------------------------------------------------+
      *                                       \
      *                                        TopLevel
      *
      *  TopLevel	- main_window		- Main display window
      *  IMAGE		- image_window		- Main image display
      *  Small		- small_image_window	- 200 x 200 image display
      *  Text		- text_window		- Textual information
      *  Histo		- image_hist_window	- Image histogram and EQ edges
      *  Inten		- intensity_window	- Grey scale
      *  Menu		- select_menu		- Menu choices
      *  Status		- status_window		- text and status display
      *
      */
      lcl.main_window = Add_Window( state.display, state.root,
				lcl.xs, lcl.ys,
				lcl.ws, lcl.hs,
				border_width, state.planes, InputOutput,
				state.visual,
				CWBackPixel,
				&xswa,
				&event_on_main_window);


      xswa.event_mask = ExposureMask | ButtonMotionMask | ButtonPressMask |
			ButtonReleaseMask | KeyPressMask | PointerMotionMask;

      state.image_window = Add_Window( state.display, lcl.main_window,
				((lcl.ws-1)-state.samples_per_line), 0,
				state.samples_per_line, state.num_scanlines,
				border_width, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask
				| state.need_colormap,
				&xswa,
				&event_on_image_window);

      xswa.event_mask = ExposureMask;

      lcl.text_window = Add_Window( state.display, lcl.main_window,
				0, 256,
				205, (lcl.hs-(256+205)),
				border_width, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_text_window);

      lcl.status_window = Add_Window( state.display, lcl.main_window,
				((lcl.ws-1)-state.samples_per_line), 802,
				800, 36,
				border_width, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_status_window);

      xswa.background_pixel = state.default_pixel;
      xswa.border_pixel     = state.white_pixel;
      xswa.event_mask = ExposureMask | ButtonMotionMask | ButtonPressMask |
			ButtonReleaseMask | KeyPressMask | PointerMotionMask | LeaveWindowMask;

      lcl.image_hist_window = Add_Window( state.display, lcl.main_window,
				0, 0,
				76, 256,
				border_width, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_hist_window);

      xswa.background_pixel = state.foreground_pixel;
      lcl.select_window = Add_Window( state.display, lcl.main_window,
				108, 0,
				100, 256,
				border_width, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask,
				&xswa,
				&event_on_select_window);

      xswa.background_pixel = state.default_pixel;
      xswa.border_pixel     = state.white_pixel;

      xswa.event_mask = ExposureMask | ButtonMotionMask | ButtonPressMask |
			ButtonReleaseMask | KeyPressMask | PointerMotionMask;

      if (lcl.show_intensity_window)
	{
	  lcl.intensity_window = Add_Window( state.display, lcl.main_window,
				76, 0,
				INTENSITY_WINDOW_WIDTH, 256,
				border_width, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask
				| state.need_colormap,
				&xswa,
				&event_on_intensity_window);

	}

      xswa.event_mask = ExposureMask;

      lcl.small_image_window = Add_Window( state.display, lcl.main_window,
				((lcl.ws-1000)/2)-1, (lcl.hs-202),
				200, 200,
				border_width, state.planes, InputOutput,
				state.visual,
				CWBackPixel | CWBorderPixel | CWEventMask
				| state.need_colormap,
				&xswa,
				&event_on_small_image_window);

      xgcv.foreground = lcl.pixels[state.number_of_colors-1];
      xgcv.background = lcl.pixels[0];
      state.image_gc = XCreateGC(state.display, state.image_window, GCForeground | GCBackground, &xgcv);
      state.small_image_gc = XCreateGC(state.display, state.image_window, GCForeground | GCBackground, &xgcv);

      if (lcl.show_intensity_window)
        lcl.color_gc = XCreateGC(state.display, lcl.intensity_window, GCForeground | GCBackground, &xgcv);

      xgcv.function = GXxor;
      lcl.zoom_rubber_gc = XCreateGC(state.display, state.image_window, 
                   GCFunction | GCForeground | GCBackground, &xgcv);

      xgcv.foreground = state.foreground_pixel;
      xgcv.background = state.default_pixel;
      lcl.hist_gc = XCreateGC(state.display, lcl.image_hist_window, GCForeground | GCBackground, &xgcv);

      lcl.display_font = XLoadFont(state.display, FONT1);

      xgcv.foreground = state.foreground_pixel;
      xgcv.background = state.default_pixel;
      xgcv.function = GXcopy;

      lcl.text_gc = XCreateGC(state.display, lcl.text_window,
		GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, lcl.text_gc, lcl.display_font);

      lcl.status_gc = XCreateGC(state.display, lcl.status_window,
		GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, lcl.status_gc, lcl.display_font);

      xgcv.background = state.foreground_pixel;
      xgcv.foreground = state.default_pixel;
      lcl.statusR_gc = XCreateGC(state.display, lcl.status_window,
		GCFunction | GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, lcl.statusR_gc, lcl.display_font);

      lcl.select_font = XLoadFont(state.display, FONT2);
      lcl.select_gc = XCreateGC(state.display, lcl.select_window, GCForeground | GCBackground, &xgcv);
      XSetFont(state.display, lcl.select_gc, lcl.select_font);

      xgcv.background = state.foreground_pixel ^ state.default_pixel;
      xgcv.foreground = state.foreground_pixel ^ state.default_pixel;
      xgcv.function = GXxor;
      lcl.select_rubber_gc = XCreateGC(state.display, lcl.select_window,
                   GCFunction | GCForeground | GCBackground, &xgcv);

      XFlush(state.display);

      if (state.volume_file_open)
	{
	  sprintf(&title_buf[0],"Image Display - %s: %s",
		txt.volume_set_name, txt.volume_name);
	  lcl.window_name = &title_buf[0];

	  sprintf(&icon_buf[0],"%s\n%s",txt.mission_name, txt.mission_phase_name);
	  lcl.icon_name = &icon_buf[0];

	}

     /*
      *  Set the window manager stuff up.
      *
      */
      xhint.flags  = 0;
      xhint.x      = lcl.xs;
      xhint.y      = lcl.ys;
      xhint.width  = lcl.ws;
      xhint.height = lcl.hs;
      XSetStandardProperties(state.display, lcl.main_window, lcl.window_name, lcl.icon_name,
				None, 0, 0, &xhint);

      XFlush(state.display);

     /*
      *  Make it all show up
      *
      */
      XSync(state.display, 0);

     /*
      *  Install the colormap
      *
      */
      if (state.need_colormap)
        XInstallColormap(state.display, state.colormap);

     /*
      *  Hit it again...
      *
      */
      XSync(state.display, 0);

      state.main_display_open = TRUE;

    }
  return TRUE;
}

/*
 *  Routine:
 *
 *	setup_colormap
 *
 *  Description:
 *
 *	Sets up the colormap (creates it and inits it)
 *
 *  Inputs:
 *
 *	Address of file information block (?)
 *
 *  Outputs:
 *
 *	TRUE  = success
 *	FALSE = error
 *
 */
void
setup_colormap()

{
  int status, i, j;
  float xform;
  XSetWindowAttributes xswa;
  XGCValues xgcv;
  XSizeHints xhint;

  if (state.debugging)
    printf("setup_colormap\n");

 /*
  *  For handy reference later...
  *
  */
  state.vis_class = state.visual->class;

 /*
  *  Writeable versus non-writeable
  *
  */
  if ((state.visual->class & 1) && (!lcl.force_shared_colors))
    {
      int allocated = 0;

     /*
      *  Flag the fact that we have a R/W colormap
      *
      */
      lcl.writeable_map = 1;

     /*
      *  If the visual is a class that is a writeable map...
      *  If the map does not *have* to be private, try and simply
      *  allocate the cells...
      *
      */
      if (!lcl.private_map)
	{
	 /*
	  *  If the colors are already setup...
	  *
	  */
          if (lcl.cmap_setup)
            {

	     /*
	      *  If the old map is big enough for the request, use it!
	      *
	      */
	      if (lcl.cmap_size >= state.number_of_colors)
                {
		 /*
		  *  Set the allocated flag, and indicate if it is private..
		  *
		  */
		  allocated   = 1;
		  lcl.private_map = lcl.cmap_priv;
		  lcl.cmap        = state.colormap;
                }
	      else
		{
		 /*
		  *  Hmmm.  Not big enough for the new colormap size,
		  *  either delete the private map or free the color cells
		  *
		  */
		  if (lcl.cmap_priv)
		    {
			/* Free of the color map   - needs to be done... */
		    }
		  else
		    {
			/* Free of the color cells - needs to be done... */
		    }

		 /*
		  *  Clear the state...
		  *
		  */
		  lcl.cmap_setup = 0;
		  lcl.cmap_priv  = 0;
		  lcl.cmap_size  = 0;
		  lcl.cmap       = 0;
		}
            }

	 /*
	  *  If the colormap is not setup after the above, then try it
	  *  now...
	  *
	  */
          if (!lcl.cmap_setup)
	    {
              int max_planes, i;

	     /*
	      *  Use the normal colormap (or at least try...)
	      *
	      */
	      allocated = XAllocColorCells(state.display,
			   lcl.defmap, 0, &lcl.plane_masks[0], 1,
			   &lcl.pixels[0], state.number_of_colors);

	     /*
	      *  Try it non-contiguous
	      *
	      */
	      if (!allocated)
		allocated = XAllocColorCells(state.display,
			   lcl.defmap, 0, &lcl.plane_masks[0], 0,
			   &lcl.pixels[0], state.number_of_colors);

	     /*
	      *  If we allocated it, then set things up...
	      *
	      */
	      if (allocated)
		{
		  lcl.cmap_size  = state.number_of_colors;
		  lcl.cmap_setup = 1;
		  lcl.cmap_priv  = 0;
		  lcl.cmap       = lcl.defmap;
	          state.colormap   = lcl.defmap;
                  state.need_colormap = 0;
		}
	    }

	 /*
	  *  Make the default pixel black, and the colormap the default
	  *  indicate we do not need to install a colormap
          *
	  */
          state.default_pixel = state.black_pixel;
	}

     /*
      *  We failed to allocate the map from the writeable cells, so now
      *  let's try the hard way...
      *
      */
      if (!allocated)
	{
         /*
	  *  Either the above failed, or it wasn't asked for... so now
	  *  try to allocate a private colormap
	  *
	  */
          if (lcl.cmap_setup)
            {
	      if (lcl.cmap)
		{
	          if (lcl.cmap_size < state.number_of_colors)
                    {
			/* Need to delete the colormap! */
		      lcl.cmap_setup = 0;
		      lcl.cmap_priv  = 0;
		      lcl.cmap_size  = 0;
		      lcl.cmap       = 0;
                    }
		}
	       else
		{

		  /* Need to punt color cells... */

		  lcl.cmap_setup = 0;
		  lcl.cmap_priv  = 0;
		  lcl.cmap_size  = 0;
		  lcl.cmap       = 0;
		}
            }

	  if (!lcl.cmap_setup)
	    state.colormap = XCreateColormap(state.display, state.root, state.visual, AllocAll);

         /*
          *  Init the pixel value array...
          *
          */
	  for (i = 0; i < state.number_of_colors; i += 1)
            {
              lcl.pixels[i] = i;
            }

	 /*
	  *  *our* black pixel is zero and white is state.number_of_colors-1;
	  *
	  */
	  state.black_pixel = state.default_pixel = 0;
          state.white_pixel = state.foreground_pixel = state.number_of_colors-1;
          lcl.cmap_priv  = state.need_colormap = CWColormap;
	  lcl.cmap_size  = state.number_of_colors;
	  lcl.cmap_setup = 1;
	  lcl.cmap       = state.colormap;

	}              

     /*
      *  Now, do a linear ramp of grey from black to white.
      *
      */
      xform = 65535.0 / state.number_of_colors;

      for (i = 0; i < state.number_of_colors; i += 1)
        {
          lcl.colors[i].pixel = lcl.pixels[i];
          lcl.colors[i].flags = DoRed | DoGreen | DoBlue;
         /*
          *  Scale the 8-bit values to 16-bits
          *
          */
          lcl.colors[i].green = i * xform;

          if (lcl.no_red_blue_guns)
	    {
	      lcl.colors[i].red   = 0;
	      lcl.colors[i].blue  = 0;
	    }
	  else
	    {
	      lcl.colors[i].red   = i * xform;
	      lcl.colors[i].blue  = i * xform;
	    }
        }

     /*
      *  Load it all in one shot...
      *
      */
      XStoreColors(state.display, state.colormap, &lcl.colors[0], state.number_of_colors);

    } /* End of if writeable map */

  else

    {
      int eh;

      lcl.writeable_map = 0;

     /*
      *  Special case for a 24-plane TrueColor visual to just load the
      *  bits directly... NOTE to myself, this really needs to use the
      *  red, green and blue bits and masks to ensure that the shift is
      *  correct, there are some devices where the R/G/B bits are not in
      *  the normal locations.
      *
      */
      if ((state.visual->class == TrueColor) && (state.planes >= 24))
        {
	  int nval;

	 /*
          *  Force the number of colors to be all 256
          *
          */
	  state.number_of_colors = MAX_VALUES;

	  for (i = 0; i < state.number_of_colors; i += 1)
	    {
              lcl.pixels[i]  = i;
	      lcl.pixels[i] |= i << 8;
	      lcl.pixels[i] |= i << 16;
	    }           
        }
      else
	{
         /*
          *  For a read-only map we do things in reverse of the above...
          *  we loop doing a color ramp from black to white and at each step
          *  we ask for the closest available color.  We then store the pixel
          *  value in the array.  Note, the above is written for the default
          *  map only.  Later we may find it useful to add non-default visual
          *  support.
          *
          */
          xform = 65535.0 / state.number_of_colors;

          for (i = 0; i < state.number_of_colors; i += 1)
            {
              lcl.colors[i].flags = DoRed | DoGreen | DoBlue;

             /*
              *  Scale the 8-bit values to 16-bits
              *
              */
              lcl.colors[i].green = i * xform;

              if (lcl.no_red_blue_guns)
	        {
	          lcl.colors[i].red   = 0;
	          lcl.colors[i].blue  = 0;
	        }
	      else
	        {
	          lcl.colors[i].red   = i * xform;
	          lcl.colors[i].blue  = i * xform;
	        }

              eh = XAllocColor(state.display, lcl.defmap, &lcl.colors[i]);

              if (!eh)
                {
                  printf("Failed to allocate static color %d\n", i);
                  lcl.pixels[i] = state.white_pixel;
                }
              else
                {
                  lcl.pixels[i] = lcl.colors[i].pixel;
                }
            }

         }

     /*
      *  Always use the default pixel...
      *
      */
      state.default_pixel = state.black_pixel;

    }

 /*
  *  If we did not allocate a private map, then we can try to use some
  *  additional colors to jazz things up.
  *
  */
  if (!state.need_colormap)
    {
      int eh;
      XColor mycolor;

          mycolor.flags = DoRed | DoGreen | DoBlue;
          mycolor.red   = lcl.default_pixel_red   << 8;
          mycolor.green = lcl.default_pixel_green << 8;
          mycolor.blue  = lcl.default_pixel_blue  << 8;
          eh = XAllocColor(state.display, lcl.defmap, &mycolor);
          state.default_pixel = mycolor.pixel;

          mycolor.flags = DoRed | DoGreen | DoBlue;
          mycolor.red   = lcl.foreground_pixel_red   << 8;
          mycolor.green = lcl.foreground_pixel_green << 8;
          mycolor.blue  = lcl.foreground_pixel_blue  << 8;
          eh = XAllocColor(state.display, lcl.defmap, &mycolor);
          state.foreground_pixel = mycolor.pixel;

          mycolor.flags = DoRed | DoGreen | DoBlue;
          mycolor.red   = lcl.highlight_pixel_red   << 8;
          mycolor.green = lcl.highlight_pixel_green << 8;
          mycolor.blue  = lcl.highlight_pixel_blue  << 8;
          eh = XAllocColor(state.display, lcl.defmap, &mycolor);
          state.red_pixel = mycolor.pixel;
    }
}

/*
 *  Routine:
 *
 *	setup_gamma_correction
 *
 *  Description:
 *
 *	Loads the gamma correction array.
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *	TRUE  = success
 *	FALSE = error
 *
 */
int
setup_gamma_correction()

{
  int i, width;
  double color_in, color_out, xform;

  if (state.debugging)
    printf("setup_gamma_correction\n");

  width = state.number_of_colors-1;

  if (lcl.enable_gamma)
  {
    xform = 1.0 / width;

    for (i = 0; i <= width; i += 1)
      {
	  {
	   /*
	    *  Translate the intensity to 0-1
	    *
	    */
            color_in = i * xform;

	   /*
	    *  Compute color**GAMMA, which is the simple gamma correction
            *  we are using.
	    *
            */
            color_out = pow(color_in, lcl.gamma);

	   /*
	    *  Transform the result back from 0.0-1.0 to 0-MAX_PIXEL
	    *
	    */
            state.gamma_correction[i] = (int)((color_out / xform) + 0.5);

	   /*
	    *  Clamp the result
	    *
	    */
            if (state.gamma_correction[i] < 0) state.gamma_correction[i] = 0;
              else if (state.gamma_correction[i] > width)
		state.gamma_correction[i] = width;
          }
      }
    }
  else
    {
     /*
      *  Gamma correction of 1.0 (linear), so shortcut it.
      *
      */
      for (i = 0; i < width; i += 1)
        {
          state.gamma_correction[i] = i;
        }
    }

  return TRUE;

}


/*
 *  Routine:
 *
 *	setup_pixel_map
 *
 *  Description:
 *
 *	Loads the map of input pixel values to output pixel values.
 *	Gamma correction and histogram equalization are also applied.
 *	A translation table (value -> value) is also built to facilitate
 *	translation of a value to the corrected value (not the pixel
 *	index of the corrected value).
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *	TRUE  = success
 *	FALSE = error
 *
 */
void
setup_pixel_map()

{

  int status, h, i, k, j, width;
  float xform1, xform2;

  if (state.debugging)
    printf("setup_pixel_map\n");

 /*
  *  Build the pixel map array.  First get the transformation for pixels
  *  between the black and white limits.  The resulting map will be
  *  both equalized between these limits, and if enabled a gamma correction
  *  will be applied.
  *
  */
  width = (MAX_VALUES-(lcl.black_limit+(MAX_VALUES-(lcl.white_limit+1))));
  xform1 = (width * 1.0) / MAX_PIXEL;
  xform2 = state.number_of_colors / (width * 1.0);

 /*
  *  Init black
  *
  */
  k = lcl.black_limit - lcl.brightness_level;

  if (k < 0)
    k = 0;
  else if (k > MAX_PIXEL) k = MAX_PIXEL;

  if (k)
    for (i = 0; i <= k; i += 1)
      {
        state.pixel_map[i] = lcl.pixels[0]; /* Black for all these... */
      }

 /*
  *  Init White
  *
  */
  k = lcl.white_limit - lcl.brightness_level;

  if (k < 0)
    k = 0;
  else if (k > MAX_PIXEL) k = MAX_PIXEL;

  for (i = k; i <= MAX_PIXEL; i += 1)
    {
       state.pixel_map[i] = lcl.pixels[state.number_of_colors-1];
    }

 /*
  *  Loop for each possible pixel value to build the pixel map
  *
  */
  for (i = lcl.black_limit; i < lcl.white_limit; i += 1)
    {

     /*
      *  Translate the value to zero based coordinate
      *
      */
      k = i - lcl.black_limit;

     /*
      *  If we are <= 0, then we are black
      *
      */
      if (k <= 0)
	{
	  j = 0;
	}
      else
	{
	 /*
	  *  Transform the value into the histogram space
	  *
	  */
          j = (k * xform1) + 0.5;

	 /*
	  *  Clamp at the high end of histogram space
	  *
	  */
          if (j > width)
            j = width;
	}

     /*
      *  The pixel has now been translated into the coordinate space
      *  of the histogram.  Now we need to translate this coordinate
      *  into the coordinate system of the colormap.
      *
      */
      j = (k * xform2) + 0.5;

     /*
      *  Clamp at the high end of colormap space
      *
      */
      if (j >= state.number_of_colors)
	j = state.number_of_colors-1;

     /*
      *  The pixel has now been translated into the coordinate space
      *  of the colormap.  Now we need to gamma correct the pixel if
      *  we need to...
      *
      */
      if (lcl.enable_gamma)
        {
          j = state.gamma_correction[j];
        }

      /*
       *  This array contains the end result of everything except the
       *  actual mapping of the pixel to a hardware pixel.  It can be
       *  thought of as the output pixel or the final intensity.  We
       *  can use this map to convert any input pixel into a final
       *  intensity value, this value is then actually used to access
       *  a hardware pixel by the state.pixel_map array below.
       *
       */
       state.pixel_intensities[i] = j;

      /*
       *  Now, finally - fetch the pixel value for this intensity
       *  corrected by the linear brightness level
       *
       */
       k = i-lcl.brightness_level;

       if (k < 0)
         k = 0;
       else if (k > MAX_PIXEL) k = MAX_PIXEL;

       state.pixel_map[k] = lcl.pixels[j];

    }
}

/*
 *  Routine:
 *
 *	main_loop
 *
 *  Description:
 *
 *	X11 applications like a main loop.  This just loops on the next
 *	event, and exits when the app_running flag is cleared.
 *
 *  Inputs:
 *
 *	None
 *
 *  Outputs:
 *
 *	None
 *
 */
void
main_loop()
  {

      if (state.debugging)
        printf("main_loop\n");

      while (lcl.app_running)
        {
         /*
          *  We simply wait for an event...
          *
          */
          XNextEvent(state.display, state.event);
	  handle_events(1, TRUE);

	  if (lcl.redraw_image)
	    {
	      display_image(0, state.num_scanlines);
	      lcl.redraw_image = FALSE;
	    }
        }
  }

/*
 *  Routine:
 *
 *	event_on_main_window
 *
 *  Description:
 *
 *	Handle events for main window (all events ignored)
 *
 *  Inputs:
 *
 *	Pointer to event
 *
 *  Outputs:
 *
 *	none
 *
 */
void
event_on_main_window(XEvent *event, int from_main_loop)
{
}

/*
 *  Routine:
 *
 *	event_on_text_window
 *
 *  Description:
 *
 *	Handle events for text window
 *
 *  Inputs:
 *
 *	Pointer to event
 *
 *  Outputs:
 *
 *	none
 *
 */
void
event_on_text_window(XEvent *event, int from_main_loop)
{
  if (event->type == Expose)
    {
      display_image_text();
    }
}

/*
 *  Routine:
 *
 *	event_on_status_window
 *
 *  Description:
 *
 *	Handle events for status window
 *
 *  Inputs:
 *
 *	Pointer to event
 *
 *  Outputs:
 *
 *	none
 *
 */
void
event_on_status_window(XEvent *event, int from_main_loop)
{
  if (event->type == Expose)
    {
      display_image_status();
    }
}

/*
 *  Routine:
 *
 *	event_on_hist_window
 *
 *  Description:
 *
 *	Handle events for histogram window
 *
 *  Inputs:
 *
 *	Pointer to event
 *
 *  Outputs:
 *
 *	none
 *
 */
void
event_on_hist_window(XEvent *event, int from_main_loop)
{
  XEvent peek_event;
  int jj, event_count;

  if (event->type == Expose)
    {
      display_image_histogram(lcl.current_file);
      return;
    }

 /*
  *  Only do this if the call was from the mainline.  Only handle
  *  exposures from a non-mainline call.
  *
  */
  if (from_main_loop)
    {
      switch (state.event->type)
        {
          case ButtonPress:
           /*
            *  See if it is in band...
            *
            */
            if ((state.event->xbutton.y > 255) ||
               (state.event->xbutton.y < 0)) break;

           /*
            *  Well, it's within the high/low for Y
            *
            */
            if ((state.event->xbutton.x >= EQ_X1) &&
                (state.event->xbutton.x <= EQ_X2))
              {
               /*
                *  It's in the EQ band
                *
                */
                if ((state.event->xbutton.y >= lcl.black_limit-4) &&
                    (state.event->xbutton.y <= lcl.black_limit+4))
                  {
                    jj = lcl.white_limit - state.event->xbutton.y;
                    if (jj > 4)
                      lcl.black_limit = state.event->xbutton.y;
                    lcl.ihw_button = 1;
                  }
                else if ((state.event->xbutton.y >= lcl.white_limit-4) &&
                         (state.event->xbutton.y <= lcl.white_limit+4))
                  {
                    jj = state.event->xbutton.y - lcl.black_limit;
                    if (jj > 4)
                      lcl.white_limit = state.event->xbutton.y;
                    lcl.ihw_button = 2;
                  }

                lcl.auto_equal = FALSE;
                display_image_histogram(lcl.current_file);
              }  /* In EQ band */
            else
              {
               /*
                *  Must be in the histogram...
                *
                */
                if (!lcl.ihw_button)
                  {
                    lcl.ihw_bright_init = lcl.brightness_level;
                    lcl.ihw_bright      = state.event->xbutton.y;
                  }
               }
            break;

          case ButtonRelease:
           /*
            *  Final execution at current position
            *
            */
            if (lcl.ihw_button)
              {
                if ((state.event->xbutton.y > 255) ||
                    (state.event->xbutton.y < 0)) break;

                if (lcl.ihw_button == 1)
                  {
                    jj = lcl.white_limit - state.event->xbutton.y;
                    if (jj > 4)
                      lcl.black_limit = state.event->xbutton.y;
                    else
                      lcl.black_limit = lcl.white_limit - 5;
                  }
                else
                  {
                    jj = state.event->xbutton.y - lcl.black_limit;
                    if (jj > 4)
                      lcl.white_limit = state.event->xbutton.y;
                    else
                      lcl.white_limit = lcl.black_limit + 5;
                  }
                lcl.ihw_button = FALSE;
                recompute_output(state.num_outlines, lcl.pixel_twiddle);
              }
            else
              {
                if (lcl.ihw_bright)
                  {
                    jj = (state.event->xmotion.y - lcl.ihw_bright) + lcl.ihw_bright_init;
                    lcl.brightness_level = jj;
                    lcl.ihw_bright = FALSE;
                    recompute_output(state.num_outlines, lcl.pixel_twiddle);
                  }
               }
             break;

          case MotionNotify:
           /*
            *  Handle any image changes while button is depressed
            *
            */
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

                if ((peek_event.xany.window == lcl.image_hist_window) &&
                    (peek_event.type == MotionNotify))
                  XNextEvent(state.display, state.event);
                else break;

                event_count = XEventsQueued(state.display, QueuedAfterFlush);
              }

            if (lcl.ihw_button)
              {
                if ((state.event->xmotion.y > 255) ||
                    (state.event->xmotion.y < 0)) break;

                if (lcl.ihw_button == 1)
                  {
                    jj = lcl.white_limit - state.event->xmotion.y;
                    if (jj > 4)
                      lcl.black_limit = state.event->xmotion.y;
                        else
                      lcl.black_limit = lcl.white_limit - 5;
                  }
                else
                  {
                    jj = state.event->xmotion.y - lcl.black_limit;
                    if (jj > 4)
                      lcl.white_limit = state.event->xmotion.y;
                        else
                      lcl.white_limit = lcl.black_limit + 5;
                  }
                display_image_histogram(lcl.current_file);
              }
            else
              {
                if (lcl.ihw_bright)
                  {
                    jj = (state.event->xmotion.y - lcl.ihw_bright) + lcl.ihw_bright_init;
                    lcl.brightness_level = jj;
                    display_image_histogram(lcl.current_file);
                  }
              }
            break;
        } /* End of switch for Image Hist events */
    } 
}

/*
 *  Routine:
 *
 *	event_on_select_window
 *
 *  Description:
 *
 *	Handle events for menu selection window
 *
 *  Inputs:
 *
 *	Pointer to event
 *
 *  Outputs:
 *
 *	none
 *
 */
void
event_on_select_window(XEvent *event, int from_main_loop)
{
  XEvent peek_event;
  int event_count;

  if (event->type == Expose)
    {
      display_select_window();
      return;
    }

  if (state.event->xany.window == lcl.select_window)
    {
     /*
      *  Handle non-exposure events for the select window
      *
      */
      int i, button;

      switch (state.event->type)
        {
          case ButtonRelease:

           /*
            *  Each button is 16 pixels high.  Turn the location into
            *  a value from 0-15.
            *
            */
            button = ((state.event->xbutton.y + 15) / 16) - 1;

           /*
            *  Clamp button from 0-15
            *
            */
            if (button > 15) button = 15;
            if (button < 0) button = 0;

            i = button * 16;

           /*
            *  If select is inverted, ignore the button
            *
            */
            if (lcl.select_inverted)
              {
                if (lcl.current_select == button) break;
                XFillRectangle(state.display, lcl.select_window, lcl.select_rubber_gc, 0, lcl.current_select * 16, 150, 16);
              }

            XFillRectangle(state.display, lcl.select_window, lcl.select_rubber_gc, 0, i, 150, 16);
            lcl.current_select  = button;
            lcl.select_inverted = 1;

            XFlush(state.display);

            break;

          case ButtonPress:

            button = ((state.event->xbutton.y + 15) / 16) - 1;

            if (button > 15) button = 15;
            if (button < 0) button = 0;

            if (lcl.current_select == button)
              {
               /*
                *  Only while we are not currently active
                *
                */
                if (from_main_loop)
                  {
                    int command;
                    command = lcl.select_offset + button;
                    txt.status_text = main_menu[command].select_text;
                    display_image_status();
                    (*main_menu[command].select_routine)();
                    txt.status_text = txt.no_select;
                    display_image_status();
                  }
                }
              break;

            case MotionNotify:

              event_count = XEventsQueued(state.display, QueuedAfterFlush);

              while (event_count > 0)
                {
                  XPeekEvent(state.display, &peek_event);

                  if ((peek_event.xany.window == lcl.select_window) &&
                      (peek_event.type == MotionNotify))
                    XNextEvent(state.display, state.event);
                  else break;

                  event_count = XEventsQueued(state.display, QueuedAfterFlush);
                }

              button = ((state.event->xmotion.y + 15) / 16) - 1;

              if (button > 15) button = 15;
              if (button < 0) button = 0;
              i = button * 16;

              if (lcl.select_inverted)
                {

                  if (lcl.current_select == button)
                  break;

                  XFillRectangle(state.display, lcl.select_window, lcl.select_rubber_gc, 0, lcl.current_select * 16, 150, 16);
                }

              XFillRectangle(state.display, lcl.select_window, lcl.select_rubber_gc, 0, i, 150, 16);
              lcl.current_select  = button;
              lcl.select_inverted = 1;

              XFlush(state.display);

              break;

            case LeaveNotify:

              if (state.event->xcrossing.mode != NotifyNormal) break;

                if (lcl.select_inverted)
                  {
                    XFillRectangle(state.display, lcl.select_window, lcl.select_rubber_gc, 0, lcl.current_select * 16, 150, 16);
                  }

                lcl.select_inverted = 0;
                lcl.current_select  = 99;

                XFlush(state.display);

                break;

        } /* End switch */
    }
}

/*
 *  Routine:
 *
 *	event_on_intensity_window
 *
 *  Description:
 *
 *	Handle events for menu selection window
 *
 *  Inputs:
 *
 *	Pointer to event
 *
 *  Outputs:
 *
 *	none
 *
 */
void
event_on_intensity_window(XEvent *event, int from_main_loop)
{
  XEvent peek_event;
  int x, y, event_count;

  if (event->type == Expose)
    {
      display_image_colors(lcl.current_file);
      return;
    }

  if (from_main_loop)
    {
      switch (state.event->type)
        {

          case ButtonPress:

            y = state.event->xbutton.y;

            if (y > MAX_PIXEL)
              y = MAX_PIXEL;
            else
              if (y < 0) y = 0;

            if (lcl.highlight_enabled)
              {
               /*
                *  Must do things differently based on map types
                *
                */
                if (lcl.writeable_map)
                  {
                    lcl.highlight_on =
                    highlight_colors_writeable(TRUE,
                                               lcl.highlight_on,
                                               lcl.highlight_address,
                                               y,
                                               lcl.highlight_width);
                    if (lcl.highlight_on)
                    lcl.highlight_address = y;
                  }
                else
                  {
                    break; /* NYI */
                  }
              }

            break;

          case ButtonRelease:

            y = state.event->xbutton.y;

            if (y > MAX_PIXEL) y = MAX_PIXEL;
            else if (y < 0) y = 0;

            if (lcl.highlight_enabled)
              {
               /*
                *  Must do things differently based on map types
                *
                */
                if (lcl.writeable_map)
                  {
                    lcl.highlight_on =
                      highlight_colors_writeable(FALSE,
                                                 lcl.highlight_on,
                                                 lcl.highlight_address,
                                                 y,
                                                 lcl.highlight_width);
                    if (lcl.highlight_on)
                      lcl.highlight_address = y;
                  }
                else
                  {
                    break; /* NYI */
                  }
              }

            break;

          case MotionNotify:

            event_count = XEventsQueued(state.display, QueuedAfterFlush);

            while (event_count > 0)
              {
                XPeekEvent(state.display, &peek_event);

                if ((peek_event.xany.window == lcl.image_hist_window) &&
                    (peek_event.type == MotionNotify))
                XNextEvent(state.display, state.event);
                else break;

                event_count = XEventsQueued(state.display, QueuedAfterFlush);
              }

            y = state.event->xmotion.y;
            if (y > MAX_PIXEL) y = MAX_PIXEL;
            else if (y < 0) y = 0;

            if (lcl.highlight_enabled)
              {
               /*
                *  Must do things differently based on map types
                *
                */
                if (lcl.writeable_map)
                  {
                    if (lcl.highlight_on)
                      lcl.highlight_on =
                        highlight_colors_writeable(TRUE,
                                                   lcl.highlight_on,
                                                   lcl.highlight_address,
                                                   y,
                                                   lcl.highlight_width);
                    if (lcl.highlight_on)
                      lcl.highlight_address = y;
                  }
                else
                  {
                    break; /* NYI */
                  }
              }

            break;
        } 
    }
}

/*
 *  Routine:
 *
 *	event_on_image_window
 *
 *  Description:
 *
 *	Handle events for the image window
 *
 *  Inputs:
 *
 *	Pointer to event
 *
 *  Outputs:
 *
 *	none
 *
 */
void
event_on_image_window(XEvent *event, int from_main_loop)
{
  XEvent peek_event;
  int x, y, event_count;

  if (event->type == Expose)
    {
      if (from_main_loop)
        display_image(state.event->xexpose.y, state.event->xexpose.height);
      else
        lcl.redraw_image = TRUE;
      return;
    }

  if (from_main_loop)
    {
      switch (state.event->type)
        {
          case ButtonPress:

            if (lcl.zoom_mode == 1)
              {
                lcl.zoom_mode = 2;

                lcl.zoom_rubber_box[1].x = state.event->xbutton.x;

                if (state.event->xbutton.x < 0)
                  state.event->xbutton.x = 0;

                if (state.event->xbutton.x >= state.samples_per_line)
                  state.event->xbutton.x = state.samples_per_line-1;

                if (state.event->xbutton.y < 0)
                  state.event->xbutton.y = 0;

                if (state.event->xbutton.y >= state.num_scanlines)
                  state.event->xbutton.y = state.num_scanlines-1;

                lcl.zoom_rubber_box[0].x = state.event->xbutton.x;
                lcl.zoom_rubber_box[1].x = state.event->xbutton.x;
                lcl.zoom_rubber_box[2].x = state.event->xbutton.x;
                lcl.zoom_rubber_box[3].x = state.event->xbutton.x;
                lcl.zoom_rubber_box[4].x = state.event->xbutton.x;
                lcl.zoom_rubber_box[0].y = state.event->xbutton.y;
                lcl.zoom_rubber_box[1].y = state.event->xbutton.y;
                lcl.zoom_rubber_box[2].y = state.event->xbutton.y;
                lcl.zoom_rubber_box[3].y = state.event->xbutton.y;
                lcl.zoom_rubber_box[4].y = state.event->xbutton.y;

                display_image_text();

              }
            break;

          case ButtonRelease:

            if (lcl.zoom_mode == 2)
              {
                lcl.zoom_mode = 0;

                undraw_zoom_rubberband();

                lcl.zoom_rubber_box[1].x = state.event->xbutton.x;

                if (state.event->xbutton.x < 0)
                  state.event->xbutton.x = 0;

                if (state.event->xbutton.x >= state.samples_per_line)
                  state.event->xbutton.x = state.samples_per_line-1;

                if (state.event->xbutton.y < 0)
                  state.event->xbutton.y = 0;

                if (state.event->xbutton.y >= state.num_scanlines)
                  state.event->xbutton.y = state.num_scanlines-1;

                  lcl.zoom_rubber_box[2].x = state.event->xbutton.x;
                  lcl.zoom_rubber_box[2].y = state.event->xbutton.y;
                  lcl.zoom_rubber_box[3].y = state.event->xbutton.y;

                 /*
                  *  Find the upper left and lower right corners
                  *
                  */
                  if (lcl.zoom_rubber_box[0].x < lcl.zoom_rubber_box[2].x)
                    {
                      lcl.zoom_upper_left[0]  = lcl.zoom_rubber_box[0].x;
                      lcl.zoom_lower_right[0] = lcl.zoom_rubber_box[2].x;

                      if (lcl.zoom_rubber_box[0].y < lcl.zoom_rubber_box[2].y)
                        {
                          lcl.zoom_upper_left[1]  = lcl.zoom_rubber_box[0].y;
                          lcl.zoom_lower_right[1] = lcl.zoom_rubber_box[2].y;
                        }
                      else
                        {
                          lcl.zoom_upper_left[1]  = lcl.zoom_rubber_box[2].y;
                          lcl.zoom_lower_right[1] = lcl.zoom_rubber_box[0].y;
                        }
                    }
                  else
                    {
                      lcl.zoom_upper_left[0]  = lcl.zoom_rubber_box[2].x;
                      lcl.zoom_lower_right[0] = lcl.zoom_rubber_box[0].x;

                      if (lcl.zoom_rubber_box[0].y < lcl.zoom_rubber_box[2].y)
                        {
                          lcl.zoom_upper_left[1]  = lcl.zoom_rubber_box[0].y;
                          lcl.zoom_lower_right[1] = lcl.zoom_rubber_box[2].y;
                        }
                      else
                        {
                          lcl.zoom_upper_left[1]  = lcl.zoom_rubber_box[2].y;
                          lcl.zoom_lower_right[1] = lcl.zoom_rubber_box[0].y;
                        }
                    }

                  if (((lcl.zoom_lower_right[0] - lcl.zoom_upper_left[0]) > 2)
                   && ((lcl.zoom_lower_right[1] - lcl.zoom_upper_left[1]) > 2))
                    {
                      zoom_image(
                                 lcl.zoom_upper_left[0],
                                 lcl.zoom_upper_left[1],
                                 lcl.zoom_lower_right[0],
                                 lcl.zoom_lower_right[1],
                                 lcl.zoom_square_aspect,
                                 lcl.zoom_style);
                    }

                  display_image_text();
              } 
            break;

          case MotionNotify:

            event_count = XEventsQueued(state.display, QueuedAfterFlush);

            while (event_count > 0)
              {
                XPeekEvent(state.display, &peek_event);

                if ((peek_event.xany.window == state.image_window) &&
                    (peek_event.type == MotionNotify))
                  XNextEvent(state.display, state.event);
                else break;

                event_count = XEventsQueued(state.display, QueuedAfterFlush);
              }

            if (lcl.zoom_mode > 1)
              {
                undraw_zoom_rubberband();

                if (state.event->xmotion.x < 0)
                  state.event->xmotion.x = 0;

                if (state.event->xmotion.x >= state.samples_per_line)
                  state.event->xmotion.x = state.samples_per_line-1;

                if (state.event->xmotion.y < 0)
                  state.event->xmotion.y = 0;

                if (state.event->xmotion.y >= state.num_scanlines)
                  state.event->xmotion.y = state.num_scanlines-1;

                lcl.zoom_rubber_box[1].x = state.event->xmotion.x;
                lcl.zoom_rubber_box[2].x = state.event->xmotion.x;
                lcl.zoom_rubber_box[2].y = state.event->xmotion.y;
                lcl.zoom_rubber_box[3].y = state.event->xmotion.y;

                draw_zoom_rubberband();
              }
            break;
        } /* End of switch */
    }
}

/*
 *  Routine:
 *
 *	event_on_small_image_window
 *
 *  Description:
 *
 *	Handle events for the small image window
 *
 *  Inputs:
 *
 *	Pointer to event
 *
 *  Outputs:
 *
 *	none
 *
 */
void
event_on_small_image_window(XEvent *event, int from_main_loop)
{
  if (event->type == Expose)
    {
      display_small_image_window();
    }
}

/*
 *  Routine:
 *
 *	handle_events
 *
 *  Description:
 *
 *	This is the event loop.  It can be called from multiple places,
 *	and may be called with a unprocessed event already dequeued.
 *	It will find the event window, and dispatch to the event handler
 *	for the window.
 *
 *  Inputs:
 *
 *	event count - if non-zero, the current event is valid.
 *	flag to indicate if the call is from the mainline (main_loop)
 *
 *  Outputs:
 *
 *	None
 *
 */
int
handle_events(int event_count, int from_mainline)

{
  XEvent peek_event;
  int x, y, i, window_found;
  int first_time = event_count;

  while (1)
    {

      lcl.processing_event = FALSE;

     /*
      *  Get the event count...
      *
      */
      event_count = XEventsQueued(state.display, QueuedAfterFlush);
      if ((event_count <= 0) && (!first_time)) return(TRUE);

     /*
      *  Now read the next event...
      *
      */
      if (!first_time)
        XNextEvent(state.display, state.event);
      else
         first_time = FALSE;

     /*
      *  Decrement the pending count...
      *
      */
      event_count -= 1;

     /*
      *  Tell any called routines that the call was done while processing an
      *  event...  just in case any are interested...
      *
      */
      lcl.processing_event = TRUE;
      window_found = 0;

      for (i = 0; i < my_windows.last; i += 1)
        {  
          if (state.event->xany.window == my_windows.list[i].window)
            {
              (*my_windows.list[i].event_rtn)(state.event, from_mainline);
              window_found = 1;
              break;
            }
        }

    } /* End the while loop */
} /* End routine */

/*
 *  Routine:
 *
 *	highlight_colors_writeable
 *
 *  Description:
 *
 *	If the colormap is writable, this routine will change the RGB
 *	value of a set of (or a single) pixel.  This is used to highlight
 *
 *  Inputs:
 *
 *	xxxxx
 *
 *  Outputs:
 *
 *	state ON/OFF (0, 1)
 *
 */
int
highlight_colors_writeable( int on_off, int is_on,
                            int prev_address, int next_address, int width)
{
  int h, i, j, k = 0, prev_pixel;
  float xform;

  if (state.debugging)
    printf("highlight_colors_writable\n");

 /*
  *  First build the color array to restore anything that had been
  *  turned on...
  *
  */
  if (is_on)
    {

     /*
      *  Loop for the width of the band
      *
      */
      for (j = prev_address; j < prev_address+width; j += 1)
        {

         /*
          *  A little tricky, we need to find the XColor structure for
	  *  the pixel at this location.  We can find the pixel right
	  *  out of the pixel map.
	  *
	  */
	  if ((j >= 0) && (j <= MAX_PIXEL))
	    {

	      prev_pixel = state.pixel_map[j];

	      for (h = 0; h < state.number_of_colors; h += 1)
		{
		  if (prev_pixel == lcl.colors[h].pixel)
		    {
		      lcl.highlight_band[k].pixel = lcl.colors[h].pixel;
		      lcl.highlight_band[k].flags = DoRed | DoGreen | DoBlue;
		      lcl.highlight_band[k].red   = lcl.colors[h].red;
		      lcl.highlight_band[k].green = lcl.colors[h].green;
		      lcl.highlight_band[k].blue  = lcl.colors[h].blue;
		      k += 1; /* One color added */
		    } /* End pixel match */
		} /* End loop through pixel array */
	    } /* End test to make sure value is in bounds */
        } /* End loop for width */
    } /* End WAS ON */

  if (on_off)
    {
      xform = 65535.0 / lcl.highlight_width;

      j = next_address;

      for (i = 0; i < width; i += 1)
        {
	  if ((j >= 0) && (j <= MAX_PIXEL))
	    {

	      lcl.highlight_band[k].pixel = state.pixel_map[j];

              lcl.highlight_band[k].flags = DoRed | DoGreen | DoBlue;

	      lcl.highlight_band[k].red   = 0;
	      lcl.highlight_band[k].blue  = 0;
              lcl.highlight_band[k].green = 0;

	      if (lcl.highlight_green)
                lcl.highlight_band[k].green = (i+1) * xform;

	      if (lcl.highlight_blue)
	        lcl.highlight_band[k].blue  = (i+1) * xform;

	      if (lcl.highlight_red)
	        lcl.highlight_band[k].red   = (i+1) * xform;

	      k += 1;
            } /* End Pixel in range */

	  j += 1;
        } /* End Loop for width */
    }  /* End turn it ON */

  if (k)
    XStoreColors(state.display, state.colormap, &lcl.highlight_band[0], k);

  XFlush(state.display);

  return (on_off);
}

/*
 *  Routine:
 *
 *	display_small_image_window
 *
 *  Description:
 *
 *	Draw the small image into the small image window
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	None
 *
 */
void
display_small_image_window()

{
  if (state.debugging)
    printf("display_small_image_window\n");

    XPutImage (state.display, lcl.small_image_window, state.small_image_gc,
	state.small_ximage, 0, 0, 0, 0, 200, 200);
    XFlush(state.display);
}

/*
 *  Routine:
 *
 *	display_small_image_scanline
 *
 *  Description:
 *
 *	Draw one or more scanlines of the small image into the small image
 *	window.
 *
 *  Inputs:
 *
 *	scanline and count
 *
 *  Outputs:
 *
 *	None
 *
 */
void
display_small_image_scanline(int scan, int count)

{
    XPutImage (state.display, lcl.small_image_window, state.small_image_gc,
	state.small_ximage, 0, scan, 0, scan, 200, count);
    XFlush(state.display);
}

/*
 *  Routine:
 *
 *	display_select_window
 *
 *  Description:
 *
 *	Draw the selection window
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	None
 *
 */
void
display_select_window()

{
    int i, j = lcl.select_offset;
    XClearWindow(state.display, lcl.select_window);

  if (state.debugging)
    printf("display_select_window\n");

    for (i = 16; i <= 256; i += 16)
      {
        XDrawString(state.display, lcl.select_window, lcl.select_gc, 5, i-4,
		    main_menu[j].select_command, strlen(main_menu[j].select_command));
        XDrawLine(state.display, lcl.select_window, lcl.select_gc, 0, i, 150, i);
	j += 1;
      }

    lcl.select_inverted = 0;
    lcl.current_select  = 0;

    XFlush(state.display);
  }

/*
 *  Routine:
 *
 *	display_image
 *
 *  Description:
 *
 *	Displays all or part of the image, from the Y position
 *	specified to the ending scanline provided.
 *
 *  Inputs:
 *
 *	starting scanline value
 *	ending scanline value
 *
 *  Outputs:
 *
 *	none
 *
 */
void
display_image(int y, int high)
  {

    if (state.debugging)
      printf("display_image\n");

    state.main_ximage->data = (char *) state.current_image;
    XPutImage (state.display, state.image_window, state.image_gc,
	state.main_ximage, 0, y, 0, y, state.samples_per_line, high);
    XFlush(state.display);
  }

/*
 *  Routine:
 *
 *  display_image_histogram
 *
 *  description:
 *
 *	Draw the histogram window contents.  The histogram in this case is
 *	the distribution of pixel intensities.
 *
 *  inputs:
 *
 *	file information block
 *
 *  outputs:
 *
 *	none
 *
 */
void
display_image_histogram(FILEINFO *fin)

{
    int i, j, k;

  if (state.debugging)
    printf("display_image_histogram\n");

  if (fin == 0) return;

  XClearWindow(state.display, lcl.image_hist_window);

    for (i = 0; i < MAX_VALUES; i += 1)
      {
       /*
        *  Draw the histogram
        *
        */
	k = lcl.brightness_level + i;

        if ((k > 0) && (k < MAX_VALUES))
	  {
            j = fin->image_hist[i] * lcl.pix_per_count;
            XDrawLine(state.display, lcl.image_hist_window, lcl.hist_gc, 0, k, j, k);
	  }

       /*
        *  Insert a band showing the equalized area
        *
        */
        if (i == lcl.black_limit)
            XDrawLine(state.display, lcl.image_hist_window, lcl.hist_gc, EQ_X1, i, EQ_X2, i);
        else if (i == lcl.white_limit)
            XDrawLine(state.display, lcl.image_hist_window, lcl.hist_gc, EQ_X1, i, EQ_X2, i);
        else if ((i > lcl.black_limit) && (i < lcl.white_limit))
            XDrawPoint(state.display, lcl.image_hist_window, lcl.hist_gc, EQ_X, i);
      }

    XFlush(state.display);
}

/*
 *  Routine:
 *
 *	display_image_colors
 *
 *  description:
 *
 *	Draw the window that is the grayscale ramp for the colors displayed.
 *
 *  inputs:
 *
 *	file information block
 *
 *  outputs:
 *
 *	none
 *
 */
void
display_image_colors(FILEINFO *fin)

{
    XGCValues xgcv;
    int i, j;
    float xform;

  if (state.debugging)
    printf("display_image_colors\n");

    if (!lcl.show_intensity_window) return;

    XClearWindow(state.display, lcl.intensity_window);

    for (i = 0; i < MAX_VALUES; i += 1)
      {
        xgcv.foreground = state.pixel_map[i];
        xgcv.background = state.pixel_map[i];
        xgcv.function = GXcopy;
        XChangeGC(state.display, lcl.color_gc, GCFunction | GCForeground | GCBackground, &xgcv);
        XDrawLine(state.display, lcl.intensity_window, lcl.color_gc, 0, i, INTENSITY_WINDOW_WIDTH-1, i);
      }

    if (lcl.show_pixel_intensity)
      {
        xgcv.foreground = state.pixel_map[MAX_PIXEL];
        xgcv.background = state.pixel_map[MAX_PIXEL];
        xgcv.function = GXcopy;
        XChangeGC(state.display, lcl.color_gc, GCForeground | GCBackground | GCFunction, &xgcv);

        xform = ((INTENSITY_WINDOW_WIDTH-1) * 1.0) / ((state.number_of_colors-1) * 1.0);

        for (i = lcl.black_limit; i <= lcl.white_limit; i += 1)
          {
            j = (xform * state.pixel_intensities[i]) + 0.5;
            XDrawPoint(state.display, lcl.intensity_window, lcl.color_gc, j, i);
          }
      }

    XFlush(state.display);
}

/*
 *  Routine:
 *
 *	display_image_text
 *
 *  description:
 *
 *	Display all of the information about the image, and the current
 *	image parameters.
 *
 *  inputs:
 *
 *	none
 *
 *  outputs:
 *
 *	none
 *
 */
void
display_image_text()

{
    int i, j, x1, x2, y1, y2;

  if (state.debugging)
    printf("display_image_text\n");

    XClearWindow(state.display, lcl.text_window);

    x1 = 0;
    x2 = x1 + 100;
    y1 = 13;
    y2 = 13;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Target Name", strlen("Target Name"));
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.target_name[0], strlen(&txt.target_name[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Spacecraft", strlen("Spacecraft"));
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.spacecraft_name[0], strlen(&txt.spacecraft_name[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Image ID", strlen("Image ID"));
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.image_id[0], strlen(&txt.image_id[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Image Number", strlen("Image Number"));
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.image_number[0], strlen(&txt.image_number[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Filter Name", strlen("Filter Name"));
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.filter_name[0], strlen(&txt.filter_name[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Size [X, Y]", strlen("Size [X, Y]"));
    sprintf(txt.work_buffer, "[%d, %d]", state.samples_per_line,state.num_scanlines);
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Color Map", strlen("Color Map"));

    if (state.need_colormap)
      {
	if (lcl.writeable_map)
          sprintf(txt.work_buffer, "Private %d RW",state.number_of_colors);
        else
          sprintf(txt.work_buffer, "Private %d RO",state.number_of_colors);
      }
    else
      {
	if (lcl.writeable_map)
          sprintf(txt.work_buffer, "Default %d RW",state.number_of_colors);
        else
          sprintf(txt.work_buffer, "Default %d RO",state.number_of_colors);
      }

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Equalization", strlen("Equalization"));

    if (lcl.auto_equal)
      sprintf(txt.work_buffer, "AUTO");
    else
      {
        if ((lcl.black_limit == 0) && (lcl.white_limit == MAX_PIXEL))
          sprintf(txt.work_buffer, "OFF");
	else
          sprintf(txt.work_buffer, "MANUAL");
      }

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

    if (lcl.auto_equal)
      {

        y1 += y2;
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"-threshold", strlen("-threshold"));
        sprintf(txt.work_buffer, "%*.*f", 4, 4, lcl.equalization_threshold);
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

        y1 += y2;
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"-black adjust", strlen("-black adjust"));
        sprintf(txt.work_buffer, "%d", lcl.black_fudge);
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));
        y1 += y2;

        XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"-white adjust", strlen("-white adjust"));
        sprintf(txt.work_buffer, "%d", lcl.white_fudge);
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

      }

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Black Limit", strlen("Black Limit"));
    sprintf(txt.work_buffer, "%d", lcl.black_limit);
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"White Limit", strlen("White Limit"));
    sprintf(txt.work_buffer, "%d", lcl.white_limit);
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Brightness", strlen("Brightness"));
    sprintf(txt.work_buffer, "%d", lcl.brightness_level);
    XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

    if (lcl.enable_gamma)
      {
        y1 += y2;

        XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Gamma", strlen("Gamma"));
        sprintf(txt.work_buffer, "%*.*f", 4, 4, lcl.gamma);
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));
      }


    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
	"Calibration", strlen("Calibration"));

    if (lcl.current_file->image_calibrated)
      XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
		"DONE", strlen("DONE"));
    else
      if (lcl.calibrate_image_data)
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
		"ENABLED", strlen("ENABLED"));
    else
      XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
		"DISABLED", strlen("DISABLED"));

    y1 += y2;

    XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"Zoom", strlen("Zoom"));

    if (lcl.zoom_displayed)
      {
        sprintf(txt.work_buffer, "DISPLAYED");
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

        y1 += y2;

        XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"-top left", strlen("-top left"));
        sprintf(txt.work_buffer, "[%d,%d]", lcl.zoom_upper_left[0], lcl.zoom_upper_left[1]);
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));

        y1 += y2;

        XDrawString(state.display, lcl.text_window, lcl.text_gc, x1, y1,
		"-bottom right", strlen("-bottom right"));
        sprintf(txt.work_buffer, "[%d,%d]", lcl.zoom_lower_right[0], lcl.zoom_lower_right[1]);
        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));
      }
    else
      {
        if (lcl.zoom_mode == 1)
          sprintf(txt.work_buffer, "Need Corner 1");
        else if (lcl.zoom_mode == 2)
          sprintf(txt.work_buffer, "Need Corner 2");
        else
          sprintf(txt.work_buffer, "OFF");

        XDrawString(state.display, lcl.text_window, lcl.text_gc, x2, y1,
			&txt.work_buffer[0], strlen(&txt.work_buffer[0]));
      }

    XFlush(state.display);
}

/*
 *  Routine:
 *
 *	display_image_status
 *
 *  Description:
 *
 *	Display the status lines at the bottom of the display
 *
 *  inputs:
 *
 *	none
 *
 *  outputs:
 *
 *	none
 *
 */
void
display_image_status()

{
    int i, j, x1, x2, y1, y2;

  if (state.debugging)
    printf("display_image_status\n");

    XClearWindow(state.display, lcl.status_window);

    x1 = 5;
    x2 = x1 + 120;
    y1 = 13;
    y2 = 32;

    XDrawString(state.display, lcl.status_window, lcl.status_gc, x1, y1,
		"State:", strlen("State:"));

    XDrawImageString(state.display, lcl.status_window, lcl.statusR_gc, x2, y1,
		txt.status_text, strlen(txt.status_text));

    y1 = y2;

    XDrawString(state.display, lcl.status_window, lcl.status_gc, x1, y1,
		"Image Note:", strlen("Image Note:"));

    XDrawString(state.display, lcl.status_window, lcl.status_gc, x2, y1,
		&txt.note_data[0], strlen(&txt.note_data[0]));

    XFlush(state.display);
}

/*
 *  Routine:
 *
 *	quit_ximq
 *
 *  Description:
 *
 *	Exits the application by setting the run flag to 0.
 *
 *  Inputs:
 *
 *	None
 *
 *  Outputs:
 *
 *	None
 *
 */
void
quit_ximq()
{
 /*
  *  Simply allow the application to exit in peace
  *
  */

  if (state.debugging)
    printf("quit_ximq\n");

  lcl.app_running = 0;
}

/*
 *  Routine:
 *
 *	new_image
 *
 *  Description:
 *
 *	Force a close of the current image, and get a new image.
 *
 *  Inputs:
 *
 *	file information block
 *
 *  Outputs:
 *
 *	none
 *
 */
void
new_image()
{

  int status = 0;

 /*
  *  Close any open files and call the process input logic.  This will
  *  get a file, open and do all the other nice things that must be done...
  *
  */
  if (state.debugging)
    printf("new_image\n");

  if (state.manual_file_select)
    {
      printf("XIMQ was started using the -file option, no index is available\n");
      return;
    }

  reset_image(lcl.current_file);

  while (!status)
    {
      close_file(lcl.current_file);
      lcl.current_file->filename_valid = 0;
      status = process_image(lcl.current_file, TRUE);
    }
}

/*
 *  Routine:
 *
 *	equalize
 *
 *  Description:
 *
 *	Enable equalization, and recompute the image
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
equalize()
{

  if (state.debugging)
    printf("equalize\n");

 /*
  *  Reset any and all controls to the program defaults...
  *
  */
  lcl.black_limit      = BLACK_LIMIT_DEFAULT;
  lcl.white_limit      = WHITE_LIMIT_DEFAULT;
  lcl.auto_equal       = AUTO_EQUAL_DEFAULT;
  lcl.black_fudge      = BLACK_FUDGE_DEFAULT;
  lcl.white_fudge      = WHITE_FUDGE_DEFAULT;
  lcl.brightness_level = BRIGHTNESS_DEFAULT;

 /*
  *  Need to recompute the histogram information
  *
  */
  recompute_output(state.num_outlines, lcl.pixel_twiddle);
}

/*
 *  Routine:
 *
 *	reset_image_variables
 *
 *  Description:
 *
 *	Reset all of the controls to their initial values, and
 *	recompute the output image.
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
reset_image_variables()
{
  
  if (state.debugging)
    printf("reset\n");

 /*
  *  Reset the image buffer pointers to the originals...
  *
  */  
  state.current_image   = state.image_buffer1;
  state.original_image  = &lcl.current_file->image_buffer[0];

 /*
  *  Reset any and all controls to the program defaults...
  *
  */
  lcl.black_limit      = BLACK_LIMIT_DEFAULT;
  lcl.white_limit      = WHITE_LIMIT_DEFAULT;
  lcl.auto_equal       = AUTO_EQUAL_DEFAULT;
  lcl.black_fudge      = BLACK_FUDGE_DEFAULT;
  lcl.white_fudge      = WHITE_FUDGE_DEFAULT;
  lcl.brightness_level = BRIGHTNESS_DEFAULT;
  lcl.gamma            = 1.20;

 /*
  *  Turn off zoom
  *
  */
  lcl.zoom_mode = 0;
  lcl.zoom_displayed = 0;
  state.zoom_buffer = state.zoom_buffer1;

  recompute_output(state.num_outlines, TRUE);
}

/*
 *  Routine:
 *
 *	noequal
 *
 *  Description:
 *
 *	Force flat image display ramp with no histogram equalization
 *	and recompute the image
 *
 *  Inputs:
 *
 *	none
 *
 *  outputs:
 *
 *	none
 *
 */
void
noequal()
{

  if (state.debugging)
    printf("noequal\n");

 /*
  *  Reset any and all controls to neutral...
  *
  */
  lcl.black_limit      = 0;
  lcl.white_limit      = MAX_PIXEL;
  lcl.auto_equal       = 0;
  lcl.black_fudge      = 0;
  lcl.white_fudge      = 0;
  lcl.brightness_level = 0;

 /*
  *  Need to recompute the histogram information
  *
  */
  recompute_output(state.num_outlines, lcl.pixel_twiddle);
}

/*
 *  Routine:
 *
 *	start_zoom
 *
 *  Description:
 *
 *	Start the zoom process
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
start_zoom()
{

  if (state.debugging)
    printf("zoom\n");

  if (lcl.zoom_mode)
    {
     /*
      *  Already in zoom mode, reset to start of zoom
      *
      */
      if (lcl.zoom_mode > 1)
        {
         /*
          *  Zoom mode was past initial stage, undraw the rubberband
          *  box and set the mode to 1
          *
          */
          undraw_zoom_rubberband();
          lcl.zoom_mode = 1;
        }
    }
  else
    {
     /*
      *  Enable zoom mode...
      *
      */
      lcl.zoom_mode = 1;
    }

  display_image_text();

}

/*
 *  Routine:
 *
 *	unzoom
 *
 *  Description:
 *
 *	Unzoom the image
 *
 *  Inputs:
 *
 *	none
 *
 *  outputs:
 *
 *	none
 *
 */
void
unzoom()
{
  
  if (state.debugging)
    printf("unzoom\n");

 /*
  *  Reset the image buffer pointers to the originals...
  *
  */  
  state.current_image   = state.image_buffer1;
  state.original_image  = &lcl.current_file->image_buffer[0];

 /*
  *  Turn off zoom
  *
  */
  lcl.zoom_mode = 0;
  lcl.zoom_displayed = 0;
  state.zoom_buffer = state.zoom_buffer1;

  recompute_output(state.num_outlines, TRUE);
}

/*
 *  Routine:
 *
 *	gamma_darken
 *
 *  Description:
 *
 *	Apply a gamma correction to darken the image.
 *
 *  Inputs:
 *
 *	none
 *
 *  outputs:
 *
 *	none
 *
 */
void
gamma_darken()
{

  if (state.debugging)
    printf("gamma_darken\n");

  lcl.gamma += lcl.gamma_shift;
  recompute_output(state.num_outlines, lcl.pixel_twiddle);

}

/*
 *  Routine:
 *
 *	gamma_lighten
 *
 *  Description:
 *
 *	Apply a gamma correction to lighten the image
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
gamma_lighten()
{

  if (state.debugging)
    printf("gamma_lighten\n");

  if (lcl.gamma > lcl.gamma_shift) 
    lcl.gamma -= lcl.gamma_shift;

  recompute_output(state.num_outlines, lcl.pixel_twiddle);
}

/*
 *  Routine:
 *
 *	recompute_output
 *
 *  Description:
 *
 *	Recompute the image output pixels and display it
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
recompute_output(int num_to_display, int redraw)
{

  unsigned char *image_p,
       *image_pointer, *original_pointer;
  int line, j = 0;

  if (state.debugging)
    printf("recompute_output\n");

  image_pointer    = state.current_image;
  original_pointer = state.original_image;

  get_image_histogram(lcl.current_file);

  (*lcl.gamma_routine)();

 /*
  *  Get a new pixel map based on the cleared state
  *
  */
  (*lcl.pixel_map_routine)();

 /*
  *  Redisplay the data, note that the image histogram call also
  *  computes hitogram related equalization.
  *
  */
  display_image_histogram(lcl.current_file);
  display_image_colors(lcl.current_file);
  display_select_window();
  display_image_text(lcl.current_file);
  display_image_status();

  if (redraw || (!lcl.pixel_twiddle))
    {
      image_p = image_pointer;

     /*
      *  Now, take the unadulterated uncompressed image, and
      *  reformat and output it...
      *
      */
      for (line = 0; line < state.num_scanlines; line += 1)
        {
          reformat_image_data(original_pointer, image_pointer);
          j += 1;
  
          if (j == num_to_display)
            {
              display_scanline(line - j, image_p, j);
              image_p = image_pointer;
              j = 0;
            }

          original_pointer += state.samples_per_line;
        }

      if (j && num_to_display)
        {
          display_scanline(line - j, image_p, j);
        }
    }
}

/*
 *  Routine:
 *
 *	brighten
 *
 *  Description:
 *
 *	Does a linear shift of the pixel intensities
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
brighten()
{

  if (state.debugging)
    printf("brighten\n");

  lcl.brightness_level += lcl.bright_shift;
  recompute_output(state.num_outlines, lcl.pixel_twiddle);

}

/*
 *  Routine:
 *
 *	darken
 *
 *  Description:
 *
 *	Does a linear shift of the pixels in intensity
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
darken()
{
  if (state.debugging)
    printf("darken\n");

  lcl.brightness_level -= lcl.bright_shift;
  recompute_output(state.num_outlines, lcl.pixel_twiddle);
}

/*
 *  Routine:
 *
 *	apply_median_filter
 *
 *  Description:
 *
 *	Sorts the pixels in a 3x3 matrix and replaces the center pixel with
 *	the median pixel.
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
apply_median_filter()
{
  unsigned char *orig_buffer;

  if (state.debugging)
    printf("apply_median_filter\n");

 /*
  *  Get the destination original buffer...
  *
  */
  if (state.original_image == state.original_image_buffer1)
    orig_buffer = state.original_image_buffer2;
  else
    orig_buffer = state.original_image_buffer1;

 /*
  *  Call the filter routine
  *
  *
  */
  median_filter(state.original_image, orig_buffer, state.current_image, state.num_outlines);

 /*
  *  Swap original data buffer...
  *
  */
  if (state.original_image == state.original_image_buffer1)
    state.original_image = state.original_image_buffer2;
  else
    state.original_image = state.original_image_buffer1;
}

/*
 *  Routine:
 *
 *	sharpen
 *
 *  Description:
 *
 *	Applies a Laplacian filter as a convolution to enhance edges.
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
sharpen_image()
{
  unsigned char *orig_buffer;

 /*
  *  L A P L A C E   F I L T E R
  *
  *  This filter will amplify edge (intensity transition) data
  *
  */
  float image_filter_f[] = {
    -1.0, -1.0, -1.0,
    -1.0,  9.0, -1.0,
    -1.0, -1.0, -1.0};

  int image_filter_i[] = {
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1};

  if (state.debugging)
    printf("sharpen_image\n");

 /*
  *  Get the destination original buffer...
  *
  */
  if (state.original_image == state.original_image_buffer1)
    orig_buffer = state.original_image_buffer2;
  else
    orig_buffer = state.original_image_buffer1;

 /*
  *  Convolve the image filter, divide the sum by 1, clamp the output and
  *  display it - 2 lines at a crack
  *
  */
  if (lcl.convolve_float)
    convolve_image_3x3(state.original_image, orig_buffer, state.current_image, &image_filter_f[0], 0, TRUE, state.num_outlines);
  else
    convolve_image_3x3_int(state.original_image, orig_buffer, state.current_image, &image_filter_i[0], 0, TRUE, state.num_outlines);

 /*
  *  Swap original data buffer...
  *
  */
  if (state.original_image == state.original_image_buffer1)
    state.original_image = state.original_image_buffer2;
  else
    state.original_image = state.original_image_buffer1;

}

/*
 *  Routine:
 *
 *	smooth
 *
 *  Description:
 *
 *	Sums the pixels in a 3x3 matrix and divides by 9 to get the average
 *	intensity.  The center pixel is then replaced by the average.
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
smooth()
{

  unsigned char *orig_buffer;

 /*
  *  This filter creates the sum of all 9 cells, which is then divided
  *  by 9 to get the average pixel intensity in the area.
  *
  */
  float image_filter_f[] = {
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0};

  int image_filter_i[] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1};

  if (state.debugging)
    printf("command_smooth_image\n");

 /*
  *  Get the destination original buffer...
  *
  */
  if (state.original_image == state.original_image_buffer1)
    orig_buffer = state.original_image_buffer2;
  else
    orig_buffer = state.original_image_buffer1;

 /*
  *  Convolve the image filter, divide the sum by 9, clamp the output and
  *  display it - 2 lines at a crack
  *
  */
  
  if (lcl.convolve_float)
    convolve_image_3x3(state.original_image, orig_buffer, state.current_image, &image_filter_f[0], 9, TRUE, state.num_outlines);
  else
    convolve_image_3x3_int(state.original_image, orig_buffer, state.current_image, &image_filter_i[0], 9, TRUE, state.num_outlines);

 /*
  *  Swap original data buffer...
  *
  */
  if (state.original_image == state.original_image_buffer1)
    state.original_image = state.original_image_buffer2;
  else
    state.original_image = state.original_image_buffer1;
}

/*
 *  Routine:
 *
 *	nada
 *
 *  Description:
 *
 *	A noop routine
 *
 *  Inputs:
 *
 *	none
 *
 *  outputs:
 *
 *	none
 *
 */
void
nada()
{
  if (state.debugging)
    printf("nada\n");
}

/*
 *  Routine:
 *
 *	calibrate
 *
 *  Description:
 *
 *	Enable image calibration.  This is done by subtracting a
 *	calibration image (the noise in the camera when looking at
 *	nothing) from the image.  This routine just enables it.
 *
 *  Inputs:
 *
 *	none
 *
 *  outputs:
 *
 *	none
 *
 */
void
calibrate()
{
  unsigned char *orig_buffer;

  if (state.debugging)
    printf("command_calibrate\n");

  lcl.calibrate_image_data = 1;

}

/*
 *  Routine:
 *
 *	uncalibrate
 *
 *  Description:
 *
 *	Disable calibration
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
uncalibrate()
{
  if (state.debugging)
    printf("uncalibrate\n");

  lcl.calibrate_image_data = 0;

}

/*
 *  Routine:
 *
 *	next_menu
 *
 *  Description:
 *
 *	Display next menu
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
next_menu()
{
  if (state.debugging)
    printf("next_menu\n");

  if (lcl.select_offset < lcl.select_max) lcl.select_offset += 16;
  display_select_window();

}

/*
 *  Routine:
 *
 *	prev_menu
 *
 *  Description:
 *
 *	Display previous menu
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
prev_menu()
{
  if (state.debugging)
    printf("prev\n");

  if (lcl.select_offset) lcl.select_offset -= 16;
  display_select_window();
}

/*
 *  Routine:
 *
 *	refresh_display
 *
 *  Description:
 *
 *	Clear the image and redraw the windows.
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
void
refresh_display()
{

  if (state.debugging)
    printf("refresh_display\n");

 /*
  *  Clear the image and repaint it, as well as the
  *  other windows.  This really doesn't do much useful, but
  *  it's a nice security blanket.
  *
  */
  XClearWindow(state.display, state.image_window);
  recompute_output(state.num_outlines, TRUE);
}

/*
 *  Routine:
 *
 *	undraw_zoom_rubberband
 *
 *  Description:
 *
 *	Remove the zoom rubberband
 *
 *  Inputs:
 *
 *	none
 *
 *  Outputs:
 *
 *	none
 *
 */
int
undraw_zoom_rubberband()
{
  if (lcl.zoom_rubber_band_on)
    {

      XDrawLines(state.display, state.image_window, lcl.zoom_rubber_gc, &lcl.zoom_rubber_box[0], 5,
                 CoordModeOrigin);

      lcl.zoom_rubber_band_on = 0;
      return TRUE;
    }
  return FALSE;
}

/*
 *  Routine:
 *
 *	draw_zoom_rubberband
 *
 *  Description:
 *
 *	Draws the zoom rubberband
 *
 *  Inputs:
 *
 *	lcl.zoom_rubber_band_on indication
 *
 *  Outputs:
 *
 *	1 if drawn, 0 if already was drawn
 *
 */
int
draw_zoom_rubberband()
{
  if (!lcl.zoom_rubber_band_on)
    {

      XDrawLines(state.display, state.image_window, lcl.zoom_rubber_gc, &lcl.zoom_rubber_box[0], 5,
                 CoordModeOrigin);

      lcl.zoom_rubber_band_on = 1;
      return TRUE;
    }
  return FALSE;
}

/*
 *  Routine:
 *
 *	zoom_image
 *
 *  Description:
 *
 *	Zoom an image.  There are several ways to do this from nearest
 *	neighbor, bilinear approximation to pixel replication.
 *
 *  Inputs:
 *
 *	The box to zoom, the aspect ratio, and the zoom style.
 *
 *  Outputs:
 *
 *	The area in the box is expanded to the full image size.
 *
 */
void
zoom_image(int x1, int y1, int x2, int y2, int aspect, int style_of_zoom)

{
  int lines, width;
  unsigned char *orig_buffer, *zb;

  if (state.debugging)
    printf("zoom_image\n");

  txt.status_text = "Zooming image...";
  display_image_status();

 /*
  *  Get the destination original buffer...
  *
  */
  if (state.original_image == state.original_image_buffer1)
    orig_buffer = state.original_image_buffer2;
  else
    orig_buffer = state.original_image_buffer1;

 /*
  *  Swap zoom buffers...
  *
  */
  if (state.zoom_buffer == state.zoom_buffer1)
    zb = state.zoom_buffer2;
  else
    zb = state.zoom_buffer1;

 /*
  *  Clear the buffer
  *
  */
  {
    unsigned int *cl, nn, nnn;
    cl = (unsigned int *) orig_buffer;
    nnn  = (state.samples_per_line * state.num_scanlines) / 4;
    for (nn = 0; nn < nnn; nn += 1)
      *cl++ = 0;
  }

  lines = (y2 - y1) + 1;
  width = (x2 - x1) + 1;

 /*
  *  Make the aspect ratio square if required...
  *
  */
  if (aspect == 1)
    {
     /*
      *  Use the larger of the two
      *
      */
      if (lines > width) width = lines;
        else lines = width;

     /*
      *  Make sure it fits in X
      *
      */
      if (width > state.samples_per_line)
	{
	  width = state.samples_per_line;
	  lines = state.samples_per_line;
	}

     /*
      *  Make sure it fits in y
      *
      */
      if (lines > state.num_scanlines)
	{
	  width = state.num_scanlines;
	  lines = state.num_scanlines;
	}

     /*
      *  Slide the origin if we need to fit the square...
      *
      */
      if ((x1 + width) >= state.samples_per_line)
	x1 = state.samples_per_line - width;

      if ((y1 + lines) >= state.num_scanlines)
	y1 = state.num_scanlines - lines;
    }

  if (style_of_zoom == 0)
    {
      pixel_replicate_zoom(state.original_image, orig_buffer, zb,
			    x1, y1, width, lines, state.num_outlines);
    }
  else if (style_of_zoom == 1)
    {
      nearest_neighbor_zoom(state.original_image, orig_buffer, zb,
			    x1, y1, width, lines, state.num_outlines);
    }
  else
    {
      int filter[] = {1, 1, 1,
                      1, 1, 1,
                      1, 1, 1};

      matrix_3x3_zoom(state.original_image, orig_buffer, zb,
			    x1, y1, width, lines, state.num_outlines,
			    &filter[0], 9, TRUE);
    }

  state.current_image = zb;

 /*
  *  Swap original data buffer...
  *
  */
  state.original_image = orig_buffer;

 /*
  *  Swap zoom buffers...
  *
  */
  state.zoom_buffer = zb;

 /*
  *  Now displaying an zoomed image
  *
  */
  lcl.zoom_displayed = 1;
}

/*
 *  Routine:
 *
 *	parse_command_line
 *
 *  Description:
 *
 *	Parse the command line
 *
 *  Inputs:
 *
 *	argv and argc
 *
 *  Outputs:
 *
 *	FALSE if error, TRUE if success
 *
 */
int
parse_command_line(int argc, char *argv[])

{
  int i;

  if (state.debugging)
    printf("parse_command_line\n");

  if (argc <= 0) return(TRUE);

  for (i = 1; i < argc; i += 1)
    {

     /*
      *  Specify the geometry of the window?
      *  
      */
      if (strncmp(argv[i], "-g", 2) == 0 )
	{
	  if ((argc-i) < 4)
            {
              usage(argc,argv,i,4);
              return(FALSE);
            }
	  i += 1;
          lcl.xs = ((atoi(argv[i]) + 3) & ~3);
	  i += 1;
          lcl.ys = ((atoi(argv[i]) + 3) & ~3);
	  i += 1;
          lcl.ws = ((atoi(argv[i]) + 3) & ~3);
	  i += 1;
          lcl.hs = ((atoi(argv[i]) + 3) & ~3);
          position_fixed = 1;
	}

     /*
      *  Specify the workstation display to output to?
      *  
      */
      else if (strncmp(argv[i], "-disp",5) == 0 )
	{
	  if ((argc-i) < 1)
            {
              usage(argc,argv,i,1);
              return(FALSE);
            }
	  i += 1;
	  lcl.display_name = argv[i];
	}

     /*
      *  Specify the filename to read from?
      *  
      */
      else if (strncmp(argv[i], "-file",5) == 0 )
	{
	  if ((argc-i) < 1)
            {
              usage(argc,argv,i,1);
              return(FALSE);
            }
	  i += 1;
	  strncpy((char *) &lcl.base_image_file->input_filename[0], argv[i], 256);
          lcl.base_image_file->filename_valid  = 1;
	  state.manual_file_select     = TRUE;
	  state.use_file_select_window = FALSE;
	  state.use_index  = FALSE;
	  state.use_volume = FALSE;
	}

     /*
      *  Specify the volume file to read from?
      *  
      */
      else if (strncmp(argv[i], "-vol",4) == 0 )
	{
	  if ((argc-i) < 1)
            {
              usage(argc,argv,i,1);
              return(FALSE);
            }
	  i += 1;
	  state.volume_filename = (unsigned char *) argv[i];
	  state.use_volume = TRUE;
	}

     /*
      *  Specify the index to read from?
      *  
      */
      else if (strncmp(argv[i], "-ind",4) == 0 )
	{
	  if ((argc-i) < 1)
            {
              usage(argc,argv,i,1);
              return(FALSE);
            }

	  i += 1;
	  state.index_filename = (unsigned char *) argv[i];
	  state.use_index = TRUE;
	}

     /*
      *  A window title?
      *  
      */
      else if (strncmp(argv[i], "-tit",4) == 0 )
	{
	  if ((argc-i) < 1)
            {
              usage(argc,argv,i,1);
              return(FALSE);
            }
	  i += 1;
	  lcl.window_name = argv[i];
	}

     /*
      *  A user icon title?
      *  
      */
      else if (strncmp(argv[i], "-icon",5) == 0 )
	{
	  if ((argc-i) < 1)
            {
              usage(argc,argv,i,1);
              return(FALSE);
            }
	  i += 1;
	  lcl.icon_name = argv[i];
	}

     /*
      *  Set private colormap
      *  
      */
      else if (strncmp(argv[i], "-cmap",5) == 0 )
	{
	  lcl.private_map = TRUE;
	}

     /*
      *  Set color count...
      *  
      */
      else if (strncmp(argv[i], "-col",4) == 0 )
	{
	  if ((argc-i) < 1)
            {
              usage(argc,argv,i,1);
              return(FALSE);
            }

	  i += 1;
	  state.number_of_colors = atoi(argv[i]);
	}

     /*
      *  Set intensity shift, and increment...
      *  
      */
      else if (strncmp(argv[i], "-sh",3) == 0 )
	{
	  if ((argc-i) < 2)
            {
              usage(argc,argv,i,2);
              return(FALSE);
            }

	  i += 1;
	  state.number_of_colors = atoi(argv[i]);
	  i += 1;
          lcl.brightness_level = atoi(argv[i]);

	  i += 1;
          lcl.bright_shift = atoi(argv[i]);

 	}

     /*
      *  Set zoom style...
      *  
      */
      else if (strncmp(argv[i], "-zoom",5) == 0 )
	{
	  if ((argc-i) < 1)
            {
              usage(argc,argv,i,1);
              return(FALSE);
            }

	  i += 1;
          lcl.zoom_style = atoi(argv[i]);
	}

     /*
      *  Set hig/low thresholds... this is a manual histogram
      *  equalization, so it by default turns off automatic
      *  histogram equalization.
      *  
      */
      else if (strncmp(argv[i], "-lim",4) == 0 )
	{
	  if ((argc-i) < 2)
            {
              usage(argc,argv,i,2);
              return(FALSE);
            }

	  i += 1;
          lcl.black_limit = atoi(argv[i]);
	  i += 1;
          lcl.white_limit = atoi(argv[i]);
          lcl.auto_equal = FALSE;
	}

     /*
      *  Set shared color?
      *  
      */
      else if (strncmp(argv[i], "-stat",5) == 0 )
	{
          lcl.force_shared_colors = TRUE;
	}

     /*
      *  Turn off automatic equalization?
      *  
      */
      else if (strncmp(argv[i], "-nosq",5) == 0 )
	{
          lcl.zoom_square_aspect = FALSE;
	}

     /*
      *  Turn off automatic equalization?
      *  
      */
      else if (strncmp(argv[i], "-noeq",5) == 0 )
	{
          lcl.auto_equal = FALSE;
	}

     /*
      *  Turn on automatic equalization?
      *  
      */
      else if (strncmp(argv[i], "-eq",3) == 0 )
	{
	  if ((argc-i) < 3)
            {
              usage(argc,argv,i,3);
              return(FALSE);
            }

	  i += 1;
          lcl.equalization_threshold = atoi(argv[i]) * 0.001;

	  i += 1;
          lcl.black_fudge = atoi(argv[i]);

	  i += 1;
          lcl.white_fudge = atoi(argv[i]);

          lcl.auto_equal = TRUE;
	}


     /*
      *  Gamma On??
      *  
      */
      else if (strncmp(argv[i], "-xga",4) == 0 )
	{
	  if ((argc-i) < 2)
            {
              usage(argc,argv,i,2);
              return(FALSE);
            }

	  i += 1;
          lcl.gamma = atoi(argv[i]) * 0.01;

	  i += 1;
          lcl.gamma_shift = atoi(argv[i]) * 0.01;
	  lcl.enable_gamma = TRUE;
	}

     /*
      *  Set X$Synchronize?
      *  
      */
      else if (strncmp(argv[i], "-sync",5) == 0 )
	{
	  lcl.set_sync = TRUE;
	}

     /*
      *  Gamma off??
      *  
      */
      else if (strncmp(argv[i], "-noga",5) == 0 )
	{
	  lcl.enable_gamma = FALSE;
	}

     /*
      *  No image color window??
      *  
      */
      else if (strncmp(argv[i], "-noim",5) == 0 )
	{
	  lcl.show_intensity_window    = FALSE;
          lcl.show_pixel_intensity = FALSE;
	}

      else if (strncmp(argv[i], "-imag",5) == 0 )
	{

	  lcl.show_intensity_window    = TRUE;
          lcl.show_pixel_intensity = 0;

	  if ((argc-i) >= 1)
            {
	      i += 1;
              lcl.show_pixel_intensity = atoi(argv[i]);
            }
	}

     /*
      *  Set float convolve
      *  
      */
      else if (strncmp(argv[i], "-flcon",6) == 0 )
	{
	  lcl.convolve_float = TRUE;
	}

     /*
      *  Set int convolve
      *  
      */
      else if (strncmp(argv[i], "-incon",6) == 0 )
	{
	  lcl.convolve_float = FALSE;
	}

     /*
      *  Set no red/blue guns
      *  
      */
      else if (strncmp(argv[i], "-norbg",6) == 0 )
	{
	  lcl.no_red_blue_guns = TRUE;
	}

     /*
      *  Set red/blue guns enabled
      *  
      */
      else if (strncmp(argv[i], "-rbg",4) == 0 )
	{
	  lcl.no_red_blue_guns = FALSE;
	}

     /*
      *  Set debug
      *  
      */
      else if (strncmp(argv[i], "-debug",6) == 0 )
	{
	  state.debugging = TRUE;
	}

     /*
      *  Hmmm.  We're out of options and haven't matched...
      *  print usage message.
      * 
      */
      else
        {
          usage(argc,argv,i, -99);
          return(FALSE);
	}
    }
  return (TRUE);
}

/*
 *  Routine:
 *
 *	usage
 *
 *  Description:
 *
 *	Displays the command line options.  This is done when an error
 *	occurs, and the error point is highlighted.
 *
 *  Inputs:
 *
 *	argv, argc and the argument that the error occured on.
 *
 *  Outputs:
 *
 *	None
 *
 */
void 
usage(int argc, char *argv[], int i, int why)

{
  int j;

  if (state.debugging)
    printf("display_options\n");

  printf("\nCommand line: <<");
  for (j = 0; j < argc; j += 1)
    printf("%s ",argv[j]);
  printf(">>\n");
  if (why < 0)
    printf("\nCould not parse: %s\n",argv[i]);
  else
    {
      printf("\nThe command: %s\n",argv[i]);
      printf("\nRequires %d arguments, try it again.\n", why);
    }
  printf("\n");
  printf("XIMQ is designed to display compressed images from a NASA Planetary Data\n");
  printf("Systems CD.  It was written by Fred Kleinsorge at Digital Equipment\n");
  printf("Corporation for the OpenVMS environment.  And is provided as a unsupported\n");
  printf("demo.  Feel free to use the source, or modify this application in any way.\n");
  printf("\n");
  printf("\nUsage:\n");
  printf("-sync                 - Set X synchronization true\n");
  printf("-title                - Display title\n");
  printf("-icon                 - Display Icon title\n");
  printf("-display FOO::s.s     - Display to show window on\n");
  printf("-file dev:file.ext    - File to read\n");
  printf("-index dev:file.ext   - Index File to use\n");
  printf("-volinfo dev:file.ext - Volume Information File to use\n");
  printf("-colors <n>           - Number of colors\n");
  printf("-cmap                 - Create private colormap\n");
  printf("-staticcolor          - Force static colormap use\n"); 
  printf("-limits <l> <h>       - B&W threshold limits low and high\n"); 
  printf("-shift <n> <n>        - Shift lightness default and shift default\n"); 
  printf("-xgamma <n> <n>        - Gamma correction * 100 and shift default * 100\n"); 
  printf("-flconvolve           - Use FLOAT for convolution calculations\n"); 
  printf("-inconvolve           - Use INT   for convolution calculations\n"); 
  printf("-nogamma              - No gamma correction\n"); 
  printf("-nosqzoom             - Allow non-square zoom\n"); 
  printf("-noequal              - Turn off automatic equalization\n"); 
  printf("-norbguns             - Turn off RED and BLUE guns\n"); 
  printf("-rbguns               - Turn on  RED and BLUE guns (default)\n"); 
  printf("-image_colors <0/1>   - Show image colors.  0 = No intensity display\n");
  printf("-noimage_colors       - Do not create image colors display\n");
  printf("-zoomstyle <n>        - Zoom style:\n"); 
  printf("                      -            0 = Pixel Replication (default) \n");
  printf("                      -            1 = Nearest Neighbor Approximation\n"); 
  printf("                      -            2 = X/Y average\n"); 
  printf("-equal <p> <l> <h>    - Automatic Histogram Equalization\n"); 
  printf("                      -            p = threshold percent * 10\n");
  printf("                      -            l = low limit adjustment\n");
  printf("                      -            h = high limit adjustment\n");
  printf("-g <x> <y> <w> <h>    - Geometry:  x = offset from left\n");
  printf("                      -            y = offset from top\n");
  printf("                      -            w = width\n");
  printf("                      -            y = height\n");
  printf("-debug                - Print debgging statements\n"); 
}
