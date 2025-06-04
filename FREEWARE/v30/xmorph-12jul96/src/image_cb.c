/* image_cb.c : Callbacks for Digital Image Warp image related widgets
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#include <string.h>
#ifndef VMS
#include <memory.h>
#endif

#include "diw_map.h"
#include "image.h"
#include "mesh.h"
#include "mjg_dialog.h"
#include "image_cb.h"

/* --------------------------------------------------------------- */

/* clear_widget: clear a widget's window
*/
void
clear_widget(Widget widget, int width, int height)
{
  Display *display = XtDisplay(widget);
  GC               gc_clear;
  XGCValues        gc_vals;

  /* Erase new window */
  XtVaGetValues(widget, XtNbackground, &gc_vals.foreground, NULL);
  gc_clear = XCreateGC(display, XtWindow(widget), GCForeground, &gc_vals);
  XFillRectangle(display, XtWindow(widget), gc_clear, 0, 0, width, height);
  XFreeGC(display, gc_clear);
}

/* --------------------------------------------------------------- */

/* load_img_cb: Callback to load image into memory
** 1. Load the image into the "original" image space
** 2. Copy the "original" image space into each of the diw_map image spaces
** 3. Redither each diw_map
**
** The term "alternate image" refers to which ever image is not being
** loaded by this call of the routine.
**
** If the image geometry of this new image differs from the previous:
** Fill the remaining image with a test ** image of appropriate size,
** Reset both meshes.
*/
void
load_img_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
  dialog_apdx_t *daP = (dialog_apdx_t *)call_data;
  int           type = (int) client_data;
  String        fn;
  rgba_image_t *imgP, *altP;

  /* Fetch the filename from the dialog box text widget */
  fn = XawDialogGetValueString(daP->dialog);

  /* Decide where in memory to put the image: source or destination */
  switch(type) {
    case 1:
      imgP = &orig_image[0];
      altP = &orig_image[1];
      break;

    case 2:
      imgP = &orig_image[1];
      altP = &orig_image[0];
      break;

    default:
      fprintf(stderr, "load_img_cb: Bad Value: dialog.type %i\n", type);
      return;
      break;
  }

  /* Allocate and load in the image */
  if(load_img(fn, imgP)) {
    fprintf(stderr, "load_img_cb: Failed to load '%s'\n", fn);
    return;
  }

  /* Check to see if geometry matches */
  if(   (imgP->ncols != global_diw_map[0].width)
     || (imgP->nrows != global_diw_map[0].height))
  {
    /* Geometry of the new image does NOT match the previous */
    int indx;

    /* Try to resize the diw_map windows */
    for(indx=0; indx<NUM_DIW_MAPS; indx++) {
      if(global_diw_map[indx].width != 0) {
        {
          /* Resize the core widgets */
          Dimension        width, height;
          XtGeometryResult xmrr;

          width = imgP->ncols;
          height = imgP->nrows;
          xmrr=XtMakeResizeRequest(global_diw_map[indx].widget, width, height,
            &width, &height);
          if(xmrr==XtGeometryAlmost) {
            /* Resize window */
            xmrr=XtMakeResizeRequest(global_diw_map[indx].widget, width, height,
              &width, &height);
            clear_widget(global_diw_map[indx].widget, width, height);
          }
        }

        /* Reallocate the pixmap and ximages */
        allocate_x_images(global_diw_map[indx].widget,
          &global_diw_map[indx].pixmap, &global_diw_map[indx].ximage,
          imgP->ncols, imgP->nrows);

        /* Set new diw_map geometry */
        global_diw_map[indx].width = imgP->ncols;
        global_diw_map[indx].height = imgP->nrows;

        /* Reset the meshes */
        reset_meshes(&global_diw_map[indx]);
      }
    }

    /* Free the space of the previous alternate image */
    /* Fill the alternate image with a test image */
    switch(type) {
      case 1:
        free_image(&orig_image[1]);
        orig_image[1].ncols = imgP->ncols;
        orig_image[1].nrows = imgP->nrows;
        make_test_image(&orig_image[1], 1);
        reset_images(2);
        break;

      case 2:
        free_image(&orig_image[0]);
        orig_image[0].ncols = imgP->ncols;
        orig_image[0].nrows = imgP->nrows;
        make_test_image(&orig_image[0], 2);
        reset_images(1);
        break;
    }
  }

  /* Force the diw_maps to use the new image */
  reset_images(type);

  /* Display the new image */
  ReditherAllImages(NULL, NULL, NULL, NULL);
}

/* --------------------------------------------------------------- */

/* save_img_cb: Callback to save image to file
** Save the image from the diw_map space to a file
*/
void
save_img_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
  dialog_apdx_t *daP = (dialog_apdx_t*)call_data;
  diw_map_t     *dmP = (diw_map_t*)client_data;
  String         fn;

  /* Fetch the filename from the dialog box text widget */
  fn = XawDialogGetValueString(daP->dialog);

  if(save_img(fn, &dmP->src_img, &dmP->dst_img, dmP->img_t)) {
    fprintf(stderr, "save_img_cb: Failed to save to '%s'\n", fn);
  }
}
