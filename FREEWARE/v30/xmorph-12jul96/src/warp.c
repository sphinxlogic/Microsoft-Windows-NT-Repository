/* warp.c : Digital Image Warping algorithms
**
** See George Wolberg's "Digital Image Warping"
** IEEE Computer Society Press order number 1944
** ISBN 0-8186-8944-7
**
** Copyrights held by various authors.  See individual routines.
**
** Do not redistribute this code.
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>

#include "my_malloc.h"
#include "warp.h"

#ifndef UCHAR
#define UCHAR unsigned char
#endif

#define MAX(x,y) ((x)>(y) ? (x) : (y))

/* --------------------------------------------------------------- */

/* resample_array: Fant's resampling algorithm, specialized to
** images with the same dimensions
**
** Based on "Nonaliasing Real-Time Spatial Transform" by Karl M. Fant.
** See IEEE Computer Graphics and Applications v6n1, January 1986.
**
** NOTE: The original code contained array bounds overruns.  I fixed
** the code up without taking resampling theory into consideration.  I
** figured the boundary pixels would be the only things hosed anyway, and
** the original code was dumping core. -- MJG 08jun94
**
** UPDATE: 20jul94: George Wolberg (himself) gives these array bounds
** modifications a thumbs up.
*/

static void
resample_array(double *F, UCHAR *src, UCHAR *dst, int len, int ipo)
{
  int ui, xi;
  double acc, intensity;
  double INSFAC, INSEG, OUTSEG;
  double *inpos; /* forward mapping function */

  /* MJG 08jun94 */
  inpos = MY_CALLOC(len+2, double, "resample_array");

  /* precompute input index for each output pixel */
  for(ui=xi=0; xi<len; xi++) {
    while( ui<(len-1) &&  F[ui+1]<xi ) ui++; /* MJG 08jun94 */

    if(ui<(len-1)) { /* MJG 08jun94 */
      inpos[xi] = ui + (double) (xi - F[ui]) / (F[ui+1] - F[ui]);
    } else {
      inpos[xi] = ui + 1.0 ;
    }
  }
  inpos[len] = len; /* MJG 08jun94 */

  INSEG = 1.0;       /* entire input pixel is available */
  OUTSEG = inpos[1]; /* # input pixels that map onto 1 output pixel */
  INSFAC = OUTSEG;   /* inverse scale factor */
  acc = 0.0;         /* accumulator */

  /* compute all output pixels */
  /* ellenberger@tle.enet.dec.com reports:
  ** the loop upper bound should be (len - 1) instead of len
  ** Just in case, I'm changing it to (len - 1)
  ** Consider this a hack until I figure out what's going on. -- 13sep95 MJG
  */
  /* for(xi = ui = 0; xi < len; ) { */
  for(xi = ui = 0; xi < (len - 1); ) {

#ifdef ARRAY_CHECK
    if(ui > ((len-1)*ipo)) {
      fprintf(stderr, "resample_array: index out of range: %i\n", ui);
    }
#endif

    /* use linear interpolation for reconstruction */
    intensity = (INSEG * src[ui]) + ((1-INSEG) * src[ui+1]);

    /* INSEG < OUTSEG: input pixel is entirely consumed before output pixel */
    if(INSEG < OUTSEG) {
      acc += (intensity * INSEG);  /* accumulation of weighted contrib */
      OUTSEG -= INSEG;             /* INSEG portion has been filled */
      INSEG = 1.0;                 /* new input pixel will be available */
      ui += ipo;                   /* index to next input pixel */
    }

    /* INSEG >= OUTSEG: input pixel is not consumed before output pixel */
    else {
      acc += (intensity * OUTSEG); /* accumulate weighted contrib */

      /* A short hack to prevent an arithmetic exception on division by zero
      ** in case INSFAC becomes zero (AT 04feb95) -- Added 13sep95
      ** [It seems that when INSFAC is zero, so is acc -- MJG 13sep95]
      */
      if (INSFAC==0) {
        INSFAC = 1;
      }
      /* End of hack (AT 04feb95) */

      dst[xi*ipo] = acc/INSFAC;    /* init output w/ normalized accumulator */
      acc = 0.0;                   /* reset accumulator for next output pixel */
      INSEG -= OUTSEG;             /* OUTSEG portion of input has been used */
      xi++;                        /* index to next output pixel */
      INSFAC = inpos[xi+1] - inpos[xi]; /* init spatially varying INSFAC */
      OUTSEG = INSFAC;             /* init spatially varying SIZFAC */
    }
  }
  my_free(inpos, "resample_array");
}

/* --------------------------------------------------------------- */

/* warp_image: 2-pass mesh-based single-channel image warping
**
** See Douglas B. Smythe "A Two-Pass Mesh Warping Algorithm for
** Object Transformation and Image Interpolation", ILM Technical Memo
** #1030, Computer Graphics Department, Lucasfilm Ltd., 1990
**
** This code was mainly written by George Wolberg, based on Smythe90.
**
** input image "in" has dimensions img_width and img_height
** xs, ys are the source mesh coordinates
** xd, yd are the destination mesh coordinates
**   both meshes have dimensions mesh_width, mesh_height
** output image "out" has same dimensions as "in"
** Mesh points must not fold over,
** and border mesh points must stay on border
*/

