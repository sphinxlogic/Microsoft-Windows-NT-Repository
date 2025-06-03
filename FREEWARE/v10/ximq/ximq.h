/*
 *   XIMQ
 *
 *   Copyright © 1991, 1994 Digital Equipment Corporation
 *   All rights reserved.
 *
 *   Permission to use, copy, modify, and distribute this software and its
 *   documentation for any purpose, without fee, and without written agreement is
 *   hereby granted, provided that the above copyright notice and the following
 *   two paragraphs appear in all copies of this software.
 *
 *   IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE TO ANY PARTY FOR
 *   DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 *   OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION.
 *
 *   DIGITAL EQUIPMENT CORPORATION SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 *   AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 *   ON AN "AS IS" BASIS, AND DIGITAL EQUIPMENT CORPORATION HAS NO OBLIGATION TO
 *   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "rms.h"
#include "secdef.h"
#include "descrip.h"
#include "ssdef.h"
#include "ctype.h"
#include "math.h"

#include "decw$include:keysym.h"
#include "decw$include:xlib.h"
#include "decw$include:xutil.h"
#include "decw$include:x.h"

#define DEFAULT_FILE   (unsigned char *) "XIMQ$:.IMQ"
#define INDEX_DEFAULT  (unsigned char *) "XIMQ$:IMGINDEX.TAB"
#define VOLUME_DEFAULT (unsigned char *) "XIMQ$:VOLDESC.SFD"
#define INDEX_FILE     (unsigned char *) "XIMQ$:[INDEX]IMGINDEX.TAB"
#define VOLUME_FILE    (unsigned char *) "XIMQ$:[000000]VOLDESC.SFD"

#define FONT1 "-Bigelow & Holmes-Menu-Medium-R-Normal--12-120-75-75-P-70-ISO8859-1"
#define FONT2 "-Bigelow & Holmes-Menu-Medium-R-Normal--10-100-75-75-P-56-ISO8859-1"
#define FONT3 "-Adobe-Helvetica-Bold-R-Normal--12-120-75-75-P-70-ISO8859-1"
#define FONT4 "fixed"

#define TRUE  1
#define FALSE 0

#define DEF_COLORS 128
#define MAX_VALUES 256
#define MAX_PIXEL MAX_VALUES-1
#define TEXT_MAX 80

#define DEF_DISPLAY_INC 10

#define DEF_SCANLINES 800
#define DEF_SAMPLES_PER_LINE 800

#define SMALL_SCANLINES 200
#define SMALL_SAMPLES_PER_LINE 200

#define INTENSITY_WINDOW_WIDTH 32

#define DEF_HIGHLIGHT_WIDTH 1
#define HIGHLIGHT_MAX 15

#define EQ_X  70
#define EQ_X1 66
#define EQ_X2 74                    

#define BLACK_LIMIT_DEFAULT 0
#define WHITE_LIMIT_DEFAULT MAX_PIXEL
#define AUTO_EQUAL_DEFAULT 1
#define BLACK_FUDGE_DEFAULT 5
#define WHITE_FUDGE_DEFAULT 20
#define BRIGHTNESS_DEFAULT 0
#define BRIGHT_SHIFT_DEFAULT 5

struct statedef {

  int workstation_open;				/* Is there a display? */
  int main_display_open;			/* Main display open */
  int debugging;				/* Print debug statements */
  int planes;					/* Number of planes */
  int screen;					/* Screen number */
  int need_colormap;				/* Window attribute */ 
  int vis_class;				/* Visual Class */
  int manual_file_select;			/* */
  int use_file_select_window;			/* */
  int file_select_window_mapped;		/* */
  int use_message_window;			/* */
  int message_window_mapped;			/* */
  int show_select_window;			/* .. the menu window */
  int white_pixel;				/* */
  int black_pixel;				/* */
  int default_pixel;				/* */
  int foreground_pixel;				/* Default foreground pixel */
  int red_pixel;				/* Red color */
  int green_pixel;				/* Green color */
  int blue_pixel;				/* Blue color */
  int number_of_colors;				/* Number of colors to use */
  int num_outlines;				/* Number out per call... */
  int num_scanlines;				/* Image scanlines */
  int samples_per_line;				/* and number of samples */
  int colormap;					/* Colormap ID */
  int pixel_map[MAX_VALUES];			/* Revectored pixels */
  int pixel_intensities[MAX_VALUES];		/* Final Color */
  int gamma_correction[MAX_VALUES];		/* Lookup Table */
  Display *display;				/* Display ID */
  Window root;					/* Window and Root IDs */
  Visual *visual;				/* Visual ID */
  XEvent *event;				/* Event packet */
  Window image_window;				/* Image window ID */
  GC image_gc;					/* GC for the image */
  GC small_image_gc;				/* GC for the small image */

  unsigned char *image_buffer1;
  unsigned char *zoom_buffer1;
  unsigned char *zoom_buffer2;
  unsigned char *original_image_buffer1;
  unsigned char *original_image_buffer2;
  unsigned char *current_image;
  unsigned char *original_image;
  unsigned char *zoom_buffer;
  unsigned char *image_buffer;

  XImage *main_ximage;			/* main image structure pointer */
  XImage *small_ximage;			/* small image structure pointer */

  unsigned char *index_filename;
  int  index_file_open;
  int  use_index;

  unsigned char *volume_filename;
  int  volume_file_open;
  int  use_volume;

};

