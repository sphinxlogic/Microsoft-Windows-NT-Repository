/*---------------------------------------------------------------------------

   rpng2 - progressive-model PNG display program                 readpng2.c

  ---------------------------------------------------------------------------

      Copyright (c) 1998-1999 Greg Roelofs.  All rights reserved.

      This software is provided "as is," without warranty of any kind,
      express or implied.  In no event shall the author or contributors
      be held liable for any damages arising in any way from the use of
      this software.

      Permission is granted to anyone to use this software for any purpose,
      including commercial applications, and to alter it and redistribute
      it freely, subject to the following restrictions:

      1. Redistributions of source code must retain the above copyright
         notice, disclaimer, and this list of conditions.
      2. Redistributions in binary form must reproduce the above copyright
         notice, disclaimer, and this list of conditions in the documenta-
         tion and/or other materials provided with the distribution.
      3. All advertising materials mentioning features or use of this
         software must display the following acknowledgment:

            This product includes software developed by Greg Roelofs
            and contributors for the book, "PNG: The Definitive Guide,"
            published by O'Reilly and Associates.

  ---------------------------------------------------------------------------*/


#include <stdlib.h>	/* for exit() prototype */

#include "png.h"	/* libpng header; includes zlib.h and setjmp.h */
#include "readpng2.h"	/* typedefs, common macros, public prototypes */


/* local prototypes */

static void readpng2_info_callback(png_structp png_ptr, png_infop info_ptr);
static void readpng2_row_callback(png_structp png_ptr, png_bytep new_row,
                                 png_uint_32 row_num, int pass);
static void readpng2_end_callback(png_structp png_ptr, png_infop info_ptr);
static void readpng2_error_handler(png_structp png_ptr, png_const_charp msg);




void readpng2_version_info()
{
    fprintf(stderr, "   Compiled with libpng %s; using libpng %s.\n",
      PNG_LIBPNG_VER_STRING, png_libpng_ver);
    fprintf(stderr, "   Compiled with zlib %s; using zlib %s.\n",
      ZLIB_VERSION, zlib_version);
} 




int readpng2_check_sig(uch *sig, int num)
{
    return png_check_sig(sig, num);
}




/* returns 0 for success, 2 for libpng problem, 4 for out of memory */

int readpng2_init(mainprog_info *mainprog_ptr)
{
    png_structp  png_ptr;	/* note:  temporary variables! */
    png_infop  info_ptr;


    /* could also replace libpng warning-handler (final NULL), but no need: */

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, mainprog_ptr,
      readpng2_error_handler, NULL);
    if (!png_ptr)
        return 4;   /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;   /* out of memory */
    }


    /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters) */


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function, unless an alternate error handler was installed--
     * but compatible error handlers must either use longjmp() themselves
     * (as in this program) or exit immediately, so here we are: */

    if (setjmp(mainprog_ptr->jmpbuf)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }


    /* instead of doing png_init_io() here, now we set up our callback
     * functions for progressive decoding */

    png_set_progressive_read_fn(png_ptr, mainprog_ptr,
      readpng2_info_callback, readpng2_row_callback, readpng2_end_callback);


    /* make sure we save our pointers for use in readpng2_decode_data() */

    mainprog_ptr->png_ptr = png_ptr;
    mainprog_ptr->info_ptr = info_ptr;


    /* and that's all there is to initialization */

    return 0;
}




/* returns 0 for success, 2 for libpng (longjmp) problem */

int readpng2_decode_data(mainprog_info *mainprog_ptr, uch *rawbuf, ulg length)
{
    png_structp png_ptr = (png_structp)mainprog_ptr->png_ptr;
    png_infop info_ptr = (png_infop)mainprog_ptr->info_ptr;


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(mainprog_ptr->jmpbuf)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        mainprog_ptr->png_ptr = NULL;
        mainprog_ptr->info_ptr = NULL;
        return 2;
    }


    /* hand off the next chunk of input data to libpng for decoding */

    png_process_data(png_ptr, info_ptr, rawbuf, length);

    return 0;
}