void
warp_image(UCHAR *in, UCHAR *out, int img_width, int img_height,
  double *xs, double *ys, double *xd, double *yd,
  int mesh_width, int mesh_height)
{
  int ai;
  int xi, yi;
  UCHAR *src, *dst, *tmp;
  double *x1, *y1, *x2, *y2;
  double *xrow1, *yrow1, *xrow2, *yrow2;
  double *map1, *map2;
  double *indx;
  double *ts, *ti, *td;


  /* allocate memory for buffers:
  ** indx stores indices used to sample splines
  ** xrow1, xrow2, yrow2, yrow2 store column data in row order for spline
  ** map1, map2 store mapping functions computed in row order in spline
  */

  ai = MAX(img_height, img_width) + 1;
  indx =  MY_CALLOC(ai, double, "warp_image");
  xrow1 =  MY_CALLOC(ai, double, "warp_image");
  yrow1 =  MY_CALLOC(ai, double, "warp_image");
  xrow2 =  MY_CALLOC(ai, double, "warp_image");
  yrow2 =  MY_CALLOC(ai, double, "warp_image");
  map1 =  MY_CALLOC(ai, double, "warp_image");
  map2 =  MY_CALLOC(ai, double, "warp_image");

  /* First pass (phase one): create tables ts and ti for x-intercepts
  ** of vertical splines in S and I.
  ** tables have mesh_width columns of height img_height
  */
  ts = MY_CALLOC(mesh_width*img_height, double, "warp_image");
  ti = MY_CALLOC(mesh_width*img_height, double, "warp_image");
  for(yi=0; yi<img_height; yi++) /* indices used to sample vert spline */
    indx[yi] = yi;

  for(xi=0; xi<mesh_width; xi++) { /* visit each vertical spline */
    /* store columns as rows for spline */
    for(yi=0; yi<mesh_height; yi++) {
      xrow1[yi] = xs[yi*mesh_width+xi];
      xrow2[yi] = xd[yi*mesh_width+xi];
      yrow1[yi] = ys[yi*mesh_width+xi];
      yrow2[yi] = yd[yi*mesh_width+xi];
    }

    /* scan convert vertical splines of S and I */
    hermite3_array(yrow1, xrow1, mesh_height, indx, map1, img_height);
    /* bug reported 30may96 by Mike Hoefelein: */
    /* hermite3_array(yrow2, xrow2, mesh_height, indx, map2, img_height); */
    hermite3_array(yrow1, xrow2, mesh_height, indx, map2, img_height);

    /* store resampled rows back into columns */
    for(yi=0; yi<img_height; yi++) {
      ts[yi*mesh_width+xi] = map1[yi];
      ti[yi*mesh_width+xi] = map2[yi];
    }
  }

  /* First pass (phase two): warp x using ts and ti.
  ** tmp holds intermediate image.
  */
  if((tmp = MY_CALLOC(img_height * img_width, UCHAR, "warp_image"))==NULL) {
    fprintf(stderr, "warp_image: Bad Alloc: tmp\n"); return;
  }

  for(xi=0; xi<img_width; xi++) /* indices used to sample horiz spline */
    indx[xi]=xi;

  for(yi=0; yi<img_height; yi++) { /* visit each row */
    /* fit spline to x-intercepts; resample over all columns */
    x1 = &ts[yi*mesh_width];
    x2 = &ti[yi*mesh_width];
    hermite3_array(x1, x2, mesh_width, indx, map1, img_width);

    /* resample source row based on map1 */
    src = &in[yi*img_width];
    dst = &tmp[yi*img_width];
    resample_array(map1, src, dst, img_width, 1);
  }

  /* free buffers */
  my_free(ts, "warp_image");
  my_free(ti, "warp_image");

  /* Second pass (phase one): create tables ti and td for y-intercepts
  ** of horiz splines in I and D.
  ** Tables have mesh_height rows of width img_width
  */
  ti = MY_CALLOC(mesh_height*img_width, double, "warp_image");
  td = MY_CALLOC(mesh_height*img_width, double, "warp_image");
  for(xi=0; xi<img_width; xi++)
    indx[xi] = xi; /* indices used to sample horiz splines */
  for(yi=0; yi<mesh_height; yi++) {
    /* scan convert horizontal splines of I and D */
    /* x1 = &xs[yi*mesh_width]; /**/
    y1 = &ys[yi*mesh_width];
    x2 = &xd[yi*mesh_width];
    y2 = &yd[yi*mesh_width];

    /* The following line is correct: (x2, y1, ...) */
    hermite3_array(x2, y1, mesh_width, indx, &ti[yi*img_width], img_width);
    hermite3_array(x2, y2, mesh_width, indx, &td[yi*img_width], img_width);
  }

  /* Second pass (phase two): warp y using ti and td */
  for(yi=0; yi<img_height; yi++)
    indx[yi] = yi;

  for(xi=0; xi<img_width; xi++) {
    /* store columns as row for hermite3_array */
    for(yi=0; yi<mesh_height; yi++) {
      xrow1[yi] = ti[yi*img_width+xi];
      yrow1[yi] = td[yi*img_width+xi];
    }

    /* fit spline to y-intercepts:  resample over all rows */
    hermite3_array(xrow1, yrow1, mesh_height, indx, map1, img_height);

    /* resample intermediate image column based on map */
    src = &tmp[xi];
    dst = &out[xi];
    resample_array(map1, src, dst, img_height, img_width);
  }

  my_free(tmp, "warp_image");
  my_free(ti, "warp_image");
  my_free(td, "warp_image");
  my_free(indx, "warp_image");
  my_free(xrow1, "warp_image");
  my_free(yrow1, "warp_image");
  my_free(xrow2, "warp_image");
  my_free(yrow2, "warp_image");
  my_free(map1, "warp_image");
  my_free(map2, "warp_image");
}