typedef struct _wininfo {
   Window window;
   void (*event_rtn)();
} WININFO;

struct window_list {
   int     last;
   WININFO list[50];
};

struct txtdef {

  char *volume_set_name;
  char *volume_set_id;
  char *volume_name;
  char *volume_id;
  char *mission_name;
  char *mission_phase_name;
  char *volume_note[8];
  char *data_set_description[40];

  char filter_name[TEXT_MAX];
  char target_name[TEXT_MAX];
  char image_id[TEXT_MAX];
  char image_number[TEXT_MAX];
  char note_data[TEXT_MAX];
  char image_time[TEXT_MAX];
  char filter_number[TEXT_MAX];
  char scan_mode[TEXT_MAX];
  char spacecraft_name[TEXT_MAX];
  char work_buffer[TEXT_MAX];

  char *status_text;
  char *no_select;
  char *calibrating_text;
  char *uncompressing_text;

};


/*
 *  Display a scanline as an inline function via a MACRO
 *
 */
#define display_scanline(line,scan,count) { int zz; state.main_ximage->data = (char *) scan;\
XPutImage (state.display, state.image_window, state.image_gc, state.main_ximage, 0, 0, 0, line, state.samples_per_line, count);\
XFlush(state.display); \
if ((zz = XEventsQueued(state.display, QueuedAfterFlush)) > 0) handle_events(zz, FALSE);};

/*
 *  Inline macro to map the image data to pixel values
 *
 */
#define reformat_image_data(input_image, image_pointer) { \
  int i; union {unsigned char *c; unsigned int *l; } new_image; \
  unsigned char *orig_image = input_image; \
  new_image.c = image_pointer; \
  if (state.planes > 8) { \
   for (i = 0; i < state.samples_per_line; i += 1) { \
    *new_image.l++ = state.pixel_map[*orig_image++]; \
    } \
   } \
  else { \
   for (i = 0; i < state.samples_per_line; i += 1) { \
    *new_image.c++ = state.pixel_map[*orig_image++]; \
    } \
   } \
  image_pointer = new_image.c; };


/*
 *  Macro's for the median filter
 *
 */
#define s2(a,b) {register int t; if ((t = b-a) < 0) {a += t; b -= t;}}
#define mn3(a,b,c) s2(a,b); s2(a,c);
#define mx3(a,b,c) s2(b,c); s2(a,c);
#define mnmx3(a,b,c) mx3(a,b,c); s2(a,b);
#define mnmx4(a,b,c,d) s2(a,b); s2(c,d); s2(a,c); s2(b,d);
#define mnmx5(a,b,c,d,e) s2(a,b); s2(c,d); mn3(a,c,e); mx3(b,d,e);
#define mnmx6(a,b,c,d,e,f) s2(a,d); s2(b,e); s2(c,f); mn3(a,b,c); mx3(d,e,f);


/*
 *  The huffman encoding data is loaded into a tree structure.
 *
 */
typedef struct leaf
  {
   struct leaf *right;
   short int dn;
   struct leaf *left;
  } NODE;

/*
 *  Image files are opened by using section files, and
 *  we maintain synthetic record pointers and state to
 *  allow access that simulates read/write/seek operations.
 *
 *  As we process the label and the image, we also load
 *  up all of the interesting file information into this
 *  block, including the uncompressed image and the
 *  subsampled small version of the image.  This image
 *  data in this record is 8-bit greyscale that has not
 *  been modified beyond being uncompressed, and perhaps
 *  calibrated (dark current subtracted from the image).
 *
 *  Image processing operations, and pixel mapping happens
 *  in other buffers.
 *
 *
 */
typedef struct fileinfo {

  int type;
  int index_file_pos;
  int input_file_mapped;
  int input_file_open;
  int start_addr;
  int end_addr;
  int current_record;
  int record_format;
  int record_bytes;
  int label_records;
  int file_records;
  int image_histogram_record;
  int image_histogram_length;
  int encoding_histogram_record;
  int encoding_histogram_length;
  int engineering_table_record;
  int engineering_table_length;
  int image_record;

  union {
    unsigned char *data;
    short int *length;
        } record;

  NODE *tree;

  struct FAB fab;

  int filename_valid;

  unsigned char input_filename[256];

  int image_hist[511];
  int encoding_hist[511];

  int image_valid;
  int image_calibrated;

  unsigned char image_buffer[DEF_SCANLINES*(DEF_SAMPLES_PER_LINE+1)];
  unsigned char small_image_buffer[4*SMALL_SCANLINES*SMALL_SAMPLES_PER_LINE];

  } FILEINFO;


/*
 *  Routine references
 *
 */