static void readpng2_info_callback(png_structp png_ptr, png_infop info_ptr)
{
    mainprog_info  *mainprog_ptr;
    int  color_type, bit_depth;
    double  gamma;


    /* setjmp() doesn't make sense here, because we'd either have to exit(),
     * longjmp() ourselves, or return control to libpng, which doesn't want
     * to see us again.  By not doing anything here, libpng will instead jump
     * to readpng2_decode_data(), which can return an error value to the main
     * program. */


    /* retrieve the pointer to our special-purpose struct, using the png_ptr
     * that libpng passed back to us (i.e., not a global this time--there's
     * no real difference for a single image, but for a multithreaded browser
     * decoding several PNG images at the same time, one needs to avoid mixing
     * up different images' structs) */

    mainprog_ptr = png_get_progressive_ptr(png_ptr);

    if (mainprog_ptr == NULL) {		/* we be hosed */
        fprintf(stderr,
          "readpng2 error:  main struct not recoverable in info_callback.\n");
        fflush(stderr);
        return;
        /*
         * Alternatively, we could call our error-handler just like libpng
         * does, which would effectively terminate the program.  Since this
         * can only happen if png_ptr gets redirected somewhere odd or the
         * main PNG struct gets wiped, we're probably toast anyway.  (If
         * png_ptr itself is NULL, we would not have been called.)
         */
    }


    /* this is just like in the non-progressive case */

    png_get_IHDR(png_ptr, info_ptr, &mainprog_ptr->width,
      &mainprog_ptr->height, &bit_depth, &color_type, NULL, NULL, NULL);


    /* since we know we've read all of the PNG file's "header" (i.e., up
     * to IDAT), we can check for a background color here */

    if (mainprog_ptr->need_bgcolor &&
        png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD))
    {
        png_color_16p pBackground;

        /* it is not obvious from the libpng documentation, but this function
         * takes a pointer to a pointer, and it always returns valid red,
         * green and blue values, regardless of color_type: */
        png_get_bKGD(png_ptr, info_ptr, &pBackground);

        /* however, it always returns the raw bKGD data, regardless of any
         * bit-depth transformations, so check depth and adjust if necessary */
        if (bit_depth == 16) {
            mainprog_ptr->bg_red   = pBackground->red   >> 8;
            mainprog_ptr->bg_green = pBackground->green >> 8;
            mainprog_ptr->bg_blue  = pBackground->blue  >> 8;
        } else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
            if (bit_depth == 1)
                mainprog_ptr->bg_red = mainprog_ptr->bg_green =
                  mainprog_ptr->bg_blue = pBackground->gray? 255 : 0;
            else if (bit_depth == 2)
                mainprog_ptr->bg_red = mainprog_ptr->bg_green =
                  mainprog_ptr->bg_blue = (255/3) * pBackground->gray;
            else /* bit_depth == 4 */
                mainprog_ptr->bg_red = mainprog_ptr->bg_green =
                  mainprog_ptr->bg_blue = (255/15) * pBackground->gray;
        } else {
            mainprog_ptr->bg_red   = (uch)pBackground->red;
            mainprog_ptr->bg_green = (uch)pBackground->green;
            mainprog_ptr->bg_blue  = (uch)pBackground->blue;
        }
    }


    /* as before, let libpng expand palette images to RGB, low-bit-depth
     * grayscale images to 8 bits, transparency chunks to full alpha channel;
     * strip 16-bit-per-sample images to 8 bits per sample; and convert
     * grayscale to RGB[A] */

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);


    /* Unlike the basic viewer, which was designed to operate on local files,
     * this program is intended to simulate a web browser--even though we
     * actually read from a local file, too.  But because we are pretending
     * that most of the images originate on the Internet, we follow the recom-
     * mendation of the sRGB proposal and treat unlabelled images (no gAMA
     * chunk) as existing in the sRGB color space.  That is, we assume that
     * such images have a file gamma of 0.45455, which corresponds to a PC-like
     * display system.  This change in assumptions will have no effect on a
     * PC-like system, but on a Mac, SGI, NeXT or other system with a non-
     * identity lookup table, it will darken unlabelled images, which effec-
     * tively favors images from PC-like systems over those originating on
     * the local platform.  Note that mainprog_ptr->display_exponent is the
     * "gamma" value for the entire display system, i.e., the product of
     * LUT_exponent and CRT_exponent. */

    if (png_get_gAMA(png_ptr, info_ptr, &gamma))
        png_set_gamma(png_ptr, mainprog_ptr->display_exponent, gamma);
    else
        png_set_gamma(png_ptr, mainprog_ptr->display_exponent, 0.45455);


    /* we'll let libpng expand interlaced images, too */

    mainprog_ptr->passes = png_set_interlace_handling(png_ptr);


    /* all transformations have been registered; now update info_ptr data and
     * then get rowbytes and channels */

    png_read_update_info(png_ptr, info_ptr);

    mainprog_ptr->rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    mainprog_ptr->channels = png_get_channels(png_ptr, info_ptr);


    /* Call the main program to allocate memory for the image buffer and
     * initialize windows and whatnot.  (The old-style function-pointer
     * invocation is used for compatibility with a few supposedly ANSI
     * compilers that nevertheless barf on "fn_ptr()"-style syntax.) */

    (*mainprog_ptr->mainprog_init)();


    /* and that takes care of initialization */

    return;
}





