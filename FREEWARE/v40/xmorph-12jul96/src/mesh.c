/* mesh.c -- mesh handling routines
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#include <stdlib.h>

#include "diw_map.h"
#include "file.h"
#include "mesh.h"

/* ---------------------------------------------------------------------- */

/* lin_interp_mesh: linear interpolation between meshes
** mo = mi1 : t==0, mo = mi2 : t==1
**
** MJG 18jul94:
** Sometimes, the roundoff error here, although extremely minute,
** triggers the bounds check in the spline evaluator.  The effect should
** be harmless, though, if the spline evaluates out of range.
*/
void
lin_interp_mesh(double *mi1, double *mi2, int nx, int ny, double t, double *mo)
{
  register int xi, yi;

  for(yi=0; yi<ny; yi++) {
    for(xi=0; xi<nx; xi++) {
      mo[yi*nx + xi] = (1.0-t)*mi1[yi*nx + xi] + t*mi2[yi*nx + xi];
    }
  }
}

/* ---------------------------------------------------------------------- */

/* reset_mesh : set image warp mesh to be a regularly spaced mesh
** mxP, myP are the mesh x, y coordinates
** nx, ny are the dimensions of the mesh
** mw, mh are the width and height of the mesh's image
*/
void
reset_mesh(double *mxP, double *myP, int nx, int ny, int mw, int mh)
{
  register int xi, yi;
  double mp_dx, mp_dy;

  mp_dx = (double)(mw - 1) / (double)(nx - 1) ;
  mp_dy = (double)(mh - 1) / (double)(ny - 1) ;

  for(yi=0; yi < ny; yi++) {
    for(xi=0; xi < nx; xi++) {
      mxP[yi*nx + xi] = (int)(mp_dx*(double)xi);
      myP[yi*nx + xi] = (int)(mp_dy*(double)yi);
    }
  }
}

/* ---------------------------------------------------------------------- */

/* reset_meshes : set image warp mesh to be a regularly spaced mesh
*/
void
reset_meshes(diw_map_t *diw_mapP)
{
  reset_mesh(diw_mapP->mesh_xs, diw_mapP->mesh_ys,
    diw_mapP->mesh_num_x, diw_mapP->mesh_num_y,
    diw_mapP->width, diw_mapP->height);

  reset_mesh(diw_mapP->mesh_xd, diw_mapP->mesh_yd,
    diw_mapP->mesh_num_x, diw_mapP->mesh_num_y,
    diw_mapP->width, diw_mapP->height);
}

/* ---------------------------------------------------------------------- */

