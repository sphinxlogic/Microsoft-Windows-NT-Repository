
/*
 * xanim_config.h
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


/* These defines are used to initialize the various flags that control
 * the behavious of XAnim. They all can be overriden on the command
 * line except for DEFAULT_PLUS_IS_ON and DEFAULT_X11_VERBOSE_FLAG.
 */


/* If this is TRUE then '+' will turn an option on and '-' will turn it off.
 * if this is FALSE then '+' will turn an option off and '-' will turn
 * it on.
 */
#define DEFAULT_PLUS_IS_ON	TRUE

/* If TRUE then animations will be buffered to display type
 */
#define DEFAULT_BUFF_FLAG	TRUE

/* If TRUE then iff animations will always loop back to 1st frame instead
 * of to the 2nd delta. There is no easy way of knowing this ahead of time
 */
#define DEFAULT_IFF_LOOP_OFF	FALSE

/* If TRUE then iff CRNG (color cycling chunks) will be activated. This
 * currently only works with single images and PSEUDO_COLOR displays.
 */
#define DEFAULT_CYCLE_ON_FLAG	TRUE

/* If TRUE then image height is reduced in half. This is useful for
 * interlaced iff images and speeding up animations.
 */
#define DEFAULT_LACE_FLAG	FALSE

/* If TRUE then the cmap will fade to black between files. Only works
 * on PSEUDO_COLOR displays.
 */
#define DEFAULT_FADE_FLAG	FALSE
#define DEFAULT_FADE_TIME	20

/* If TRUE then window will be the size of the largest image. Smaller
 * images and animations will be in upper left hand corner.
 */

#define DEFAULT_NORESIZE_FLAG	FALSE

/* If this is TRUE then a single step will go forward through the frames
 * until the screen is actually modified. A color map change is not 
 * considered to modify the screen. This is useful with images that are
 * unrelated and have their own color maps.
 */
#define DEFAULT_UPDATE_FLAG	TRUE

/* This affect IFF type 5 and J compressions as well as most FLI/FLC type
 * compressions. Only the minimum area of the screen is updated that
 * contains the changes from one image to the next. This is forced off
 * in MONOCHROME mode due to peculiarities of the Floyd-Steinburg
 * dithering algorithm. Having this on can cause "apparent" pauses in
 * the animation because of the time difference between updating the
 * entire screen and just part of it. This will occur if your hardware
 * can not display the images at the specified rate. Turning optimization
 * off will force the entire animation to go at the slower rate.
 */
#define DEFAULT_OPTIMIZE_FLAG	TRUE

/* If this is TRUE and DEFAULT_BUFF_FLAG is TRUE, the images will be
 * put into pixmaps. Pixmaps have the following advantages:
 *     they are stored locally  (in case you're running remotely)
 *     they aren't copied twice (like most X11 servers do with XImages)
 *     they could be in display memory(if your hardware has enough of it)
 * It usually speeds things up.
 */
#define DEFAULT_PIXMAP_FLAG	TRUE
/* If TRUE then Floyd-Steinberg dithering is used for MONOCHROME displays
 */
#define DEFAULT_DITHER_FLAG	FALSE

/* This cause XAnim to print out debugging information. Valid levels are
 * from 1 to 5, with 0 being off.
 */
#define DEFAULT_DEBUG		0

/* When this is TRUE it causes XAnim to print out extra information about
 * the animations
 */
#define DEFAULT_VERBOSE		FALSE

/* This is the default number of times to loop through each file before
 * continuing on.
 */
#define DEFAULT_LOOPEACH_FLAG	1

/* This is the number of milliseconds between frames of the animation.
 * If 0 then the number of milliseconds is taken from the animation
 * itself.
 */
#define DEFAULT_JIFFY_FLAG	0

/* Not yet supported
 */
#define DEFAULT_PACK_FLAG	FALSE

/* This causes XAnim to print out more information about the X11
 * display on which it is running.
 */
#define DEFAULT_X11_VERBOSE_FLAG	TRUE

/* COLOR MAP STUFF.
 * FLI's are currently always mapped directly to display's colormap.
 * IFF's are currently mapped directly but at the top of the colormap.
 * GIF's have a couple of options.
 *    luma_sort:   sorts the color map based on color's brightness
 *    try_to_1st:  tries to fit new cmaps into 1st but if not, it
 *		   allocates a new one or forcibly remapping to 1st.
 *    map_to_1st:  remaps new cmaps into 1st cmap. If try_to_1st fails
 *		   or isn't set.
 *		   Eventually dither_flag will affect this as well.
 */

#define DEFAULT_CMAP_LUMA_SORT	TRUE
#define DEFAULT_CMAP_TRY_TO_1ST	TRUE
#define DEFAULT_CMAP_MAP_TO_1ST	FALSE
   
