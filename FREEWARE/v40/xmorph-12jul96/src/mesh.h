/* mesh.h -- mesh handling routines header
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#ifndef _MESH_H__INCLUDED_
#define _MESH_H__INCLUDED_

#include "diw_map.h"

extern void lin_interp_mesh(double *mi1, double *mi2, int nx, int ny, double t, double *mo);

extern void reset_mesh(double *mx, double *my, int nx, int ny, int mw, int mh);

extern void reset_meshes(diw_map_t *diw_mapP);

extern int functionalize_mesh(double *mxP, double *myP, int nx, int ny, int mw, int mh);

extern int nearest_meshpoint(int px, int py, double *mxP, double *myP, int nx, int ny, int *mi, int *mj, int *dx, int *dy);

extern void free_mesh(double *mxP, double *myP, char *proc);

extern int alloc_mesh(double **mxPP, double **myPP, int nx, int ny,
char *proc);

extern int add_mesh_line(double **mxPP, double **myPP, int nx, int ny, int mi, double mt, int type);

extern int delete_mesh_line(double **mxPP, double **myPP, int nx, int ny, int mi, int type);

extern int load_mesh(char *fn, double **mxPP, double **myPP, int *nxP, int *nyP);

/* save_mesh: save a mesh to file named fn */
extern int save_mesh(char *fn, double *mxP, double *myP, int nx, int ny);

#endif /* _MESH_H__INCLUDED_ */