/* functionalize_mesh : set image warp mesh to be functional and bounded
** mxP, myP are the mesh x, y coordinates
** nx, ny are the dimensions of the mesh
** mw, mh are the width and height of the mesh's image
** return number of changes.
**
** This routine only enforces vertical and horizontal functional lines.
** (I.e. lines can cross diagonally.)
**
** The problem with this routine is that if a point is out of its box,
** it's mathematically ambiguous whether that point should be moved,
** or whether the adjacent point should be moved.  Moving either fixed
** the functionality, but usually there is an intuitive choice which
** this algorithm does not see.  This algorithm moves both points.
** To fix this problem, a heuristic could be employed to place a point
** within some weighted average of its neighbors.  Another posibility
** would be to generate the spline and use the values the spline is
** forced to use.  The problem with this is that the spline already
** expects functional data, so forcing a spline might break the
** spline.  Yet another possibility is to make a first-pass through
** the data to see which points need fixing, by the criteria used in
** this routine, along with an additional backwards criterion to ensure
** symmetry..  Then, the second pass would weight the changes according to
** which points require changes.  This would, at least, keep major
** crossovers effects localized (such as when a single point crosses
** over many points).
**
** This could be looked at as a bare-bones functionalizer-- one which
** simply guarentees that meshes are functional.  It is probably the
** job of another algorithm to make the mesh look more like what the
** user intended, but the user should have done what it intended in the
** first place...
*/
int
functionalize_mesh(double *mxP, double *myP, int nx, int ny, int mw, int mh)
{
  register int xi, yi;
  double mxv, myv;
  int loop_change;
  int mesh_change=0;

  /* Repeat the mesh changes until the mesh stops changing */
  /* (but stop trying after a while to avoid long or infinite loops) */
  do {
    loop_change = 0;

    /* Force top and bottom edges to be at borders */
    for(xi=0; xi < nx; xi++) {
      if(myP[xi]!=0) {
        myP[xi] = 0;
        loop_change++;
      }
      if(myP[(ny-1)*nx + xi] != (mh-1)) {
        myP[(ny-1)*nx + xi] = mh-1;
        loop_change++;
      }
    }

    myP[0] = 0;
    for(yi=1; yi < ny; yi++) {
      /* Force left and right edges to be at borders */
      if(mxP[yi*nx + 0] != 0) {
        mxP[yi*nx + 0] = 0;
        loop_change++;
      }
      if(mxP[yi*nx + (nx-1)] != (mw-1)) {
        mxP[yi*nx + (nx-1)] = mw-1;
        loop_change++;
      }

      /* Enforce functionality */
      for(xi=1; xi < nx; xi++) {
        /* make current point right of previous point */
        if(mxP[yi*nx + xi] <= mxP[yi*nx + (xi-1)]) {
          mxv = (mxP[yi*nx + xi] + mxP[yi*nx + (xi-1)]) / 2;
          mxP[yi*nx + xi]     = mxv + 1;
          mxP[yi*nx + (xi-1)] = mxv - 1;
          loop_change++;
        }
        /* make current point below point in previous row */
        if(myP[yi*nx + xi] <= myP[(yi-1)*nx + xi]) {
          myv = (myP[yi*nx + xi] + myP[(yi-1)*nx + xi]) / 2;
          myP[yi*nx + xi]     = myv + 1;
          myP[(yi-1)*nx + xi] = myv - 1 ;
          loop_change++;
        }

        /* make current point inside image boundary */
        if(mxP[yi*nx + xi] > (mw-nx+xi)) {
          mxP[yi*nx + xi]  =  mw-nx+xi;
          loop_change++;
        }
        /* make current point inside image boundary */
        if(myP[yi*nx + xi] > (mh-ny+yi)) {
          myP[yi*nx + xi]  =  mh-ny+yi;
          loop_change++;
        }
      }
    }
    if(loop_change) mesh_change++;
  } while ((mesh_change < (nx+ny)) && loop_change);

  return(mesh_change);
}

/* ---------------------------------------------------------------------- */

/* nearest_meshpoint: find the nearest meshpoint and return square of distance
** fill in the indices of the meshpoint and the x,y distances.
** distances are dx=(px-mxP[]) , dy=(py-myP[])
** px, py: (in) the pointer coordinates (and can be out of range)
** mxP, myP: (in) the mesh x and y arrays
** nx, ny: (in) the mesh dimensions
** mi, mj: (out) indices of closest meshpoint
** dx, dy: (out) x, y distance of pointer from nearest meshpoint
** Returns:  square distance between pointer and meshpoint
*/
int
nearest_meshpoint(int px, int py, double *mxP, double *myP, int nx, int ny, int *mi, int *mj, int *dx, int *dy)
{
  register int      xi, yi;
  register int      m_dx, m_dy;
  register long int m_d;
  long int          m_d_min;

  m_d_min = 15000;

  /* Guarentee p[xy] is in range */
  if(px<mxP[0]) px = mxP[0];
  if(py<myP[0]) py = myP[0];
  if(px>mxP[ny*nx-1]) px = mxP[nx*ny-1];
  if(py>myP[ny*nx-1]) py = myP[nx*ny-1];

  for(yi=0; yi<ny; yi++) {
    for(xi=0; xi<nx; xi++) {
      m_dx = px - mxP[yi*nx+xi];
      m_dy = py - myP[yi*nx+xi];
      m_d = m_dx*m_dx + m_dy*m_dy;

      if(m_d < m_d_min) {
        m_d_min = m_d;
        *mi = xi;
        *mj = yi;
        if(dx!=NULL) *dx = m_dx;
        if(dy!=NULL) *dy = m_dy;
      }
    }
  }
  return(m_d_min);
}

/* ---------------------------------------------------------------------- */

