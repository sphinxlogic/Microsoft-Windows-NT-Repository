/* warp.h : Digital Image Warping algorithms header
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#ifndef _WARP__INCLUDED_
#define _WARP__INCLUDED_

#ifndef UCHAR
#define UCHAR unsigned char
#endif

/* warp_image: 2-pass mesh-based single-channel image warping */
extern void
warp_image(UCHAR *in, UCHAR *out, int img_width, int img_height,
  double *xs, double *ys, double *xd, double *yd,
  int mesh_width, int mesh_height);

#endif
