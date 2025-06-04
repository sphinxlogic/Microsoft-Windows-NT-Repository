/* mesh_cb.c : Callbacks for Digital Image Warp mesh related widgets
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#include "diw_map.h"
#include "mjg_dialog.h"
#include "mesh.h"
#include "mesh_cb.h"

/* ---------------------------------------------------------------------- */

/* reset_mesh_cb: callback to reset a mesh
** Accesses global_diw_map[0] mesh information
** Changes global_diw_map.mesh_[xy][s|d]
** Fakes an expose event to all diw_map image regions to redraw meshes
*/
void
reset_mesh_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
  int type = (int) client_data;

  switch(type) {
    case 1:
      reset_mesh(global_diw_map[0].mesh_xs, global_diw_map[0].mesh_ys,
        global_diw_map[0].mesh_num_x, global_diw_map[0].mesh_num_y,
        global_diw_map[0].width, global_diw_map[0].height);
      break;

    case 2:
      reset_mesh(global_diw_map[0].mesh_xd, global_diw_map[0].mesh_yd,
        global_diw_map[0].mesh_num_x, global_diw_map[0].mesh_num_y,
        global_diw_map[0].width, global_diw_map[0].height);
      break;

    default:
      fprintf(stderr, "reset_mesh_cb: Bad Value: type: %i\n", type);
      return;
      break;
  }

  /* Now redraw the meshes */
  FakeAllExpose(0, NULL, NULL, NULL);
}

/* ---------------------------------------------------------------------- */

/* functionalize_mesh_cb: callback to functionalize a mesh
** Accesses global_diw_map[0] mesh information
** Changes global_diw_map.mesh_[xy][s|d]
** Fakes an expose event to all diw_map image regions to redraw meshes
*/
void
functionalize_mesh_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
  int type = (int) client_data;
  int chg;

  switch(type) {
    case 1:
    chg=functionalize_mesh(global_diw_map[0].mesh_xs, global_diw_map[0].mesh_ys,
        global_diw_map[0].mesh_num_x, global_diw_map[0].mesh_num_y,
        global_diw_map[0].width, global_diw_map[0].height);
      break;

    case 2:
    chg=functionalize_mesh(global_diw_map[0].mesh_xd, global_diw_map[0].mesh_yd,
        global_diw_map[0].mesh_num_x, global_diw_map[0].mesh_num_y,
        global_diw_map[0].width, global_diw_map[0].height);
      break;

    default:
      fprintf(stderr, "functionalize_mesh_cb: Bad Value: type: %i\n", type);
      return;
      break;
  }

  /* If the mesh changed, redraw the meshes */
  if(chg) FakeAllExpose(0, NULL, NULL, NULL);
}

/* ---------------------------------------------------------------------- */

/* load_mesh_cb: callback to load mesh from file
** Accesses global_diw_map[0] mesh information
** Changes global_diw_map.mesh_[xy][s|d]
** Frees memory for old mesh
** Allocates memory for new mesh
** Fakes an expose event to all diw_map image regions to redraw meshes
*/
void
load_mesh_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
  dialog_apdx_t *daP = (dialog_apdx_t *)call_data;
  int type = (int) client_data;
  String fn;
  double *mxP, *myP;
  int nx, ny;
  int load_mesh_rv;

  fn = XawDialogGetValueString(daP->dialog);
  if(fn==NULL) {
    return;
  } else {
    if(load_mesh_rv = load_mesh(fn, &mxP, &myP, &nx, &ny)) {
      fprintf(stderr, "load_mesh returned %i\n", load_mesh_rv);
      return;
    }

    switch(type) {
      case 1:
        free_mesh(global_diw_map[0].mesh_xs, global_diw_map[0].mesh_ys,
	  "load_mesh_cb");
        global_diw_map[0].mesh_xs = mxP;
        global_diw_map[0].mesh_ys = myP;
        break;

      case 2:
        free_mesh(global_diw_map[0].mesh_xd, global_diw_map[0].mesh_yd,
	  "load_mesh_cb");
        global_diw_map[0].mesh_xd = mxP;
        global_diw_map[0].mesh_yd = myP;
        break;

      default:
        fprintf(stderr, "load_mesh_cb: Bad Value: type: %i\n", type);
        return;
        break;
    }

    /* Check to see if new mesh geometry matches old */
    if(  (global_diw_map[0].mesh_num_x != nx)
       ||(global_diw_map[0].mesh_num_y != ny))
    {
      /* Geometries did not match */
      /* Reset the alternate mesh */
      switch(type) {
        case 1:
          free_mesh(global_diw_map[0].mesh_xd, global_diw_map[0].mesh_yd,
	    "load_mesh_cb");
          alloc_mesh(&global_diw_map[0].mesh_xd, &global_diw_map[0].mesh_yd,
            nx, ny, "load_mesh_cb");
          reset_mesh(global_diw_map[0].mesh_xd, global_diw_map[0].mesh_yd,
            nx, ny,
            global_diw_map[0].src_img.ncols, global_diw_map[0].src_img.nrows);
          break;

        case 2:
          free_mesh(global_diw_map[0].mesh_xs, global_diw_map[0].mesh_ys,
	    "load_mesh_cb");
          alloc_mesh(&global_diw_map[0].mesh_xs, &global_diw_map[0].mesh_ys,
            nx, ny, "load_mesh_cb");
          reset_mesh(global_diw_map[0].mesh_xs, global_diw_map[0].mesh_ys,
            nx, ny,
            global_diw_map[0].src_img.ncols, global_diw_map[0].src_img.nrows);
          break;

        default:
          fprintf(stderr, "load_mesh_cb: Bad Value: type: %i\n", type);
          return;
          break;
      }

      /* Update the geometry parameters */
      global_diw_map[0].mesh_num_x = nx;
      global_diw_map[0].mesh_num_y = ny;
    }
  }

  /* Propagate the mesh changes to all of the diw_maps */
  diw_map_of_widget(NULL);

  /* Redraw the meshes */
  FakeAllExpose(0, NULL, NULL, NULL);
}

/* ---------------------------------------------------------------------- */

/* save_mesh_cb: callback to save mesh to file
** Accesses global_diw_map[0] mesh information
*/
void
save_mesh_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
  dialog_apdx_t *daP = (dialog_apdx_t *)call_data;
  int type = (int) client_data;
  String fn;
  double *mxP, *myP;
  int nx, ny;
  int save_mesh_rv;

  fn = XawDialogGetValueString(daP->dialog);
  if(fn==NULL) {
    return;
  } else {

    switch(type) {
      case 1:
        mxP = global_diw_map[0].mesh_xs;
        myP = global_diw_map[0].mesh_ys;
        break;

      case 2:
        mxP = global_diw_map[0].mesh_xd;
        myP = global_diw_map[0].mesh_yd;
        break;

      default:
        fprintf(stderr, "save_mesh_cb: Bad Value: type: %i\n", type);
        return;
        break;
    }

    nx = global_diw_map[0].mesh_num_x;
    ny = global_diw_map[0].mesh_num_y;

    if(save_mesh_rv = save_mesh(fn, mxP, myP, nx, ny)) {
      fprintf(stderr, "save_mesh returned %i\n", save_mesh_rv);
      return;
    }
  }
}