void
free_mesh(double *mxP, double *myP, char *proc)
{
#ifdef DEBUG_ALLOC
  printf("freeing mesh %p %p from %s\n", mxP, myP, proc);
#endif
  my_free(mxP, "free_mesh");
  my_free(myP, "free_mesh");
}

/* ---------------------------------------------------------------------- */

int
alloc_mesh(double **mxPP, double **myPP, int nx, int ny, char *proc)
{
  if((*mxPP=MY_CALLOC(nx*ny, double, "alloc_mesh"))==NULL) {
    fprintf(stderr, "%s: Bad Alloc\n", proc);
    return(1);
  }
  if((*myPP=MY_CALLOC(nx*ny, double, "alloc_mesh"))==NULL) {
    my_free(*mxPP, "alloc_mesh");
    fprintf(stderr, "%s: Bad Alloc\n", proc);
    return(1);
  }
#ifdef DEBUG_ALLOC
  printf("allocated mesh %p %p from %s\n", *mxPP, *myPP, proc);
#endif
  return(0);
}

/* ---------------------------------------------------------------------- */

/* add_mesh_line: add a mesh line
**
** nx, ny are the dimensions of the incoming mesh array
**
** mi is the upper left index of the quadrangle enclosing the new line
**  (i.e. mi is less than the index of the new line in the new mesh)
**  for adding vertical lines, mi is the index of the nearby left column
**  for adding horizontal lines, mi is the index of the nearby upper row
**
** mt is the relative distance between the surrounding mesh lines
**
** type is 1 for vertical lines or 2 for horizontal lines
**
** Allocate memory for the mesh and sets the incoming mesh pointers to
**  the newly allocated array
**
** Caller must increment the appropriate local analogy to nx or ny
**
** Return nonzero if add_mesh_line fails
*/
int
add_mesh_line(double **mxPP, double **myPP, int nx, int ny, int mi, double mt, int type)
{
  int xi, yi;
  int new_nx, new_ny;
  double *new_mxP, *new_myP;

  switch(type) {
    /* Add vertical */
    case 1:
      /* Add a column */
      new_nx = nx + 1;
      new_ny = ny;
      break;

    /* Add horizontal */
    case 2:
      /* Add a row */
      new_nx = nx;
      new_ny = ny + 1;
      break;

    /* Invalid type */
    default:
      fprintf(stderr, "add_mesh_line: Bad Value: type: %i\n", type);
      return(-1);
  }

  if(alloc_mesh(&new_mxP, &new_myP, new_nx, new_ny, "add_mesh_line")) return(1);

  switch(type) {
    /* --- --- --- Add vertical --- --- --- */
    case 1:
      /* Copy the left columns */
      for(yi=0; yi<ny; yi++) {
        for(xi=0; xi<=mi; xi++) {
          new_mxP[yi*new_nx + xi] = (*mxPP)[yi*nx + xi];
          new_myP[yi*new_nx + xi] = (*myPP)[yi*nx + xi];
        }
      }

      /* Copy the right columns */
      for(yi=0; yi<ny; yi++) {
        for(xi=mi+1; xi<nx; xi++) {
          new_mxP[yi*new_nx + (xi+1)] = (*mxPP)[yi*nx + xi];
          new_myP[yi*new_nx + (xi+1)] = (*myPP)[yi*nx + xi];
        }
      }

      /* Add the new column */
      {
        double mx1, mx2, mxv;
        double my1, my2, myv;
        for(yi=0; yi<ny; yi++) {
          mx1 = (*mxPP)[yi*nx + mi];
          mx2 = (*mxPP)[yi*nx + (mi+1)];
          mxv = (1.0-mt)*mx1 + mt*mx2;
          new_mxP[yi*new_nx + (mi+1)] = mxv;
          my1 = (*myPP)[yi*nx + mi];
          my2 = (*myPP)[yi*nx + (mi+1)];
          myv = (1.0-mt)*my1 + mt*my2;
          new_myP[yi*new_nx + (mi+1)] = myv;
        }
      }
      break;

    /* --- --- --- Add horizontal --- --- --- */
    case 2:
      /* Copy the top rows */
      for(yi=0; yi<=mi; yi++) {
        for(xi=0; xi<nx; xi++) {
          new_mxP[yi*new_nx + xi] = (*mxPP)[yi*nx + xi];
          new_myP[yi*new_nx + xi] = (*myPP)[yi*nx + xi];
        }
      }

      /* Copy the bottom rows */
      for(yi=mi+1; yi<ny; yi++) {
        for(xi=0; xi<nx; xi++) {
          new_mxP[(yi+1)*new_nx + xi] = (*mxPP)[yi*nx + xi];
          new_myP[(yi+1)*new_nx + xi] = (*myPP)[yi*nx + xi];
        }
      }

      /* Add the new row */
      {
        double mx1, mx2, mxv;
        double my1, my2, myv;
        for(xi=0; xi<nx; xi++) {
          mx1 = (*mxPP)[(mi)*nx + xi];
          mx2 = (*mxPP)[(mi+1)*nx + xi];
          mxv = (1.0-mt)*mx1 + mt*mx2;
          new_mxP[(mi+1)*new_nx + xi] = mxv;
          my1 = (*myPP)[(mi)*nx + xi];
          my2 = (*myPP)[(mi+1)*nx + xi];
          myv = (1.0-mt)*my1 + mt*my2;
          new_myP[(mi+1)*new_nx + xi] = myv;
        }
      }
      break;

    /* --- --- --- Invalid type --- --- --- */
    default:
      fprintf(stderr, "add_mesh_line: Bad Value: type: %i\n", type);
      return(-1);
  }

  free_mesh(*mxPP, *myPP, "add_mesh_line");
  *mxPP = new_mxP;
  *myPP = new_myP;

  return(0);
}

