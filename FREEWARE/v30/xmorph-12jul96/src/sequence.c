/* sequence.c: routines to make a sequence of digital image warp frames
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifndef VMS
#include <X11/Xaw/Scrollbar.h>
#else
#include <X11Xaw/Scrollbar.h>
#endif

#include "diw_map.h"
#include "mjg_dialog.h"
#include "warp.h"
#include "mesh.h"
#include "image.h"
#include "sequence.h"


/* ---------------------------------------------------------------------- */

static char *sequence_file_name = NULL;
static int  sequence_num_frames = 30;

/* ---------------------------------------------------------------------- */

#define SHARPNESS 2.0

static double
sigmoid_func(double x)
{
  register double as;
  as=atan(SHARPNESS);
  return((atan((x-0.5)*SHARPNESS*2.0)+as)/(2.0*as));
}

/* ---------------------------------------------------------------------- */
static double
linear_func(double x)
{
  return(x);
}

/* ---------------------------------------------------------------------- */
void
warp_rgba_image(rgba_image_t *inP, rgba_image_t *outP, double *sxP, double *syP, double *dxP, double *dyP, int nx, int ny)
{
  warp_image(inP->ri,outP->ri, inP->ncols,inP->nrows, sxP,syP, dxP,dyP, nx,ny);
  warp_image(inP->gi,outP->gi, inP->ncols,inP->nrows, sxP,syP, dxP,dyP, nx,ny);
  warp_image(inP->bi,outP->bi, inP->ncols,inP->nrows, sxP,syP, dxP,dyP, nx,ny);
  warp_image(inP->ai,outP->ai, inP->ncols,inP->nrows, sxP,syP, dxP,dyP, nx,ny);
}

/* ---------------------------------------------------------------------- */
void
warp_sequence(diw_map_t *dmP, int steps, char *basename, int show)
{
  double x;
  double dissolve_t, warp_t;
  double *mxP, *myP;
  int nx, ny;
  int frame;
  char iname[255];

  nx = dmP->mesh_num_x;    ny = dmP->mesh_num_y;

  if(basename==NULL) basename="warp";

  if(alloc_mesh(&mxP, &myP, nx, ny, "warp_sequence")) return;

  if(steps<0) steps=0;

  for(frame=0; frame<steps; frame++) {

    x = (double)frame / (steps-1);
    dissolve_t = sigmoid_func(x);
    warp_t = linear_func(x);

    if(show) {
      double dt, wt;
      dt = dmP->img_t;
      wt = dmP->mesh_t;
      dmP->img_t = dissolve_t;
      dmP->mesh_t = warp_t;
      XawScrollbarSetThumb(dmP->dissolve_sb, dissolve_t, -1.0);
      XawScrollbarSetThumb(dmP->warp_sb, warp_t, -1.0);
      WarpImage(dmP->widget, NULL, NULL, NULL);
      dmP->img_t = dt;
      dmP->mesh_t = wt;

    } else {

      lin_interp_mesh(dmP->mesh_xs, dmP->mesh_xd, nx, ny, warp_t, mxP);
      lin_interp_mesh(dmP->mesh_ys, dmP->mesh_yd, nx, ny, warp_t, myP);
      warp_rgba_image(&orig_image[0], &dmP->src_img, dmP->mesh_xs, dmP->mesh_ys, mxP, myP, nx, ny);
      warp_rgba_image(&orig_image[1], &dmP->dst_img, dmP->mesh_xd, dmP->mesh_yd, mxP, myP, nx, ny);

    }

    sprintf(iname, "%s%04i", basename, frame);

    save_img(iname, &dmP->src_img, &dmP->dst_img, dissolve_t);
  }

  /* free_mesh added WA (MJG 13sep95) */
  free_mesh(mxP, myP, "warp_sequence");
}

/* =============================================================== */
/*                           Callbacks                             */
/* --------------------------------------------------------------- */
void
set_sequence_file_name_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
  dialog_apdx_t *daP = (dialog_apdx_t *)call_data;
  char *fn;

  fn = XawDialogGetValueString(daP->dialog);
  if(fn==NULL) {
    return;
  } else {
    if(sequence_file_name != NULL) {
      my_free(sequence_file_name, "set_sequence_file_name");
    }
    sequence_file_name = strdup(fn);
  }
}

/* --------------------------------------------------------------- */
void
set_sequence_num_frames_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
  dialog_apdx_t *daP = (dialog_apdx_t *)call_data;
  char *nf;

  nf = XawDialogGetValueString(daP->dialog);
  if(nf==NULL) {
    return;
  } else {
    sequence_num_frames = atoi(nf);
  }
}

/* --------------------------------------------------------------- */
void
warp_sequence_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
  warp_sequence(&global_diw_map[0], sequence_num_frames, sequence_file_name, True);
}

/* =============================================================== */
/*                           Actions                               */
/* --------------------------------------------------------------- */

/* WarpSequence: action to generate a warp sequence
*/
void
WarpSequence(Widget widget, XEvent *evt, String *prms, Cardinal *n_prms)
{
  diw_map_t *diw_mapP;

  if((diw_mapP = diw_map_of_widget(widget)) == NULL) {
    fprintf(stderr, "WarpSequence: Bad Widget for diw_map\n");
    return;
  }
  warp_sequence(diw_mapP, sequence_num_frames, sequence_file_name, True);
}