extern int subtract_image();
extern int nearest_neighbor_zoom();
extern int pixel_replicate_zoom();
extern int matrix_3x3_zoom();
extern int convolve_image_3x3();
extern int convolve_image_3x3_int();
extern int median_filter();
extern int display_message_window();
extern int hide_message_window();

/*
 *  XIMQ.C
 *
 */
extern void reset_image(FILEINFO *);
extern int  process_image(FILEINFO *, int );
extern int  create_small_image(unsigned char *, unsigned char *, int, int, int);
extern int  create_uncompressed_image(FILEINFO *, unsigned char *, int);
extern int  get_image_histogram(FILEINFO *);
extern int  get_init_encoding_histogram(FILEINFO *);
extern int  get_new_image_file(FILEINFO *);
extern int  open_workstation();
extern int  create_main_display();
extern void setup_colormap();
extern int  setup_gamma_correction();
extern void  setup_pixel_map();
extern void main_loop();
extern int  handle_events(int, int);
extern int  highlight_colors_writeable( int, int, int, int, int);
extern void display_small_image_window();
extern void display_small_image_scanline(int, int);
extern void display_select_window();
extern void display_image(int, int);
extern void display_image_histogram(FILEINFO *);
extern void display_image_colors(FILEINFO *);
extern void display_image_text();
extern void display_image_status();
extern void quit_ximq();
extern void new_image();
extern void equalize();
extern void reset_image_variables();
extern void noequal();
extern void start_zoom();
extern void unzoom();
extern void gamma_darken();
extern void gamma_lighten();
extern void brighten();
extern void darken();
extern void apply_median_filter();
extern void sharpen_image();
extern void smooth();
extern void nada();
extern void calibrate();
extern void uncalibrate();
extern void next_menu();
extern void prev_menu();
extern void refresh_display();
extern int  undraw_zoom_rubberband();
extern int  draw_zoom_rubberband();
extern void zoom_image(int, int, int, int, int, int);
extern void recompute_output(int, int);
extern int  parse_command_line(int, char *[]);
extern void usage(int, char *[], int, int);
extern Window Add_Window(Display *, Window, int, int, int, int, int, int, int,
                  Visual *, int, XSetWindowAttributes *, void (*rtn)());
extern void event_on_main_window(XEvent *, int);
extern void event_on_text_window(XEvent *, int);
extern void event_on_status_window(XEvent *, int);
extern void event_on_hist_window(XEvent *, int);
extern void event_on_select_window(XEvent *, int);
extern void event_on_intensity_window(XEvent *, int);
extern void event_on_image_window(XEvent *, int);
extern void event_on_small_image_window(XEvent *, int);


/*
 *  XIMQ_IMAGE_FINDER.C
 *
 */
extern int  nocase_strcmp(unsigned char *, unsigned char *, int);
extern void clear_string(unsigned char *, int);
extern void copy_string(unsigned char *, unsigned char *, int);
extern int  process_volume_info();
extern int  parse_volume_string(unsigned char *, int, unsigned char **, int);
extern int  position_stream(unsigned char *, int, int);
extern int  create_file_select_window(int);
extern int  display_file_text(int, int, int);
extern int  display_file_selected();
extern int  display_match_window(int, GC);
extern int  display_select_file_window();
extern int  display_top_arrow();
extern int  display_bottom_arrow();
extern int  display_scroll_window();
extern int  scroll_text_window(int);
extern int  get_image_file(unsigned char *, int, int *, int *, int, unsigned char *);
extern int  hide_file_select_window();
extern int  draw_cursor(int);
extern int  insert_character(XEvent *, int);
extern int  open_volume_descriptor();
extern int  open_index_file();
extern int  scan_and_display();
extern int  scan_index();
extern int  scan_calib();
extern int  find_calibration_image();
extern void event_on_fs_text_window(XEvent *, int);
extern void event_on_file_select_window(XEvent *, int);
extern void event_on_fs_scroll_window(XEvent *, int);
extern void event_on_fs_ta_window(XEvent *, int);
extern void event_on_fs_ba_window(XEvent *, int);
extern void event_on_fs_match_window(XEvent *, int);
extern void event_on_mw_window(XEvent *, int);
extern void event_on_mw8_window(XEvent *, int);
extern void event_on_fs_selected_window(XEvent *, int);

/*
 *  XIMQ_FILEIO.C
 *
 */
extern int open_image_file();
extern int process_label();
extern int parse_value();
extern int parse_string();
extern int read_var();
extern int set_pos_var();
extern int seek_var();
extern int rewind_file(FILEINFO *);
extern int close_file(FILEINFO *);

/*
 *  XIMQ_DECOMPRESS.C
 *
 */
extern NODE *huff_tree(int *);
extern NODE *new_node(int);
extern void delete_node(NODE *);
extern void sort_freq(int *, NODE **, int);
extern void dcmprs(unsigned char *, unsigned char *, int *, int *, NODE *);

/*
 *  FIND_POSITION.C
 *
 */
extern int FindBestWindowPosition( Display *, Window,
			           int, int, int, int,
                                   int *, int *, int, Window);

/*
 *  Common extern data references
 *
 */
extern struct statedef state;
extern struct txtdef txt;