/* ---------------------------------------------------------------------- */

/* delete_mesh_line: delete a mesh line
**
** nx, ny are the dimensions of the incoming mesh array
**
** mi is index the of a mesh point on the to-be-deleted line
**  for deleting vertical lines, mi is the index of the column
**  for deleting horizontal lines, mi is the index of the row
**
** type is 1 for vertical lines or 2 for horizontal lines
**
** Allocate memory for the mesh and set the incoming mesh pointers to
**  the newly allocated array
**
** Caller must decrement the appropriate local analogy to nx or ny
**
** Return nonzero if delete_mesh_line fails
*/
int
delete_mesh_line(double **mxPP, double **myPP, int nx, int ny, int mi, int type)
{
  int xi, yi;
  int new_nx, new_ny;
  double *new_mxP, *new_myP;

  switch(type) {
    /* Delete vertical */
    case 1:
      /* Delete a column */
      new_nx = nx - 1;
      new_ny = ny;
      break;

    /* Delete horizontal */
    case 2:
      /* Delete a row */
      new_nx = nx;
      new_ny = ny - 1;
      break;

    /* Invalid type */
    default:
      fprintf(stderr, "delete_mesh_line: Bad Value: type: %i\n", type);
      return(-1);
  }

  if(alloc_mesh(&new_mxP,&new_myP,new_nx,new_ny, "delete_mesh_line")) return(1);

  switch(type) {
    /* --- --- --- Delete vertical --- --- --- */
    case 1:
      /* Copy the left columns */
      for(yi=0; yi<ny; yi++) {
        for(xi=0; xi<mi; xi++) {
          new_mxP[yi*new_nx + xi] = (*mxPP)[yi*nx + xi];
          new_myP[yi*new_nx + xi] = (*myPP)[yi*nx + xi];
        }
      }

      /* Copy the right columns */
      for(yi=0; yi<ny; yi++) {
        for(xi=mi+1; xi<nx; xi++) {
          new_mxP[yi*new_nx + (xi-1)] = (*mxPP)[yi*nx + xi];
          new_myP[yi*new_nx + (xi-1)] = (*myPP)[yi*nx + xi];
        }
      }

      break;

    /* --- --- --- Delete horizontal --- --- --- */
    case 2:
      /* Copy the top rows */
      for(yi=0; yi<mi; yi++) {
        for(xi=0; xi<nx; xi++) {
          new_mxP[yi*new_nx + xi] = (*mxPP)[yi*nx + xi];
          new_myP[yi*new_nx + xi] = (*myPP)[yi*nx + xi];
        }
      }

      /* Copy the bottom rows */
      for(yi=mi+1; yi<ny; yi++) {
        for(xi=0; xi<nx; xi++) {
          new_mxP[(yi-1)*new_nx + xi] = (*mxPP)[yi*nx + xi];
          new_myP[(yi-1)*new_nx + xi] = (*myPP)[yi*nx + xi];
        }
      }

      break;

    /* --- --- --- Invalid type --- --- --- */
    default:
      fprintf(stderr, "delete_mesh_line: Bad Value: type: %i\n", type);
      return(-1);
  }

  free_mesh(*mxPP, *myPP, "delete_mesh_line");
  *mxPP = new_mxP;
  *myPP = new_myP;

  return(0);
}