static void readpng2_row_callback(png_structp png_ptr, png_bytep new_row,
                                  png_uint_32 row_num, int pass)
{
    mainprog_info  *mainprog_ptr;


    /* first check whether the row differs from the previous pass; if not,
     * nothing to combine or display */

    if (!new_row)
        return;


    /* retrieve the pointer to our special-purpose struct so we can access
     * the old rows and image-display callback function */

    mainprog_ptr = png_get_progressive_ptr(png_ptr);


    /* have libpng either combine the new row data with the existing row data
     * from previous passes (if interlaced) or else just copy the new row
     * into the main program's image buffer */

    png_progressive_combine_row(png_ptr, mainprog_ptr->row_pointers[row_num],
      new_row);


    /* finally, call the display routine in the main program with the number
     * of the row we just updated */

    (*mainprog_ptr->mainprog_display_row)(row_num);


    /* and we're ready for more */

    return;
}





static void readpng2_end_callback(png_structp png_ptr, png_infop info_ptr)
{
    mainprog_info  *mainprog_ptr;


    /* retrieve the pointer to our special-purpose struct */

    mainprog_ptr = png_get_progressive_ptr(png_ptr);


    /* let the main program know that it should flush any buffered image
     * data to the display now and set a "done" flag or whatever, but note
     * that it SHOULD NOT DESTROY THE PNG STRUCTS YET--in other words, do
     * NOT call readpng2_cleanup() either here or in the finish_display()
     * routine; wait until control returns to the main program via
     * readpng2_decode_data() */

    (*mainprog_ptr->mainprog_finish_display)();


    /* all done */

    return;
}





void readpng2_cleanup(mainprog_info *mainprog_ptr)
{
    png_structp png_ptr = (png_structp)mainprog_ptr->png_ptr;
    png_infop info_ptr = (png_infop)mainprog_ptr->info_ptr;

    if (png_ptr && info_ptr)
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    mainprog_ptr->png_ptr = NULL;
    mainprog_ptr->info_ptr = NULL;
}





static void readpng2_error_handler(png_structp png_ptr, png_const_charp msg)
{
    mainprog_info  *mainprog_ptr;

    /* This function, aside from the extra step of retrieving the "error
     * pointer" (below) and the fact that it exists within the application
     * rather than within libpng, is essentially identical to libpng's
     * default error handler.  The second point is critical:  since both
     * setjmp() and longjmp() are called from the same code, they are
     * guaranteed to have compatible notions of how big a jmp_buf is,
     * regardless of whether _BSD_SOURCE or anything else has (or has not)
     * been defined. */

    fprintf(stderr, "readpng2 libpng error: %s\n", msg);
    fflush(stderr);

    mainprog_ptr = png_get_error_ptr(png_ptr);
    if (mainprog_ptr == NULL) {		/* we are completely hosed now */
        fprintf(stderr,
          "readpng2 severe error:  jmpbuf not recoverable; terminating.\n");
        fflush(stderr);
        exit(99);
    }

    longjmp(mainprog_ptr->jmpbuf, 1);
}