/* ---------------------------------------------------------------------- */

/* load_mesh: load a mesh from file named fn
** if successful, allocates memory for the meshes and sets nxP and nyP
** returns nonzero if load_mesh fails
*/
int
load_mesh(char *fn, double **mxPP, double **myPP, int *nxP, int *nyP)
{
  int xi, yi;
  double mesh_point;
  char magic[2];
  FILE *fP;

  if((fP=fopen(fn, "r"))==NULL) {
    fprintf(stderr, "load_mesh: could not read file '%s'\n", fn);
    return(1);
  }

  if(get_block(fP, magic, 2)) {
    fprintf(stderr, "load_mesh: premature EOF in file '%s'\n", fn);
    fclose(fP);
    return(EOF);
  }

  /* M2 indicates an ASCII mesh file */
  if(magic[0]=='M' && magic[1]=='2') {
    /* Read the mesh geometry */
    if(fscanf(fP, "%i", nxP)!=1 || *nxP<0) {
      fprintf(stderr, "load_mesh: missing uint\n");
      fclose(fP);
      return(2);
    }
    if(fscanf(fP, "%i", nyP)!=1 || *nyP<0) {
      fprintf(stderr, "load_mesh: missing uint\n");
      fclose(fP);
      return(3);
    }

    if(alloc_mesh(mxPP, myPP, *nxP, *nyP, "load_mesh")) {
      fclose(fP);
      return(6);
    }

    /* Read the mesh point values */
#ifdef TRANSPOSE_MESH
    for(xi=0; xi < *nxP; xi++) {
      for(yi=0; yi < *nyP; yi++) {
#else
    for(yi=0; yi < *nyP; yi++) {
      for(xi=0; xi < *nxP; xi++) {
#endif
        if(fscanf(fP, "%lf", &mesh_point)!=1) {
          fprintf(stderr, "load_mesh: missing float at %i %i\n", xi, yi);
          fclose(fP);
          free_mesh(*mxPP, *myPP, "load_mesh");
          return(4);
        }
        (*mxPP)[yi* *nxP + xi] = mesh_point;

        if(fscanf(fP, "%lf", &mesh_point)!=1) {
          fprintf(stderr, "load_mesh: missing float at %i %i\n", xi, yi);
          fclose(fP);
          free_mesh(*mxPP, *myPP, "load_mesh");
          return(4);
        }
        (*myPP)[yi* *nxP + xi] = mesh_point;
      }
    }
  } else {
    fprintf(stderr, "load_mesh: file was not a valid mesh file\n");
    fclose(fP);
    return(5);
  }
  fclose(fP);
  return(0);
}

/* ---------------------------------------------------------------------- */

/* save_mesh: save a mesh to file named fn
*/
int
save_mesh(char *fn, double *mxP, double *myP, int nx, int ny)
{
  int xi, yi;
  FILE *fP;

  if((fP=fopen(fn, "w"))==NULL) {
    fprintf(stderr, "save_mesh: could not write file '%s \n", fn);
    return(1);
  }

  /* M2 indicates an ASCII mesh file */
  fprintf(fP, "M2\n");

  /* Write the mesh geometry */
  fprintf(fP, "%i %i\n", nx, ny);

  /* Write the mesh point values */
  for(yi=0; yi < ny; yi++) {
    for(xi=0; xi < nx; xi++) {
      fprintf(fP, "%lg ", mxP[yi*nx + xi]);
      fprintf(fP, "%lg\n", myP[yi*nx + xi]);
    }
  }

  fclose(fP);
  return(0);
}
